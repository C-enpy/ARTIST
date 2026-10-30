# 71 — Nettoyage du primaire : test ciblé vert, 142 succès en non-régression

Le coordinateur a relu le déplacement du stop/reap dans le `finally` englobant, avant snapshots et fermeture. Le test d'interruption après acquisition passe, avec le même repère structurel et les mêmes assertions. La suite complète découvre **144 tests : 142 satisfaits, deux SKIP de liens symboliques, aucun échec ou erreur**.

Les empreintes sont stables et les tests/mappings inchangés depuis le RED. Les deux SKIP restent des limites runtime Windows non résolues, sans nouveau droit ou contournement. Aucun traitement des descendants n'est ajouté.

Un ré-audit indépendant ciblé est demandé avant réception du cœur : couverture du résidu après acquisition et de la fenêtre entre attente et nettoyage. Les répétitions arbitraires d'interruption pendant le nettoyage restent best-effort, sans promesse d'immunité absolue. L'auteur conserve le candidat stable.

Même une réception favorable ne validera que la fondation du cœur : composition du collecteur et des adaptateurs encore à livrer, tests matériels historiques provisoirement bloqués depuis ce candidat. Aucun moteur, compilation, OpenGL, rendu, benchmark, installation, cloud ou descendant réel exécuté.

## Preuves

- Candidat `e1bef8ddef6e16b81408260daca594be00bcc1d5`, auteur pool-3, runner coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_primary_ownership_interrupt.py -v`, puis même commande avec `test_*.py`, worktree auteur.
- Tests/mappings inchangés depuis `aa9c17c68fff5ecd29190139b308177049be47f8`.
- Journaux et empreintes : `out/verification-synthetic/primary-ownership-green-2026-10-30T03-56-24-721Z/`.
