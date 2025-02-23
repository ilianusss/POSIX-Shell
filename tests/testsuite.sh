#!/bin/sh

# Couleurs pour l'affichage
green="\033[38;5;82m"
red="\033[38;5;217m"
default="\033[0m"

run_testsuite() {
    local yml_file="$1"
    local testfile="tests/test"

    local test_count=0
    local fail_count=0

    local test_name=""
    local command=""
    local expected_status=""
    local expected_output=""

    # Fonction pour « finaliser » un test, c.-à-d. l'exécuter et vérifier le résultat
    finalize_test() {
        # On ne lance un test que si on a effectivement lu un nom (évite un « test vide »)
        if [ -n "$test_name" ]; then
            (( test_count++ ))

            # Écriture de la commande dans un fichier de test (optionnel)
            echo -e "$command" > "$testfile"

            # 1) Exécuter la commande dans votre shell
            local actual_output
            local actual_status
            actual_output=$(echo -e "$command" | timeout 2s ./src/42sh 2>&1)
            actual_status=$?

            # 2) Exécuter aussi dans bash --posix, pour obtenir la sortie « de référence »
            #    (on le fait une seule fois)
            if [ -z "$expected_output" ]; then
                expected_output=$(echo -e "$command" | timeout 2s bash --posix 2>&1)
                expected_status=$?
            fi

            # 4) Vérifier les résultats : code de retour et sortie
            if [ "$actual_status" -eq 124 ]; then
                # 124 = code de sortie de timeout
                echo -e "${red}====================================${default}"
                echo -e "${red}${test_name} timed out${default}"
                echo
                (( fail_count++ ))
            else
                # Vérification du code de retour ET de la sortie attendue
                if [ "$actual_output" != "$expected_output" ] || [ "$actual_status" -ne "$expected_status" ]; then
                    echo -e "${red}====================================${default}"
                    echo -e "${red}${test_name} failed:${default}"
                    echo -e "${red}Expected output :\n${expected_output}${default}"
                    echo -e "${red}Received output :\n${actual_output}${default}"
                    echo
                    echo -e "${red}===(expected status : $expected_status) (received : $actual_status)====${default}"
                    echo
                    (( fail_count++ ))
                else
                    echo -e "${green}${test_name} passed${default}"
                fi
            fi
        fi
    }

    echo
    echo "Running tests from: $yml_file"

    # On lit le fichier YML ligne à ligne
    while IFS= read -r line; do
        # On saute les lignes vides
        if [ -z "$line" ]; then
            continue
        fi

        # On ignore la clé "tests:" elle-même
        if [[ "$line" =~ ^[[:space:]]*tests: ]]; then
            continue
        fi

        # Quand on rencontre "- name: " => on finalise le test précédent et on initialise un nouveau test
        if [[ "$line" =~ ^[[:space:]]*-[[:space:]]*name:[[:space:]]*\"(.*)\" ]]; then
            # Finalise le test précédent
            finalize_test

            # On capture la valeur de la capture regex (entre guillemets)
            test_name="${BASH_REMATCH[1]}"
            command=""
            expected_status=""
            expected_output=""

        # On capture la ligne command: "xxx"
        elif [[ "$line" =~ [[:space:]]*command:[[:space:]]*\"(.*)\" ]]; then
            command="${BASH_REMATCH[1]}"

        # On capture la ligne expected_status: 0 (par ex.)
        elif [[ "$line" =~ [[:space:]]*expected_status:[[:space:]]*([0-9]+) ]]; then
            expected_status="${BASH_REMATCH[1]}"

        # On capture la ligne expected_output: "xxx"
        elif [[ "$line" =~ [[:space:]]*expected_output:[[:space:]]*\"(.*)\" ]]; then
            expected_output="${BASH_REMATCH[1]}"
            expected_output="$(echo -e "$expected_output")"
        fi
    done < "$yml_file"

    # Ne pas oublier de finaliser le dernier test après la boucle
    finalize_test

    echo "Finished running tests from $yml_file."

    # Calcul du nombre de tests réussis
    local success_count=$(( test_count - fail_count ))
    echo "Success: $success_count/$test_count tests"
    echo
}

# Exécution de la suite de tests sur un ou plusieurs fichiers YAML
run_testsuite "tests/step1.yml"
run_testsuite "tests/step2.yml"
