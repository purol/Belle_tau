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
from sklearn.metrics import roc_auc_score, roc_curve
from sklearn.model_selection import train_test_split


# ============================================================
# 설정
# ============================================================

signal_directory = Path("./signal")
background_directory = Path("./background")

tree_name = "gen_info"

# Region A: tau+:fake candidate가 존재
# Region B: tau+:fake candidate가 존재하지 않음
region_branch = (
    "nParticlesInList__botau__pl__clfake__bc"
)


# Region A에서 사용할 변수
variables_a = [
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
    "nParticlesInList__botau__pl__clfake__bc",
    "sumValueInList__botau__pl__clBCS_deltaE__cm__spM__bc",
    "sumValueInList__botau__pl__clBCS_deltaE__cm__spdeltaE__bc",
    "sumValueInList__botau__pl__clBCS_dM__cm__spM__bc",
    "sumValueInList__botau__pl__clBCS_dM__cm__spdeltaE__bc",
    "nParticlesInList__botau__pl__clfake_strict0__bc",
    "nParticlesInList__botau__pl__clfake_strict1__bc",
]


# Region B에서 사용할 변수
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
    "foxWolframR3",
    "harmonicMomentThrust0",
    "harmonicMomentThrust1",
    "R2",
]


# Background는 signal event 수의 최대 몇 배까지 읽을지
background_to_signal_ratio = 10


# Train/validation 분리 비율
validation_fraction = 0.25
random_seed = 42


# 목표 signal efficiency
efficiency_target = 0.985


# Region A/B threshold 동시 scan 개수
n_threshold_scan = 300


# ROOT 파일을 한 번에 읽을 entry 수
step_size = 100_000


# 출력 경로
output_directory = Path("./bdt_output")

model_a_path = (
    output_directory / "bdt_region_a.joblib"
)

model_b_path = (
    output_directory / "bdt_region_b.joblib"
)

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
    variables_a + variables_b
)

branches_to_read = unique_preserving_order(
    all_feature_variables + [region_branch]
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
    디렉터리 아래 ROOT 파일들을 재귀적으로 읽는다.

    중요:
    여기서는 모든 branch가 finite인지 검사하지 않는다.

    Region B에서는 Region A 전용 BCS 변수가 NaN일 수 있으므로,
    읽기 단계에서 finite cut을 걸면 Region B event가 모두
    제거될 수 있다.

    max_events가 주어지면 읽은 ROOT entry 수가 max_events에
    도달하는 즉시 중지한다.
    """

    root_files = find_root_files(directory)

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
                        for values in chunk_arrays.values()
                    }

                    if len(lengths) != 1:
                        raise RuntimeError(
                            "Branches have different lengths "
                            f"in {file_path}: {lengths}"
                        )

                    n_chunk = next(iter(lengths))

                    if n_chunk == 0:
                        continue

                    # 여기서는 모든 event를 보존한다.
                    selected_indices = np.arange(
                        n_chunk,
                        dtype=np.int64,
                    )

                    if max_events is not None:
                        remaining = (
                            max_events - n_collected
                        )

                        if remaining <= 0:
                            break

                        selected_indices = (
                            selected_indices[:remaining]
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
                "No values were collected for branch: "
                f"{branch}"
            )

        output[branch] = np.concatenate(
            arrays
        )

    # 모든 branch 길이가 동일한지 최종 검사
    output_lengths = {
        len(values)
        for values in output.values()
    }

    if len(output_lengths) != 1:
        raise RuntimeError(
            "Collected branches have inconsistent "
            f"lengths: {output_lengths}"
        )

    print(
        f"Collected {n_collected:,} events "
        f"from {directory}"
    )

    return output


# ============================================================
# Region population 출력
# ============================================================

def print_region_population(
    label: str,
    data: dict[str, np.ndarray],
) -> None:
    """
    Region branch 값에 따른 event 개수를 출력한다.
    """

    values = data[region_branch]

    finite_mask = np.isfinite(values)

    region_a = (
        finite_mask
        & (values > 0.5)
    )

    region_b = (
        finite_mask
        & (values <= 0.5)
    )

    n_total = len(values)
    n_region_a = np.count_nonzero(region_a)
    n_region_b = np.count_nonzero(region_b)
    n_nonfinite = np.count_nonzero(
        ~finite_mask
    )

    print()
    print(f"{label} region population")
    print(f"  total:              {n_total:,}")
    print(
        f"  Region A (> 0.5):   "
        f"{n_region_a:,}"
    )
    print(
        f"  Region B (<= 0.5):  "
        f"{n_region_b:,}"
    )
    print(
        f"  non-finite region:  "
        f"{n_nonfinite:,}"
    )

    if n_total > 0:
        print(
            "  Region A fraction: "
            f"{n_region_a / n_total:.6f}"
        )
        print(
            "  Region B fraction: "
            f"{n_region_b / n_total:.6f}"
        )


# ============================================================
# Train/validation 분리
# ============================================================

def split_indices(
    n_events: int,
    validation_fraction: float,
    random_seed: int,
) -> tuple[np.ndarray, np.ndarray]:
    """
    event index를 train과 validation으로 무작위 분리한다.
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
        np.asarray(train_indices),
        np.asarray(validation_indices),
    )


# ============================================================
# Region별 matrix 생성
# ============================================================

def select_region_matrix(
    data: dict[str, np.ndarray],
    indices: np.ndarray,
    variables: list[str],
    use_region_a: bool,
) -> tuple[np.ndarray, np.ndarray, int]:
    """
    지정된 index 중 Region A 또는 B event를 선택한다.

    그 후 해당 Region에서 사용하는 변수에 대해서만
    finite cut을 적용한다.

    반환값:
        X
        finite cut까지 통과한 원래 event index
        non-finite 변수 때문에 제외된 event 수
    """

    region_values = data[region_branch][
        indices
    ]

    finite_region = np.isfinite(
        region_values
    )

    if use_region_a:
        region_mask = (
            finite_region
            & (region_values > 0.5)
        )
    else:
        region_mask = (
            finite_region
            & (region_values <= 0.5)
        )

    selected_indices = indices[
        region_mask
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

    n_removed_nonfinite = np.count_nonzero(
        ~finite_feature_mask
    )

    return (
        X[finite_feature_mask],
        selected_indices[finite_feature_mask],
        n_removed_nonfinite,
    )


# ============================================================
# BDT 생성
# ============================================================

def make_bdt() -> HistGradientBoostingClassifier:
    """
    sklearn histogram-based gradient boosting classifier를
    생성한다.
    """

    return HistGradientBoostingClassifier(
        learning_rate=0.05,
        max_iter=300,
        max_leaf_nodes=31,
        max_depth=None,
        min_samples_leaf=20,
        l2_regularization=1.0,
        early_stopping=True,
        validation_fraction=0.15,
        n_iter_no_change=20,
        random_state=random_seed,
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
    use_region_a: bool,
) -> HistGradientBoostingClassifier:
    """
    Region A 또는 Region B의 BDT를 학습한다.
    """

    region_name = (
        "A" if use_region_a else "B"
    )

    (
        X_signal,
        selected_signal_indices,
        n_signal_nonfinite,
    ) = select_region_matrix(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        use_region_a=use_region_a,
    )

    (
        X_background,
        selected_background_indices,
        n_background_nonfinite,
    ) = select_region_matrix(
        data=background_data,
        indices=background_indices,
        variables=variables,
        use_region_a=use_region_a,
    )

    print()
    print(f"Region {region_name} training input")
    print(
        f"  signal usable:       "
        f"{len(X_signal):,}"
    )
    print(
        f"  background usable:   "
        f"{len(X_background):,}"
    )
    print(
        f"  signal non-finite:   "
        f"{n_signal_nonfinite:,}"
    )
    print(
        f"  background non-finite: "
        f"{n_background_nonfinite:,}"
    )

    if len(X_signal) == 0:
        raise RuntimeError(
            f"Region {region_name} contains no "
            "usable signal training events."
        )

    if len(X_background) == 0:
        raise RuntimeError(
            f"Region {region_name} contains no "
            "usable background training events."
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

    # signal/background의 total weight가 같아지도록 설정
    signal_weight = (
        len(y) / (2.0 * len(X_signal))
    )

    background_weight = (
        len(y) / (2.0 * len(X_background))
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

    classifier = make_bdt()

    classifier.fit(
        X,
        y,
        sample_weight=sample_weight,
    )

    print()
    print(f"Region {region_name} training result")
    print(f"  signal:     {len(X_signal):,}")
    print(
        f"  background: "
        f"{len(X_background):,}"
    )
    print(
        f"  features:   {len(variables):,}"
    )
    print(
        f"  iterations: "
        f"{classifier.n_iter_:,}"
    )

    return classifier


# ============================================================
# Validation score 계산
# ============================================================

def evaluate_region(
    classifier: HistGradientBoostingClassifier,
    signal_data: dict[str, np.ndarray],
    background_data: dict[str, np.ndarray],
    signal_indices: np.ndarray,
    background_indices: np.ndarray,
    variables: list[str],
    use_region_a: bool,
) -> dict[str, np.ndarray | float | int]:
    """
    Region A 또는 B의 validation sample에서 성능을 계산한다.
    """

    region_name = (
        "A" if use_region_a else "B"
    )

    (
        X_signal,
        selected_signal_indices,
        n_signal_nonfinite,
    ) = select_region_matrix(
        data=signal_data,
        indices=signal_indices,
        variables=variables,
        use_region_a=use_region_a,
    )

    (
        X_background,
        selected_background_indices,
        n_background_nonfinite,
    ) = select_region_matrix(
        data=background_data,
        indices=background_indices,
        variables=variables,
        use_region_a=use_region_a,
    )

    print()
    print(f"Region {region_name} validation input")
    print(
        f"  signal usable:       "
        f"{len(X_signal):,}"
    )
    print(
        f"  background usable:   "
        f"{len(X_background):,}"
    )
    print(
        f"  signal non-finite:   "
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

    background_scores = classifier.predict_proba(
        X_background
    )[:, 1]

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
        "y": y,
        "scores": scores,
        "tpr": tpr,
        "rejection": 1.0 - fpr,
        "thresholds": thresholds,
        "auc": float(auc),
        "n_signal_nonfinite": int(
            n_signal_nonfinite
        ),
        "n_background_nonfinite": int(
            n_background_nonfinite
        ),
    }


# ============================================================
# Threshold 후보 생성
# ============================================================

def make_threshold_candidates(
    result: dict,
    n_scan: int,
) -> np.ndarray:
    """
    BDT score의 quantile을 이용해 threshold 후보를 만든다.
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

    # 모든 event를 통과시키는 threshold
    pass_all_threshold = np.nextafter(
        np.min(scores),
        -np.inf,
    )

    # 모든 event를 탈락시키는 threshold
    reject_all_threshold = np.nextafter(
        np.max(scores),
        np.inf,
    )

    thresholds = np.concatenate([
        [pass_all_threshold],
        thresholds,
        [reject_all_threshold],
    ])

    return np.unique(thresholds)


# ============================================================
# A/B threshold 동시 최적화
# ============================================================

def optimize_combined_thresholds(
    result_a: dict,
    result_b: dict,
    efficiency_target: float,
    n_scan: int,
) -> dict[str, float | int]:
    """
    Validation sample에서 Region A와 B의 threshold를
    동시에 scan한다.

    signal efficiency가 목표 이상인 조합 중
    background rejection이 가장 큰 조합을 선택한다.
    """

    thresholds_a = make_threshold_candidates(
        result_a,
        n_scan,
    )

    thresholds_b = make_threshold_candidates(
        result_b,
        n_scan,
    )

    signal_scores_a = np.asarray(
        result_a["signal_scores"]
    )

    background_scores_a = np.asarray(
        result_a["background_scores"]
    )

    signal_scores_b = np.asarray(
        result_b["signal_scores"]
    )

    background_scores_b = np.asarray(
        result_b["background_scores"]
    )

    n_signal_total = (
        len(signal_scores_a)
        + len(signal_scores_b)
    )

    n_background_total = (
        len(background_scores_a)
        + len(background_scores_b)
    )

    if n_signal_total == 0:
        raise RuntimeError(
            "Validation set contains no signal."
        )

    if n_background_total == 0:
        raise RuntimeError(
            "Validation set contains no background."
        )

    signal_pass_a = np.array([
        np.count_nonzero(
            signal_scores_a > threshold
        )
        for threshold in thresholds_a
    ])

    background_pass_a = np.array([
        np.count_nonzero(
            background_scores_a > threshold
        )
        for threshold in thresholds_a
    ])

    signal_pass_b = np.array([
        np.count_nonzero(
            signal_scores_b > threshold
        )
        for threshold in thresholds_b
    ])

    background_pass_b = np.array([
        np.count_nonzero(
            background_scores_b > threshold
        )
        for threshold in thresholds_b
    ])

    best = None

    for index_a, threshold_a in enumerate(
        thresholds_a
    ):
        combined_signal = (
            signal_pass_a[index_a]
            + signal_pass_b
        )

        signal_efficiencies = (
            combined_signal / n_signal_total
        )

        valid_b_mask = (
            signal_efficiencies
            >= efficiency_target
        )

        if not np.any(valid_b_mask):
            continue

        combined_background = (
            background_pass_a[index_a]
            + background_pass_b
        )

        background_rejections = (
            1.0
            - combined_background
            / n_background_total
        )

        valid_b_indices = np.flatnonzero(
            valid_b_mask
        )

        best_local_index = valid_b_indices[
            np.argmax(
                background_rejections[
                    valid_b_mask
                ]
            )
        ]

        candidate = {
            "threshold_a": float(
                threshold_a
            ),
            "threshold_b": float(
                thresholds_b[best_local_index]
            ),
            "signal_efficiency": float(
                signal_efficiencies[
                    best_local_index
                ]
            ),
            "background_rejection": float(
                background_rejections[
                    best_local_index
                ]
            ),
            "signal_selected": int(
                combined_signal[
                    best_local_index
                ]
            ),
            "signal_total": int(
                n_signal_total
            ),
            "background_selected": int(
                combined_background[
                    best_local_index
                ]
            ),
            "background_total": int(
                n_background_total
            ),
        }

        if best is None:
            best = candidate
            continue

        if (
            candidate["background_rejection"]
            > best["background_rejection"]
        ):
            best = candidate

        elif (
            np.isclose(
                candidate["background_rejection"],
                best["background_rejection"],
            )
            and candidate["signal_efficiency"]
            > best["signal_efficiency"]
        ):
            best = candidate

    if best is None:
        raise RuntimeError(
            "No threshold pair satisfies the "
            "requested signal efficiency "
            f"{efficiency_target:.6f}."
        )

    return best


# ============================================================
# Combined validation ROC
# ============================================================

def calculate_combined_roc(
    result_a: dict,
    result_b: dict,
    n_scan: int,
) -> tuple[np.ndarray, np.ndarray]:
    """
    Region A/B threshold를 독립적으로 scan하여 combined ROC의
    envelope를 계산한다.
    """

    thresholds_a = make_threshold_candidates(
        result_a,
        n_scan,
    )

    thresholds_b = make_threshold_candidates(
        result_b,
        n_scan,
    )

    signal_a = np.asarray(
        result_a["signal_scores"]
    )

    background_a = np.asarray(
        result_a["background_scores"]
    )

    signal_b = np.asarray(
        result_b["signal_scores"]
    )

    background_b = np.asarray(
        result_b["background_scores"]
    )

    n_signal = (
        len(signal_a)
        + len(signal_b)
    )

    n_background = (
        len(background_a)
        + len(background_b)
    )

    if n_signal == 0 or n_background == 0:
        raise RuntimeError(
            "Cannot calculate combined ROC with "
            "empty signal or background."
        )

    signal_pass_b = np.array([
        np.count_nonzero(
            signal_b > threshold
        )
        for threshold in thresholds_b
    ])

    background_pass_b = np.array([
        np.count_nonzero(
            background_b > threshold
        )
        for threshold in thresholds_b
    ])

    best_rejection_by_signal_count = {}

    for threshold_a in thresholds_a:
        signal_pass_a = np.count_nonzero(
            signal_a > threshold_a
        )

        background_pass_a = np.count_nonzero(
            background_a > threshold_a
        )

        combined_signal = (
            signal_pass_a
            + signal_pass_b
        )

        combined_background = (
            background_pass_a
            + background_pass_b
        )

        combined_rejection = (
            1.0
            - combined_background
            / n_background
        )

        for signal_count, rejection in zip(
            combined_signal,
            combined_rejection,
        ):
            signal_count = int(signal_count)

            old_rejection = (
                best_rejection_by_signal_count.get(
                    signal_count,
                    -np.inf,
                )
            )

            if rejection > old_rejection:
                best_rejection_by_signal_count[
                    signal_count
                ] = float(rejection)

    signal_counts = np.array(
        sorted(
            best_rejection_by_signal_count
        ),
        dtype=np.int64,
    )

    efficiencies = (
        signal_counts / n_signal
    )

    rejections = np.array([
        best_rejection_by_signal_count[
            int(signal_count)
        ]
        for signal_count in signal_counts
    ])

    # ROC envelope를 단조롭게 정리
    rejections = np.maximum.accumulate(
        rejections[::-1]
    )[::-1]

    return efficiencies, rejections


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
        signal_data[region_branch]
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
        f"(maximum {max_background_events:,} events)..."
    )

    background_data = read_events_from_directory(
        directory=background_directory,
        branches=branches_to_read,
        max_events=max_background_events,
    )

    n_background = len(
        background_data[region_branch]
    )

    # --------------------------------------------------------
    # Dataset 요약
    # --------------------------------------------------------

    print()
    print("Dataset summary")
    print(f"  signal:     {n_signal:,}")
    print(
        f"  background: "
        f"{n_background:,}"
    )
    print(
        "  background/signal: "
        f"{n_background / n_signal:.3f}"
    )

    print_region_population(
        "Signal",
        signal_data,
    )

    print_region_population(
        "Background",
        background_data,
    )

    # 전체 dataset에 A/B가 모두 존재하는지 확인
    signal_region_values = signal_data[
        region_branch
    ]

    background_region_values = background_data[
        region_branch
    ]

    n_signal_a = np.count_nonzero(
        np.isfinite(signal_region_values)
        & (signal_region_values > 0.5)
    )

    n_signal_b = np.count_nonzero(
        np.isfinite(signal_region_values)
        & (signal_region_values <= 0.5)
    )

    n_background_a = np.count_nonzero(
        np.isfinite(background_region_values)
        & (background_region_values > 0.5)
    )

    n_background_b = np.count_nonzero(
        np.isfinite(background_region_values)
        & (background_region_values <= 0.5)
    )

    if n_signal_a == 0:
        raise RuntimeError(
            "Signal dataset contains no Region A events."
        )

    if n_signal_b == 0:
        raise RuntimeError(
            "Signal dataset contains no Region B events."
        )

    if n_background_a == 0:
        raise RuntimeError(
            "Background dataset contains no "
            "Region A events."
        )

    if n_background_b == 0:
        raise RuntimeError(
            "Background dataset contains no "
            "Region B events."
        )

    # --------------------------------------------------------
    # Train/validation 분리
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
    # Region A/B BDT 학습
    # --------------------------------------------------------

    classifier_a = train_region_bdt(
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=signal_train_indices,
        background_indices=background_train_indices,
        variables=variables_a,
        use_region_a=True,
    )

    classifier_b = train_region_bdt(
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=signal_train_indices,
        background_indices=background_train_indices,
        variables=variables_b,
        use_region_a=False,
    )

    # --------------------------------------------------------
    # Validation 평가
    # --------------------------------------------------------

    result_a = evaluate_region(
        classifier=classifier_a,
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=signal_validation_indices,
        background_indices=background_validation_indices,
        variables=variables_a,
        use_region_a=True,
    )

    result_b = evaluate_region(
        classifier=classifier_b,
        signal_data=signal_data,
        background_data=background_data,
        signal_indices=signal_validation_indices,
        background_indices=background_validation_indices,
        variables=variables_b,
        use_region_a=False,
    )

    print()
    print("Validation AUC")
    print(
        f"  Region A: "
        f"{result_a['auc']:.6f}"
    )
    print(
        f"  Region B: "
        f"{result_b['auc']:.6f}"
    )

    # --------------------------------------------------------
    # A/B threshold 동시 최적화
    # --------------------------------------------------------

    best = optimize_combined_thresholds(
        result_a=result_a,
        result_b=result_b,
        efficiency_target=efficiency_target,
        n_scan=n_threshold_scan,
    )

    print()
    print(
        "=== OPTIMAL VALIDATION CUTS "
        f"@ efficiency >= "
        f"{efficiency_target:.4f} ==="
    )

    print(
        "Region A threshold: "
        f"{best['threshold_a']:.8f}"
    )

    print(
        "Region B threshold: "
        f"{best['threshold_b']:.8f}"
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
    # ROC plot
    # --------------------------------------------------------

    (
        combined_efficiency,
        combined_rejection,
    ) = calculate_combined_roc(
        result_a=result_a,
        result_b=result_b,
        n_scan=n_threshold_scan,
    )

    figure, axis = plt.subplots(
        figsize=(7, 6)
    )

    axis.plot(
        result_a["tpr"],
        result_a["rejection"],
        label=(
            "Region A validation "
            f"(AUC={result_a['auc']:.4f})"
        ),
    )

    axis.plot(
        result_b["tpr"],
        result_b["rejection"],
        label=(
            "Region B validation "
            f"(AUC={result_b['auc']:.4f})"
        ),
    )

    axis.plot(
        combined_efficiency,
        combined_rejection,
        linewidth=3,
        label="Combined A ⊕ B validation",
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

    joblib.dump(
        {
            "model": classifier_a,
            "variables": variables_a,
            "region_branch": region_branch,
            "region_condition": "> 0.5",
        },
        model_a_path,
    )

    joblib.dump(
        {
            "model": classifier_b,
            "variables": variables_b,
            "region_branch": region_branch,
            "region_condition": "<= 0.5",
        },
        model_b_path,
    )

    # --------------------------------------------------------
    # Threshold 및 성능 저장
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
        "validation_fraction": (
            validation_fraction
        ),
        "random_seed": random_seed,
        "efficiency_target": efficiency_target,
        "region_branch": region_branch,
        "variables_a": variables_a,
        "variables_b": variables_b,
        "signal_region_a": int(
            n_signal_a
        ),
        "signal_region_b": int(
            n_signal_b
        ),
        "background_region_a": int(
            n_background_a
        ),
        "background_region_b": int(
            n_background_b
        ),
        "region_a_validation_auc": float(
            result_a["auc"]
        ),
        "region_b_validation_auc": float(
            result_b["auc"]
        ),
        "region_a_validation_signal_nonfinite": int(
            result_a["n_signal_nonfinite"]
        ),
        "region_a_validation_background_nonfinite": int(
            result_a["n_background_nonfinite"]
        ),
        "region_b_validation_signal_nonfinite": int(
            result_b["n_signal_nonfinite"]
        ),
        "region_b_validation_background_nonfinite": int(
            result_b["n_background_nonfinite"]
        ),
        **best,
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
    print(
        f"  Region A model: "
        f"{model_a_path}"
    )
    print(
        f"  Region B model: "
        f"{model_b_path}"
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