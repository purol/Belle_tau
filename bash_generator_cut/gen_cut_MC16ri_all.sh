#!/bin/sh

gbasf2 ./python/gbasf2_Chimi.py --force -p CCBAR_GEN_16i_1 -i /belle/collection/MC/MC16ri_run1_ccbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ccbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p DDBAR_GEN_16i_1 -i /belle/collection/MC/MC16ri_run1_ddbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ddbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p SSBAR_GEN_16i_1 -i /belle/collection/MC/MC16ri_run1_ssbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ssbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p TAUPAIR_GEN_16i_1 -i /belle/collection/MC/MC16ri_run1_taupair_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type taupair --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p UUBAR_GEN_16i_1 -i /belle/collection/MC/MC16ri_run1_uubar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type uubar --energy 4S --prompt --vertex --gencut"

gbasf2 ./python/gbasf2_Chimi.py --force -p CCBAR_GEN_16i_2 -i /belle/collection/MC/MC16ri_run2_PXDoff_ccbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ccbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p DDBAR_GEN_16i_2 -i /belle/collection/MC/MC16ri_run2_PXDoff_ddbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ddbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p SSBAR_GEN_16i_2 -i /belle/collection/MC/MC16ri_run2_PXDoff_ssbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ssbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p TAUPAIR_GEN_16i_2 -i /belle/collection/MC/MC16ri_run2_PXDoff_taupair_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type taupair --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p UUBAR_GEN_16i_2 -i /belle/collection/MC/MC16ri_run2_PXDoff_uubar_4S_v1 -s light-2604-jellyfish --cputime 1600 --cputime 1600 --basf2opt="-- --sample MC16ri --type uubar --energy 4S --prompt --vertex --gencut"

gbasf2 ./python/gbasf2_Chimi.py --force -p CCBAR_GEN_16i_3 -i /belle/collection/MC/MC16ri_run2_PXDon_ccbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ccbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p DDBAR_GEN_16i_3 -i /belle/collection/MC/MC16ri_run2_PXDon_ddbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ddbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p SSBAR_GEN_16i_3 -i /belle/collection/MC/MC16ri_run2_PXDon_ssbar_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type ssbar --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p TAUPAIR_GEN_16i_3 -i /belle/collection/MC/MC16ri_run2_PXDon_taupair_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample MC16ri --type taupair --energy 4S --prompt --vertex --gencut"
gbasf2 ./python/gbasf2_Chimi.py --force -p UUBAR_GEN_16i_3 -i /belle/collection/MC/MC16ri_run2_PXDon_uubar_4S_v1 -s light-2604-jellyfish --cputime 1600 --cputime 1600 --basf2opt="-- --sample MC16ri --type uubar --energy 4S --prompt --vertex --gencut"