# Matrice conceptuelle — capacité et niveau de preuve, quatre API

Rédaction originale, 2026-10-16. **Aucune matrice exécutée sur quatre API.** Le
backend ARTIST implémenté reste OpenGL ; les autres colonnes conceptuelles sont
des expertises. Cette fiche n'établit aucun support pour le matériel local.

## Enregistrement commun à renseigner, pas une sonde exécutable

Pour chaque feature : API/device/OS/driver, requête et résultat observé ou inconnu,
supporté (oui/non/inconnu), activé (oui/non/non applicable/inconnu), compatible
avec le workload (oui/non/inconnu), limites, format/usages/samples, target/ABI,
fallback ou motif de refus, source/version/date. Les booléens seuls perdent les
préconditions ; « non applicable » n'est pas « true ». L'activation Vulkan n'a
pas un équivalent littéral pour chaque feature D3D12/Metal/OpenGL.

Conserver séparément pour chaque API : compilation statique, validation offline,
runtime, device/GPU, capture, profiling et tests d'image. PASS exige commande,
artefact, oracle et résultat ; FAIL exige écart ; NOT_AVAILABLE/SKIPPED d'un
critère requis donnent un verdict INCONCLUSIVE. Un SDK absent n'empêche pas une
procédure documentaire ; son absence ne prouve pas une feature GPU absente.

## OpenGL

- Plateformes : contexte desktop approprié ; disponibilité OS/driver différente
  de la disponibilité du symbole dans GLEW. ARTIST teste OpenGL 3.3 core, pas 4.6.
- Interroger contexte courant/thread, `glGetString` version/vendor/renderer et
  `glGetIntegerv` profil/limites ; extensions via `glGetStringi` là où disponible.
- Supporté : version/extension + limites, fonctions chargées et format/usage.
  Activé : états/chemin choisis ; pas de feature-chain de device Vulkan à inventer.
  Formats avancés via queries adaptées à la version, sinon validation de création/
  FBO completeness pour le contrat réel ; ne pas appeler une query indisponible.
- Compute/SSBO core 4.3, stockage persistant core 4.4 ou extensions applicables :
  3.3 ne suffit pas. Mesh shading non garanti par le core ; variantes d'extensions
  éventuelles à vérifier spécifiquement, pas « RTX donc mesh shaders ».
- Fallback : vertex/index classiques pour géométrie, chemin CPU ou autres passes
  autorisées pour un calcul non disponible. Préserver résultat/qualité ; refus si
  impossible. KHR_debug et capture/profiling sont des capacités distinctes.

## Vulkan

- Runtime/loader/ICD/device et version effective, extensions **device** distinctes
  des extensions instance. SDK/headers ou vulkaninfo seuls ne prouvent pas un rendu.
- `vkGetPhysicalDeviceFeatures2` avec chaînes valides, propriétés/limites et queue
  families ; `vkGetPhysicalDeviceFormatProperties2` puis propriétés image pour
  combinaison format/tiling/usages/flags/samples réellement voulue. Fonction core
  ou extension correspondante disponible avant appel.
- Supporté ≠ activé : demander les features nécessaires à `vkCreateDevice`, avec
  extensions/dépendances requises ; conserver la configuration effective. Compatible
  exige limites et ABI, pas le booléen principal seul.
- Mesh shading : `VK_EXT_mesh_shader`, `meshShader`, `taskShader` si utilisé,
  propriétés mesh/payload/workgroup ; ne pas substituer `VK_NV_mesh_shader` comme
  ABI identique. Ray tracing ou nom RTX n'est pas une requête mesh shader.
- Fallback : vertex/index, compute+culling seulement si capacités et contrat le
  permettent. Validation layers et synchronization validation conditionnelles,
  ni installées ni présumées par cette fiche.

## D3D12

- Windows/runtime/adapter/device sélectionné et toolchain shader ; DXC trouvé
  prouve un compilateur, pas la disponibilité d'un device ou d'une queue.
- `CheckFeatureSupport` selon structure/version, vérifier HRESULT ; feature level
  n'est pas une synthèse de toutes les options. `D3D12_FEATURE_D3D12_OPTIONS7`
  → MeshShaderTier / `D3D12_MESH_SHADER_TIER_NOT_SUPPORTED`, shader model approprié
  via `D3D12_FEATURE_SHADER_MODEL`, puis exigences PSO/root signature/bindings.
- Format support, typed UAV load/store, depth/color usage et samples sont des
  questions distinctes : `D3D12_FEATURE_FORMAT_SUPPORT`, queries associées et
  niveaux de multisampling adaptés au format, pas une simple taille texel.
- Activé signifie ici chemin/PSO/target choisis et compatibles, pas un booléen
  Vulkan transposé. Enhanced barriers exigent leur support propre, indépendamment
  du choix mesh shader.
- Fallback : draw vertex/index ; pas de silently dropping geometry. Debug layer,
  GPU-based validation, PIX/capture/profiling : présence et exécution à établir
  séparément. Ne pas conclure depuis un nom de carte ou un sample compilé.

## Metal

- Hôte Apple, OS/SDK/Xcode/outils et `MTLDevice` choisi. Vérifier availability de
  l'API avant interrogation, `supportsFamily` et propriétés spécifiques pertinentes.
  Les versions Metal 3/4 et les familles ne sont pas un numéro universel de GPU.
- Feature tables Apple et limites de threadgroup/payload/formats/samples/argument
  buffers selon le workload. Mesh/object functions seulement si support effectif,
  langage/compilation/pipeline compatibles ; ne pas déduire depuis un Apple/RTX label.
- Activation : variante de pipeline/fonctions choisie, pas un pNext Vulkan fictif.
  Heap/storage/hazard modes et residency à vérifier pour le modèle de commandes
  réellement disponible. Capacité famille ≠ validation de tout format/usage.
- Fallback : vertex/index ou autre chemin dont la qualité est contractualisée.
  MSL généré sur Windows = artefact intermédiaire ; Metal compile/link/runtime/
  GPU/capture Apple INCONCLUSIVE sans hôte/outils réels. RenderDoc n'est pas un
  remplacement universel de la capture Apple.

## Cas transversaux à ne pas fusionner

Nominal : feature supportée et activée selon l'API, contraintes satisfaites →
éligible à un test pipeline/device. Négatifs : requête non disponible → inconnu ;
support true mais activation omise → chemin refusé ; limites dépassées ou format
incompatible → fallback/refus ; GPU non sélectionné → relevé inapplicable.
Une compilation shader peut PASS pendant que GPU/image restent INCONCLUSIVE.

## Sources et dispositions

Liens officiels ; aucune licence tierce déduite, aucune copie/adaptation de code.
Pour copier ultérieurement : commit exact, chemin et notices, pas arbre SHA seul.

- [Vulkan, enabling features](https://docs.vulkan.org/guide/latest/enabling_features.html).
- [Vulkan, formats](https://docs.vulkan.org/spec/latest/chapters/formats.html).
- [D3D12, niveaux et requêtes](https://learn.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels).
- [D3D12, OPTIONS7](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_feature_data_d3d12_options7).
- [Metal, supportsFamily](https://developer.apple.com/documentation/metal/mtldevice/supportsfamily(_:)).
- [Metal, feature tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf).
- [OpenGL, glGetStringi](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetString.xhtml).

Ces sources évoluent : revalider la version ciblée. Page accessible/titre extrait
ne signifie pas corps technique audité, ni API disponible. Aucun test sur quatre
plateformes n'a été exécuté pour rédiger cette matrice.
