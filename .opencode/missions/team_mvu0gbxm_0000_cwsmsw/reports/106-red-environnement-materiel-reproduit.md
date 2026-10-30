# 106 — Héritage logiciel et refus manquants reproduits

Le coordinateur a vérifié le diff de fixture : parent entièrement synthétique,
dix méthodes et assertions inchangées, aucune modification de production ou des
anciens tests. Le seul module nouveau a ensuite été exécuté avec des factories
strictement en mémoire.

Résultat : **10 tests, 33 échecs d'assertion avec sous-cas**, aucune erreur ou skip.
Les variables de forçage apparaissent effectivement dans l'environnement enfant
matériel et les transports invalides atteignent la factory au lieu du refus
attendu. Les empreintes sont stables. Ce RED n'est ni une panne d'outil, ni un
contexte GPU absent. Les assertions aval masquées ne sont pas toutes prétendues
individuellement discriminées.

Le GREEN minimal est autorisé uniquement sur la construction de l'environnement
dans runner.py : contrôle du matériel explicitement déclaré, refus ValueError
attribuable avant lancement, suppression des deux variables dans la copie enfant,
puis application du delta. CPU/logiciel et parent inchangés. Capture, durée de vie
du primaire, modèles et adaptateurs restent hors modification.

Les dix tests restent intacts. Après relecture du candidat corrigé, le lead
exécutera ciblés et non-régression, puis une revue indépendante précédera
l'intégration. Les adaptateurs attendent cette entrée commune intégrée.

## Preuves

- Candidat `eefb1fdda5e06e581bdebcb74df7ce80c3e86f8a`, auteur pool-26 ; runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_hardware_child_environment_contract.py -v`, worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/hardware-child-environment-red-2026-10-30T07-13-08-668Z/`.

Aucun enfant réel, outil natif, moteur, OpenGL, rendu, benchmark, installation ou
cloud exécuté. Aucune qualification matérielle n'est déduite des données factices.
