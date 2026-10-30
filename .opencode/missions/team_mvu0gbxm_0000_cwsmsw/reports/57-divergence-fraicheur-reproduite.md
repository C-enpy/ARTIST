# 57 — Divergence de fraîcheur reproduite sur fichiers factices

Le coordinateur a relu les deux nouveaux tests puis exécuté leur fichier seul : **deux tests, deux échecs d'assertion**, aucun error ou skip. Après un bundle synthétique favorable, l'ajout d'une DLL factice ou d'un deuxième fichier correspondant à la cible laisse le rapport `CURRENT`, code 0, au lieu de `STALE`, code 4, attendu par l'oracle proposé.

L'implémentation est inchangée depuis le candidat réaudité, et les empreintes contrôlées sont stables pendant le run. Les fichiers sont des données possédées par la fixture ; aucun binaire n'est chargé ou exécuté, aucun processus moteur ou sonde de révision lancé.

Ce résultat reproduit la différence entre les critères de population du build et ceux du rapport. Il ne démontre pas que le binaire utilisé ou le résultat du test passé a changé. Le domaine précis de fraîcheur reste soumis à la consultation architecte déjà ouverte ; aucun GREEN n'est autorisé avant ratification locale de ce choix. Les tests acquis ne sont pas affaiblis.

## Preuves

- Candidat `0f751ce9b9bf2a1b079b76b54c7a8f1b7061aaac`, auteur pool-3, runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_report_build_population.py -v`, depuis le worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/build-population-red-2026-10-30T03-02-39-246Z/`.

Le défaut de capture Windows reste indépendamment bloquant. Aucun moteur, compilation, rendu, benchmark, installation ou cloud exécuté.
