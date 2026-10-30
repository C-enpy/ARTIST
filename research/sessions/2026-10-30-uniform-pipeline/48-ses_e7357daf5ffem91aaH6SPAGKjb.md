# pool-37 (@test-engineer teammate)

Session: ses_e7357daf5ffem91aaH6SPAGKjb
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T11:53:28.949Z — msg_18ca86671001806gT720p5pH44

## Admission préalable — pool-37

### Lectures réussies
- `AGENTS.md` (262 lignes) — **ARTIST — instructions OpenCode**.
- `.opencode/agents/test-engineer.md` (251 lignes) — premier titre : **Méthode portable**.
- `.opencode/missions/team_mvu0gbxm_0000_cwsmsw/reports/142-helper-integre-verification-synthetique.md` — **142 — Helper intégré, réception finale encore à compléter**.
- Skill `artist-build-validation` chargé — **Build et preuves graduées**.

### Pin observé
- HEAD fourni : `1abe4d108997ae72b3f81faf99ec70ca7edc4dac`.
- HEAD effectif : **non observé** ; aucun Git appelé.
- Rapport 142 : intégration helper `52e7330a7fd2e9667a80c6750db2fdf5ce84bc34`, collecteur `9aedc0a`. Déclarations documentaires, pas vérification indépendante des commits.
- Pins historiques profil `e4b0aa7` / skill `40a7cc3` distincts du HEAD fourni : ni appliqués ni réécrits.

### Identité, contexte et capacités
- Identité exposée : `pool-37`, `test-engineer`, `openai/gpt-6.1-sol`, variante medium.
- Racine exposée : `<USER_HOME>/source/repos/ARTIST`.
- Contexte demandé : fresh, ARTIST/pool-5, sans worktree. Aucun ancien worktree pool-5 consulté.
- Outils exposés : `read`, `glob`, `grep`, `skill`, `execute`, wrapper parallèle ; catalogue Code Mode `team_message`.
- **Preuve runtime positive** : lectures locales et chargement du skill réussis.
- **Simple déclaration** : deny général, exceptions lecture/recherche/skill/communication et interdictions edit/shell/subagent. Leur enforcement négatif n’a pas été testé volontairement.
- Le fichier de profil lu ne démontre pas, seul, son chargement effectif ni la composition des permissions natives.

### Contraintes respectées
Aucune édition, exécution de tests, commande shell/PowerShell/Git, API, fetch, délégation ou communication d’équipe. Aucun grant reçu ou appliqué. Aucun generator ni analyse des gaps commencé.

### Restitution
Finale native disponible : présent livrable. Réception effective et admission à confirmer par le lead.

**PASS** : lectures demandées et chargement du skill.  
**INCONCLUSIVE** : HEAD effectif, routage natif complet et enforcement des interdictions. Aucun refus rencontré. Aucun résultat moteur ou pipeline revendiqué.

## assistant — 2026-10-30T12:00:05.671Z — msg_18cae7423001LUEKKq6drtOZqq

# Candidat — plan generator borné, sans code

**pool-37 — test-engineer — lecture seule.** Admission runtime confirmée par le lead. Cette remise ne contient aucun patch, aucune modification ni exécution. Persistance et travaux de tests ultérieurs restent au lead/auteur distinct.

## 1. Identité et niveau de preuve

- HEAD fourni et vérifié **par le lead** : `1abe4d108997ae72b3f81faf99ec70ca7edc4dac`.
- Producteurs intégrés selon le lead et le rapport 142 : helper `52e7330`, collecteur `9aedc0a`.
- HEAD, SHA256 et delta : non calculés par ce profil.
- Lectures réussies : plan QA §13–14, contrats `windows-helper-evidence-v1.md` et `windows-adapter-binding-v1.md`, rapport 142, sources/fixtures ciblées ci-dessous.
- **Preuve personnelle : static-presence.** Les résultats d’exécution sont attribués au coordinateur, pas à pool-37.
- Résultats synthétiques transmis : helper 56/56, adaptateur Windows 54/54, core 219 succès et cinq skips sur 224, contexte 28/28, scénarios 31/31, intégration 11/11. Ils ne démontrent pas les nouvelles coutures proposées.
- Qualification native Windows/Linux, C++, moteur, GPU et performances : **NON_EXECUTE**.

Le présent contexte n’a écrit ni production ni tests. Les oracles viennent des contrats ratifiés, non d’une reproduction du comportement observé.

## 2. Priorités et risques

| Gap | Obligation et risque | I×P | Décision |
|---|---|---:|---|
| G-1 | §14 W3–W6, lignes 465–470 et 550–552 : receipt favorable sans couture réelle producteur→collecteur→réadmission | 5×4=20 | **DEFER** réalisation, plan défini |
| G-2 | §13 ligne395, binding lignes10–22 : déclarations correctes mais environnement effectif enfant erroné | 5×3=15 | **KEEP** proposition ciblée |
| G-3 | §13 lignes319–325 : ancienne fermeture globale devenue oracle obsolète | 5×3=15 | **MODIFY** |
| G-4 | §13 lignes347–350 : prototype dispatch confondu avec le raccordement actuel, ou réintroduction helper-run | 4×4=16 | **MODIFY/REPLACE**, mapping ci-dessous |

Déclencheurs : absence de témoin réel, héritage de contexte ambiant, retrait brutal d’une ancienne assertion, exécution d’un prototype incompatible.

Portée : attribution prepare/build, environnement des commandes et validité des conclusions QA. Détection : octets/hash/ordre d’événements, observation factory, sentinelles zéro effet et mapping explicite. Mitigation : réutiliser les fixtures admises, sans helper complet ni outil natif.

## 3. G-1 — même bundle, même témoin, vraie collecte

### Obligation → invariant

Le helper fragmentaire produit réellement les rôles et le témoin ; le core collecte ces octets sous lease avant `PreparationReceipt`, puis réadmet **ces mêmes octets** avant toute description build ou effet tooling. Aucun témoin synthétique de substitution, receipt préfabriqué ou seconde capture primaire.

### Scénarios → oracles

**G-1a nominal — KEEP comme cible, réalisation DEFER**

1. Préparer un seul arbre source synthétique et un seul bundle possédé.
2. Faire passer `evidence.prepare` par les fragments source admis du helper, avec factory mémoire.
3. Observer avant `_finish` :
   - leases bundle/arbre actifs ;
   - writers fermés ;
   - rôles nécessaires présents et traces favorables ;
   - SHA256 des octets produits inscrit dans manifeste et receipt.
4. Appeler `evidence.build` avec ce receipt et ce bundle.
5. Vérifier témoin et payload byte-for-byte inchangés ; aucune ouverture de réécriture du témoin ; build seul, sans bootstrap/configure.
6. Vérifier qu’une commande helper n’entraîne qu’une capture commune primaire. Les journaux privés ne constituent pas une seconde exécution.

**Oracle indépendant :** contrat privé lignes27–40, 53–78 et 80–104 ; hash calculé depuis les octets lus, non depuis le validateur de production. Le témoin atteste des déclarations, **pas l’identité binaire des outils**.

**G-1b négatif — KEEP comme cible, réalisation DEFER**

Après prepare favorable, modifier ou retirer le témoin, puis demander build sur un bundle neuf pour chaque cas.

Attendu :
- refus défavorable avant `adapter.describe_build` ;
- compteur factory/tooling inchangé ;
- aucune réécriture réparatrice ;
- absence conservée, ou octets altérés conservés pour diagnostic.

Les mutations exhaustives de schéma restent aux tests core existants : ne pas les dupliquer dans cette couture.

**G-1c échec de préparation — KEEP comme cible**

Une seule variante : faux Conan nonzero après production des partiels.

Attendu : pas de configure, pas de témoin favorable fabriqué, partiels collectés avant receipt et première cause conservée. Réutiliser les tests existants pour les autres issues.

### Réutilisation vérifiée et précondition manquante

Existent :
- `NativeBindingCase.fragments`, `run_case`, `native_binding_fixture.ps1` ;
- `ArtistBindingMemoryProcess`, action `entry`, factory mémoire et export source contrôlé ;
- `HelperFixture`, `HelperRunner`, `RuntimeFixture` ;
- `BundleSink`, `evidence.prepare/build` ;
- `WindowsHelperEvidenceContractTests.finish_observer`, `assert_artifacts`, `assert_no_build`.

**Limite concrète :** `NativeBindingCase.run_case` recrée actuellement son propre case, bundle, transport et témoin éventuel (lignes137–195). `HelperRunner` fabrique séparément des preuves privées. Les juxtaposer ne démontrerait donc pas G-1.

Le symbole littéral `SINKFixtureRuntime` du brief n’a pas été retrouvé dans les fixtures core ; le symbole vérifié est `runtime_fixtures.RuntimeFixture`. Aucune API de raccordement supposée n’est inventée ici.

**DEFER propriétaire :** lead puis auteur de tests distinct, complexité ≥70. Avant code, proposer et faire relire l’adaptation minimale permettant aux fragments d’utiliser l’unique bundle appartenant au sink. Le programme fragmentaire doit être installé avant le snapshot source, sans masquage des contrôles d’identité. Pas de recopie après coup d’un autre bundle ni d’assouplissement du core.

## 4. G-2 — environnement Windows effectif jusqu’à la factory

### Obligation → invariant

Les six variables de contexte de chaque commande proviennent explicitement du plan/receipt. En hardware, les forçages logiciels hérités sont absents de l’environnement effectivement transmis. Le parent entièrement synthétique reste identique.

### Scénarios → oracles

**G-2a hardware — KEEP**

Réutiliser les descriptions réelles de `WindowsAdapter` pour :
- chaque route workload sélectionnée ;
- découverte CTest ;
- exécution CTest.

Passer chaque `CommandSpec` au vrai `SubprocessRunner(popen_factory=…)`, avec factory mémoire stricte analogue à `EffectiveLinuxEnvironmentTests.effective`.

À la factory :
- argv et cwd exactement ceux de la commande ;
- `shell=False`, stdin DEVNULL, deux captures possédées ;
- les six champs correspondent au plan/receipt ; request-id conforme au rôle et aux argv lorsqu’il y figure ;
- absence exacte de `LIBGL_ALWAYS_SOFTWARE` et `GALLIUM_DRIVER` ;
- un seul processus mémoire et captures nettoyées ;
- parent synthétique inchangé après retour.

Négatifs : parent contenant valeurs de contexte périmées et chacun des deux forçages, séparément puis ensemble. Aucun enfant réel.

**G-2b CPU — KEEP**

Parent synthétique avec ancien contexte hardware et forçages logiciels.

Attendu : six variables de contexte absentes de l’enfant ; marqueurs ordinaires et politique existante d’héritage des forçages conservés. Ne pas imposer une nouvelle politique CPU.

**G-2c logiciel Windows — KEEP test existant, REJECT nouvelle exécution**

`test_adapter_binding.py::test_27_windows_software_profile_and_asan_configuration_have_no_silent_fallback` refuse actuellement le profil Windows `opengl-software`.

Ne pas fabriquer de commande logicielle Windows pour imiter Linux. Conserver ce refus ; la politique Linux logiciel reste couverte séparément.

### Réutilisation et emplacement proposé

- Fixtures existantes : `WindowsBindingFixture`, helpers de `WindowsAdapterBindingTests`, `WindowsHardwareBindingTransportTests`.
- Modèle de factory : `verification_linux/test_effective_linux_environment.py`.
- Source observée : `runner.py`, lignes44–57 et94–96.
- Futur fichier proposé, non créé :  
  `scripts/tests/verification_windows/test_effective_windows_environment.py`.

Ne pas dériver exclusivement les attendus de `command.env_delta` : conserver les assertions indépendantes plan/receipt déjà présentes dans `WindowsHardwareBindingTransportTests.hardware`.

## 5. G-3 — remplacement précis de l’ancienne barrière

### Obligation → risque → scénario → oracle

L’ancienne assertion exige encore le texte « phases non implémentées ». La source actuelle admet prepare/build et refuse run avant payload/dispatch. **Incompatibilité statique constatée ; test non exécuté ici.**

**Décision MODIFY**, pas suppression.

Dans `test_wrappers.py::WindowsRealHelperBarrierTests` :
- renommer l’oracle pour exprimer admission fine avant effets ;
- contrôler population unique et ordre des régions ;
- vérifier flags → refus run → lecture payload → résolution inputs → admission arbre build ;
- vérifier absence de tooling/env/contexte/process avant dispatch ;
- conserver les refus fonctionnels et anti-fallthrough des fragments ;
- conserver pins WRAPPERS/NATIVE et contrôle legacy.

La proposition de `SOURCE_GREEN_READY.md`, lignes136–163, est une base cohérente avec les lignes1191–1229 de la source observée. Elle reste un contrôle structurel : **elle ne remplace pas** les cas exécutés d’admission.

Nominal : prepare/build admis atteignent leur branche sous fixture valide.  
Négatif : run, opt-in absent, flags/payload invalides ne produisent aucun effet et n’atteignent pas legacy.

Couverture existante à conserver : `HelperFunctionalAdmissionTests.test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects`, `test_invalid_payload_and_binding_have_zero_effects_and_no_repair`, et contrôles d’extraction/pins.

## 6. G-4 — mapping bidirectionnel du prototype dispatch

Les noms ci-dessous appartiennent à `WindowsDispatchTests`. **Conserver les exigences ; remplacer leur preuve prototype par les tests actuels indiqués.** Ne pas élargir `dispatch_fixture.ps1` pour lui faire accepter le nouveau helper.

| Ancien test / exigence | Preuve actuelle ou proposée | Décision |
|---|---|---|
| `test_prepare_without_opt_in_refuses_before_effects` | Functional admission : invalid phases/opt-in/flags, zéro effet | REPLACE preuve prototype |
| `test_prepare_authorized_runs_conan_then_cmake_only` | `test_prepare_conan_before_fresh_cmake_profiles_trees_and_testing` | REPLACE |
| `test_build_only_selected_target_without_bootstrap_or_configure` | `test_build_is_one_selected_command` + refus arbre/tooling | REPLACE |
| `test_run_only_preserves_argument_boundaries` | Adapter binding tests05/06/19 + G-2 argv exact à factory ; wrappers quoting | REPLACE ; REJECT helper-run |
| `test_child_failure_preserves_raw_code_and_stops_sequence` | `test_first_nonzero_stops_prepare_and_preserves_raw_data` + native first-cause | REPLACE |
| `test_environment_present_and_absent_restored_after_success` | Functional `test_child_environment_and_restore_success_nonzero_exception_absent_empty` | REPLACE |
| `test_environment_present_and_absent_restored_after_failure` | Même test, branches nonzero/exception ; G-2 pour parent Python | REPLACE |
| `test_selected_phases_never_fall_through_to_legacy_sentinel` | Admission fragmentaire et sentinelle réelle ; run refusé | MODIFY mapping des phases |
| `test_fixture_refuses_unapproved_effects_before_evaluation` | Functional harness `test_mutated_effects_are_rejected_before_evaluation` + native contrôle source | REPLACE |
| `test_fixture_can_record_fake_child_and_restore_map_on_exit` | Functional baseline/exception + native factory codes0/17 | REPLACE |

Compléments conservés :
- **Quoting natif : KEEP** `WindowsWrapperTests.test_quote_round_trip_through_owned_finite_python_child` et `test_child_wrapper_preserves_code_17_and_all_arguments`. Exécution seulement par le lead après relecture du programme factice.
- **Command line StartInfo : KEEP** `NativeBindingSourceTests.test_real_child_wrapper_to_real_binding_records_exact_start_info`.
- **Null/vide/restauration : KEEP** tests wrappers, native context map et functional précités.
- **Codes bruts/capture : KEEP** core et native ; pas de duplication dans G-2.

**Sens inverse :**
- admission/phases → functional + G-3 ;
- prepare/build/stop → functional ;
- argv directs/environnement enfant → adapter + G-2 ;
- quoting Windows → wrappers/native ;
- refus d’effets/pins → harness functional/native ;
- témoin/collecte/réadmission → core/native + G-1.

Le prototype et ses preuves historiques restent identifiés comme historiques, jamais comptés comme qualification actuelle. Leur remplacement doit être explicite dans l’inventaire de sélection et le rapport : ni skip global, ni disparition silencieuse. Aucun helper-run ne doit être ajouté pour les verdir.

## 7. Périmètre futur et critères de sortie

Fichiers candidats uniquement, après validation du mapping :
- G-1 : nouveau test de couture dédié ; adaptation minimale de fixture native et fixture core, à préciser par l’auteur avant édition ;
- G-2 : nouveau fichier nommé ci-dessus ;
- G-3 : `test_wrappers.py` ;
- G-4 : sélection/migration explicite de `test_dispatch.py` et `dispatch_fixture.ps1`, sans modification production.

**Aucune nouvelle infrastructure.** Pas de réparation des jonctions/symlinks, du non-ASCII cmd ou du gap Dispose/writers hérité. Test06 jonction reste NON_EXECUTE ; cinq skips connus restent INCONCLUSIVE.

Ordre recommandé : persister ce plan → auteur distinct et fixture relue → G-3/G-4 et G-2 → couture G-1 après validation de son raccordement → revue ciblée indépendante → exécution lead et intégration.

Le runner doit fournir pour chaque sélection : commande exacte, cwd, identité runner, HEAD/delta/SHA256, versions, attendus/observés, découverte/exécution/échecs/skips et première cause. Aucune commande nouvelle n’est présentée ici comme déjà exécutée.

## 8. État final

- Propositions G-1 à G-4 : **INCONCLUSIVE**, non exécutées.
- RED/GREEN/REFACTOR nouveaux : **NON_EXECUTE** ; aucun RED historique rejoué artificiellement.
- Auto-réparations : **0**.
- Bug de production reproduit : **aucun**.
- Réception concernée : G-1 et G-2 restent des gaps de preuve ; G-3/G-4 doivent être migrés avant de présenter une sélection historique comme actuelle.
- Livrable : **plan prêt pour revue/persistance par le lead**, pas fonctionnement natif démontré.

## assistant — 2026-10-30T12:01:42.372Z — msg_18cafeddf001RPcOT6n4SjTB8u

Information reçue : documentation intégrée en `9a45bf6`, sans changement du pipeline ni des tests selon le lead.

Le plan G-1–G-4 remis reste applicable. Le guide n’est pas une preuve de couverture : gaps **INCONCLUSIVE**, validations nouvelles **NON_EXECUTE**.

Aucune lecture supplémentaire, modification ou exécution. Candidat prêt pour revue et persistance par le lead ; arrêt du worker.

## assistant — 2026-10-30T12:02:14.091Z — msg_18cb069c5001ifTnTxoKpIVIEd

# Plan generator G1–G4 — candidat autonome

**Auteur : pool-37, test-engineer.** Restitution exclusivement textuelle, sans code ni patch. Aucun candidate_id natif distinct du plan complet n’est connu de ce profil.

## 1. Périmètre et preuves

- HEAD fourni et vérifié **par le lead** : `1abe4d108997ae72b3f81faf99ec70ca7edc4dac`.
- Producteurs intégrés selon le lead et le rapport142 : helper `52e7330`, collecteur `9aedc0a`.
- Documentation ensuite intégrée en `9a45bf6`, sans changement du pipeline ou des tests selon le lead. Sa prose ne clôt aucun gap.
- Sources lues : plan QA §13–14, contrats `windows-helper-evidence-v1.md` et `windows-adapter-binding-v1.md`, rapport142, sources et fixtures ciblées citées ci-dessous.
- Preuve personnelle : **static-presence**. Aucun Git, calcul SHA256, test ou programme exécuté par pool-37.
- Résultats synthétiques transmis par le lead : helper56/56, adaptateur Windows54/54, core219 succès et cinq skips sur224, contexte28/28, scénarios31/31, intégration11/11. Ces résultats ne démontrent pas les nouvelles coutures.
- Qualification native Windows/Linux, C++, moteur, GPU et performances : **NON_EXECUTE**.

Ce contexte n’a écrit ni production ni tests. Les oracles sont issus du plan et des contrats ratifiés, pas déduits du comportement pour le justifier.

## 2. Priorités

| Proposition | Risque/invariant | QA I×P | Décision |
|---|---|---:|---|
| G-1 | Receipt favorable sans couture réelle helper→collecteur→réadmission | 5×4=20 | **DEFER** réalisation sous précondition précise |
| G-2 | Déclarations correctes mais environnement effectif enfant erroné | 5×3=15 | **KEEP** proposition |
| G-3 | Ancienne fermeture globale devenue oracle obsolète | 5×3=15 | **MODIFY** |
| G-4 | Prototype dispatch présenté comme preuve actuelle ou imposant helper-run | 4×4=16 | **MODIFY/REPLACE** avec mapping explicite |

Déclencheurs : témoin synthétique de substitution, contexte ambiant hérité, suppression d’une assertion historique sans remplacement, prototype incompatible réactivé.

Portée : attribution prepare/build, environnement des commandes et fiabilité des conclusions QA. Détection : comparaison d’octets/hash, ordre des événements, compteurs de factory, sentinelles zéro effet et mapping bidirectionnel. Mitigation : fixtures existantes, aucun helper complet ni outil natif.

## 3. G-1 — même bundle, même témoin, vraie collecte

### Obligation → invariant

Plan QA §14 W3–W6, lignes465–470 et550–552 ; contrat privé lignes27–40,53–78 et80–104.

Les fragments réels admis du helper produisent les rôles et le témoin. Le core collecte ces octets sous lease avant `PreparationReceipt`, puis réadmet **exactement ces octets** avant description build ou tooling.

Interdits : receipt préfabriqué, remplacement du témoin par `witness_data`, copie après coup depuis un autre bundle, seconde capture primaire ou réécriture réparatrice.

### G-1a — nominal

**Scénario :**
1. Un arbre source synthétique et un bundle possédé uniques.
2. `evidence.prepare` traverse les fragments admis, avec factory mémoire.
3. Avant `_finish`, observer leases bundle/arbre, writers fermés, rôles nécessaires et traces favorables.
4. Comparer les SHA256 des octets réellement produits au manifeste et au receipt.
5. Appeler `evidence.build` avec ce receipt et ce bundle.
6. Vérifier témoin/payload inchangés, absence de réécriture et build sans bootstrap/configure.
7. Vérifier une seule capture commune primaire par commande helper.

**Oracle :**
- octets lus directement et empreintes recalculées indépendamment ;
- population et champs définis par le contrat ;
- événement de collecte avant publication du receipt ;
- journaux privés distincts d’une deuxième exécution.

Le témoin atteste des chemins et métadonnées déclarés, **pas de l’identité binaire des outils**.

**Décision : KEEP comme cible ; réalisation DEFER.**

### G-1b — négatif minimal

Après prepare favorable, modifier ou retirer le témoin ; utiliser un bundle neuf par cas.

**Attendu :**
- refus avant `adapter.describe_build` ;
- compteurs runner/factory/tooling inchangés ;
- aucun remplacement du témoin ;
- absence ou octets altérés conservés pour diagnostic.

Ne pas recopier la matrice exhaustive de schéma déjà couverte par les tests core.

**Décision : KEEP comme cible ; réalisation DEFER.**

### G-1c — échec de préparation

Un seul cas de couture : faux Conan nonzero après production de partiels.

**Attendu :**
- pas de configure ;
- aucun témoin favorable fabriqué ;
- partiels collectés avant receipt ;
- première cause conservée.

Les autres issues restent aux tests existants.

**Décision : KEEP comme cible ; même précondition.**

### Réutilisation vérifiée et blocage précis

Symboles existants :
- `NativeBindingCase.fragments`, `NativeBindingCase.run_case` ;
- `native_binding_fixture.ps1`, `ArtistBindingMemoryProcess`, action `entry` ;
- `HelperFixture`, `HelperRunner`, `runtime_fixtures.RuntimeFixture` ;
- `BundleSink`, `evidence.prepare`, `evidence.build` ;
- `WindowsHelperEvidenceContractTests.finish_observer`, `assert_artifacts`, `assert_no_build`.

Limite observée : `NativeBindingCase.run_case` recrée son propre case, bundle, transport et témoin éventuel aux lignes137–195. `HelperRunner` fabrique séparément les preuves privées. Les juxtaposer ne prouve pas G-1.

Le nom littéral `SINKFixtureRuntime` du brief n’a pas été retrouvé dans les fixtures core consultées ; le symbole vérifié est `runtime_fixtures.RuntimeFixture`. Aucune API de raccordement supposée n’est proposée comme existante.

**DEFER_TO : lead puis auteur de tests distinct, complexité ≥70.** Avant code, faire relire l’adaptation minimale permettant aux fragments d’utiliser l’unique bundle du sink. Installer le programme fragmentaire avant snapshot source, sans masquer les contrôles d’identité. Ne pas assouplir le core pour faciliter la fixture.

## 4. G-2 — WindowsAdapter jusqu’à l’environnement effectif de la factory

### Obligation → invariant

Plan QA §13 ligne395 ; binding Windows lignes10–22.

Les six variables de contexte doivent être effectivement transmises depuis plan/receipt pour chaque invocation hardware. Les forçages logiciels hérités sont retirés. Le parent entièrement synthétique reste inchangé.

### G-2a — hardware

**Scénario :**
- descriptions réelles de `WindowsAdapter` pour chaque route workload sélectionnée, découverte CTest et exécution CTest ;
- passage de chaque `CommandSpec` au vrai `SubprocessRunner(popen_factory=…)` ;
- factory mémoire stricte, aucun enfant réel ;
- parent synthétique contenant contexte périmé et forçages séparément, puis ensemble.

**Oracle à la factory :**
- argv et cwd exacts ;
- `shell=False`, stdin DEVNULL, deux captures possédées ;
- six champs conformes au plan/receipt ;
- request-id conforme au rôle et aux argv lorsqu’il y figure ;
- absence exacte de `LIBGL_ALWAYS_SOFTWARE` et `GALLIUM_DRIVER` ;
- un seul processus mémoire ;
- captures fermées/nettoyées ;
- parent synthétique identique après retour.

Ne pas dériver tous les attendus de `command.env_delta` : préserver les assertions indépendantes plan/receipt.

**Décision : KEEP.**

### G-2b — CPU

**Scénario :** parent synthétique contenant contexte hardware périmé et forçages logiciels.

**Oracle :**
- six variables de contexte absentes de l’enfant ;
- marqueurs ordinaires préservés ;
- politique actuelle d’héritage des forçages conservée ;
- parent inchangé.

Aucune nouvelle politique CPU n’est introduite.

**Décision : KEEP.**

### G-2c — logiciel Windows

`test_adapter_binding.py::test_27_windows_software_profile_and_asan_configuration_have_no_silent_fallback` refuse le profil Windows `opengl-software`.

**Nominal :** refus explicite du profil non admis.  
**Négatif :** aucune commande de remplacement ou repli silencieux.

**Décision : KEEP le test existant ; REJECT une nouvelle exécution logicielle Windows calquée sur Linux.**

### Harness réutilisé

- `WindowsBindingFixture` ;
- helpers de `WindowsAdapterBindingTests` ;
- `WindowsHardwareBindingTransportTests.hardware` ;
- modèle mémoire de `verification_linux/test_effective_linux_environment.py` ;
- source `runner.py`, lignes44–57 et94–96.

Futur fichier proposé, non créé :  
`scripts/tests/verification_windows/test_effective_windows_environment.py`.

Les tests Linux existants ne doivent pas être déplacés ni remplacés.

## 5. G-3 — remplacer précisément l’ancienne barrière

### Obligation → risque

Plan QA §13 lignes319–325 : remplacer l’ancienne fermeture globale uniquement après mapping, sans supprimer les garanties d’admission, de zéro effet et d’absence de fallback.

Le test `WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch` exige encore le texte « phases non implémentées ». La source observée admet prepare/build et refuse run avant payload/dispatch.

**Constat statique d’oracle obsolète ; aucun échec exécuté revendiqué.**

### Scénario → oracle

**Nominal :** prepare/build valides atteignent leur branche sous fixture admise.

**Négatif :** run, opt-in absent, flags ou payload invalides :
- refus avant effets ;
- aucun tooling/env/process ;
- aucune chute legacy.

Dans `test_wrappers.py::WindowsRealHelperBarrierTests` :
- renommer le test vers l’admission fine ;
- contrôler population unique et ordre des régions ;
- vérifier flags → refus run → lecture payload → résolution inputs → admission arbre build ;
- vérifier absence d’effets avant dispatch ;
- conserver contrôles legacy et pins WRAPPERS/NATIVE.

La proposition de `SOURCE_GREEN_READY.md`, lignes136–163, constitue une base cohérente avec la source observée aux lignes1191–1229. Elle reste une preuve structurelle, pas un substitut aux refus exécutés.

**Décision : MODIFY**, jamais suppression simple.

Preuves fonctionnelles à conserver :
- `test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects` ;
- `test_invalid_payload_and_binding_have_zero_effects_and_no_repair` ;
- contrôles d’extraction, mutations et pins de `HelperFunctionalHarnessTests`.

## 6. G-4 — migration explicite du prototype dispatch

### Obligation → invariant

Plan QA §13 lignes347–350 et contrat binding : exécution directe par adaptateur/runner commun, aucun helper-run.

`test_dispatch.py` et `dispatch_fixture.ps1` utilisent un prototype limité au dispatch, avec variables injectées. Ils ne constituent pas la preuve du raccordement actuel. Ne pas élargir leur allowlist pour réintroduire le contrat abandonné.

### Mapping ancienne exigence → preuve actuelle/proposée

Les noms suivants appartiennent à `WindowsDispatchTests`.

| Ancien test | Preuve de remplacement | Décision |
|---|---|---|
| `test_prepare_without_opt_in_refuses_before_effects` | Functional admission : phases/opt-in/flags invalides, zéro effet | REPLACE |
| `test_prepare_authorized_runs_conan_then_cmake_only` | `test_prepare_conan_before_fresh_cmake_profiles_trees_and_testing` | REPLACE |
| `test_build_only_selected_target_without_bootstrap_or_configure` | `test_build_is_one_selected_command` et refus arbre/tooling | REPLACE |
| `test_run_only_preserves_argument_boundaries` | Adapter tests05/06/19, G-2 argv exact à factory, wrappers quoting | REPLACE ; REJECT helper-run |
| `test_child_failure_preserves_raw_code_and_stops_sequence` | `test_first_nonzero_stops_prepare_and_preserves_raw_data`, native first-cause | REPLACE |
| `test_environment_present_and_absent_restored_after_success` | Functional `test_child_environment_and_restore_success_nonzero_exception_absent_empty` | REPLACE |
| `test_environment_present_and_absent_restored_after_failure` | Même test, branches nonzero/exception ; G-2 pour parent Python | REPLACE |
| `test_selected_phases_never_fall_through_to_legacy_sentinel` | Admission fragmentaire et sentinelle ; run refusé | MODIFY mapping des phases |
| `test_fixture_refuses_unapproved_effects_before_evaluation` | Functional mutations avant évaluation et native contrôle source | REPLACE |
| `test_fixture_can_record_fake_child_and_restore_map_on_exit` | Functional baseline/exception et native factory codes0/17 | REPLACE |

Ici **REPLACE signifie MODIFY la couverture et son inventaire**, pas supprimer une exigence.

### Garanties à garder

- **Quoting natif — KEEP :**
  - `WindowsWrapperTests.test_quote_round_trip_through_owned_finite_python_child` ;
  - `test_child_wrapper_preserves_code_17_and_all_arguments`.
- **StartInfo réel — KEEP :**
  - `NativeBindingSourceTests.test_real_child_wrapper_to_real_binding_records_exact_start_info`.
- **Null/vide/restauration — KEEP :** tests wrappers, native context map et functional précités.
- **Codes bruts/capture — KEEP :** core/native existants ; ne pas les dupliquer dans G-2.
- **Helper-run — REJECT :** contraire au contrat actuel.
- **Skip global/archive invisible — REJECT :** masquerait la migration.

### Mapping inverse

| Preuve | Exigences conservées |
|---|---|
| Functional admission + G-3 | Admission fine, zéro effet, absence de fallback |
| Functional prepare/build | Ordre Conan→configure, cibles exactes, arrêt au premier nonzero |
| Adapter + G-2 | Commandes directes, limites argv et environnement effectif |
| Wrappers/native | Quoting Windows et command line réellement transmise |
| Harness functional/native | Refus des effets non admis, extraction et pins |
| Core/native + G-1 | Témoin, collecte sous lease, publication et réadmission |

Le prototype et ses preuves historiques restent identifiés comme historiques. Ils ne sont pas comptés comme qualification actuelle. Leur remplacement doit être visible dans la sélection et le rapport de couverture.

## 7. Réalisation minimale et barrières

Fichiers candidats uniquement, après revue/persistance du plan :
- G-1 : test de couture dédié et adaptation minimale des fixtures native/core, à préciser par l’auteur avant édition ;
- G-2 : nouveau fichier nommé ci-dessus ;
- G-3 : `test_wrappers.py` ;
- G-4 : migration explicite de `test_dispatch.py` et `dispatch_fixture.ps1`.

Aucune modification production, nouveau backend ou infrastructure de tests.

Ordre :
1. Lead persiste et commit le plan/mapping.
2. Auteur de tests distinct, complexité ≥70.
3. Relecture des fixtures, extractions et frontières d’effets.
4. G-3/G-4 et G-2 ; G-1 après validation du raccordement minimal.
5. Runner lead : tests ciblés puis non-régression pertinente.
6. Revue indépendante ciblée, corrections et intégration avant audit consommateur.

Les compléments ne remplacent pas le RED des auteurs. Sur une implémentation intégrée déjà correcte, ne pas fabriquer un RED historique ni appeler RED une panne du harness.

Le runner doit conserver : identité, commande exacte, cwd, HEAD/delta/SHA256, versions, attendu/observé, découverts/exécutés/échoués/skipped et première cause. Changement de candidat ⇒ preuve à requalifier.

## 8. Limites et décision de sortie

- G-1–G-4 : **INCONCLUSIVE**, nouvelles validations non exécutées.
- RED/GREEN/REFACTOR nouveaux : **NON_EXECUTE**.
- Auto-réparations : **0**.
- Bug de production reproduit : **aucun**.
- Test06 jonction : **NON_EXECUTE**.
- Cinq skips connus : **INCONCLUSIVE**, pas des succès.
- Non-ASCII cmd et Dispose/writers hérités : hors de cette correction, qualification ultérieure.
- Aucun helper complet, outil natif, suite moteur, rendu, benchmark, installation ou cloud autorisé ici.

**Décision globale : plan prêt pour revue et persistance par le lead. G-1/G-2 restent des gaps de preuve ; G-3/G-4 exigent une migration explicite avant toute présentation de la couverture historique comme actuelle. Livraison pour qualification ultérieure, jamais fonctionnement natif démontré.**