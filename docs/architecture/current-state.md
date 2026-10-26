---
title: État réel d'ARTIST
category: architecture
tags: [artist, architecture, cpp, rendering]
aliases: [État actuel ARTIST]
summary: "Source82bf52d : ASan555 acquis ; causesB1 vérifiées, calibrationB3 autorisée sans correctifReader, fermeture motivéeC2 acceptée."
sources: [.opencode/missions/team_mvnxps48_0000_dfde7a/reports/21-audit-final-accepte.md, .opencode/missions/team_mvnxps48_0000_dfde7a/reports/22-bilan-final-et-dette-board.md, .opencode/missions/team_mvnxps48_0000_dfde7a/reports/25-reconciliation-native-verifiee.md]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
status: observed
source_revision: 82bf52d52e594fae02593866669a1af715127d1f
created: 2026-10-03
updated: 2026-10-26
---

# État réel d'ARTIST

> [!info] Photographie finale de la continuation bornée — 2026-10-25
> Candidat C++82bf52d : correction de8lignes dans un seul test lifetime mocké,
> aucune production modifiée par cette continuation. **ASan555/555**
> (484CPU+71OpenGL), zéro failure/error/skip,17historiques chacun une fois ;
> **484CPU normaux réexécutés**,71GL normaux antérieurs réutilisés par identité.
> Revue statique indépendante du diff et **audit final test-reviewer fresh reçus,
> PASS borné O1–6 sans finding bloquant ; continuation terminée à cette portée**.
> Audit statique, pas de tests exécutés par reviewer ni PASS global moteur/harness.
> Archives rouge/verte et manifestes conservés :
> `_meta/session-2026-10-25-qa-continuation.json` et
> [[development/build-and-test|commandes et portée]]. F8/C2/B1/GCC restent ouverts ;
> les ADR ne sont pas promus et aucune performance/image/Linux n'est validée.

Le graphe importé ASAN→AUDIT→CLOTURE a été **réconcilié7/7completed sous verrou
natif**, sur choix opérateur, sans nouveau worker/audit ni changement de politique.
Équipe/worktrees conservés, non archivés ; politique non approuvée par effet du
verrou, sans start du lead. Les dettes techniques ne sont pas fermées :
[[references/session-2026-10-25-qa-continuation|synthèse sourcée et résolution du statut]].

**Étape ultérieure du25octobre : scope initial restauré sous révision3.** Les
anciennes7tâches restent acquises ; trois enquêtes nouvelles B1/B3/B5 ont été
lancées. B1 dispose maintenant de deux témoins causaux négatifs historiques
(1et2échecs) et de nominaux actuels verts (4et6RUN,5cas uniques), sans nouveau
correctif. `_meta/session-2026-10-25-b1-causal-validation.json` conserve preuves
et limites. Contrat Reader/F8 seulement proposé : **la
reprise du scope initial n'est pas terminée**, et7/7ne désigne pas ce nouveau lot.

**Décisions opérateur puis intégration QA (25–26octobre)** : calibration stdio et
recherche OS close bornées autorisées, sans correction Reader ni substitution
catch/close. Fermeture administrative motivée B5/C2 explicitement acceptée :
défaut de fixture établi, explication forte pour Empty/probable pour MultiBlock,
pas reconstruction complète ou verdict moteur. Primaires manquants conservés ;
réouverture sur preuve contradictoire ou nouveau symptôme identifié. Décisions34
et amendement ciblé QA17 intégrés dans `fb3c9b9` ; aucune calibration exécutée à
ce jalon, B1 attend encore sa revue finale. Ces décisions actualisent les dettes
de la photographie historique ci-dessus, sans changer les anciens verdicts.

**Jalon du26octobre — contrôles de la future sonde, pas correctif Reader** :
le helper Win32 privé `scripts/probes/qa-f8-stdio/native_read.hpp` a suivi RED
causal12325→8192 puis GREEN4/4lectures complètes, comparateur inchangé ; deux
contrats CLI séparés ont aussi été vérifiés par le lead. Preuves portables :
`_meta/session-2026-10-26-stdio-control-red.json` et
`_meta/session-2026-10-26-stdio-control-green.json`, source intégrée961d92e.
Revue indépendante fresh reçue et acceptée au rapport63 de la mission (commit
46dd389) : aucun défaut démontré à ce périmètre ; erreurs authentiques de lecture,
close et allocation non exécutées. Aucun transfert du verdict ASan555 au probe.
La calibration stdio M0/M1/M2 reste à écrire/exécuter. Le refus close par protection
OS a été admis seulement pour un probe FILE* en enfant dédié (décision52) ;
contrat statique privé `can_reap_process` a suivi RED de définition manquante puis
GREEN de deux TU compilées, quatre assertions policy inchangées (sourcef235db8,
preuves `_meta/session-2026-10-26-close-policy-{red,green}.json`). Cette preuve
de compilation ne valide aucun wait ni futur caller ; parent/enfant encore au
stade proposition, aucun essai close lancé.
M5Reader/non-publication/catch et adoption du futur Reader restent ouverts ;
ni nouveau backend, ni image/performance/GCC/Linux/Release validés.

**Recentrage opérateur —26octobre** : le contrat normal est désormais celui d'un
fichier **stable pendant sa lecture, sans écriture concurrente** ; solution
portable, contrôles d'erreur et acceptation du contenu après réussite, sans
synchronisation ajoutée pour un scénario concurrent hors contrat. La poursuite
des probes stdio/close est arrêtée : leurs builds existent, mais **aucun RUN de
caractérisation n'a été lancé** ; le manifeste stdio sélectionné est conservé
NOT_RUN. Inventaire des consommateurs et contrat minimal en préparation RO,
avant retrait méthodique des surcouches/tests devenus spécifiques à l'exploration.
À ce jalon aucun nettoyage source ni correction Reader encore exécutés ; preuves
historiques/correctifs utiles/diffs étrangers préservés. Décision et limites :
rapport83 de la mission team_mvnxps48_0000_dfde7a, entrées83/84 commit45d1884.

> [!info] Photographie historique — 2026-10-20
> Candidat `fdbce15`, vérifié le 2026-10-20 : **546/546 natifs** (477 unitaires +
> 69 OpenGL), zéro échec/skip rapporté par CTest. Le chemin d'exécution fermé
> N=2 (`ClosedExecution`) est intégré **en coexistence** avec `IPipeline`
> ([[architecture/missions/2026-10-19-pipeline-execution-path|fiche de mission]]) ;
> les garanties ownership Pipeline A1+A2, IPass A2/F6 et F7 restent en place.
> **Aucune performance ni équivalence d'image mesurée** ; le virtuel `IPass::use`
> demeure (coût constaté, jamais revendiqué comme gain). ASan, Release et autres
> plateformes restent hors périmètre. Preuves :
> [[references/session-2026-10-20-pipeline-execution-path|session de mission]] et
> `_meta/session-2026-10-20-pipeline-execution-path.json`.

## Ce qui est présent

- `ARTIST/CMakeLists.txt` déclare `ArtistLib` comme bibliothèque `INTERFACE`.
- `ARTIST/include/graphic/pipeline/` expose `Pipeline`, `Pass`, `Shader`, `Uniform`
  et `Attribute`. Des contexts stockent leurs données ; des composants de backend
  effectuent les opérations.
- `ARTIST/include/graphic/opengl/` contient les contexts, profils, validators et
  composants OpenGL. Un nom Vulkan/DirectX/Metal dans un commentaire n'est pas une
  implémentation de ces backends.
- `conanfile.txt` déclare GLM, GLFW, GLEW, OpenGL et magic_enum.
- `ARTIST/tests/` contient les tests GoogleTest/GoogleMock ;
  `ARTIST/component-tests/` contient des tests utilisant GLFW/GLEW et OpenGL 3.3 core.

Il n'y a pas de cooker, de SceneIR/RenderIR, de gestionnaire de streaming, de
physique ou de suite de benchmarks moteur établis par les fichiers inspectés.
Leurs descriptions dans les rapports sont des orientations.

## La partie effectivement statique

Dans `graphic/pipeline/Pipeline.hpp`, `Pipeline<API, PROFILE>` est contraint par
`API::Validator::validatePipeline`. Les composants sont choisis avec des alias
paramétrés par profil et appelés au travers de méthodes `on` sélectionnées par
`if constexpr`. Les autres primitives emploient un schéma similaire.

`graphic/validator/ComponentConcept.hpp` déclare `HasComponent`, `HasOnMethod`
(contexte partagé, uniform/attribute) et `HasBorrowedOnMethod` (contexte emprunté
par référence, pipeline/pass). Les concepts `OpenGLPipelineFlow`,
`OpenGLShaderFlow` et leurs variantes, dans `PipelineConcept.hpp` et
`ShaderConcept.hpp` sous `graphic/opengl/validator/`, exigent des composants et
une méthode `on` compatible. `requires` valide à la **compilation**, pas
« dynamiquement ». Les concepts ne prouvent ni l'existence d'un contexte
graphique, ni l'image produite.

## Ownership migré — Pipeline (A1)

- `IPipeline::m_context` est `std::unique_ptr<typename API::PipelineContext>`
  (`Pipeline.hpp:179`) ; l'injection par `unique_ptr` rejette un nul avec
  `ERROR::PIPELINE::NULL_CONTEXT` (`invalid_argument`). Le wrapper est
  non-copiable ; les constructions move transfèrent le pointeur sans copier le
  sous-objet base, l'assignation move du concret (base protégée) détruit
  l'ancien contenu destination avant transfert et le self-move est no-op ; un
  accès moved-from lève `ERROR::PIPELINE::FREED_CONTEXT` (`runtime_error`,
  `Pipeline.hpp:158`). Aucun destructeur de base n'appelle un hook virtuel.
- `getContext() const` retourne `API::PipelineContext&` (`Pipeline.hpp:154`) :
  un emprunt, jamais un owner, jamais un accesseur partagé conservé.
- `PipelineContext::m_passes` est `std::vector<std::unique_ptr<IPass<API>>>` ;
  les constructeurs consomment ce vecteur (`Pipeline.hpp:48-62`).
  `addPass(std::unique_ptr)` rejette un nul avant insertion
  (`ERROR::PIPELINE::NULL_PASS`, `PipelineContext.hpp:46`) et vérifie la
  cardinalité (`ERROR::PIPELINE::PASS_COUNT_LIMIT_EXCEEDED`, `length_error`,
  `:36`) avant insertion — la limite est testée par prédicat isolé, jamais par
  allocation réelle ; un ajout ne change pas le curseur.
- `getPass(index)`/`forPass(index)` retournent `IPass&` — un emprunt, jamais un
  owner — avec la garde C2 conservée
  `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` (`out_of_range`,
  `PipelineContext.hpp:71`) ; `vector::at` reste écarté volontairement.
- **Curseur** : invariants `c ∈ {-1} ∪ [0, N)`, `N ≤ INT_MAX` ;
  `setCurrentPass` accepte −1 et `[0, N)` et rejette tout autre entier **avant
  mutation** ; `use(index)` vérifie le contexte possédé, valide l'index **avant
  mutation**, mémorise l'ancien curseur, appelle le hook emprunté et, si le hook
  lève, **restaure l'ancien curseur puis relance inchangé** — un rollback CPU
  (`Pipeline.hpp:122-133`) ; le commentaire de code précise qu'il ne s'agit
  **jamais d'un rollback backend**. `use(−1)` est invalide. `hasNext()` n'a plus
  d'addition non protégée du curseur ; `useNext` conserve sa sémantique publique
  (vide → reset + false ; suivant → `use(c+1)` ; épuisé → reset + false ; échec
  de `use` conserve l'ancien curseur). `reset()` remet seulement le curseur à
  −1 : reset ≠ unbind, aucun `glUseProgram(0)` (`Pipeline.hpp:141`).
- Les emists de passes n'étendent aucun lifetime ; l'ajout réalloue le vecteur
  mais ne déplace pas les objets passes ; le move du wrapper conserve les
  adresses ; l'assignation move **invalide les emprunts destination** ;
  l'acquisition de composants `User`/`Resetter` utilise `on(Context&)` emprunté
  (`HasBorrowedOnMethod`), avec de fines surcharges `on(shared_ptr<Context>)`
  conservées (nul → même type + `ERROR::SHADER::NON_VALID_CONTEXT`, sinon
  délégation sans rétention).

## Ownership migré — `IPass` et cycle du programme (A2/F6)

- `IPass::m_context` est `std::unique_ptr<typename API::PassContext>`
  (`Pass.hpp:234`) ; la construction rejette un nul
  (`ERROR::PASS::NULL_CONTEXT`, `Pass.hpp:67`). Copie interdite, construction
  move `noexcept`, self-move no-op, assignation de base protégée avec cleanup
  de l'ancienne destination **avant** transfert (garantie de base : si le
  cleanup lève, les pointeurs ne sont pas transférés, aucune rollback backend).
- `getContext() const` est une référence `API::PassContext&` gardée par
  `ctx()` (`Pass.hpp:189-191, 244-248`) ; sur une source moved-from, l'accesseur
  lève l'erreur exacte `ERROR::PASS::FREED_CONTEXT` (`runtime_error`). Aucun
  accesseur partagé de repli. Un cas unitaire **getter moved-from**
  (`MovedFromPassContextGetterThrows`) verrouille ce contrat, ajouté à A2
  finale ; **deux cas alias (unitaire et GL) ont été remplacés 1:1** avec leur
  caractérisation archivée : après la migration, l'alias ne prolonge plus ni le
  programme ni le contexte (`PassMoveAssignmentPreservesContextAliasesNotOldProgram`).
- `free()` remet le handle programme à zéro sans détruire le contexte ni ses
  shaders : les emprunts CPU restent valides ; `free()` moved-from reste no-op.
  **Aucune promesse de cycle `free(); load()` direct** : `IPass::load()` recharge
  d'abord les shaders et le strict B persiste — un `load()` sur handle vivant
  rejette `ERROR::PASS::RELOAD_WITHOUT_FREE` / `ERROR::SHADER::RELOAD_WITHOUT_FREE`
  avant tout `glCreate*` ; le cycle complet exige de libérer le programme **et
  chaque shader** concerné, sans rollback global.
- Construction move : le nouvel owner conserve les adresses et les emprunts
  source ; la source déplacée rejette ses accesseurs. Destruction : programme
  libéré, contexte détruit, shaders détruits ; un contexte GL courant et
  compatible reste requis — la suppression d'un programme courant peut être
  différée par le driver jusqu'à l'unbind. `Pass` libère son programme avant
  la destruction des shaders de son contexte ; la publication du handle reste
  unique après un lien vérifié (mission C), `glDeleteProgram` unique sur échec
  pré-publication.
- `~IPipeline`/`~IPass` : aucun hook virtuel de base ; la libération RAII est
  portée par le côté dérivé (défaut durable depuis l'incident ODR/`_purecall`).

## Ce qui reste partagé — uniform/attribute (F7) et divers

- `PassContext::getUniforms()/getAttributes()` empruntent les maps de
  `unique_ptr<Uniform>/unique_ptr<IAttribute>` indexées par chaînes. Contextes
  Uniform/Attribute possédés en unique_ptr, getContext() par référence ;
  getUniform/getAttribute renvoient des pointeurs empruntés nullables. Rejet des
  owners nuls avant insertion. Emprunts invalidés au remplacement/reload/destruction,
  aucun handle générationnel ni garantie d'usage GPU après invalidation.
- UniformReader est exigé par concept (présence+on(C&) void), utilise la capacité
  driver des noms, valide location/size/doublons intra-réflexion. Reload garde
  les noms anciens absents et peut publier partiellement avant erreur.
- `Uniform::set<T>` porte une seule valeur ; les34setters OpenGL refusent size!=1
  avant GL et30appels vector/matrix utilisent count1. Arrays restent réfléchis mais
  leur set single-value est rejeté. Six types vérifiés nativement dont fp64 observé ;
  28autres INCONCLUSIVE driver, inventaire/revue statique seulement.
- `graphic/context/AttributeContext.hpp` stocke un `std::any` et récupère une
  valeur par `std::any_cast<std::shared_ptr<T>>`.
- `graphic/pipeline/Shader.hpp` possède son `ShaderContext` en `unique_ptr` et
  expose un emprunt par référence ; ses composants OpenGL prennent
  `Context&`. `Shader` libère le shader natif dans son destructeur dérivé.
- La lecture de fichier shader passe par la seam `detail` de mission I0
  (`extractShaderSource` + `readShaderFile`, `Reader.hpp`) — transaction
  extraire dans une string locale → `clear()`/`close()` vérifiée → publication.
  L'extraction consomme par `read()`/`gcount()` et rejette
  `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ` hors EOF ; un fichier vide
  lisible produit une chaîne vide. **Les branches fermeture échouée et
  `ifstream::failure` réel ne sont pas exercées** (F8 partiel).
- `use()` sur un programme zéro appelle `glUseProgram(0)` : unbind explicite
  valide en OpenGL, conservation documentée (F10), pas une correction.
- `common/exception/TraceableException.hpp` : les deux branches retournent
  désormais `getStackTraceString()` **par `std::string` valeur** (S-GSS,
  `433bdbc`) ; le défaut d'usage n'expose plus aucune référence pendante.
  Latent conservé : la branche `#if` libstdc++ a un appel `const` vers un
  membre non `const` — inatteignable sous MSVC sur cette baseline, coupe
  séparée avant toute exposition GCC.

## Chemin d'exécution fermé N=2 (X, 2026-10-20)

- `graphic/pipeline/ClosedExecution.hpp` : `ClosedExecution<API>` — deux emprunts
  `IPass<API>&` stockés dans l'ordre d'activation, `run()` = séquence littérale
  `first.use(); second.use();` ; échec à l'étape k ⇒ étape k+1 jamais lancée,
  exception propagée **identique** (aucun try/catch/wrap). Ne lit ni n'écrit le
  curseur, aucun cache de handle, aucun ownership, pas de paramètre `PROFILE`,
  indépendant d'`IPipeline` : **coexistence** (`Pipeline<API, PROFILE>` non
  remplacé, aucun consumer migré). ADR-0002 reste **Proposed** — période de
  coexistence tolérée, pas la cible terminale (tuple/concrets, hors mission).
- Effets GL exactement ceux d'`IPass::use()` : l'unique site `glUseProgram` du
  moteur reste `component/pass/User.hpp:33` ; `reset` ≠ unbind inchangé (F10).
- Tests : 10 unitaires (`ClosedExecutionTests` + `ClosedExecutionPassTests` —
  ordre `InSequence` par invocation, curseur pré-états −1/0/1, échecs k=1/k=2,
  restart après échec, moved-from réel ×2, chaînes croisées anti-mémoïsation,
  destructeur sans `free()`) et 5 OpenGL (`PipelineTest.ClosedExecution*` —
  readback `GL_CURRENT_PROGRAM`, interaction reset/F10, `passID=0` sans
  exception, garde RAII `ProgramUnbinder`, asserts `glIsProgram` pré-run).

## Ce qui est vérifié en exécution (et ce qui ne l'est pas)

- Suites à `fdbce15` : **546/546 natifs** (477 unitaires + 69 OpenGL), zéro
  échec/skip rapporté par CTest. Preuve RED→GREEN tracée : commit `36c6a13`
  (tests seuls, sans le type) → `fatal error C1083` attendu reproduit, puis
  `ac692db` (le type) → vert ; rounds F (P1/P2/R-1/R-2, `6273698`) et F1
  (`fdbce15`) vérifiés par le lead (runner), JUnit recoupés, empreintes des
  cinq fichiers gelés consignées (`_meta/session-2026-10-20-pipeline-execution-path.json`).
- Les comptages historiques datés restent exacts : baseline Shader→Pass
  **122/122** (95 + 27), loaded-owner **137/137** (106 + 31, ASan 106 + 9),
  F5 **143/143** + 121 ASan, mission C `e736bba` **165/165** + 143 ASan,
  I0 `18edc72` 214 + 197, mission Pipeline `f914d77` 215/215 + 198 ASan
  (bilan QA : 14 PASS, 3 FAIL, 7 INCONCLUSIVE, archivé).
- **Non vérifié** : les 17 autres GL sous ASan (sélection, pas abandon),
  une absence exhaustive d'UB ou de fuite driver, Release, Linux, CI distante,
  UBSan/TSan, couverture, équivalence d'image, performance, coût machine et
  fréquence de workload. LTO/dévirtualisation peuvent modifier les appels
  générés ; un refcount shared (uniform/attribute) n'est pas synonyme
  d'allocation à chaque appel.

## Contradictions documentaires corrigées

- « Tout le pipeline est résolu à la compilation » devient : composants
  sélectionnés statiquement, ownership et registre de passes migrés
  (`vector<unique_ptr>` + curseur), mais le dispatch par famille d'appel reste
  `if constexpr` et la frontière execution/cook reste à établir.
- « Sans overhead » devient une hypothèse à éprouver sur un workload.
- « Support desktop/mobile et multiples API » devient ambition de portabilité,
  avec OpenGL seul effectivement présent et une matrice à vérifier.
- Les instructions de build génériques sont remplacées par
  [[development/build-and-test|des prérequis, blocages et commandes précis]].
- Les exemples Cen'py de persistence absente et le snake_case prescrit à contre-
  courant du code sont remplacés par [[development/cpp-guidelines|les conventions locales]].

## Résultat des tranches précédentes (historiques, datés)

La chaîne de build utilise Conan 2 et des targets CMake explicites. La
baseline historique après la tranche Shader→Pass compte **122/122 tests**
(95 unitaires + 27 OpenGL, dont 14 et 5 tests lifetime respectivement),
après la migration franche d'ownership de la tranche Shader→Pass
([[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]) : `IShader`/`IPass`
non-copiables et déplaçables, contexte de shader en `unique_ptr`,
`PassContext::m_shaders` en `unique_ptr<IShader>`, composants GL en emprunts
`on(Context&)`, `Pass::shared()` supprimé. `Uniform`, `Attribute` et
`Pipeline` conservaient leurs `shared_ptr` actuels — désormais migré
uniquement pour `Pipeline` et `IPass` ci-dessus, pas pour `Uniform`/
`Attribute`. Les loaders rejettent un handle déjà vivant
(`RELOAD_WITHOUT_FREE`) : la présence de la garde est statique, mais la
valeur du handle est contrôlée au runtime pendant la préparation.

La [[architecture/missions/2026-10-15-loaded-owner-move|mission loaded-owner
move]] est terminée : **137/137** normaux (106 unit + 31 GL, dont 25 + 9
lifetime) et **106 CPU + 9 lifetime GL sous ASan**, sans défaut observé dans
ces exécutions. La [[architecture/missions/2026-10-15-f5-gl-diagnostics|mission
F5]] est terminée dans son périmètre : diagnostics GL vides compilation/link
bornés dans les deux loaders après RED causal, **143/143 normaux distincts**
(112 unit + 31 OpenGL) et **121 cas ASan distincts**. La
[[architecture/missions/2026-10-15-failure-contracts|mission C]] est terminée
à `e736bba` : capacité d'info-log bornée, publication différée du handle
programme, gardes d'index sur les accesseurs, lecture fichier qui rejette un
fichier absent ou illisible et accepte un fichier vide lisible, conservation
documentée de `glUseProgram(0)` sur programme zéro. **165/165 normales
distinctes** (134 unitaires + 31 OpenGL) et **143 cas ASan distincts** ;
revue PASS de lecture avec réserves α–δ et QA 12 PASS / 2 INCONCLUSIVE sur
14 critères — résultats datés, pas des totaux quotidiens actuels.

## Amendement oracle Attribute — terminé et vérifié le 2026-10-17

La [[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline]] est
terminée et archivée intégralement ; la fiche courte d'alors ne sélectionnait
aucune nouvelle mission et pointait seulement
l'**amendement oracle terminé au périmètre** : correction de l'oracle GMock préexistant
(`AttributeTests.cpp:65`, `AttributeTest.SetTest`, diagnostic après summary,
exit 0), autorisée par l'utilisateur le 2026-10-17. Le plan PM est validé et
le RED oracle-only a été recoupé (binaire1, CTest8, JUnit failure1 avant résumé).
Le GREEN est intégré en `b9c90fd` (test seul, moteur inchangé) : revue fresh
**PASS sans finding**, QA indépendante **176 CPU + 176 CPU ASan**, contrôles
négatifs binaire1/CTest8/JUnit failure1 et positifs0/0/failure0 répétés. L'ancien
faux vert reste historique ; l'attente effective `Setter<shared_ptr<int>>`,
`Times(1)`, le contexte et la valeur sont désormais contrôlés avant résumé.
Le pin GoogleTest v1.14.0 garantit statiquement le TearDown après assertion fatale.
Manifeste : `_meta/session-2026-10-17-attribute-oracle-validation.json`.

Une fragilité d'ordre Loader/ShaderLoader est reproduite sur le vrai binaire
pré-G1 et le GREEN : mêmes neuf échecs sous seed20261017 (effectif61219), même
échec sous seed7. Indépendance de l'amendement établie pour ces deux ordres,
pas de PASS global toutes permutations ; aucune correction supplémentaire engagée.

[[engineering-principles|Le cap]] explique l'intention ;
[[adr/README|les ADR]] proposent les contrats ;
[[architecture/correction-plan|le plan de correction]] ordonne les preuves.
