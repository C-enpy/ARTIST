# G2/G3/G4 — TESTS_READY, migration explicite et qualification à exécuter

## Identité et autorisation

- Source H0 : `040824cff819b074bfe0738efb3e35f66973555f`, QA §13–15 commité.
- Producteurs déjà intégrés : helper `52e7330`, collecteur `9aedc0a`.
- Plan textuel auteur : `result_mvuxtter_008j_btze0z`, relu/ratifié par le lead.
- Autorisation d'édition communiquée par le lead après commit canonique
  `50dc58763b563d03dac6e7bcdf3658925d33b11e` :
  `.opencode/evidence/mission-uniform-pipeline/windows-test-migration-plan.md`,
  QA §15 et rapport148. Pas de merge de main dans le worktree auteur.
- PM/LLM : complexité brute/retenue73, risque66, incertitude moyenne,
  politique/grille/modèle inchangés. Auteur NONRUNNER.

Les cinq fichiers de cette tranche sont ce document, le nouveau
`test_effective_windows_environment.py`, `test_wrappers.py`, `test_dispatch.py`
et `dispatch_fixture.ps1`. Production, pins, fixtures communes, tests Linux,
`wrapper_fixture.ps1` et branche G1 restent READONLY.

**TESTS_READY ne signifie pas PASS.** Aucun import cible, unittest, PowerShell,
programme enfant factice, outil natif, moteur, rendu, benchmark, installation,
workflow ou cloud n'a été exécuté par l'auteur. AST Python/PowerShell non exécuté :
Context Mode refuse le worktree extérieur à sa racine canonique ; pas de nouvelle
tentative ni contournement par un autre lanceur. Le lead doit relire le candidat
et les programmes avant toute exécution nominative, conserver HEAD/diff/hashes,
cwd, commandes, journaux bruts, comptages et causes, puis vérifier le diff intégré.

## G2 — composition réelle, effets mémoire

`EffectiveWindowsEnvironmentTests` utilise `WindowsBindingFixture` inchangée,
les vrais receipts `evidence.prepare/build` sur le seed mémoire existant, le vrai
`WindowsAdapter` et le vrai `SubprocessRunner` avec factory injectée. Aucun héritage
d'une classe concrète de tests, aucun double remplaçant le runner commun.

Les attentes argv sont assemblées indépendamment du builder de production depuis
le plan et les mappings du receipt. Les six attentes de contexte sont explicites :
profil demandé, run_id du receipt, hash du plan, request_id rôle/index, vendor et
renderer demandés. Ni `command.env_delta`, ni l'environnement ambiant ne servent
à fabriquer l'attendu. Le delta est lui-même comparé à cet attendu.

- `test_01_hardware_each_route_has_effective_independent_context` : routes
  `pass-name` et `prepared-float`, identités visibles liées à argv.
- `test_02_hardware_ctest_discovery_and_execution_use_distinct_bound_contexts` :
  cache du même arbre inventorié, CTest déclaré, découverte avant exécution,
  configuration/sélection/JUnit exacts, request IDs distincts.
- `test_03_cpu_clears_stale_context_without_changing_forcing_inheritance` :
  six contextes périmés retirés, marqueur ordinaire et valeur vide conservés,
  héritage historique des forçages inchangé, pas de nouvelle policy CPU.
- `test_04_factory_oracle_rejects_independent_memory_counterexamples` : contrôle
  mémoire valide puis chacun des six champs périmé/absent, forçages conservés
  ou remplacés par vide, mauvais cwd/stdin/shell, captures confondues/absentes,
  et argv altéré. Le même oracle utilisé à la factory doit rejeter ces mutants.
  Aucun mutant ne lance de processus et aucun n'est annoncé comme RED production.

Chaque commande est observée avec les quatre parents entièrement synthétiques :
aucun forçage, LIBGL_ALWAYS_SOFTWARE seul, GALLIUM_DRIVER seul, les deux ensemble.
Les six anciennes valeurs ambiantes sont également synthétiques. Hardware exige
l'**absence** des deux clés de forçage, pas une valeur false/0/vide de remplacement.
La comparaison porte sur toute la map effective et tout le parent synthétique.
Aucun snapshot réel du parent n'entre dans les assertions ou journaux des tests.

La factory exige argv/cwd exacts, kwargs fermés, shell=False, stdin=DEVNULL,
deux fichiers de capture distincts ouverts sous le répertoire possédé, un seul
Process mémoire et une attente finie exacte. Les octets synthétiques sont écrits
sur ces seules captures, relus par le vrai runner et comparés ; fermeture/retrait
des captures vérifiés. Les erreurs/codes/timeout exhaustifs du runner existant
ne sont pas redoublés ici. Des gardes interdisent les lanceurs subprocess, os.system
et le réseau ; toute tentative reste observable même si un appelant la rattrape.

Les exécutables/outils de `WindowsBindingFixture` sont des octets inertes dans
un arbre possédé. Les descriptions ne lancent même pas le seed mémoire. Aucune
fabrication de sortie CTest, aucun chargement de GL, aucun contrôle du GPU cible.
Windows opengl-software demeure refusé par le test A27 existant ; aucun test de
backend logiciel Linux n'est copié ou modifié.

## G3 — oracle structurel borné

Dans `WindowsRealHelperBarrierTests`, seule la méthode de fermeture globale devient
`test_real_helper_admits_only_validated_prepare_build_before_effects`.

Elle vérifie unicité/ordre WRAPPERS→NATIVE→BINDING→DISPATCH, puis, dans le préfixe
**après les définitions BINDING**, root→flags→refus run→payload→inputs→arbre build.
Le contrôle PSBoundParameters reste présent, le refus run renvoie au direct adapter,
et tooling/MSVC/contexte/process ne peuvent apparaître avant dispatch.
Les définitions BINDING ne sont pas prises pour des appels exécutés.

Cette preuve structurelle n'est pas une preuve fonctionnelle de zéro effet.
Admission fine, extraction, mutations et sentinelle restent les tests F/H ci-dessous.
Le helper, legacy, WRAPPERS/NATIVE et les pins de production ne sont pas modifiés.

## G4 — inventaire ancien → preuve actuelle

Noms abrégés ci-dessous, tous dans `scripts/tests/verification_windows/` :

- F = `test_helper_functional.HelperFunctionalAdmissionTests`.
- H = `test_helper_functional.HelperFunctionalHarnessTests`.
- A = `test_adapter_binding.WindowsAdapterBindingTests`.
- N = `test_native_binding.NativeBindingSourceTests`.
- NH = `test_native_binding.NativeBindingHarnessTests`.
- W = `test_wrappers.WindowsWrapperTests`.
- D = `test_dispatch.WindowsDispatchMigrationTests`.
- E = `test_effective_windows_environment.EffectiveWindowsEnvironmentTests`.

### Dix méthodes de WindowsDispatchTests remplacées

1. `test_prepare_without_opt_in_refuses_before_effects` : nonzero, journal vide,
   parent intact, pas de fallback →
   F.`test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects` et assertions
   communes `assert_rejected/run_case` (cause attendue, payload/parent inchangés,
   événements vides et sentinelle absente).
2. `test_prepare_authorized_runs_conan_then_cmake_only` : Conan avant CMake,
   host/build Release, sortie/preset/arbre/toolchain exacts, workloads ON,
   testing OFF, aucun build/test →
   F.`test_prepare_reaches_real_binding`,
   F.`test_prepare_conan_before_fresh_cmake_profiles_trees_and_testing` et
   D.`test_prepare_release_cpu_jobs_one_preserves_order_and_exclusions`.
   Ce dernier garde explicitement les exclusions --build ET --test-dir.
3. `test_build_only_selected_target_without_bootstrap_or_configure` : une commande,
   CPU seule, parallel1, pas de bootstrap/configure →
   D.`test_build_release_cpu_jobs_one_has_exact_selected_argv_only`,
   F.`test_build_is_one_selected_command`,
   F.`test_unprepared_build_refuses_before_tooling_environment_or_repair`,
   F.`test_missing_build_tooling_does_not_bootstrap_or_configure`.
4. `test_run_only_preserves_argument_boundaries` : programme admis/argv exacts,
   un enfant → A.`test_05_cpu_workload_uses_admitted_nested_mapping_and_exact_shared_argv`,
   A.`test_06_gl_routes_use_shared_visible_options_and_repeatable_unique_request_ids`,
   A.`test_19_actual_cpu_execution_is_direct_and_collected_by_common_runner_only`,
   E01/E02/E03, W quoting/code17 et N exact StartInfo. Run ne passe PLUS par
   un payload/exécutable helper-run ; l'adaptateur direct décrit les options visibles.
5. `test_child_failure_preserves_raw_code_and_stops_sequence` : premier Conan17
   OU23, code brut identique, aucun configure →
   F.`test_first_nonzero_stops_prepare_and_preserves_raw_data`,
   D.`test_first_conan_code_23_stops_before_configure_and_keeps_raw_streams`,
   N.`test_capture_first_cause_survives_dispose_secondary`.
   D couvre spécifiquement premier23, non couvert par premier17/second23 de F.
6. `test_environment_present_and_absent_restored_after_success` : parent présent,
   absent/vide, environnement enfant et restauration →
   F.`test_child_environment_and_restore_success_nonzero_exception_absent_empty`,
   W.`test_environment_snapshot_is_detached`,
   W.`test_restore_removes_absent_and_retains_empty_values`,
   N.`test_context_map_copies_null_empty_and_private_lifetime` et E03.
   L'ancien enfant non filtré est remplacé par le VRAI filtre actuel : canari et
   KEEP absents, métadonnées MSVC/INCLUDE admises ; pas de garantie supprimée.
7. `test_environment_present_and_absent_restored_after_failure` : mêmes parents,
   enfant17 et restauration → mêmes preuves que6, branches nonzero/exception,
   plus F.`test_msvc_exception_and_wrong_metadata_restore_before_child`.
8. `test_selected_phases_never_fall_through_to_legacy_sentinel` : prepare/build/run
   sans fallback → F nominaux prepare/build et F admission pour run refusé,
   sentinelle vérifiée dans CHAQUE `FunctionalFixtureCase.run_case`, plus G3.
   Ancien run admis est une attente remplacée, pas une autorité à rétablir.
9. `test_fixture_refuses_unapproved_effects_before_evaluation` : sept mutants,
   code96/erreur harness/aucun effet/parent intact →
   H.`test_mutated_effects_are_rejected_before_evaluation`,
   H.`test_audit_never_evaluates_an_allowlisted_effect`,
   H.`test_marker_population_root_and_pin_mutants_are_harness_errors`,
   NH.`test_unapproved_effect_source_is_never_evaluated` et
   D.`test_file_write_mutant_is_rejected_as_audit_data_without_evaluation`.
   Le mutant exact File.WriteAllText reste DATA en mode=audit : aucun scriptblock
   évalué, même si son audit était défectueux. Allowlist commune inchangée.
10. `test_fixture_can_record_fake_child_and_restore_map_on_exit` : baseline0/17,
    un enfant mémoire, arguments littéraux et map restaurée →
    H.`test_root_binding_and_all_fake_leaves_baseline`,
    H.`test_fake_child_exception_is_independently_observable`,
    NH.`test_memory_factory_capture_zero_and_17`, W quoting/code17 et N exact
    StartInfo. La baseline actuelle contrôle aussi raw bytes, Dispose et cwd.

Les programmes nus conan/cmake de l'ancien prototype sont remplacés par les
chemins admis actuels conan.exe/cmake.exe ; aucune résolution PATH implicite.
Les noms arbitraires de fakes ne deviennent pas un contrat public de production.

### Onzième méthode : ancienne composition W

`W.test_local_composition_separates_phases_and_restores_environment` est remplacée
visiblement, pas skippée ni transformée en test d'existence de noms :

- prepare0 : Conan puis CMake, retour0, pas d'erreur → F prepare + D prepare.
- build0 : CMake seul, retour0, pas d'erreur → F build + D build.
- run0 : un programme/argv admis, retour0, pas d'erreur → A05/A06/A19 et E.
- prepare17 : Conan seul, retour17, aucun configure → F first_nonzero.
- Chacun des quatre parents : restauration exacte → assertions communes de F,
  W snapshot/restore, E parent synthétique ; pas d'assimilation des environnements
  outil filtrés à l'environnement générique du workload.
- Frontière réelle du wrapper et arguments →
  N.`test_real_child_wrapper_to_real_binding_records_exact_start_info` et les deux
  tests W ci-dessous, inchangés.

W conserve INTACTS `test_quote_round_trip_through_owned_finite_python_child` et
`test_child_wrapper_preserves_code_17_and_all_arguments` : empty/spaces/quotes,
backslashes, shell-literals, tab/newline et Unicode restent dans ARGUMENTS.
Aucun autre test W n'est modifié. L'ancienne action compose de la fixture commune
reste hors sélection et n'est plus une preuve du helper actuel.

### Fixture dispatch : dépendance préservée, programme retiré

`wrapper_fixture.ps1:258–281` parse le fichier et extrait l'unique fonction
`Assert-FixtureDispatch`. Cette fonction est conservée VERBATIM, sans extension
d'allowlist. Le consommateur commun reste inchangé. Le reste de l'ancien programme
dispatch autonome est retiré ; toute invocation du fichier refuse explicitement
AVANT lecture/évaluation du payload et renvoie à ce document. Parser/extraction
AST à vérifier par le lead, sans exécuter le fichier complet.

## Inventaire inverse et absence de double exécution

- F admission + G3 → anciennes1/8 et zéro effet/anti-fallback de la composition W.
- F prepare/build + D prepare/build → anciennes2/3 et composition prepare0/build0.
- F first_nonzero + D Conan23 + N première cause → ancienne5 et composition17.
- F restauration + W snapshot/restore + N contexte + E → anciennes6/7 et les
  quatre parents de la composition W.
- A05/A06/A19 + E + W/N arguments → ancienne4 et composition run0.
- H/NH audit/extraction + D audit FileWriteAllText → ancienne9.
- H/NH baselines + W/N arguments/codes → ancienne10.
- G1 (autre branche, non modifiée) reste propriétaire de la couture témoin/collecte/
  réadmission ; cette migration ne remplace pas cette preuve.

`test_dispatch.py` expose l'inventaire ancien comme données, pas des alias de tests.
D hérite seulement de `FunctionalFixtureCase`, sans méthode test ; le module
importe `test_helper_functional` comme MODULE, pas ses classes concrètes dans
son espace de découverte. Aucun load_tests ni appel d'une autre méthode test.
Les quatre compléments D ne rejouent pas toute la matrice F/H/N.
Le lead prend l'UNION nominative des preuves utiles une seule fois, en réutilisant
les preuves inchangées du rapport142 quand valides. Aucun discovery Windows global.

## Commandes FUTURES proposées — lead seulement, NON_EXECUTE par l'auteur

Cwd : `<candidat-intégré>/scripts/tests/verification_windows`.
Interpréteur proposé déjà utilisé au rapport142 : `C:/Python313/python.exe`.
Sa présence/version et les programmes restent à vérifier par le lead ; pas
d'installation ni de changement de toolchain impliqué par ces commandes.

Après relecture, nouveau G2 (zéro processus enfant) et oracle structurel G3 :

```powershell
C:/Python313/python.exe -B -m unittest -v test_effective_windows_environment.EffectiveWindowsEnvironmentTests test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_admits_only_validated_prepare_build_before_effects
```

Après admission des programmes factices existants, quatre compléments G4 seulement :

```powershell
C:/Python313/python.exe -B -m unittest -v test_dispatch.WindowsDispatchMigrationTests
```

Garanties wrapper/StartInfo à conserver, sélection nominative à relire :

```powershell
C:/Python313/python.exe -B -m unittest -v test_wrappers.WindowsWrapperTests.test_quote_round_trip_through_owned_finite_python_child test_wrappers.WindowsWrapperTests.test_child_wrapper_preserves_code_17_and_all_arguments test_native_binding.NativeBindingSourceTests.test_real_child_wrapper_to_real_binding_records_exact_start_info
```

Les autres remplaçants sont les IDs complets définis plus haut, pas un discovery
ou une autorisation implicite de lancer tous les tests historiques PowerShell.
Le lead conserve les sorties brutes et distingue précondition/harness, assertion
de contrat et résultat d'un programme factice. Zéro test ou skip ne vaut pas PASS.
Un échec production nouvellement révélé reste ROUGE et revient au lead, sans fix
de production dans cette tranche. Aucun faux RED de l'ancien prototype.

## Limites conservées

- Exécution de cette tranche : NON_EXECUTE chez l'auteur ; TDD runtime et
  non-régression à réaliser par le lead, sans revendication GREEN anticipée.
- Revue indépendante fresh après intégration réelle de B ET G1, pas fin auteur.
- Test06 jonction, cinq skips existants, encodage cmd non ASCII et gap
  Dispose/writers hérités : non qualifiés, pas effacés par ce mapping.
- Windows/Linux natifs, C++23/STL, build moteur, CTest réel, OpenGL logiciel ou
  matériel, rendu/image et performance : NON_EXECUTE pour cette tranche.
  Livraison pour qualification ultérieure, aucun fonctionnement natif démontré.
