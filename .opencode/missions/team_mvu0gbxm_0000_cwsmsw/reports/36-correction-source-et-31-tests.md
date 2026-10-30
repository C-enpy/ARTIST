# 36 — Correction source ciblée et 31 tests Python verts

Le propriétaire a corrigé uniquement le filtre des deux options cibles natives : elles exigent une chaîne non vide, sans filtre « non blanc ». La validation des scalaires, contrôles, paires et limite 4096 reste en place. Le coordinateur a relu le petit diff ; son ré-audit indépendant est demandé avant levée du finding.

Deux tests de caractérisation Python vérifient maintenant la conservation exacte des espaces et espaces insécables. Le protocole Python les admettait déjà : aucun RED artificiel n'est revendiqué. Les 29 tests acquis sont inchangés.

Exécution coordinateur : **31/31 tests passent**, aucun error ou skip, empreintes contrôlées stables. Ce résultat ne valide pas l'exécution du parseur C++ modifié ; celle-ci demeure interdite dans cette mission. Le cœur doit encore aligner son modèle suivant son propre test de régression.

## Preuves

Candidat `1eaa1875cd3757fc94ab84f4c4992a5124ce6781`, auteur uniform-scenarios. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v`.

Journaux et empreintes : `out/verification-synthetic/scenario-target-whitespace-nonregression-2026-10-30T00-39-28-537Z/`. Source : `WorkloadProtocol.hpp:186–199`, tests `test_target_whitespace.py`. Aucun moteur, build, rendu, benchmark, installation ou cloud lancé. Réception et intégration source encore en attente du ré-audit.
