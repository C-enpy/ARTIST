# Sources et dispositions T06

Rédaction originale des procédures, oracles et exemples. Aucun fichier, code,
script, guide ou superviseur tiers copié/adapté. Les liens ci-dessous sont des
références de lecture, pas des commandes ni des preuves d'exécution.

## Documentation normative ciblée

Références officielles Khronos, `latest` mutable : version exacte à figer avec la
cible de chaque expérience. Licence des pages web non établie ici : **lien
seulement**, aucune redistribution. T02 a observé HTTP 403 sur docs.vulkan.org le
2026-10-16 ; disponibilité de chaque refpage non revendiquée dans T06. Le dépôt
[Vulkan-Docs](https://github.com/KhronosGroup/Vulkan-Docs) reste l'alternative
documentaire officielle, pas un miroir installé ni une preuve de conformité.

- [Features2](https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceFeatures2.html)
  et [création du device](https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateDevice.html) :
  support/query/activation, struct chains et VkResult ; VK-01.
- [Pipeline de rendu dynamique](https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineRenderingCreateInfo.html)
  et [begin rendering](https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBeginRendering.html) :
  formats/sample count/compatibilité du pipeline ; VK-02.
- [Mise à jour des descriptors](https://docs.vulkan.org/refpages/latest/refpages/source/vkUpdateDescriptorSets.html) :
  type, bounds, durée de vie des sets ; VK-03.
- [Synchronisation et cache control](https://docs.vulkan.org/spec/latest/chapters/synchronization.html)
  et [BufferMemoryBarrier2](https://docs.vulkan.org/refpages/latest/refpages/source/VkBufferMemoryBarrier2.html) :
  stages/access, transferts de famille et scopes ; VK-03/VK-04.
- [Invalidation mémoire mappée](https://docs.vulkan.org/refpages/latest/refpages/source/vkInvalidateMappedMemoryRanges.html) :
  visibilité hôte, nonCoherentAtomSize, bornes mapping/allocation ; VK-03.

## Guides officiels consultés

Trois chapitres du Vulkan-Guide récupérés via leurs URLs raw officielles le
2026-10-16, branche `main` mutable, pas de commit résolu. Notice source observée
CC-BY-4.0 ; disposition **référencer**, sans copie/adaptation. Les guides expliquent
des usages ; les valid usages de la spécification applicable restent normatifs.

- [Enabling features](https://github.com/KhronosGroup/Vulkan-Guide/blob/main/chapters/enabling_features.adoc) :
  features supportées/activées, pNext, core/extensions ; prérequis device/loader.
- [Synchronization](https://github.com/KhronosGroup/Vulkan-Guide/blob/main/chapters/synchronization.adoc) :
  primitives et validation conditionnelle ; requiert une API/queue de la cible.
- [Validation overview](https://github.com/KhronosGroup/Vulkan-Guide/blob/main/chapters/validation_overview.adoc) :
  limites des différentes validations, pas une garantie de correction visuelle.
- [Vulkan-ValidationLayers](https://github.com/KhronosGroup/Vulkan-ValidationLayers) :
  outil de validation ; versions loader/layer/config compatibles requises.
  Licence exacte pour copie non résolue par T02 : lien seulement, aucun install.
  `VK_LAYER_KHRONOS_validation` absente de l'énumération T02, pas présumée disponible.

## Format natif et frontières de réemploi

- [OpenCode V2 agents](https://opencode.ai/v2/docs/agents),
  [skills](https://opencode.ai/v2/docs/skills) et
  [permissions](https://opencode.ai/v2/docs/permissions) : pages officielles
  récupérées le 2026-10-16 ; frontmatter/permissions et chargement progressif.
  Pages mutables, licence de copie non établie, liens seulement. La convention
  locale name=ID ne devient pas une obligation native inventée ; le modèle/effort
  reste à la politique de mission. Aucun champ Cursor importé par analogie.
- Audit T02 : `tarkansarim/CppStudio` est un candidat documentaire, pas un
  superviseur à importer. Son **arbre SHA** n'est pas un **commit** résolu. Aucun
  extrait repris, aucune commande de scaffolding exécutée. Toute future adaptation
  exige commit, chemin, licence et notices exacts ; licence inconnue = lien seulement.
- VMA/MoltenVK ne sont pas des dépendances d'ARTIST par ce skill. Une décision
  d'allocation ou de portabilité mérite son contrat, son owner et ses preuves.

Ces lectures ne constituent ni découverte native du nouveau skill, ni disponibilité
SDK/layer, ni compilation SPIR-V, ni création de device, ni capture/GPU/performance.
