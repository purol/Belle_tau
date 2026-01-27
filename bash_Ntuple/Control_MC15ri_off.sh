#!/bin/sh

# bkgs
gbasf2 ./python/gbasf2_Ichirin.py --force -p UUBAR_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_uubar_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type uubar --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p DDBAR_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_ddbar_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type ddbar --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p SSBAR_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_ssbar_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type ssbar --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p CCBAR_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_ccbar_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type ccbar --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p TAUTAU_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_taupair_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type taupair --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EE_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_ee_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type ee --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EEEE_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_eeee_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type eeee --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EEMUMU_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_eemumu_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type eemumu --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EETAUTAU_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_eetautau_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type eetautau --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EEPIPI_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_eepipi_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type eepipi --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EEKK_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_eeKK_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type eeKK --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p EEPP_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_eepp_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type eepp --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p GG_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_gg_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type gg --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p MUMU_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_mumu_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type mumu --energy off --control"
gbasf2 ./python/gbasf2_Ichirin.py --force -p MUMUMUMU_CTRL2_OF_1 -i /belle/collection/MC/MC15ri_mumumumu_4S_offres_v1 -s light-2505-deimos --cputime 800 --basf2opt="-- --sample MC15ri --type mumumumu --energy off --control"
