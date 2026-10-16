---
title: État réel d'ARTIST
category: architecture
tags: [artist, architecture, cpp, rendering]
aliases: [État actuel ARTIST]
summary: Photographie source de la bibliothèque OpenGL et des mécanismes dynamiques qui coexistent avec ses composants statiques, à l'état vérifié de la mission C.
status: observed
source_revision: a50f4661fcbd740cd9614931cbe80e949fae9e57
created: 2026-10-03
updated: 2026-10-16
---

# État réel d'ARTIST

> [!info] Portée de cette photographie
> HEAD `a50f466`, vérifié le 2026-10-16. Le code est identique à `e736bba`
> (intégration de la mission C) ; seuls des documents ont changé depuis.
> Suites normales **165/165 distinctes** (134 unitaires + 31 OpenGL) et **143
> cas ASan distincts** (134 + 9 GL lifetime ; 22 autres GL non sélectionnés),
> revue et QA indépendantes PASS avec réserves consignées. ADR-0003 reste
> Accepted/partial. La mission Pipeline A1+A2 est sélectionnée
> ([[architecture/next-mission|fiche]]), pas commencée. Voir
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
- `graphic/context/PipelineContext.hpp` stocke un
  `std::vector<std::shared_ptr<IPass<API>>>` et un indice de passe courant.
  Depuis la mission C, `getPass(index)` rejette un index négatif ou hors bornes
  avec `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` (`out_of_range`) avant toute
  indexation ; `addPass()` accepte encore un pass nul, `setCurrentPass` reste
  sans garde et `hasNext()` additionne un au curseur — pré-état de la tranche
  Pipeline sélectionnée, non migrée.
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
- `IPass` reçoit un contexte en `unique_ptr`, mais le conserve encore en
  `shared_ptr` ; `getContext()` retourne une copie qui peut étendre sa durée de
  vie. Ce partage transitoire n'est pas une ownership entièrement unique.
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
  `noexcept`. Le self-move ne change rien. Un alias de l'ancien contexte Pass
  voit `passID=0` et peut encore retenir ses shaders ; il ne suit pas le nouvel owner.
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
`on(Context&)`, `Pass::shared()` supprimé. `Uniform`, `Attribute` et `Pipeline`
conservent leurs `shared_ptr` actuels — tranches suivantes. Les corrections de
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

La mission Pipeline A1+A2 est [[architecture/next-mission|sélectionnée le
2026-10-16]], pas commencée : `PipelineContext` stocke toujours un
`vector<std::shared_ptr<IPass>>` et accepte un pass nul, le curseur reste sans
garde et `IPipeline` partage encore son contexte par copie. Cette sélection ne
produit aucun résultat de test ; l'état décrit ci-dessus est celui d'avant
tranche.

[[engineering-principles|Le cap]] explique l'intention ;
[[adr/README|les ADR]] proposent les contrats ;
[[architecture/correction-plan|le plan de correction]] ordonne les preuves.
