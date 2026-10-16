---
title: "Prochaine mission — Pipeline A1+A2 : ownership du Pipeline, curseur et contexte unique"
category: architecture
tags: [artist, architecture, cpp, ownership, testing, pipeline]
summary: "Mission P1 : tranche Pipeline A1 (A1a stockage/emprunts, A1b m_passes unique_ptr + bornes/curseur, A1c lifetime GL chargé) puis A2/F6 sous contrat complémentaire ; micro-incidents ouverts en branches latérales ; ADR-0004 reste hors mission."
status: selected
implementation: partial
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
Exécution engagée dans la mission native approuvée.** Cette fiche remplace celle de la mission C
([[architecture/missions/2026-10-15-failure-contracts|archivée]]).

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
  inchangés. Ces owners CPU externes peuvent survivre ; leurs métadonnées GL
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
Un défaut réellement découvert reçoit son propre RED causal avant correction.

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
L'avancement effectivement vérifié est précisé en tête de cette fiche ;
aucun PASS global ne résulte de la sélection ni des seules preuves A1a+A1b.

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[development/known-incidents|Incidents]] ·
[[architecture/missions/2026-10-15-failure-contracts|Mission C]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
