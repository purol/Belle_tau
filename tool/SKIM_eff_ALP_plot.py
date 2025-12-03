import numpy as np
import matplotlib.pyplot as plt

def GetUncerEff(eff_array_, trial = 10000):
    eff_uncer_array = [(e*(1-e)/trial)**(0.5) for e in eff_array_]
    return eff_uncer_array

def AddDataPoint(ax_, x_data_, y_data_, y_data_err_, legend_):
    ax_.errorbar(x_data_, y_data_, yerr = y_data_err_, marker="o", label=legend_)

signal_eff = 0.6976

eff_m02_VmA = []
eff_uncer_m02_VmA = GetUncerEff(eff_m02_VmA)

eff_m02_VpA = [0.6616, 0.6683, 0.6496, 0.6417, 0.6094, 0.3872, 0.2629, 0.0932, 0.0634]
eff_uncer_m02_VpA = GetUncerEff(eff_m02_VpA)

eff_m04_VmA = []
eff_uncer_m04_VmA = GetUncerEff(eff_m04_VmA)

eff_m04_VpA = [0.7008, 0.7079, 0.6971, 0.6932, 0.6798, 0.5085, 0.3638, 0.1339, 0.0909]
eff_uncer_m04_VpA = GetUncerEff(eff_m04_VpA)

eff_m06_VmA = []
eff_uncer_m06_VmA = GetUncerEff(eff_m06_VmA)

eff_m06_VpA = [0.6910, 0.6970, 0.6816, 0.6971, 0.6808, 0.5400, 0.4128, 0.1619, 0.1002]
eff_uncer_m06_VpA = GetUncerEff(eff_m06_VpA)

eff_m08_VmA = []
eff_uncer_m08_VmA = GetUncerEff(eff_m08_VmA)

eff_m08_VpA = [0.6894, 0.6802, 0.6807, 0.6848, 0.6763, 0.5621, 0.4487, 0.1731, 0.1092]
eff_uncer_m08_VpA = GetUncerEff(eff_m08_VpA)

eff_m10_VmA = []
eff_uncer_m10_VmA = GetUncerEff(eff_m10_VmA)

eff_m10_VpA = [0.6857, 0.6886, 0.6883, 0.6793, 0.6770, 0.5721, 0.4742, 0.1850, 0.1210]
eff_uncer_m10_VpA = GetUncerEff(eff_m10_VpA)

eff_m12_VmA = []
eff_uncer_m12_VmA = GetUncerEff(eff_m12_VmA)

eff_m12_VpA = [0.6876, 0.6863, 0.6808, 0.6891, 0.6731, 0.5780, 0.4766, 0.1896, 0.1136]
eff_uncer_m12_VpA = GetUncerEff(eff_m12_VpA)

eff_m14_VmA = []
eff_uncer_m14_VmA = GetUncerEff(eff_m14_VmA)

eff_m14_VpA = [0.6853, 0.6906, 0.6958, 0.6832, 0.6725, 0.5803, 0.4827, 0.1932, 0.1205]
eff_uncer_m14_VpA = GetUncerEff(eff_m14_VpA)

eff_m16_VmA = []
eff_uncer_m16_VmA = GetUncerEff(eff_m16_VmA)

eff_m16_VpA = [0.6988, 0.6932, 0.6901, 0.6921, 0.6748, 0.5909, 0.5005, 0.2001, 0.1225]
eff_uncer_m16_VpA = GetUncerEff(eff_m16_VpA)

lifetime_axis = [0.1, 0.5, 1, 5, 10, 50, 100, 500, 1000] # mm

fig, ax = plt.subplots()
ax.set_ylabel("efficiency")
ax.set_xlabel(r"$c\cdot\tau$ [mm]")
"""
AddDataPoint(ax, lifetime_axis, eff_m02_VmA, eff_uncer_m02_VmA, r"m=0.2 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m04_VmA, eff_uncer_m04_VmA, r"m=0.4 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m06_VmA, eff_uncer_m06_VmA, r"m=0.6 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m08_VmA, eff_uncer_m08_VmA, r"m=0.8 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m10_VmA, eff_uncer_m10_VmA, r"m=1.0 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m12_VmA, eff_uncer_m12_VmA, r"m=1.2 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m14_VmA, eff_uncer_m14_VmA, r"m=1.4 GeV, V-A")
AddDataPoint(ax, lifetime_axis, eff_m16_VmA, eff_uncer_m16_VmA, r"m=1.6 GeV, V-A")

plt.axhline(y=signal_eff, color="r", linestyle="--")
ax.text(200, signal_eff + 0.005, "efficiency of prompt decay = 0.67", color="r", ha="left", va="bottom")

ax.legend()
plt.grid(True, linestyle="--")
plt.show()
plt.close()
"""
# V+A #
fig, ax = plt.subplots()
ax.set_ylabel("efficiency")
ax.set_xlabel(r"$c\cdot\tau$ [mm]")

AddDataPoint(ax, lifetime_axis, eff_m02_VpA, eff_uncer_m02_VpA, r"m=0.2 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m04_VpA, eff_uncer_m04_VpA, r"m=0.4 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m06_VpA, eff_uncer_m06_VpA, r"m=0.6 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m08_VpA, eff_uncer_m08_VpA, r"m=0.8 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m10_VpA, eff_uncer_m10_VpA, r"m=1.0 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m12_VpA, eff_uncer_m12_VpA, r"m=1.2 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m14_VpA, eff_uncer_m14_VpA, r"m=1.4 GeV, V+A")
AddDataPoint(ax, lifetime_axis, eff_m16_VpA, eff_uncer_m16_VpA, r"m=1.6 GeV, V+A")

plt.axhline(y=signal_eff, color="r", linestyle="--")
ax.text(200, signal_eff + 0.005, "efficiency of prompt decay = 0.67", color="r", ha="left", va="bottom")

ax.legend()
plt.grid(True, linestyle="--")
plt.show()
