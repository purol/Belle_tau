#!/bin/bash

# =================================================================================== #
#  PREDEFINED VARIABLES
#  You SHOULD change these values 

export Analysis_Name="Konpaku_CTRL" # name of analysis
export Analysis_VerName="v000" # version of analysis

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple_CTRL" # Ntuple path

export FBDT_weight_DIR="/home/belle2/junewoo/storage_ghi/tau_Analysis/Konpaku/v012" # FBDT weight file path
# =================================================================================== #

export shell_DIR="${Belle_tau_DIR}/bash"

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

bash ${shell_DIR}/submitter_Analysis_CTRL.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_Plotter_CTRL.sh

bash ${shell_DIR}/submitter_FBDT_Application_CTRL.sh
wait_job "FBDTAPP"
bash ${shell_DIR}/checker_FBDT_Application_CTRL.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi