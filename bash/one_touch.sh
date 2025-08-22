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

bash ./submitter_Analysis.sh
wait_job "Analyze"

bash ./checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_fit_2D.sh
wait_job "2DFIT"

bash ./submitter_Analysis_second.sh
wait_job "Analyze"

bash ./checker_Analysis_second.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_Plotter.sh
bash ./submitter_FBDT_splitter.sh
wait_job "MVASPLIT"
bash ./checker_FBDT_splitter.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_Autogluon_train.sh
wait_job "MVATRN"
bash ./checker_Autogluon_train.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_autogluon_Application.sh
wait_job "MVAAPP"
bash ./checker_autogluon_Application.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_CSV_Application.sh
wait_job "CSVAPP"
bash ./checker_CSV_Application.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_KStest.sh

bash ./submitter_Plotter_secondary.sh

bash ./submitter_PunziFOM.sh
wait_job "FBDTFOM"

bash ./submitter_Analysis_third.sh
wait_job "Analyze"
bash ./checker_Analysis_third.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_Plotter_third.sh

bash ./submitter_Calculator.sh
wait_job "PIDCAL"

bash ./submitter_PCA.sh
wait_job "PCA"

bash ./submitter_Create_workspace.sh
wait_job "CRTWS"

bash ./submitter_CLs.sh
wait_job "TAUCLS"

bash ./submitter_Merge.sh
wait_job "MERGECLS"

bash ./submitter_ReadCLs.sh
wait_job "READCLS"


