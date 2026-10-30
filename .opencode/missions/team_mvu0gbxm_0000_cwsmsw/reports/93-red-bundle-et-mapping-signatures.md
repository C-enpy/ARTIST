# 93 — RED du bundle observé, signatures de doubles adaptées sans affaiblir les oracles

Le coordinateur a relu les 22 méthodes, la note de remise et le confinement. L'auteur avait livré deux ajouts non commités ; le lead a vérifié leur identité et l'absence d'autres modifications puis persisté ce candidat dans le worktree auteur avant exécution. Aucun code de production ou ancien test n'est modifié à ce stade.

Résultat : **22 tests, 25 échecs d'assertion avec sous-cas, aucune erreur et un SKIP**. Les échecs portent sur l'ancienne signature, le bundle non transmis, le transport absent et les refus/interruptions non raccordés. Le lien symbolique est non exécuté pour WinError1314 ; aucun privilège ni substitution n'est ajouté. Empreintes stables. Les assertions aval masquées par l'absence de transport ne sont pas prétendues toutes discriminées par ce premier RED.

Le GREEN minimal est autorisé sur le protocole de préparation et la matérialisation privée du sink. Les schémas, runner, rapport, adaptateurs Windows/Linux, scripts moteur et C++ restent hors delta. Description pure et validation des permissions précèdent les effets de transport ; aucune résolution de dépendance implicite n'est permise.

Trois adaptations d'anciens tests sont ratifiées, après vérification de leurs usages : ajouter le keyword à `AdapterDouble.describe_preparation`, à l'unique lambda de préparation de `test_runtime_contract.py`, et à l'attente de signature dans `test_plan_contract.py`. Aucune autre assertion ou sémantique des 172 tests acquis n'est modifiée. Les 22 nouvelles méthodes restent intactes ; l'auteur ne lance aucun test.

Le candidat GREEN sera relu puis exécuté par le lead et soumis à revue indépendante fresh. Les consommateurs attendent encore l'entrée core réellement intégrée ; ce RED ne prouve pas le futur helper Windows/Linux.

## Preuves

- Auteur : pool-20 ; persistance du candidat et runner réels : coordinateur.
- Entrée QA `222a34164ec91fde54dfdafc288004ea21c99f75` ; candidat RED persisté `2c55ca5b99c08bf0dc92f2b05a38a1085bef96b3`.
- Test : `scripts/tests/verification_core/test_bundle_preparation_contract.py`, SHA256 `2beee06dd123d25374a25ce1aee72741dcce71b397dd26894ab36117ae315b1b` ; note `BUNDLE_PREPARATION_RED_READY.md` à la racine de ce worktree.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_bundle_preparation_contract.py -v`, worktree pool-20.
- Logs et empreintes : `out/verification-synthetic/bundle-preparation-red-2026-10-30T05-55-03-120Z/`.

Aucun processus réel, CTest réel, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
