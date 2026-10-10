#!/bin/sh

# proc16 4S
gbasf2 ./python/gbasf2_Larva.py --force -p p164S_CTRL16_1 -i /belle/collection/Data/proc16_4S_all_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample data --type data --energy 4S --control"

# proc16 off
gbasf2 ./python/gbasf2_Larva.py --force -p p16OFF_CTRL16_1 -i /belle/collection/Data/proc16_offres_all_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample data --type data --energy off --control"

# proc16 5S
gbasf2 ./python/gbasf2_Larva.py --force -p p165S_CTRL16_1 -i /belle/collection/Data/proc16_5Sscan_all_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample data --type data --energy 5Sscan --control"

# prompt 4S
gbasf2 ./python/gbasf2_Larva.py --force -p pro4S_CTRL16_1 -i /belle/collection/Data/exp30_to_exp35_all_4S_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample data --type data --energy 4S --control"

# prompt off
gbasf2 ./python/gbasf2_Larva.py --force -p proOFF_CTRL16_1 -i /belle/collection/Data/exp30_to_exp35_all_4Soffres_v1 -s light-2604-jellyfish --cputime 1600 --basf2opt="-- --sample data --type data --energy off --control"