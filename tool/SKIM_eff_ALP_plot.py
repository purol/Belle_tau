import numpy as np
import matplotlib.pyplot as plt

def GetUncerEff(eff_array_, trial = 10000):
    eff_uncer_array = [(e*(1-e)/trial)**(0.5) for e in eff_array_]
    return eff_uncer_array

def AddDataPoint(ax_, x_data_, y_data_, y_data_err_, legend_):
    ax_.errorbar(x_data_, y_data_, yerr = y_data_err_, marker="o", label=legend_)

signal_eff = 0.6737

eff_m02_VmA = [0.6278, 0.6304, 0.6259, 0.6162, 0.5770, 0.3585, 0.2372, 0.0688, 0.0366]
eff_uncer_m02_VmA = GetUncerEff(eff_m02_VmA)

eff_m02_VpA = [0.6280, 0.6313, 0.6179, 0.6093, 0.5852, 0.3598, 0.2334, 0.0660, 0.0328]
eff_uncer_m02_VpA = GetUncerEff(eff_m02_VpA)

eff_m04_VmA = [0.6730, 0.6718, 0.6819, 0.6713, 0.6483, 0.4793, 0.3276, 0.1099, 0.0637]
eff_uncer_m04_VmA = GetUncerEff(eff_m04_VmA)

eff_m04_VpA = [0.6697, 0.6777, 0.6669, 0.6642, 0.6532, 0.4830, 0.3354, 0.1080, 0.0585]
eff_uncer_m04_VpA = GetUncerEff(eff_m04_VpA)

eff_m06_VmA = [0.6655, 0.6517, 0.6526, 0.6664, 0.6519, 0.5108, 0.3821, 0.1344, 0.0801]
eff_uncer_m06_VmA = GetUncerEff(eff_m06_VmA)

eff_m06_VpA = [0.6611, 0.6651, 0.6485, 0.6652, 0.6506, 0.5107, 0.3847, 0.1348, 0.0712]
eff_uncer_m06_VpA = GetUncerEff(eff_m06_VpA)

eff_m08_VmA = [0.6561, 0.6565, 0.6575, 0.6523, 0.6423, 0.5287, 0.4194, 0.1526, 0.0835]
eff_uncer_m08_VmA = GetUncerEff(eff_m08_VmA)

eff_m08_VpA = [0.6496, 0.6505, 0.6498, 0.6550, 0.6458, 0.5333, 0.4100, 0.1457, 0.0833]
eff_uncer_m08_VpA = GetUncerEff(eff_m08_VpA)

eff_m10_VmA = [0.6527, 0.6586, 0.6506, 0.6455, 0.6436, 0.5401, 0.4302, 0.1552, 0.0893]
eff_uncer_m10_VmA = GetUncerEff(eff_m10_VmA)

eff_m10_VpA = [0.6539, 0.6559, 0.6536, 0.6471, 0.6454, 0.5433, 0.4400, 0.1563, 0.0885]
eff_uncer_m10_VpA = GetUncerEff(eff_m10_VpA)

eff_m12_VmA = [0.6587, 0.6558, 0.6552, 0.6499, 0.6524, 0.5448, 0.4405, 0.1543, 0.0920]
eff_uncer_m12_VmA = GetUncerEff(eff_m12_VmA)

eff_m12_VpA = [0.6552, 0.6463, 0.6486, 0.6568, 0.6422, 0.5430, 0.4436, 0.1568, 0.0832]
eff_uncer_m12_VpA = GetUncerEff(eff_m12_VpA)

eff_m14_VmA = [0.6545, 0.6499, 0.6529, 0.6388, 0.6597, 0.5463, 0.4479, 0.1614, 0.0869]
eff_uncer_m14_VmA = GetUncerEff(eff_m14_VmA)

eff_m14_VpA = [0.6506, 0.6565, 0.6590, 0.6460, 0.6423, 0.5437, 0.4413, 0.1513, 0.0859]
eff_uncer_m14_VpA = GetUncerEff(eff_m14_VpA)

eff_m16_VmA = [0.6633, 0.6589, 0.6569, 0.6571, 0.6567, 0.5520, 0.4574, 0.1648, 0.0940]
eff_uncer_m16_VmA = GetUncerEff(eff_m16_VmA)

eff_m16_VpA = [0.6691, 0.6613, 0.6598, 0.6631, 0.6460, 0.5592, 0.4626, 0.1611, 0.0899]
eff_uncer_m16_VpA = GetUncerEff(eff_m16_VpA)

lifetime_axis = [0.1, 0.5, 1, 5, 10, 50, 100, 500, 1000] # mm

fig, ax = plt.subplots()
ax.set_ylabel("efficiency")
ax.set_xlabel(r"$c\cdot\tau$ [mm]")

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
