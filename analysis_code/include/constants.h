#ifndef CONSTANTS_H
#define CONSTANTS_H

// luminosity
# define lumi_BelleII_4S 0.36357 // ab-1
# define lumi_BelleII_off 0.04228 // ab-1
# define lumi_BelleII_10810 0.00469 // ab-1


// Scale factors
# define Scale_BelleII_4S_CHG_MC15ri (lumi_BelleII_4S/6.0)
# define Scale_BelleII_4S_MIX_MC15ri (lumi_BelleII_4S/6.0)
# define Scale_BelleII_4S_UUBAR_MC15ri (lumi_BelleII_4S/1.0)
# define Scale_BelleII_4S_DDBAR_MC15ri (lumi_BelleII_4S/1.0)
# define Scale_BelleII_4S_SSBAR_MC15ri (lumi_BelleII_4S/1.0)
# define Scale_BelleII_4S_CHARM_MC15ri (lumi_BelleII_4S/1.0)
# define Scale_BelleII_4S_MUMU_MC15ri (lumi_BelleII_4S/1.0)
# define Scale_BelleII_4S_EE_MC15ri (lumi_BelleII_4S/0.1)
# define Scale_BelleII_4S_EEEE_MC15ri (lumi_BelleII_4S/0.2)
# define Scale_BelleII_4S_EEMUMU_MC15ri (lumi_BelleII_4S/0.2)
# define Scale_BelleII_4S_EEPIPI_MC15ri (lumi_BelleII_4S/1.0)
# define Scale_BelleII_4S_EEKK_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_EEPP_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_PIPIISR_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_PIPIPI0ISR_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_KKISR_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_GG_MC15ri (lumi_BelleII_4S/0.5)
# define Scale_BelleII_4S_EETAUTAU_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_K0K0BARISR_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_MUMUMUMU_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_MUMUTAUTAU_MC15ri (lumi_BelleII_4S/2.0)
# define Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri (lumi_BelleII_4S/10.0)
# define Scale_BelleII_4S_TAUPAIR_MC15ri (lumi_BelleII_4S/1.0)

# define Scale_BelleII_off_UUBAR_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_DDBAR_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_SSBAR_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_CHARM_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_EE_MC15ri (lumi_BelleII_off/0.005)
# define Scale_BelleII_off_EEEE_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_EEMUMU_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_EETAUTAU_MC15ri (lumi_BelleII_off/0.5)
# define Scale_BelleII_off_EEPIPI_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_EEKK_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_EEPP_MC15ri (lumi_BelleII_off/0.5)
# define Scale_BelleII_off_GG_MC15ri (lumi_BelleII_off/0.005)
# define Scale_BelleII_off_MUMU_MC15ri (lumi_BelleII_off/0.05)
# define Scale_BelleII_off_MUMUMUMU_MC15ri (lumi_BelleII_off/0.5)
# define Scale_BelleII_off_TAUPAIR_MC15ri (lumi_BelleII_off/0.05)

# define Scale_BelleII_10810_BBs_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_BsBs_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_CHG_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_MIX_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_UUBAR_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_DDBAR_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_SSBAR_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_CHARM_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_MUMU_MC15ri (lumi_BelleII_10810/0.046)
# define Scale_BelleII_10810_TAUPAIR_MC15ri (lumi_BelleII_10810/0.046)


// for signal
# define tau_crosssection_4S 0.919 // cross section in 4S (nb)
# define tau_crosssection_off 0.929 // cross section in off-resonance (nb)
# define tau_crosssection_10810 0.880 // cross section in 10.810 GeV (nb)
# define Nevt_taupair_BelleII_4S ((lumi_BelleII_4S/0.000000001) * tau_crosssection_4S)
# define Nevt_taupair_BelleII_off ((lumi_BelleII_off/0.000000001) * tau_crosssection_off)
# define Nevt_taupair_BelleII_10810 ((lumi_BelleII_10810/0.000000001) * tau_crosssection_10810)

# define Nevt_SIGNAL_BelleII_4S_MC15ri 10000000 // 5000000 + 5000000
# define Nevt_SIGNAL_BelleII_off_MC15ri 400000 // 200000 + 200000
# define Nevt_SIGNAL_BelleII_10810_MC15ri 400000 // 200000 + 200000

# define BR_SIGNAL 0.00000001 // just set 10^(-8) 

# define Nevt_SIGNAL_BelleII_4S (Nevt_taupair_BelleII_4S * BR_SIGNAL * 2.0)
# define Nevt_SIGNAL_BelleII_off (Nevt_taupair_BelleII_off * BR_SIGNAL * 2.0)
# define Nevt_SIGNAL_BelleII_10810 (Nevt_taupair_BelleII_10810 * BR_SIGNAL * 2.0)

# define Scale_SIGNAL_BelleII_4S_MC15ri (Nevt_SIGNAL_BelleII_4S/Nevt_SIGNAL_BelleII_4S_MC15ri)
# define Scale_SIGNAL_BelleII_off_MC15ri (Nevt_SIGNAL_BelleII_off/Nevt_SIGNAL_BelleII_off_MC15ri)
# define Scale_SIGNAL_BelleII_10810_MC15ri (Nevt_SIGNAL_BelleII_10810/Nevt_SIGNAL_BelleII_10810_MC15ri)

// systematics
# define track_rel_uncertainty 0.24 // %

#endif 