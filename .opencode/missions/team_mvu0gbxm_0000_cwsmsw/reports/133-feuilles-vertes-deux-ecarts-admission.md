# 133 — Feuilles vertes, deux écarts d'admission à corriger

La correction nominative du harnais est relue. Sur le candidat admis, les **six
baselines natives et vingt tests source passent**, puis les **sept baselines
haut niveau passent**. Les onze tests d'admission donnent ensuite **deux échecs**,
aucune erreur ou skip. Les empreintes restent stables ; pas de GREEN global.

Le premier cas accepte un argument inconnu au lieu de refuser avant effets.
Le second tente trois écritures de restauration de la map enfant alors que
l'obtention du tooling a échoué avant toute application d'environnement.
Ces deux écarts sont transmis au propriétaire pour correction de l'entrée et
du suivi d'étape, sans modifier les assertions ou cacher les événements dans
les doubles. Aucun défaut de mutation de l'environnement OS réel n'est démontré.

Le refus des arguments doit rester causal sans imprimer de valeurs sensibles.
La restauration reste nécessaire après application, y compris partielle ; elle
ne doit pas être simplement supprimée pour faire passer le second test.
La source modifiée et ses nouveaux hashes seront relus avant réadmission.

Les vingt tests source prouvent cette composition sous doubles mémoire, pas
une exécution native des outils. Le helper n'est pas encore intégré. La revue
indépendante et la couture avec le collecteur restent à effectuer.

## Preuves

- Candidat `8f6fce2d9d521713aefd63056c76f090ebef1d7f`, source production27dd1ed ;
  auteur pool-31, exécutant coordinateur.
- Commandes nominatives `C:/Python313/python.exe -B -m unittest -v` :
  NativeBindingHarnessTests, NativeBindingSourceTests (module test_native_binding),
  puis HelperFunctionalHarnessTests et HelperFunctionalAdmissionTests
  (module test_helper_functional), cwd tests Windows du worktree auteur.
- Journaux/manifeste et chemins des cas bruts : `out/verification-synthetic/windows-helper-binding-green-v2-2026-10-30T10-54-43-645Z/`.

Seul PowerShell test-owned avec source admise et effets simulés a été lancé.
Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté ; qualification native NON_EXECUTE.
