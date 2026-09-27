#!/bin/bash

# =================================================================================== #
#  PREDEFINED VARIABLES
#  You SHOULD change these values 

export Analysis_Name="Ibaraki" # name of analysis
export Analysis_VerName="v000" # version of analysis

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple" # Ntuple path

Background_Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CCBAR"
    "MUMU" "EE" "EEEE" "EEMUMU" "LLXX" "HHISR" "GG" 
    "TAUPAIR"
    ) # name of directories under ${Ntuple_DIR} for background sample. Do not include colon.
export Signal_Type="SIGNAL" # name of directories under ${Ntuple_DIR} for prompt signal sample. Do not include colon.
export ALP_Type="ALP" # name of directories under ${Ntuple_DIR} for prompt ALP signal sample. Do not include colon.

Background_Legends=("B#bar{B}" "B#bar{B}" "q#bar{q}" "q#bar{q}" "q#bar{q}" "q#bar{q}"
    "#mu#mu" "ee" "others" "ee#mu#mu" "others" "others" "others"
    "#tau#bar{#tau}"
    ) # legends of background sample for plots. Do not include colon.
export Signal_Legends="SIGNAL" # legends of prompt sample for plots. Do not include colon.
export ALP_Legends="SIGNAL" # legends of prompt sample for plots. Do not include colon.

export MC_version="MC16ri" # version of MC. This should be under ${Ntuple_DIR}/${Analysis_Name}/(type name)

input_variables_one=(
    "missingEnergyOfEventCMS"
    "CleoConeCS__bo1__cm__spcleanMask__bc"
    "KSFWVariables__bohso20__cm__spcleanMask__bc"
    "harmonicMomentThrust2"
    "cleoConeThrust0"
    "harmonicMomentThrust1"
    "roeE__bocleanMask__bc"
    "harmonicMomentThrust4"
    "cosTBTO__bocleanMask__bc"
    "diff_cosToThrustOfEvent_CM"
    "foxWolframR3"
    "sphericity"
    "second_muon_p"
    "cosAngleBetweenMomentumAndVertexVector"
    "missingMomentumOfEventCMS_Py"
    "missingMomentumOfEvent_Px"
    "third_muon_p"
    "avg_cosToThrustOfEvent_CM"
    "roeM__bocleanMask__bc"
    "foxWolframR4"
) # list of input variables for the region 1
input_variables_two=(
    "visibleEnergyOfEventCMS"
    "roeM__bocleanMask__bc"
    "missingMomentumOfEvent"
    "KSFWVariables__bohso22__cm__spcleanMask__bc"
    "CleoConeCS__bo1__cm__spcleanMask__bc"
    "roeE__bocleanMask__bc"
    "harmonicMomentThrust1"
    "harmonicMomentThrust2"
    "diff_cosToThrustOfEvent_CM"
    "cosTBTO__bocleanMask__bc"
    "third_muon_p"
    "harmonicMomentThrust4"
    "KSFWVariables__bomm2__cm__spcleanMask__bc"
    "avg_cosToThrustOfEvent_CM"
    "cleoConeThrust0"
    "second_muon_p"
    "missingMomentumOfEventCMS_Px"
    "missingMass2OfEvent"
    "missingMomentumOfEventCMS_Py"
    "foxWolframR3"
) # list of input variables for the region 2
# =================================================================================== #


export shell_DIR="${Belle_tau_DIR}/bash"

Types_With_ALP=(
  "${Background_Types[@]}"
  "${ALP_Type}"
)

Types_With_SIGNAL=(
  "${Background_Types[@]}"
  "${Signal_Type}"
)

Types_With_SIGNAL_ALP=(
  "${Background_Types[@]}"
  "${Signal_Type}"
  "${ALP_Type}"
)

Legends_With_ALP=(
  "${Background_Legends[@]}"
  "${ALP_Legends}"
)

Legends_With_SIGNAL=(
  "${Background_Legends[@]}"
  "${Signal_Legends}"
)

Legends_With_SIGNAL_ALP=(
  "${Background_Legends[@]}"
  "${Signal_Legends}"
  "${ALP_Legends}"
)

export Types_STR_WITH_ALP
Types_STR_WITH_ALP=$(IFS=:; echo "${Types_With_ALP[*]}")

export Types_STR_WITH_SIGNAL
Types_STR_WITH_SIGNAL=$(IFS=:; echo "${Types_With_SIGNAL[*]}")

export Types_STR_WITH_SIGNAL_ALP
Types_STR_WITH_SIGNAL_ALP=$(IFS=:; echo "${Types_With_SIGNAL_ALP[*]}")

export input_variables_one_STR
input_variables_one_STR=$(IFS=:; echo "${input_variables_one[*]}")

export input_variables_two_STR
input_variables_two_STR=$(IFS=:; echo "${input_variables_two[*]}")

export Background_Types_STR
Background_Types_STR=$(IFS=:; echo "${Background_Types[*]}")

export Legends_STR_WITH_ALP
Legends_STR_WITH_ALP=$(IFS=:; echo "${Legends_With_ALP[*]}")

export Legends_STR_WITH_SIGNAL
Legends_STR_WITH_SIGNAL=$(IFS=:; echo "${Legends_With_SIGNAL[*]}")

export Legends_STR_WITH_SIGNAL_ALP
Legends_STR_WITH_SIGNAL_ALP=$(IFS=:; echo "${Legends_With_SIGNAL_ALP[*]}")

export Background_Legends_STR
Background_Legends_STR=$(IFS=:; echo "${Background_Legends[*]}")

wait_all_job() {
  while true; do
    # Get the number of jobs (excluding the header line)
    job_count=$(bjobs 2>/dev/null | tail -n +2 | wc -l)
  
    if [[ $job_count -eq 0 ]]; then
      echo "No remaining jobs."
      break
    else
      echo "Currently, there are $job_count job(s) running. Checking again in 5 minutes..."
    fi

    # Wait for 5 minutes
    sleep 300
done
}

wait_job() {
  JOBNAME=$1
  while true; do
    # Filter jobs with the JOBNAME and count them
    job_count=$(bjobs 2>/dev/null | grep -w "${JOBNAME}" | wc -l)

    if [[ $job_count -eq 0 ]]; then
      echo "No jobs with the name '${JOBNAME}' are running."
      break
    else
      echo "Currently, there are $job_count job(s) with the name '${JOBNAME}' running. Checking again in 5 minutes..."
    fi

    # Wait for 5 minutes
    sleep 300
  done
}

bash ${shell_DIR}/submitter_Analysis.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_fit_2D.sh
wait_job "2DFIT"

bash ${shell_DIR}/submitter_Analysis_second.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis_second.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_Plotter.sh
bash ${shell_DIR}/submitter_FBDT_splitter.sh
wait_job "MVASPLIT"
bash ${shell_DIR}/checker_FBDT_splitter.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_FBDTGridSearch.sh
wait_job "FBDTTRN"
bash ${shell_DIR}/checker_FBDTGridSearch.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_FBDT_AUC_train.sh
wait_job "AUCTRN"

bash ${shell_DIR}/submitter_FBDT_AUC_test.sh
wait_job "AUCTST"

bash ${shell_DIR}/submitter_ReadGridSearchFiles.sh
wait_job "GRIDFILE"

bash ${shell_DIR}/submitter_FBDT_Application_split.sh
wait_job "FBDTAPP"
bash ${shell_DIR}/checker_FBDT_Application_split.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_KStest.sh
