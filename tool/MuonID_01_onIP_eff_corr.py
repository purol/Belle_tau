import sys
import numpy as np
import matplotlib.pyplot as plt
from IPython.display import HTML

# Path to scripts on KEKCC
sys_path = '/group/belle2/dataprod/Systematics/systematic_corrections_framework/scripts'
# for NAF: 
# sys_path = '/nfs/dust/belle2/group/dataprod/Systematics/systematic_corrections_framework/scripts'
sys.path.insert(1, sys_path)

import weight_table as wm
from show_db_content import show_db_content
from show_variables import show_ntuple_variables

ratio_cfg = {
    "cut": "muonID > 0.1",
    "particle_type": "mu",
    "data_collection": "proc16+prompt",
    "mc_collection": "MC16rd_proc16+prompt",
    "track_variables": ["p", "cosTheta"],
    "precut": "[-3.0 < dz < 3.0] and [dr < 1.0]",
    "model_names": ["Radmumu"],
    "output": "MuonEff01_on.csv",
    "binning": [[0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0],
               [-0.866, -0.682, -0.4226, -0.1045, 0.225, 0.5, 0.766, 0.8829, 0.9563]]
}

efficiency = wm.produce_data_mc_ratio(**ratio_cfg)
efficiency.plot()

plt.savefig("MuonEff01_on.png", dpi=400, bbox_inches="tight")