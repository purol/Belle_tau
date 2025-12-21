#!/bin/sh

# bkgs
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p UUBAR_SKIM_OF_1 -i /belle/collection/MC/MC15ri_uubar_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p DDBAR_SKIM_OF_1 -i /belle/collection/MC/MC15ri_ddbar_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p SSBAR_SKIM_OF_1 -i /belle/collection/MC/MC15ri_ssbar_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p CCBAR_SKIM_OF_1 -i /belle/collection/MC/MC15ri_ccbar_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p TAUTAU_SKIM_OF_1 -i /belle/collection/MC/MC15ri_taupair_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EE_SKIM_OF_1 -i /belle/collection/MC/MC15ri_ee_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EEEE_SKIM_OF_1 -i /belle/collection/MC/MC15ri_eeee_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EEMUMU_SKIM_OF_1 -i /belle/collection/MC/MC15ri_eemumu_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EETAUTAU_SKIM_OF_1 -i /belle/collection/MC/MC15ri_eetautau_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EEPIPI_SKIM_OF_1 -i /belle/collection/MC/MC15ri_eepipi_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EEKK_SKIM_OF_1 -i /belle/collection/MC/MC15ri_eeKK_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p EEPP_SKIM_OF_1 -i /belle/collection/MC/MC15ri_eepp_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p GG_SKIM_OF_1 -i /belle/collection/MC/MC15ri_gg_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p MUMU_SKIM_OF_1 -i /belle/collection/MC/MC15ri_mumu_4S_offres_v1 -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p MUMUMUMU_SKIM_OF_1 -i /belle/collection/MC/MC15ri_mumumumu_4S_offres_v1 -s light-2505-deimos --cputime 800

# signal
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p SIGNAL_N_SKIM_OF_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00036273/s00/e1003/4S_offres/r00000/3410931052/mdst -s light-2505-deimos --cputime 800
gbasf2 ./python/tau_mumumu_TauToMuMuMu.py --force -p SIGNAL_P_SKIM_OF_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00036274/s00/e1003/4S_offres/r00000/3410931053/mdst -s light-2505-deimos --cputime 800