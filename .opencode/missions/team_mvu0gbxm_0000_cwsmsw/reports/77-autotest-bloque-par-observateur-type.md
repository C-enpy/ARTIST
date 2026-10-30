# 77 — L'auto-test bloque sur le getter typé, pas sur le produit

## Résultat

Après revue du correctif d'observation et des getters, le coordinateur lance l'auto-test seul. Il échoue par `HARNESS/96` avant tout appel produit : l'instantané indique une clé présente et vide, conforme à l'environnement factice initial, tandis que le getter typé la déclare absente. Le même désaccord est enregistré après l'arrêt du harnais.

Les treize autres méthodes et les 37 tests précédents ne sont pas lancés. Empreintes stables ; aucune assertion produit modifiée. Ce résultat est une limite de l'observateur typé dans cette forme d'appel, pas un RED produit ni une preuve de mutation de l'environnement.

## Correction bornée de l'observateur

Le coordinateur retient les snapshots Process neufs et l'extraction exacte des quatre marqueurs connus par présence/valeur comme oracle. La précondition initiale doit rester conforme aux données assainies ; la comparaison avant/après et la distinction vide/absent restent exigées par les assertions existantes.

Les lectures assignées et typées restent visibles comme diagnostics, mais leur seul désaccord ne doit plus provoquer une panne artificielle du harnais. Snapshot impossible, type invalide ou marqueur initial réellement absent/inexact restent des préconditions en échec. Un démonstrateur sur maps factices doit confirmer que l'observateur distingue vide/absent et détecte un changement de valeur, sans mutation OS.

Aucune nouvelle campagne de diagnostic, restauration d'environnement, installation ou PInvoke n'est demandée. Le code produit et son pin restent inchangés ; le prochain diff du harnais sera relu avant auto-test.

## Preuves et limites

- HEAD vérifié `a5628d462e07f4f804610aa14d22586f9e17ee92`, source/fixture/pin identiques au candidat annoncé `6a19ad7e58ae78830d08de0bdce7c748ba269210`.
- Commande : Python unittest avec le seul nom complet `test_native_contracts.WindowsNativeContractTests.test_14_review_pin_and_independent_native_fixture_selfcheck`, worktree Windows ; commande exacte dans le manifeste.
- Logs, empreintes et résultats : `out/verification-synthetic/windows-observation-green-2026-10-30T04-23-29-066Z/` ; données de cas dans `windows-native-fixtures/20261030T042329Z-d79191939ef24e65a5c492e3f8f398a5` du worktree.

Aucun nouveau GREEN Windows ; pas de helper complet, MSVC, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté. Cas reparse toujours NON_EXECUTE.
