---
title: "Mission terminée — Pipeline A1+A2 : le Pipeline, ses passes et leur contexte deviennent propriétaires"
category: architecture
tags: [artist, architecture, cpp, ownership, testing]
summary: "Mission Pipeline A1+A2 du 2026-10-16 : ownership unique du Pipeline et des passes (A1a/A1b), lifetime GL chargé (A1c), contexte unique d'IPass et getters par emprunt (A2/F6) ; branches H-ASan, S-GSS, DOC-REF, F8-SEAM, H-RESTORE et H-FIXTURE intégrées ; I0 214/214 + 197 ASan, final f914d77 215/215 + 198 ASan exacts, harnais 9+5 vérifié sur 95c2b9e ; oracle GMock Attribute préexistant FAIL conservé ; ADR-0003 Accepted/partial, aucun PASS global."
status: completed
implementation: verified
source_revision: 738891fb98e23ee0dbe4afb1f1009c225105d894
created: 2026-10-16
updated: 2026-10-16
sources: [docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/development/known-incidents.md, docs/development/build-and-test.md, docs/architecture/correction-plan.md, docs/_meta/session-2026-10-16-pipeline-i-a1-validation.json, docs/_meta/session-2026-10-16-pipeline-a1c-validation.json, docs/_meta/session-2026-10-16-pipeline-i0-validation.json, docs/_meta/session-2026-10-16-asan-caller-restoration.json, docs/_meta/session-2026-10-16-pipeline-final-validation.json]
provenance:
  extracted: 0.9
  inferred: 0.08
  ambiguous: 0.02
---

# Mission terminée : le Pipeline, ses passes et leur contexte sont propriétaires

**Périmètre de vérification : Windows x64 Debug, MSVC 19.44 / toolset 14.44, candidats intégrés
`18edc72` (I0) puis `f914d77` (A2 + H-RESTORE) puis `95c2b9e` (H-FIXTURE), heads producteurs
`fc0254a`, `3526ead`, `b44ba0b`, `14d5c15`, `1dfff22`, `4d1b655`, campagnes du 2026-10-16.**
L'état de clôture documentaire est le HEAD principal `738891f` : le code moteur C++ est
identique à `f914d77`, le correctif `95c2b9e` est Python-only. Cette mission applique
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 (Accepted / partial)]] ; elle n'adopte aucun
ADR `Proposed` (ADR-0004 reste hors mission), n'introduit ni handle générationnel ni
templatisation nouvelle et n'ouvre aucun backend. Les preuves non versionnées sont dans le
dépôt principal : **`C:/Users/djden/source/repos/ARTIST/out/mission-a1a2/`**
(`final-verification/` — `report.json` SHA-256 `c22213a3…`, 85 artefacts recoupés ;
`post-fixture-verification/` — `report.json` SHA-256 `ac855ff…`, 21 artefacts recoupés ;
`a2-author-final-1dfff22/` ; `h-fixture-author-final-4d1b655/` ; étapes intermédiaires par
pool). Empreintes des rapports et manifestes recoupées avec les SHA-256 du dépôt principal
à la clôture documentaire. Chaque tranche renvoie son manifeste `_meta/` gelé.

> [!important] Ce que cette clôture affirme — et ne lève pas
> Les suites normales comptent **215/215 cas distincts** (176 unitaires + 39 GL) et la
> sélection ASan exacte **198 cas** (176 + 22 GL : 9 `ShaderLifetimeComponentTests`
> historiques + 5 `PipelineTest` chargés + 8 nouveaux `PipelineLifetimeTest`) sur le candidat
> `f914d77`, zéro échec/skip *rapporté par CTest*. Cela **ne vaut pas** une conformité globale
> des oracles : le diagnostic GMock préexistant `AttributeTest.SetTest`
> (`AttributeTests.cpp:65`) a été reproduit **directement ET via CTest normal/ASan** malgré
> `exit 0` / `Test Passed`. Le bilan final indépendant est **14 PASS / 3 FAIL /
> 7 INCONCLUSIVE** (original immuable) ; ADR-0003 reste Accepted/partial ; les FAIL
> historiques (protocole RED A1c, pollution d'environnement I0) restent consignés.

## Résultat vérifié au périmètre

- **I0 — candidat gelé `18edc72`** (seam F8 et isolation hors parent TEMP hérité, corrigée en
  `b44ba0b` après RED causal) : union normale rejouée **175 CPU + 39 GL = 214/214**, zéro
  échec/skip, 129 empreintes d'entrée inchangées ; ASan intégré exact **175 + 22 GL = 197**,
  sélection déclarée avant exécution. Le FAIL distinct de restauration d'environnement
  appelant (ASAN_OPTIONS initialement absente restant définie au retour du wrapper) est
  reproduit par la QA I0 et confié à H-RESTORE ; FAIL historique RED A1c conservé. Rapport
  de QA : `out/mission-a1a2/i0-verification/report.json` (SHA-256 `14310521…`), 43/43
  artefacts recoupés. Manifeste : `docs/_meta/session-2026-10-16-pipeline-i0-validation.json`.
- **Candidat combiné `f914d77`** (six fichiers A2, octets conformes au GREEN producteur
  `1dfff22`, plus H-RESTORE `1a75c75`) : normal **176 unit + 39 GL = 215**, ASan
  **176 + 22 = 198 exact**, zéro échec/skip compté, **17 autres GL non sélectionnés**.
  Changement d'inventaire : **1 test getter moved-from ajouté** ; **2 cas alias (1 unit, 1 GL)
  archivés puis remplacés 1:1** par le contrat de destruction du contexte/shaders
  destination. 129/129 empreintes sources conformes, 85/85 artefacts recoupés. Manifeste :
  `docs/_meta/session-2026-10-16-pipeline-final-validation.json`.
- **Sondes getter-owner fraîches (contrôle de contrat post-implémentation, pas un RED
  historique)** : overlay byte-exact du `Pass.hpp` I0 compilé → exit **0** ; header final →
  exit **2** (`C2440` : conversion d'une référence de contexte en `shared_ptr`) ; contrôle
  référence → exit **0** ; codes de sortie vérifiés **numériquement** (un récapitulatif JSON
  antérieur à chaînes a été rejeté et conservé comme invalide :
  `a2-probe-summary-original-invalid.json`). Les interdictions de constructeur `Pass` à
  contexte partagé préexistaient : invariants, pas une preuve de la migration du getter.
  Preuve : `out/mission-a1a2/final-verification/getter-probes-fresh.json`.
- **H-RESTORE — 5 cas harnais réels (4 bras + Q1), oracle dans le même `powershell.exe`
  appelant** : bras `throw-absent-absent`, `throw-present-present`, `return-absent-absent`,
  `return-present-present` + Q1 hermétique ; échec Conan contrôlé (exit 1) après
  normalisation et succès `-ConfigureOnly`. RED causal 2/5 : les deux bras « absence »
  fuiyaient la valeur canonique au retour ou à l'exception ; GREEN 5/5 après le fix
  `1a75c75`. Ces 5 cas sont **distincts des 198** ASan du moteur ; les mixtes absence/
  présence n'ont pas été exécutés. Manifeste :
  `docs/_meta/session-2026-10-16-asan-caller-restoration.json`.
- **H-FIXTURE `95c2b9e` (runner du harnais corrigé, PM/LLM brute 65 → retenue 70 / risque 64)** :
  **9 tests unitaires de fixture** (+1 test depuis `f914d77`) et **5 contrôles réels du
  wrapper** réexécutés **indépendamment** (14/14 conformes) ; RED d'injection causal isolé
  avant correction sur fixture temporaire privée : la création du fichier fixture levant après
  déplacement du build laissait le build sous `.hold-hrestore`. Identités sources :
  **130 entrées conformes (128 inchangées + 1 runner modifié + 1 test de fixture ajouté)** ;
  cinq binaires de SHA identiques avant/après ; état du build restauré sans hold résiduel
  (ASan directory/probe/unit/component). **Les entrées moteur C++ n'ont pas changé : les
  215/198 ne sont PAS relancés et PAS recertifiés comme une campagne sur `95c2b9e`** — pas
  de claim de nouvelle campagne 198 sur ce head ; preuve de portée par **delta et identité**,
  pas par réexécution. Manifeste : `_meta/session-2026-10-16-pipeline-final-validation.json`
  (§ `post_fixture_verification`), rapport `out/mission-a1a2/post-fixture-verification/report.json`.
- **Le harnais de mission est 9 + 5 = 14 cas de scripts, distincts des 215/198 moteur** :
  jamais un total « 229 tests moteur ».

## Revues et QA indépendantes

- **R-I0 (correctness-reviewer fresh, `task_mvbcv58i_004e_twggrf`, pool-9)** : 24 fichiers du
  diff, 129/129 empreintes conformes, aucun défaut bloquant du code A1 identifié (lecture
  statique, pas une preuve exhaustive). Deux P3 non bloquants : restauration ASAN_OPTIONS
  (traité ensuite par H-RESTORE), question `removeError`/liens du helper TEMP. FAIL
  historique A1c conservé et transmis tel quel à V-I0.
- **V-I0 (rendering-verifier fresh, `task_mvbcv58j_004f_8z2wcx`, pool-10)** : **9 PASS / 2
  FAIL / 2 INCONCLUSIVE** sur 13 critères. FAIL exécuté : restauration d'environnement
  reproduite par invocation réelle du wrapper. 214/214 et ASan 197 exact, canari 0/86,
  options strictes. Artefacts 43/43 recoupés. Sources 129/129 identiques avant/après.
- **R-A2 (fresh, `task_mvbguqgw_005u_sqbmz7`)** : 8 fichiers finals de diff, 129/129 sources
  conformes, aucun défaut nouveau C++ identifié par lecture statique, aucune exécution.
  Deux constats qualifiés : **P2 fixture du nouveau runner H-RESTORE**
  (`scripts/test_asan_env.py:154-170,391,404-405` — rename en `.hold-hrestore` puis écriture
  levante avant du retour du helper, `held` non remis : le finally de l'appelant ne peut
  pas restaurer le build original) ; **oracle GMock préexistant** `AttributeTest.SetTest`
  aussi présent dans un `LastTest.log` CTest I0 malgré `Test Passed`, pas seulement en
  exécution directe (correction de périmètre antérieure conservée).
- **V-A2 — QA finale sur `f914d77` (`task_mvbguqgx_005v_xl1gnj`)** :
  - **24 critères : 14 PASS, 3 FAIL, 7 INCONCLUSIVE** ; bilan original immuable conservé.
  - Comptages : 215/215 normaux et 198 ASan, 0 fail / 0 skip comptés ; **réserves
    sémantiques préservées** : l'oracle `ATTRIBUTE_ORACLE_FAIL` est **FAIL**
    (`AttributeTests.cpp:65` : `EXPECT_CALL(*MockSetter<int>::instance(), mockOn(_))`,
    attendu une fois, observé jamais, constaté après le résumé « ALL SUITES PASSED » et
    `exit 0` — reproduit **directement (176)**, **ET** sous CTest normal individuel, **ET**
    dans le `LastTest.log` ASan final) ; les deux FAIL historiques sont retenus
    (`HISTORICAL_A1C_RED_PROTOCOL`, `HISTORICAL_I0_CALLER_POLLUTION`).
  - **Sortie JUnit des campagnes tronquée à 1024 octets** (`system-out`, `failures=0`) :
    l'absence de diagnostic dans ces XML **ne prouve pas** l'absence du défaut. Un témoin
    séparé fixé à **65536** (`normal-attribute-settest.xml`, per-test) conserve le
    diagnostic ; `asan-LastTest.log:2494-2501` et l'extrait
    `final-verification/asan-attribute-settest-excerpt.txt` le prouvent aussi sur ASan.
    **Ne pas annoncer « toutes les suites conformes » ni « PASS global »**, ni un défaut
    « direct seulement ».
  - Autres preuves : canari 0/86 avec 36 TU/objets instrumentés ; sondes getter 0/2/0 ;
    **5 contrôles H-RESTORE** réels passés ; options d'activation Q1 hostiles refusées
    **1/1** (fail-close attendu) ; GPU réel RTX 3090 / OpenGL 3.3.0 / driver 617.42 ;
    **premier échec ASan exit 8 avant toute exécution** : chemin relatif `tests-from-file`
    résolu contre le répertoire de build, réinvocation en absolu → 198/198 — **invocation,
    pas RED moteur** (log original conservé, seule l'invocation change) ; état du build ASan
    (probe/unit/component) restauré sans directory `.hold` après les contrôles wrapper.
  - Verdict global : *"NO GLOBAL PASS: A2-specific observed contracts conform, preexisting
    mock oracle FAIL and historical FAILs preserved; fixture-fault QA remains
    unexercised"*. Les critères passants : `IDENTITY`, `TOOLCHAIN_NORMAL_BUILD`,
    `NORMAL_ACCOUNTING`, `I0_TEST_DELTA`, `ASAN_CLEAN_POLICY_CANARY`, `ASAN_ACCOUNTING`,
    `FIRST_INVOCATION_PRESERVED`, `GETTER_COMPILE_CONTRACT`, `A2_CPU_CONTRACTS`,
    `A2_OPENGL_LOADED_OWNER`, `PIPELINE_F10`, `HRESTORE_SAME_CALLER`, `Q1_FAIL_CLOSED`,
    `BINARY_RUNTIME_WITNESS`. INCONCLUSIVE : `HRESTORE_FIXTURE_FAULT_PATH`,
    `READER_REMOVEERROR_DANGLING`, `F8_CLOSE_OS_FAILURE`, `OTHER_TOOLCHAINS_RELEASE`,
    `VISUAL`, `PERFORMANCE`, `VALIDATION_LEVEL_REFERENCE`.
- **V-FIXTURE (fresh, `task_mvbhptmn_006j_pzsc0a`, produit `4d1b655`)** : 21/21 artefacts et
  **130/130 empreintes source recoupées** ; verdict P2 **VERIFIED** avec limites explicites,
  **global NONE**. L'injection de faute est une fixture TEMP privée : elle prouve le chemin
  d'erreur **Python**, pas une panne OS réelle d'interruption brutale. La fenêtre
  signal entre le rename et l'assignation d'état et le TOCTOU restent bornés par la
  possession exclusive du build ; aucune atomicité filesystem promise. Le mécanisme
  auto-restaure le build ou rapporte explicitement l'échec de rollback ; le résidu
  `Locked` du passé H-ASan reste une preuve de mécanisme suffisant, pas le déclencheur
  historique du 21:00 de la mission C.

## Journal des filières, intégrations et commits

Base B0 `a50f466` : sélection utilisateur de la mission, fiche G-A ratifiée. La liste
ci-dessous recense les producteurs et les barrières ; les preuves et limites détaillées sont
dans les manifestes `_meta/`.

- **DOC-REF (`cc48c4e`)** : photographie `current-state` et `build-and-test` à l'état mission C
  (étape antérieure à la tranche, conservée telle quelle).
- **S-GSS — `getStackTraceString` par valeur** : RED `befb5a8` (test seul) → fix `36e4a34`,
  intégré en `433bdbc` + doc `0998f05`. RED double réel : ASan strict `stack-buffer-overflow`
  sur le temporaire de 23 caractères (hors SSO 15), lu depuis `TraceableExceptionTests.cpp`,
  options strictes ; sonde compile-time hors dépôt `C2338` sur la signature `const&`. GREEN
  2/2 ASan, suite 167/167 (136 unit + 31 GL, 0 skip), canari 0/86. Premier consommateur de
  l'accesseur : tests uniquement, zéro appelant de production ajouté — défaut resté dormant
  en production. **Latent OPEN découvert par S-GSS** : branche `#if` (libstdc++),
  `getStackTraceString() const` appelle `format_stacktrace` membre non const — la première
  instanciation réelle de cette branche ne compilerait pas ; branche inatteignable sous
  MSVC et non compilable sur GCC MSYS2 13.2 (sans `std::stacktrace`). Coupe dédiée
  recommandée avant toute exposition sous GCC/libstdc++.
- **H-ASan — normalisation Q1 et héritage hostile (`eb8659e` → intégré `760f4f8`, doc
  `c1254b6`)** : normalisation du harnais sous `-AddressSanitizer` avant tout enfant
  instrumenté ; canari fail-close sur héritage hostile ; `inherited_asan_options` consigné.
  PREUVE RED : shim au point d'appel ctest — env hostile permissive (`exitcode=0`,
  `log_path` redirigé) transmise tel quel, campagne 134/134 « PASSED » et harnais exit 0,
  probe use-after-free → exit 0 (masquage mécaniquement prouvé) ; un `log_path` malformé
  est rejeté par le parsing du runtime (fail-close). GREEN : observateur canonique au point
  ctest, campagne 134/134 honnête. Module normalisateur inchangé depuis
  (`asan-env.ps1` SHA `2b81ab49…`).
- **D-A1 (pool-1, producteur `fc0254a`) → intégration I-A1 `d319096`, doc `11e4400`** : premier
  build union échoué au lien `LNK2019 __std_rotate` dans l'archive locale `gmock_main.lib`
  (`out/mission-a1a2/i-a1-first-link-fail.log`). Expérience discriminante : reconstruire
  `gmock_main` dans l'environnement courant (archive `d24dbf3a…` → `0cd5fc45…`, référence
  UNDEF éliminée) sans aucun changement de source moteur → **167/167 unit** (134 baseline +
  31 D-A1 + 2 S-GSS). Classification : incident d'artefact d'intégration, **pas un RED
  causal moteur** ; l'historique de compilation exact de l'archive reste inconnu. Aucune
  preuve GL ou ASan tirée de cette expérience.
- **Contrats passés en revue** : F8 ratifié (`101ddcb`), plan GPU/QA (`4287cb7`, `3c17f0c`,
  `1759a5f`, `40a7cc3`), I-A1 proof (`11e4400`).
- **D-A1c (pool-5, RED `df35b3f` → GREEN producteur `3526ead`) → intégration `3c670c9`, doc
  `407c578`** : RED posé par la sonde « members-owner détruits après teardown » ; GREEN
  intégré : **39/39 GL** rejoués sur le principal après reconfiguration/découverte fraîche,
  sélection producteur ASan **13/13** (5 `PipelineTest` devenus chargés + 8 nouveaux
  `PipelineLifetimeTest`), 9 historiques non rejoués à cette étape. Empreintes 2/2 sources,
  18/18 artefacts producteur. **FAIL historique du protocole RED** (`df35b3f`) confirmé par
  l'auteur : `ContextCheckedOwner::~ContextCheckedOwner` (`ProgramTests.cpp:69-80`) vérifie
  le drapeau CPU puis appelle la libération après `TearDown()` — contrairement au protocole
  « aucun appel GL volontaire après destruction du contexte » ; **sonde dangereuse
  INTERDITE de réexécution**, comportement driver/WGL silencieux non qualifiant, GREEN
  final ne contient plus ce chemin, **pas de requalification CPU-only**. Addendum :
  `out/mission-a1a2/pool-5/A1c-evidence-qualification-addendum.md`
  (SHA `37052b92…`). Manifeste :
  `docs/_meta/session-2026-10-16-pipeline-a1c-validation.json`.
- **F8-SEAM (pool-7)** : ruling G-F8 ratifié (`101ddcb`), exigence d'isolation des fixtures
  héritées (`f8a18a8`), runner QA séparé (`e4b0aa7`), demande utilisateur de retrait des
  alias (`79596ca`) ; RED causal d'ascendance de fixture (le `TearDown` ancien supprimait
  récursivement le parent `%TEMP%/artist-reader-tests`) → candidat corrigé `b44ba0b`,
  intégré dans I0 `18edc72`. Preuve producteur : unit 175, filtre ASan 16 (12 ReaderTests +
  4), canari 0/86, sauvegardes `out/mission-a1a2/pool-7-final-b44ba0b/` et
  `out/mission-a1a2/pool-7-original-0e4f113/`. La transaction d'extraction réelle est
  prouvée (4096 octets, sentinel, traduction exacte, contexte vidé, `bad_alloc` non
  traduit) ; fermeture échouée et panne filesystem réelle restent non prouvées.
- **I0 (`18edc72`, docs `0948cbd`, ratification A2 `67c1c0d`, revue `59c9b6d`, QA `b4b7e87`)** :
  candidat gelé, union complète, R/V fresh terminés, alias SUBST privés retirés.
- **H-RESTORE (pool-11, producteur `14d5c15`) → intégré `1a75c75`, doc `83d7bdd`** : wrapper
  `scripts/build-windows.ps1` + `scripts/test_asan_env.py` (module `asan-env.ps1` inchangé,
  SHA `2b81ab49…`) ; sauvegarde des deux variables (absence/null compris) à l'entrée du
  wrapper sous `-AddressSanitizer`, restauration en `finally` sur retour et exception ;
  politique enfants inchangée. RED `red-unfixed-c2` : 2/5 échecs causaux (les deux bras
  d'absence) ; GREEN 5/5. 60 fichiers de preuves sauvegardés ;
  `out/mission-a1a2/h-restore-pool-11/` et manifeste coordinateur. Aucun test réel lancé sur
  le principal pendant le bail Conan A2.
- **D-A2 (pool-12, producteur `1dfff22` → intégré `f914d77`)** : six fichiers, étapes A2a/A2b
  chez le même écrivain ; RED causal `MovedFromPassContextGetterThrows` (ancien retour
  partagé vide sans exception) ; GREEN minimal ; adaptations mécaniques des consommateurs
  chez l'écrivain (cibles GL temporairement non adaptées jamais présentées comme GREEN
  global) ; 2 cas alias unit/GL archivés puis remplacés 1:1 ; livraison avec manifeste
  coordinateur (`out/mission-a1a2/a2-delivery-coordinator-manifest.json`) et 53 artefacts
  figés.
- **Barrières documentaires** : `2ace3ba` (snapshot candidat avant vérification finale),
  `e4448e6` (réserves R-A2 conservées + correction du périmètre de l'oracle GMock),
  `9130076` (reconciliations accounting QA finale), `738891f` (rollback fixture vérifié
  indépendamment + gate de clôture documentaire).
- **H-FIXTURE (pool-15, producteur `4d1b655`) → intégré `95c2b9e`, vérifié par
  `task_mvbhptmn_006j_pzsc0a`** : deux fichiers Python (runner `scripts/test_asan_env.py`
  modifié, nouveau `scripts/test_asan_env_fixture.py`, 9 cas) ; à aucun moment le cache
  Conan/GPU du QA n'a été partagé.

Les barrières du coordinateur (I-A1, I0, gate avant/après D-A2, V-FIXTURE) ont été
vérifiées **sur empreintes et diffs**, pas sur la fin de travail déclarée d'un auteur.

## Statuts par finding

| Finding | Contrat observé à la clôture (`f914d77`/`95c2b9e`) | Preuve | Statut |
|---|---|---|---|
| F1/F2/F3 (Shader→Pass) | Conservés — handles zéro par défaut, re-loads stricts ; `IShader::getContext()` reste une référence gardée. **F3 actualisé par A2** : `IPass::getContext()`/`ctx()` retournent désormais une **référence gardée** (`FREED_CONTEXT`), plus une copie de `shared_ptr`. | `IShader.hpp`/`Pass.hpp` à `f914d77` | Conservé + actualisé |
| F4 — bornes | **A1b complète le curseur** : `getPass` garde conservée ; `use(index)` valide l'index **avant** mutation, rollback curseur CPU si le hook lève (aucun rollback backend) ; invariant `c ∈ {-1} ∪ [0,N)` ; `setCurrentPass` valide avant mutation (−2/N/INT_MIN/INT_MAX rejetés). | `Pipeline.hpp`, `PipelineContext.hpp`, tests `use(index)` | **Traité** (accesseur → curseur inclus) |
| F5/Q1 borne log | Conservé : capacité `[1, 4096]`, `written` borné, exceptions métier inchangées. | `shader/Loader.hpp:82-83`, `pass/Loader.hpp:97-98` | Traité (mission C) |
| Q3 — publication | Conservé : publication différée, un seul `setPassID`, `glDeleteProgram` unique pré-publication. | `pass/Loader.hpp:30-69` | Traité (mission C) |
| F6 — contexte IPass unique | **TRAITÉ (A2/F6)** : `IPass::m_context` en `unique_ptr` ; `getContext()` référence gardée; self-move no-op ; assignation libère le programme destination puis **détruit le contexte destination et ses shaders** (aucun alias ne les prolonge) ; emprunts source valides après transfert ; source moved-from rejette les accès. Sondes de compilation : ancien header (overlay I0) 0, header final **2** (`C2440`), contrôle référence 0. | `Pass.hpp`, `out/mission-a1a2/final-verification/getter-probes-fresh.json` | **Traité**, ADR-0003 partiel actualisé |
| F7 — UniformReader | **Différé, hors mission** : `OpenGLPassFlow` n'exige toujours pas `UniformReader` (absence silencieuse sous `if constexpr`) ; concept négatif à écrire dans la tranche uniform/attribute, avec l'adoption explicite d'ADR-0004. | `PassConcept.hpp` | Différé |
| F8 — lecture fichier | Seam d'extraction testée (badbit réel, sentinel, traduction exacte, `bad_alloc` non traduit) ; **fermeture échouée et panne filesystem réelle non prouvées** ; `removeError`/liens : question INCONCLUSIVE. | `ReaderTests.cpp` (I0), meta I0 | **Partiel**, inchangé sur les branches non prouvées |
| F10 — `glUseProgram(0)` | Conservation documentée : `use()` sur programme zéro appelle `glUseProgram(0)` (unbind explicite). `reset()` = curseur −1, **aucun unbind**. | `UserTests.cpp` (caractérisation conservée) | Conservation |
| S-GSS | **TRAITE** : retour par valeur, branches alignées, tests RED/GREEN ; latent `#if` libstdc++ (const-correctness) **OPEN**, hors mission, non compilable sous GCC 13.2. | meta `pipeline-i0` + `TraceableException.hpp:53-59` | Traité + latent OPEN |
| Q2/Q4/Q5 | Info, inchangés (non observés/non testés). | `known-incidents` | Info |
| H-ASan Q1 harness | **TRAITE** : normalisation canonique avant tout enfant, fail-close hostile, `inherited_asan_options`. | `asan-env.ps1`, `test_asan_env.py` | Traité |
| H-RESTORE | **TRAITE (`1a75c75`)** : sauvegarde/restauration des deux variables, absence compris, sur retour et exception ; 4 bras + Q1 séparés des 198. La **pollution I0 reste FAIL historique** (pas effacée par le GREEN). | meta `asan-caller-restoration` | Traité + historique conservé |
| H-FIXTURE | **TRAITE (`95c2b9e`)** : rollback exception-safe du build déplacé ; vérifié indépendamment 9 + 5 ; pas de relance moteur. | meta `pipeline-final-validation` § `post_fixture_verification` | Traité |
| Oracle GMock `AttributeTest.SetTest` | **OPEN, hors mission** : diagnostic après résumé, direct **et** CTest normal/ASan malgré `exit 0`/`Passed` ; JUnit campagne tronqué (1024), témoin per-test 65536. Priorité claire dans les incidents ; correction C++ interdite à cette mission. | `final-verification/asan-LastTest.log:2494-2501`, `normal-attribute-settest.xml` | OPEN, à ouvrir/follow-up |
| Latent `#if` libstdc++ | **OPEN** (hors mission, GCC inatteignable ici). | meta `pipeline-i0` | OPEN |
| `validation-levels` de référence | **ABSENTE, non restaurée** ; l'absence est qualifiée | `final-verification` § limits | Qualifiée, non restaurée |
| Fenêtres signal/TOCTOU et interruption FSM | Bornées par la possession exclusive du build ; **pas d'atomicité OS promise**. | `post-fixture-verification/report.json` | Limite consignée |

## Limites, réserves et non-revendications

- **Lifetime CPU/GPU** : l'invalidation des emprunts à l'assignation/destruction et le
  transfert d'owner unique sont testés **CPU** (et GL calendriers pour l'observation des
  handles, pas pour la destruction en vol). Le contexte GL **courant** et les ressources
  **en vol** (destruction différée d'un programme courant jusqu'à unbind, commandes GPU en
  cours) restent **non résolus** : aucun `glFinish`, aucun fence, aucun mécanisme de retraite
  nouveau. **Un handle seul ne résout pas la destruction en vol.**
- **Aucune image, draw, readback, mesure de performance, Release, Linux, CI distante,
  coverage, UBSan ou TSan validés. Pas de preuve d'UB exhaustif ni de RAII global.**
- **Les 17 autres GL ne sont pas sélectionnés sous ASan** (ni abandon) ; pas de campagne
  dédiée décidée.
- **Aucun PASS global** : oracle GMock Attribute préexistant FAIL conservé, dois FAIL
  historiques conservés ; les docs ne prétendent pas le contraire.
- **Pas de templatisation en handle générationnel, pas d'ADR-0004 adopté implicitement** ;
  pas d'extension vers les wrappers uniform/attribute.
- **A1c RED historique** : sonde interdite de réexécution ; FAIL du protocole conservé,
  non requalifié CPU-only.
- **Snapshots binaires post-campagne** (témoin avant/après stables directs) : aucun hash
  pré-build/pré-campagne, ce ne sont pas des empreintes rétrodatées.
- **Procédure strict B** : re-load sans free rejeté avant toute création ; `free(); load()`
  recompile le shader ; `pass.free()` ne libère pas ses shaders ; le cycle complet exige la
  libération de chaque shader puis `pass.load()` sous un contexte GL valide ; **pas de
  promesse `free(); load()` pass directe** (`IPass::load()` recharge d'abord les shaders).
- **Retraits d'alias** : S:/T: retirés ultérieurement, U:/Y:/Z: retirés en fin de mission,
  sortie `subst` vide, **aucun alias recréé**, aucun fichier/cache/worktree supprimé,
  preuves préservées (`out/mission-a1a2/alias-retirement.json`). L'utilisateur voulait
  terminer puis dégager les alias — ce point est fait.

## Candidatures ouvertes à la clôture — AUCUNE n'est sélectionnée

L'utilisateur n'a sélectionné aucune nouvelle mission. Les candidates suivantes restent
inscrites, sans décision :

1. **Gestion pré-existante ouverte — oracle GMock `AttributeTest.SetTest`** : défaut de
   suite préexistant hors mission (mock attendu zéro appel, diagnostic post-résumé).
   À **ouvrir et suivre** dans les incidents, avec priorité, sans modification C++
   de cette mission.
2. **ADR-0004** (layouts typés / bindings) : Proposed / not-started, adoption explicite
   requise.
3. **F7** : exiger `UniformReader` dans `OpenGLPassFlow` ou écrire le concept
   négatif signé (tranche uniform/attribute).
4. **F8-extraction complémentaire** : close-failure et panne OS réelle (INCONCLUSIVE),
   ou ruling d'ajournement.
5. **Latent `#if` libstdc++** de `getStackTraceString` (avant toute exposition GCC).
6. **Les 17 autres GL non sélectionnés sous ASan** (campagne dédiée à décider), Release/CI/couverture,
   performance et équivalence visuelle (ADR-0006).

## Navigation

- [[architecture/correction-plan|Plan de correction]] ·
  [[development/known-incidents|Incidents]] ·
  [[development/build-and-test|Build et tests]] ·
  [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
  manifestes : `_meta/session-2026-10-16-pipeline-i-a1-validation.json`,
  `_meta/session-2026-10-16-pipeline-a1c-validation.json`,
  `_meta/session-2026-10-16-pipeline-i0-validation.json`,
  `_meta/session-2026-10-16-asan-caller-restoration.json`,
  `_meta/session-2026-10-16-pipeline-final-validation.json`.
- La fiche [[architecture/next-mission|next-mission]] est un pointeur vers cette archive :
  aucune nouvelle mission n'est sélectionnée.

## Fiche d'exécution d'origine — copie intégrale (autorité de contrat)

> [!info] Copie intégrale
> La fiche d'exécution d'origine (sélection utilisateur du 2026-10-16, committée jusqu'à
> `738891f`) est recopiée ci-dessous sans modification ; elle reste l'autorité de contrat de
> la mission désormais terminée. Les statuts courants sont dans les sections supérieures de
> cette archive.

## Prochaine mission : Pipeline A1+A2 — le Pipeline devient propriétaire

**Sélectionnée par l'utilisateur le 2026-10-16 après consultation ; périmètre défini avec la
fiche de contrat préalable de l'architecte (G-A, 2026-10-16, ratifiée par le coordinateur).
Exécution engagée dans la mission native approuvée.** Cette fiche remplace celle de la
mission C ([[architecture/missions/2026-10-15-failure-contracts|archivée]]).

> [!info] Avancement vérifié, non clôture globale
> A1a+A1b intégrés en `d319096` ; I-A1 vérifiée sur l'union : 167/167 CPU,
> 15/15 empreintes sources conformes, manifeste
> `_meta/session-2026-10-16-pipeline-i-a1-validation.json`.
> A1c intégrée en `3c670c9` : 39/39 GL rejoués sur le principal et sélection
> producteur ASan 13/13 (pas les 9 GL historiques). Réserve conservée : la
> sonde RED historique de bail appelait le cleanup GL après teardown,
> contrairement au protocole ; elle n'existe plus dans le GREEN. Manifeste
> `_meta/session-2026-10-16-pipeline-a1c-validation.json`. État historique A1 ;
> pour A2 et les barrières finales, voir le candidat intégré ci-dessous.

> [!info] Candidat I0 intégré après F8
> `18edc72` inclut la seam et l'isolation hors parent TEMP hérité, corrigée en
> `b44ba0b` après RED causal. Union normale rejouée : **175 CPU + 39 GL = 214**,
> zéro échec/skip, 129 empreintes d'entrée inchangées. R/V fresh terminés sur ce
> candidat : QA normale **214/214**, ASan intégré **175 + 22 GL = 197/197**.
> Pas de blocage A1 identifié ; FAIL distinct de restauration env appelant
> reproduit et confié à H-RESTORE, FAIL historique RED A1c conservé. A2 et le
> correctif du harnais sont admis en branches isolées, pas encore validés.
> Manifeste : `_meta/session-2026-10-16-pipeline-i0-validation.json`.

> [!info] Candidat combiné A2/F6 + H-RESTORE intégré
> `f914d77` intègre les six fichiers A2, octets conformes au GREEN producteur,
> avec le correctif de restauration du harnais `1a75c75`. Producteur A2 :
> **176 CPU + 39 GL = 215** et sélection **198 ASan**, sans échec/skip ;
> sonde getter-owner ancien/final/contrôle recoupée **0/2/0**. Ces résultats
> ne remplacent pas une campagne du candidat combiné : R-A2 et V-A2 fresh
> admis après shutdown/merge, sources 129/129 conformes. Clôture documentaire
> encore retenue jusqu'à leurs verdicts ; aucun PASS global annoncé.
> Manifeste : `_meta/session-2026-10-16-pipeline-final-validation.json`.

> [!warning] Barrière finale R-A2
> Revue fresh reçue : aucun défaut nouveau C++ A2 identifié, mais **P2 du
> nouveau runner H-RESTORE** : si la création du fichier fixture échoue après
> déplacement du build, celui-ci peut rester sous `.hold-hrestore`. H-FIXTURE
> est admis en branche isolée (complexité brute65 → 70, risque64), sans GPU ni
> cache partagé ; RED d'injection sur fixture temporaire avant correction.
> Pas d'exécution du staging risqué sur le build réel avant intégration du fix.
> Autre réserve : le diagnostic GMock préexistant est aussi dans un LastTest.log
> CTest I0 malgré `Test Passed`, pas seulement en exécution directe. Absence
> dans les JUnit scannés ne signifie pas absence de diagnostic dans les logs.

> [!info] QA finale reçue sur `f914d77`, sans PASS global
> 215 cas normaux et 198 ASan comptés CTest sans échec/skip ; cinq contrôles
> H-RESTORE séparés, getter-owner frais 0/2/0. **14 critères PASS, 3 FAIL,
> 7 INCONCLUSIVE** : oracle GMock préexistant FAIL reproduit aussi sous CTest,
> deux FAIL historiques conservés ; fenêtre de faute fixture encore à corriger.
> 85/85 artefacts et 129/129 empreintes sources recoupés par le coordinateur.
> La QA est terminée et ses baux libérés. La clôture attend H-FIXTURE intégré
> puis sa vérification indépendante, sans élargir tacitement la mission au
> défaut Attribute préexistant. Rapport et limites dans le manifeste final.

> [!info] H-FIXTURE intégré et vérifié après la QA moteur
> `95c2b9e` : deux fichiers Python uniquement, neuf tests de fixture puis cinq
> contrôles réels du wrapper **réexécutés indépendamment**, tous conformes.
> 21/21 artefacts et 130/130 empreintes recoupés, cinq binaires et état du build
> identiques avant/après, aucun hold résiduel. Les entrées moteur n'ont pas
> changé : pas de relance GPU/215/198 sans nouvelle hypothèse. La clôture
> documentaire peut consommer ces résultats ; oracle Attribute et historiques
> restent des FAIL distincts, aucun PASS global. Les fenêtres signal/TOCTOU ne
> sont pas une garantie d'atomicité filesystem ou d'interruption brutale.

## Pourquoi cette mission

L'ordre adopté le 2026-10-15 : figer les contrats d'échec (mission C, terminée à
`e736bba`) **avant** le churn structurel de la tranche Pipeline, puis seulement
[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] avec adoption explicite.
C est fermée : 165/165 normaux, 143 ASan distincts, revue et QA indépendantes
PASS avec réserves consignées. ADR-0003 reste **Accepted / partial** : la
migration Shader→Pass est prouvée, le Pipeline non.

## Périmètre sélectionné (décision utilisateur du 2026-10-16)

- **Cœur** : A1a + A1b (un seul écrivain CPP) → A1c (preuves GL chargées) —
  contrat intégral ci-dessous.
- **A2/F6** : contexte unique de `IPass` — **incluse**, mais gated : fiche de
  contrat complémentaire de l'architecte **après** I-A1 et les preuves A1
  (décisions ouvertes listées §A2). Ne jamais fusionner les deux transferts
  d'ownership.
- **Branches latérales disjointes** : H-ASan (anomalie ASan incrémentale +
  durcissement harness), S-GSS (`getStackTraceString()` retour par valeur),
  DOC-REF (`current-state.md` + `build-and-test.md` à jour), F8-extraction
  (preuve d'extraction échouée via seam — ruling requis : petit hook production
  dans `Reader`, décision du coordinateur avec l'architecte).
- **Hors mission** : ADR-0004 (adoption explicite = sélection séparée), Q2/Q4/Q5,
  Release/CI/perf/image/ADR-0006, F8 exhaustif, signature exacte d'
  `OpenGLPassFlow`, les 22 autres GL sous ASan (sauf campagne dédiée décidée).

## Contrat G-A — intégral (autorité de la tranche A1)

### Faits de départ (vérifiés à `af14c88`)

- `Pipeline.hpp` : `~IPipeline()` déclaré (l.31) ; constructeurs
  `initializer_list<shared_ptr<IPass>>` + contexte partagé (l.39-50) ; `forPass`
  rend un `shared_ptr` (l.60-63) ; `hasNext()` additionne un au curseur
  (l.78-81) ; `useNext()` l.88-97 (appel ultérieur après épuisement → `reset()`) ;
  `use(index)` mute le curseur **avant** le hook (l.104-108) ; `getContext()`
  copie le `shared_ptr` (l.122-125) ; `Pipeline<API,PROFILE>` sélectionne ses
  composants statiquement (l.145-184).
- `PipelineContext.hpp` : `addPass()` accepte un nul (l.20-23) ; `getPass()`
  garde F4 de C2 (l.38-44) ; `setCurrentPass` sans garde (l.52-55) ;
  `m_passes` = `vector<shared_ptr<IPass>>` (l.63), curseur −1 (l.64).
- `pipeline/User.hpp` lit le curseur puis `IPass::use()` (l.20-26) ;
  `pipeline/Resetter.hpp` remet le curseur à −1 (l.20-26).
- `Pass.hpp` : non-copiable, movable noexcept (l.53-56) ; `load()` charge les
  shaders avant le programme (l.97-109) ; `getContext()` partage encore le
  contexte (l.184-187) — déviation nommée F6 ; destructeur concret libère le
  programme (l.262-268).
- Consommateurs : `PipelineTests.cpp` (4 tests), `PipelineContextTests.cpp`
  (6 tests F4 de C2), `UserTests`/`ResetterTests`, mocks pipeline,
  `MockPipelineContext.hpp` (hérite du stockage réel),
  `ProgramTests.cpp` (5 tests Pipeline, passes **non chargées** en membres,
  détruites après teardown du contexte — succès non qualifiant, cf.
  known-incidents).

### A1a — stockage/emprunts du wrapper

- `IPipeline::m_context` → `unique_ptr<API::PipelineContext>` ; injection par
  `unique_ptr` (rejet nul : `ERROR::PIPELINE::NULL_CONTEXT`,
  `invalid_argument`).
- `getContext() const` → `API::PipelineContext&` (précédent `IShader::getContext`) ;
  aucun accesseur partagé conservé.
- Wrapper non-copiable ; construction move explicite (transfert du pointeur,
  sans copier le sous-objet base) ; assignation move exposée par le concret
  avec base protégée ; self-move strictement no-op ; l'assignation détruit les
  anciennes passes destination avant remplacement ; accès moved-from rejeté
  (`ERROR::PIPELINE::FREED_CONTEXT`, `runtime_error`) ; aucun destructeur de
  base n'appelle un hook virtuel.
- Composants User/Resetter : `on(Context&)` canonique (`HasBorrowedOnMethod`
  dans `OpenGLPipelineFlow`) ; fines surcharges `on(shared_ptr<Context>)`
  **conservées** (nul → même type + même message
  `ERROR::SHADER::NON_VALID_CONTEXT`, sinon délégation, aucune rétention) —
  erreurs publiques préservées sans partager le chemin migré.
- `PipelineContext` ni copiable ni déplaçable.

### A1b — `m_passes` unique_ptr, `forPass` emprunt, bornes/curseur

- Constructeurs consomment `vector<unique_ptr<IPass>>` ; suppression des
  constructeurs `initializer_list<shared_ptr<IPass>>` (rupture franche,
  assumée).
- `addPass(unique_ptr<IPass>)` : rejet du nul avant insertion
  (`ERROR::PIPELINE::NULL_PASS`, `invalid_argument`) ; vérification de
  cardinalité avant insertion (`ERROR::PIPELINE::PASS_COUNT_LIMIT_EXCEEDED`,
  `length_error`, « Pass count cannot exceed {INT_MAX} ») — testée par
  prédicat isolé, JAMAIS par allocation réelle ; un ajout ne change pas le
  curseur.
- `getPass(index)`/`forPass(index)` → `IPass&` (jamais un owner) ; conservation
  C2 : `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` (`out_of_range`).
- Curseur : invariants `c ∈ {-1} ∪ [0,N)`, `N ≤ INT_MAX` ; `setCurrentPass`
  accepte −1 et [0,N), rejette tout autre entier **avant mutation** (−2, N,
  INT_MIN, INT_MAX) ; `use(index)` : contexte possédé vérifié → validation de
  l'index **avant mutation** → mémoriser l'ancien curseur → positionner le
  candidat → hook emprunté → si le hook lève, restaurer l'ancien curseur et
  relancer inchangé (rollback CPU, AUCUN rollback backend promis) ; `use(−1)`
  invalide.
- `hasNext` : vide→false ; initial avec N>0→true ; dernière sélectionnée→false ;
  sans addition non protégée. `useNext` : sémantique publique conservée — vide→
  reset+false ; suivante→`use(c+1)` puis s'il en reste ; dernier appel utile
  laisse le curseur sur la dernière passe ; appel après épuisement→reset+false ;
  échec de `use` conserve l'ancien curseur. `reset` : curseur −1 seulement,
  aucun `IPass::use()`, aucun `glUseProgram(0)` (F10 reste une conservation :
  reset ≠ unbind).
- Emprunts : n'étendent aucun lifetime ; l'ajout réalloue le vecteur mais ne
  déplace pas les objets passes ; le move conserve les adresses ; l'assignation
  move invalide les emprunts destination ; aucun effacement/recyclage d'index ;
  sérialisation par l'appelant.

### A1c — lifetime CPU/GPU chargé (après barrière I-A1)

- `ProgramTests.cpp` : owners locaux au corps du test, créés **après** `SetUp()`
  et détruits avant `TearDown()` (ou teardown dérivé détruisant les owners avant
  le contexte GL) ; passes **chargées** avant transfert au Pipeline ; aucun alias
  partagé de `PassContext` vivant pour ces preuves ; handles GL copiés pour
  observation, jamais owners.
- Preuves attendues : programme créé et lié ; `use(i)` sélectionne le programme
  attendu (`GL_CURRENT_PROGRAM`) ; `reset()` remet le curseur sans détruire le
  programme ni unbind ; unbind explicite **par le test** avant destruction puis
  `glIsProgram` ; moves de Pipeline chargé (transfert d'identité, destination
  possédante, self-move, source déplacée rejetée) ; témoin CPU de bail
  (owner détruit → contexte détruit) complémentaire, jamais substitut GL.
- OpenGL peut différer la suppression d'un programme courant : aucun `glFinish`,
  fence ou mécanisme de retraite nouveau ; la suppression observable n'est pas
  une preuve exhaustive de fuite driver.

### Frontières et hors-périmètre

- Compile-time : appelabilité des composants empruntés (`HasBorrowedOnMethod` —
  pas une preuve de signature exacte). Link/cook : rien de nouveau. Runtime de
  préparation : registre de passes et transferts. Runtime d'exécution : curseur,
  contrôle d'index, invocation polymorphe par activation de passe — aucun
  dispatch par draw.
- UniformReader/AttributeReader/F7/ADR-0004 hors coupe ; `glFunctionMock.hpp`
  intact ; strict B conservé ; pas de changement de profil, dépendance,
  `noexcept` existant ; erreurs publiques préservées ; aucune sécurité
  concurrente nouvelle ; aucune performance revendiquée.

### Partition des fichiers (un seul écrivain)

- **Écrivain CPP unique pour A1a puis A1b** (même auteur, deux cycles TDD) :
  `Pipeline.hpp`, `PipelineContext.hpp`, `component/pipeline/{User,Resetter}.hpp`,
  `validator/PipelineConcept.hpp`, mocks `MockUser`/`MockResetter` +
  `MockPipelineContext` vérifié, `PipelineTests.cpp`, nouveaux
  `PipelineOwnershipTests.cpp`/`PipelineCursorTests.cpp`/
  `PipelineBorrowedContractTests.cpp`, `PipelineContextTests.cpp`,
  `UserTests.cpp`, `ResetterTests.cpp`, et `ProgramTests.cpp` en **adaptation
  mécanique d'API seule** (compilable, sans chargement).
- **Écrivain GPU pour A1c** après I-A1 : `ProgramTests.cpp` (transféré à la
  barrière) + nouveau `component-tests/src/graphic/pipeline/PipelineLifetimeComponentTests.cpp`.
- CMake de tests en `GLOB_RECURSE` : reconfiguration obligatoire après chaque
  ajout et à chaque barrière (sinon découverte incomplète — incident C2 :
  153/153 pré-glob invalide remplacé par 165/165).

### RED causaux attendus

- A1a : move réel (source déplacée sans accès — l'ancien code copie la base) ;
  copie interdite (sondes de traits + compilation négative séparée) ;
  `getContext()` par référence ; composants positifs `Context&` / négatifs
  shared-only ; destruction unique sans appel virtuel pur depuis la base.
- A1b : `addPass(nullptr)` avant insertion (compte+curseur conservés) ;
  `setCurrentPass` −2/N/INT_MIN/INT_MAX sans mutation ; `use(index)` invalide →
  aucun User appelé + curseur conservé (RED causal de l'ordre actuel) ; hook
  levant → restauration ; vide/singleton/deux passes/dernier utile/appel
  supplémentaire ; identité `forPass` stable après croissance ; destruction
  unique au remplacement.
- A1c : témoin CPU du bail — la version « membre détruit après teardown » échoue
  causalement ; assertions de programme chargé échouent face aux fixtures non
  chargées actuelles ; aucun appel GL volontaire après destruction de contexte.

### Tests existants dont le sens change

`PipelineTest.UseTest` (curseur + identité), `IteratePassesTest` (fin vs reset
ultérieur), `ResetPipelineTest` (plus de `{nullptr}`), `GetContext` (référence +
identité), les 6 `PipelineContextTests` F4 (owners uniques, adresses),
`PipelineUserTests.useProgram_ValidContext` (passe enregistrée avant
`setCurrentPass(0)`), `ResetterTests.resetProgram_ValidContext` (plus d'index 42),
2 tests `NullContext` des composants (valides via les façades), les 5 tests de
`ProgramTests.cpp` (programmes chargés — `UsePassTest` ne caractérise plus
implicitement F10, le test F10 dédié reste).

## A2/F6 — contrat complémentaire ratifié, exécution après I0/R/V

Consultation G-A2 en lecture seule, puis décisions du coordinateur : **un seul
écrivain CPP pour toute A2**, deux étapes chez le même auteur ; pas de handoff
au milieu d'une rupture d'API. Rejet des nuls dans les maps uniform/attribute
**hors A2**. Archiver la caractérisation de l'ancien alias, ne pas conserver
un test devenu incompatible ni affaiblir la preuve du nouveau contrat.

Ce contrat ne décrit **pas** le candidat I0 : son `IPass` reste partagé.
Publication de D-A2 seulement après revue/QA I0 reçues et arbitrées, producteurs
intégrés, ressources libérées. Nouvelle revue et QA indépendantes **après A2**,
avant DOC-F/I1. ADR-0003 reste Accepted/partial ; ADR-0004 Proposed/hors mission.

### Stockage, accesseur et lifetimes

- `IPass::m_context` devient `unique_ptr<API::PassContext>`. Les signatures de
  construction (déjà `unique_ptr`), rejet NULL_CONTEXT, copie interdite,
  construction move noexcept et self-move no-op sont conservés. Pas de
  destructeur de base appelant un hook virtuel.
- `getContext() const` devient une référence `API::PassContext&`, gardée par
  `ctx()` ; moved-from lève l'erreur FREED_CONTEXT existante, exacte. Aucun
  accesseur partagé de repli. Assignation de base protégée et cleanup natif
  destination avant transfert conservés : après succès de `free()`, remplacement
  du contexte destination détruit ses shaders. Si cleanup lève avant transfert,
  pointeurs non transférés ; aucun rollback backend promis.
- `free()` remet le handle programme à zéro sans détruire le contexte ni ses
  shaders : emprunts CPU encore valides. **Ne pas promettre `free(); load()`
  directement** : `IPass::load()` recharge d'abord les shaders (`Pass.hpp:97-110`)
  et strict B reste applicable. Le cycle complet exige aussi de libérer les
  shaders concernés, comme avant A2 ; pas de rollback global ajouté.
- Construction move : adresses et emprunts source conservés par le nouvel
  owner ; accesseurs gardés de la source déplacée rejetés, `free()` moved-from
  reste no-op comme avant (pas de nouvelle erreur). Assignation : emprunts vers l'ancien
  contexte destination invalidés, emprunts source conservés. Destruction :
  programme libéré, contexte détruit, shaders détruits ; contexte GL courant
  requis. Aucun emprunt invalide déréférencé dans les tests.
- Maps uniform/attribute et contextes de leurs wrappers restent partagés,
  inchangés.Ces owners CPU externes peuvent survivre ; leurs métadonnées GL
  peuvent être obsolètes après free/remplacement/destruction du Pass. Ne pas
  promettre leur usage GPU après invalidation ni ajouter handle générationnel,
  réflexion ou validation de binding ADR-0004/F7. Aucun lifetime GPU en vol résolu.

### Partition et preuves

- Écrivain CPP unique : `ARTIST/include/graphic/pipeline/Pass.hpp`, unitaires
  `PassTests.cpp`/`ShaderLifetimeTests.cpp`, composantes GL
  `ShaderLifetimeComponentTests.cpp`, `ProgramTests.cpp` et
  `PipelineLifetimeComponentTests.cpp`. Mocks à vérifier ; aucune modification
  supplémentaire sans constat de compilation et arbitrage du coordinateur.
  Pas de changement Shader/Loader/contexts/GLSL/glFunctionMock/harness.
- Étape A2a : RED causal de `getContext()` moved-from (ancien retour partagé
  vide sans exception), contrôle positif de compilation puis sondes distinctes
  retour-référence/copies interdites/absence d'ancien accesseur owner ; GREEN
  minimal. Adaptations mécaniques des consommateurs chez le même écrivain,
  sans présenter les cibles GL temporairement non adaptées comme GREEN global.
- Étape A2b : conserver les preuves GL chargées A1c (owners body-local avant
  teardown, handles observés par valeur, aucun alias vivant). Archiver avant
  réécriture le cas historique « alias prolonge les shaders » ; après migration,
  prouver la destruction destination sans alias, le transfert d'identité,
  self-move, moved-from, binding et suppression différée jusqu'à unbind.
  Programme courant et contexte CPU vivant sont des critères distincts.
- La réserve RED A1c `df35b3f` reste consignée : pas de réexécution de sa sonde
  chargée après teardown, pas de requalification rétroactive en CPU-only.
  Une caractérisation déjà verte n'est pas un nouveau RED ; un défaut découvert
  reçoit sa propre preuve causale avant correction.
- Suites normales CPU puis GL réelles, découverte/JUnit frais et empreintes ;
  ASan sélection déclaré avant run, clean-first et sonde reconstruite/canari
  0/86. Ne pas réduire les 9 cas GL historiques ni la sélection Pipeline A1c
  sans décision explicite. GPU/build exclusifs pendant la phase concernée.
- Aucun alias `subst` à recréer. Si chemins longs : sorties de build courtes
  et distinctes sous `out/` du principal ou le répertoire temporaire OpenCode
  approuvé, ressources attribuées par le lead ; conserver l'isolation des
  sources. Réutiliser tooling/cache Conan existants sous bail, pas les copier.

### Évaluation et fermeture

PM/LLM local : D-A2 fusionne les deux étapes d'auteur proposées par G-A2,
complexité brute/retenue **78/78**, risque **80** (ownership, rupture d'accesseur,
matrice CPU/GL et cleanup). Ce regroupement n'unit pas A1 et A2 et réduit les
handoffs/builds ; aucune sélection de modèle hors grille approuvée. R-A2
**55/70**, V-A2 **58/72**, fresh, après intégration A2 ; pas de nouveau code de
test/runner par les reviewers. Incertitudes à lever : ordre de destruction
chargée et exceptions, preuve de non-prolongation par l'ancien accesseur.
DOC-F attend ces verdicts, garde les réserves historiques et actualise F6/ADR
uniquement au périmètre effectivement prouvé. Aucun gain perf/image revendiqué.

## F8-SEAM — contrat complémentaire ratifié le 2026-10-16

Ruling G-F8 reçu en lecture seule de l'architecte, puis ratifié par le
coordinateur après lecture de `Reader::on` et du setter virtuel de
`ShaderContext`. Ce contrat autorise uniquement les deux fichiers ci-dessous ;
aucune preuve d'exécution F8 supplémentaire n'est acquise à la ratification.
ADR-0003 reste `Accepted / partial`, ADR-0004 `Proposed / not-started` et hors
mission. Le constat H-TEMP-LOCK reste un mécanisme suffisant, pas une cause
historique établie.

### Propriété, partition et évaluation

- Un seul `cpp-engineer` :
  `ARTIST/include/graphic/opengl/pipeline/component/shader/Reader.hpp` et
  `ARTIST/tests/src/graphic/opengl/pipeline/component/shader/ReaderTests.cpp`.
  Aucun changement Shader/Loader/contexte, profil, dépendance ou harness ;
  aucun chevauchement des fichiers A1c. Documentation au coordinateur.
- Deux cycles chez le même écrivain car ils partagent `ReaderTests.cpp` :
  hygiène de fixture, puis extraction testable. Lecture/édition CPU parallèles
  à A1c, mais les anciennes fixtures `%TEMP%/artist-reader-tests` demandent un
  bail du coordinateur avant toute campagne les utilisant. GPU reste à A1c.
- Évaluation PM/LLM locale : complexité brute **68**, retenue **70** (plancher
  développement), risque **55**. La seam, l'unwind iostream et les préconditions
  de fixture interagissent ; périmètre de préparation CPU borné à deux fichiers.
  Incertitude : comportement exact de la STL lors d'une exception d'extraction,
  à établir par compteurs/état du flux, pas par hypothèse. Scores heuristiques.

### Cycle 1 — fixture TEMP, RED causal requis

Répertoire possédé par chaque fixture, acquis par `create_directory` avec
collisions traitées ; timestamp/random seuls ne prouvent pas l'exclusivité.
Contrôler création/open/write/close et limiter le cleanup au répertoire possédé.
L'espace des nouvelles racines doit être **extérieur** à l'arbre hérité
`%TEMP%/artist-reader-tests` : l'ancien `TearDown()` en supprime récursivement
le parent, y compris des sous-racines pourtant uniques. Prouver cette isolation
vis-à-vis des fixtures anciennes avant de libérer le bail partagé. Les tests
de suppression utilisent uniquement des arbres qu'ils possèdent, jamais une
suppression volontaire du vrai parent partagé pour provoquer une collision.
Le helper de chemin absent doit rejeter une suppression échouée ou un résidu
avant d'appeler le Reader. Aucun skip, aucune assertion métier englobant le
setup. RED : placer un répertoire non vide au chemin demandé absent ; l'ancien
helper ignore l'erreur et retourne le chemin. GREEN : diagnostic de setup
précis, aucun appel Reader après cet échec ; deux fixtures isolées, cleanup
préservant le voisin.

### Cycle 2 — refactoring caractérisé et preuve d'extraction

Conserver l'entrée `OpenGLShaderReader<Classic>::on(Context&)`. Dans un espace
`detail` interne à `Reader.hpp`, fonctions `inline` header-only :

```cpp
std::string extractShaderSource(std::istream& source);
using SourceExtractor = std::string(std::istream&);
void readShaderFile(graphic::api::OpenGL::ShaderContext& context,
                    SourceExtractor& extract);
```

`on` appelle `readShaderFile(context, extractShaderSource)`. L'extracteur possède
l'unique boucle `read/gcount` actuelle et le rejet fail-sans-EOF, sans toucher
au contexte ni au masque d'exceptions, sans fermer ni traduire l'erreur.
Précondition : flux vivant, masque `badbit` actif. La transaction conserve
vider contexte → fichier local propriétaire/open → extraction en string locale
→ clear/close/vérification → publication ; seul `ifstream::failure` est traduit,
messages publics inchangés. Le callback synchrone n'est ni retenu ni nul et ne
ferme/remplace/conserve pas le fichier. Aucune nouvelle abstraction générale IO.
Au plus un appel par fichier chargé, jamais par bloc/draw/frame ; pas de gain
de performance ou de dévirtualisation revendiqué.

Caractériser les chemins existants avant refactoring. Une seam absente qui ne
compile pas n'est pas un RED comportemental ; le nouveau test peut être GREEN
immédiatement si le catch existant est correct. Ne pas inventer de RED moteur.
Un défaut réellement découvert reçoit sa propre preuve causale avant correction.

L'adaptateur de test reçoit le fichier réel ouvert puis appelle **le même**
extracteur avec un streambuf local défaillant et un istream sous masque badbit.
Premier `xsgetn` : 4096 octets exactement ; deuxième : compteur puis exception
`ios_base::failure` sentinel. Prouver le bloc consommé, la deuxième demande et
badbit réellement posé. Un `xsgetn` court seul pose EOF/fail et ne simule pas
cette erreur. Vérifier type final et message exact
`"ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n" + sentinel.what()` calculé sur la
STL courante ; contexte prérempli devenu vide, aucun bloc partiel publié.
Une exception non-ios injectée (`bad_alloc`) doit rester non traduite, contexte
vide. Interdit : callback levant directement l'erreur métier ou boucle mockée.

Succès canonique : vide, moins de 4096, exactement 4096, plusieurs blocs avec
dernier court, contenu identique ; conserver le cas absent et ses diagnostics.
Un setter virtuel de test peut observer que le filebuf emprunté pendant
l'extraction est fermé au moment de la publication finale, uniquement tant que
la transaction vit. Aucun close-hook nouveau. Lifetimes streambuf/istream et
fichier restent bornés à l'appel/unwind, aucune observation pendante.

### Intégration et limites

JUnit frais des ReaderTests puis CPU union ; sélection ASan CPU déclarée avant
exécution, clean-first, sonde reconstruite, canari 0/86 et premier FAIL conservé.
Pas de GL concurrent à A1c. La fin d'auteur ne vaut pas intégration : I0 attend
le diff F8 intégré et ses preuves avant R/V. La fermeture échouée et une panne
filesystem réelle en cours de lecture restent non prouvées ; **F8 partiel**,
aucune validation Release/autre plateforme/GPU/image/performance nouvelle.

## Découpage prévisionnel et scores (PM/LLM, heuristiques — pas des probabilités)

Après I0 effectivement intégré/revu/vérifié, publication prête (pas admission
anticipée) : D-A2 `task_mvbdn9zv_0050_uhkfan` et H-RESTORE
`task_mvbdkwsd_004y_wiixu8`. H-RESTORE appartient à `build-engineer` :
`scripts/build-windows.ps1`, `scripts/test_asan_env.py`, module `asan-env.ps1`
seulement si nécessaire. Sauvegarder/restaurer ASAN_OPTIONS et
ASAN_ACTIVATION_OPTIONS, absence/null compris, sur succès et exception après
normalisation ; garder la politique stricte des enfants inchangée. RED du
nouveau test avant correction, oracle dans le **même** PowerShell appelant,
puis GREEN et non-régression normalisation/fail-close. PM/LLM : brute62 →
retenue70 (plancher développement), risque54, contexte fresh/worktree.
Cette branche scripts n'a aucun ownership des fichiers CPP A2 ni du GPU ;
les caches/output A2 sont réservés au CPP. Intégrer les deux branches avant
revue/QA finales ; aucun alias à recréer et aucun gain revendiqué.

H-RESTORE livré puis intégré en `1a75c75` : deux fichiers, empreintes conformes
aux octets GREEN, module normaliseur inchangé ; régressions wrapper réel
5 cas (4 bras + Q1), RED causal 2 échecs sur absence puis GREEN 5/5. Preuves
producteur sauvegardées, vérification indépendante finale encore attendue.
Ne pas exécuter ces contrôles réels sur le principal pendant le bail Conan A2.
Manifeste : `_meta/session-2026-10-16-asan-caller-restoration.json`.

```text
B0 (af14c88)
 ├── D-A1 (A1a+A1b) ── I-A1 ── D-A1c ───────────┐
 ├── H-ASan ─────────────────────────────────────┤
 ├── S-GSS ──────────────────────────────────────┤
 ├── DOC-REF ────────────────────────────────────┤
 └── F8-SEAM (après ruling) ─────────────────────┤
                                                I0
                                              ┌─┴─┐
                                              R   V
                                              └─┬─┘
                                       D-A2 (contrat A2 complémentaire)
                                             ┌─┴─┐
                                           R-A2 V-A2
                                             └─┬─┘
                                            DOC-F ── I1
```

| Tâche | Agent | Scores C/R | Contexte |
|---|---|---|---|
| D-A1 (A1a+A1b) | cpp-engineer | 84 / 80 | fresh, worktree |
| D-A1c | gpu-engineer | 74 / 78 | fresh, worktree, après I-A1 |
| H-ASan | build-engineer | 74 / 64 | fresh, worktree |
| S-GSS | cpp-engineer | 70 / 48 | fresh, worktree |
| DOC-REF | engine-architect | 32 / 32 | fresh, worktree |
| F8-SEAM | cpp-engineer | 70 / 55 | fresh, worktree, après ruling |
| R | correctness-reviewer | 63 / 77 | fresh, après I0 |
| V | rendering-verifier | 66 / 78 | fresh, après I0 |
| D-A2 | cpp-engineer | 78 / 80 | fresh, worktree, après contrat A2 + I0/R/V |
| R-A2 | correctness-reviewer | 55 / 70 | fresh, après intégration D-A2 |
| V-A2 | rendering-verifier | 58 / 72 | fresh, après intégration D-A2 |
| DOC-F | engine-architect | 44 / 42 | fresh, après R/V/D-A2/R-A2/V-A2 |

Barrières du coordinateur : **I-A1** (intégration D-A1 vérifiée : empreintes,
re-glob, découverte CPU, transfert `ProgramTests.cpp`) ; **I0** (candidat gelé,
union des fichiers autorisés, smoke, preuves disponibles) ; **I1** (clôture
docs-only + réindexation unique). Les rapports R/V arrivent avec leurs
réserves : elles entrent dans DOC-F sans être « corrigées » silencieusement.

## Ressources et garde-fous de session

- **Demande utilisateur de clôture (2026-10-16)** : terminer la mission et ses
  validations, puis retirer les alias `subst` possédés, notamment U:/Y:/Z:.
  Ne supprimer aucun fichier, cache ou worktree à cette occasion. Vérifier
  le mapping exact et l'absence de commande active avant chaque retrait ;
  préserver/copier les preuves avant retrait du dernier alias utilisé.
  S:/T: retirés précédemment ; **U:/Y:/Z: retirés après arrêt des workers et
  copie des preuves**, sortie `subst` finale vide. Aucun fichier supprimé.
  Les validations suivantes utilisent les chemins réels ; ne pas recréer
  d'alias. Preuve locale : `out/mission-a1a2/alias-retirement.json`.
- Pool cible : **8 agents concurrents maximum** (la capacité machine monte à ~30 ;
  au-delà de 8, GPU/build sérialisé et attribution des défauts ne gagnent plus
  de parallélisme réel). `max_running_members: 8`.
- Routage modèles : développement jusqu'à complexe = `opencode-go/glm-5.3-flash`
  (variante `max`) ; revues/QA/consultations et classification = raisonnement
  `openai/gpt-6.1-sol` (variante `high`) ; docs sans choix explicite = routage
  natif.
- Environnement build : `VsDevCmd.bat -arch=x64` **dans le même processus** que
  cmake/ctest (sinon C1083 `cstddef` / LNK1104 `gdi32.lib`) ; `subst` d'une
  lettre libre après C1083, déclarée ; ASan **clean-first** systématique, sonde
  reconstruite, canari 0/86, premier FAIL conservé (incident incrémental OPEN) ;
  re-glob à chaque ajout de fichier ; refs Ensemble restaurées avec garde
  d'absence (motif récurrent) ; ODR : fixtures à noms uniques ; `.grepai/config.yaml`
  = dérive de watcher hors mission, jamais intégrée ; `out/` non versionné :
  les preuves ne voyagent pas dans les worktrees (chemins absolus du dépôt
  principal fournis aux lecteurs).

## Critères observables

1. RED causal par sous-tranche, exécuté isolément, mécanisme documenté (une
   panne d'environnement n'est pas un RED).
2. GREEN minimal sans affaiblissement ; suites normales serialisées CPU→GL avec
   JUnit neufs : **165 + ajoutés réels** ; inventaire des tests dont le sens
   change.
3. ASan séparé clean-first : baseline **143 + nouveaux cas sélectionnés
   explicitement** (les nouveaux tests GL lifetime de A1c entrent dans la
   sélection après déclaration) ; canari vérifié ; les 22 autres GL restent hors
   sélection sans campagne dédiée.
4. Revue indépendante (correctness-reviewer, fresh) sur le diff intégré B0→I0 et
   QA indépendante (rendering-verifier, fresh) — verdicts par critère, réserves
   conservées.
5. ADR-0003 reste `Accepted / partial` : `implementation` actualisé seulement au
   périmètre prouvé. A2 contracté séparément avant exécution.
6. DOC-F : archive de mission, statuts par finding, manifeste `_meta`, wikilinks
   vérifiés, réindexation QMD une fois en I1.

## Hors mission

ADR-0004 (adoption explicite séparée) ; Q2/Q4/Q5 ; Release/CI distante/couverture ;
performance, équivalence visuelle et ADR-0006 ; F8 exhaustif ; signature exacte
des concepts ; les 22 autres GL sous ASan sans campagne dédiée ; toute refonte
de render graph ou de backend.

## Autorité et limites de la sélection

Sélection utilisateur du 2026-10-16 parmi les candidates postérieures à C
(ordre architecte : Pipeline avant ADR-0004). Fiche de contrat préalable G-A
produite le 2026-10-16 par consultation fresh (`engine-architect`,
`gpt-6.1-sol#high`), ratifiée par le coordinateur ; coupe PM/LLM fresh
(`project-manager`, `gpt-6.1-sol#high`). Une première amorce de l'équipe
(`pipeline-a1`, 4 workers) a été **arrêtée avant toute intégration** — HEAD
`af14c88` inchangé, WIP non conservé : cette fiche est l'état de référence.
Ce dernier constat décrit l'amorce arrêtée, pas l'exécution native en cours.
L'avancement effectivement vérifié est précisé en tête de la fiche d'origine ;
aucun PASS global ne résulte de la sélection ni des seules preuves A1a+A1b.

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[development/known-incidents|Incidents]] ·
[[architecture/missions/2026-10-15-failure-contracts|Mission C]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]

