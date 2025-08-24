# shell script to push autogluon model

# Define your variables
USER="junewoo"
HOST="login.cc.kek.jp"
BASE_REMOTE="/home/belle2/junewoo/storage_b2/Analysis"
BASE_LOCAL="./"  # local destination

ARG1="Okina"
ARG2="v009"

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" \
       "EEPIPI" "EEKK" "EEPP" "PIPIISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" \
       "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "SIGNAL")

LOCAL_PATH="${BASE_LOCAL}/${ARG1}/${ARG2}/AutogluonModels/"
REMOTE_PATH="${BASE_REMOTE}/${ARG1}/${ARG2}/"

rsync -avz "${LOCAL_PATH%/}" "${USER}@${HOST}:${REMOTE_PATH}"