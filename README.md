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

## Toy MC Study (MC16rd)

`bash/one_touch_MC16rd.sh` and `bash/one_touch_MC16rd_ALP.sh` run a toy study after workspace creation. The existing positional arguments, `bsub -q s` submission, job waiting and log checking are also used for this stage. ALP studies run independently for every `(mass, life, A, B)` workspace.

- Injected signal strength: `0, 0.25, ..., 5`.
- Outer toys: `ToyMC_NToys=10000` per injected strength.
- Batch size: `ToyMC_ToysPerJob=10`; adjust after timing representative workspaces against the queue time limit.
- Inner CLs toys: `ToyMC_CLsToys=10000` for each hypothesis at each tested strength.
- Base seed: `ToyMC_Seed=12345`; the workspace name, injected strength and absolute toy index determine each nonzero toy seed independently of batch size.

Build the executables before running the pipeline:

```bash
make -C analysis_code ./bin/Run_ToyMC ./bin/ReadToyMC ./bin/Run_CLs
```

The standalone executables use positional arguments:

```bash
# input path, workspace file, output path, injected mu, first toy index,
# toys in this job, CLs toys per hypothesis, initial scan step, base seed
./analysis_code/bin/Run_ToyMC ./Yuugi/v000 workspace.root ./Yuugi/v000/ToyMC/out 1.25 0 10 10000 0.1 12345

# ToyMC root path, output path, expected toys per injected mu
./analysis_code/bin/ReadToyMC ./Yuugi/v000/ToyMC/out ./Yuugi/v000/ToyMC 10000
```

Generation uses the eight bin contents of `bkg_MC_th1d` and `signal_MC_th1d` directly: each count is an independent `Poisson(background_MC + injected_mu * signal_MC)` draw, including signal in the control regions. There is no preliminary fit to `obsData` and no adjustment of the MC background to enforce the ABCD relation. MC nonclosure is therefore preserved. The existing positional arguments are unchanged: `workspace.root` uses `histogram_output.root` from the same input directory, and `workspace_<mass>_<life>_<A>_<B>.root` uses the matching `histogram_output_<mass>_<life>_<A>_<B>.root`. Missing histograms, incompatible binning, and negative or nonfinite expectations are errors; zero-expectation bins generate zero events.

Systematic truth remains at the workspace nominal values. Global observables fluctuate for each toy; nuisances are profiled in the fits. The ABCD workspace is used for fitting and the internal CLs hypothesis toys, not for generating the outer study's event counts. Coverage thus tests the ABCD analysis against the specified MC background, including any nonclosure. The output stores toy counts, seeds, fit results and CLs scan points. File checksums and run metadata are not stored or compared.

Each outer toy is fitted and then treated as the observation for a 90% frequentist CLs upper limit. Conditional generation fits and likelihood-ratio fits share the empty-bin handling in `analysis_code/include/ABCD_fit.h`. The UL scan brackets the `CLs=0.1` crossing and refines it to `0.0025 + 0.005 * mu_high`; injection is limited to five, but the UL scan can extend to the workspace POI maximum. Reaching that maximum without a crossing is a failure, not a clipped upper limit. Finite inner-toy statistics remain a separate source of uncertainty; the per-point CLs errors and local UL error estimate are stored and can be checked with increased `ToyMC_CLsToys`.

Outputs are written under `ToyMC/` or `ToyMC_<mass>_<life>_<A>_<B>/`:

- `out/ToyMC_<mu>_<first_index>.root`: toy fits, ULs, statuses, seeds and scan points.
- `ToyMC.root` and `ToyMC.txt`: merged results, fitted mean and its standard error, bias, UL median and central 68%/95% intervals, coverage and failure fractions.
- `fitted_mu_mean.png`, `fitted_mu_bias.png`, `upper_limit.png`, `coverage_valid.png`, `coverage_bounds.png`, `fit_failure.png` and `ul_failure.png`.

`ul_status` is zero on success; values 1--5 indicate a fit/generation failure, invalid CLs/conditional fit, failed inner fit, missing crossing before the POI maximum, or scan nonconvergence. Failed calculations remain in the output. `coverage` is reported only when every toy at that injection has a valid UL. Otherwise it is NaN, `coverage_valid` describes the successful subset, and `coverage_low/high` bound the full-ensemble coverage by treating unresolved toys as all excluded/all covered. These bounds describe missing results, not a statistical confidence interval. `ambiguous` counts valid limits whose final numerical bracket contains the injected strength. The nonnegative POI boundary can produce a positive fitted mean at zero injection.

The merge checks the toy count and duplicate toy indices. Batch and merge job logs are checked by the shell scripts. Use one output directory per workspace and run configuration; input-file and configuration matching is not checked by the reader. Existing logs are truncated before resubmission, and re-running the same batch replaces its ROOT output. Statistical fit/UL failures remain in the result and failure fractions.
