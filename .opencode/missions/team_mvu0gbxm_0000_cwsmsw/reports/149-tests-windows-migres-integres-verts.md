# 149 — Tests Windows migrés, intégrés et verts

Le coordinateur a relu les cinq fichiers livrés et leurs frontières d'exécution.
Les trois fichiers Python passent l'analyse syntaxique sans import ; la fixture
PowerShell passe le parsing sans évaluation de son programme. Sa fonction d'audit
est identique à l'ancienne et reste extractible. Le diff respecte les seuls fichiers
autorisés ; les onze méthodes wrappers conservées sont identiques par comparaison
AST. Les deux méthodes retirées sont celles du mapping accepté, avec leurs
obligations réattribuées visiblement et l'oracle d'admission fine ajouté.

Après shutdown et merge natifs, les cinq fichiers reçus sont identiques au
candidat auteur. Les tests ont été intégrés pour validation dans l'arbre principal,
sans changement de production, de pin ou de fixture commune.

Résultats exécutés par le coordinateur :
- **5/5** : quatre tests d'environnement effectif et l'oracle d'admission fine.
- **4/4** : préparation Release/Jobs1, build CPU unique/Jobs1, premier Conan23
  sans configure et mutant FileWriteAllText traité uniquement comme donnée d'audit.
- **10/10** : wrappers conservés, dont quoting/code17, et raccordement réel du
  wrapper au binding avec capture simulée de StartInfo.

Soit **19 tests, sans échec, erreur ou skip**, empreintes avant/après stables.
Les programmes et fixtures de helper réutilisés sont inchangés par rapport à la
vérification du rapport142. Ce résultat ne relance pas ou ne transforme pas les
56/54 tests historiques en nouvelle campagne ; leurs preuves restent attribuées
à leur candidat. Les compteurs précédents ne sont pas additionnés aux19.

La branche des tests Windows est intégrée et vérifiée localement. Sa réception
finale et la clôture de sa tâche attendent la couture G1 et l'audit fresh prévu
hors board d'implémentation. Le guide conservera ses gaps jusqu'à cette réception.
La migration n'est pas une preuve de fonctionnement natif Windows/Linux ou GPU.

## Identités, commandes et preuves

- Auteur pool-40, candidat `8dcd032d4e4e27a059dcbf9275c30ad3e3d64e71`, base040824c.
- Intégration `a1307735af91a7758030a74c6f9a46216fd45569`, exécutant coordinateur.
- Préflight depuis les blobs Git du dépôt canonique uniquement :
  `out/verification-synthetic/windows-migration-preflight-2026-10-30T12-30-37-125Z/`.
  Aucun nouvel accès au worktree refusé à Context Mode ; pas de contournement
  de ce refus ou de modification de permissions.
- Exécution : `out/verification-synthetic/windows-migration-integrated-2026-10-30T12-31-46-914Z/`.
  Manifestes : commandes exactes, cwd, candidat, hashes avant/après et journaux.
- Cwd tests : `scripts/tests/verification_windows`. Python3.13 `-B -m unittest -v`
  sur EffectiveWindowsEnvironmentTests et le seul test WindowsRealHelperBarrierTests ;
  WindowsDispatchMigrationTests ; WindowsWrapperTests et la seule méthode native
  test_real_child_wrapper_to_real_binding_records_exact_start_info.

G2 utilise uniquement une factory mémoire ; G4 et StartInfo des fragments PowerShell
relus sous effets simulés ; quoting/code17 un enfant Python factice fini possédé.
Aucun helper complet, ancienne fixture dispatch autonome, cmd/vcvars, Conan/CMake/
CTest réel, suite moteur même CPU, OpenGL, rendu, benchmark, installation ou cloud
exécuté. Pas de découverte globale Windows ou test de jonction06.
