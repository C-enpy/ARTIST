# 15 — Protocole scenarios : RED semantique confirme

Le socle nominal de construction argv/lecture JSON est disponible au candidat b38c3492a98cdf64530228a5820770c0fa28db9e. Le lead a relu ce code pur et constate que les 17 tests sont inchanges depuis la premiere remise. Aucun import core, acces fichier de production, subprocess ou GL dans le protocole.

Run lead : 17 tests, 84 echecs d'assertion en comptant les sous-cas, code1. Les assertions atteignent maintenant les validations de bornes, selection, identite, schema, compteurs et samples : les ValueError attendues ne sont pas encore levees. Ce RED est different du premier controle de simple presence du module. Empreintes avant/apres inchangees.

Le lead autorise GREEN pour cette seule tranche de protocole Python, sans affaiblissement des tests. Les producteurs C++ attendent encore son GREEN observe. Pas de validation graphique ou de performance ajoutee au mandat.

Attribution : auteur uniform-scenarios, executant lead, Python3.13.1 Windows. Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v.

Journaux et identites : out/verification-synthetic/scenario-protocol-semantic-red-2026-10-29T23-11-13-594Z/. Aucun workload, CMake/Conan/CTest, rendu, benchmark, installation ou cloud execute.
