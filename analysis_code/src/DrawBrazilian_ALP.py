#!/usr/bin/env python3
import argparse
import os
import re
from glob import glob
import matplotlib.pyplot as plt

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description = "argument to draw brazilian flag plot")

    parser.add_argument("--input", required=True, type=str, help="input path")
    parser.add_argument('--lifetime', required=True, type=float, help="lifetime of ALP particle")
    parser.add_argument('--A', required=True, type=int, help="A parameter of ALP particle")
    parser.add_argument('--B', required=True, type=int, help="B parameter of ALP particle")
    parser.add_argument("--output", required= True, type=str, help = "output path")

    args = parser.parse_args()

    input_path = args.input
    lifetime = args.lifetime
    A = args.A
    B = args.B
    output_path = args.output

    # get CLs directories
    lifetime_str = f"{lifetime}".rstrip("0").rstrip(".")
    CLsDirs = glob(input_path + "/CLs_*_%s_%d_%d" % (lifetime_str, A, B))
    CLsDirs.sort()

    # read files
    results = []

    for CLsDir in CLsDirs:
        f = open(CLsDir + "/CLs.log", "r")

        dir_name = os.path.basename(CLsDir)
        match = re.match(rf"CLs_(.+)_{re.escape(lifetime_str)}_{A}_{B}$", dir_name)
        ALPMass = float(match.group(1))
        
        text = f.read()
        f.close()
        
        match = re.search(r"Expected mu:\s*([+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)", text)
        if match:
            median = float(match.group(1))
        else:
            continue
            
        match = re.search(r"Expected mu \+1sigma:\s*([+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)", text)
        if match:
            plus1 = float(match.group(1))
        else:
            continue
            
        match = re.search(r"Expected mu -1sigma:\s*([+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)", text)
        if match:
            minus1 = float(match.group(1))
        else:
            continue
        
        match = re.search(r"Expected mu \+2sigma:\s*([+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)", text)
        if match:
            plus2 = float(match.group(1))
        else:
            continue
        
        match = re.search(r"Expected mu -2sigma:\s*([+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)", text)
        if match:
            minus2 = float(match.group(1))
        else:
            continue

        results.append((ALPMass, median, plus1, minus1, plus2, minus2))

    results.sort(key=lambda x: x[0])

    ALPMassList = [x[0] for x in results]
    expectedULMedianList = [x[1] for x in results]
    expectedULPlusOneList = [x[2] for x in results]
    expectedULMinusOneList = [x[3] for x in results]
    expectedULPlusTwoList = [x[4] for x in results]
    expectedULMinusTwoList = [x[5] for x in results]

    # draw
    plt.figure(figsize=(8,6))

    # +-2sigma band (yellow)
    plt.fill_between(ALPMassList, expectedULMinusTwoList, expectedULPlusTwoList, label = r"Expected $\pm$ 2$\sigma$", alpha = 0.8)

    # +-1sigma band (green)
    plt.fill_between(ALPMassList, expectedULMinusOneList, expectedULPlusOneList, label = r"Expected $\pm$ 1$\sigma$", alpha = 0.8)

    # median
    plt.plot(ALPMassList, expectedULMedianList, "--", linewidth = 2, label = "Expected median")

    # set ui
    plt.xlabel("Axion-like particle mass [GeV]")
    plt.ylabel("Upper limit of branching fraction")
    plt.legend()
    plt.tight_layout()

    plt.savefig(output_path + "/Brazilian_" + str(lifetime) + "_" + str(A) + "_" + str(B) + ".png", dpi = 300)
    plt.close()
    
