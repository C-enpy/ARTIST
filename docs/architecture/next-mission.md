---
title: "Prochaine mission — Pipeline A1+A2 : ownership du Pipeline, curseur et contexte unique"
category: architecture
tags: [artist, architecture, cpp, ownership, testing, pipeline]
summary: "Mission P1 : tranche Pipeline A1 (A1a stockage/emprunts, A1b m_passes unique_ptr + bornes/curseur, A1c lifetime GL chargé) puis A2/F6 sous contrat complémentaire ; micro-incidents ouverts en branches latérales ; ADR-0004 reste hors mission."
status: selected
implementation: not-started
source_revision: af14c88
created: 2026-10-15
updated: 2026-10-16
sources: [docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/development/known-incidents.md, docs/architecture/missions/2026-10-15-failure-contracts.md, docs/architecture/correction-plan.md]
provenance:
  extracted: 0.95
  inferred: 0.05
  ambiguous: 0.0
---

# Prochaine mission : Pipeline A1+A2 — le Pipeline devient propriétaire

**Sélectionnée par l'utilisateur le 2026-10-16 après consultation ; périmètre défini avec la
fiche de contrat préalable de l'architecte (G-A, 2026-10-16, ratifiée par le coordinateur).
À exécuter dans une session neuve.** Cette fiche remplace celle de la mission C
([[architecture/missions/2026-10-15-failure-contracts|archivée]]).

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

## A2/F6 — décisions explicitement ouvertes (fiche complémentaire requise)

A1 ne touche ni `IPass::m_context`, ni `IPass::getContext()`, ni les
consommateurs Uniform/Attribute. Avant A2, l'architecte doit décider : quel
accesseur remplace les aliases partagés de `PassContext` ; quels emprunts
survivent à move/free/reload/destruction ; comment les derniers aliases et
shaders sont détruits avant teardown GL ; comment les bindings deviennent
invalides sans anticiper ADR-0004 ; quels consommateurs migrer et quelles
preuves loaded-owner conserver. Un handle générationnel ne résoudrait pas une
destruction GPU en vol.

## Découpage prévisionnel et scores (PM/LLM, heuristiques — pas des probabilités)

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
                                             └──┬──┘
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
| D-A2 | cpp-engineer | 74 / 78 | fresh, worktree, après contrat A2 + I0 |
| DOC-F | engine-architect | 44 / 42 | fresh, après R/V/D-A2 |

Barrières du coordinateur : **I-A1** (intégration D-A1 vérifiée : empreintes,
re-glob, découverte CPU, transfert `ProgramTests.cpp`) ; **I0** (candidat gelé,
union des fichiers autorisés, smoke, preuves disponibles) ; **I1** (clôture
docs-only + réindexation unique). Les rapports R/V arrivent avec leurs
réserves : elles entrent dans DOC-F sans être « corrigées » silencieusement.

## Ressources et garde-fous de session

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
**P est sélectionnée, pas corrigée** ; aucun PASS global ne résulte de cette
sélection.

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[development/known-incidents|Incidents]] ·
[[architecture/missions/2026-10-15-failure-contracts|Mission C]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
