#!/bin/bash

export Analysis_Name="Konpaku"
export Analysis_VerName="v000"

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

bash ./submitter_plot_M_ALP.sh

bash ./submitter_fit_2D_ALP.sh
wait_job "2DFIT"

bash ./submitter_Analysis_second_ALP.sh
wait_job "Analyze"

bash ./checker_Analysis_second.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_Plotter_ALP.sh
bash ./submitter_FBDT_splitter.sh
wait_job "MVASPLIT"
bash ./checker_FBDT_splitter.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_FBDTGridSearch_ALP.sh
wait_job "FBDTTRN"
bash ./checker_FBDTGridSearch_ALP.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_FBDT_AUC_train_ALP.sh
wait_job "AUCTRN"

bash ./submitter_FBDT_AUC_test_ALP.sh
wait_job "AUCTST"

bash ./submitter_ReadGridSearchFiles_ALP.sh
wait_job "GRIDFILE"

bash ./submitter_FBDT_Application_ALP.sh
wait_job "FBDTAPP"
bash ./checker_FBDT_Application.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ./submitter_KStest_ALP.sh

bash ./submitter_Plotter_secondary_ALP.sh

bash ./submitter_PunziFOM_ALP.sh
wait_job "FBDTFOM"

bash ./submitter_Calculator_ALP.sh
wait_job "SYSTCAL"

bash ./submitter_PCA_ALP.sh
wait_job "PCA"

bash ./submitter_Create_workspace_ALP.sh
wait_job "CRTWS"

bash ./submitter_CLs_ALP.sh
wait_job "TAUCLS"

bash ./submitter_Merge_ALP.sh
wait_job "MERGECLS"

bash ./submitter_ReadCLs_ALP.sh
wait_job "READCLS"