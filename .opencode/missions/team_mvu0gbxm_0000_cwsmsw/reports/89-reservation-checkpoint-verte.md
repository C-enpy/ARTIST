# 89 — Chemin interne réservé, régression verte

Le coordinateur a relu le petit diff de réservation : admission et publication utilisent la même constante privée ; les destinations canoniques des sorties JUnit et workload ne peuvent plus viser le checkpoint interne. Le refus intervient avant découverte ou enfant, sans nouvelle API.

Le test ciblé passe dans ses deux modes, absolu et relatif normalisé. La non-régression donne **170 succès et deux SKIP sur 172 tests du cœur**, puis **28/28 tests du parseur**, aucun échec ou erreur. Tests, mappings et empreintes restent inchangés depuis le RED. Les deux liens symboliques restent non exécutés faute de droits Windows.

Le ré-audit indépendant ciblé du dernier bloc est demandé. Les trois défauts précédents étaient déjà levés ; le reviewer n'est pas chargé d'un nouvel audit global. Le candidat reste stable et non intégré jusqu'à réception. La future interface bundle et les adaptateurs ne sont pas encore libérés.

## Preuves

- Candidat `bdcaac8228de27d989458965ea890245a31d99c9`, auteur pool-14 ; runner : coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_context_checkpoint_ownership.py -v`, puis même découverte avec `test_*.py`, puis `verification_context` avec `test_*.py`, worktree auteur.
- Tests inchangés depuis `a4aa6bbe58650f789e7f0e41325b23802452416d`.
- Journaux et empreintes : `out/verification-synthetic/context-checkpoint-green-2026-10-30T05-27-26-854Z/`.
- Ré-audit sous `task_mvuhh0f9_003z_78xct7`, lecture seule, aucune exécution par le reviewer.

Aucun moteur, compilation, CTest réel, OpenGL, rendu, benchmark, installation ou cloud exécuté. Le pipeline complet n'est pas encore réceptionné.
