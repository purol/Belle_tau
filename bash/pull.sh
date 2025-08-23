# shell script to get ROOT files for autogluon

eval "$(ssh-agent -s)"
trap 'kill $SSH_AGENT_PID' EXIT
ssh-add

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

for type in "${Types[@]}"; do
    for suffix in "final_output_train" "final_output_test"; do
        REMOTE_PATH="${BASE_REMOTE}/${ARG1}/${ARG2}/${type}/${suffix}/"
        LOCAL_PATH="${BASE_LOCAL}/${ARG1}/${ARG2}/${type}/${suffix}/"
        mkdir -p "$LOCAL_PATH"
        rsync -avz "${USER}@${HOST}:${REMOTE_PATH}" "$LOCAL_PATH" || {
            echo "Error during rsync for ${type}/${suffix}, aborting."
            exit 1
        }
    done
done
rsync -avz "${USER}@${HOST}:/${BASE_REMOTE}/${ARG1}/${ARG2}/M_deltaE_result.txt" "${BASE_LOCAL}/${ARG1}/${ARG2}/"