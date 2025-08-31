#!/bin/sh

# bkgs
gbasf2 ./python/tau_mumumu_SKIM.py --force -p UUBAR_SKIM_1 -i /belle/collection/MC/MC15ri_uubar_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p DDBAR_SKIM_1 -i /belle/collection/MC/MC15ri_ddbar_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p SSBAR_SKIM_1 -i /belle/collection/MC/MC15ri_ssbar_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p CCBAR_SKIM_1 -i /belle/collection/MC/MC15ri_ccbar_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p MUMU_SKIM_1 -i /belle/collection/MC/MC15ri_mumu_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p TAUTAU_SKIM_1 -i /belle/collection/MC/MC15ri_taupair_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p MIX_SKIM_1 -i /belle/collection/MC/MC15ri_mixed_1abinv_v2 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p MIX_SKIM_2 -i /belle/collection/MC/MC15ri_mixed_2abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p MIX_SKIM_3 -i /belle/collection/MC/MC15ri_mixed_3abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p CHG_SKIM_1 -i /belle/collection/MC/MC15ri_charged_1abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p CHG_SKIM_2 -i /belle/collection/MC/MC15ri_charged_2abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p CHG_SKIM_3 -i /belle/collection/MC/MC15ri_charged_3abinv_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EE_SKIM_1 -i /belle/collection/MC/MC15ri_ee_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p GG_SKIM_1 -i 	/belle/collection/MC/MC15ri_gg_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EEEE_SKIM_1 -i /belle/collection/MC/MC15ri_eeee_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EEKK_SKIM_1 -i /belle/collection/MC/MC15ri_eeKK_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EETAUTAU_SKIM_1 -i /belle/collection/MC/MC15ri_eetautau_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EEMUMU_SKIM_1 -i /belle/collection/MC/MC15ri_eemumu_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EEPIPI_SKIM_1 -i /belle/collection/MC/MC15ri_eepipi_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p EEPP_SKIM_1 -i /belle/collection/MC/MC15ri_eepp_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p K0K0ISR_SKIM_1 -i /belle/collection/MC/MC15ri_K0K0barISR_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p KKISR_SKIM_1 -i /belle/collection/MC/MC15ri_KKISR_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p MUMUMUMU_SKIM_1 -i /belle/collection/MC/MC15ri_mumumumu_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p MUMUTAUTAU_SKIM_1 -i /belle/collection/MC/MC15ri_mumutautau_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p PIPIISR_SKIM_1 -i 	/belle/collection/MC/MC15ri_pipiISR_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p PIPIPI0ISR_SKIM_1 -i /belle/collection/MC/MC15ri_pipipi0ISR_v1 -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p TAUTAUTAUTAU_SKIM_1 -i /belle/collection/MC/MC15ri_tautautautau_v1 -s light-2505-deimos

# signal
gbasf2 ./python/tau_mumumu_SKIM.py --force -p SIGNAL_N_SKIM_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00026883/s00/e1003/4S/r00000/3410931052/mdst -s light-2505-deimos
gbasf2 ./python/tau_mumumu_SKIM.py --force -p SIGNAL_P_SKIM_1 -i /belle/MC/release-06-00-08/DB00002100/MC15ri_b/prod00026884/s00/e1003/4S/r00000/3410931053/mdst -s light-2505-deimos