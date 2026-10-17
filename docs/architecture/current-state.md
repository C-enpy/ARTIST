---
title: État réel d'ARTIST
category: architecture
tags: [artist, architecture, cpp, rendering]
aliases: [État actuel ARTIST]
summary: Photographie source de la bibliothèque OpenGL à l'état de la mission Pipeline A1+A2 (C++ f914d77, harnais 95c2b9e) : Pipeline et IPass propriétaires uniques, curseur borné, F7 hors tranche, harnais de tests restauré, mécanismes statiques et limites observées.
status: observed
source_revision: f914d77b17b1c3013bf06679c0f94537ee7ba24a
created: 2026-10-03
updated: 2026-10-17
---

# État réel d'ARTIST

> [!info] Portée de cette photographie
> HEAD `738891f` vérifié le 2026-10-17 : le C++ est identique à `f914d77`
> (intégration de la mission Pipeline A1+A2/F6, après I0 `18edc72`) ; le seul
> changement de code postérieur est le harnais Python `95c2b9e` (H-FIXTURE,
> deux fichiers scripts/). Côté C++ : `IPipeline::m_context` et
> `IPass::m_context` sont des `unique_ptr`, `PipelineContext::m_passes` stocke
> des `unique_ptr<IPass>`, le curseur est borné et validé avant mutation. Les
> maps uniform/attribute restent partagées (F7 hors tranche). Suites vérifiées à
> `f914d77` : **215/215 normales** (176 unitaires + 39 OpenGL) et **198/198**
> ASan sélection déclarée (176 + 22 GL ; 17 autres GL non sélectionnés) ; bilan
> QA indépendant **24 critères — 14 PASS, 3 FAIL, 7 INCONCLUSIVE**, aucun PASS
> global ([[architecture/missions/2026-10-16-pipeline-ownership|mission
> archivée]]). ADR-0003 reste **Accepted / partial**. **Aucune performance ni
> équivalence d'image n'est mesurée.**

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

- `PassContext::getUniforms()/getAttributes()` exposent des maps d'
  `shared_ptr<Uniform>/shared_ptr<IAttribute>` indexées par `std::string`
  (`Pass.hpp:151, 159`) ; les contexts des wrappers `Uniform`/`Attribute`
  restent partagés, inchangés. Ces owners CPU externes peuvent survivre au
  pass ; leurs métadonnées GL peuvent devenir obsolètes après `free()`,
  remplacement ou destruction du pass : **ne pas promettre leur usage GPU après
  invalidation**, aucun handle générationnel ni validation de binding ADR-0004
  n'existe.
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

## Ce qui est vérifié en exécution (et ce qui ne l'est pas)

- Suites à `f914d77` : **215/215 normales** (176 unitaires + 39 OpenGL) et
  **198/198 ASan** sélection déclarée avant run (176 + 22 GL) ; zéro
  échec/skip **rapporté par CTest** — ce n'est **pas** une conformité de tous
  les oracles. Bilan QA indépendant : **24 critères — 14 PASS, 3 FAIL,
  7 INCONCLUSIVE**, 85/85 artefacts recoupés : l'oracle GMock préexistant
  (`AttributeTest.SetTest`), le FAIL du protocole RED A1c et le FAIL historique
  d'environnement appelant (corrigé par H-RESTORE sans effacer la preuve)
  restent retenus. L'état du harnais à `95c2b9e` est vérifié séparément
  (quatorze contrôles harnais **séparés**, cinq binaires identiques, aucun
  rerun moteur).
- Les comptages historiques datés restent exacts : baseline Shader→Pass
  **122/122** (95 + 27), loaded-owner **137/137** (106 + 31, ASan 106 + 9),
  F5 **143/143** + 121 ASan, mission C `e736bba` **165/165** + 143 ASan,
  I0 `18edc72` 214 + 197.
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

## Extension en attente (PENDING)

La [[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline]] est
terminée et archivée intégralement ; la [[architecture/next-mission|fiche
courte]] ne sélectionne **aucune nouvelle mission** et pointe seulement
l'**amendement oracle en cours** : correction de l'oracle GMock préexistant
(`AttributeTests.cpp:65`, `AttributeTest.SetTest`, diagnostic après summary,
exit 0), autorisée par l'utilisateur le 2026-10-17. Le plan PM est validé et
le RED oracle-only a été recoupé (binaire1, CTest8, JUnit failure1 avant résumé).
Le code d'amendement n'est pas encore intégré ; GREEN et revue/QA indépendantes
restent attendus, sans réécriture des résultats historiques.

[[engineering-principles|Le cap]] explique l'intention ;
[[adr/README|les ADR]] proposent les contrats ;
[[architecture/correction-plan|le plan de correction]] ordonne les preuves.
