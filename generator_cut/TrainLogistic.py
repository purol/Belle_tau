#!/usr/bin/env python3
import uproot
import numpy as np
import matplotlib.pyplot as plt

from sklearn.linear_model import LogisticRegression
from sklearn.preprocessing import StandardScaler, PolynomialFeatures
from sklearn.metrics import roc_curve

# ================= CONFIG =================
sig_file = "./myskimstrong_1_ntuple/signal_train.root"
bkg_file = "./myskimstrong_reverse_ntuple/Ntuple_reverse.root"
tree_name = "tree"

variables = [
    "thrust",
    "thrustAxisCosTheta",
    "genTotalPhotonsEnergyOfEvent",
    "Net_gencut",
    "sphericity",
    "aplanarity",
    "Ntau_gencut",
    "Ntrack_gencut",
    "NTBz_gencut",
    "foxWolframR1",
    "foxWolframR2",
    "foxWolframR3",
    "harmonicMomentThrust0",
    "harmonicMomentThrust1"
]
# =========================================


# ================= READ ROOT =================
def read_vars(fname):
    with uproot.open(fname) as f:
        tree = f[tree_name]
        arrs = [tree[v].array(library="np") for v in variables]

    X = np.vstack([np.asarray(a).ravel() for a in arrs]).T
    mask = np.all(np.isfinite(X), axis=1)
    return X[mask]


X_sig = read_vars(sig_file)
X_bkg = read_vars(bkg_file)


# ================= STRONG CUT =================
idx_fake = variables.index("Ntau_gencut")

def strong_cut(X):
    return X[:, idx_fake] > 0.5


maskA_sig = strong_cut(X_sig)
maskA_bkg = strong_cut(X_bkg)

XA_sig, XB_sig = X_sig[maskA_sig], X_sig[~maskA_sig]
XA_bkg, XB_bkg = X_bkg[maskA_bkg], X_bkg[~maskA_bkg]


# ================= TRAIN: QUADRATIC LR =================
def train_lr_quadratic(X_sig, X_bkg):
    X = np.vstack([X_sig, X_bkg])
    y = np.concatenate([np.ones(len(X_sig)), np.zeros(len(X_bkg))])

    scaler = StandardScaler()
    Xs = scaler.fit_transform(X)

    poly = PolynomialFeatures(degree=2, include_bias=False)
    Xp = poly.fit_transform(Xs)

    clf = LogisticRegression(
        C=10.0,
        penalty="l2",
        solver="lbfgs",
        max_iter=1000,
        class_weight="balanced"
    )
    clf.fit(Xp, y)

    LL = clf.decision_function(Xp)
    fpr, tpr, _ = roc_curve(y, LL)

    return {
        "LL": LL,
        "y": y,
        "tpr": tpr,
        "rej": 1.0 - fpr,
        "weights": clf.coef_[0],
        "bias": clf.intercept_[0],
        "scaler": scaler,
        "poly": poly
    }


# ================= TRAIN ALL MODELS =================
resA = train_lr_quadratic(XA_sig, XA_bkg)
resB = train_lr_quadratic(XB_sig, XB_bkg)
resALL = train_lr_quadratic(X_sig, X_bkg)


# ================= COMBINED ROC (A ⊕ B) =================
def combined_roc(resA, resB, n_scan=300):
    thrA = np.quantile(resA["LL"], np.linspace(0.01, 0.99, n_scan))
    thrB = np.quantile(resB["LL"], np.linspace(0.01, 0.99, n_scan))

    best = {}
    n_sig = np.sum(resA["y"] == 1) + np.sum(resB["y"] == 1)
    n_bkg = np.sum(resA["y"] == 0) + np.sum(resB["y"] == 0)

    for tA in thrA:
        selA = resA["LL"] > tA
        for tB in thrB:
            selB = resB["LL"] > tB

            sig = np.sum(selA & (resA["y"] == 1)) + np.sum(selB & (resB["y"] == 1))
            bkg = np.sum(selA & (resA["y"] == 0)) + np.sum(selB & (resB["y"] == 0))

            eff = sig / n_sig
            rej = 1.0 - bkg / n_bkg
            best[round(eff, 4)] = max(best.get(round(eff, 4), 0.0), rej)

    effs = np.array(sorted(best.keys()))
    rejs = np.array([best[e] for e in effs])
    return effs, rejs


effC, rejC = combined_roc(resA, resB)


# ================= OPTIMAL CUTS =================
def best_at_eff_single(res, eff_target):
    idx = np.argmax(res["tpr"] >= eff_target)
    return dict(
        eff=res["tpr"][idx],
        rej=res["rej"][idx],
        thr=np.quantile(res["LL"], 1 - res["tpr"][idx])
    )


def best_at_eff_combined(resA, resB, eff_target, n_scan=300):
    thrA = np.quantile(resA["LL"], np.linspace(0.01, 0.99, n_scan))
    thrB = np.quantile(resB["LL"], np.linspace(0.01, 0.99, n_scan))

    yA, yB = resA["y"], resB["y"]
    n_sig = np.sum(yA == 1) + np.sum(yB == 1)
    n_bkg = np.sum(yA == 0) + np.sum(yB == 0)

    best = {"rej": -1}

    for tA in thrA:
        selA = resA["LL"] > tA
        for tB in thrB:
            selB = resB["LL"] > tB

            sig = np.sum(selA & (yA == 1)) + np.sum(selB & (yB == 1))
            bkg = np.sum(selA & (yA == 0)) + np.sum(selB & (yB == 0))

            eff = sig / n_sig
            if eff < eff_target:
                continue

            rej = 1.0 - bkg / n_bkg
            if rej > best["rej"]:
                best = dict(eff=eff, rej=rej, thrA=tA, thrB=tB)

    return best


EFF_TARGET = 0.985

best_comb = best_at_eff_combined(resA, resB, EFF_TARGET)
best_all  = best_at_eff_single(resALL, EFF_TARGET)

print("\n=== OPTIMAL CUTS @ eff=0.985 ===")
print("A ⊕ B :", best_comb)
print("ALL   :", best_all)


# ================= PLOT =================
plt.figure(figsize=(7,6))
plt.plot(resA["tpr"], resA["rej"], label="Region A")
plt.plot(resB["tpr"], resB["rej"], label="Region B")
plt.plot(effC, rejC, lw=3, label="Combined A ⊕ B")
plt.plot(resALL["tpr"], resALL["rej"], lw=3, ls="--", label="No strong cut")
plt.xlabel("Signal efficiency")
plt.ylabel("Background rejection")
plt.grid(True, ls="--", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.show()


# ================= basf2 ALIAS EXPORT =================
def print_alias(name, res):
    poly = res["poly"]
    w = res["weights"]
    bias = res["bias"]
    mean = res["scaler"].mean_
    std  = res["scaler"].scale_

    feature_names = poly.get_feature_names_out(variables)
    terms = []

    for coef, feat in zip(w, feature_names):
        if abs(coef) < 1e-6:
            continue

        factors = []
        for token in feat.split(" "):
            if "^2" in token:
                v = token.replace("^2", "")
                i = variables.index(v)
                factors.append(f"(({v}-{mean[i]:.6f})/{std[i]:.6f})^2")
            else:
                v = token
                i = variables.index(v)
                factors.append(f"(({v}-{mean[i]:.6f})/{std[i]:.6f})")

        terms.append(f"{coef:.6f}*({'*'.join(factors)})")

    formula = " + ".join(terms) + f" + ({bias:.6f})"

    print(
        f'va.variables.addAlias(\n'
        f'    "{name}",\n'
        f'    "formula({formula})"\n'
        f')\n'
    )


print("\n=== basf2 aliases ===")
print_alias("LL_score_A_quad", resA)
print_alias("LL_score_B_quad", resB)
print_alias("LL_score_ALL_quad", resALL)
