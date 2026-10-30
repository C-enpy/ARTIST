# Bundle preparation — TESTS_RED_READY, auteur pool-20

## Admission et état

- Worktree : `ensemble-2dc31e0a-cwsmsw-pool-20`, branche
  `ensemble-2b6e80fe-bf1b-498d-9574-dd0d31dc7d77`.
- Entrée vérifiée par Git avant écriture :
  `222a34164ec91fde54dfdafc288004ea21c99f75`, arbre propre.
- Contrats effectivement lus : `windows-adapter-binding-v1.md`,
  `qa-plan.md` §11, `qa-bundle-inputs.md`, rapport
  `92-qa-bundle-ratifiee-avant-auteur.md`.
- Autorisation reçue : **tests RED seulement**. Aucun changement de production,
  des adaptateurs, du runner, des modèles, des anciens tests ou des scripts de build.
  Aucune exécution de test, import du nouveau module, campagne, installation ou réseau.
- Le fichier nouveau est
  `scripts/tests/verification_core/test_bundle_preparation_contract.py`.
  Cette note est le seul autre fichier ajouté.

**RED non exécuté, GREEN non autorisé.** Les 22 méthodes sont un comptage AST,
pas un nombre de tests découverts/exécutés. Aucune ancienne preuve n'est attribuée
au nouveau delta.

## Plan local et mapping avant édition des anciens tests

La tranche s'appuie sur les vraies interfaces `evidence.prepare/build`,
`BundleSink` et les modèles actuels. Le nouveau double accepte déjà
`bundle_root=None` : l'absence actuelle du keyword produit un oracle de contrat,
pas une erreur accidentelle de fixture. Le fake runner ne crée ni le payload,
ni son empreinte, ni les logs privés. Leur observation ne répète donc pas le mock.

Après RED causal et autorisation GREEN seulement, adaptations proposées :

1. `scripts/tests/verification_core/runtime_fixtures.py` : uniquement ajouter
   `*, bundle_root=None` à `AdapterDouble.describe_preparation` ; comportement,
   CommandSpec et assertions métier inchangés.
2. `scripts/tests/verification_core/test_runtime_contract.py` : même ajout au
   paramétrage de la lambda `fixture.adapter.describe_preparation` (ligne240
   de l'entrée). Ne pas modifier l'oracle d'interdiction de phase/résolution.
3. `scripts/tests/verification_core/test_plan_contract.py` : l'unique attente
   `inspect.signature(PlatformAdapter.describe_preparation)` devient
   `('self', 'plan', 'bundle_root')`. Les attentes build/execution/environment
   et toutes assertions métier restent inchangées.

Ces trois fichiers restent **intacts au RED**. Les adaptateurs Windows/Linux
et leurs tests ne sont pas repris ici : leurs consommateurs attendent le core
réellement intégré. Pas de fallback TypeError pour maintenir une ancienne API.

## Registre B1–B6 ↔ T1–T7

- **T1 / B1, B6, I3** : signature obligatoire keyword-only ; transmission du
  chemin absolu de chaque bundle sous lease Windows et Linux ; deux bundles du
  même plan restent distincts ; ancien callable et TypeError interne ne donnent
  ni deuxième appel sans keyword, ni lancement.
- **T2 / B2** : observation à l'entrée du runner ; quatre champs uniques exacts,
  build root attendu, union `OwnedZulu, SharedTarget, OwnedAlpha` ordonnée et
  dédupliquée sur deux scénarios ; streams observés fermés, SHA256 et artefact
  déjà inscrits au manifeste en mémoire ; logs prepare présents, logs build absents.
- **T3 / B2, B3** : build lit les octets du PreparationReceipt ; aucun open
  d'écriture du payload, contenu/mtime/digest conservés ; mutation, disparition,
  reçu étranger/incomplet/échoué refusés avant adaptateur et runner, sans réparation.
- **T4 / B2, B4** : fichier ou répertoire préexistant à la place du payload ou des
  logs prepare/build ; refus avant commande et contenu préexistant préservé.
  Symlink du payload vers `raw/ctest-population.json` : refus, population intacte.
  Ce dernier cas fait SKIP explicitement si la fixture symlink est indisponible,
  sans élévation, jonction ou substitution.
- **T5 / B1, B4, B6** : opt-in absent interdit le bootstrap Windows déclaré et sa
  matérialisation ; double Linux générique sans bootstrap toujours admis sans
  opt-in ; CLI list/plan répétables, sans création de sink/adapter/runner ni effet.
- **T6 / B2, B5, I2** : interruptions injectées dans le writer JSON existant
  (préfixe partiel, après fermeture avant hash), puis après `artifact` ; aucun
  enfant, code130 et reçu interrompu, preuve partielle conservée, locks libérés,
  nouvelle tentative dans un autre bundle. Retours factices nonzero, timeout et
  interruption en prepare/build : codes, flags, stdout/stderr partiels et trace
  unique conservés par la capture commune.
- **T7 / B6** : champs Request/CommandSpec/les trois receipts inchangés,
  aller-retour de modèles/reçus réels et signature build inchangée.

Chaque méthode est rattachée à une obligation dans son nom/docstring. Les
contrats anciens de pureté et de prédécesseur sont réutilisés sans les affaiblir.
Le contrôle des descriptions de production Windows/Linux reste au raccordement
de leurs propriétaires : tester une description du double ne le prouverait pas.

## Confinement et ownership

Les sorties appartiennent à des `TemporaryDirectory` dont le préfixe comporte
des espaces ; aucune sortie partagée, fixture native ou arbre de build réel.
Les faux CMakeCache/exécutables sont uniquement des octets inertes sous ces
répertoires. `RevisionDouble` évite tout enfant Git dans les tests.

`OwnedBundleRunner.run` est un appel Python injecté, pas un programme. Il refuse
tout outil autre que le vocabulaire privé et toute phase hors prepare/build.
Des sentinelles Popen/run/check_output/os.system/socket/SubprocessRunner.run
enregistrent les tentatives, même si le core attrape leur assertion ; la cleanup
du test vérifie que cette liste est vide. Le test consultatif réutilise en plus
`forbid_effects` pour surveiller imports workloads, créations, écritures et enfants.

Le sink reste l'owner attendu du transport sous lease ; l'adaptateur emprunte
uniquement le chemin. Aucune nouvelle API de test, setter ou modèle de transport.
La fermeture est observée sur les streams Path.open du code actuel ; ce n'est
pas une preuve générale concernant tous les descripteurs système possibles.
Les interruptions sont bornées aux coutures existantes, pas une simulation de
power-loss, de concurrence hostile ou d'interruptions répétées.

## Remise au runner lead

Depuis la racine du worktree, après relecture et persistance du candidat :

```text
python -B -m unittest discover -s scripts/tests/verification_core -p 'test_bundle_preparation_contract.py' -v
```

Attendu au RED : échecs de contrat sur la signature/valeur transmise et sur
l'absence du payload ou des refus exclusifs, pas un import défectueux, un outil
absent ou un FileNotFoundError de fixture. Des tests de garanties existantes
peuvent déjà passer ; le cas symlink peut être SKIP selon les droits disponibles.
Ne pas annoncer de comptage exécuté avant le journal du runner.

Après RED causal, GREEN explicitement autorisé et relecture du delta final :

```text
python -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Le lead conserve interpréteur/cwd/HEAD/diff/empreintes, logs bruts, comptages,
skips et motif causal. Les autres suites synthétiques pertinentes et la revue
indépendante restent décidées/exécutées par le coordinateur. Aucune commande
moteur, CMake, CTest, Conan, vcvars, GL, rendu, benchmark ou cloud n'est proposée.

## Vérifications réellement faites et limites

- Contrôle Git de HEAD et de l'arbre avant édition ; lecture des contrats et
  callers exacts ; inspection statique des méthodes prepare/build/lease/artifact.
- AST du nouveau fichier avec la stdlib seulement, sans import/exec du module :
  syntaxe acceptée, 22 méthodes `test_*`. Contrôle à rejouer sur le fichier final
  et empreinte à joindre à la restitution.
- Exécution auteur : **0 test**, aucun RED/GREEN démontré, aucun programme factice
  lancé. Windows/Linux natifs, moteur, C++ et GPU : **NON_EXECUTE**.
- Les témoins de streams et l'injection dans `_write_json`/`artifact` sont des
  coutures privées bornées du code connu, pas un nouveau contrat public.
- Les 172 tests historiques n'ont pas été modifiés ni rejoués par l'auteur.
