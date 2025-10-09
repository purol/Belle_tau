import os

# Parameter sets
masses = [round(0.2 * i, 1) for i in range(1, 9)]  # 0.2 to 1.6 GeV
lifetimes = [0.1, 0.5, 1, 5, 10, 50, 100, 500, 1000]  # mm
coefficients = [(1, 1), (1, -1)]  # (A_COEF, B_COEF)

# Directories
dat_dir = "dat_files"
os.makedirs(dat_dir, exist_ok=True)

# Template
dat_template = """0.08711                         * c*tau(tau life time) (mm) 
1                               * switch for long lived (1:no decay, 0:decay)
1                               * switch for KKMC-JETSET (1:on, 0:off)
********** Above is for basf *********

* MCEventType: 341cltnkeu Descriptor: tau- -> pi-nu, tau+ -> mu+alpha [and cc]
* alpha -> pi- pi+ [alpha: pseudoscalar, mass 1 GeV, width 4 MeV, ctau = 50 cm]

BeginX
********************************************************************************
*               ACTUAL DATA FOR THIS PARTICULAR RUN
********************************************************************************
*indx_______________ccccccccc0ccccccccc0ccccccccc0ccccccccc0ccccccccc0ccccccccc0
*     Center-of-mass energy [GeV]
    1        10.58D0      CMSene =xpar( 1) Average Center of mass energy [GeV]
    2        0.007e0      DelEne =xpar( 2)  Beam energy spread [GeV]
*   5             -1
********************************************************************************
*   61       0.7071d0      spin1x  polarization vector beam 1
*   62            0d0      spin1y  polarization vector beam 1
*   63       0.7071d0      spin1z  polarization vector beam 1
*   64            0d0      spin1x  polarization vector beam 2
*   65      -0.7071d0      spin1y  polarization vector beam 2
*   66       0.7071d0      spin1z  polarization vector beam 2
********************************************************************************
*     Define process
  415              1      KFfin, Tau
********************************************************************************
  901              4      Ihvp  ! =1,2,3,4
********************************************************************************
 2001            0e0      Jak1   =xpar(71)  First  Tau decay mask (tau-)
 2002          373e0      Jak2   =xpar(72)  Second Tau decay mask (tau+)
********************************************************************************
 2900            1e0      BBB    0: ORIG  1: BBB
 2901            0e0      EQUALBR
 2902            2e0      FF2PIRHO
 2903            1e0      IRCHL3PI
 2904            0e0      IFKPIPI
 2905            0e0      IFCURR4PI
 2909            1e0      CHCONJUGATE = 1: swap JAK1/JAK2 for alternate events
 2910            0e0      TauToElOrMu = 1: swap e/mu decay for alternate events
********************************************************************************
 2911{mass:>13.3f}d0      AMALFA: mass of exotic axion-like particle [GeV]
 2912            0e0      ISALFA: spin of exotic axion-like particle
 2913{a_coef:>13.1f}d0      VCALFA: coupling coefficient A in (A+B gamma5) term
 2914{b_coef:>13.1f}d0      ACALFA: coupling coefficient B in (A+B gamma5) term
 2920            3e0      0:invi,1:gg,2:e+e-,3:mu+mu-,4:pi+pi-,5:pi0pi0,6:pi0eta,7:etaeta,8:K+K-,9:KS0KS0,10:pi+K-,11:pi-K+
 2921        0.004d0      GMALFA: width of the exotic particle [GeV]
 2922{lifetime:>13.3f}d0      CTALFA : c*tau (alpha lifetime) (mm)
********************************************************************************
EndX
"""

dat_files = []
for mass in masses:
    for lifetime in lifetimes:
        for a_coef, b_coef in coefficients:
            filename = f"{dat_dir}/alpha_mass{mass:.1f}_life{lifetime}_A{a_coef}_B{b_coef}.dat"
            with open(filename, "w") as f:
                f.write(dat_template.format(mass=mass, lifetime=lifetime, a_coef=a_coef, b_coef=b_coef))
            dat_files.append(filename)

print(f"Generated {len(dat_files)} .dat files in {dat_dir}/")
