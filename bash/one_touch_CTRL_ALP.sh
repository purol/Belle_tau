#!/bin/bash

# =================================================================================== #
#  PREDEFINED VARIABLES
#  You SHOULD change these values 

export Analysis_Name="Konpaku_CTRL" # name of analysis
export Analysis_VerName="v000" # version of analysis

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple_CTRL" # Ntuple path

Background_Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM"
    "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR"
    "PIPIPI0ISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" 
    "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "BBs" "BsBs"
    "pipipinu_pure"
    ) # name of directories under ${Ntuple_DIR} for background sample. Do not include colon.
export Signal_Type="KS0pinu" # name of directories under ${Ntuple_DIR} for prompt signal sample. Do not include colon.
export ALP_Type="ALP" # name of directories under ${Ntuple_DIR} for prompt ALP signal sample. Do not include colon.

Background_Legends=("B#bar{B}" "B#bar{B}" "q#bar{q}" "q#bar{q}" "q#bar{q}" "q#bar{q}"
    "#mu#mu" "ee" "others" "ee#mu#mu" "others" "others" "others" "others"
    "others" "others" "others" "others" "others" "#mu#mu#mu#mu" 
    "others" "others" "#tau#bar{#tau}" "others" "others"
    "#tau#to#pi#pi#pi#nu"
    ) # legends of background sample for plots. Do not include colon.
export Signal_Legends="#tau#rightarrowK_{S}^{0}#pi#nu" # legends of prompt sample for plots. Do not include colon.
export ALP_Legends="SIGNAL" # legends of prompt sample for plots. Do not include colon.

export MC_version="MC15ri" # version of MC. This should be under ${Ntuple_DIR}/${Analysis_Name}/(type name)

input_variables_one=(
    "missingEnergyOfEventCMS"
    "cleoConeThrust0"
    "diff_cosToThrustOfEvent_CM"
    "second_muon_p"
    "cosAngleBetweenMomentumAndVertexVector"
    "first_muon_p"
    "missingMomentumOfEventCMS_theta"
    "totalEnergyOfParticlesInList__bogamma__clevtshape_kinematics__bc"
    "useCMSFrame__bopx__bc"
    "dphi"
    "cosTBz__bocleanMask__bc"
    "third_muon_theta"
    "dcosTheta"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "harmonicMomentThrust3"
    "CleoConeCS__bo3__cm__spcleanMask__bc"
    "aplanarity"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "KSFWVariables__bohso03__cm__spcleanMask__bc"
    "cosToThrustOfEvent"
    "KSFWVariables__bohso00__cm__spcleanMask__bc"
    "KSFWVariables__bohoo3__cm__spcleanMask__bc"
    "cleoConeThrust5"
    "cleoConeThrust6"
    "cleoConeThrust8"
    "KSFWVariables__bohoo0__cm__spcleanMask__bc"
    "cleoConeThrust7"
    "charge_times_ROEcharge"
    "dr"
) # list of input variables for the region 1
input_variables_two=(
    "missingEnergyOfEventCMS"
    "second_muon_p"
    "diff_cosToThrustOfEvent_CM"
    "missingMomentumOfEventCMS_Px"
    "cosAngleBetweenMomentumAndVertexVector"
    "first_muon_p"
    "roeEextra__bocleanMask__bc"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "dcosTheta"
    "missingMomentumOfEventCMS_theta"
    "dr"
    "KSFWVariables__bohoo3__cm__spcleanMask__bc"
    "third_muon_theta"
    "dphi"
    "KSFWVariables__bohoo0__cm__spcleanMask__bc"
    "cleoConeThrust1"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "harmonicMomentThrust3"
    "nParticlesInList__bopi__pl__clevtshape_kinematics__bc"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "cleoConeThrust2"
    "KSFWVariables__bohso04__cm__spcleanMask__bc"
    "aplanarity"
    "cleoConeThrust3"
    "KSFWVariables__bohso03__cm__spcleanMask__bc"
    "cleoConeThrust5"
    "cleoConeThrust6"
    "CleoConeCS__bo8__cm__spcleanMask__bc"
    "cleoConeThrust8"
    "cleoConeThrust4"
) # list of input variables for the region 1

export FBDT_weight_DIR="/home/belle2/junewoo/storage_ghi/tau_Analysis/Konpaku_ALP/v012" # FBDT weight file path
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

bash ${shell_DIR}/submitter_Analysis_CTRL_ALP.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_Plotter_CTRL_ALP.sh

bash ${shell_DIR}/submitter_FBDT_Application_CTRL_ALP.sh
wait_job "FBDTAPP"
bash ${shell_DIR}/checker_FBDT_Application_CTRL.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_Plotter_secondary_CTRL_ALP.sh