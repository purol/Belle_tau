#!/usr/bin/env bash
# Run in the Belle II environment with weight_table and its data available.
# Existing CSV, PNG, and per-script log files in OUTPUT_DIR are overwritten.
set -euo pipefail

usage() {
    printf 'Usage: bash %s [--dry-run] [OUTPUT_DIR]\n' "${0##*/}"
    printf 'Default OUTPUT_DIR: current directory. Override Python with PYTHON=/path/to/python.\n'
}

dry_run=false
output_dir=''
while (( $# > 0 )); do
    case "$1" in
        --dry-run) dry_run=true ;;
        -h|--help) usage; exit 0 ;;
        --) shift; break ;;
        -*) printf 'Unknown option: %s\n' "$1" >&2; usage >&2; exit 2 ;;
        *)
            if [[ -n "$output_dir" ]]; then
                usage >&2
                exit 2
            fi
            output_dir=$1
            ;;
    esac
    shift
done
if (( $# > 1 )) || { (( $# == 1 )) && [[ -n "$output_dir" ]]; }; then
    usage >&2
    exit 2
fi
if (( $# == 1 )); then output_dir=$1; fi
output_dir=${output_dir:-$PWD}

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
python=${PYTHON:-python3}
shopt -s nullglob
generators=("$script_dir"/*ID_[0-9]*_eff_corr*.py)
combiners=("$script_dir"/*ID_eff_corr*.py)
if (( ${#generators[@]} == 0 || ${#combiners[@]} == 0 )); then
    printf 'Missing PID generation or combination scripts in: %s\n' "$script_dir" >&2
    exit 1
fi
scripts=("${generators[@]}" "${combiners[@]}")

printf 'PID generators: %d; combiners: %d\n' "${#generators[@]}" "${#combiners[@]}"
printf 'Output directory: %s\n' "$output_dir"
if "$dry_run"; then
    for script in "${scripts[@]}"; do printf '%s\n' "${script##*/}"; done
    exit 0
fi
if ! command -v "$python" >/dev/null 2>&1; then
    printf 'Python executable not found: %s\n' "$python" >&2
    exit 1
fi
mkdir -p -- "$output_dir"
cd -- "$output_dir"
log_dir="$PWD/pid_correction_logs"
mkdir -p -- "$log_dir"
export MPLBACKEND=${MPLBACKEND:-Agg}

index=0
for script in "${scripts[@]}"; do
    index=$((index + 1))
    name=${script##*/}
    log_file="$log_dir/${name%.py}.log"
    printf '\n[%d/%d] %s\n' "$index" "${#scripts[@]}" "$name"
    if "$python" -u "$script" 2>&1 | tee "$log_file"; then
        :
    else
        status=$?
        printf 'FAILED: %s (exit %d). Log: %s\n' "$name" "$status" "$log_file" >&2
        exit "$status"
    fi
done
printf '\nCompleted all %d PID scripts. Outputs: %s\nLogs: %s\n' "${#scripts[@]}" "$PWD" "$log_dir"
