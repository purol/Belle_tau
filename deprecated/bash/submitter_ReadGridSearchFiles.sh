#!/bin/bash

code="${Belle_tau_DIR}/analysis_code/bin/ReadGridSearchFile"

bsub -q s -J GRIDFILE -o "/dev/null" ${code} "./${Analysis_Name}/${Analysis_VerName}/GridSearch_one/out/" "./${Analysis_Name}/${Analysis_VerName}/GridSearch_one/"
bsub -q s -J GRIDFILE -o "/dev/null" ${code} "./${Analysis_Name}/${Analysis_VerName}/GridSearch_two/out/" "./${Analysis_Name}/${Analysis_VerName}/GridSearch_two/"

