#!/usr/bin/env python3

import uproot
import decayHash
import basf2 as b2
from decayHash import DecayHashMap
import sys
import os
import glob
import gc
import re

# put manually
deltaE_peak = -0.000354
deltaE_left_sigma = 0.013997
deltaE_right_sigma = 0.012297
M_peak = 1.777107
M_left_sigma = 0.004605
M_right_sigma = 0.004834
theta = -0.247233

root_path = sys.argv[1] # path which includes named root file
hashmap_path = sys.argv[2] # path which includes hashmap

output_path = sys.argv[3]
output_file_name = sys.argv[4]

root_list = [f for f in os.listdir(root_path) if f.endswith('.root')]
f = open(output_path + "/" + output_file_name, 'w')

for i in range(0, len(root_list)):

    try:
        data = uproot.open(os.path.join(root_path, root_list[i]))["tau_lfv"].arrays(library="pd")
    except Exception as e:
        continue

    if len(data) == 0:
        continue

    # extract job id
    match = re.search(r'job(\d+)', root_list[i])
    if match:
        job_number = match.group(1)
    else:
        print("No job number found.")
        sys.exit(0)

    hashfile = glob.glob(hashmap_path+"*"+str(job_number)+ "*.root")
    hashfile = [f for f in hashfile if "vertex" not in f]
    if not hashfile:
        print("Hashfile is not found")
        continue
    else:
        hashfile = hashfile[0]
    
    hashmap = DecayHashMap(hashfile, removeRadiativeGammaFlag=True)

    #print(root_list[i])
    #print(hashfile)

    for j in range(0, len(data)):

        candidate = data.iloc[j][["extraInfo__boDecayHash__bc", "extraInfo__boDecayHashExtended__bc"]].values
        f.write(str(data.iloc[j]["__experiment__"]) + "\n")
        f.write(str(data.iloc[j]["__run__"]) + "\n")
        f.write(str(data.iloc[j]["__event__"]) + "\n")
        f.write(str(data.iloc[j]["__candidate__"]) + "\n")
        f.write(str(data.iloc[j]["__ncandidates__"]) + "\n")

        # get variables
        Mtau = data.iloc[j]["M_inv_tau"]
        deltaE = data.iloc[j]["deltaE"]


        # cut
        if ((deltaE_peak - 5*deltaE_left_sigma)< deltaE) and (deltaE < (deltaE_peak - 5*deltaE_right_sigma)) and ((M_peak - 5*M_left_sigma)< Mtau) and (Mtau < (M_peak - 5*M_right_sigma)):
            pass
        else:
            continue

        # print the reconstruced decay
        f.write("Reconstructed Decay: " + "\n")
        rec = hashmap.get_reconstructed_decay(*candidate)
        f.write(rec.to_string() + "\n")

        # print the original decay as simulated in MC
        f.write("Monte Carlo Decay: " + "\n")
        org = hashmap.get_original_decay(*candidate)
        f.write(org.to_string() + "\n")

        f.write("===============================" + "\n")
        f.flush()

    del [[data]]
    gc.collect()
    
f.close()
sys.exit(0)
