---
name: vulkan-backend
description: Expertise Vulkan pour négocier device/features/queues, traduire descriptors/layouts/pipelines et diagnostiquer mémoire, barriers, semaphores, fences ou validation layers. À charger aussi pour un graphics pipeline Vulkan avec depth testing ; ne prouve pas un backend ARTIST ni un SDK disponible.
---

# Vulkan — contrats API et preuves conditionnelles

## Déclencheurs et exclusions

Charger pour une décision Vulkan : feature/extension, queue family, descriptor,
layout d'image, pipeline, transfert de propriété ou hazard concret. Charger
seulement la recette utile, pas tout le corpus GPU. La description permet la
sélection native ; sa présence textuelle ne prouve ni découverte ni appel natif.

« Crée un graphics pipeline Vulkan avec depth testing » : principal
`gpu-pipeline-engineer`, skills `gpu-pipelines` + `vulkan-backend`. Ce skill porte
la traduction Vulkan, pas la structure générique du pipeline. Un défaut
OpenGL seul → `gpu-engineer` ; API non précisée → clarification. Compilation
shader seule → `shader-engineer` ; solveur/simulation → `gpu-simulation-engineer` ;
allocation/retrait → `gpu-resource-engineer` ; scheduling → `render-graph-engineer`.
Ces IDs sont des frontières, pas un dispatcher exécutable.

## Pin ARTIST

Photographie source du contrat T03, base `40a7cc3eac80a4a2d344116365384d48bf92ae48` :
backend implémenté OpenGL, `ARTIST/include/graphic/opengl/` ; tests réels
`ARTIST/component-tests/` avec GLFW/GLEW 3.3 core. Vulkan est une **expertise**, pas
un backend livré. `PipelineContext.hpp` stocke les passes via `unique_ptr` ;
`getPass` retourne un emprunt. Une génération CPU ne prouve pas la fin GPU.
En cas d'écart au dépôt ouvert, produire `Pin observé`, pas réécrire le pin.
ADR-0001/0002/0004/0006/0007 : Proposed ; ADR-0003/0005 : Accepted/partial ;
ADR-0008 : Accepted/verified, périmètre Windows Debug seulement.

Frontières : compile = structure et variantes bornées ; link = instanciations
et visibilité ; cook = SPIR-V/réflexion immuables versionnés, clés
source/options/cible/ABI ; runtime = device, objets driver, résidences et frames.
Le cache de pipeline est propre à sa compatibilité driver/device/UUID ; il n'est
pas un asset SPIR-V universel. Dispatch **par lot** amortit sélection/bindings,
pas les hazards ou le coût driver. Pas de type par ressource/instance vivante.

## Préconditions, plateformes et outils

Lire [artist-build-validation](../artist-build-validation/SKILL.md) avant exécution.
Linux/Windows : loader+ICD/device et outils de la cible à vérifier. Apple via
MoltenVK exige les extensions/subset de portabilité observés ; ce n'est ni Metal
natif, ni une preuve Apple sous Windows. Aucun téléchargement/install au chargement.

Relever OS, GPU/UUID/driver, versions loader/device/application, features/extensions
supportées et activées, formats/limits, queues et indices, threads hôtes, shaders
(source/hash/entry point/options/cible SPIR-V), scène, seed, frame et résultat
attendu. Vulkan n'a pas de « contexte courant » OpenGL ; documenter le device,
les command pools/queues utilisés et leur synchronisation externe côté CPU.

T02, observation datée 2026-10-16 : `vulkaninfo --summary` a fonctionné, mais
`VK_LAYER_KHRONOS_validation` n'était pas listée ; pas de SDK Vulkan installé
identifié. Ne pas transformer `vulkaninfo` en preuve de layers, compilateur ou
application valide. Refaire uniquement les sondes nécessaires et autorisées,
sous bail du lead si ressource partagée. Pas de sonde/capture/benchmark implicite.

## Procédure courte

1. **Négocier.** Énumérer extensions d'instance/device et layers ; query des
   features/limits/formats/queues puis liste minimale activée à `vkCreateDevice`.
   Un nom GPU, une version d'API ou une extension listée ne suffit pas.
   Suivre [VK-01](references/recipes.md#vk-01--device-et-capacités).
2. **Définir l'interface.** Réflexion SPIR-V et entry point ↔ set/binding/type/
   count/stageFlags ↔ pipeline layout, push constants, vertex input/attachments.
   Vérifier offset/range/dynamic offsets et alignement CPU/GPU. Adapter les
   layouts/données, pas seulement faire compiler un shader incompatible.
3. **Créer et remplir.** Usage/format/tiling/sample count → memory requirements
   (`memoryTypeBits`, size, alignment) → allocation/binding → upload. Les flush
   non cohérents respectent `nonCoherentAtomSize` et la zone réellement mappée.
   Le contrat upload/dernier consommateur appartient au propriétaire ressources.
4. **Enregistrer et soumettre.** Pipeline compatible rendu/compute, descriptors
   vivants, queues autorisant les stages ; chaque command pool appartient à sa
   queue family, pas aux autres familles. Même ordre de soumission ≠ visibilité.
   Tenir un ledger RAW/WAR/WAW : ressource/range, producteur, consommateur,
   stages/access, layout avant/après et familles. Pour WAR, l'ordre peut suffire ;
   RAW/WAW exigent la dépendance mémoire appropriée. Une transition de layout ou
   un transfert d'ownership peut ajouter ses propres contraintes.
5. **Synchroniser au bon domaine.** Barrier = dépendance dans le flux de queue ;
   semaphore = dépendance de soumissions/queues, pas attente CPU implicite ;
   fence ou attente de timeline = observation de complétion côté hôte. Les
   semaphores ne remplacent pas une transition de layout ou un transfert de
   propriété EXCLUSIVE. Distinguer ordre, visibilité et attente CPU. Aucune
   correspondance universelle des « resource states » D3D12 vers Vulkan.
   [VK-03/VK-04](references/recipes.md) fixent stages/access/portées concrets.
6. **Consommer et retirer.** Attendre seulement le travail pertinent avant readback
   CPU ; invalider la mémoire non cohérente avec `vkInvalidateMappedMemoryRanges`.
   Retirer objets, allocations, descriptor pools et command pools après fin de
   toutes les utilisations de **toutes les queues** concernées. Un compteur de
   génération rejette un handle périmé, il ne rend pas une ressource libre.
   Ne pas comparer les valeurs de timelines indépendantes comme une horloge unique.
   La durée dépend de l'objet : un shader module peut être détruit après création
   réussie du pipeline qui l'utilise, pas les buffers/descriptors encore en vol.
7. **Isoler puis vérifier.** Pour image incorrecte, premier pass et ses inputs
   avant composition complète ; [VK-02](references/recipes.md#vk-02--graphics-et-depth)
   fixe un oracle depth sans WSI. `vkDeviceWaitIdle` peut discriminer une hypothèse,
   jamais devenir la correction par défaut sans mécanisme établi et mesure.

Pièges : feature supportée non activée ; mise à jour/reset d'un descriptor set
encore consommé (update-after-bind exige ses features/flags/conditions, pas une
exemption générale) ; mémoire libérée trop tôt ; command buffer pending réutilisé ;
cache périmé ; sRGB appliqué à des données ; mélange clip OpenGL [-1,1] et Vulkan
[0,1] pour Z ; oubli de convention viewport/frontFace. Pour WSI, la fin d'une
soumission ne prouve pas que la présentation a fini de consommer son semaphore.

## Validation et réussite

- **Statique/CPU** : contrats ABI, négociation, ledger et liens/config ; tests de
  compilation positifs/négatifs seulement avec toolchain fonctionnelle.
- **Compilation/offline** : compiler vers la cible SPIR-V exacte, puis `spirv-val`
  si réellement disponible avec `--target-env` correspondant. Noter versions,
  options, hashes et diagnostics. Cela ne prouve pas layout/pipeline/driver.
- **Validation API** : vérifier `vkEnumerateInstanceLayerProperties`, activer
  `VK_LAYER_KHRONOS_validation` seulement si disponible. `VK_EXT_debug_utils`
  requiert son extension ; créer le messenger/log si utilisable. Vérifier la
  configuration réellement supportée de synchronization validation et GPU-assisted
  avant activation ; garder leurs logs séparés et le VUID réellement émis, pas
  un VUID inventé. Une couche absente = INCONCLUSIVE pour ce critère, pas PASS.
- **GPU/image** : exécuter le nominal et un négatif isolé sous autorisation avec
  oracle gelé. Le négatif invalide ne va jamais dans le chemin de production.
  Comparer readback/image, pas seulement zéro warning. Destruction/recréation
  multi-frame et ressources en vol font partie d'un correctif de lifetime.
- **Performance** : campagne séparée, instrumentation déclarée, workload/qualité
  identiques et répétitions. Validation/GPU-assisted perturbent le coût ; aucun
  gain déduit d'une capture ni de ces recettes documentaires.

PASS seulement pour le critère exécuté, FAIL pour l'écart causal, INCONCLUSIVE
pour précondition absente, zéro test ou skip requis. Fournir candidat HEAD/delta/
hashes, commande/outils/version, découvert/exécuté/échoué/skipped, attendu/observé,
logs et statut par niveau. Les tests Node du lot contrôlent le texte, pas Vulkan,
la découverte native, l'enforcement ou la performance.

## Sources et transfert

[Sources officielles et dispositions](references/sources.md) ; recettes originales
nominales/négatives dans [recettes](references/recipes.md). Ne rien installer ni
importer depuis un lien. Limites/outils non disponibles restent explicites.

`DEFER_TO` au lead avec backend/candidat/contrat, fichiers/ownership, reproduction,
binding times, hazard/ledger, oracle, essais, inconnues et bail requis. Pour une
décision multi-API : `gpu-portability-engineer` ; architecture : `engine-architect` ;
validation indépendante : `rendering-verifier`. Une feuille ne délègue pas et ne
change pas de profil pour obtenir plus de droits.
