# 111 — RED du raccordement Linux sur l'ancien runner

Le coordinateur a relu puis exécuté les quatre tests du consommateur Linux :
**4 tests, 9 échecs d'assertion matériels avec sous-cas**, aucune erreur ou skip.
Les baselines CPU neutre/forcé et logiciel explicite passent. Les captures et
factories sont possédées, le parent entièrement synthétique ; aucun processus.

Ce RED est intentionnellement constaté sur l'ancien runner du worktree Linux.
Les six valeurs sont bien décrites par l'adaptateur ; le forçage logiciel demeure
hérité au lancement avec ce runner. Production Linux, anciens tests et empreintes
restent inchangés. Aucune copie du correctif commun dans une branche n'est faite.

Le correctif commun est déjà intégré et vérifié dans l'arbre principal. Le reviewer
Linux est rappelé pour le ré-audit ciblé de cette composition de sources et du
test discriminant. Après avis, un merge natif des seuls fichiers Linux permettra
au coordinateur de vérifier le GREEN dans l'arbre principal. Cette vérification
reste à faire ; ni la levée statique ni le RED ne valent réception finale.

## Preuves et identités

- Candidat tests Linux `fa78654d4d62e8a0b31ef2b2c6cc4be183db3505`, production inchangée depuis `3e697590c03869f53b64aec8dde0cd706bceff52`.
- Ancien blob runner `cfc88742b7bccca9bb868885451bcb91b7400ac9` ; correctif commun intégré `358921c42b83f5ae619c97fb8f7c1a66700ef4e6` (rapport109).
- Auteur pool-24 ; runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p test_effective_linux_environment.py -v`, worktree Linux.
- Journaux et empreintes : `out/verification-synthetic/linux-effective-environment-red-2026-10-30T07-28-30-510Z/`.
- Ré-audit ciblé : reviewer Linux initial, tâche `task_mvum0xq5_0059_mkoad4`, lecture seule.

Linux natif, outils, moteur, GPU, rendu, benchmark, installation et cloud
NON_EXECUTE. Ces tests ne prouvent aucun renderer matériel.
