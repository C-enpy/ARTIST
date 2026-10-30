# pool-40 (@build-engineer teammate)

Session: ses_e7347e5dfffeUx3aZMUwj1DYBO
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T12:11:10.281Z — msg_18cb89845001nRpO68Lvp2yY1N

Je prépare uniquement le plan et le mapping, sans édition ni exécution de tests. Le répertoire courant est `main` ; j’ai localisé le worktree `pool-40` sur la branche annoncée et vais vérifier ses sources à H0, sans modifier la connexion Serena partagée.

## assistant — 2026-10-30T12:12:04.171Z — msg_18cb96ac8001B44FYzTtjzLc45

Le worktree est propre à `040824c` (H0) ; QA §15 est commité à ce même HEAD, et les commits helper/collecteur sont ancêtres. Context Mode refuse la lecture directe de ce worktree hors de sa racine canonique. Je conserve cette limite et consulte les copies canoniques suivies à H0, sans toucher aux modifications locales de profils présentes sur `main`.

## assistant — 2026-10-30T12:16:01.303Z — msg_18cbd0912001hJX3fmGr443bf6

## Première remise — PLAN + MAPPING, sans édition

### État vérifié

- Worktree : `<USER_HOME>/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-40`.
- Branche : `ensemble-26c25b30-bb1c-40d8-93a3-11990b0fa78c`.
- HEAD : **`040824cff819b074bfe0738efb3e35f66973555f`**, H0 attendu.
- Worktree propre ; aucun changement effectué.
- QA §15 est commité à H0 ; helper `52e7330` et collecteur `9aedc0a` sont ancêtres vérifiés.
- Lus : QA §13–15, contrats Windows binding/preuves privées, rapports142–145 et `SOURCE_GREEN_READY.md:136–169`.
- CMake, presets, Conan et CI inspectés : intégration Conan2 conservée, installation avant CMake, générateur Ninja. **Aucune configuration ou compilation autorisée/exécutée.**
- PM/LLM conservé : complexité brute/retenue **73**, risque **66**, incertitude moyenne ; politique et modèle inchangés.

Serena est rattaché à ARTIST canonique, mais son serveur actif couvre seulement C++ : extraction symbolique Python indisponible. Analyse réalisée par lectures ciblées et recherches exactes. Context Mode refuse le worktree extérieur à sa racine ; les copies canoniques consultées étaient identiques à H0 pour les sources concernées.

## 1. Ownership proposé après GO

Uniquement :

1. Nouveau `scripts/tests/verification_windows/test_effective_windows_environment.py`.
2. `scripts/tests/verification_windows/test_wrappers.py`.
3. `scripts/tests/verification_windows/test_dispatch.py`.
4. `scripts/tests/verification_windows/dispatch_fixture.ps1`.
5. Nouvelle note `scripts/tests/verification_windows/G2_G3_G4_MAPPING.md`.

Production, pins, fixtures communes, tests Linux et fichiers A0/G1 restent **READONLY**.

## 2. G2 — environnement réellement remis à la factory

### Composition retenue

Réutiliser :

- `WindowsBindingFixture` ;
- `evidence.prepare/build` avec son seed mémoire existant, pour obtenir de vrais reçus ;
- `WindowsAdapter.describe_execution` ;
- le vrai `SubprocessRunner(popen_factory=…)`.

Ne pas reprendre le `setUp` de `WindowsAdapterBindingTests` : il interdit justement `SubprocessRunner.run`. Réutiliser seulement les données/helpers nécessaires ou leur séquence publique minimale, sans hériter des classes de tests concrètes.

### Petit support local nécessaire

Dans le nouveau fichier seulement :

- création/nettoyage de `WindowsBindingFixture` et obtention des reçus ;
- parent synthétique fermé ;
- attentes indépendantes de contexte et d’argv ;
- factory stricte et `OwnedProcess` mémoire ;
- confinement des captures temporaires sous le cas possédé.

Aucun nouveau programme enfant, module de fixture partagé ou backend.

### Cas proposés

**G2-01 — chaque route matérielle effective**

Scénario `gl.float-routes-correctness`, routes `pass-name` et `prepared-float`.

Pour chaque commande, observer à la factory :

- argv intégral et cwd attendus ;
- exactement les paramètres `cwd/env/stdin/stdout/stderr/shell` ;
- `shell=False`, `stdin=DEVNULL` ;
- deux captures distinctes, ouvertes et possédées ;
- un seul objet Process mémoire, attente finie ;
- parent synthétique intégralement inchangé.

**G2-02 — découverte et exécution CTest effectives**

Sélection réelle `gl.float-preparation`/`gl.float-lifetime` :

- découverte avant exécution ;
- outil provenant du cache inventorié ;
- arbre/configuration exacts ;
- sélection ancrée, `--no-tests=error`, sérialisation et JUnit sous le bundle ;
- factory mémoire uniquement : **CTest n’est jamais lancé**.

**G2-03 — CPU sans contexte ambiant**

`cpu.float-mirror` :

- suppression des six variables de contexte périmées ;
- conservation du marqueur ordinaire ;
- conservation de la politique actuelle d’héritage des forçages logiciels ;
- parent inchangé.

**G2-04 — contre-exemples discriminants des oracles**

Données mémoire volontairement incorrectes : champ de contexte périmé, request ID divergent, forçage conservé, argv/cwd/captures incorrects. L’oracle indépendant doit les rejeter. Ce sont des contrôles du test, **pas un RED attribué à la production**.

### Attentes indépendantes

Les six valeurs attendues seront construites explicitement depuis :

- profil demandé ;
- `BuildReceipt.run_id` ;
- `plan.plan_sha256` ;
- request ID attendu selon le rôle et l’index ;
- vendor/renderer demandés.

**Aucune construction de l’attendu global par copie d’`env_delta`.**

Pour les workloads, vérifier aussi les identités visibles dans argv et leur égalité avec l’environnement effectif. Pour CTest, vérifier les identifiants distincts découverte/exécution et leur rattachement au reçu.

Parents matériels : sans forçage, chaque forçage individuellement, puis les deux ensemble. Dans tous les cas matériels, absence exacte de `LIBGL_ALWAYS_SOFTWARE` et `GALLIUM_DRIVER`, pas valeurs vides/fausses.

Le refus Windows `opengl-software` reste couvert par `test_adapter_binding…test_27_windows_software_profile_and_asan_configuration_have_no_silent_fallback`. Aucun équivalent logiciel Windows ne sera ajouté.

## 3. G3 — remplacement de l’oracle global obsolète

Dans `WindowsRealHelperBarrierTests`, remplacer uniquement :

`test_real_helper_remains_closed_before_dispatch`

par :

`test_real_helper_admits_only_validated_prepare_build_before_effects`.

Oracle proposé :

- unicité et ordre des régions WRAPPERS/NATIVE/BINDING/DISPATCH ;
- racine réelle avant admission ;
- ordre **root → flags → refus run → payload → inputs → admission arbre build → dispatch** ;
- conservation du contrôle `PSBoundParameters.ContainsKey('VerificationPhase')` ;
- aucun appel tooling/MSVC/contexte/process avant dispatch ;
- aucun changement du helper, du legacy ou des pins.

L’ancienne assertion « phases non implémentées » est remplacée parce qu’elle contredit le helper intégré, pas pour masquer un défaut.

Cette preuve structurelle ne remplace pas :

- `HelperFunctionalAdmissionTests.test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects` ;
- `…test_invalid_payload_and_binding_have_zero_effects_and_no_repair` ;
- les contrôles mutations/extraction ;
- les sentinelles vérifiées par `FunctionalFixtureCase.run_case`.

## 4. G4 — mapping exact des dix anciennes méthodes

Abréviations :

- **F** : `test_helper_functional.HelperFunctionalAdmissionTests`.
- **H** : `test_helper_functional.HelperFunctionalHarnessTests`.
- **A** : `test_adapter_binding.WindowsAdapterBindingTests`.
- **N** : `test_native_binding.NativeBindingSourceTests`.
- **NH** : `test_native_binding.NativeBindingHarnessTests`.
- **W** : `test_wrappers.WindowsWrapperTests`.

### 1. `test_prepare_without_opt_in_refuses_before_effects`

Ancien : nonzero, événements vides, parent intact, aucune sentinelle legacy.

Remplacement :

- F.`test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects`.
- Assertions communes `assert_rejected/run_case` : cause attendue, événements vides, parent et payload inchangés, absence de fallback.

### 2. `test_prepare_authorized_runs_conan_then_cmake_only`

Ancien : Conan puis CMake seulement ; paramètres host/build Release ; `-of`, preset, arbre et toolchain exacts ; workloads ON, testing OFF ; aucun build/test.

Remplacement :

- F.`test_prepare_conan_before_fresh_cmake_profiles_trees_and_testing`.
- F.`test_prepare_reaches_real_binding`.

**Complément ciblé dans `test_dispatch.py`** : cas CPU Release/Jobs1 conservant explicitement les exclusions `--build` **et `--test-dir`**, ainsi que l’ordre et les paramètres historiques.

Les noms nus `conan/cmake` deviennent les chemins admis actuels vers `conan.exe/cmake.exe` : aucune recherche PATH implicite.

### 3. `test_build_only_selected_target_without_bootstrap_or_configure`

Ancien : une seule commande CMake, cible CPU seule, `--parallel 1`, aucun bootstrap/configure.

Remplacement :

- F.`test_build_is_one_selected_command`.
- F.`test_unprepared_build_refuses_before_tooling_environment_or_repair`.
- F.`test_missing_build_tooling_does_not_bootstrap_or_configure`.

Complément du cas historique : **Release, cible CPU unique, Jobs1**, argv exact et un seul enfant.

### 4. `test_run_only_preserves_argument_boundaries`

Ancien : un enfant, programme et arguments égaux au payload.

Remplacement :

- A.`test_05_cpu_workload_uses_admitted_nested_mapping_and_exact_shared_argv`.
- A.`test_06_gl_routes_use_shared_visible_options_and_repeatable_unique_request_ids`.
- A.`test_19_actual_cpu_execution_is_direct_and_collected_by_common_runner_only`.
- G2 : argv réellement reçu par la factory.
- W : quoting et code17 conservés.

Le payload helper-run n’est plus l’autorité : **run reste direct adapter → runner**.

### 5. `test_child_failure_preserves_raw_code_and_stops_sequence`

Ancien : premier enfant17 ou23, code brut conservé, aucun CMake après échec Conan.

Remplacement :

- F.`test_first_nonzero_stops_prepare_and_preserves_raw_data`.
- N.`test_capture_first_cause_survives_dispose_secondary`.

**Complément requis** : le test F actuel couvre premier17 et second23, mais pas premier23. Ajouter dans `test_dispatch.py` le cas **Conan23 → aucun configure**, avec code brut et stderr conservés.

### 6. `test_environment_present_and_absent_restored_after_success`

Ancien : parents présent/vide/valeurs vides, environnement enfant attendu, parent restauré.

Remplacement :

- F.`test_child_environment_and_restore_success_nonzero_exception_absent_empty`.
- W.`test_restore_removes_absent_and_retains_empty_values`.
- N.`test_context_map_copies_null_empty_and_private_lifetime`.
- G2 : parent OS entièrement synthétique inchangé.

L’ancien environnement enfant non filtré ne sera pas reproduit : le contrat actuel impose le filtre réel et l’exclusion des canaris/secrets.

### 7. `test_environment_present_and_absent_restored_after_failure`

Même mapping que6, branches nonzero17 et exception, plus :

- F.`test_msvc_exception_and_wrong_metadata_restore_before_child`.

Les assertions distinguent restauration, absence/vide et première cause.

### 8. `test_selected_phases_never_fall_through_to_legacy_sentinel`

Ancien : prepare/build/run ne tombent pas dans le legacy.

Remplacement :

- prepare/build : F nominaux ;
- run : F admission, refus explicite avant effet ;
- `FunctionalFixtureCase.run_case` contrôle systématiquement la sentinelle ;
- G3 contrôle l’ordre source.

**Run admis par le helper est rejeté comme ancienne attente obsolète**, sans supprimer la garantie anti-fallthrough.

### 9. `test_fixture_refuses_unapproved_effects_before_evaluation`

Ancien : sept mutants, code96, erreur harness, aucun événement, parent intact.

Remplacement :

- H.`test_mutated_effects_are_rejected_before_evaluation`.
- H.`test_audit_never_evaluates_an_allowlisted_effect`.
- H.`test_marker_population_root_and_pin_mutants_are_harness_errors`.
- NH.`test_unapproved_effect_source_is_never_evaluated`.

**Complément requis** : le mutant exact `[System.IO.File]::WriteAllText…` n’est pas dans la liste H actuelle. Le conserver dans un nouveau test de `test_dispatch.py`, exclusivement via `mode="audit"` : ce chemin parse les données et ne crée aucun scriptblock pour le mutant.

### 10. `test_fixture_can_record_fake_child_and_restore_map_on_exit`

Ancien : baseline indépendante0/17, un enfant mémoire, arguments littéraux, map enfant et restauration.

Remplacement :

- H.`test_root_binding_and_all_fake_leaves_baseline`.
- H.`test_fake_child_exception_is_independently_observable`.
- NH.`test_memory_factory_capture_zero_and_17`.
- W quoting/code17.
- N.`test_real_child_wrapper_to_real_binding_records_exact_start_info`.

Les anciens noms/chaînes de fixture ne constituent pas une exigence publique ; préservation des frontières d’arguments et des issues reste obligatoire.

## 5. Dépendance supplémentaire découverte — à ratifier dans le mapping

`wrapper_fixture.ps1:258–281` extrait **`Assert-FixtureDispatch` depuis `dispatch_fixture.ps1`**. Une suppression ou un remplacement intégral de cette fixture casserait ce consommateur READONLY.

Par ailleurs, W.`test_local_composition_separates_phases_and_restores_environment` fournit le **DISPATCH actuel** à l’ancien audit ; cette composition est elle aussi obsolète. Les lectures montrent notamment que `$verificationRequested` n’appartient pas à l’ancienne allowlist.

### Proposition bornée

- Conserver **verbatim** `Assert-FixtureDispatch` dans `dispatch_fixture.ps1`, pour son consommateur historique.
- Retirer son ancien programme exécutable de qualification et rendre son invocation autonome explicitement refusée, avec renvoi à la note de migration ; aucun payload évalué.
- Migrer visiblement aussi l’ancienne méthode de composition W :
  - prepare/build/arrêt → F ;
  - run direct → A + G2 ;
  - restauration → F + wrappers dédiés ;
  - wrapper réel → N exact StartInfo.
- Conserver intacts les autres tests wrappers, spécialement quoting et rawcode17.
- Aucun nouveau snippet simulant helper-run, aucune extension d’allowlist.

**Cette onzième migration doit être acceptée explicitement par le lead dans le mapping avant édition.** Sans elle, laisser le test de composition cassé ou retirer sa dépendance silencieusement rendrait G4 incomplet.

## 6. Organisation évitant doublons et preuves tautologiques

Proposition : remplacer le module prototype par :

- un inventaire visible des dix anciennes méthodes et de leurs remplaçants ;
- seulement les compléments comportementaux réellement manquants : cas historique Release/Jobs1, premier code23, mutant FileWriteAllText ;
- une classe dérivée du **socle sans méthodes de test** `FunctionalFixtureCase`, jamais d’une classe concrète F/H/N/W ;
- aucun `load_tests` réexécutant les remplaçants ;
- aucun test affirmant seulement qu’un nom de méthode existe ;
- aucun skip global ou archive invisible.

La note B portera le mapping inverse :

- F/G3 → admission, phases, zéro effet, sentinelle ;
- F + compléments → ordre, cibles et codes ;
- A/G2 → run direct, argv et environnement effectif ;
- W/N → quoting et StartInfo ;
- H/NH → extraction, pins, audit et baselines ;
- G1 → couture du témoin, explicitement hors ownership B.

Le lead sélectionnera chaque méthode existante **une seule fois**, nominativement. Aucune découverte Windows globale.

## 7. Frontières, preuves et arrêt

- G2 : zéro processus réel, binaires inertes, captures possédées, aucune lecture/dump du véritable environnement dans les assertions.
- Tests PowerShell historiques : seulement sélection nominative relue par le lead ; interpréteur fixe et programmes factices existants, jamais helper complet/outils natifs.
- Aucune modification des pins ni des allowlists communes.
- AST sans import seulement après GO édition.
- Aucun faux RED fondé sur le prototype obsolète. Un défaut de production révélé reste rouge et retourne au lead.

### Matrice réellement exercée

- Windows : **inspection statique et Git uniquement**.
- Tests synthétiques G2/G3/G4 : **NON_EXECUTE**.
- Configuration/build/CTest moteur : **NON_EXECUTE**.
- Qualification native Windows/Linux, OpenGL et performances : **NON_EXECUTE**.

Commandes finales de contrôle, exécutées dans le worktree indiqué : `git rev-parse HEAD`, `git status --porcelain=v1`, `git diff --stat`. HEAD H0, état propre, diff vide.

**Remise textuelle uniquement : aucun fichier, fixture, patch ou note écrit. Le lead peut persister/committer ce mapping, ratifier la migration supplémentaire identifiée, puis donner le GO tests-only. Arrêt à cette première remise.**

## assistant — 2026-10-30T12:20:04.267Z — msg_18cc0be24001RJ3lNIPSZ61M1L

GO reçu. Je vais modifier uniquement les cinq fichiers ratifiés, sans toucher au HEAD source ni aux fixtures communes. Je ne lancerai aucun test, import cible ou programme factice ; les contrôles impossibles via Context Mode resteront signalés pour le lead.

## assistant — 2026-10-30T12:24:22.296Z — msg_18cc4ae13001okzrh60hghRTFH

Les changements sont limités aux fichiers ratifiés. Le prototype dispatch n’est plus exécutable ; sa fonction d’audit reste en place pour l’extraction AST historique. Les nouveaux tests G2 comparent l’environnement reçu par la factory à des attentes issues du plan/reçu, sans les recopier depuis `env_delta`. Je termine l’inventaire bidirectionnel et les commandes futures pour le lead.

## assistant — 2026-10-30T12:28:18.614Z — msg_18cc84931001chsE7zNTMqOUVH

La relecture a corrigé un contre-exemple non discriminant : le profil matériel ambiant était identique au profil attendu. Le mutant utilise désormais explicitement `cpu-only`. Le diff indexé ne contient que les cinq fichiers autorisés et passe `git diff --cached --check`. Je crée le commit candidat ; AST et tests restent NON_EXECUTE.

## assistant — 2026-10-30T12:28:47.900Z — msg_18cc8bb96001O2ZMHriKyNqwdA

## TESTS_READY — candidat pour revue du lead

**Commit : `8dcd032d4e4e27a059dcbf9275c30ad3e3d64e71`**  
Parent direct : H0 `040824cff819b074bfe0738efb3e35f66973555f`. Worktree propre ; aucun merge de main.

### Livraison

Exactement les cinq fichiers autorisés, présents sur disque :

- `test_effective_windows_environment.py` : quatre tests G2, vrai adaptateur/runner et factory mémoire stricte ; attentes indépendantes d’`env_delta`, parents entièrement synthétiques, routes matérielles/CTest/CPU et contre-exemples.
- `test_wrappers.py` : oracle G3 d’admission fine ; composition obsolète migrée visiblement. Autres tests wrappers, quoting et code17 inchangés.
- `test_dispatch.py` : inventaire historique et quatre compléments G4 — prepare Release/Jobs1, build CPU/Jobs1, premier Conan23, mutant FileWriteAllText audit-only.
- `dispatch_fixture.ps1` : programme autonome explicitement refusé avant lecture du payload ; fonction `Assert-FixtureDispatch` conservée sans modification d’allowlist.
- `G2_G3_G4_MAPPING.md` : mapping bidirectionnel des dix méthodes et des quatre sous-cas de composition, frontières et commandes futures.

Production, pins, fixtures communes, Linux et G1 inchangés. Aucun héritage de classe concrète, `load_tests`, skip global ou helper-run ajouté.

### Contrôles réellement exercés

- **Windows / Git et relecture uniquement** : périmètre de cinq fichiers vérifié ; `git diff --cached --check` sans erreur.
- **AST Python/PowerShell : NON_EXECUTE** — refus Context Mode hors racine conservé, sans nouvelle tentative ni contournement.
- **Tests G2/G3/G4 et non-régression : NON_EXECUTE** par l’auteur.
- **Build, moteur, rendu, qualification native Windows/Linux et performance : NON_EXECUTE**.

Aucune première cause runtime disponible ; aucun GREEN revendiqué.

### Commandes futures — lead seulement

Cwd : `<candidat-intégré>/scripts/tests/verification_windows`, après relecture et contrôle syntaxique/extraction AST :

```powershell
C:/Python313/python.exe -B -m unittest -v test_effective_windows_environment.EffectiveWindowsEnvironmentTests test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_admits_only_validated_prepare_build_before_effects
C:/Python313/python.exe -B -m unittest -v test_dispatch.WindowsDispatchMigrationTests
```

La note contient également la sélection nominative quoting/code17/StartInfo et les preuves existantes réutilisables, sans double exécution. Pas de découverte Windows globale ni test06 jonction.

**À reprendre par le lead :** revue, exécution autorisée, intégration puis audit fresh après intégration de G1 et B. Tout défaut de production révélé reste rouge, hors correction autonome.