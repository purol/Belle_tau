#ifndef CONSTANTS_H
#define CONSTANTS_H

#define lumi_scale (1.0)

// luminosity
# define lumi_BelleII_4S (lumi_scale * 0.49841) // ab-1
# define lumi_BelleII_off (lumi_scale * 0.061) // ab-1
# define lumi_BelleII_10810 (lumi_scale * 0.00469) // ab-1
# define lumi_BelleII_5S (lumi_scale * 0.01976) // ab-1

// luminosity uncertainty
// https://xwiki.desy.de/xwiki/bin/view/BI/Belle%20II%20Internal/Data%20Production%20WebHome/Offline%20Luminosity%20Page/?srid=IIYdLMEl
# define lumi_BelleII_4S_uncertainty (lumi_scale * 0.0017) // ab-1
# define lumi_BelleII_off_uncertainty (lumi_scale * 0.0002) // ab-1
# define lumi_BelleII_10810_uncertainty (lumi_scale * 0.000027) // ab-1
# define lumi_BelleII_5S_uncertainty (lumi_scale * 0.00011) // ab-1


// Scale factors
# define Scale_BelleII_4S_CHG_MC15ri (lumi_BelleII_4S/6.0)
# define Scale_BelleII_4S_MIX_MC15ri (lumi_BelleII_4S/6.0)
# define Scale_BelleII_4S_UUBAR_MC15ri (lumi_BelleII_4S/8.0)
# define Scale_BelleII_4S_DDBAR_MC15ri (lumi_BelleII_4S/8.0)
# define Scale_BelleII_4S_SSBAR_MC15ri (lumi_BelleII_4S/8.0)
# define Scale_BelleII_4S_CHARM_MC15ri (lumi_BelleII_4S/8.0)
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

# define Scale_BelleII_4S_CHG_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_MIX_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_UUBAR_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_DDBAR_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_SSBAR_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_CHARM_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_MUMU_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_EE_MC16ri (lumi_BelleII_4S/0.175)
# define Scale_BelleII_4S_EEEE_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_EEMUMU_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_hhISR_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_GG_MC16ri (lumi_BelleII_4S/1.35)
# define Scale_BelleII_4S_llXX_MC16ri (lumi_BelleII_4S/1.1)
# define Scale_BelleII_4S_TAUPAIR_MC16ri (lumi_BelleII_4S/1.1)

# define Scale_BelleII_4S_CHG_MC16rd (lumi_BelleII_4S/1.94633937621)
# define Scale_BelleII_4S_MIX_MC16rd (lumi_BelleII_4S/1.94563145615)
# define Scale_BelleII_4S_UUBAR_MC16rd (lumi_BelleII_4S/1.94633637827)
# define Scale_BelleII_4S_DDBAR_MC16rd (lumi_BelleII_4S/1.94633884394)
# define Scale_BelleII_4S_SSBAR_MC16rd (lumi_BelleII_4S/1.94633884394)
# define Scale_BelleII_4S_CHARM_MC16rd (lumi_BelleII_4S/1.94633711858)
# define Scale_BelleII_4S_MUMU_MC16rd (lumi_BelleII_4S/1.94633884394)
# define Scale_BelleII_4S_EE_MC16rd (lumi_BelleII_4S/0.0486578614459)
# define Scale_BelleII_4S_EEEE_MC16rd (lumi_BelleII_4S/0.486566030869)
# define Scale_BelleII_4S_EEMUMU_MC16rd (lumi_BelleII_4S/0.486578551736)
# define Scale_BelleII_4S_hhISR_MC16rd (lumi_BelleII_4S/0.453130660382)
# define Scale_BelleII_4S_GG_MC16rd (lumi_BelleII_4S/0.973157373475)
# define Scale_BelleII_4S_llXX_MC16rd (lumi_BelleII_4S/1.94633637827)
# define Scale_BelleII_4S_TAUPAIR_MC16rd (lumi_BelleII_4S/1.94633937621)
# define Scale_BelleII_4S_BB_MC16rd (lumi_BelleII_4S/1.94598541618) // just average of CHG, MIX
# define Scale_BelleII_4S_UDSC_MC16rd (lumi_BelleII_4S/1.94633779618) // just average of udsc

# define Scale_BelleII_off_UUBAR_MC16rd (lumi_BelleII_off/0.239977757314)
# define Scale_BelleII_off_DDBAR_MC16rd (lumi_BelleII_off/0.239977757314)
# define Scale_BelleII_off_SSBAR_MC16rd (lumi_BelleII_off/0.239977757314)
# define Scale_BelleII_off_CHARM_MC16rd (lumi_BelleII_off/0.239977757314)
# define Scale_BelleII_off_MUMU_MC16rd (lumi_BelleII_off/0.239977757314)
# define Scale_BelleII_off_EE_MC16rd (lumi_BelleII_off/0.00599944393284)
# define Scale_BelleII_off_EEEE_MC16rd (lumi_BelleII_off/0.0586445594131)
# define Scale_BelleII_off_EEMUMU_MC16rd (lumi_BelleII_off/0.0586450400904)
# define Scale_BelleII_off_hhISR_MC16rd (lumi_BelleII_off/0.0599961249791)
# define Scale_BelleII_off_GG_MC16rd (lumi_BelleII_off/0.119988878657)
# define Scale_BelleII_off_llXX_MC16rd (lumi_BelleII_off/0.238369538444)
# define Scale_BelleII_off_TAUPAIR_MC16rd (lumi_BelleII_off/0.239977757314)
# define Scale_BelleII_off_UDSC_MC16rd (lumi_BelleII_off/0.239977757314) // just average of udsc

# define Scale_BelleII_5S_UUBAR_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_DDBAR_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_SSBAR_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_CHARM_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_MUMU_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_EE_MC16rd (lumi_BelleII_5S/0.00196347700299)
# define Scale_BelleII_5S_EEEE_MC16rd (lumi_BelleII_5S/0.0196347700299)
# define Scale_BelleII_5S_EEMUMU_MC16rd (lumi_BelleII_5S/0.0196347700299)
# define Scale_BelleII_5S_hhISR_MC16rd (lumi_BelleII_5S/0.0196347700299)
# define Scale_BelleII_5S_GG_MC16rd (lumi_BelleII_5S/0.0392695400598)
# define Scale_BelleII_5S_llXX_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_TAUPAIR_MC16rd (lumi_BelleII_5S/0.0785390801196)
# define Scale_BelleII_5S_UDSC_MC16rd (lumi_BelleII_5S/0.0785390801196) // just average of udsc


// for signal
# define tau_crosssection_4S 0.919 // cross section in 4S (nb)
# define TAU_CROSSSECTION(E) ( tau_crosssection_4S * (10.58 * 10.58) / ((E) * (E)) )
# define tau_crosssection_off TAU_CROSSSECTION(10.52) // cross section in off-resonance (nb)
# define tau_crosssection_10810 TAU_CROSSSECTION(10.810) // cross section in 10.810 GeV (nb)
# define tau_crosssection_5S TAU_CROSSSECTION(((10.657*3.544/19.764)+(10.706*1.628/19.764)+(10.751*9.880/19.764)+(10.810*4.713/19.764))) // Just average energy
# define tau_crosssection_4S_reluncertainty (0.003 / tau_crosssection_4S) // cross section uncertainty in 4S (nb). Relative uncertainty.
# define Nevt_taupair_BelleII_4S ((lumi_BelleII_4S/0.000000001) * tau_crosssection_4S)
# define Nevt_taupair_BelleII_off ((lumi_BelleII_off/0.000000001) * tau_crosssection_off)
# define Nevt_taupair_BelleII_10810 ((lumi_BelleII_10810/0.000000001) * tau_crosssection_10810)
# define Nevt_taupair_BelleII_5S ((lumi_BelleII_5S/0.000000001) * tau_crosssection_5S)

# define Nevt_SIGNAL_BelleII_4S_MC15ri 10000000 // 5000000 + 5000000
# define Nevt_SIGNAL_BelleII_off_MC15ri 400000 // 200000 + 200000
# define Nevt_SIGNAL_BelleII_10810_MC15ri 400000 // 200000 + 200000
# define Nevt_SIGNAL_BelleII_4S_MC16ri 11000000 // run1:5000000, run2 PXD OFF:3000000, run2 PXD ON: 3000000
# define Nevt_SIGNAL_BelleII_4S_MC16rd 4632618
# define Nevt_SIGNAL_BelleII_off_MC16rd 564223
# define Nevt_SIGNAL_BelleII_5S_MC16rd 186901

# define BR_SIGNAL 0.00000001 // just set 10^(-8) 

# define Nevt_SIGNAL_BelleII_4S (Nevt_taupair_BelleII_4S * BR_SIGNAL * 2.0)
# define Nevt_SIGNAL_BelleII_off (Nevt_taupair_BelleII_off * BR_SIGNAL * 2.0)
# define Nevt_SIGNAL_BelleII_10810 (Nevt_taupair_BelleII_10810 * BR_SIGNAL * 2.0)
# define Nevt_SIGNAL_BelleII_5S (Nevt_taupair_BelleII_5S * BR_SIGNAL * 2.0)

# define Scale_SIGNAL_BelleII_4S_MC15ri (Nevt_SIGNAL_BelleII_4S/Nevt_SIGNAL_BelleII_4S_MC15ri)
# define Scale_SIGNAL_BelleII_off_MC15ri (Nevt_SIGNAL_BelleII_off/Nevt_SIGNAL_BelleII_off_MC15ri)
# define Scale_SIGNAL_BelleII_10810_MC15ri (Nevt_SIGNAL_BelleII_10810/Nevt_SIGNAL_BelleII_10810_MC15ri)

# define Scale_SIGNAL_BelleII_4S_MC16ri (Nevt_SIGNAL_BelleII_4S/Nevt_SIGNAL_BelleII_4S_MC16ri)

# define Scale_SIGNAL_BelleII_4S_MC16rd (Nevt_SIGNAL_BelleII_4S/Nevt_SIGNAL_BelleII_4S_MC16rd)
# define Scale_SIGNAL_BelleII_off_MC16rd (Nevt_SIGNAL_BelleII_off/Nevt_SIGNAL_BelleII_off_MC16rd)
# define Scale_SIGNAL_BelleII_5S_MC16rd (Nevt_SIGNAL_BelleII_5S/Nevt_SIGNAL_BelleII_5S_MC16rd)

# define Nevt_SIGNAL (Nevt_SIGNAL_BelleII_4S + Nevt_SIGNAL_BelleII_off + Nevt_SIGNAL_BelleII_5S)


// for ALP signal
# define Nevt_ALP_BelleII_4S_MC15ri 200000
# define Nevt_ALP_BelleII_4S_MC16ri 220000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_01 10000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_1 10000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_10 10000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_100 15000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_250 50000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_500 80000
# define Nevt_ALP_BelleII_4S_MC16rd_ctau_1000 90000

# define BR_ALP 0.00000001 // just set 10^(-8) 

# define Nevt_ALP_BelleII_4S (Nevt_taupair_BelleII_4S * BR_ALP * 2.0)
# define Nevt_ALP_BelleII_off (Nevt_taupair_BelleII_off * BR_ALP * 2.0)
# define Nevt_ALP_BelleII_10810 (Nevt_taupair_BelleII_10810 * BR_ALP * 2.0)
# define Nevt_ALP_BelleII_5S (Nevt_taupair_BelleII_5S * BR_ALP * 2.0)

# define Scale_ALP_BelleII_4S_MC15ri ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC15ri) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16ri (Nevt_ALP_BelleII_4S/Nevt_ALP_BelleII_4S_MC16ri)
# define Scale_ALP_BelleII_4S_MC16rd_ctau_01 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_01) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16rd_ctau_1 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_1) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16rd_ctau_10 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_10) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16rd_ctau_100 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_100) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16rd_ctau_250 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_250) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16rd_ctau_500 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_500) // here, we only produce on-resonance, but we need to cover all energy
# define Scale_ALP_BelleII_4S_MC16rd_ctau_1000 ((Nevt_ALP_BelleII_4S + Nevt_ALP_BelleII_off + Nevt_ALP_BelleII_5S)/Nevt_ALP_BelleII_4S_MC16rd_ctau_1000) // here, we only produce on-resonance, but we need to cover all energy

// systematics
# define track_rel_uncertainty 0.24 // %

#endif 