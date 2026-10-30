# Environnement effectif et migration Windows — mapping B accepté

Proposition `result_mvuxtter_008j_btze0z`, pool-40/build-engineer, lecture et Git
uniquement, worktree H0 `040824cff819b074bfe0738efb3e35f66973555f` propre.
Helper52e7330 et collecteur9aedc0a intégrés. PM/LLM développement brut/retenu73,
risque66, incertitude moyenne, politique inchangée. Aucune validation dynamique.

## Ownership après ratification lead

Seuls cinq fichiers : nouveau test_effective_windows_environment.py, test_wrappers.py,
test_dispatch.py, dispatch_fixture.ps1 et nouvelle note G2_G3_G4_MAPPING.md, tous
sous scripts/tests/verification_windows/. Aucun autre fichier historique, fixture
partagée, pin/admission, production, test Linux ou fichier de couture G1 modifié.

## G2 — environnement effectif et oracles

Réutiliser WindowsBindingFixture et evidence.prepare/build avec le seed mémoire
existant pour obtenir les vrais reçus, WindowsAdapter.describe_execution et vrai
SubprocessRunner(popen_factory mémoire). Ne pas hériter des classes de tests
concrètes ou de leur setUp interdisant SubprocessRunner.run. Support strictement
local : création/nettoyage du cas, parent fermé synthétique, attentes indépendantes,
factory et OwnedProcess mémoire, captures confinées au cas.

- Chaque route matérielle gl.float-routes-correctness pass-name/prepared-float :
  argv intégral et cwd indépendants, kwargs exacts cwd/env/stdin/stdout/stderr/shell,
  shell=False, stdin DEVNULL, captures distinctes ouvertes/possédées, un Process,
  attente finie et parent intégralement inchangé.
- Découverte puis exécution CTest gl.float-preparation/gl.float-lifetime : outil
  du cache inventorié, arbre/config, sélection ancrée, --no-tests=error,
  sérialisation et JUnit dans le bundle, identifiants distincts liés au reçu.
- CPU cpu.float-mirror : six variables de contexte ambiant supprimées, marqueur
  ordinaire et politique d'héritage des forçages logiciels conservés, parent intact.
- Attentes six champs issues explicitement du profil, BuildReceipt.run_id,
  plan.plan_sha256, request ID attendu par rôle/index, vendor/renderer demandés,
  jamais copie globale env_delta. Identités workload argv et env concordantes.
- Matériel sans forçage, chaque forçage isolé puis paire : absence des deux clés
  LIBGL_ALWAYS_SOFTWARE/GALLIUM_DRIVER, pas des valeurs vides/fausses. Windows
  opengl-software reste refusé par le test27 existant, pas nouveau fallback.
- Contre-exemples mémoire champ périmé, request ID divergent, forçage conservé,
  argv/cwd/captures erronés : l'oracle indépendant doit rejeter. Pas un RED produit.

Aucun programme enfant nouveau ou outil réel ; CTest est uniquement une description
reçue par la factory mémoire. Les tests du runner existants de codes/capture ne
sont pas dupliqués sans obligation précise.

## G3 — fermeture globale remplacée par admission fine

Dans WindowsRealHelperBarrierTests, remplacer seulement l'ancien test
test_real_helper_remains_closed_before_dispatch par
test_real_helper_admits_only_validated_prepare_build_before_effects. Vérifier
unicité/ordre WRAPPERS/NATIVE/BINDING/DISPATCH et root réel → flags → refus run →
payload → inputs → admission arbre build → dispatch. Conserver ContainsKey phase,
absence d'appels tooling/MSVC/contexte/process avant dispatch, legacy et pins.
Éviter l'ancien découpage qui confond les définitions avec des appels exécutés.
La structure n'est pas une preuve de comportement : maintenir les scénarios
fonctionnels phases/opt-in/flags/payload, mutations/extraction et sentinelles.

## G4 — obligations anciennes vers preuves conservées et compléments

F = HelperFunctionalAdmissionTests, H = HelperFunctionalHarnessTests,
A = WindowsAdapterBindingTests, N = NativeBindingSourceTests,
NH = NativeBindingHarnessTests, W = WindowsWrapperTests.

1. prepare_without_opt_in_refuses_before_effects → F.invalid_phases_opt_in_and_legacy_flags_have_zero_effects,
   assert_rejected/run_case : cause exacte, zéro événement, parent/payload intacts,
   absence de sentinelle legacy.
2. prepare_authorized_runs_conan_then_cmake_only → F.prepare_conan_before_fresh_cmake_profiles_trees_and_testing
   et prepare_reaches_real_binding. Complément CPU Release/Jobs1 dans test_dispatch :
   ordre, host/build profiles, -of/preset/arbre/toolchain, workloads ON/testing OFF,
   exclusions explicites --build ET --test-dir. Outils absolus admis, pas noms PATH.
3. build_only_selected_target_without_bootstrap_or_configure → F.build_is_one_selected_command,
   unprepared_build_refuses_before_tooling_environment_or_repair et
   missing_build_tooling_does_not_bootstrap_or_configure. Complément Release CPU
   unique Jobs1, argv exact, un enfant, aucun bootstrap/configure.
4. run_only_preserves_argument_boundaries → A.tests05/06/19, G2 argv factory,
   W quoting/code17. Run direct adaptateur/runner, helper-run rejeté.
5. child_failure_preserves_raw_code_and_stops_sequence → F.first_nonzero_stops_prepare_and_preserves_raw_data,
   N.capture_first_cause_survives_dispose_secondary. Complément premier Conan23
   (le test actuel couvre premier17/second23), code/bruts stderr, aucun configure.
6. environment_present_and_absent_restored_after_success → F.child_environment_and_restore_success_nonzero_exception_absent_empty,
   W.restore_removes_absent_and_retains_empty_values, N.context_map_copies_null_empty_and_private_lifetime,
   G2 parent synthétique intact ; filtre source/canaris, pas ancien env non filtré.
7. environment_present_and_absent_restored_after_failure → mêmes frontières en
   nonzero17/exception et F.msvc_exception_and_wrong_metadata_restore_before_child.
8. selected_phases_never_fall_through_to_legacy_sentinel → prepare/build F nominaux,
   refus run F, sentinelle commune run_case et G3 ordre. Helper-run favorable est
   une attente obsolète, la garantie anti-fallthrough reste obligatoire.
9. fixture_refuses_unapproved_effects_before_evaluation → H.mutated_effects_are_rejected_before_evaluation,
   audit_never_evaluates_an_allowlisted_effect, marker_population_root_and_pin_mutants_are_harness_errors,
   NH.unapproved_effect_source_is_never_evaluated. Complément mutant exact
   [System.IO.File]::WriteAllText en mode audit uniquement, données parsées,
   aucun scriptblock construit/exécuté pour ce mutant.
10. fixture_can_record_fake_child_and_restore_map_on_exit → H.root_binding_and_all_fake_leaves_baseline,
    fake_child_exception_is_independently_observable, NH.memory_factory_capture_zero_and_17,
    W quoting/code17, N.real_child_wrapper_to_real_binding_records_exact_start_info.

Chaque nom abrégé ci-dessus doit être restitué par la note auteur avec son nom
exact test_*, fichier, assertions couvertes et commande nominative proposée. Un
inventaire visible accompagne le module migré, mais aucune simple assertion de
présence d'un nom ne constitue une preuve comportementale.

## Dépendance supplémentaire ratifiée avant édition

Le lead a recoupé wrapper_fixture.ps1:258–281 et test_wrappers.py:179–190 :
le mode compose extrait l'AST de Assert-FixtureDispatch de dispatch_fixture.ps1,
puis évalue l'ancien fragment DISPATCH avec les anciennes variables et helper-run.
Cette composition est obsolète ; la laisser cassée ne préserverait aucune garantie.

Migration supplémentaire explicitement AUTORISÉE : remplacer visiblement la
méthode W.test_local_composition_separates_phases_and_restores_environment et
inventorier ses sous-cas prepare0/build0/run0/prepare17 vers F nominaux et arrêt,
A+G2 run direct, F+W restauration et N exact StartInfo. Ne pas réintroduire helper-run,
étendre une allowlist ou modifier wrapper_fixture.ps1 pour faire accepter l'ancien
fragment. Aucun autre test wrapper touché, en particulier quoting et code17.

Conserver VERBATIM l'unique fonction Assert-FixtureDispatch dans dispatch_fixture.ps1
pour ce consommateur AST historique READONLY, sans l'étendre. Retirer l'ancien
programme autonome de qualification : invocation désormais refusée explicitement
avant lecture/évaluation de payload, avec renvoi à la note de migration. Parsing
du fichier et extraction de cette fonction doivent rester possibles. Pas archive
invisible, skip global ou code prétendant qualifier le dispatch actuel.

Remplacer le prototype test_dispatch.py par inventaire explicite et seuls compléments
manquants ci-dessus, dérivés de FunctionalFixtureCase sans méthodes de test,
jamais d'une classe concrète F/H/N/W. Aucun load_tests doublonnant les suites.
Ne pas transformer la méthode de composition retirée en un test tautologique ;
ses obligations sont réattribuées aux preuves comportementales nommées et exercées.

## Mapping inverse et réception

F/G3 → admission, phases/zéro effet/sentinelle ; F+compléments → ordre/cibles/codes ;
A/G2 → run direct/argv/env ; W/N → quoting/StartInfo ; H/NH → extraction/pins/audit/
baselines ; G1 → même témoin/collecte/réadmission, hors ownership B. Pas de preuve
de couture revendiquée depuis les seeds G2. Chaque remplacement sera sélectionné
une fois nominativement par le lead, aucune découverte Windows globale.

Sur production intégrée correcte : caractérisation et contre-exemples, pas faux RED
ancien prototype. Bug production reste rouge et revient au lead. Auteur non runner,
pas unittest/PS/import cible/faux enfant/moteur/outils/cloud. Relecture des programmes
avant exécution lead, logs/hashes/candidat/cwd/comptages/causes attribués et contrôle
du diff après shutdown/merge. Audit final fresh hors board attend G1 et B intégrés.
Contexte MCP : refus worktree extérieur déjà observé par auteur, ne pas réessayer
ou contourner ce refus ; signaler les contrôles indisponibles. Cela n'autorise
aucun nouvel accès ou changement de permissions. Qualification native NON_EXECUTE.
