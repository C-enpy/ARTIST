# pool-42 (@build-engineer teammate)

Session: ses_e732e3bc7ffePV0udI2KpxrVvg
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T12:39:04.917Z — msg_18cd225d1001eMjZTq0lc4ck8F

Le worktree attribué est propre, mais son HEAD contient déjà la migration G2/G3/G4 (`a130773`), et non uniquement H0 avec des ajouts documentaires. Je reste en lecture seule pour vérifier le mapping existant et signaler précisément cette divergence d’entrée, sans relancer de tests.

## assistant — 2026-10-30T12:41:14.563Z — msg_18cd4203f001OtWYAFLN2kAZg7

## Remise PLAN + MAPPING — lecture seule

**Première cause : l’entrée est déjà dépassée.** Le worktree attribué est propre, mais contient déjà la réalisation G2/G3/G4.

- Branche : `ensemble-4d233c15-c2f6-44f1-9d3c-fe31a80b5536`
- HEAD : `b420b2d9304874803d03accd30dbd482e3e45c4c`
- H0 `040824c` est ancêtre de HEAD, mais le descendant **n’est pas documentaire seulement** : intégration des tests par `a1307735af91a7758030a74c6f9a46216fd45569`.
- QA §15 était bien commité dans H0.
- **Aucune édition, aucun import cible, aucun test ou programme factice exécuté par moi.**

Constat reproductible, depuis le worktree `ensemble-2dc31e0a-cwsmsw-pool-42` :

```powershell
git rev-parse HEAD
git status --short
git merge-base --is-ancestor 040824cff819b074bfe0738efb3e35f66973555f HEAD
git diff --name-only 040824cff819b074bfe0738efb3e35f66973555f HEAD -- scripts
```

Résultat : statut propre, ancêtre confirmé, cinq fichiers de la tranche déjà intégrés.

## Livrables existants vérifiés sur disque

- `.opencode/evidence/mission-uniform-pipeline/windows-test-migration-plan.md`
- `scripts/tests/verification_windows/G2_G3_G4_MAPPING.md`
- Les quatre fichiers Python/PowerShell prévus par l’ownership.

Le mapping est déjà ratifié par `50dc587`, QA §15 et rapport148. Il ne faut donc pas lancer une seconde migration depuis cette remise.

## G2 — support minimal et oracles

La réalisation présente utilise `EffectiveWindowsEnvironmentTests`, `WindowsBindingFixture`, les vrais reçus `evidence.prepare/build`, `WindowsAdapter` et `SubprocessRunner(popen_factory=...)`.

Support local uniquement :

- parent complet synthétique, installé avec `clear=True` ;
- factory mémoire et `OwnedProcess`, sans enfant réel ;
- captures distinctes dans un répertoire possédé ;
- attentes argv/contexte construites indépendamment de `command.env_delta`.

Les quatre méthodes présentes couvrent :

1. Routes hardware `pass-name` et `prepared-float`.
2. Découverte puis exécution CTest avec request IDs distincts.
3. CPU : retrait des six contextes périmés, héritage des forçages inchangé.
4. Oracle discriminant : six champs absents/périmés, forçages conservés ou vides, mauvais argv/cwd/stdin/shell/captures.

Chaque invocation compare argv, cwd, `shell=False`, stdin DEVNULL, environnement effectif complet, captures et parent inchangé. Les deux forçages sont testés séparément, ensemble et absents ; hardware exige leur **absence**, pas une valeur vide.

Le refus Windows logiciel reste une preuve existante : aucun backend Linux logiciel copié.

## G3 — remplacement borné

Dans `WindowsRealHelperBarrierTests` :

`test_real_helper_remains_closed_before_dispatch`
→ `test_real_helper_admits_only_validated_prepare_build_before_effects`.

Oracle conservé : unicité/ordre des régions, puis root → flags → refus run → payload → inputs → arbre build, sans tooling/contexte/process avant dispatch.

Cette preuve reste **structurelle**. Elle ne remplace pas les tests fonctionnels d’admission, zéro effet, mutations et sentinelle. Helper, legacy et pins restent inchangés.

## G4 — mapping des anciennes obligations

Préfixes : **F/H** = classes admission/harness de `test_helper_functional`, **A** = `WindowsAdapterBindingTests`, **N/NH** = classes source/harness de `test_native_binding`, **D** = `WindowsDispatchMigrationTests`, **E** = G2.

Les dix anciennes méthodes sont réattribuées explicitement :

1. **`prepare_without_opt_in_refuses_before_effects`**  
   → F.`test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects` : refus, événements vides, parent/payload intacts, aucune sentinelle.

2. **`prepare_authorized_runs_conan_then_cmake_only`**  
   → F.`test_prepare_reaches_real_binding`, F.`test_prepare_conan_before_fresh_cmake_profiles_trees_and_testing`, D.`test_prepare_release_cpu_jobs_one_preserves_order_and_exclusions` : Conan avant configure, profils/arbre/toolchain/options exacts, aucun build/test.

3. **`build_only_selected_target_without_bootstrap_or_configure`**  
   → D.`test_build_release_cpu_jobs_one_has_exact_selected_argv_only`, F.`test_build_is_one_selected_command` et refus arbre/tooling : cible CPU seule, jobs1, aucun bootstrap/configure.

4. **`run_only_preserves_argument_boundaries`**  
   → A05/A06/A19, E, wrappers quoting/code17 et N exact StartInfo : run direct adapter/runner, argv exacts ; **aucun helper-run**.

5. **`child_failure_preserves_raw_code_and_stops_sequence`**  
   → F.`test_first_nonzero_stops_prepare_and_preserves_raw_data`, D.`test_first_conan_code_23_stops_before_configure_and_keeps_raw_streams`, N première cause : codes bruts17/23, aucun configure après échec.

6. **`environment_present_and_absent_restored_after_success`**  
   → F.`test_child_environment_and_restore_success_nonzero_exception_absent_empty`, wrappers snapshot/restauration, N contexte, E03 : présent/absent/vide et parent préservés.

7. **`environment_present_and_absent_restored_after_failure`**  
   → mêmes preuves, branches nonzero/exception et F.`test_msvc_exception_and_wrong_metadata_restore_before_child`.

8. **`selected_phases_never_fall_through_to_legacy_sentinel`**  
   → F prepare/build nominaux, admission refusant run, sentinelle commune et G3. L’ancien run admis n’est pas rétabli.

9. **`fixture_refuses_unapproved_effects_before_evaluation`**  
   → H mutations/audit/pins, NH extraction, D.`test_file_write_mutant_is_rejected_as_audit_data_without_evaluation` : mutant traité comme donnée, jamais évalué ; allowlist inchangée.

10. **`fixture_can_record_fake_child_and_restore_map_on_exit`**  
    → H baselines/exception, NH.`test_memory_factory_capture_zero_and_17`, wrappers et N StartInfo : enfant mémoire, codes0/17, arguments, captures et restauration.

L’ancienne composition wrapper est également réattribuée : prepare/build à F+D, run à A+E, prepare17 à F, restauration à F/W/N/E. Les deux garanties quoting/code17 restent intactes.

`dispatch_fixture.ps1` conserve `Assert-FixtureDispatch` pour son consommateur AST ; son programme autonome refuse explicitement. Aucun skip global, archive invisible ou extension d’allowlist.

L’inventaire inverse figure dans `G2_G3_G4_MAPPING.md:202–221`. Les remplaçants sont sélectionnés par union nominative, sans aliases exécutables ni appel d’une méthode test depuis une autre.

## Résultat et suite proposée au lead

- **Matrice personnellement exercée :** lectures et diagnostics Git seulement ; zéro test.
- **Preuve historique attribuée :** rapport149 annonce 19 tests synthétiques réussis sur l’intégration `a130773`, sans skip. Je ne les ai ni rejoués ni réattribués à mon intervention.
- **Correction proposée :** reconnaître cette affectation comme dépassée et reprendre au gate déjà documenté : intégration G1 puis audit fresh, sans nouvelle édition G2/G3/G4.
- Qualification native Windows/Linux, jonction06, moteur, CTest réel, OpenGL et performances : **NON_EXECUTE**.

Aucun secret d’environnement lu, aucune modification de permissions, aucune ressource cloud ou Docker créée.