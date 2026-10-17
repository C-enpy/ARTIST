# Procédure D3D12 conditionnelle et oracles

Document de travail, pas un harness ni un journal d'exécution. Utiliser uniquement
un candidat/harness autorisé ; pas d'installation, capture ou benchmark implicite.
Un test textuel de ce document ne prouve aucun appel Direct3D 12.

## 1. Reproduction et frontières

Identifier HEAD+delta et hashes, Windows/runtime/SDK/Agility, GPU/adapter LUID/node,
driver, queues employées, threads de recording/soumission, shaders source/entry/
profil/options/hash DXIL, scène/frame, capacités requises et attendu. D3D12 n'a
pas le « contexte courant » OpenGL : vérifier device, objets associés et exclusion
des enregistrements concurrents sur une même list/allocator ; ne pas inventer un
make-current. Si l'erreur est visuelle, isoler un pass et ses entrées d'abord.

Conserver un ledger par ressource/génération/subresource : propriétaire, alias
mémoire, producteur, lecteur/écrivain, plage, queue et dernière fence de chaque
queue. `gpu-resource-engineer` possède les invariants RAW/WAR/WAW et l'ownership ;
ce skill propose leur traduction D3D12 au lead, pas une nouvelle architecture.

## 2. Création, upload, binding et PSO

1. Interroger `CheckFeatureSupport` pour le Shader Model, les formats/usage/MSAA,
   resource binding tier et root signature. **Root Signature 1.1** n'est pas acquise
   parce que les headers exposent ses enums : respecter version et flags de
   stabilité/volatilité de descriptors/données. Fallback 1.0 seulement si le
   contrat reste représentable, sinon refus explicite.
2. Confronter réflexion DXIL et root signature : `register` b/t/u/s, `space`,
   nombre de descriptors, offsets/ranges, visibilité stages, static samplers,
   constantes et limite du root layout. Sérialisation réussie ≠ bindings valides.
3. Vérifier heap type/capacité, handle CPU/GPU et incrément issu du device, pas
   sizeof supposé. `SetDescriptorHeaps` lie au plus un heap shader-visible
   `CBV_SRV_UAV` et un `SAMPLER`. Les `RTV/DSV` ne sont pas ces tables
   shader-visible. Après changement de heap, rétablir les descriptor tables
   concernées ; après changement de root signature, rétablir ses paramètres.
   Distinguer `SetGraphicsRoot...` et `SetComputeRoot...`. Les descriptors ne
   retiennent pas l'objet ressource ; garder ressource, heap et contenu du slot
   valides jusqu'à la dernière consommation en vol, selon les flags RS retenus.
4. PSO graphics : root signature compatible, VS/PS et stages réellement utilisés,
   input layout (semantic/index/format/offset/stride, instancing), topologie,
   RTV/DSV formats, sample count, raster/depth/blend. PSO compute : CS, root
   signature, dimensions de dispatch et limites du device. Un PSO n'initialise
   pas tous les bindings/viewport/scissor ; le fixer ne lie pas à lui seul la
   root signature. Vérifier les HRESULT de création/Close/Reset, pas seulement
   celui de compilation DXIL.
5. CPU/GPU : CBV adresse/taille alignées à 256 octets ; structured stride vs raw
   views/typeless et bornes/offsets exacts. Pour les copies textures, utiliser
   `GetCopyableFootprints`, RowPitch/numRows/rowSize et offset de chaque subresource,
   pas width×bpp pour du BC. Baseline classique : pitch aligné 256 et placement
   512 ; aucune extension relaxant ces contraintes supposée. Vérifier séparément
   l'alignement des placed resources via l'information d'allocation du device.
6. Matrices CPU/HLSL (row/column-major et ordre mul), espaces, unités, normales,
   winding/cull, profondeur clip [0,1], viewport et handedness explicitement
   choisis ; D3D12 n'impose pas à lui seul une scène left-handed. Distinguer format
   ressource et vues, couleur sRGB vs données linéaires, HDR/tonemapping/sortie.
   Préserver le signal visuel, pas ajuster l'image de référence pour cacher l'écart.

## 3. Allocators, lists, queues et réutilisation

- `ID3D12CommandAllocator::Reset` n'est sûr que sans list en cours de recording
  associée et après complétion de toutes les utilisations GPU de sa mémoire.
  Aucune list enregistrée encore destinée à soumission ne doit dépendre de la
  mémoire réinitialisée ; terminer le GPU ne règle pas les références non soumises.
  Aucun Reset concurrent du même allocator. Associer sa retraite aux tokens de
  soumission réels, pas à un modulo frame ou au seul retour de Present.
- `ID3D12GraphicsCommandList::Reset` est différent : une command list fermée
  peut être réenregistrée alors que son précédent travail s'exécute, avec un
  allocator compatible et disponible pour le recording. Cela n'autorise PAS
  le reset/recyclage de la mémoire de l'ancien allocator en vol. Ne pas utiliser
  un allocator déjà attaché à une autre list en cours d'enregistrement.
- `ExecuteCommandLists` puis `ID3D12CommandQueue::Signal(fence, valeur)` place le
  signal après le travail de cette queue. Une fence monotone par timeline évite
  les valeurs ambiguës entre producteurs ; consigner la valeur de chaque submit.
  Un signal CPU `ID3D12Fence::Signal` ne démontre pas l'achèvement du GPU.
- Dépendance inter-queues : signal producteur puis `ID3D12CommandQueue::Wait`
  côté consommateur avant l'accès. Ce Wait ne bloque pas le CPU ; il impose une
  dépendance GPU. Pas d'ownership transfer de queue family Vulkan à recopier.
- Pour réutiliser un slot upload/descriptor/allocator ou libérer son owner CPU,
  vérifier `GetCompletedValue` sur **toutes les queues** concernées (ou une
  jonction de fences démontrée). Si le token n'est pas terminé, différer/allouer
  un autre slot, ou utiliser `SetEventOnCompletion` et une attente CPU bornée
  autorisée ; ne jamais supposer que Map d'un readback attend le GPU.
  `UINT64_MAX` de GetCompletedValue signale un device removed, pas une retraite
  réussie. Traiter HRESULT/device-lost sans boucler ni réutiliser les ressources.
- COM/RAII prouve la vie CPU, pas la fin des commandes. Owner, emprunts et
  génération sont distincts du token de complétion. Un handle périmé est refusé
  avant binding ; incrémenter sa génération ne permet pas un free immédiat.
  Conserver aussi les objets/stockages référencés selon leurs contrats D3D12,
  notamment descriptors, ressources, heaps, PSO et root signatures en usage.

## 4. États, accès et Enhanced Barriers

Séparer **ordre d'exécution**, **visibilité des accès** et **attente CPU**. Une
transition ne remplace pas le Wait inter-queues ni l'attente avant réemploi CPU ;
une fence terminée ne corrige pas un state/binding invalide au moment du draw.

### Voie legacy

- Suivre `D3D12_RESOURCE_STATES` par subresource et usages autorisés de la queue.
  Before correspond à l'état connu ; After aux accès du consommateur. Une écriture
  n'est pas à combiner librement avec des bits de lecture. Une transition couvre
  les usages et synchronisations qu'elle spécifie, pas tous les hazards du frame.
- Distinguer transition, barrière UAV pour accès UAV dépendants sans changement
  d'état, et barrière aliasing pour réaffectation de mémoire chevauchante. Ne pas
  cumuler toutes les barrières « par sécurité » ; partir du ledger RAW/WAR/WAW.
- `COMMON`, promotion implicite et decay sont soumis à conditions/type de
  ressource et frontières d'exécution. Ne pas supposer un decay entre deux lists
  du même `ExecuteCommandLists`. Consigner les conditions ou utiliser des états
  explicites conformes. Les transferts vers/depuis COPY queue exigent les règles
  COMMON correspondantes en plus des fences ; pas un transfert de famille Vulkan.
- Heaps UPLOAD classiques : état GENERIC_READ ; READBACK : COPY_DEST. Ne pas
  inventer des transitions de ces buffers. Backbuffer : transition compatible
  vers RENDER_TARGET puis PRESENT avant présentation. Split barriers seulement
  avec paire/plage ordonnées et sans accès interdit entre les deux moitiés.

### Voie enhanced — gate obligatoire

Vérifier SDK/runtime/Agility compatibles, succès de `CheckFeatureSupport` avec
`D3D12_FEATURE_D3D12_OPTIONS12`, `EnhancedBarriersSupported == TRUE` et interface
`ID3D12GraphicsCommandList7` disponible avant `Barrier`. Headers/DXC/GPU marketing
ne suffisent pas. Feature absente ou interrogation échouée : **fallback legacy**
qui préserve le contrat, ou refus explicite si non représentable.

Décrire pour chaque barrière `SyncBefore`/`SyncAfter` (portée d'exécution),
`AccessBefore`/`AccessAfter` (accès/visibilité), et pour les textures
`LayoutBefore`/`LayoutAfter` avec les subresources exactes. Les buffers n'ont
pas de texture layout. Vérifier compatibilité des sync/access/layouts et du type
de queue. Ne pas transcrire des bits legacy ou Vulkan par substitution de noms ;
ne pas conserver les règles promotion/decay legacy comme un modèle enhanced.
Si les deux modèles coexistent, définir et prouver leurs frontières d'interop,
pas alterner implicitement au milieu d'une chaîne d'accès.

## 5. Résidence, destruction et diagnostic

Allocation/COM/state corrects ne garantissent pas la résidence. Inventorier objets
pageables, budget/courant (adapter), heaps et politique de dépassement ; création
non-résidente éventuelle requiert une prise en charge explicite. Avec
`EnqueueMakeResident`, vérifier le résultat et la fence de fin de résidence,
ordonner l'usage GPU après celle-ci. `Evict`, réutilisation d'alias ou destruction
seulement après les dernières consommations pertinentes. Un budget dépassé peut
causer échec/stall : relever l'observation, pas annoncer un gain par suballocation.

Si disponible, activer debug layer **avant la création du device** ; configurer
GBV avant celui-ci aussi. Lire `InfoQueue` avec sévérité/ID/texte/objets/frame,
nommer les objets et conserver les premiers messages causaux. GBV produit des
messages asynchrones : attendre la fence du travail diagnostiqué avant de les
collecter. Son instrumentation/modification du chemin d'exécution n'est **pas une mesure de performance**.
Si D3D12GetDebugInterface/GBV est indisponible, qualifier l'absence sans installer
un SDK ou affirmer « zéro erreur ». Device removal : raison et DRED si configuré
et disponible ; aucune collecte ni intégration PIX automatisée présumée.

## 6. Oracles conditionnels, à matérialiser dans un harness autorisé

### D12-N1 — nominal minimal reproductible

Windows + device identifié, capacité Shader Model/RTV/copies requise et harness
réel sont des préconditions. Fixer 64 × 64, frame 0, un triangle non indexé en
clip space (-0.5,-0.5), (0.5,-0.5), (0,0.5), z=0.5, w=1 ; matrices identité,
cull/depth/blend désactivés, pas de MSAA, clear noir opaque. VS/PS minimaux
(positions et couleur rouge opaque constante), source/entry/options/DXIL hashés ;
profils vs_6_0/ps_6_0 seulement après confirmation du support Shader Model 6.0.
Une CBV b0/space0 fournit l'identité, avec root descriptor table et slot dédiés.
Formats : RT offscreen `R8G8B8A8_UNORM` linéaire, aucune conversion sRGB implicite.

Chaîne sur une seule DIRECT queue : créer VB DEFAULT COPY_DEST et staging UPLOAD,
copier VB → transition VERTEX_AND_CONSTANT_BUFFER ; lier heaps/root signature/
PSO/CBV/VB/viewport/scissor/topologie → DrawInstanced(3) → RT RENDER_TARGET vers
COPY_SOURCE → copie dans READBACK COPY_DEST avec footprints → Signal. Attendre
la fence de cette copie avant Map/lecture CPU ; garder tous les inputs/descriptors
jusqu'à la fin de leurs usages. Refaire plusieurs frames et recycler les slots
seulement à retraite, en enregistrant leurs générations et valeurs de fence.

Oracle : pixel central intérieur rouge RGBA `(255, 0, 0, 255)`, pixel extérieur
noir opaque, sans comparaison des arêtes de rasterisation ; zéro message ERROR/
CORRUPTION inattendu quand validation disponible, aucun owner libéré avant son
dernier token. Ne pas modifier la référence/tolérance pour verdir. Ce nominal
ne prouve ni compute, ni multi-queues, ni Enhanced Barriers, ni performance.

### Contre-exemples discriminants

- **D12-X1 — allocator en vol** : token de retraite non terminé ; demander son
  recyclage. Oracle : garde CPU refuse/diffère Reset et aucun réemploi de mémoire.
  Après preuve de complétion, Reset est permis et N1 reste correct. Ne pas lancer
  volontairement l'UB allocator-reset GPU comme condition du test. Tester à part
  list reset avec autre allocator disponible pour ne pas interdire le cas légal.
- **D12-X2 — mauvais state** : modèle de plan omettant COPY_DEST → VERTEX_AND_CONSTANT_BUFFER
  ou RENDER_TARGET → COPY_SOURCE. Oracle : garde du plan rejette avant submit ;
  si une reproduction API invalide est autorisée séparément, obtenir un diagnostic
  state correspondant de la debug layer/GBV. Une mauvaise image seule ne localise
  pas cette cause. Variante multi-queues : retirer Wait producteur/consommateur,
  vérifier que le ledger rejette, puis établir Signal/Wait et états corrects.
- **D12-X3 — mauvais binding** : PSO/shader b1 avec root signature n'exposant que
  b0, ou table vers heap/génération erronés. Oracle : incompatibilité rejetée à
  création/validation ou avant binding ; nominal inchangé après correction.
  Une compilation HLSL réussie n'est pas le résultat attendu de ce négatif.
- **D12-X4 — ownership/réemploi** : remplacer un owner tandis qu'une queue lit
  encore son ancien descriptor. Oracle : ancien handle refusé côté CPU pour les
  nouveaux accès ; ancienne ressource/slot conservés jusqu'à toutes ses fences.
  Après retrait, nouveau handle/génération valides ; aucune éviction prématurée.
- **D12-X5 — feature absente** : OPTIONS12 indisponible ou support FALSE ; aucun
  appel enhanced. Oracle : fallback legacy annoncé et même résultat N1, ou refus
  avant submit si le contrat exige une feature non représentable. Un résultat de
  feature injecté teste le garde CPU, PAS le support matériel.

Fixer les logs/assertions attendus avant implémentation, distinguer erreurs
intentionnelles du négatif et erreurs inattendues du nominal. Le debug layer ne
détecte pas tous les hazards ; absence de message n'est pas une preuve d'absence.
Sans device/harness/validation autorisés : **INCONCLUSIVE** pour ces oracles,
jamais PASS obtenu par une fixture textuelle, un shader compilé ou zéro test.
