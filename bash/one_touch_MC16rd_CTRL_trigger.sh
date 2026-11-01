#!/bin/bash

# =================================================================================== #
#  PREDEFINED VARIABLES
#  You SHOULD change these values 

export Analysis_Name="Larva_CTRL" # name of analysis
export Analysis_VerName="v000" # version of analysis

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple_CTRL" # Ntuple path

Background_Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CCBAR"
    "MUMU" "EE" "EEEE" "EEMUMU" "LLXX" "HHISR" "GG" 
    "TAUPAIR"
    "KS0pinu_pure"
    ) # name of directories under ${Ntuple_DIR} for background sample. Do not include colon.
export Signal_Type="pipipinu" # name of directories under ${Ntuple_DIR} for prompt signal sample. Do not include colon.
export ALP_Type="ALP" # name of directories under ${Ntuple_DIR} for prompt ALP signal sample. Do not include colon.
export data_Type="data"

Background_Legends=("others" "others" "q#bar{q}" "q#bar{q}" "q#bar{q}" "q#bar{q}"
    "#mu#mu" "ee" "others" "ee#mu#mu" "others" "others" "others"
    "#tau#bar{#tau}"
    "#tau#bar{#tau}"
    ) # legends of background sample for plots. Do not include colon.
export Signal_Legends="#tau#rightarrow#pi#pi#pi#nu" # legends of prompt sample for plots. Do not include colon.
export ALP_Legends="SIGNAL" # legends of prompt sample for plots. Do not include colon.
export data_Legends="data"

export MC_version="MC16rd" # version of MC. This should be under ${Ntuple_DIR}/${Analysis_Name}/(type name)
export Trigger_syst_vars=(
    "highest_pt"
    "lowest_pt"
) # list of candidate variables
export Trigger_syst_binnings=(
    "0.0"  "5.0" "8"
    "0.0"  "5.0" "8"
) # min max Nbin for each candidate variable, in the same order
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

Types_With_SIGNAL_data=(
  "${Background_Types[@]}"
  "${Signal_Type}"
  "${data_Type}"
)

Types_With_SIGNAL_ALP_data=(
  "${Background_Types[@]}"
  "${Signal_Type}"
  "${ALP_Type}"
  "${data_Type}"
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
  "${data_Legends}"
)

Legends_With_SIGNAL_data=(
  "${Background_Legends[@]}"
  "${Signal_Legends}"
  "${ALP_Legends}"
  "${data_Legends}"
)

Legends_With_SIGNAL_ALP_data=(
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

export Types_STR_WITH_SIGNAL_DATA
Types_STR_WITH_SIGNAL_DATA=$(IFS=:; echo "${Types_With_SIGNAL_data[*]}")

export Types_STR_WITH_SIGNAL_ALP_DATA
Types_STR_WITH_SIGNAL_ALP_DATA=$(IFS=:; echo "${Types_With_SIGNAL_ALP_data[*]}")

export Legends_STR_WITH_ALP
Legends_STR_WITH_ALP=$(IFS=:; echo "${Legends_With_ALP[*]}")

export Legends_STR_WITH_SIGNAL
Legends_STR_WITH_SIGNAL=$(IFS=:; echo "${Legends_With_SIGNAL[*]}")

export Legends_STR_WITH_SIGNAL_ALP
Legends_STR_WITH_SIGNAL_ALP=$(IFS=:; echo "${Legends_With_SIGNAL_ALP[*]}")

export Legends_STR_WITH_SIGNAL_DATA
Legends_STR_WITH_SIGNAL_DATA=$(IFS=:; echo "${Legends_With_SIGNAL_data[*]}")

export Legends_STR_WITH_SIGNAL_ALP_DATA
Legends_STR_WITH_SIGNAL_ALP_DATA=$(IFS=:; echo "${Legends_With_SIGNAL_ALP_data[*]}")

export Trigger_syst_vars_STR
Trigger_syst_vars_STR=$(IFS=:; echo "${Trigger_syst_vars[*]}")

export Trigger_syst_binnings_STR
Trigger_syst_binnings_STR=$(IFS=:; echo "${Trigger_syst_binnings[*]}")

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

bash ${shell_DIR}/submitter_Analysis_CTRL_trigger.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_calculate_trigger_syst_CTRL_trigger.sh
wait_job "TriggerSyst"