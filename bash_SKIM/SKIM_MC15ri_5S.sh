#!/bin/sh

# bkgs
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p BBs_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029043/s00/e1003/10810/r00000/BBstar/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p BsBs_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029044/s00/e1003/10810/r00000/BstarBstar/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p CCBAR_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029045/s00/e1003/10810/r00000/ccbar/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p CHG_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029046/s00/e1003/10810/r00000/charged/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p DDBAR_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029047/s00/e1003/10810/r00000/ddbar/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p MIX_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029048/s00/e1003/10810/r00000/mixed/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p MUMU_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029049/s00/e1003/10810/r00000/mumu/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p SSBAR_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029050/s00/e1003/10810/r00000/ssbar/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p TAUTAU_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029051/s00/e1003/10810/r00000/taupair/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p UUBAR_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00029052/s00/e1003/10810/r00000/uubar/mdst -s light-2505-deimos --cputime 800

# signal
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p SIGNAL_N_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00036275/s00/e1003/10810/r00000/3410931052/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p SIGNAL_P_SKIM_5S_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00036276/s00/e1003/10810/r00000/3410931053/mdst -s light-2505-deimos --cputime 800