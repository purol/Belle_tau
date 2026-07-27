#!/usr/bin/env python3

from __future__ import annotations

import json
from pathlib import Path

import joblib
import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np
import uproot

from sklearn.ensemble import HistGradientBoostingClassifier
from sklearn.inspection import permutation_importance
from sklearn.metrics import roc_auc_score, roc_curve
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
    "thrust",
    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "pt_sum_gencut",
    "sphericity",
#    "aplanarity",
#    "Ntrack_gencut",
    "cosTBz",
    "foxWolframR1",
#    "foxWolframR2",
#    "foxWolframR3",
    "harmonicMomentThrust0",
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
    "averageValueInList__botau__pl__clfake_strict2__cm__spdeltaE__bc",
#    "extraInfo__bostd_M2__bc",
#    "extraInfo__bostd_deltaE2__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE2__cmdaughterLowest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM2__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM2__cmdaughterLowest__bop__bc__bc",
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
    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "pt_sum_gencut",
    "sphericity",
    "aplanarity",
    "Ntrack_gencut",
    "cosTBz",
    "foxWolframR1",
    "foxWolframR2",
#    "foxWolframR3",
    "harmonicMomentThrust0",
#    "harmonicMomentThrust1",
    "R2",
    "genMissingEnergyOfEventCMS",
    "nParticlesInList__botau__pl__clfake_strict0__bc",
#    "nParticlesInList__botau__pl__clfake_strict1__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cm__spM__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cm__spMbc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cm__spM__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM1__cm__spMbc__bc",
#    "averageValueInList__botau__pl__clfake_strict1__cm__spM__bc",
    "averageValueInList__botau__pl__clfake_strict1__cm__spdeltaE__bc",
#    "extraInfo__bostd_M1__bc",
#    "extraInfo__bostd_deltaE1__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE1__cmdaughterLowest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM1__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM1__cmdaughterLowest__bop__bc__bc",
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
    "foxWolframR1",
#    "foxWolframR2",
#    "foxWolframR3",
    "harmonicMomentThrust0",
#    "harmonicMomentThrust1",
    "R2",
    "genMissingEnergyOfEventCMS",
    "nParticlesInList__botau__pl__clfake_strict0__bc",
#    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cm__spM__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cm__spMbc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cm__spM__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cm__spdeltaE__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cm__spMbc__bc",
    "averageValueInList__botau__pl__clfake_strict0__cm__spM__bc",
    "averageValueInList__botau__pl__clfake_strict0__cm__spdeltaE__bc",
    "extraInfo__bostd_M0__bc",
    "extraInfo__bostd_deltaE0__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_deltaE0__cmdaughterLowest__bop__bc__bc",
#    "sumValueInList__botau__pl__clBCS_strict_dM0__cmdaughterHighest__bop__bc__bc",
    "sumValueInList__botau__pl__clBCS_strict_dM0__cmdaughterLowest__bop__bc__bc",
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


# ------------------------------------------------------------
# Dataset 및 학습 설정
# ------------------------------------------------------------

# Background는 signal event 수의 최대 몇 배까지 읽을지
background_to_signal_ratio = 150

# Train/validation 분리 비율
validation_fraction = 0.25

random_seed = 42

# 목표 signal efficiency
efficiency_target = 0.3

# 각 Region의 threshold 후보 개수
n_threshold_scan = 300

# Permutation importance 설정
importance_n_repeats = 5
importance_max_events_per_class = 50_000

# ROOT 파일을 한 번에 읽을 entry 수
step_size = 100_000


# ------------------------------------------------------------
# 출력 경로
# ------------------------------------------------------------

output_directory = Path("./bdt_output")

model_paths = {
    region_name: (
        output_directory
        / f"bdt_region_{region_name.lower()}.joblib"
    )
    for region_name in region_names
}

result_json_path = (
    output_directory / "bdt_thresholds.json"
)

roc_plot_path = (
    output_directory / "roc_curve_validation.png"
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


all_feature_variables = unique_preserving_order(
    variables_a
    + variables_b
    + variables_c
    + variables_d
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

    max_events가 주어지면 읽은 ROOT entry가 해당 수에
    도달하는 즉시 중지한다.
    """

    root_files = find_root_files(
        directory
    )

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
                    chunk_arrays = {}

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

                        chunk_arrays[branch] = values

                    lengths = {
                        len(values)
                        for values
                        in chunk_arrays.values()
                    }

                    if len(lengths) != 1:
                        raise RuntimeError(
                            "Branches have different "
                            f"lengths in {file_path}: "
                            f"{lengths}"
                        )

                    n_chunk = next(
                        iter(lengths)
                    )

                    if n_chunk == 0:
                        continue

                    selected_indices = np.arange(
                        n_chunk,
                        dtype=np.int64,
                    )

                    if max_events is not None:
                        remaining = (
                            max_events
                            - n_collected
                        )

                        if remaining <= 0:
                            break

                        selected_indices = (
                            selected_indices[
                                :remaining
                            ]
                        )

                    if len(selected_indices) == 0:
                        continue

                    for branch in branches:
                        collected[branch].append(
                            chunk_arrays[branch][
                                selected_indices
                            ].astype(
                                np.float64,
                                copy=False,
                            )
                        )

                    n_collected += len(
                        selected_indices
                    )

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
        f"from {directory}"
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

def select_region_matrix(
    data: dict[str, np.ndarray],
    indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> tuple[np.ndarray, np.ndarray, int]:
    """
    지정한 index 중 특정 Region event를 선택한다.

    Region 선택 후, 해당 Region에서 사용하는 변수에 대해서만
    finite cut을 적용한다.

    반환:
        X
        usable event의 원래 index
        non-finite feature 때문에 제거된 event 수
    """

    full_region_mask = get_region_mask(
        data,
        region_name,
    )

    selected_indices = indices[
        full_region_mask[indices]
    ]

    if len(selected_indices) == 0:
        empty_matrix = np.empty(
            (0, len(variables)),
            dtype=np.float64,
        )

        return (
            empty_matrix,
            selected_indices,
            0,
        )

    X = np.column_stack([
        data[variable][selected_indices]
        for variable in variables
    ])

    finite_feature_mask = np.all(
        np.isfinite(X),
        axis=1,
    )

    n_removed_nonfinite = int(
        np.count_nonzero(
            ~finite_feature_mask
        )
    )

    return (
        X[finite_feature_mask],
        selected_indices[
            finite_feature_mask
        ],
        n_removed_nonfinite,
    )


# ============================================================
# BDT 생성
# ============================================================

def make_bdt(
    region_name: str,
) -> HistGradientBoostingClassifier:
    """
    Region별 BDT를 생성한다.
    """

    region_seed_offset = {
        "A": 0,
        "B": 1,
        "C": 2,
        "D": 3,
    }[region_name]

    return HistGradientBoostingClassifier(
        learning_rate=0.05,
        max_iter=1000,
        max_leaf_nodes=31,
        max_depth=None,
        min_samples_leaf=20,
        l2_regularization=1.0,
        early_stopping=True,
        validation_fraction=0.15,
        n_iter_no_change=20,
        random_state=(
            random_seed
            + region_seed_offset
        ),
    )


# ============================================================
# Region별 BDT 학습
# ============================================================

def train_region_bdt(
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    signal_indices: np.ndarray,
    background_indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> HistGradientBoostingClassifier:
    """
    특정 Region의 BDT를 학습한다.
    """

    (
        X_signal,
        selected_signal_indices,
        n_signal_nonfinite,
    ) = select_region_matrix(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        region_name=region_name,
    )

    (
        X_background,
        selected_background_indices,
        n_background_nonfinite,
    ) = select_region_matrix(
        data=background_data,
        indices=background_indices,
        variables=variables,
        region_name=region_name,
    )

    print()
    print(
        f"Region {region_name} "
        "training input"
    )

    print(
        f"  signal usable:         "
        f"{len(X_signal):,}"
    )

    print(
        f"  background usable:     "
        f"{len(X_background):,}"
    )

    print(
        f"  signal non-finite:     "
        f"{n_signal_nonfinite:,}"
    )

    print(
        f"  background non-finite: "
        f"{n_background_nonfinite:,}"
    )

    if len(X_signal) == 0:
        raise RuntimeError(
            f"Region {region_name} contains "
            "no usable signal training events."
        )

    if len(X_background) == 0:
        raise RuntimeError(
            f"Region {region_name} contains "
            "no usable background training events."
        )

    X = np.vstack([
        X_signal,
        X_background,
    ])

    y = np.concatenate([
        np.ones(
            len(X_signal),
            dtype=np.int8,
        ),
        np.zeros(
            len(X_background),
            dtype=np.int8,
        ),
    ])

    # Signal과 background의 total training weight가
    # 각각 동일해지도록 설정
    signal_weight = (
        len(y)
        / (2.0 * len(X_signal))
    )

    background_weight = (
        len(y)
        / (2.0 * len(X_background))
    )

    sample_weight = np.concatenate([
        np.full(
            len(X_signal),
            signal_weight,
            dtype=np.float64,
        ),
        np.full(
            len(X_background),
            background_weight,
            dtype=np.float64,
        ),
    ])

    classifier = make_bdt(
        region_name
    )

    classifier.fit(
        X,
        y,
        sample_weight=sample_weight,
    )

    print()
    print(
        f"Region {region_name} "
        "training result"
    )

    print(
        f"  signal:     "
        f"{len(X_signal):,}"
    )

    print(
        f"  background: "
        f"{len(X_background):,}"
    )

    print(
        f"  features:   "
        f"{len(variables):,}"
    )

    print(
        f"  iterations: "
        f"{classifier.n_iter_:,}"
    )

    return classifier


# ============================================================
# Validation 평가
# ============================================================

def evaluate_region(
    classifier: HistGradientBoostingClassifier,
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    signal_indices: np.ndarray,
    background_indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> dict:
    """
    특정 Region의 validation 성능을 계산한다.
    """

    (
        X_signal,
        selected_signal_indices,
        n_signal_nonfinite,
    ) = select_region_matrix(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        region_name=region_name,
    )

    (
        X_background,
        selected_background_indices,
        n_background_nonfinite,
    ) = select_region_matrix(
        data=background_data,
        indices=background_indices,
        variables=variables,
        region_name=region_name,
    )

    print()
    print(
        f"Region {region_name} "
        "validation input"
    )

    print(
        f"  signal usable:         "
        f"{len(X_signal):,}"
    )

    print(
        f"  background usable:     "
        f"{len(X_background):,}"
    )

    print(
        f"  signal non-finite:     "
        f"{n_signal_nonfinite:,}"
    )

    print(
        f"  background non-finite: "
        f"{n_background_nonfinite:,}"
    )

    if len(X_signal) == 0:
        raise RuntimeError(
            f"Region {region_name} validation "
            "contains no usable signal events."
        )

    if len(X_background) == 0:
        raise RuntimeError(
            f"Region {region_name} validation "
            "contains no usable background events."
        )

    signal_scores = classifier.predict_proba(
        X_signal
    )[:, 1]

    background_scores = (
        classifier.predict_proba(
            X_background
        )[:, 1]
    )

    y = np.concatenate([
        np.ones(
            len(signal_scores),
            dtype=np.int8,
        ),
        np.zeros(
            len(background_scores),
            dtype=np.int8,
        ),
    ])

    scores = np.concatenate([
        signal_scores,
        background_scores,
    ])

    fpr, tpr, thresholds = roc_curve(
        y,
        scores,
    )

    auc = roc_auc_score(
        y,
        scores,
    )

    return {
        "signal_scores": signal_scores,
        "background_scores": background_scores,
        "scores": scores,
        "tpr": tpr,
        "rejection": 1.0 - fpr,
        "thresholds": thresholds,
        "auc": float(auc),
        "n_signal_nonfinite": (
            n_signal_nonfinite
        ),
        "n_background_nonfinite": (
            n_background_nonfinite
        ),
        "n_signal": len(signal_scores),
        "n_background": len(
            background_scores
        ),
    }


# ============================================================
# Region별 permutation importance
# ============================================================

def print_region_variable_importance(
    classifier: HistGradientBoostingClassifier,
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    signal_indices: np.ndarray,
    background_indices: np.ndarray,
    variables: list[str],
    region_name: str,
) -> None:
    """
    Validation sample에서 Region별 permutation importance를
    ROC AUC 감소량으로 계산하여 출력한다.

    계산 시간을 제한하기 위해 signal과 background에서 각각
    최대 importance_max_events_per_class개를 사용한다.
    """

    (
        X_signal,
        _,
        _,
    ) = select_region_matrix(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        region_name=region_name,
    )

    (
        X_background,
        _,
        _,
    ) = select_region_matrix(
        data=background_data,
        indices=background_indices,
        variables=variables,
        region_name=region_name,
    )

    region_seed_offset = {
        "A": 0,
        "B": 1,
        "C": 2,
        "D": 3,
    }[region_name]

    importance_seed = (
        random_seed
        + 1000
        + region_seed_offset
    )

    rng = np.random.default_rng(
        importance_seed
    )

    if (
        len(X_signal)
        > importance_max_events_per_class
    ):
        signal_choice = rng.choice(
            len(X_signal),
            size=importance_max_events_per_class,
            replace=False,
        )
        X_signal = X_signal[signal_choice]

    if (
        len(X_background)
        > importance_max_events_per_class
    ):
        background_choice = rng.choice(
            len(X_background),
            size=importance_max_events_per_class,
            replace=False,
        )
        X_background = X_background[
            background_choice
        ]

    X = np.vstack([
        X_signal,
        X_background,
    ])

    y = np.concatenate([
        np.ones(
            len(X_signal),
            dtype=np.int8,
        ),
        np.zeros(
            len(X_background),
            dtype=np.int8,
        ),
    ])

    importance = permutation_importance(
        estimator=classifier,
        X=X,
        y=y,
        scoring="roc_auc",
        n_repeats=importance_n_repeats,
        random_state=importance_seed,
        n_jobs=1,
    )

    order = np.argsort(
        importance.importances_mean
    )[::-1]

    print()
    print(
        f"Region {region_name} "
        "permutation importance"
    )
    print(
        "  importance = validation ROC AUC decrease"
    )
    print(
        f"  sample: signal={len(X_signal):,}, "
        f"background={len(X_background):,}"
    )

    for rank, index in enumerate(
        order,
        start=1,
    ):
        print(
            f"  {rank:2d}. "
            f"{variables[index]}: "
            f"{importance.importances_mean[index]:.8f} "
            f"+/- "
            f"{importance.importances_std[index]:.8f}"
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
# 메인
# ============================================================

def main() -> None:
    output_directory.mkdir(
        parents=True,
        exist_ok=True,
    )

    # --------------------------------------------------------
    # Signal 읽기
    # --------------------------------------------------------

    print("Reading signal...")

    signal_data = read_events_from_directory(
        directory=signal_directory,
        branches=branches_to_read,
        max_events=None,
    )

    n_signal = len(
        signal_data[strict0_branch]
    )

    # --------------------------------------------------------
    # Background 읽기
    # --------------------------------------------------------

    max_background_events = (
        background_to_signal_ratio
        * n_signal
    )

    print()
    print(
        "Reading background "
        f"(maximum "
        f"{max_background_events:,} events)..."
    )

    background_data = (
        read_events_from_directory(
            directory=background_directory,
            branches=branches_to_read,
            max_events=max_background_events,
        )
    )

    n_background = len(
        background_data[strict0_branch]
    )

    # --------------------------------------------------------
    # Dataset 요약
    # --------------------------------------------------------

    print()
    print("Dataset summary")

    print(
        f"  signal:     "
        f"{n_signal:,}"
    )

    print(
        f"  background: "
        f"{n_background:,}"
    )

    print(
        "  background/signal: "
        f"{n_background / n_signal:.3f}"
    )

    signal_region_counts = (
        print_region_population(
            "Signal",
            signal_data,
        )
    )

    background_region_counts = (
        print_region_population(
            "Background",
            background_data,
        )
    )

    for region_name in region_names:
        if (
            signal_region_counts[
                region_name
            ]
            == 0
        ):
            raise RuntimeError(
                "Signal dataset contains no "
                f"Region {region_name} events."
            )

        if (
            background_region_counts[
                region_name
            ]
            == 0
        ):
            raise RuntimeError(
                "Background dataset contains no "
                f"Region {region_name} events."
            )

    # --------------------------------------------------------
    # Train/validation 분리
    # --------------------------------------------------------

    (
        signal_train_indices,
        signal_validation_indices,
    ) = split_indices(
        n_events=n_signal,
        validation_fraction=(
            validation_fraction
        ),
        random_seed=random_seed,
    )

    (
        background_train_indices,
        background_validation_indices,
    ) = split_indices(
        n_events=n_background,
        validation_fraction=(
            validation_fraction
        ),
        random_seed=random_seed + 1,
    )

    print()
    print("Train/validation split")

    print(
        f"  signal train:       "
        f"{len(signal_train_indices):,}"
    )

    print(
        f"  signal validation:  "
        f"{len(signal_validation_indices):,}"
    )

    print(
        f"  background train:   "
        f"{len(background_train_indices):,}"
    )

    print(
        f"  background validation: "
        f"{len(background_validation_indices):,}"
    )

    # --------------------------------------------------------
    # Region A/B/C/D 모델 학습
    # --------------------------------------------------------

    classifiers = {}

    for region_name in region_names:
        variables = region_configs[
            region_name
        ]["variables"]

        classifiers[region_name] = (
            train_region_bdt(
                signal_data=signal_data,
                background_data=background_data,
                signal_indices=(
                    signal_train_indices
                ),
                background_indices=(
                    background_train_indices
                ),
                variables=variables,
                region_name=region_name,
            )
        )

    # --------------------------------------------------------
    # Validation 평가
    # --------------------------------------------------------

    results = {}

    for region_name in region_names:
        variables = region_configs[
            region_name
        ]["variables"]

        results[region_name] = (
            evaluate_region(
                classifier=classifiers[
                    region_name
                ],
                signal_data=signal_data,
                background_data=background_data,
                signal_indices=(
                    signal_validation_indices
                ),
                background_indices=(
                    background_validation_indices
                ),
                variables=variables,
                region_name=region_name,
            )
        )

    print()
    print("Validation AUC")

    for region_name in region_names:
        print(
            f"  Region {region_name}: "
            f"{results[region_name]['auc']:.6f}"
        )

    # --------------------------------------------------------
    # Region별 variable importance 출력
    # --------------------------------------------------------

    for region_name in region_names:
        variables = region_configs[
            region_name
        ]["variables"]

        print_region_variable_importance(
            classifier=classifiers[
                region_name
            ],
            signal_data=signal_data,
            background_data=background_data,
            signal_indices=(
                signal_validation_indices
            ),
            background_indices=(
                background_validation_indices
            ),
            variables=variables,
            region_name=region_name,
        )

    # --------------------------------------------------------
    # A/B/C/D threshold 동시 최적화
    # --------------------------------------------------------

    best = optimize_region_thresholds(
        results=results,
        efficiency_target=(
            efficiency_target
        ),
        n_scan=n_threshold_scan,
    )

    print()
    print(
        "=== OPTIMAL VALIDATION CUTS "
        f"@ efficiency >= "
        f"{efficiency_target:.4f} ==="
    )

    for region_name in region_names:
        threshold_key = (
            f"threshold_"
            f"{region_name.lower()}"
        )

        print(
            f"Region {region_name} threshold: "
            f"{best[threshold_key]:.8f}"
        )

    print(
        "Signal efficiency:   "
        f"{best['signal_efficiency']:.6f} "
        f"({best['signal_selected']:,}/"
        f"{best['signal_total']:,})"
    )

    print(
        "Background rejection: "
        f"{best['background_rejection']:.6f}"
    )

    print(
        "Background retention: "
        f"{1.0 - best['background_rejection']:.6f} "
        f"({best['background_selected']:,}/"
        f"{best['background_total']:,})"
    )

    # --------------------------------------------------------
    # Region별 working point 성능
    # --------------------------------------------------------

    region_working_points = {}

    print()
    print("Region working points")

    for region_name in region_names:
        threshold_key = (
            f"threshold_"
            f"{region_name.lower()}"
        )

        working_point = (
            calculate_region_working_point(
                result=results[region_name],
                threshold=best[
                    threshold_key
                ],
            )
        )

        region_working_points[
            region_name
        ] = working_point

        print()
        print(f"Region {region_name}")

        print(
            "  signal efficiency:    "
            f"{working_point['signal_efficiency']:.6f} "
            f"({working_point['signal_selected']:,}/"
            f"{working_point['signal_total']:,})"
        )

        print(
            "  background retention: "
            f"{working_point['background_retention']:.6f} "
            f"({working_point['background_selected']:,}/"
            f"{working_point['background_total']:,})"
        )

        print(
            "  background rejection: "
            f"{working_point['background_rejection']:.6f}"
        )

    # --------------------------------------------------------
    # ROC plot
    # --------------------------------------------------------

    (
        combined_efficiency,
        combined_rejection,
    ) = calculate_combined_roc(
        results=results,
        n_scan=n_threshold_scan,
    )

    figure, axis = plt.subplots(
        figsize=(7, 6)
    )

    for region_name in region_names:
        result = results[region_name]

        axis.plot(
            result["tpr"],
            result["rejection"],
            label=(
                f"Region {region_name} "
                f"(AUC={result['auc']:.4f})"
            ),
        )

    axis.plot(
        combined_efficiency,
        combined_rejection,
        linewidth=3,
        label="Combined A ⊕ B ⊕ C ⊕ D",
    )

    axis.scatter(
        [best["signal_efficiency"]],
        [best["background_rejection"]],
        marker="o",
        s=70,
        label=(
            "Working point "
            f"(ε={best['signal_efficiency']:.4f})"
        ),
    )

    axis.set_xlabel(
        "Signal efficiency"
    )

    axis.set_ylabel(
        "Background rejection"
    )

    axis.set_xlim(
        0.0,
        1.01,
    )

    axis.set_ylim(
        0.0,
        1.01,
    )

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
    # 모델 저장
    # --------------------------------------------------------

    for region_name in region_names:
        joblib.dump(
            {
                "model": classifiers[
                    region_name
                ],
                "variables": region_configs[
                    region_name
                ]["variables"],
                "strict0_branch": (
                    strict0_branch
                ),
                "strict1_branch": (
                    strict1_branch
                ),
                "strict2_branch": (
                    strict2_branch
                ),
                "region_name": region_name,
                "region_condition": (
                    region_configs[
                        region_name
                    ]["description"]
                ),
            },
            model_paths[region_name],
        )

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
        "n_background": int(
            n_background
        ),
        "background_to_signal_ratio": (
            background_to_signal_ratio
        ),
        "validation_fraction": (
            validation_fraction
        ),
        "random_seed": random_seed,
        "efficiency_target": (
            efficiency_target
        ),
        "n_threshold_scan": (
            n_threshold_scan
        ),
        "strict0_branch": (
            strict0_branch
        ),
        "strict1_branch": (
            strict1_branch
        ),
        "strict2_branch": (
            strict2_branch
        ),
        "signal_region_counts": (
            signal_region_counts
        ),
        "background_region_counts": (
            background_region_counts
        ),
        "regions": {},
        "combined_working_point": best,
    }

    for region_name in region_names:
        output_result["regions"][
            region_name
        ] = {
            "description": (
                region_configs[
                    region_name
                ]["description"]
            ),
            "variables": (
                region_configs[
                    region_name
                ]["variables"]
            ),
            "validation_auc": float(
                results[
                    region_name
                ]["auc"]
            ),
            "validation_signal": int(
                results[
                    region_name
                ]["n_signal"]
            ),
            "validation_background": int(
                results[
                    region_name
                ]["n_background"]
            ),
            "validation_signal_nonfinite": int(
                results[
                    region_name
                ][
                    "n_signal_nonfinite"
                ]
            ),
            "validation_background_nonfinite": int(
                results[
                    region_name
                ][
                    "n_background_nonfinite"
                ]
            ),
            "working_point": (
                region_working_points[
                    region_name
                ]
            ),
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

    print()
    print("Saved files")

    for region_name in region_names:
        print(
            f"  Region {region_name} model: "
            f"{model_paths[region_name]}"
        )

    print(
        f"  Thresholds:     "
        f"{result_json_path}"
    )

    print(
        f"  ROC plot:       "
        f"{roc_plot_path}"
    )


if __name__ == "__main__":
    main()
