#!/bin/bash

# =================================================================================== #
#  PREDEFINED VARIABLES
#  You SHOULD change these values 

export Analysis_Name="Konpaku" # name of analysis
export Analysis_VerName="v000" # version of analysis

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple" # Ntuple path

Background_Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "PIPIPI0ISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "BBs" "BsBs") # name of directories under ${Ntuple_DIR} for background sample
Signal_Type="SIGNAL" # name of directories under ${Ntuple_DIR} for prompt signal sample
ALP_Type="ALP" # name of directories under ${Ntuple_DIR} for prompt ALP signal sample
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

export Types_STR_WITH_ALP
Types_STR_WITH_ALP=$(IFS=:; echo "${Types_With_ALP[*]}")

export Types_STR_WITH_SIGNAL
Types_STR_WITH_SIGNAL=$(IFS=:; echo "${Types_With_SIGNAL[*]}")

export Types_STR_WITH_SIGNAL_ALP
Types_STR_WITH_SIGNAL_ALP=$(IFS=:; echo "${Types_With_SIGNAL_ALP[*]}")

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

bash ${shell_DIR}/submitter_Analysis_ALP.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_plot_M_ALP.sh

bash ${shell_DIR}/submitter_fit_2D_ALP.sh
wait_job "2DFIT"

bash ${shell_DIR}/submitter_Analysis_second_ALP.sh
wait_job "Analyze"

bash ${shell_DIR}/checker_Analysis_second.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_FBDT_splitter.sh
wait_job "MVASPLIT"
bash ${shell_DIR}/checker_FBDT_splitter.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_FBDTGridSearch_ALP.sh
wait_job "FBDTTRN"
bash ${shell_DIR}/checker_FBDTGridSearch_ALP.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_FBDT_AUC_train_ALP.sh
wait_job "AUCTRN"

bash ${shell_DIR}/submitter_FBDT_AUC_test_ALP.sh
wait_job "AUCTST"

bash ${shell_DIR}/submitter_ReadGridSearchFiles_ALP.sh
wait_job "GRIDFILE"

bash ${shell_DIR}/submitter_FBDT_Application_ALP.sh
wait_job "FBDTAPP"
bash ${shell_DIR}/checker_FBDT_Application.sh
if [[ $? -ne 0 ]]; then
  echo "Unsuccessful logs found. Stopping the one touch analysis."
  exit 1
fi

bash ${shell_DIR}/submitter_KStest_ALP.sh

bash ${shell_DIR}/submitter_PunziFOM_ALP.sh
wait_job "FBDTFOM"

bash ${shell_DIR}/submitter_Calculator_ALP.sh
wait_job "SYSTCAL"

bash ${shell_DIR}/submitter_PCA_ALP.sh
wait_job "PCA"

bash ${shell_DIR}/submitter_Create_workspace_ALP.sh
wait_job "CRTWS"

bash ${shell_DIR}/submitter_CLs_ALP.sh
wait_job "TAUCLS"

bash ${shell_DIR}/submitter_Merge_ALP.sh
wait_job "MERGECLS"

bash ${shell_DIR}/submitter_ReadCLs_ALP.sh
wait_job "READCLS"

bash ${shell_DIR}/submitter_Plotter_ALP.sh

bash ${shell_DIR}/submitter_Plotter_secondary_ALP.sh