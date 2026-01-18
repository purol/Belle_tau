#!/bin/bash

export Analysis_Name="Okina"
export Analysis_VerName="v004"

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau"
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple"


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

bash ./submitter_Analysis_ALP.sh
wait_job "Analyze"

bash ./checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_fit_ALP.sh
wait_job "ALPFIT"