---
title: État réel d'ARTIST
category: architecture
tags: [artist, architecture, cpp, rendering]
aliases: [État actuel ARTIST]
summary: Photographie source de la bibliothèque OpenGL et des mécanismes dynamiques qui coexistent avec ses composants statiques, à l'état vérifié de la mission Pipeline A1+A2.
status: observed
source_revision: 738891fb98e23ee0dbe4afb1f1009c225105d894
created: 2026-10-03
updated: 2026-10-16
---

# État réel d'ARTIST

> [!info] Portée de cette photographie
> HEAD `738891f`, vérifié le 2026-10-16 (clôture DOC-F). Le code moteur C++ est
> identique à `f914d77` (intégration de A2/F6) ; depuis, seul le correctif
> harness Python `95c2b9e` et des documents ont changé. Suites normales
> **215/215 distinctes** (176 unitaires + 39 OpenGL) et sélection ASan exacte
> de **198 cas** (176 + 22 GL : 9 ShaderLifetime historiques + 5 PipelineTest
> chargés + 8 PipelineLifetime ; 17 autres GL non sélectionnés). La revue et la
> QA indépendantes sont terminées **sans PASS global** : **14 PASS / 3 FAIL /
> 7 INCONCLUSIVE**, oracle GMock préexistant `AttributeTest.SetTest`
> reproduit direct et sous CTest malgré `exit 0`. ADR-0003 reste
> Accepted/partial. La mission Pipeline A1+A2 est terminée et archivée
> ([[architecture/missions/2026-10-16-pipeline-ownership|archive]]) ; la fiche
> de sélection est devenue un pointeur ([[architecture/next-mission]]). Voir
> [[architecture/missions/2026-10-15-failure-contracts|la mission C archivée]]
> et [[development/build-and-test|les commandes]]. Aucune performance ni
> équivalence d'image n'est mesurée.

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
(contexte partagé) et `HasBorrowedOnMethod` (contexte emprunté par référence).
Les concepts `OpenGLPipelineFlow` et `OpenGLShaderFlow`, dans `PipelineConcept.hpp`
et `ShaderConcept.hpp` sous `graphic/opengl/validator/`, exigent des composants et une méthode
`on` compatible. `requires` valide à la **compilation**, pas « dynamiquement ».
Les concepts ne prouvent ni l'existence d'un contexte graphique, ni l'image produite.

## Les frontières runtime actuelles

- `IPipeline`, `IPass`, `IShader` et `IAttribute` comportent des fonctions
  virtuelles dans leurs headers de `graphic/pipeline/`.
- `graphic/context/PipelineContext.hpp` stocke désormais un
  `std::vector<std::unique_ptr<IPass<API>>>` et un indice de passe courant
  (mission A1+A2) : `addPass()` rejette un pass nul (`ERROR::PIPELINE::NULL_PASS`)
  et vérifie `N ≤ INT_MAX` par prédicat isolé
  (`ERROR::PIPELINE::PASS_COUNT_LIMIT_EXCEEDED`, jamais testé par allocation
  réelle) ; `getPass(index)` rejette un index négatif ou hors bornes
  avec `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` (`out_of_range`) avant toute
  indexation ; `setCurrentPass` valide avant mutation ; le curseur est
  transactionnel : `use(index)` valide l'index **avant** de le muter, puis
  restaure l'ancien curseur si le hook lève — rollback CPU seulement, aucun
  rollback backend promis ; `hasNext()` ne fait plus d'addition non protégée ;
  `reset()` remet le curseur à −1 sans `glUseProgram(0)` (F10 reste une
  conservation : reset ≠ unbind).
- `IPipeline` possède son contexte en `unique_ptr<API::PipelineContext>`
  (rejet nul `ERROR::PIPELINE::NULL_CONTEXT`) et l'expose par référence gardée
  (`ERROR::PIPELINE::FREED_CONTEXT` sur moved-from) ; l'assignation move
  détruit les passes destination avant le remplacement (A1a).
- `graphic/context/PassContext.hpp` possède les shaders dans un
  `vector<unique_ptr<IShader<API>>>`. Les uniforms/attributes restent partagés
  dans des maps indexées par `std::string`. `getShader(index)` est gardé comme
  `getPass` (`ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE`) ; `vector::at` est
  écarté volontairement.
- `graphic/context/AttributeContext.hpp` stocke un `std::any` et récupère une
  valeur par `std::any_cast<std::shared_ptr<T>>`.
- `graphic/pipeline/Shader.hpp` possède un `unique_ptr<ShaderContext>` et expose
  un emprunt par référence ; ses composants OpenGL prennent `Context&`.
  `Shader` libère le shader natif dans son destructeur dérivé, pas dans `~IShader`.
- `IPass` possède son contexte en `unique_ptr` depuis A2/F6 ;
  `getContext()`/`ctx()` retournent une **référence gardée** — moved-from lève
  `ERROR::PASS::FREED_CONTEXT`, et l'ancien accesseur partagé qui pouvait
  étendre la durée de vie ne compile plus (sonde `C2440` verdict frais). Le
  contexte destination d'une assignation est détruit avec ses shaders (aucun
  alias conservé après transfert) ; les emprunts source restent valides.
  `Pass` libère son programme avant la destruction des shaders de son contexte.
  Le handle programme est publié une seule fois après un lien vérifié ; un échec
  pré-publication appelle `glDeleteProgram` une fois puis relance (publication
  différée, mission C). `load()` charge d'abord les shaders : pas de rollback
  global. La capacité d'info-log des deux loaders est bornée à `[1, 4096]`,
  écriture et accès terminal compris ; cette borne limite l'allocation, pas la
  pénurie mémoire du processus.
- La lecture de fichier shader n'active que `badbit`, extrait par
  `read()`/`gcount()` et rejette `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ`
  avant EOF ; un fichier vide lisible produit une chaîne vide. Les branches
  fermeture échouée et `ifstream::failure` réel ne sont pas exercées.
- `use()` sur un programme zéro appelle `glUseProgram(0)` : unbind explicite
  valide en OpenGL, conservation documentée (F10), pas une correction.
- `Shader` et `Pass` sont `final`. Leurs assignations move libèrent l'ancienne
  destination avant transfert et peuvent propager une exception de cleanup ;
  les opérateurs de base sont `protected`, les constructeurs move restent
  `noexcept`. Le self-move ne change rien. L'alias partagé de l'ancien contexte
  Pass n'existe plus depuis A2 : l'assignation détruit le contexte destination
  **avec ses shaders** (cas alias archivés puis remplacés 1:1 dans la mission
  A1+A2) — il ne suit pas le nouvel owner.
- `common/exception/TraceableException.hpp` porte un incident ouvert dormant :
  la branche `#else` (active sous MSVC) retourne `"Stacktrace not available"`
  par référence vers un temporaire ; zéro appelant dans `ARTIST/`. Coupe
  séparée avant toute exposition.

Ces mécanismes sont **observés dans le source**. Leur coût machine et leur
fréquence dans un workload ne sont pas mesurés ici. LTO/dévirtualisation peuvent
modifier les appels générés ; un refcount n'est pas synonyme d'allocation à chaque
appel. La cible est de rendre les contrats et coûts explicites.

## Contradictions documentaires corrigées

- « Tout le pipeline est résolu à la compilation » devient : composants
  sélectionnés statiquement, composition/ownership/exécution actuellement hybrides.
- « Sans overhead » devient une hypothèse à éprouver sur un workload.
- « Support desktop/mobile et multiples API » devient ambition de portabilité,
  avec OpenGL seul effectivement présent et une matrice à vérifier.
- Les instructions de build génériques sont remplacées par
  [[development/build-and-test|des prérequis, blocages et commandes précis]].
- Les exemples Cen'py de persistence absente et le snake_case prescrit à contre-
  courant du code sont remplacés par [[development/cpp-guidelines|les conventions locales]].

## Résultat de la tranche Shader→Pass

La chaîne de build utilise désormais Conan 2 et des targets CMake explicites.
La baseline historique après migration compte **122/122 tests**
(95 unitaires + 27 OpenGL, dont 14 et 5 tests lifetime respectivement), après la migration franche
d'ownership de la tranche Shader→Pass ([[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]])
: `IShader`/`IPass` non-copiables et déplaçables, contexte de shader en `unique_ptr`,
`PassContext::m_shaders` en `unique_ptr<IShader>`, composants GL en emprunts
`on(Context&)`, `Pass::shared()` supprimé. `Uniform` et `Attribute` conservent
leurs `shared_ptr` (hors tranche, F7 différé) ; `Pipeline` est passé possédant
dans la [[architecture/missions/2026-10-16-pipeline-ownership|mission A1+A2]].
Les corrections de
portabilité antérieures (noms de templates dépendants, contrainte `Uniform`,
fixtures) restent en place. Les loaders rejettent un
handle déjà vivant (`RELOAD_WITHOUT_FREE`) : la présence de la garde est statique,
mais la valeur du handle est contrôlée au runtime pendant la préparation.

La [[architecture/missions/2026-10-15-loaded-owner-move|mission loaded-owner move]] est terminée :
**137/137** normaux (106 unit + 31 GL, dont 25 + 9 lifetime) vérifiés indépendamment,
et **106 CPU + 9 lifetime GL sous ASan**, sans défaut observé dans ces exécutions.
Les cas limites de rechargement complet et les autres
[[development/known-incidents|findings]] restent ouverts. La garantie après
exception de cleanup est de base, sans rollback des detach ; un cleanup qui jette
en destructeur peut encore terminer le processus. Contexte/thread GL valide et
sérialisation restent des préconditions du consommateur. Aucun Pipeline migré.

La [[architecture/missions/2026-10-15-f5-gl-diagnostics|mission F5]] est terminée
dans son périmètre : diagnostics GL vides compilation/link bornés dans les deux
loaders après un RED causal, **143/143 normaux distincts** (112 unit + 31
OpenGL) et **121 cas ASan distincts** (112 + 9 GL lifetime) vérifiés
indépendamment, revue et QA PASS. Ces chiffres sont historiques depuis la
mission C ; F4, F8 et Q1 (exposition INT_MAX) y ont avancé à leur tour, le
harness ASan et Pipeline restaient hors de cette coupe.

La [[architecture/missions/2026-10-15-failure-contracts|mission C]] est terminée
à `e736bba` : capacité d'info-log bornée dans les deux loaders (10 cas
paramétrés `InfoLogLengths`, dont INT_MAX et −5), publication différée du
handle programme, gardes d'index sur les accesseurs des deux contexts, lecture
fichier qui rejette un fichier absent ou illisible et accepte un fichier vide
lisible (branches fermeture échouée et `ifstream::failure` réel non exercées),
conservation documentée de `glUseProgram(0)` sur programme zéro. **165/165
normaux distincts** (134 unitaires + 31 OpenGL) et **143 cas ASan distincts**
(134 + 9 GL lifetime ; 22 autres GL non sélectionnés), revue PASS de lecture
avec réserves α–δ et QA 12 PASS / 2 INCONCLUSIVE sur 14 critères. Les anomalies
consignées — baseline RED 20:35 non causale, campagne ASan incrémentale 133/134
non reproduite après rebuild propre, `getStackTraceString` dormant — ne sont
pas des preuves et ne comptent pas dans les totaux
([[development/known-incidents|le registre]]). Le RED F4 est un plantage STL
debug, pas un rejet métier observé (réserve α) ; le curseur `use(index)` reste
hors coupe.

La mission Pipeline A1+A2 est **terminée dans son périmètre et archivée le
2026-10-16** ([[architecture/missions/2026-10-16-pipeline-ownership|archive]]) :
`IPipeline` et `IPass` possèdent leurs contexts en `unique_ptr`, `m_passes` est
un registre d'owners uniques, le curseur est transactionnel et les accesseurs
retournent des emprunts gardés. Preuves recoupées : I0 `18edc72` **214/214**
(175 CPU + 39 GL) et **197 ASan** exacts ; candidat combiné `f914d77`
**215/215** (176 CPU + 39 GL) et **198 ASan** exacts (176 + 22 GL, 17 autres non
sélectionnés) ; V-A2 **14 PASS / 3 FAIL / 7 INCONCLUSIVE**, sans PASS global
(oracle GMock `AttributeTest.SetTest` préexistant FAIL conservé, direct et sous
CTest malgré `exit 0`). L'invalidation des emprunts à l'assignation/destruction
est testée CPU ; le contexte GL courant de destruction et les ressources en vol
restent un contrat du consommateur ;
[[architecture/next-mission|next-mission]] est désormais un pointeur — aucune
nouvelle mission n'est sélectionnée.

[[engineering-principles|Le cap]] explique l'intention ;
[[adr/README|les ADR]] proposent les contrats ;
[[architecture/correction-plan|le plan de correction]] ordonne les preuves.
