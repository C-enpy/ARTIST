# Procédures Metal — choisir le chemin réellement disponible

Recettes originales, à exécuter seulement dans un périmètre autorisé. Pas de
fixture Metal livrée dans le moteur ARTIST, pas de preuve GPU implicite. Les
sources officielles et limites sont dans [sources](sources.md).

## P1 — Capacités, environnement et reproduction

Noter macOS/iOS et build OS (le driver Apple dépend aussi de l'OS), appareil,
nom/identité du `MTLDevice`, familles via `supportsFamily`, limites/formats,
architecture CPU, SDK/Xcode/toolchain, cible de déploiement, version MSL et API
exacte. Distinguer Metal 3, commandes classiques `MTLCommandQueue`, et nouveaux
objets Metal 4 `MTL4CommandQueue` ; « Metal disponible » ne choisit pas la version.

- Un SDK qui expose un sélecteur n'autorise pas son appel sur un OS plus ancien.
  Vérifier availability OS, garde compile-time/weak-link appropriée et réponse
  du device pour la famille et chaque feature, format, sample count ou limite.
- À la consultation du 2026-10-16, `MTL4CommandQueue`/`MTL4CommandBuffer` sont
  documentés à partir de macOS/iOS/iPadOS/tvOS/visionOS 26.0. Ce seuil OS n'est pas
  une preuve de famille compatible. Metal 3 n'est pas non plus une promesse de
  toutes les features. Consulter les availability headers du SDK cible et les
  feature tables Apple ; ne pas coder une liste universelle depuis un nom GPU.
- Enregistrer supporté / activé / compatible et un fallback sémantiquement
  acceptable, ou un refus explicite. Cas **N-FAMILY** : ne pas construire le
  chemin Metal 4 si sa famille/feature requise manque ; ne pas invoquer un
  sélecteur indisponible pour « tester » sa présence.
- Un outil **non résolu sur PATH n'est pas une absence absolue**. T02 n'a pas
  obtenu de voie Apple sur Windows ; cela ne dit rien d'un Mac distant ou d'un
  chemin non sondé. Demander une machine/voie autorisée, sans installation.

Sur un Mac autorisé uniquement, relever `sw_vers`, `xcodebuild -version`,
`xcode-select -p`, `xcrun --sdk macosx --show-sdk-path`, puis
`xcrun --sdk macosx --find metal` et `xcrun --sdk macosx --find metallib`.
Leur succès ne prouve toujours ni device ni frame. Ne pas changer xcode-select.

Réduction : fixer shaders/entry/options/hash, scène, caméra, seed, frame, taille,
formats, numéro de queue/command buffer, thread d'encodage et oracle. Pas de
« contexte courant OpenGL » fictif : relever device/queues/encoders Apple et
thread UI/AppKit si présentation, pool autorelease par thread concerné.

## P2 — Ownership, upload, heaps et retrait

Tenir un ledger par allocation : owner CPU, emprunts, génération, size/alignement,
storage/cache/hazard mode, heap/offset/aliases, usages et dernières complétions
sur **toutes les queues**. Une génération périmée rejette un handle CPU ; une
génération ne constitue jamais une complétion GPU. Le compteur de frame non plus.

1. **Créer** : metal-cpp ne bénéficie pas d'ARC pour ses objets C++. Les règles
   Cocoa alloc/new/copy/mutableCopy produisent une référence possédée (+1) à
   équilibrer par `release`. `NS::TransferPtr` adopte une référence possédée ;
   `NS::RetainPtr` acquiert une référence supplémentaire sur un objet emprunté.
   Ne pas employer `delete`, double-release ou transfert d'un emprunt non retenu.
   Encadrer les objets `autorelease` par un pool borné à chaque thread/frame ;
   retenir explicitement ce qui doit survivre au drainage du pool. Les callbacks
   de complétion ne doivent pas capturer un owner CPU déjà détruit.
2. **Allouer** : choisir shared/private/managed selon device/OS et CPU/GPU usages.
   Shared n'autorise pas une écriture CPU pendant une lecture GPU. Private exige
   une copie GPU depuis staging. Sur managed lorsque disponible : publier les
   écritures CPU avec `didModifyRange`, synchroniser le retour GPU→CPU approprié
   puis attendre sa complétion avant lecture ; ni une barrière shader ni le
   simple mode mémoire ne remplace ce protocole.
3. **Heap** : demander size/alignment au device pour le descriptor effectif,
   vérifier offsets et budget, documenter aliasing. Les allocations qui se
   chevauchent ne deviennent réutilisables qu'après leurs dernières utilisations
   et les dépendances requises. `makeAliasable` n'attend pas les commandes en vol.
   Une texture memoryless ne sert pas de stockage persistant entre passes/frames.
4. **Résidence/binding** : garder vivants backing heap, ressources, vues,
   samplers et structures indirectes. Une adresse GPU dans un argument buffer ou
   une argument table n'est pas un owner. Vérifier `useResource`/`useHeap` quand
   nécessaires pour les accès indirects du chemin classique ; renseigner usage
   et stages réels. Avec `MTLResidencySet` disponible, ajouter allocations/heaps,
   `commit` des changements, associer le set au command buffer/à la queue selon
   l'API ; retirer seulement après dernières utilisations. Une résidence permet
   l'accès GPU, elle ne prouve ni ownership C++, ni visibilité mémoire.
5. **En vol** : un `MTLCommandBuffer` classique avec `retainedReferences=true`
   conserve ses références directes ; ne pas en déduire qu'il retient toutes les
   ressources indirectes. Avec `retainedReferences=false`, maintenir soi-même
   les références jusqu'à complétion. Un `MTL4CommandBuffer` ne retient pas les
   ressources : contrat d'owner explicite jusqu'au dernier usage GPU.
6. **Recycler/détruire** : retirer les ressources en vol selon les tokens de
   complétion de toutes les queues, y compris copie/compute/render. Metal 4 peut
   réencoder un command buffer après soumission ; cela n'autorise pas à reset son
   `MTL4CommandAllocator` avant la complétion de tout travail utilisant sa mémoire.
   Employer des slots d'allocators/ressources distincts. Avant shutdown/resize,
   cesser de soumettre, récupérer les complétions, libérer les owners dans les
   contraintes de thread de leurs objets (notamment UI), puis drainer les pools.

**N-INFLIGHT** vérifie l'interdiction de destruction/réutilisation/reset prématuré
au niveau du contrat, sans provoquer volontairement un use-after-free du driver.

## P3 — Encoders et dépendances, pas de barrière universelle

Tracer producteur → sous-ressource/plage → consommateur : RAW, WAR, WAW, stages,
lectures/écritures, queue et frontière encoder/command buffer/CPU. Distinguer
**ordre**, **visibilité** et **attente CPU**. Lire/lire n'exige pas de dépendance
de données ; aliasing, accès indirect et load/store implicites des attachments
peuvent en introduire. Terminer chaque encoder selon son API ; ne pas partager
un encodeur entre threads sans garantie documentée.

- Chemin classique : vérifier le hazard mode effectif, pas seulement `default`.
  Device/heap/ressources indirectes n'ont pas nécessairement le même tracking.
  Ne compter sur tracked que dans sa portée garantie. Pour des passes non
  suivies, placer `MTLFence` update/wait aux stages producteur/consommateur
  appropriés ; une fence entre passes n'est ni une fence CPU ni un event
  inter-queues. Pour un conflit dans une passe, choisir la primitive intra-pass
  supportée plutôt qu'une fence inter-pass inapplicable.
- **Metal 4 : `hazardTrackingMode` est sans effet pour `MTL4CommandQueue`.**
  Identifier soi-même les dépendances ; choisir les barriers producteur,
  consommateur ou intra-pass et leurs stages/access/portées d'après l'API SDK
  réellement employée. Le compute Metal 4 est concurrent : ne pas hériter de
  l'hypothèse de dispatchs sériels du compute classique par défaut. La présence
  d'un residency set ne rétablit pas le hazard tracking.
- Inter-queues d'un device : `MTLEvent` signal/wait avec valeurs cohérentes ;
  ne pas supposer l'ordre CPU des commits suffisant. Pour interaction CPU ou
  plusieurs devices/processus lorsqu'elle est supportée : `MTLSharedEvent` et
  notifications/valeurs. Les callbacks CPU exigent leur propre synchronisation.
- Le CPU lit un résultat staging/shared seulement après la complétion du travail
  qui l'écrit et, si nécessaire, la copie/synchronisation de mémoire managed.
  Une attente CPU globale temporaire peut discriminer un hazard ; la correction
  décrit la dépendance précise, pas un `waitUntilCompleted` dans chaque frame.

## P4 — Compilation, pipeline, ABI et image

Conserver source, cible/standard MSL, entry points/stages, defines, options
mathématiques, specializations/function constants, SDK/compiler et hashes. Le
**MSL généré sur Windows ne prouve ni compilation Apple ni pipeline ni image**.
Un converter DXIL→Metal ou Slang est une option de compilation distincte ; ne
pas lui attribuer les garanties du compilateur Apple ni l'adopter dans ARTIST.

Sur Mac autorisé avec source réduite fournie et `METAL_STD` choisi selon le SDK,
procédure offline illustrative (pas de téléchargement ni exécution implicite) :

```sh
xcrun --sdk macosx metal -std="$METAL_STD" -c Repro.metal -o Repro.air
xcrun --sdk macosx metallib Repro.air -o Repro.metallib
```

Consigner aussi cible de déploiement/options nécessaires à la plateforme réelle.
Ne pas les deviner ni reprendre macosx pour iOS. Ces commandes testent un artefact
offline Apple, pas les nouvelles APIs de compilation Metal 4. Le chargement des
fonctions et la création du pipeline réel sont des contrôles supplémentaires ;
collecter les erreurs complètes, même après compilation réussie.

- Graphics : entry/stages compatibles, `MTLVertexDescriptor`, slots/step rate,
  stride/offset/type/alignement, fonctions et formats color/depth/stencil du
  render pass, `sampleCount`, load/store, resolve, viewport/scissor, culling,
  depth/blend. Ne pas lire un attachment marqué don't-care comme sortie valide.
- Compute : fonctions/bindings, tailles des buffers, workgroup dimensions,
  limites par pipeline/device, threadgroup memory et bornes en bord de dispatch.
  Aucun nombre de threads codé depuis un autre GPU sans vérification.
- ABI : documenter `buffer(0)`/textures/samplers/argument buffers ou argument
  tables Metal 4. Vérifier tailles **et offsets de chaque champ** CPU/MSL ;
  `float3` aligné (typiquement 16) ≠ `packed_float3` (12), ni implicitement un
  tableau CPU de trois floats. Comparer reflection quand disponible et un test
  sentinelle ; la reflection seule n'établit pas l'image. Ne pas réutiliser
  aveuglément std140/HLSL packing comme ABI Metal.
- Espaces : matrices row/column et transposition, unités/normales, handedness
  choisie par l'application, convention de viewport/Y/UV et clip/depth Metal
  (z normalisé 0..1). Ne pas copier le depth OpenGL -1..1 sans adaptation.
  Séparer textures données linéaires, couleur sRGB, conversion de l'attachment,
  alpha, gamut/HDR/exposure et tonemapping ; empêcher double gamma/conversion.

## P5 — Drawable, present, erreurs et capture

Obtenir le drawable tard et limiter les références conservées pour ne pas
épuiser le pool. `nextDrawable`/currentDrawable peuvent échouer : un
**drawable nul** suspend cette frame, sans soumettre une cible absente ni deadlock sur un
slot en vol. Sur resize, renouveler les attachments à la taille/formats corrects
et retirer les anciens après complétion, pas après simple changement de pointeur.

- Classique : encoder la cible issue du drawable, terminer les encoders,
  programmer `presentDrawable` sur le command buffer avant commit. Ne pas
  confondre fin d'encodage, command buffer completed et affichage à l'écran.
- Metal 4 : la recette Apple attend la disponibilité par `waitForDrawable`,
  commit les command buffers, annonce la fin GPU avec `signalDrawable`, puis
  appelle le drawable `present()`. Vérifier le device commun et les residency
  sets du view/renderer. Ce n'est pas un appel aveugle au present classique.
- Pour chaque soumission : relever status/erreurs de complétion et labels
  ressource/pipeline/pass/frame. Activer Metal API Validation et Shader Validation
  seulement si l'environnement/SDK les offre. Une absence de message n'est pas
  une preuve d'absence d'erreur. Conserver les messages pertinents, pas des logs
  synthétiques ; le ralentissement de validation n'est pas un benchmark release.
- Si disponibles et autorisés, employer Xcode GPU Frame Capture/Metal Debugger
  ou `MTLCaptureManager` (destination prise en charge, scope/device/queue et
  erreurs de démarrage vérifiés). Capturer une frame réduite et examiner les
  bindings, étapes/attachments et ressources réelles. Comparer séparément image
  et oracle. Sans voie Apple, capture = **INCONCLUSIVE** ; RenderDoc ne remplace
  pas ces outils sur Metal. Ne pas inventer de MCP, capture ni automatisation.

## P6 — Scénarios et oracles avant patch

### NOMINAL — un pass et une consommation vérifiable

Préconditions : machine Apple autorisée, SDK/OS/famille compatibles, shader
minimal fourni, contexte fenêtre seulement si présentation, aucune fixture ARTIST
Metal supposée. Déclarer graphics ou compute, ne pas mélanger les oracles.

Graphics : triangle à sommets fixes, caméra identité, cible 64×64 RGBA8Unorm,
clear noir, couleur constante rouge linéaire, sans blend/MSAA/depth/tonemapping.
Comparer un pixel intérieur et un extérieur éloignés des bords ; répéter après
resize et sur au moins deux réutilisations de chaque slot en vol. Comparer la
présentation séparément de la cible offscreen. Compute : vecteur d'entiers
sentinelles, opération déterministe et bornée, sortie CPU exactement attendue
après copie/complétion ; observer plusieurs rotations des slots.

**Attendu** : valeurs exactes du cas choisi, pipeline compatible, aucun diagnostic
pertinent, retrait seulement après dernière consommation. **Oracle** : valeurs
analytiques déclarées avant run, readback à la bonne complétion, ledger de tokens
et messages de validation. **Intent** : distinguer données/ABI/pipeline de pacing.
Un triangle correct ne démontre pas le solveur ni la performance du moteur.

### N-INFLIGHT — destruction, overwrite ou allocator reset prématuré

Préparer deux frames utilisant le même slot ; laisser son token incomplet dans
un test de contrat CPU, sans submit illégal au driver. Tenter retrait/réutilisation
et reset allocator ; inclure une seconde queue dont le dernier token reste actif.
**Attendu** : refus ou report explicite, owner/backing vivant, autre slot possible ;
retrait admis uniquement quand toutes les dernières utilisations sont complètes.
**Oracle** : ledger/tokens/génération et comptages d'allocations/destruction.
**Intent** : un retain CPU, la résidence ou une génération neuve ne remplace pas
le retirement GPU. Ce test CPU ne prouve pas la complétion réelle ; la variante
Apple valide les tokens et le résultat multi-frame sans use-after-free volontaire.

### N-FAMILY — feature/famille requise non supportée

Injecter au gate de sélection un profil explicitement incompatible (et le tester
sur device incompatible si disponible), sans fausse réponse du driver.
**Attendu** : fallback documenté de qualité équivalente ou refus diagnostic avant
création/appel non supporté. **Oracle** : chemin choisi et zéro appel du chemin
interdit ; si aucun device incompatible n'est accessible, preuve native de ce cas
INCONCLUSIVE. **Intent** : un SDK récent/nom commercial GPU ne suffit pas.

### N-ABI — layout CPU/MSL incompatible

Après nominal, changer uniquement le stride/offset ou packing d'un champ float3
dans une fixture contrôlée, en gardant source/options/cible connues.
**Attendu** : contrôleur ABI rejette avant submit, ou test sentinelle échoue
nettement à l'offset attendu ; ne pas affaiblir la tolérance ni changer le golden.
**Oracle** : layout CPU (`sizeof`/`alignof`/offsets), contrat MSL/reflection et
readback sentinelle. **Intent** : shader compilé ≠ interface/pipeline valides.
Si aucun contrôleur/pipeline Apple n'existe, documenter GAP, ne pas en inventer un.

Tests texte de T08 : présence de ces procédures et mutations inertes seulement.
Exécution de ces quatre scénarios, compilation Apple, qualité d'image et mesures
restent à établir sur le candidat et la plateforme autorisés.
