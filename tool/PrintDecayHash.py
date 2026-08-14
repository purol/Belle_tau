#!/usr/bin/env python3
import uproot
import basf2 as b2
from decayHash import DecayHashMap
import sys

import argparse

# event info to check ("__experiment__", "__run__", "__event__", "__production__", "__candidate__", "__ncandidates__")
event_info_list = [("__experiment__", "__run__", "__event__", "__production__", "__candidate__", "__ncandidates__")]

# get event variable
parser = argparse.ArgumentParser(description='file info')

parser.add_argument('--input', required=True, type=str, help='input root file')
parser.add_argument('--hashmap', required=True, type=str, help='input hashmap file')

args = parser.parse_args()

input_file = args.input
hashmap_file = args.hashmap

# read files
data = uproot.open(b2.find_file(input_file))["variables"].arrays(library="pd")
hashmap = DecayHashMap(b2.find_file(hashmap_file), removeRadiativeGammaFlag=False)

for event_info in event_info_list:
    __experiment__ = event_info[0]
    __run__ = event_info[1]
    __event__ = event_info[2]
    __production__ = event_info[3]
    __candidate__ = event_info[4]
    __ncandidates__ = event_info[5]

    event = data[(data['__experiment__'] == __experiment__) & (data['__run__'] == __run__) & (data['__event__'] == __event__) & (data['__production__'] == __production__) & (data['__candidate__'] == __candidate__) & (data['__ncandidates__'] == __ncandidates__)]
    event = event.iloc[0][["extraInfo__boDecayHash__bc", "extraInfo__boDecayHashExtended__bc"]].values

    print(event_info)

    # print the reconstructed decay
    print("Reconstructed Decay: ")
    rec = hashmap.get_reconstructed_decay(*event)
    print(rec.to_string())

    # print the original decay as simulated in MC
    print("Monte Carlo Decay: ")
    org = hashmap.get_original_decay(*event)
    print(org.to_string())

    print("")