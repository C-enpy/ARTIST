# 22 — Non-régression des scripts Linux et cible graphique explicite

## Résultats vérifiés

Les deux suites historiques ciblées des scripts Linux passent : **9/9** pour le helper et **10/10** pour la collecte de preuves. Le coordinateur les a exécutées sur des copies exactes dans une fixture isolée, pour que leurs écritures de journaux ne touchent pas un arbre moteur. Les empreintes des originaux et des copies sont identiques avant/après. Les commandes de construction sont doublées ; le seul enfant réel de ces tests est un petit programme Python retournant volontairement 7.

Les douze nouveaux tests de la cible graphique attendue ont aussi été relus puis exécutés : **14 échecs d'assertion avec sous-cas**, aucun error ou skip. Ils détectent l'absence des nouveaux arguments et le refus de leur schéma attendu. Ce premier RED porte sur la disponibilité de l'extension ; les contre-exemples de comparaison des observations ne sont pas encore atteints. Un socle nominal minimal est autorisé avant le RED sémantique, sans changement des dix-sept tests précédents.

## Revue et suite

Une revue indépendante statique des sources C++ est active sur le profil correctness-reviewer, sans worktree d'écriture. Elle contrôle notamment les durées de vie, le contexte courant, les oracles par cellule et le protocole borné. La précision de cible matérielle est connue comme modification en cours, pas comme résultat déjà intégré. Une tentative de différer la fiche a été refusée après son admission ; le contrôle natif a confirmé une seule revue active. Aucun arrêt ou cycle de réadmission effectué.

Le cœur et le dispatch Windows poursuivent leurs implémentations minimales après RED observé. Les adaptateurs attendent toujours les interfaces communes réellement intégrées. Aucun build moteur, rendu, benchmark, installation ou cloud.

## Annexe de preuves

- Non-régression Linux, candidat `42828c3a01f997783a3d86da4013a99bf4ef5309` : `out/verification-synthetic/linux-helper-nonregression-2026-10-29T23-57-13-844Z/` ; copies des cinq fichiers utilisés conservées dans `source-fixture/`.
- Cible graphique, candidat `57e47178d4a1d268aac0f2a88caf849df3e1ef01` : `out/verification-synthetic/scenario-hardware-red-2026-10-29T23-58-12-756Z/`.
- Runner : coordinateur, Python 3.13.1, découverte unittest avec `-B` et `-v`, filtres exacts `test_build_linux.py`, `test_linux_evidence.py`, `test_hardware_expectation.py`. Commandes, cwd, journaux et empreintes dans les manifestes.
