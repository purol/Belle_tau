#!/usr/bin/env python3

from __future__ import annotations

import gc
import json
import os
import tempfile
from pathlib import Path

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np
import uproot

from sklearn.linear_model import LogisticRegression
from sklearn.metrics import roc_auc_score, roc_curve
from sklearn.preprocessing import PolynomialFeatures, StandardScaler
from sklearn.model_selection import train_test_split


# ============================================================
# 설정
# ============================================================

signal_directory = Path("./signal")
background_directory = Path("./background")

tree_name = "gen_info"


# ------------------------------------------------------------
# Region 정의에 사용할 branch
# ------------------------------------------------------------

strict0_branch = (
    "nParticlesInList__botau__pl__clfake_strict0__bc"
)

strict1_branch = (
    "nParticlesInList__botau__pl__clfake_strict1__bc"
)

strict2_branch = (
    "nParticlesInList__botau__pl__clfake_strict2__bc"
)


# ------------------------------------------------------------
# Region A에서 사용할 변수
#
# Region A:
# tau+:fake_strict2 candidate가 존재
# ------------------------------------------------------------

variables_a = [
#    "thrust",
    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "pt_sum_gencut",
#    "sphericity",
#    "aplanarity",
    "Ntrack_gencut",
    "cosTBz",
    "foxWolframR1",
#    "foxWolframR2",
#    "foxWolframR3",
#    "harmonicMomentThrust0",
#    "harmonicMomentThrust1",
#    "R2",
#    "genMissingEnergyOfEventCMS",
    "nParticlesInList__botau__pl__clfake_strict0__bc",
#    "nParticlesInList__botau__pl__clfake_strict1__bc",
    "nParticlesInList__botau__pl__clfake_strict2__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cm__spM__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cm__spMbc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM2__cm__spM__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM2__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM2__cm__spMbc__bc",
#    "averageValueInList__botau__pl__clfake_strict2__cm__spM__bc",
#    "averageValueInList__botau__pl__clfake_strict2__cm__spdeltaE__bc",
#    "extraInfo__bostd_M2__bc",
#    "extraInfo__bostd_deltaE2__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cmdaughterHighest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cmdaughterLowest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM2__cmdaughterHighest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM2__cmdaughterLowest__bop__bc__bc",
]


# ------------------------------------------------------------
# Region B에서 사용할 변수
#
# Region B:
# tau+:fake_strict1 candidate는 존재하지만
# tau+:fake_strict2 candidate는 존재하지 않음
# ------------------------------------------------------------

variables_b = [
    "thrust",
#    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "pt_sum_gencut",
    "sphericity",
#    "aplanarity",
    "Ntrack_gencut",
    "cosTBz",
    "foxWolframR1",
    "foxWolframR2",
#    "foxWolframR3",
    "harmonicMomentThrust0",
#    "harmonicMomentThrust1",
    "R2",
#    "genMissingEnergyOfEventCMS",
    "nParticlesInList__botau__pl__clfake_strict0__bc",
#    "nParticlesInList__botau__pl__clfake_strict1__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cm__spM__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cm__spMbc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cm__spM__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM1__cm__spdeltaE__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cm__spMbc__bc",
#    "averageValueInList__botau__pl__clfake_strict1__cm__spM__bc",
    "averageValueInList__botau__pl__clfake_strict1__cm__spdeltaE__bc",
#    "extraInfo__bostd_M1__bc",
#    "extraInfo__bostd_deltaE1__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cmdaughterHighest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cmdaughterLowest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cmdaughterHighest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cmdaughterLowest__bop__bc__bc",
]


# ------------------------------------------------------------
# Region C에서 사용할 변수
#
# Region C:
# tau+:fake_strict0 candidate는 존재하지만
# tau+:fake_strict1/strict2 candidate는 존재하지 않음
# ------------------------------------------------------------

variables_c = [
#    "thrust",
    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "pt_sum_gencut",
    "sphericity",
#    "aplanarity",
    "Ntrack_gencut",
    "cosTBz",
#    "foxWolframR1",
    "foxWolframR2",
#    "foxWolframR3",
#    "harmonicMomentThrust0",
#    "harmonicMomentThrust1",
    "R2",
#    "genMissingEnergyOfEventCMS",
    "nParticlesInList__botau__pl__clfake_strict0__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cm__spM__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cm__spMbc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM0__cm__spM__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cm__spdeltaE__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cm__spMbc__bc",
#    "averageValueInList__botau__pl__clfake_strict0__cm__spM__bc",
    "averageValueInList__botau__pl__clfake_strict0__cm__spdeltaE__bc",
#    "extraInfo__bostd_M0__bc",
#    "extraInfo__bostd_deltaE0__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cmdaughterLowest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cmdaughterHighest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cmdaughterLowest__bop__bc__bc",
]


# ------------------------------------------------------------
# Region D에서 사용할 변수
#
# Region D:
# tau+:fake_strict0 candidate가 존재하지 않음
# ------------------------------------------------------------

variables_d = [
    "thrust",
    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "pt_sum_gencut",
    "sphericity",
    "aplanarity",
    "Ntrack_gencut",
    "cosTBz",
    "foxWolframR1",
    "foxWolframR2",
    "foxWolframR3",
    "harmonicMomentThrust0",
    "harmonicMomentThrust1",
    "R2",
    "genMissingEnergyOfEventCMS",
]


# ------------------------------------------------------------
# Region별 설정
# ------------------------------------------------------------

region_configs = {
    "A": {
        "variables": variables_a,
        "description": "strict2 > 0.5",
    },
    "B": {
        "variables": variables_b,
        "description": (
            "strict1 > 0.5 and strict2 <= 0.5"
        ),
    },
    "C": {
        "variables": variables_c,
        "description": (
            "strict0 > 0.5 and strict1 <= 0.5 "
            "and strict2 <= 0.5"
        ),
    },
    "D": {
        "variables": variables_d,
        "description": "strict0 <= 0.5",
    },
}

region_names = ["A", "B", "C", "D"]
trained_region_names = ["A"]
rejected_region_names = ["B", "C", "D"]


# ------------------------------------------------------------
# Dataset 및 학습 설정
# ------------------------------------------------------------

# Background는 signal event 수의 최대 몇 배까지 읽을지
background_to_signal_ratio = 50

# Train/validation 분리 비율
validation_fraction = 0.25

random_seed = 42

# 목표 signal efficiency
efficiency_target = 0.4

# Stage 1 loose logistic regression의 Region A signal efficiency 목표
# (B/C/D는 reject하므로 0.98은 overall efficiency가 아니라 Region A 내부 efficiency)
stage1_efficiency_target_a = 0.99

# Stage 2 loose logistic regression의 conditional signal efficiency 목표
# 즉 Stage 1을 통과한 signal 중 98%를 Stage 2에서도 유지한다.
stage2_conditional_efficiency_target = 0.98

# 각 Region의 threshold 후보 개수
n_threshold_scan = 300

# ROOT 파일을 한 번에 읽을 entry 수
step_size = 100_000

# 메모리 절약을 위해 feature 저장은 float32 사용
storage_dtype = np.float32

# Validation prediction은 전체 행렬을 한 번에 만들지 않고
# 이 크기만큼 batch로 나누어 수행
prediction_batch_size = 100_000

# Quadratic logistic regression 설정
# 참고 TrainLogistic.py와 동일하게 degree=2, L2, C=10을 사용한다.
logistic_degree = 2
logistic_C = 10.0
logistic_solver = "saga"
logistic_max_iter = 1000
logistic_tol = 1.0e-4

# Polynomial feature를 RAM 전체에 만들지 않고 batch 단위로 memmap에 쓴다.
# TMPDIR가 설정되어 있으면 batch node의 local scratch를 우선 사용한다.
polynomial_build_batch_size = 50_000

# basf2 formula 출력 정밀도
formula_precision = 8


# ------------------------------------------------------------
# 출력 경로
# ------------------------------------------------------------

output_directory = Path("./logistic_output")

result_json_path = (
    output_directory / "logistic_cascade3_thresholds.json"
)

roc_plot_path = (
    output_directory / "roc_curve_cascade3_validation.png"
)


# ============================================================
# 보조 함수
# ============================================================

def unique_preserving_order(
    values: list[str],
) -> list[str]:
    """
    순서를 유지하면서 중복을 제거한다.
    """

    return list(dict.fromkeys(values))


# Region B/C/D are rejected without evaluating a classifier, so their
# feature branches do not need to be loaded.  The strict branches below are
# sufficient to classify every event into A/B/C/D.
all_feature_variables = unique_preserving_order(
    variables_a
)

branches_to_read = unique_preserving_order(
    all_feature_variables
    + [
        strict0_branch,
        strict1_branch,
        strict2_branch,
    ]
)


# ============================================================
# ROOT 파일 탐색
# ============================================================

def find_root_files(
    directory: Path,
) -> list[Path]:
    """
    디렉터리 아래 모든 ROOT 파일을 재귀적으로 찾는다.
    """

    if not directory.exists():
        raise FileNotFoundError(
            f"Directory does not exist: {directory}"
        )

    root_files = sorted(
        directory.rglob("*.root")
    )

    if not root_files:
        raise RuntimeError(
            f"No ROOT files found under: {directory}"
        )

    return root_files


# ============================================================
# ROOT 파일에서 event 읽기
# ============================================================

def read_events_from_directory(
    directory: Path,
    branches: list[str],
    max_events: int | None = None,
) -> dict[str, np.ndarray]:
    """
    디렉터리 아래 ROOT 파일을 재귀적으로 읽는다.

    읽기 단계에서는 전체 변수에 finite cut을 적용하지 않는다.

    Region C에서는 BCS candidate 변수가 NaN일 수 있으므로,
    모든 branch에 finite 조건을 적용하면 Region C event가
    제거될 수 있다.

    max_events가 주어진 경우에는 각 branch의 최종 배열을
    처음부터 한 번만 할당하고 그 안에 직접 채운다.
    따라서 chunk list + np.concatenate가 동시에 존재하면서
    생기는 peak memory를 피한다.

    저장 dtype은 storage_dtype(float32)을 사용한다.
    """

    root_files = find_root_files(
        directory
    )

    # Background처럼 max_events가 정해져 있으면 최종 크기의
    # 배열을 한 번만 할당하여 concatenate copy를 피한다.
    if max_events is not None:
        output = {
            branch: np.empty(
                max_events,
                dtype=storage_dtype,
            )
            for branch in branches
        }
        collected = None
    else:
        # Signal처럼 전체 entry 수를 미리 모르는 경우에는
        # chunk를 모은 뒤 마지막에 concatenate한다.
        # Signal sample은 background보다 훨씬 작으므로 peak에
        # 미치는 영향이 제한적이다.
        output = None
        collected = {
            branch: []
            for branch in branches
        }

    n_collected = 0

    for file_index, file_path in enumerate(
        root_files,
        start=1,
    ):
        if (
            max_events is not None
            and n_collected >= max_events
        ):
            break

        try:
            with uproot.open(file_path) as root_file:
                if tree_name not in root_file:
                    print(
                        f"[WARNING] Tree '{tree_name}' "
                        f"not found: {file_path}"
                    )
                    continue

                tree = root_file[tree_name]

                available_branches = set(
                    tree.keys()
                )

                missing_branches = [
                    branch
                    for branch in branches
                    if branch not in available_branches
                ]

                if missing_branches:
                    print(
                        f"[WARNING] Missing branches in "
                        f"{file_path}:"
                    )

                    for branch in missing_branches:
                        print(f"    {branch}")

                    continue

                print(
                    f"[{file_index}/{len(root_files)}] "
                    f"Reading {file_path}"
                )

                for arrays in tree.iterate(
                    expressions=branches,
                    step_size=step_size,
                    library="np",
                ):
                    # 길이는 첫 branch로 확인한다.
                    first_values = np.asarray(
                        arrays[branches[0]]
                    ).reshape(-1)
                    n_chunk = len(first_values)

                    if n_chunk == 0:
                        continue

                    if max_events is None:
                        n_take = n_chunk
                    else:
                        remaining = (
                            max_events
                            - n_collected
                        )

                        if remaining <= 0:
                            break

                        n_take = min(
                            n_chunk,
                            remaining,
                        )

                    start = n_collected
                    stop = n_collected + n_take

                    for branch in branches:
                        values = np.asarray(
                            arrays[branch]
                        ).reshape(-1)

                        if not np.issubdtype(
                            values.dtype,
                            np.number,
                        ):
                            raise TypeError(
                                "Non-numeric branch: "
                                f"{branch}, "
                                f"dtype={values.dtype}"
                            )

                        if len(values) != n_chunk:
                            raise RuntimeError(
                                "Branches have different "
                                f"lengths in {file_path}: "
                                f"{branch} has {len(values)}, "
                                f"expected {n_chunk}"
                            )

                        if max_events is not None:
                            # Assignment 과정에서 storage_dtype으로
                            # 직접 cast되므로 별도의 full-size astype
                            # 결과를 만들지 않는다.
                            output[branch][start:stop] = (
                                values[:n_take]
                            )
                        else:
                            collected[branch].append(
                                values[:n_take].astype(
                                    storage_dtype,
                                    copy=False,
                                )
                            )

                    n_collected = stop

                    if (
                        max_events is not None
                        and n_collected >= max_events
                    ):
                        break

        except Exception as error:
            print(
                f"[WARNING] Failed to read "
                f"{file_path}: {error}"
            )

    if n_collected == 0:
        raise RuntimeError(
            f"No usable events found under: "
            f"{directory}"
        )

    if max_events is None:
        output = {}

        for branch, arrays in collected.items():
            if not arrays:
                raise RuntimeError(
                    "No values were collected for "
                    f"branch: {branch}"
                )

            output[branch] = np.concatenate(
                arrays
            )

        # chunk list가 잡고 있던 reference를 즉시 해제한다.
        del collected
        gc.collect()

    else:
        # 실제로 읽은 event 수까지만 view를 반환한다.
        # max_events까지 읽었다면 원래 배열과 동일하다.
        output = {
            branch: values[:n_collected]
            for branch, values in output.items()
        }

    output_lengths = {
        len(values)
        for values in output.values()
    }

    if len(output_lengths) != 1:
        raise RuntimeError(
            "Collected branches have "
            "inconsistent lengths: "
            f"{output_lengths}"
        )

    print(
        f"Collected {n_collected:,} events "
        f"from {directory} "
        f"with dtype={np.dtype(storage_dtype).name}"
    )

    return output


# ============================================================
# Region mask
# ============================================================

def get_region_mask(
    data: dict[str, np.ndarray],
    region_name: str,
) -> np.ndarray:
    """
    Region A/B/C/D mask를 반환한다.

    A:
        strict2 > 0.5

    B:
        strict2 <= 0.5 and strict1 > 0.5

    C:
        strict2 <= 0.5 and strict1 <= 0.5
        and strict0 > 0.5

    D:
        strict0 <= 0.5
    """

    strict0_values = data[strict0_branch]
    strict1_values = data[strict1_branch]
    strict2_values = data[strict2_branch]

    finite_mask = (
        np.isfinite(strict0_values)
        & np.isfinite(strict1_values)
        & np.isfinite(strict2_values)
    )

    no_strict2 = strict2_values <= 0.5
    no_strict1 = strict1_values <= 0.5
    no_strict0 = strict0_values <= 0.5

    if region_name == "A":
        return finite_mask & (strict2_values > 0.5)

    if region_name == "B":
        return (
            finite_mask
            & no_strict2
            & (strict1_values > 0.5)
        )

    if region_name == "C":
        return (
            finite_mask
            & no_strict2
            & no_strict1
            & (strict0_values > 0.5)
        )

    if region_name == "D":
        return finite_mask & no_strict0

    raise ValueError(
        f"Unknown region name: {region_name}"
    )


# ============================================================
# Region population 출력
# ============================================================

def print_region_population(
    label: str,
    data: dict[str, np.ndarray],
) -> dict[str, int]:
    """
    각 Region의 event 수를 출력한다.
    """

    strict0_values = data[strict0_branch]
    strict1_values = data[strict1_branch]
    strict2_values = data[strict2_branch]

    finite_region_values = (
        np.isfinite(strict0_values)
        & np.isfinite(strict1_values)
        & np.isfinite(strict2_values)
    )

    counts = {
        region_name: int(
            np.count_nonzero(
                get_region_mask(
                    data,
                    region_name,
                )
            )
        )
        for region_name in region_names
    }

    n_total = len(strict0_values)

    n_nonfinite = int(
        np.count_nonzero(
            ~finite_region_values
        )
    )

    n_classified = sum(
        counts.values()
    )

    print()
    print(f"{label} region population")
    print(f"  total:              {n_total:,}")

    for region_name in region_names:
        print(
            f"  Region {region_name}:           "
            f"{counts[region_name]:,}"
        )

    print(
        f"  classified total:   "
        f"{n_classified:,}"
    )

    print(
        f"  non-finite region:  "
        f"{n_nonfinite:,}"
    )

    if n_total > 0:
        for region_name in region_names:
            fraction = (
                counts[region_name]
                / n_total
            )

            print(
                f"  Region {region_name} fraction: "
                f"{fraction:.6f}"
            )

    if n_classified + n_nonfinite != n_total:
        raise RuntimeError(
            f"{label}: Region classification "
            "does not cover all events."
        )

    return counts


# ============================================================
# Train/validation 분리
# ============================================================

def split_indices(
    n_events: int,
    validation_fraction: float,
    random_seed: int,
) -> tuple[np.ndarray, np.ndarray]:
    """
    전체 event index를 train과 validation으로 나눈다.
    """

    indices = np.arange(
        n_events,
        dtype=np.int64,
    )

    train_indices, validation_indices = (
        train_test_split(
            indices,
            test_size=validation_fraction,
            random_state=random_seed,
            shuffle=True,
        )
    )

    return (
        np.asarray(
            train_indices,
            dtype=np.int64,
        ),
        np.asarray(
            validation_indices,
            dtype=np.int64,
        ),
    )


# ============================================================
# Region별 입력 행렬 생성
# ============================================================

def select_region_indices(
    data: dict[str, np.ndarray],
    indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> tuple[np.ndarray, int]:
    """
    지정한 index 중 특정 Region에 속하고, 해당 Region에서
    사용하는 모든 feature가 finite인 event의 원래 index를
    반환한다.

    중요한 점은 여기서 2D feature matrix를 만들지 않는다는
    것이다. 각 variable을 한 번씩 확인하므로 peak memory를
    작게 유지할 수 있다.
    """

    full_region_mask = get_region_mask(
        data,
        region_name,
    )

    selected_indices = indices[
        full_region_mask[indices]
    ]

    if len(selected_indices) == 0:
        return selected_indices, 0

    finite_feature_mask = np.ones(
        len(selected_indices),
        dtype=bool,
    )

    for variable in variables:
        finite_feature_mask &= np.isfinite(
            data[variable][selected_indices]
        )

    n_removed_nonfinite = int(
        np.count_nonzero(
            ~finite_feature_mask
        )
    )

    if n_removed_nonfinite == 0:
        return selected_indices, 0

    return (
        selected_indices[finite_feature_mask],
        n_removed_nonfinite,
    )


def fill_feature_matrix(
    X: np.ndarray,
    row_slice: slice,
    data: dict[str, np.ndarray],
    selected_indices: np.ndarray,
    variables: list[str],
) -> None:
    """
    이미 할당된 X의 지정된 row에 feature를 직접 채운다.

    np.column_stack([...])처럼 모든 column의 temporary array를
    동시에 보관하지 않고 한 column씩 처리한다.
    """

    for column_index, variable in enumerate(
        variables
    ):
        X[
            row_slice,
            column_index,
        ] = data[variable][selected_indices]


def build_feature_matrix(
    data: dict[str, np.ndarray],
    selected_indices: np.ndarray,
    variables: list[str],
) -> np.ndarray:
    """
    선택된 event의 feature matrix를 정확히 한 번 할당한다.
    """

    X = np.empty(
        (
            len(selected_indices),
            len(variables),
        ),
        dtype=storage_dtype,
    )

    fill_feature_matrix(
        X=X,
        row_slice=slice(None),
        data=data,
        selected_indices=selected_indices,
        variables=variables,
    )

    return X



def _iter_index_batches(
    selected_indices: np.ndarray,
    batch_size: int,
):
    """selected index를 작은 batch로 나누어 반환한다."""

    for start in range(0, len(selected_indices), batch_size):
        stop = min(start + batch_size, len(selected_indices))
        yield selected_indices[start:stop]


def fit_region_scaler(
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    selected_signal_indices: np.ndarray,
    selected_background_indices: np.ndarray,
    variables: list[str],
) -> StandardScaler:
    """
    Training sample 전체에 대해 StandardScaler를 fit한다.

    전체 base-feature matrix를 만들지 않고 partial_fit을 사용한다.
    Signal/background class weight는 scaler에는 적용하지 않는다.
    이는 참고 TrainLogistic.py의 StandardScaler.fit(X)와 같은 전략이다.
    """

    scaler = StandardScaler(copy=False)

    for data, selected_indices in (
        (signal_data, selected_signal_indices),
        (background_data, selected_background_indices),
    ):
        for batch_indices in _iter_index_batches(
            selected_indices,
            polynomial_build_batch_size,
        ):
            X_batch = build_feature_matrix(
                data=data,
                selected_indices=batch_indices,
                variables=variables,
            )
            scaler.partial_fit(X_batch)
            del X_batch

    return scaler


def make_polynomial_transformer(
    n_features: int,
) -> PolynomialFeatures:
    """degree-2 polynomial feature ordering을 정의한다."""

    poly = PolynomialFeatures(
        degree=logistic_degree,
        include_bias=False,
        order="C",
    )

    # 실제 event를 넣을 필요 없이 feature 수만 알려주면 된다.
    poly.fit(
        np.zeros(
            (1, n_features),
            dtype=storage_dtype,
        )
    )

    return poly


def build_polynomial_batch(
    data: dict[str, np.ndarray],
    selected_indices: np.ndarray,
    variables: list[str],
    scaler: StandardScaler,
    poly: PolynomialFeatures,
) -> np.ndarray:
    """
    raw variables -> standardization -> quadratic expansion.

    이 함수는 한 batch에 대해서만 polynomial matrix를 만든다.
    """

    X_batch = build_feature_matrix(
        data=data,
        selected_indices=selected_indices,
        variables=variables,
    )

    # copy=False이므로 가능한 경우 X_batch 자체를 표준화한다.
    X_scaled = scaler.transform(
        X_batch,
        copy=False,
    )

    X_poly = poly.transform(X_scaled)

    del X_scaled
    del X_batch

    return np.asarray(
        X_poly,
        dtype=storage_dtype,
        order="C",
    )


def _scratch_directory() -> Path:
    """memmap을 만들 scratch directory를 반환한다."""

    tmpdir = os.environ.get("TMPDIR")

    if tmpdir:
        directory = Path(tmpdir)
    else:
        directory = output_directory / "tmp"

    directory.mkdir(
        parents=True,
        exist_ok=True,
    )

    return directory


def train_region_logistic(
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    signal_indices: np.ndarray,
    background_indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> dict:
    """
    특정 Region의 quadratic logistic regression을 학습한다.

    Training/validation split, Region selection, finite selection,
    signal/background total training weight equalization은 기존 BDT 코드와
    동일하게 유지한다.

    degree-2 matrix는 큰 sample에서 RAM을 많이 사용하므로 임시 memmap에
    한 번 작성하고, 학습이 끝난 즉시 삭제한다.
    """

    (
        selected_signal_indices,
        n_signal_nonfinite,
    ) = select_region_indices(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        region_name=region_name,
    )

    (
        selected_background_indices,
        n_background_nonfinite,
    ) = select_region_indices(
        data=background_data,
        indices=background_indices,
        variables=variables,
        region_name=region_name,
    )

    n_signal_usable = len(selected_signal_indices)
    n_background_usable = len(selected_background_indices)

    print()
    print(f"Region {region_name} logistic training input")
    print(f"  signal usable:         {n_signal_usable:,}")
    print(f"  background usable:     {n_background_usable:,}")
    print(f"  signal non-finite:     {n_signal_nonfinite:,}")
    print(f"  background non-finite: {n_background_nonfinite:,}")

    if n_signal_usable == 0:
        raise RuntimeError(
            f"Region {region_name} contains no usable signal training events."
        )

    if n_background_usable == 0:
        raise RuntimeError(
            f"Region {region_name} contains no usable background training events."
        )

    n_total = n_signal_usable + n_background_usable

    scaler = fit_region_scaler(
        signal_data=signal_data,
        background_data=background_data,
        selected_signal_indices=selected_signal_indices,
        selected_background_indices=selected_background_indices,
        variables=variables,
    )

    poly = make_polynomial_transformer(
        len(variables)
    )
    n_poly_features = int(
        poly.n_output_features_
    )

    cache_bytes = (
        n_total
        * n_poly_features
        * np.dtype(storage_dtype).itemsize
    )

    print(f"  original features:     {len(variables):,}")
    print(f"  quadratic features:    {n_poly_features:,}")
    print(
        "  temporary polynomial cache: "
        f"{cache_bytes / (1024 ** 3):.3f} GiB"
    )

    scratch_directory = _scratch_directory()
    cache_path = None
    X_poly_memmap = None

    try:
        with tempfile.NamedTemporaryFile(
            prefix=f"lr_region_{region_name}_",
            suffix=".mmap",
            dir=scratch_directory,
            delete=False,
        ) as temporary_file:
            cache_path = Path(
                temporary_file.name
            )

        X_poly_memmap = np.memmap(
            cache_path,
            mode="w+",
            dtype=storage_dtype,
            shape=(
                n_total,
                n_poly_features,
            ),
            order="C",
        )

        row_start = 0

        for data, selected_indices in (
            (signal_data, selected_signal_indices),
            (background_data, selected_background_indices),
        ):
            for batch_indices in _iter_index_batches(
                selected_indices,
                polynomial_build_batch_size,
            ):
                X_poly_batch = build_polynomial_batch(
                    data=data,
                    selected_indices=batch_indices,
                    variables=variables,
                    scaler=scaler,
                    poly=poly,
                )

                row_stop = row_start + len(batch_indices)
                X_poly_memmap[
                    row_start:row_stop,
                    :,
                ] = X_poly_batch

                row_start = row_stop
                del X_poly_batch

        if row_start != n_total:
            raise RuntimeError(
                f"Region {region_name}: polynomial cache row mismatch: "
                f"{row_start} != {n_total}"
            )

        X_poly_memmap.flush()

        # 기존 BDT와 동일하게 signal/background의 total training weight를
        # 각각 동일하게 만든다.
        signal_weight = (
            n_total
            / (2.0 * n_signal_usable)
        )
        background_weight = (
            n_total
            / (2.0 * n_background_usable)
        )

        y = np.empty(
            n_total,
            dtype=np.int8,
        )
        y[:n_signal_usable] = 1
        y[n_signal_usable:] = 0

        sample_weight = np.empty(
            n_total,
            dtype=storage_dtype,
        )
        sample_weight[:n_signal_usable] = signal_weight
        sample_weight[n_signal_usable:] = background_weight

        classifier = LogisticRegression(
            C=logistic_C,
            penalty="l2",
            solver=logistic_solver,
            max_iter=logistic_max_iter,
            tol=logistic_tol,
            fit_intercept=True,
            random_state=(
                random_seed
                + {"A": 0, "B": 1, "C": 2, "D": 3}[region_name]
            ),
        )

        classifier.fit(
            X_poly_memmap,
            y,
            sample_weight=sample_weight,
        )

        del y
        del sample_weight

        # memmap 삭제 전에 coefficient/intercept는 일반 ndarray/scalar로 복사한다.
        weights = np.asarray(
            classifier.coef_[0],
            dtype=np.float64,
        ).copy()
        bias = float(
            classifier.intercept_[0]
        )
        n_iter = int(
            np.max(classifier.n_iter_)
        )

    finally:
        if X_poly_memmap is not None:
            del X_poly_memmap

        gc.collect()

        if (
            cache_path is not None
            and cache_path.exists()
        ):
            cache_path.unlink()

    del selected_signal_indices
    del selected_background_indices
    gc.collect()

    print()
    print(f"Region {region_name} logistic training result")
    print(f"  signal:              {n_signal_usable:,}")
    print(f"  background:          {n_background_usable:,}")
    print(f"  original features:   {len(variables):,}")
    print(f"  quadratic features:  {n_poly_features:,}")
    print(f"  iterations:          {n_iter:,}")

    return {
        "classifier": classifier,
        "scaler": scaler,
        "poly": poly,
        "variables": list(variables),
        "weights": weights,
        "bias": bias,
        "n_iter": n_iter,
        "n_signal_train": n_signal_usable,
        "n_background_train": n_background_usable,
    }


def predict_region_scores(
    model: dict,
    data: dict[str, np.ndarray],
    selected_indices: np.ndarray,
    variables: list[str],
) -> np.ndarray:
    """
    Validation sample의 quadratic logistic decision score를 batch로 계산한다.
    """

    scores = np.empty(
        len(selected_indices),
        dtype=np.float64,
    )

    scaler = model["scaler"]
    poly = model["poly"]
    classifier = model["classifier"]

    for start in range(
        0,
        len(selected_indices),
        prediction_batch_size,
    ):
        stop = min(
            start + prediction_batch_size,
            len(selected_indices),
        )

        batch_indices = selected_indices[start:stop]

        X_poly_batch = build_polynomial_batch(
            data=data,
            selected_indices=batch_indices,
            variables=variables,
            scaler=scaler,
            poly=poly,
        )

        scores[start:stop] = (
            classifier.decision_function(
                X_poly_batch
            )
        )

        del X_poly_batch

    return scores


def evaluate_region(
    model: dict,
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    signal_indices: np.ndarray,
    background_indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> dict:
    """특정 Region의 validation ROC/AUC를 계산한다."""

    (
        selected_signal_indices,
        n_signal_nonfinite,
    ) = select_region_indices(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        region_name=region_name,
    )

    (
        selected_background_indices,
        n_background_nonfinite,
    ) = select_region_indices(
        data=background_data,
        indices=background_indices,
        variables=variables,
        region_name=region_name,
    )

    n_signal_usable = len(selected_signal_indices)
    n_background_usable = len(selected_background_indices)

    print()
    print(f"Region {region_name} validation input")
    print(f"  signal usable:         {n_signal_usable:,}")
    print(f"  background usable:     {n_background_usable:,}")
    print(f"  signal non-finite:     {n_signal_nonfinite:,}")
    print(f"  background non-finite: {n_background_nonfinite:,}")

    if n_signal_usable == 0:
        raise RuntimeError(
            f"Region {region_name} validation contains no usable signal events."
        )

    if n_background_usable == 0:
        raise RuntimeError(
            f"Region {region_name} validation contains no usable background events."
        )

    scores = np.empty(
        n_signal_usable + n_background_usable,
        dtype=np.float64,
    )

    scores[:n_signal_usable] = predict_region_scores(
        model=model,
        data=signal_data,
        selected_indices=selected_signal_indices,
        variables=variables,
    )

    scores[n_signal_usable:] = predict_region_scores(
        model=model,
        data=background_data,
        selected_indices=selected_background_indices,
        variables=variables,
    )

    del selected_signal_indices
    del selected_background_indices

    signal_scores = scores[:n_signal_usable]
    background_scores = scores[n_signal_usable:]

    y = np.empty(
        len(scores),
        dtype=np.int8,
    )
    y[:n_signal_usable] = 1
    y[n_signal_usable:] = 0

    fpr, tpr, thresholds = roc_curve(
        y,
        scores,
    )
    auc = roc_auc_score(
        y,
        scores,
    )

    del y
    gc.collect()

    return {
        "signal_scores": signal_scores,
        "background_scores": background_scores,
        "scores": scores,
        "tpr": tpr,
        "rejection": 1.0 - fpr,
        "thresholds": thresholds,
        "auc": float(auc),
        "n_signal_nonfinite": n_signal_nonfinite,
        "n_background_nonfinite": n_background_nonfinite,
        "n_signal": n_signal_usable,
        "n_background": n_background_usable,
    }


def _format_number(value: float) -> str:
    return f"{float(value):.{formula_precision}g}"


def make_basf2_formula(
    model: dict,
) -> str:
    """
    StandardScaler와 PolynomialFeatures까지 모두 전개한 basf2 formula를 만든다.

    출력 score는 LogisticRegression.decision_function과 동일한
    w . phi(x) + b 이다. sigmoid는 cut ordering에 필요하지 않다.
    """

    variables = model["variables"]
    weights = model["weights"]
    bias = model["bias"]
    scaler = model["scaler"]
    poly = model["poly"]

    mean = np.asarray(
        scaler.mean_,
        dtype=np.float64,
    )
    scale = np.asarray(
        scaler.scale_,
        dtype=np.float64,
    )

    if len(weights) != len(poly.powers_):
        raise RuntimeError(
            "Polynomial feature/weight length mismatch."
        )

    standardized = [
        (
            f"(({variable}-({_format_number(mean[index])}))"
            f"/({_format_number(scale[index])}))"
        )
        for index, variable in enumerate(variables)
    ]

    terms = [
        f"({_format_number(bias)})"
    ]

    for coefficient, powers in zip(
        weights,
        poly.powers_,
    ):
        factors = []

        for index, power in enumerate(powers):
            if power == 0:
                continue
            if power == 1:
                factors.append(
                    standardized[index]
                )
            elif power == 2:
                factors.append(
                    f"({standardized[index]}^2)"
                )
            else:
                factors.append(
                    f"({standardized[index]}^{int(power)})"
                )

        if not factors:
            continue

        terms.append(
            f"({_format_number(coefficient)})*"
            f"({'*'.join(factors)})"
        )

    return " + ".join(terms)


def print_basf2_alias(
    alias_name: str,
    model: dict,
) -> None:
    """복사해서 basf2 Python steering에 넣을 수 있는 alias를 출력한다."""

    formula = make_basf2_formula(model)

    print(
        'va.variables.addAlias(\n'
        f'    "{alias_name}",\n'
        f'    "formula({formula})"\n'
        ')\n'
    )


# ============================================================
# Threshold 후보 생성
# ============================================================

def make_threshold_candidates(
    result: dict,
    n_scan: int,
) -> np.ndarray:
    """
    Score quantile로 threshold 후보를 만든다.
    """

    scores = np.asarray(
        result["scores"]
    )

    quantiles = np.linspace(
        0.0,
        1.0,
        n_scan,
    )

    thresholds = np.quantile(
        scores,
        quantiles,
    )

    thresholds = np.unique(
        thresholds
    )

    pass_all_threshold = np.nextafter(
        np.min(scores),
        -np.inf,
    )

    reject_all_threshold = np.nextafter(
        np.max(scores),
        np.inf,
    )

    thresholds = np.concatenate([
        [pass_all_threshold],
        thresholds,
        [reject_all_threshold],
    ])

    return np.unique(
        thresholds
    )


# ============================================================
# Region별 threshold 통과 수 사전 계산
# ============================================================

def calculate_threshold_counts(
    result: dict,
    n_scan: int,
) -> dict:
    """
    각 threshold에 대해 통과하는 signal/background 수를
    사전에 계산한다.
    """

    thresholds = make_threshold_candidates(
        result,
        n_scan,
    )

    signal_scores = np.asarray(
        result["signal_scores"]
    )

    background_scores = np.asarray(
        result["background_scores"]
    )

    signal_pass = np.array([
        np.count_nonzero(
            signal_scores > threshold
        )
        for threshold in thresholds
    ])

    background_pass = np.array([
        np.count_nonzero(
            background_scores > threshold
        )
        for threshold in thresholds
    ])

    return {
        "thresholds": thresholds,
        "signal_pass": signal_pass,
        "background_pass": background_pass,
        "n_signal": len(signal_scores),
        "n_background": len(
            background_scores
        ),
    }


# ============================================================
# 여러 Region threshold의 Pareto frontier 계산
# ============================================================

def _region_threshold_options(
    threshold_info: dict,
) -> list[dict]:
    """
    동일한 (signal_pass, background_pass)를 만드는 threshold를
    하나로 압축한다.
    """

    best_by_counts = {}

    for threshold, signal_pass, background_pass in zip(
        threshold_info["thresholds"],
        threshold_info["signal_pass"],
        threshold_info["background_pass"],
    ):
        key = (
            int(signal_pass),
            int(background_pass),
        )

        # 같은 통과 수라면 더 높은 threshold를 보존한다.
        old = best_by_counts.get(key)
        if old is None or threshold > old:
            best_by_counts[key] = float(threshold)

    return [
        {
            "signal": signal_count,
            "background": background_count,
            "threshold": threshold,
        }
        for (
            signal_count,
            background_count,
        ), threshold in best_by_counts.items()
    ]


def _prune_pareto_states(
    states_by_signal: dict[int, dict],
) -> list[dict]:
    """
    더 많은 signal을 살리면서 background도 더 적게 남기는
    다른 state가 존재하면 해당 state를 제거한다.
    """

    ordered = sorted(
        states_by_signal.values(),
        key=lambda state: state["signal"],
        reverse=True,
    )

    frontier = []
    best_background = np.inf

    for state in ordered:
        if state["background"] < best_background:
            frontier.append(state)
            best_background = state["background"]

    return frontier


def build_combined_frontier(
    results: dict[str, dict],
    n_scan: int,
) -> tuple[list[dict], int, int]:
    """
    A/B/C/D threshold 후보를 순차적으로 합치면서
    Pareto-optimal state만 남긴다.

    300^4 전체 조합을 직접 scan하지 않으므로 훨씬 빠르다.
    """

    threshold_data = {
        region_name: calculate_threshold_counts(
            results[region_name],
            n_scan,
        )
        for region_name in region_names
    }

    n_signal_total = sum(
        threshold_data[region_name]["n_signal"]
        for region_name in region_names
    )

    n_background_total = sum(
        threshold_data[region_name]["n_background"]
        for region_name in region_names
    )

    frontier = [
        {
            "signal": 0,
            "background": 0,
            "thresholds": {},
        }
    ]

    for region_name in region_names:
        options = _region_threshold_options(
            threshold_data[region_name]
        )

        states_by_signal = {}

        for state in frontier:
            for option in options:
                signal_count = (
                    state["signal"]
                    + option["signal"]
                )

                background_count = (
                    state["background"]
                    + option["background"]
                )

                old = states_by_signal.get(
                    signal_count
                )

                if (
                    old is None
                    or background_count
                    < old["background"]
                ):
                    thresholds = dict(
                        state["thresholds"]
                    )
                    thresholds[region_name] = (
                        option["threshold"]
                    )

                    states_by_signal[signal_count] = {
                        "signal": signal_count,
                        "background": background_count,
                        "thresholds": thresholds,
                    }

        frontier = _prune_pareto_states(
            states_by_signal
        )

        print(
            f"Threshold frontier after Region "
            f"{region_name}: {len(frontier):,} states"
        )

    return (
        frontier,
        n_signal_total,
        n_background_total,
    )


# ============================================================
# A/B/C/D threshold 동시 최적화
# ============================================================

def optimize_region_thresholds(
    results: dict[str, dict],
    efficiency_target: float,
    n_scan: int,
) -> dict:
    """
    전체 signal efficiency가 목표 이상인 Pareto state 중
    background가 가장 적게 남는 state를 선택한다.
    """

    (
        frontier,
        n_signal_total,
        n_background_total,
    ) = build_combined_frontier(
        results=results,
        n_scan=n_scan,
    )

    minimum_signal = int(
        np.ceil(
            efficiency_target
            * n_signal_total
        )
    )

    valid_states = [
        state
        for state in frontier
        if state["signal"] >= minimum_signal
    ]

    if not valid_states:
        raise RuntimeError(
            "No A/B/C/D threshold combination "
            "satisfies signal efficiency >= "
            f"{efficiency_target:.6f}"
        )

    best_state = min(
        valid_states,
        key=lambda state: (
            state["background"],
            -state["signal"],
        ),
    )

    result = {
        f"threshold_{region_name.lower()}": float(
            best_state["thresholds"][region_name]
        )
        for region_name in region_names
    }

    result.update(
        {
            "signal_efficiency": float(
                best_state["signal"]
                / n_signal_total
            ),
            "background_rejection": float(
                1.0
                - best_state["background"]
                / n_background_total
            ),
            "signal_selected": int(
                best_state["signal"]
            ),
            "signal_total": int(
                n_signal_total
            ),
            "background_selected": int(
                best_state["background"]
            ),
            "background_total": int(
                n_background_total
            ),
        }
    )

    return result


# ============================================================
# Combined ROC 계산
# ============================================================

def calculate_combined_roc(
    results: dict[str, dict],
    n_scan: int,
) -> tuple[np.ndarray, np.ndarray]:
    """
    A/B/C/D threshold 후보의 Pareto frontier에서 combined ROC를
    계산한다.
    """

    (
        frontier,
        n_signal_total,
        n_background_total,
    ) = build_combined_frontier(
        results=results,
        n_scan=n_scan,
    )

    ordered = sorted(
        frontier,
        key=lambda state: state["signal"],
    )

    efficiencies = np.array(
        [
            state["signal"] / n_signal_total
            for state in ordered
        ],
        dtype=np.float64,
    )

    rejections = np.array(
        [
            1.0
            - state["background"]
            / n_background_total
            for state in ordered
        ],
        dtype=np.float64,
    )

    return efficiencies, rejections


# ============================================================
# Region별 working point 성능
# ============================================================

def calculate_region_working_point(
    result: dict,
    threshold: float,
) -> dict:
    """
    선택된 threshold에서 한 Region의 성능을 계산한다.
    """

    signal_scores = np.asarray(
        result["signal_scores"]
    )

    background_scores = np.asarray(
        result["background_scores"]
    )

    n_signal = len(signal_scores)
    n_background = len(
        background_scores
    )

    signal_selected = int(
        np.count_nonzero(
            signal_scores > threshold
        )
    )

    background_selected = int(
        np.count_nonzero(
            background_scores > threshold
        )
    )

    signal_efficiency = (
        signal_selected / n_signal
        if n_signal > 0
        else np.nan
    )

    background_retention = (
        background_selected / n_background
        if n_background > 0
        else np.nan
    )

    return {
        "threshold": float(threshold),
        "signal_selected": (
            signal_selected
        ),
        "signal_total": n_signal,
        "signal_efficiency": float(
            signal_efficiency
        ),
        "background_selected": (
            background_selected
        ),
        "background_total": n_background,
        "background_retention": float(
            background_retention
        ),
        "background_rejection": float(
            1.0 - background_retention
        ),
    }


# ============================================================
# Region A only: B/C/D fixed rejection
# ============================================================

def count_region_events(
    data: dict[str, np.ndarray],
    indices: np.ndarray,
    region_name: str,
) -> int:
    """주어진 index 중 특정 Region에 속하는 event 수를 센다."""

    region_mask = get_region_mask(
        data,
        region_name,
    )

    return int(
        np.count_nonzero(
            region_mask[indices]
        )
    )


def optimize_region_a_only(
    result_a: dict,
    signal_region_counts: dict[str, int],
    background_region_counts: dict[str, int],
    efficiency_target: float,
    n_scan: int,
) -> dict:
    """
    Region A의 threshold만 최적화한다.

    Region B/C/D는 항상 reject한다. 따라서 전체 signal efficiency는
    A에서 살아남은 signal / A+B+C+D 전체 signal 로 정의한다.
    Region A에서 feature가 non-finite라 score를 계산할 수 없는 event도
    reject되므로 denominator에는 포함되고 numerator에는 포함되지 않는다.
    """

    threshold_info = calculate_threshold_counts(
        result=result_a,
        n_scan=n_scan,
    )

    n_signal_total = sum(
        signal_region_counts.values()
    )
    n_background_total = sum(
        background_region_counts.values()
    )

    minimum_signal = int(
        np.ceil(
            efficiency_target
            * n_signal_total
        )
    )

    max_selectable_signal = int(
        threshold_info["n_signal"]
    )

    if minimum_signal > max_selectable_signal:
        max_efficiency = (
            max_selectable_signal
            / n_signal_total
            if n_signal_total > 0
            else 0.0
        )

        raise RuntimeError(
            "Region A alone cannot satisfy the requested overall "
            f"signal efficiency >= {efficiency_target:.6f}. "
            f"Maximum achievable efficiency is {max_efficiency:.6f}."
        )

    candidates = []

    for threshold, signal_pass, background_pass in zip(
        threshold_info["thresholds"],
        threshold_info["signal_pass"],
        threshold_info["background_pass"],
    ):
        signal_pass = int(signal_pass)
        background_pass = int(background_pass)

        if signal_pass < minimum_signal:
            continue

        candidates.append(
            {
                "threshold": float(threshold),
                "signal": signal_pass,
                "background": background_pass,
            }
        )

    if not candidates:
        raise RuntimeError(
            "No Region A threshold satisfies the requested overall "
            f"signal efficiency >= {efficiency_target:.6f}."
        )

    best_state = min(
        candidates,
        key=lambda state: (
            state["background"],
            -state["signal"],
            -state["threshold"],
        ),
    )

    return {
        "threshold_a": float(
            best_state["threshold"]
        ),
        "threshold_b": None,
        "threshold_c": None,
        "threshold_d": None,
        "region_b_action": "reject_all",
        "region_c_action": "reject_all",
        "region_d_action": "reject_all",
        "signal_efficiency": float(
            best_state["signal"]
            / n_signal_total
        ),
        "background_rejection": float(
            1.0
            - best_state["background"]
            / n_background_total
        ),
        "signal_selected": int(
            best_state["signal"]
        ),
        "signal_total": int(
            n_signal_total
        ),
        "background_selected": int(
            best_state["background"]
        ),
        "background_total": int(
            n_background_total
        ),
        "maximum_signal_efficiency_with_a_only": float(
            max_selectable_signal
            / n_signal_total
        ),
    }


def calculate_region_a_only_roc(
    result_a: dict,
    signal_region_counts: dict[str, int],
    background_region_counts: dict[str, int],
    n_scan: int,
) -> tuple[np.ndarray, np.ndarray]:
    """
    B/C/D를 항상 reject한 상태에서 A threshold만 움직인 overall ROC를 만든다.
    """

    threshold_info = calculate_threshold_counts(
        result=result_a,
        n_scan=n_scan,
    )

    n_signal_total = sum(
        signal_region_counts.values()
    )
    n_background_total = sum(
        background_region_counts.values()
    )

    best_background_by_signal = {}

    for signal_pass, background_pass in zip(
        threshold_info["signal_pass"],
        threshold_info["background_pass"],
    ):
        signal_pass = int(signal_pass)
        background_pass = int(background_pass)

        old = best_background_by_signal.get(
            signal_pass
        )

        if old is None or background_pass < old:
            best_background_by_signal[
                signal_pass
            ] = background_pass

    ordered_signal = sorted(
        best_background_by_signal
    )

    efficiencies = np.asarray(
        [
            signal_pass / n_signal_total
            for signal_pass in ordered_signal
        ],
        dtype=np.float64,
    )

    rejections = np.asarray(
        [
            1.0
            - best_background_by_signal[signal_pass]
            / n_background_total
            for signal_pass in ordered_signal
        ],
        dtype=np.float64,
    )

    return efficiencies, rejections


def calculate_region_a_working_point(
    result_a: dict,
    threshold: float,
    n_signal_region_a: int,
    n_background_region_a: int,
) -> dict:
    """
    Region A에서 non-finite feature event도 reject된 것으로 포함해 성능을 계산한다.
    """

    signal_selected = int(
        np.count_nonzero(
            result_a["signal_scores"] > threshold
        )
    )
    background_selected = int(
        np.count_nonzero(
            result_a["background_scores"] > threshold
        )
    )

    signal_efficiency = (
        signal_selected / n_signal_region_a
        if n_signal_region_a > 0
        else np.nan
    )
    background_retention = (
        background_selected / n_background_region_a
        if n_background_region_a > 0
        else np.nan
    )

    return {
        "threshold": float(threshold),
        "signal_selected": signal_selected,
        "signal_total": int(n_signal_region_a),
        "signal_efficiency": float(signal_efficiency),
        "background_selected": background_selected,
        "background_total": int(n_background_region_a),
        "background_retention": float(background_retention),
        "background_rejection": float(
            1.0 - background_retention
        ),
    }


def fixed_reject_working_point(
    n_signal: int,
    n_background: int,
) -> dict:
    """항상 reject하는 Region의 working point."""

    return {
        "threshold": None,
        "action": "reject_all",
        "signal_selected": 0,
        "signal_total": int(n_signal),
        "signal_efficiency": 0.0,
        "background_selected": 0,
        "background_total": int(n_background),
        "background_retention": 0.0,
        "background_rejection": 1.0,
    }


# ============================================================
# 메인
# ============================================================

# ============================================================
# Two-stage cascade helpers
# ============================================================

def score_region_indices(
    model: dict,
    data: dict[str, np.ndarray],
    indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> tuple[np.ndarray, np.ndarray, int]:
    """Region/finite selection 후 원래 index와 decision score를 반환한다."""

    selected_indices, n_nonfinite = select_region_indices(
        data=data,
        indices=indices,
        variables=variables,
        region_name=region_name,
    )

    if len(selected_indices) == 0:
        return (
            selected_indices,
            np.empty(0, dtype=np.float64),
            n_nonfinite,
        )

    scores = predict_region_scores(
        model=model,
        data=data,
        selected_indices=selected_indices,
        variables=variables,
    )

    return selected_indices, scores, n_nonfinite


def choose_loose_threshold(
    signal_scores: np.ndarray,
    n_signal_denominator: int,
    efficiency_target: float,
) -> dict:
    """
    strict 'score > threshold' cut에서 signal efficiency >= target을
    만족시키는 가장 높은 threshold를 선택한다.

    denominator에는 Region A의 non-finite event도 포함한다. 따라서
    non-finite event가 너무 많아 target을 달성할 수 없으면 중단한다.
    """

    signal_scores = np.asarray(signal_scores, dtype=np.float64)

    if n_signal_denominator <= 0:
        raise RuntimeError("Loose-cut signal denominator is zero.")

    minimum_signal = int(
        np.ceil(efficiency_target * n_signal_denominator)
    )

    if minimum_signal > len(signal_scores):
        maximum_efficiency = len(signal_scores) / n_signal_denominator
        raise RuntimeError(
            "The loose stage cannot reach the requested Region A signal "
            f"efficiency >= {efficiency_target:.6f}. "
            f"Maximum achievable efficiency is {maximum_efficiency:.6f}."
        )

    if minimum_signal == 0:
        threshold = np.nextafter(
            np.max(signal_scores),
            np.inf,
        )
    else:
        ordered = np.sort(signal_scores)[::-1]
        boundary_score = ordered[minimum_signal - 1]
        # selection uses score > threshold, so put the threshold just below
        # the boundary score. Ties may make the achieved efficiency slightly
        # larger than the requested target, never smaller.
        threshold = np.nextafter(
            boundary_score,
            -np.inf,
        )

    selected = int(
        np.count_nonzero(signal_scores > threshold)
    )

    return {
        "threshold": float(threshold),
        "signal_selected": selected,
        "signal_total": int(n_signal_denominator),
        "signal_efficiency": float(
            selected / n_signal_denominator
        ),
    }


def build_score_result(
    signal_scores: np.ndarray,
    background_scores: np.ndarray,
    n_signal_nonfinite: int = 0,
    n_background_nonfinite: int = 0,
) -> dict:
    """이미 계산한 signal/background score로 ROC/AUC result를 만든다."""

    signal_scores = np.asarray(signal_scores, dtype=np.float64)
    background_scores = np.asarray(background_scores, dtype=np.float64)

    if len(signal_scores) == 0 or len(background_scores) == 0:
        raise RuntimeError(
            "Cannot build ROC result with an empty signal/background sample."
        )

    scores = np.concatenate([
        signal_scores,
        background_scores,
    ])

    y = np.empty(len(scores), dtype=np.int8)
    y[:len(signal_scores)] = 1
    y[len(signal_scores):] = 0

    fpr, tpr, thresholds = roc_curve(y, scores)
    auc = roc_auc_score(y, scores)

    del y

    return {
        "signal_scores": signal_scores,
        "background_scores": background_scores,
        "scores": scores,
        "tpr": tpr,
        "rejection": 1.0 - fpr,
        "thresholds": thresholds,
        "auc": float(auc),
        "n_signal_nonfinite": int(n_signal_nonfinite),
        "n_background_nonfinite": int(n_background_nonfinite),
        "n_signal": int(len(signal_scores)),
        "n_background": int(len(background_scores)),
    }


def cascade_region_a_working_point(
    result_final_stage: dict,
    threshold_final_stage: float,
    n_signal_region_a: int,
    n_background_region_a: int,
) -> dict:
    """
    앞선 loose cascade cut을 이미 통과한 sample의 final-stage score로
    Region A 최종 working point를 계산한다.

    Denominator는 cascade 적용 전 Region A 전체 event 수이다.
    """

    signal_selected = int(
        np.count_nonzero(
            result_final_stage["signal_scores"]
            > threshold_final_stage
        )
    )
    background_selected = int(
        np.count_nonzero(
            result_final_stage["background_scores"]
            > threshold_final_stage
        )
    )

    signal_efficiency = (
        signal_selected / n_signal_region_a
        if n_signal_region_a > 0
        else np.nan
    )
    background_retention = (
        background_selected / n_background_region_a
        if n_background_region_a > 0
        else np.nan
    )

    return {
        "threshold_final_stage": float(
            threshold_final_stage
        ),
        "signal_selected": signal_selected,
        "signal_total": int(n_signal_region_a),
        "signal_efficiency": float(signal_efficiency),
        "background_selected": background_selected,
        "background_total": int(n_background_region_a),
        "background_retention": float(background_retention),
        "background_rejection": float(
            1.0 - background_retention
        ),
    }


# ============================================================
# 메인: Region A three-stage logistic cascade
# ============================================================

def main() -> None:
    output_directory.mkdir(
        parents=True,
        exist_ok=True,
    )

    # --------------------------------------------------------
    # Signal / background 읽기
    # --------------------------------------------------------

    print("Reading signal...")

    signal_data = read_events_from_directory(
        directory=signal_directory,
        branches=branches_to_read,
        max_events=None,
    )

    n_signal = len(signal_data[strict0_branch])

    max_background_events = (
        background_to_signal_ratio * n_signal
    )

    print()
    print(
        "Reading background "
        f"(maximum {max_background_events:,} events)..."
    )

    background_data = read_events_from_directory(
        directory=background_directory,
        branches=branches_to_read,
        max_events=max_background_events,
    )

    n_background = len(background_data[strict0_branch])

    # --------------------------------------------------------
    # Dataset / Region 요약
    # --------------------------------------------------------

    print()
    print("Dataset summary")
    print(f"  signal:     {n_signal:,}")
    print(f"  background: {n_background:,}")
    print(
        "  background/signal: "
        f"{n_background / n_signal:.3f}"
    )

    signal_region_counts = print_region_population(
        "Signal",
        signal_data,
    )
    background_region_counts = print_region_population(
        "Background",
        background_data,
    )

    for region_name in region_names:
        if signal_region_counts[region_name] == 0:
            raise RuntimeError(
                "Signal dataset contains no "
                f"Region {region_name} events."
            )
        if background_region_counts[region_name] == 0:
            raise RuntimeError(
                "Background dataset contains no "
                f"Region {region_name} events."
            )

    # --------------------------------------------------------
    # Train/validation split
    # --------------------------------------------------------

    (
        signal_train_indices,
        signal_validation_indices,
    ) = split_indices(
        n_events=n_signal,
        validation_fraction=validation_fraction,
        random_seed=random_seed,
    )

    (
        background_train_indices,
        background_validation_indices,
    ) = split_indices(
        n_events=n_background,
        validation_fraction=validation_fraction,
        random_seed=random_seed + 1,
    )

    print()
    print("Train/validation split")
    print(
        f"  signal train:          "
        f"{len(signal_train_indices):,}"
    )
    print(
        f"  signal validation:     "
        f"{len(signal_validation_indices):,}"
    )
    print(
        f"  background train:      "
        f"{len(background_train_indices):,}"
    )
    print(
        f"  background validation: "
        f"{len(background_validation_indices):,}"
    )

    train_signal_region_counts = {
        region_name: count_region_events(
            data=signal_data,
            indices=signal_train_indices,
            region_name=region_name,
        )
        for region_name in region_names
    }
    train_background_region_counts = {
        region_name: count_region_events(
            data=background_data,
            indices=background_train_indices,
            region_name=region_name,
        )
        for region_name in region_names
    }

    validation_signal_region_counts = {
        region_name: count_region_events(
            data=signal_data,
            indices=signal_validation_indices,
            region_name=region_name,
        )
        for region_name in region_names
    }
    validation_background_region_counts = {
        region_name: count_region_events(
            data=background_data,
            indices=background_validation_indices,
            region_name=region_name,
        )
        for region_name in region_names
    }

    print()
    print("Training policy")
    print(
        "  Region A stage 1: loose quadratic logistic regression "
        f"(target A efficiency >= {stage1_efficiency_target_a:.4f})"
    )
    print(
        "  Region A stage 2: retrain on Stage-1 survivors, then "
        "apply another loose cut "
        f"(conditional efficiency >= "
        f"{stage2_conditional_efficiency_target:.4f})"
    )
    print(
        "  Region A stage 3: retrain on Stage-2 survivors and "
        "choose the final cut for overall signal efficiency >= "
        f"{efficiency_target:.4f}"
    )
    print("  Region B/C/D: reject all")

    # ========================================================
    # Stage 1: first loose logistic regression
    # ========================================================

    model_stage1 = train_region_logistic(
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=signal_train_indices,
        background_indices=background_train_indices,
        variables=variables_a,
        region_name="A",
    )
    gc.collect()

    (
        train_signal_a_indices,
        train_signal_stage1_scores,
        train_signal_stage1_nonfinite,
    ) = score_region_indices(
        model=model_stage1,
        data=signal_data,
        indices=signal_train_indices,
        variables=variables_a,
        region_name="A",
    )

    (
        train_background_a_indices,
        train_background_stage1_scores,
        train_background_stage1_nonfinite,
    ) = score_region_indices(
        model=model_stage1,
        data=background_data,
        indices=background_train_indices,
        variables=variables_a,
        region_name="A",
    )

    stage1_cut = choose_loose_threshold(
        signal_scores=train_signal_stage1_scores,
        n_signal_denominator=train_signal_region_counts["A"],
        efficiency_target=stage1_efficiency_target_a,
    )
    stage1_threshold = stage1_cut["threshold"]

    train_signal_stage1_pass_mask = (
        train_signal_stage1_scores > stage1_threshold
    )
    train_background_stage1_pass_mask = (
        train_background_stage1_scores > stage1_threshold
    )

    stage2_signal_train_indices = (
        train_signal_a_indices[
            train_signal_stage1_pass_mask
        ]
    )
    stage2_background_train_indices = (
        train_background_a_indices[
            train_background_stage1_pass_mask
        ]
    )

    print()
    print(
        "=== STAGE 1 LOOSE CUT "
        "(determined on training sample) ==="
    )
    print(
        f"Stage 1 threshold: "
        f"{stage1_threshold:.12g}"
    )
    print(
        "Training Region A signal efficiency: "
        f"{len(stage2_signal_train_indices) / train_signal_region_counts['A']:.6f} "
        f"({len(stage2_signal_train_indices):,}/"
        f"{train_signal_region_counts['A']:,})"
    )
    print(
        "Training Region A background retention: "
        f"{len(stage2_background_train_indices) / train_background_region_counts['A']:.6f} "
        f"({len(stage2_background_train_indices):,}/"
        f"{train_background_region_counts['A']:,})"
    )

    if len(stage2_signal_train_indices) == 0:
        raise RuntimeError(
            "Stage 1 rejects all training signal."
        )
    if len(stage2_background_train_indices) == 0:
        raise RuntimeError(
            "Stage 1 rejects all training background."
        )

    # ========================================================
    # Stage 2: train on Stage-1 survivors
    # ========================================================

    model_stage2 = train_region_logistic(
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=stage2_signal_train_indices,
        background_indices=stage2_background_train_indices,
        variables=variables_a,
        region_name="A",
    )
    gc.collect()

    # Stage 2 loose threshold is determined on the Stage-1
    # survivor training sample. Its efficiency is conditional on
    # passing Stage 1.
    (
        train_signal_stage2_indices,
        train_signal_stage2_scores,
        train_signal_stage2_nonfinite,
    ) = score_region_indices(
        model=model_stage2,
        data=signal_data,
        indices=stage2_signal_train_indices,
        variables=variables_a,
        region_name="A",
    )

    (
        train_background_stage2_indices,
        train_background_stage2_scores,
        train_background_stage2_nonfinite,
    ) = score_region_indices(
        model=model_stage2,
        data=background_data,
        indices=stage2_background_train_indices,
        variables=variables_a,
        region_name="A",
    )

    stage2_cut = choose_loose_threshold(
        signal_scores=train_signal_stage2_scores,
        n_signal_denominator=len(
            stage2_signal_train_indices
        ),
        efficiency_target=(
            stage2_conditional_efficiency_target
        ),
    )
    stage2_threshold = stage2_cut["threshold"]

    train_signal_stage2_pass_mask = (
        train_signal_stage2_scores > stage2_threshold
    )
    train_background_stage2_pass_mask = (
        train_background_stage2_scores > stage2_threshold
    )

    stage3_signal_train_indices = (
        train_signal_stage2_indices[
            train_signal_stage2_pass_mask
        ]
    )
    stage3_background_train_indices = (
        train_background_stage2_indices[
            train_background_stage2_pass_mask
        ]
    )

    stage2_train_conditional_signal_eff = (
        len(stage3_signal_train_indices)
        / len(stage2_signal_train_indices)
    )
    stage2_train_conditional_background_retention = (
        len(stage3_background_train_indices)
        / len(stage2_background_train_indices)
    )
    stage2_train_cumulative_signal_eff_a = (
        len(stage3_signal_train_indices)
        / train_signal_region_counts["A"]
    )
    stage2_train_cumulative_background_retention_a = (
        len(stage3_background_train_indices)
        / train_background_region_counts["A"]
    )

    print()
    print(
        "=== STAGE 2 LOOSE CUT "
        "(determined on Stage-1 survivor training sample) ==="
    )
    print(
        f"Stage 2 threshold: "
        f"{stage2_threshold:.12g}"
    )
    print(
        "Training conditional signal efficiency: "
        f"{stage2_train_conditional_signal_eff:.6f} "
        f"({len(stage3_signal_train_indices):,}/"
        f"{len(stage2_signal_train_indices):,})"
    )
    print(
        "Training conditional background retention: "
        f"{stage2_train_conditional_background_retention:.6f} "
        f"({len(stage3_background_train_indices):,}/"
        f"{len(stage2_background_train_indices):,})"
    )
    print(
        "Training cumulative Region A signal efficiency "
        "(Stage 1 + Stage 2): "
        f"{stage2_train_cumulative_signal_eff_a:.6f}"
    )
    print(
        "Training cumulative Region A background retention "
        "(Stage 1 + Stage 2): "
        f"{stage2_train_cumulative_background_retention_a:.6f}"
    )

    if len(stage3_signal_train_indices) == 0:
        raise RuntimeError(
            "Stage 2 rejects all training signal."
        )
    if len(stage3_background_train_indices) == 0:
        raise RuntimeError(
            "Stage 2 rejects all training background."
        )

    # ========================================================
    # Stage 3: final logistic regression on Stage-2 survivors
    # ========================================================

    model_stage3 = train_region_logistic(
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=stage3_signal_train_indices,
        background_indices=stage3_background_train_indices,
        variables=variables_a,
        region_name="A",
    )
    gc.collect()

    # ========================================================
    # Validation: Stage 1 fixed -> Stage 2 fixed -> Stage 3
    # ========================================================

    (
        validation_signal_a_indices,
        validation_signal_stage1_scores,
        validation_signal_stage1_nonfinite,
    ) = score_region_indices(
        model=model_stage1,
        data=signal_data,
        indices=signal_validation_indices,
        variables=variables_a,
        region_name="A",
    )

    (
        validation_background_a_indices,
        validation_background_stage1_scores,
        validation_background_stage1_nonfinite,
    ) = score_region_indices(
        model=model_stage1,
        data=background_data,
        indices=background_validation_indices,
        variables=variables_a,
        region_name="A",
    )

    result_stage1 = build_score_result(
        signal_scores=validation_signal_stage1_scores,
        background_scores=validation_background_stage1_scores,
        n_signal_nonfinite=(
            validation_signal_stage1_nonfinite
        ),
        n_background_nonfinite=(
            validation_background_stage1_nonfinite
        ),
    )

    validation_signal_stage1_pass_mask = (
        validation_signal_stage1_scores
        > stage1_threshold
    )
    validation_background_stage1_pass_mask = (
        validation_background_stage1_scores
        > stage1_threshold
    )

    stage2_signal_validation_indices = (
        validation_signal_a_indices[
            validation_signal_stage1_pass_mask
        ]
    )
    stage2_background_validation_indices = (
        validation_background_a_indices[
            validation_background_stage1_pass_mask
        ]
    )

    n_validation_signal_total = sum(
        validation_signal_region_counts.values()
    )
    n_validation_background_total = sum(
        validation_background_region_counts.values()
    )

    stage1_signal_eff_a = (
        len(stage2_signal_validation_indices)
        / validation_signal_region_counts["A"]
    )
    stage1_background_retention_a = (
        len(stage2_background_validation_indices)
        / validation_background_region_counts["A"]
    )
    stage1_overall_signal_eff = (
        len(stage2_signal_validation_indices)
        / n_validation_signal_total
    )
    stage1_overall_background_retention = (
        len(stage2_background_validation_indices)
        / n_validation_background_total
    )

    print()
    print("=== STAGE 1 VALIDATION PERFORMANCE ===")
    print(
        f"Region A AUC: "
        f"{result_stage1['auc']:.6f}"
    )
    print(
        "Region A signal efficiency: "
        f"{stage1_signal_eff_a:.6f} "
        f"({len(stage2_signal_validation_indices):,}/"
        f"{validation_signal_region_counts['A']:,})"
    )
    print(
        "Region A background retention: "
        f"{stage1_background_retention_a:.6f} "
        f"({len(stage2_background_validation_indices):,}/"
        f"{validation_background_region_counts['A']:,})"
    )
    print(
        "Overall signal efficiency after Stage 1 "
        "(B/C/D rejected): "
        f"{stage1_overall_signal_eff:.6f}"
    )
    print(
        "Overall background retention after Stage 1 "
        "(B/C/D rejected): "
        f"{stage1_overall_background_retention:.6f}"
    )

    # Stage 2 validation scores are evaluated only for Stage-1
    # survivors.
    (
        validation_signal_stage2_indices,
        validation_signal_stage2_scores,
        validation_signal_stage2_nonfinite,
    ) = score_region_indices(
        model=model_stage2,
        data=signal_data,
        indices=stage2_signal_validation_indices,
        variables=variables_a,
        region_name="A",
    )

    (
        validation_background_stage2_indices,
        validation_background_stage2_scores,
        validation_background_stage2_nonfinite,
    ) = score_region_indices(
        model=model_stage2,
        data=background_data,
        indices=stage2_background_validation_indices,
        variables=variables_a,
        region_name="A",
    )

    result_stage2 = build_score_result(
        signal_scores=validation_signal_stage2_scores,
        background_scores=validation_background_stage2_scores,
        n_signal_nonfinite=(
            validation_signal_stage2_nonfinite
        ),
        n_background_nonfinite=(
            validation_background_stage2_nonfinite
        ),
    )

    validation_signal_stage2_pass_mask = (
        validation_signal_stage2_scores
        > stage2_threshold
    )
    validation_background_stage2_pass_mask = (
        validation_background_stage2_scores
        > stage2_threshold
    )

    stage3_signal_validation_indices = (
        validation_signal_stage2_indices[
            validation_signal_stage2_pass_mask
        ]
    )
    stage3_background_validation_indices = (
        validation_background_stage2_indices[
            validation_background_stage2_pass_mask
        ]
    )

    stage2_conditional_signal_eff = (
        len(stage3_signal_validation_indices)
        / len(stage2_signal_validation_indices)
    )
    stage2_conditional_background_retention = (
        len(stage3_background_validation_indices)
        / len(stage2_background_validation_indices)
    )
    stage2_cumulative_signal_eff_a = (
        len(stage3_signal_validation_indices)
        / validation_signal_region_counts["A"]
    )
    stage2_cumulative_background_retention_a = (
        len(stage3_background_validation_indices)
        / validation_background_region_counts["A"]
    )
    stage2_overall_signal_eff = (
        len(stage3_signal_validation_indices)
        / n_validation_signal_total
    )
    stage2_overall_background_retention = (
        len(stage3_background_validation_indices)
        / n_validation_background_total
    )

    print()
    print("=== STAGE 2 VALIDATION PERFORMANCE ===")
    print(
        "Conditional AUC on Stage-1 survivors: "
        f"{result_stage2['auc']:.6f}"
    )
    print(
        "Conditional signal efficiency: "
        f"{stage2_conditional_signal_eff:.6f} "
        f"({len(stage3_signal_validation_indices):,}/"
        f"{len(stage2_signal_validation_indices):,})"
    )
    print(
        "Conditional background retention: "
        f"{stage2_conditional_background_retention:.6f} "
        f"({len(stage3_background_validation_indices):,}/"
        f"{len(stage2_background_validation_indices):,})"
    )
    print(
        "Cumulative Region A signal efficiency "
        "(Stage 1 + Stage 2): "
        f"{stage2_cumulative_signal_eff_a:.6f}"
    )
    print(
        "Cumulative Region A background retention "
        "(Stage 1 + Stage 2): "
        f"{stage2_cumulative_background_retention_a:.6f}"
    )
    print(
        "Overall signal efficiency after Stage 2 "
        "(B/C/D rejected): "
        f"{stage2_overall_signal_eff:.6f}"
    )
    print(
        "Overall background retention after Stage 2 "
        "(B/C/D rejected): "
        f"{stage2_overall_background_retention:.6f}"
    )

    if stage2_overall_signal_eff < efficiency_target:
        raise RuntimeError(
            "After the fixed Stage-1 and Stage-2 loose cuts, "
            "the maximum possible overall signal efficiency is "
            f"only {stage2_overall_signal_eff:.6f}, below the "
            f"requested final target "
            f"{efficiency_target:.6f}."
        )

    required_stage3_conditional_eff = (
        efficiency_target
        / stage2_overall_signal_eff
    )

    print(
        "Required Stage-3 conditional signal efficiency "
        "(approximately): "
        f"{required_stage3_conditional_eff:.6f}"
    )

    # Stage 3 is evaluated only on events surviving both loose
    # stages.
    result_stage3 = evaluate_region(
        model=model_stage3,
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=stage3_signal_validation_indices,
        background_indices=stage3_background_validation_indices,
        variables=variables_a,
        region_name="A",
    )
    gc.collect()

    print()
    print("Stage 3 conditional validation AUC")
    print(
        "  Region A Stage-1+2 survivors: "
        f"{result_stage3['auc']:.6f}"
    )

    # Final Stage-3 threshold is optimized on validation using
    # the original overall-efficiency denominator A+B+C+D.
    best = optimize_region_a_only(
        result_a=result_stage3,
        signal_region_counts=(
            validation_signal_region_counts
        ),
        background_region_counts=(
            validation_background_region_counts
        ),
        efficiency_target=efficiency_target,
        n_scan=n_threshold_scan,
    )

    stage3_threshold = best["threshold_a"]

    print()
    print(
        "=== FINAL THREE-STAGE CASCADE CUT "
        f"@ overall efficiency >= "
        f"{efficiency_target:.4f} ==="
    )
    print(
        f"Stage 1 loose threshold: "
        f"{stage1_threshold:.12g}"
    )
    print(
        f"Stage 2 loose threshold: "
        f"{stage2_threshold:.12g}"
    )
    print(
        f"Stage 3 final threshold: "
        f"{stage3_threshold:.12g}"
    )
    print("Region B/C/D: REJECT ALL")
    print(
        "Overall signal efficiency:   "
        f"{best['signal_efficiency']:.6f} "
        f"({best['signal_selected']:,}/"
        f"{best['signal_total']:,})"
    )
    print(
        "Overall background rejection: "
        f"{best['background_rejection']:.6f}"
    )
    print(
        "Overall background retention: "
        f"{1.0 - best['background_rejection']:.6f} "
        f"({best['background_selected']:,}/"
        f"{best['background_total']:,})"
    )

    # --------------------------------------------------------
    # Region working points
    # --------------------------------------------------------

    region_working_points = {
        "A": cascade_region_a_working_point(
            result_final_stage=result_stage3,
            threshold_final_stage=stage3_threshold,
            n_signal_region_a=(
                validation_signal_region_counts["A"]
            ),
            n_background_region_a=(
                validation_background_region_counts["A"]
            ),
        ),
        "B": fixed_reject_working_point(
            n_signal=validation_signal_region_counts["B"],
            n_background=(
                validation_background_region_counts["B"]
            ),
        ),
        "C": fixed_reject_working_point(
            n_signal=validation_signal_region_counts["C"],
            n_background=(
                validation_background_region_counts["C"]
            ),
        ),
        "D": fixed_reject_working_point(
            n_signal=validation_signal_region_counts["D"],
            n_background=(
                validation_background_region_counts["D"]
            ),
        ),
    }

    # --------------------------------------------------------
    # ROC plot
    # --------------------------------------------------------

    (
        cascade_efficiency,
        cascade_rejection,
    ) = calculate_region_a_only_roc(
        result_a=result_stage3,
        signal_region_counts=(
            validation_signal_region_counts
        ),
        background_region_counts=(
            validation_background_region_counts
        ),
        n_scan=n_threshold_scan,
    )

    figure, axis = plt.subplots(
        figsize=(7, 6)
    )

    axis.plot(
        result_stage1["tpr"],
        result_stage1["rejection"],
        label=(
            "Stage 1, Region A "
            f"(AUC={result_stage1['auc']:.4f})"
        ),
    )
    axis.plot(
        result_stage2["tpr"],
        result_stage2["rejection"],
        label=(
            "Stage 2 conditional "
            f"(AUC={result_stage2['auc']:.4f})"
        ),
    )
    axis.plot(
        result_stage3["tpr"],
        result_stage3["rejection"],
        label=(
            "Stage 3 conditional "
            f"(AUC={result_stage3['auc']:.4f})"
        ),
    )
    axis.plot(
        cascade_efficiency,
        cascade_rejection,
        linewidth=3,
        label=(
            "Overall cascade: Stage1 fixed + "
            "Stage2 fixed + Stage3 scan"
        ),
    )
    axis.scatter(
        [best["signal_efficiency"]],
        [best["background_rejection"]],
        marker="o",
        s=70,
        label=(
            "Final working point "
            f"(eff={best['signal_efficiency']:.4f})"
        ),
    )

    axis.set_xlabel("Signal efficiency")
    axis.set_ylabel("Background rejection")
    axis.set_xlim(0.0, 1.01)
    axis.set_ylim(0.0, 1.01)
    axis.grid(
        True,
        linestyle="--",
        alpha=0.6,
    )
    axis.legend()
    figure.tight_layout()
    figure.savefig(
        roc_plot_path,
        dpi=150,
        bbox_inches="tight",
    )
    plt.close(figure)

    # --------------------------------------------------------
    # JSON 결과 저장
    # --------------------------------------------------------

    output_result = {
        "tree_name": tree_name,
        "signal_directory": str(
            signal_directory.resolve()
        ),
        "background_directory": str(
            background_directory.resolve()
        ),
        "n_signal": int(n_signal),
        "n_background": int(n_background),
        "background_to_signal_ratio": (
            background_to_signal_ratio
        ),
        "validation_fraction": validation_fraction,
        "random_seed": random_seed,
        "stage1_efficiency_target_region_a": (
            stage1_efficiency_target_a
        ),
        "stage2_conditional_efficiency_target": (
            stage2_conditional_efficiency_target
        ),
        "final_overall_efficiency_target": (
            efficiency_target
        ),
        "n_threshold_scan": n_threshold_scan,
        "logistic_degree": logistic_degree,
        "logistic_C": logistic_C,
        "logistic_solver": logistic_solver,
        "logistic_max_iter": logistic_max_iter,
        "logistic_tol": logistic_tol,
        "signal_region_counts": signal_region_counts,
        "background_region_counts": (
            background_region_counts
        ),
        "validation_signal_region_counts": (
            validation_signal_region_counts
        ),
        "validation_background_region_counts": (
            validation_background_region_counts
        ),
        "stage1": {
            "action": (
                "loose_quadratic_logistic_regression"
            ),
            "variables": variables_a,
            "basf2_alias": (
                "LR_score_A_stage1_quad"
            ),
            "threshold": float(stage1_threshold),
            "training_iterations": int(
                model_stage1["n_iter"]
            ),
            "validation_auc_region_a": float(
                result_stage1["auc"]
            ),
            "validation_region_a_signal_efficiency": (
                float(stage1_signal_eff_a)
            ),
            "validation_region_a_background_retention": (
                float(stage1_background_retention_a)
            ),
            "validation_overall_signal_efficiency": (
                float(stage1_overall_signal_eff)
            ),
            "validation_overall_background_retention": (
                float(
                    stage1_overall_background_retention
                )
            ),
        },
        "stage2": {
            "action": (
                "loose_quadratic_logistic_regression_"
                "on_stage1_survivors"
            ),
            "variables": variables_a,
            "basf2_alias": (
                "LR_score_A_stage2_quad"
            ),
            "threshold": float(stage2_threshold),
            "training_iterations": int(
                model_stage2["n_iter"]
            ),
            "validation_auc_conditional": float(
                result_stage2["auc"]
            ),
            "validation_conditional_signal_efficiency": (
                float(stage2_conditional_signal_eff)
            ),
            "validation_conditional_background_retention": (
                float(
                    stage2_conditional_background_retention
                )
            ),
            "validation_cumulative_region_a_signal_efficiency": (
                float(stage2_cumulative_signal_eff_a)
            ),
            "validation_overall_signal_efficiency": (
                float(stage2_overall_signal_eff)
            ),
            "validation_overall_background_retention": (
                float(
                    stage2_overall_background_retention
                )
            ),
        },
        "stage3": {
            "action": (
                "final_quadratic_logistic_regression_"
                "on_stage2_survivors"
            ),
            "variables": variables_a,
            "basf2_alias": (
                "LR_score_A_stage3_quad"
            ),
            "training_iterations": int(
                model_stage3["n_iter"]
            ),
            "validation_auc_conditional": float(
                result_stage3["auc"]
            ),
            "threshold": float(stage3_threshold),
            "required_conditional_signal_efficiency_approx": (
                float(required_stage3_conditional_eff)
            ),
        },
        "regions": {
            "A": {
                "description": (
                    region_configs["A"]["description"]
                ),
                "action": (
                    "three_stage_quadratic_"
                    "logistic_cascade"
                ),
                "working_point": (
                    region_working_points["A"]
                ),
            },
            "B": {
                "description": (
                    region_configs["B"]["description"]
                ),
                "action": "reject_all",
                "working_point": (
                    region_working_points["B"]
                ),
            },
            "C": {
                "description": (
                    region_configs["C"]["description"]
                ),
                "action": "reject_all",
                "working_point": (
                    region_working_points["C"]
                ),
            },
            "D": {
                "description": (
                    region_configs["D"]["description"]
                ),
                "action": "reject_all",
                "working_point": (
                    region_working_points["D"]
                ),
            },
        },
        "combined_working_point": best,
    }

    with result_json_path.open(
        "w",
        encoding="utf-8",
    ) as output_file:
        json.dump(
            output_result,
            output_file,
            indent=2,
            ensure_ascii=False,
        )

    # --------------------------------------------------------
    # basf2 수식 출력
    # --------------------------------------------------------

    print()
    print("=== basf2 quadratic logistic aliases ===")
    print(
        "# Region A three-stage cascade. "
        "Regions B/C/D are rejected."
    )

    print(
        f"# Stage 1 loose threshold: "
        f"{stage1_threshold:.16g}"
    )
    print_basf2_alias(
        alias_name="LR_score_A_stage1_quad",
        model=model_stage1,
    )

    print(
        f"# Stage 2 loose threshold: "
        f"{stage2_threshold:.16g}"
    )
    print_basf2_alias(
        alias_name="LR_score_A_stage2_quad",
        model=model_stage2,
    )

    print(
        f"# Stage 3 final threshold: "
        f"{stage3_threshold:.16g}"
    )
    print_basf2_alias(
        alias_name="LR_score_A_stage3_quad",
        model=model_stage3,
    )

    print("# Final event selection")
    print("# basf2 cut expression:")
    print(
        f'# "{strict2_branch} > 0.5 and '
        f'LR_score_A_stage1_quad > '
        f'{stage1_threshold:.16g} and '
        f'LR_score_A_stage2_quad > '
        f'{stage2_threshold:.16g} and '
        f'LR_score_A_stage3_quad > '
        f'{stage3_threshold:.16g}"'
    )

    print()
    print("Saved files")
    print(
        f"  Thresholds: "
        f"{result_json_path}"
    )
    print(
        f"  ROC plot:   "
        f"{roc_plot_path}"
    )


if __name__ == "__main__":
    main()
