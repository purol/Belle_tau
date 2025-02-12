#ifndef CONSTANTS_H
#define CONSTANTS_H

# define Scale_CHG_MC15ri (0.36537/1.0)
# define Scale_MIX_MC15ri (0.36537/1.0)
# define Scale_UUBAR_MC15ri (0.36537/1.0)
# define Scale_DDBAR_MC15ri (0.36537/1.0)
# define Scale_SSBAR_MC15ri (0.36537/1.0)
# define Scale_CHARM_MC15ri (0.36537/1.0)
# define Scale_MUMU_MC15ri (0.36537/1.0)
# define Scale_EE_MC15ri (0.36537/0.1)
# define Scale_EEEE_MC15ri (0.36537/0.2)
# define Scale_EEMUMU_MC15ri (0.36537/0.2)
# define Scale_EEPIPI_MC15ri (0.36537/1.0)
# define Scale_EEKK_MC15ri (0.36537/2.0)
# define Scale_EEPP_MC15ri (0.36537/2.0)
# define Scale_PIPIISR_MC15ri (0.36537/2.0)
# define Scale_KKISR_MC15ri (0.36537/2.0)
# define Scale_GG_MC15ri (0.36537/0.5)
# define Scale_EETAUTAU_MC15ri (0.36537/2.0)
# define Scale_K0K0BARISR_MC15ri (0.36537/2.0)
# define Scale_MUMUMUMU_MC15ri (0.36537/2.0)
# define Scale_MUMUTAUTAU_MC15ri (0.36537/2.0)
# define Scale_TAUTAUTAUTAU_MC15ri (0.36537/10.0)
# define Scale_TAUPAIR_MC15ri (0.36537/1.455052)

// for signal
# define tau_crosssection 0.919 // nb
# define Nevt_taupair ((0.36537/0.000000001) * tau_crosssection)

# define Nevt_SIGNAL_MC15ri 5000000

# define BR_SIGNAL 0.00000001 // just set 10^(-8) 
# define Nevt_SIGNAL (Nevt_taupair * BR_SIGNAL * 2.0)
# define Scale_SIGNAL_MC15ri (Nevt_SIGNAL/Nevt_SIGNAL_MC15ri)

// systematics
# define track_rel_uncertainty 0.24 // %

#endif 