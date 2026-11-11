import csv
import math

# Run: python MuonID_eff_corr.py
# Read and write CSV files in the current directory. Existing outputs are overwritten.

# MC16rd: MySampleType = 4
sample_bounds = (3.5, 4.5)
# PDG code: mu- = +13, mu+ = -13
pid_bounds = {"minus": (12.5, 13.5), "plus": (-13.5, -12.5)}
# ALP table: offIP for index 0/1, onIP for index 2
index_bounds = {"off": (-0.5, 1.5), "on": (1.5, 2.5)}

def ReadCSV(filename_):
    with open(filename_, newline="", encoding="utf-8-sig") as file:
        rows = list(csv.DictReader(file))
    if not rows:
        raise ValueError(f"Empty CSV file: {filename_}")
    return rows

def CombineTables(tables_, threshold_, ip_modes_, isItALP = False):
    combined = []
    for ip_mode in ip_modes_:
        for charge in ("minus", "plus"):
            pid_min, pid_max = pid_bounds[charge]
            for original in tables_[threshold_, ip_mode, charge]:
                # Keep the original correction factors and uncertainties.
                row = {
                    "sample_min": sample_bounds[0],
                    "sample_max": sample_bounds[1],
                    "PID_min": pid_min,
                    "PID_max": pid_max,
                    **original,
                    # Reverse the bounds when converting cosTheta to Theta.
                    "Theta_max": math.acos(float(original["cosTheta_min"])),
                    "Theta_min": math.acos(float(original["cosTheta_max"])),
                }
                if isItALP:
                    index_min, index_max = index_bounds[ip_mode]
                    row = {"index_min": index_min, "index_max": index_max, **row}
                combined.append(row)
    return combined

def WriteCSV(filename_, rows_):
    with open(filename_, "w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=list(rows_[0]), lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows_)
    print(f"Saved {filename_} ({len(rows_)} rows)")

# Read all eight input tables
tables = {}
for threshold in ("01", "05"):
    for ip_mode in ("off", "on"):
        for charge in ("minus", "plus"):
            filename = f"MuonEff{threshold}_{ip_mode}_{charge}.csv"
            tables[threshold, ip_mode, charge] = ReadCSV(filename)

# Combine prompt (onIP) and ALP (offIP + onIP) tables
outputs = {}
for threshold in ("01", "05"):
    outputs[f"MuonEff{threshold}_prompt.csv"] = CombineTables(tables, threshold, ("on",))
    outputs[f"MuonEff{threshold}_ALP.csv"] = CombineTables(tables, threshold, ("off", "on"), isItALP = True)

# Save all four combined tables
for filename, rows in outputs.items():
    WriteCSV(filename, rows)
