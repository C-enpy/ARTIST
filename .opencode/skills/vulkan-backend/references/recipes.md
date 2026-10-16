# Recettes Vulkan originales — à sélectionner, pas à lancer au chargement

Ces protocoles ne sont pas un backend, un test GPU déjà exécuté ni un script à
copier/compiler tel quel. Geler un candidat, les sources shader et leurs hashes,
API/features et un oracle avant d'écrire la fixture. Ne rien installer. Les
étapes invalides sont réservées à une fixture jetable autorisée, avec validation
effectivement active ; sinon le test négatif API reste INCONCLUSIVE.

## VK-01 — Device et capacités

**Préconditions.** Loader/ICD et device Vulkan réellement disponibles. Distinguer
la version loader, la version device et `VkApplicationInfo::apiVersion` demandée.
Sous 1.1, la voie `Features2` exige l'extension d'instance correspondante et ses
entry points ; ne pas appeler un symbole absent. Une version promue en core ne
signifie pas que ses features optionnelles sont activées.

**Nominal.** Pour une recette 1.3, consigner :

1. `vkEnumerateInstanceLayerProperties`, extensions instance et
   `vkEnumerateDeviceExtensionProperties`. Distinguer support, activation et
   compatibilité. Pas de `VK_LAYER_KHRONOS_validation` présumée.
2. `vkGetPhysicalDeviceFeatures2` avec chaîne `pNext` légale : features 1.3
   `synchronization2`/`dynamicRendering`, 1.2 `timelineSemaphore` si nécessaire.
   Ne pas doubler les structs promus et leurs alias extension dans la chaîne.
3. `vkGetPhysicalDeviceQueueFamilyProperties`, flags et queueCount : famille
   graphics/compute/transfer réellement utilisable ; présentation par surface
   seulement si la fixture utilise WSI. Une queue compute n'est pas graphics.
4. Limites et propriétés formats avec le tiling/usage/sample count voulus.
   Construire une chaîne d'activation minimale, séparée des résultats query,
   à `vkCreateDevice`. En chemin pré-1.3, vérifier extensions et features KHR
   nécessaires, puis charger les bons entry points : pas de fallback nominal
   obtenu en changeant uniquement un nom de fonction.
5. `VkDeviceQueueCreateInfo` par famille choisie, indices inférieurs à queueCount,
   priorités valides. L'application synchronise ses threads accédant à chaque
   queue/pool ; les queues distinctes ne créent aucune dépendance automatique.

**Négatif.** Fixture CPU de négociation : feature `synchronization2` requise mais
`VK_FALSE` dans les résultats query. Attendu : refus explicite AVANT création
ou fallback legacy équivalent justifié/testé, jamais `vkCmdPipelineBarrier2`
non activé. Dans une fixture API autorisée distincte, une feature non supportée
demandée à `vkCreateDevice` doit être rapportée avec le `VkResult` réel
(`VK_ERROR_FEATURE_NOT_PRESENT` attendu pour ce cas), pas ignorée.

**Oracle.** Trace des queries et de la chaîne activée ; nominal = contrat rempli,
négatif CPU = diagnostic nommant la feature/voie refusée, sans soumission.
Le faux résultat query est une fixture de négociation, pas une preuve du GPU.

**Limite.** `vulkaninfo --summary` prouve au mieux son énumération datée. Sans
outils/layers/fixture, pas de validation API. T02 n'a pas trouvé Khronos ni SDK ;
la lecture de cette recette ne change pas cet état.

## VK-02 — Graphics et depth

**Préconditions.** Device/queue graphics ; `dynamicRendering` et `synchronization2`
activés, ou variante legacy explicitement distincte. Format couleur
`VK_FORMAT_R8G8B8A8_UNORM`, profondeur `VK_FORMAT_D32_SFLOAT`, 64 × 64, un sample,
tiling optimal supportant les usages : couleur COLOR_ATTACHMENT + TRANSFER_SRC,
depth DEPTH_STENCIL_ATTACHMENT. Relever formats/features réellement queryés ;
pas de remplacement silencieux de l'oracle en cas d'absence.

**Nominal.** Une seule pass offscreen, sans WSI, scène/frame fixes :

- Vertex shader sans vertex buffer, positions XY (-0,75;-0,75), (0,75;-0,75),
  (0;0,75) par `gl_VertexIndex`, couvrant le pixel (32,32). Deux draws : profondeur NDC constante
  0,25 rouge puis 0,75 verte, couleurs alpha 1. Consigner les sources/entry points
  SPIR-V ; push constants depth float offset0, couleur vec4 offset16, bloc32 octets
  avec padding CPU explicite, range stages VERTEX|FRAGMENT. Le vertex lit depth,
  le fragment lit couleur ; confirmer les offsets dans le SPIR-V produit.
- Pas de matrices implicites : clip Z Vulkan [0,1], W=1, viewport positif 64 × 64,
  minDepth=0/maxDepth=1, scissor entier. Culling désactivé ; coordonnées et sens Y
  déclarés. Le test n'est pas une conversion de handedness. Ni blend, ni MSAA,
  ni tonemapping, ni sRGB : valeurs linéaires dans UNORM.
- `VkPipelineRenderingCreateInfo` décrit exactement formats couleur/depth,
  sample count du pipeline compatible avec les attachments de `VkRenderingInfo`.
  `depthTestEnable=VK_TRUE`, `depthWriteEnable=VK_TRUE`, `VK_COMPARE_OP_LESS` ;
  clear couleur noire alpha 1, depth=1. Viewport/scissor dynamiques définis si
  déclarés dynamiques. Render area 64², une couche/mip, stencil absent ; couleur
  loadOp=CLEAR/storeOp=STORE, profondeur loadOp=CLEAR/storeOp=DONT_CARE.
  Pipeline layout et push-constant ranges compatibles.
- Première utilisation, contenus jetés : UNDEFINED → COLOR_ATTACHMENT_OPTIMAL,
  destination COLOR_ATTACHMENT_OUTPUT/COLOR_ATTACHMENT_WRITE ; UNDEFINED →
  DEPTH_ATTACHMENT_OPTIMAL, destination EARLY_FRAGMENT_TESTS|LATE_FRAGMENT_TESTS
  et DEPTH_STENCIL_ATTACHMENT_READ|DEPTH_STENCIL_ATTACHMENT_WRITE. Pour une
  ressource recyclée en vol, UNDEFINED n'efface PAS l'obligation de synchroniser.
- Après fin du rendu, `VkImageMemoryBarrier2` couleur :
  `srcStageMask=VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT`,
  `srcAccessMask=VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT`,
  `dstStageMask=VK_PIPELINE_STAGE_2_COPY_BIT`,
  `dstAccessMask=VK_ACCESS_2_TRANSFER_READ_BIT`,
  layout COLOR_ATTACHMENT_OPTIMAL → `VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL`,
  aspect couleur/mip0/layer0 seulement, familles `VK_QUEUE_FAMILY_IGNORED`.
  `vkCmdCopyImageToBuffer` vers readback TRANSFER_DST, offset aligné, extent 64²,
  `bufferRowLength=0`, `bufferImageHeight=0` : 4 octets/pixel, 256 octets/ligne.
- Readback COPY/TRANSFER_WRITE → HOST/HOST_READ, puis fence de cette soumission
  et invalidation non cohérente comme VK-03. Ressources vivantes jusqu'à complétion.

**Négatif.** Deux cas séparés, ne pas mélanger leurs causes : (a) désactiver le
depth test dans la variante fixture, même ordre des draws : centre vert, donc
l'oracle rouge échoue sans exiger de message API ; (b) format couleur de
`VkRenderingInfo` différent du format déclaré au pipeline (sans fonctionnalité
de formats inutilisés/dynamiques qui légaliserait le cas) : diagnostic de
compatibilité attendu de la validation active, relever le VUID réellement obtenu.

**Oracle.** Nominal : readback pixel (32,32) RGBA=(255,0,0,255) ; fond (0,0)
RGBA=(0,0,0,255), depth visible indirectement par occlusion. Refaire ordre inversé
pour confirmer l'absence de dépendance à l'ordre, résultat rouge inchangé. Aucun
seuil visuel relâché pour rendre le négatif vert.

**Limite.** Ce protocole teste un pass et l'API, pas une composition PBR, un
resolve MSAA, le WSI, les performances ou tous les espaces du moteur.

## VK-03 — Descriptors, compute et readback

**Préconditions.** Queue compute acceptant les commandes de copie nécessaires,
`synchronization2` activé ; limites autorisant `local_size_x = 64` et 4 groupes.
Shader entier déterministe : 256 uint32, `output[i] = i ^ 0xA5u`, garde `i < 256`.
SPIR-V et layout concordent : set0/binding0, `VK_DESCRIPTOR_TYPE_STORAGE_BUFFER`,
count1, stage COMPUTE ; aucune hypothèse de float atomics/descriptor indexing.

**Nominal.** Buffer GPU de 1024 octets, usage STORAGE_BUFFER|TRANSFER_SRC, mémoire
compatible `memoryTypeBits`, taille/alignment de `vkGetBufferMemoryRequirements`.
Buffer readback TRANSFER_DST, mémoire HOST_VISIBLE ; ne pas présumer HOST_COHERENT.
Descriptor offset0, range1024 ; toute autre base respecte
`minStorageBufferOffsetAlignment`, dynamic offsets éventuels inclus dans les bornes.
Pool/layout/set, pipeline layout et shader déclarent le même binding/type/count.
Pas d'upload initial : le shader écrit tous les éléments ; aucun shader ne lit
une allocation non initialisée. S'il y a upload, documenter en plus son producteur.

Bind pipeline+set, dispatch (4,1,1), puis `vkCmdPipelineBarrier2` avec
`VkBufferMemoryBarrier2` portant sur ce buffer, offset0/size1024 :

- `srcStageMask=VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT` ;
  `srcAccessMask=VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT` ;
- `dstStageMask=VK_PIPELINE_STAGE_2_COPY_BIT` ;
  `dstAccessMask=VK_ACCESS_2_TRANSFER_READ_BIT` ;
- indices familles `VK_QUEUE_FAMILY_IGNORED`, pas de layout pour un buffer.

`vkCmdCopyBuffer` copie exactement 1024 octets vers readback. Nouvelle barrière
sur readback : src COPY/TRANSFER_WRITE, soit
`VK_PIPELINE_STAGE_2_COPY_BIT`/`VK_ACCESS_2_TRANSFER_WRITE_BIT`, dst
`VK_PIPELINE_STAGE_2_HOST_BIT`/`VK_ACCESS_2_HOST_READ_BIT`. Fence signalée pour
cette soumission, `vkWaitForFences` vérifié avant lecture CPU (timeout/erreur ≠
complétion). Pour mémoire non cohérente, `vkInvalidateMappedMemoryRanges` sur
allocation mappée : offset multiple de `nonCoherentAtomSize`, fin arrondie dans
l'allocation ou règle de fin d'allocation, jamais au-delà du mapping. Un atom
partagé avec des écritures hôtes actives nécessite isolation/coordination.
Lire les uint32 après copie vers stockage CPU aligné ; comparer les 256 valeurs.

**Négatif.** (a) `vkUpdateDescriptorSets` avec descriptorType UNIFORM_BUFFER sur
un binding STORAGE_BUFFER : violation précise de type, oracle diagnostic de
validation active, pas un test de compilateur shader. (b) Omettre la dépendance
compute-write → copy-read dans une fixture distincte : synchronization validation
attendue pour le hazard RAW si elle couvre ce chemin ; relever message/config.
Une copie apparemment correcte sans message n'autorise pas l'absence de barrière.
Un set périmé/génération erronée doit être rejeté avant binding par le contrat
CPU, pas soumis volontairement au driver pour « voir si cela marche ».

**Oracle.** Nominal = 256 comparaisons exactes contre `i ^ 0xA5u`, compte/logs et
complétion observée. Négatifs = type/hazard causal et non une erreur d'outil.
Destruction : après la complétion, retirer set/pool/buffers puis mémoire ; ne pas
reset un pool ou libérer une allocation pendant un usage pending.

**Limite.** Un mock ne valide pas les caches GPU. GPU-assisted peut compléter les
accès descriptors hors bornes seulement si disponible/configuré ; ce protocole
ne rend pas cette instrumentation universelle. Shader compilé ≠ descriptor valide.

## VK-04 — Deux queues, ownership et générations

**Préconditions.** Famille transfer distincte de compute, deux queues réellement
créées, timelineSemaphore et synchronization2 activés. Buffer
`VK_SHARING_MODE_EXCLUSIVE`, usage TRANSFER_DST|STORAGE_BUFFER ; buffer/range
identiques dans les deux halves du transfert. Si une seule famille existe,
ce scénario multi-famille reste INCONCLUSIVE, pas un PASS simulé.

**Nominal.** Upload par `vkCmdCopyBuffer` sur transfer. Release via
`VkBufferMemoryBarrier2` : src COPY/TRANSFER_WRITE ;
`dstStageMask=VK_PIPELINE_STAGE_2_NONE`, `dstAccessMask=VK_ACCESS_2_NONE` ;
srcQueueFamilyIndex=transfer, dstQueueFamilyIndex=compute. Submit transfer signale
la valeur n d'un timeline semaphore APRÈS le release :
`signal stageMask=VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT` dans `vkQueueSubmit2`.

Submit compute attend n avec `wait stageMask=VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT`.
Acquire correspondant : `srcStageMask=VK_PIPELINE_STAGE_2_NONE`,
`srcAccessMask=VK_ACCESS_2_NONE` ; dst COMPUTE_SHADER/SHADER_STORAGE_READ
(WRITE aussi si consommateur modifie), mêmes indices/range. Les portées semaphore
ALL_COMMANDS sont choisies ici pour encadrer les opérations d'ownership par défaut,
pas comme attente globale CPU ; ne pas les réduire au seul shader sans vérifier
les règles de transfert et les éventuelles extensions/flags autorisant cela.

Consommer, puis signaler sa complétion. Le ledger enregistre qui a produit n et
la dernière utilisation sur chaque queue. Les générations de handles empêchent
la réutilisation d'une référence CPU périmée, pas la destruction GPU anticipée.
Pour revenir à transfer, produire le transfert inverse ; une seule paire ne
confère pas une propriété permanente à toutes les queues.

**Négatif.** Omettre release/acquire mais conserver le semaphore : ordre inter-queue
présent, ownership EXCLUSIVE incorrect. Diagnostic de transfert attendu sous
validation active, même si le résultat semble juste. Autre contrat CPU séparé :
retrait demandé alors que le token de dernière utilisation compute n'est pas
complété → mise en retraite différée, aucune destruction/réallocation immédiate.

**Oracle.** Journal release → signal(n) → wait(n) → acquire → consommation →
complétion → retrait. Vérifier données et messages, plus plusieurs générations
avec charge en vol. Pour deux queues d'une MÊME famille, pas de transfert de
famille (`VK_QUEUE_FAMILY_IGNORED`) mais dépendance semaphore/mémoire conservée.
En mode CONCURRENT, pas d'ownership transfer ; hazards et transitions d'image
demeurent. Un semaphore ne change pas à lui seul le layout d'une image.

**Limite.** Pas de chronologie numérique commune entre semaphores indépendants ;
pas d'attente globale ajoutée pour blanchir le cas. La vie d'une ressource ne se
déduit pas du retour de submit ou de la seule destruction de l'owner C++.
