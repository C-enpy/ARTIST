# 21 — Dispatch Windows : RED comportemental sous outils factices

## Résultat et conséquence

Le pipeline Windows doit isoler préparation, construction et exécution sans retomber sur l'ancienne campagne. Après revue du harnais et du fragment exact, le coordinateur a exécuté ses dix méthodes de test : quatre passent, six échouent pour la raison attendue, soit onze échecs d'assertion avec les sous-cas. Aucun error, skip ou timeout.

Les autotests du harnais passent : il sait enregistrer un enfant fictif, conserver ses codes 0/17 et restaurer une map d'environnement. Il rejette les constructions non autorisées avant évaluation. Les échecs du dispatch proviennent uniquement du refus provisoire des phases encore non implémentées.

L'auteur peut implémenter ce fragment avec les quatre frontières factices convenues. Les fonctions réelles de lancement et d'accès à MSVC restent une tranche distincte ; les tests actuels ne prouvent pas leur fonctionnement.

## Confinement et limites

Seul un programme de test PowerShell a été lancé. Il lit un fragment audité comme donnée, puis remplace tous les effets par des événements et des maps. Aucun helper complet, compilateur, Conan, CMake, CTest ou workload n'est exécuté. L'audit est une protection étroite pour le code relu, pas une sandbox générale.

## Preuves

Auteur : windows-pipeline ; runner : coordinateur. Candidat `b96df6530e486c6057e8ad7e7414a4a4946110a9`, empreintes inchangées pendant le run. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_dispatch.py -v`.

Logs et manifeste : `out/verification-synthetic/windows-dispatch-red-2026-10-29T23-56-06-752Z/`. Données de chaque fixture dans le worktree auteur : `out/verification-synthetic/windows-dispatch-fixtures/20261029T235600Z-79d70755a9ed4d5787cd283698a8a41b/`.
