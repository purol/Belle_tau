# Belle II $\tau \to \mu \mu \mu$ End-to-End Data Pipeline

## Overview
This repository contains a fully automated, end-to-end data processing and analysis pipeline for the $\tau \to \mu \mu \mu$ search in the Belle II experiment. 

It handles the entire lifecycle of massive physics data: from distributed grid computing for processing **Petabyte-scale raw data and over 10 Billion Monte Carlo (MC) events**, to an automated bash-scripting orchestrator that performs machine learning (FastBDT) training, cut optimization, and final statistical upper-limit calculations.

This project relies heavily on my custom OOP-based C++ engine, **[Belle2_analysis](https://github.com/purol/Belle2_analysis)**, which is included as a submodule.

## Tech Stack & Engineering Focus
- **Distributed Computing:** KEK Internal Grid Computing Cluster (LSF Batch System)
- **Framework Integration:** Belle II Official Framework (`basf2` - Python)
- **Pipeline Orchestration:** Bash / Shell scripting (`bjobs` polling, error handling)
- **Core Engine & Statistics:** C++17, ROOT
- **Key Focus:** Massive Data Processing, Automated Batch Orchestration, Data Pipeline Reliability

## System Architecture & Workflow

The pipeline is entirely automated and divided into two main stages:

### Phase 1: Distributed Grid Computing (`./python`)
- Utilizes the official `basf2` framework via Python scripts.
- Submits thousands of jobs to the **KEK LSF grid computing system** to process **Petabytes of raw collision data and 10B+ MC events**.
- Skims and reduces the raw datasets into structured ROOT Ntuples for local analysis.

### Phase 2: "One-Touch" Automated Analysis Pipeline (`./bash/one_touch.sh`)
Once the Ntuples are prepared, the `one_touch.sh` script automates the entire downstream analysis with a single command. **This 12-hour autonomous pipeline acts as an orchestrator (similar to Apache Airflow)** with the following features:
- **Dependency Management:** Uses `bjobs` to poll the status of LSF batch jobs (checking every 5 minutes) and dynamically executes the next stage.
- **Error Handling:** Automatically stops the pipeline (`exit 1`) if any sub-job fails (`$? -ne 0`), preventing cascading failures.
- **Full Workflow Execution:** 
  1. Data Cut & Preselection (Using custom C++ string expression parser)
  2. 2D Fitting & MVA Split
  3. Hyperparameter Grid Search & FastBDT (Machine Learning) Training
  4. AUC Testing & Punzi FOM Optimization
  5. PCA & Final CLs (Statistical Upper Limit) Calculations

## Engineering Achievements
- **Processed 10 Billion+ Events:** Successfully managed and analyzed Petabyte-scale raw data, ensuring memory safety and computational efficiency using a custom C++ memory-optimized Loader class.
- **Massive Pipeline Automation:** Replaced highly manual, step-by-step physics analysis procedures with a single robust bash pipeline. This **12-hour continuous workflow completely eliminated the need for manual monitoring** and significantly improved operational efficiency.
