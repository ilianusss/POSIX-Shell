#!/bin/sh

# Couleurs pour l'affichage
green="\033[38;5;82m"
red="\033[38;5;217m"
default="\033[0m"

run_testsuite() {
    local yml_file="$1"
    local testfile="tests/test"

    local test_count=0
    local leak_count=0

    local test_name=""
    local command=""

    # Fonction pour « finaliser » un test, c.-à-d. l'exécuter avec Valgrind et vérifier les memory leaks
    finalize_test() {
        if [ -n "$test_name" ]; then
            (( test_count++ ))

            # On écrit la "commande" dans un fichier de test
            echo -e "$command" > "$testfile"

            # Exécute la commande avec Valgrind
            valgrind_output=$(echo -e "$command" | valgrind --leak-check=full --log-file=valgrind.log ./src/42sh 2>&1)
            
            # Vérifie la présence de memory leaks dans le fichier log de Valgrind
            if grep -q "definitely lost: [1-9][0-9]* bytes" valgrind.log; then
                echo -e "${red}====================================${default}"
                echo -e "${red}${test_name} has memory leaks:${default}"
                grep "definitely lost:" valgrind.log
                echo
                (( leak_count++ ))
            else
                echo -e "${green}${test_name} has no memory leaks${default}"
            fi
        fi
    }

    echo
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
            finalize_test

            # On capture la valeur de la capture regex (entre guillemets)
            test_name="${BASH_REMATCH[1]}"
            command=""

        # On capture la ligne command: "xxx"
        elif [[ "$line" =~ [[:space:]]*command:[[:space:]]*\"(.*)\" ]]; then
            command="${BASH_REMATCH[1]}"
        fi
    done < "$yml_file"

    # Ne pas oublier de finaliser le dernier test après la boucle
    finalize_test

    echo "Finished running tests from $yml_file."

    # Calcul du nombre de tests avec des leaks
    local success_count=$(( test_count - leak_count ))
    echo "Tests without leaks: $success_count/$test_count"
    echo "Tests with leaks: $leak_count/$test_count"
    echo
    rm "valgrind.log"
}

# Exécution de la suite de tests sur un ou plusieurs fichiers YAML
run_testsuite "tests/step1.yml"
run_testsuite "tests/step2.yml"
