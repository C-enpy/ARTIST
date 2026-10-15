---
title: État réel d'ARTIST
category: architecture
tags: [artist, architecture, cpp, rendering]
aliases: [État actuel ARTIST]
summary: Photographie source de la bibliothèque OpenGL et des mécanismes dynamiques qui coexistent avec ses composants statiques.
status: observed
source_revision: e589aeece2c5abc46aedb089c6a88a6f7dbaa239
source_diff_fingerprint: a1e66dd728c8e6b0700cf7db694040c9592ce18acaf53145590ad0e85053c234
created: 2026-10-03
updated: 2026-10-15
---

# État réel d'ARTIST

> [!info] Portée de cette photographie
> HEAD `e589aee` et diff intégré D `0bbca504` + H `a423fc25`, vérifiés le
> 2026-10-15. Pas de nouveau commit principal ; 15 fichiers source/build empreintés.
> Suites normales 137/137, ASan 106 CPU + 9 lifetime GL.
> Voir [[references/session-2026-10-15-loaded-owner-move|la synthèse et les limites]]
> et [[development/build-and-test|les commandes]].
> Aucune performance ni équivalence d'image n'est mesurée.

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
- `graphic/context/PassContext.hpp` possède les shaders dans un
  `vector<unique_ptr<IShader<API>>>`. Les uniforms/attributes restent partagés
  dans des maps indexées par `std::string`.
- `graphic/context/AttributeContext.hpp` stocke un `std::any` et récupère une
  valeur par `std::any_cast<std::shared_ptr<T>>`.
- `graphic/pipeline/Shader.hpp` possède un `unique_ptr<ShaderContext>` et expose
  un emprunt par référence ; ses composants OpenGL prennent `Context&`.
  `Shader` libère le shader natif dans son destructeur dérivé, pas dans `~IShader`.
- `IPass` reçoit un contexte en `unique_ptr`, mais le conserve encore en
  `shared_ptr` ; `getContext()` retourne une copie qui peut étendre sa durée de
  vie. Ce partage transitoire n'est pas une ownership entièrement unique.
  `Pass` libère son programme avant la destruction des shaders de son contexte.
- `Shader` et `Pass` sont `final`. Leurs assignations move libèrent l'ancienne
  destination avant transfert et peuvent propager une exception de cleanup ;
  les opérateurs de base sont `protected`, les constructeurs move restent
  `noexcept`. Le self-move ne change rien. Un alias de l'ancien contexte Pass
  voit `passID=0` et peut encore retenir ses shaders ; il ne suit pas le nouvel owner.

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
fixtures) restent en place. Le graphe d'ownership des tranches non migrées et le
modèle d'exécution restent ceux décrits ci-dessus. Les loaders rejettent un
handle déjà vivant (`RELOAD_WITHOUT_FREE`) : la présence de la garde est statique,
mais la valeur du handle est contrôlée au runtime pendant la préparation.

La [[architecture/missions/2026-10-15-loaded-owner-move|mission loaded-owner move]] est maintenant terminée :
**137/137** normaux (106 unit + 31 GL, dont 25 + 9 lifetime) vérifiés indépendamment,
et **106 CPU + 9 lifetime GL sous ASan**, sans défaut observé dans ces exécutions.
Les cas limites de rechargement complet et les autres
[[development/known-incidents|findings]] restent ouverts. La garantie après
exception de cleanup est de base, sans rollback des detach ; un cleanup qui jette
en destructeur peut encore terminer le processus. Contexte/thread GL valide et
sérialisation restent des préconditions du consommateur. Aucun Pipeline migré.

La [[architecture/next-mission|prochaine mission sélectionnée]] est F5, diagnostics
GL vides compilation/link. Risque de source confirmé, pas de RED ni correction
exécutés pour cette sélection ; aucun résultat antérieur n'est étendu à F5.

[[engineering-principles|Le cap]] explique l'intention ;
[[adr/README|les ADR]] proposent les contrats ;
[[architecture/correction-plan|le plan de correction]] ordonne les preuves.
