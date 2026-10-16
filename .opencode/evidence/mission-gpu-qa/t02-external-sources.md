# T02 — provenance et disposition du corpus externe GPU/QA

Audit HTTP en lecture seule du 2026-10-16, via Context Mode urllib.request.
Aucune installation, exécution de code téléchargé ou import de skill externe.
Les recommandations ci-dessous ne sont ni une autorisation d'installation ni
une décision d'architecture adoptée. Voir `t02-source-inventory.md` pour le local.

## Contrat de lecture et couverture

Corpus GPU.md sections 4–5 : 13 dépôts contenant des compétences, 41 entrées
de contenu inspectées (13 SKILL.md hors Apple + 7 GUIDE.md CppStudio + 20
SKILL.md Apple + 1 agent Apple). Un module GUIDE.md n'est pas un Agent Skill.
Les arbres GitHub récursifs des 13 dépôts ont répondu sans troncature ; les
41 fichiers ont été récupérés et leurs préconditions/effets de bord inspectés.
La licence est celle signalée par l'API GitHub au niveau dépôt, non une garantie
sur chaque fichier ou dépendance. Copie interdite tant que les notices exactes
et attributions nécessaires ne sont pas relues par l'auteur intégrateur.

40 dépôts GitHub distincts explicitement liés dans le corpus, plus le dépôt
vkd3d-proton recherché et vérifié : 41 métadonnées de dépôt accessibles.
32 dépôts de référence section 5 (dont trois aussi section 4), README récupéré
pour 31 ; README Vulkan-Tutorial non trouvé aux trois noms sondés, sans rendre
le dépôt indisponible. Cet audit porte sur les entrées et documents prioritaires,
pas sur une lecture exhaustive des millions de lignes des bibliothèques.
12 URL de documentation/formats ont été sondées ; 9 HTTP 200, 3 HTTP 403
(dont OpenCode V2 hors corpus GPU, pour le catalogue natif).

Convention de chaque ligne : `https://github.com/<dépôt>` est l'URL vérifiée ;
les chemins sont relatifs au dépôt, vérifiés par tree API puis raw HTTP HEAD.
Date d'observation commune : 2026-10-16. Les SHA ci-dessous sont des SHA d'arbre
renvoyés par `/git/trees/HEAD`, pas des commits prétendument épinglés.
Le contenu raw HEAD a été lu immédiatement après, mais aucune atomicité entre
ces requêtes n'est garantie. Une reprise reproductible doit épingler un commit
résolu et recontrôler les notices. Statut par défaut : accessible HTTP, non testé
localement. Couverture locale par défaut : documentaire seulement, aucun backend
nouveau. Ces champs communs s'appliquent à chacune des sous-entrées.

## 4A — Vulkan et C++ GPU

### tarkansarim/CppStudio

URL : https://github.com/tarkansarim/CppStudio ; arbre
`69ca731da7145b4c57e5282d69cfe30bda3bb2ba` ; updated 2026-10-14 ; Unlicense
signalée, LICENSE présent. Racine `skills/cpp-cuda-vulkan-studio/`.

- `SKILL.md` : vrai skill, 1 834 caractères ; **référencer**, pas importer son
  backbone. Le routage modulaire est utile, le superviseur existant ARTIST prime.
- `modules/vulkan-compute-sync/GUIDE.md` : guide interne ; **adapter** concepts
  barrières/queues/descriptors au futur spécialiste Vulkan ; SDK/device/layers
  requis pour preuve, pas couverts par le seul skill ARTIST actuel.
- `modules/modern-cpp-cmake/GUIDE.md` : guide interne ; **référencer** seulement,
  redondant avec artist-build-validation/cpp-contracts ; ne pas changer Conan.
- `modules/gpu-profiling-workstation/GUIDE.md` : guide interne ; **adapter**
  inventaire et protocoles, dépend des profilers/compteurs réels ; recouvrement
  artist-performance, pas de workstation à installer automatiquement.
- `modules/cuda-kernel-authoring/GUIDE.md` : guide interne ; **référencer** en
  complément simulation, CUDA hors quatre API et hors backend actuel ; nvcc/device
  ne sont pas des préconditions générales de l'équipe.
- `modules/cppstudio-supervisor/GUIDE.md` : guide interne ; **rejeter** comme
  routeur importé ; orchestration/sous-délégation étrangères au contrat des feuilles.
- `modules/agentic-control-harness/GUIDE.md` : guide interne ; **référencer** les
  principes de contrôle observable ; ne pas exécuter scaffolding/scripts ou
  substituer son harness à OpenCode V2.
- `modules/viewport-session-testing/GUIDE.md` : guide interne ; **adapter** les
  critères de session/capture ; exige application pilotable et oracle, absent
  comme preuve de mission ; ne pas confondre capture et qualité/performance.

Effets potentiels : génération de projets, scripts Python, boucles de profilage,
supervision ; tous non exécutés. Couverture : frontières et protocoles existants
à référencer, connaissance Vulkan à créer/spécialiser plutôt que recopier le pack.

### sawickiap/gpu-info-skill

URL : https://github.com/sawickiap/gpu-info-skill ; arbre
`cff2f81c7b7ca35574a4275418dad4449ac8b051` ; updated 2026-02-28 ; MIT signalée,
LICENSE présent. `gpu-info/SKILL.md`, vrai skill (1 461 caractères), références
`gpu-info/references/api_directx12.md` et `api_vulkan.md` trouvées.
**Adapter** les requêtes de capacités dans un contrat commun, pas supposer
l'universalité du GPU local. Dépend des outils d'inspection D3D12/Vulkan et du
driver ; branches spécifiques Windows. Local : vulkaninfo fonctionnel, pas de
rapport complet formats/features ni de preuve D3D12.

## 4B — assets DirectX, pas cœur D3D12

- https://github.com/microsoft/DirectXTex ; arbre
  `4234f77df9c6c8bb2880607b82d5d597b54d1fa9` ; updated 2026-10-16 ; MIT.
  `skills/directxtex-usage/SKILL.md` (10 070 caractères) et reference/overview.md
  existent. Vrai skill, **adapter/référencer** pour artist-asset-cooking : DDS,
  BC, mipmaps, conversions. Dépendance bibliothèque DirectXTex, exemples vcpkg,
  options tools/openexr/dx12 ; ne pas importer ces commandes dans Conan 2.
- https://github.com/microsoft/DirectXMesh ; arbre
  `27e180bbed50de536e4f666fb21eccf1914d88f1` ; updated 2026-10-11 ; MIT.
  `skills/directxmesh-usage/SKILL.md` (4 697 caractères), reference/overview.md
  trouvés. Vrai skill, **adapter/référencer** meshes, normales, adjacency,
  meshlets au cook. Exemples d'ajout de dépendance/vcpkg non exécutés.

Couverture locale : skill assets existant ; ni PSO, root signatures, descriptor
heaps, residency, fences ni diagnostic PIX ne sont remplacés par ces deux skills.
Le spécialiste cœur D3D12 est donc **à créer**, pas à annoncer implémenté.

## 4C — Metal

### apple/game-porting-toolkit

URL : https://github.com/apple/game-porting-toolkit ; catalogue vérifié sous
`game-porting-skills/skills/` ; arbre `4f42344c90be22112f46becc3fb9687f423f2f88` ;
updated 2026-10-16 ; Apache-2.0 signalée. LICENSE racine et licences samples
distinctes présentes. Les vingt noms ci-dessous ont chacun un `SKILL.md`
accessible sous ce préfixe ; vrais skills, pas bibliothèques :

- `translating-to-metal4-api` : **adapter** correspondances API/TBDR/capacités.
- `managing-metal4-synchronization` : **adapter** hazards et portée des barrières.
- `managing-metal4-resources` : **adapter** heaps, vues, residency et upload.
- `creating-metal4-shader-pipelines` : **adapter** compilation/pipeline Metal 4.
- `presenting-metal-drawables` : **adapter** présentation, pacing, HDR/colorspace.
- `managing-metal-cpp-lifetimes` : **adapter** retain/release et contrats C++.
- `integrating-metal-shaderconverter-shaders` : **référencer** binding model et
  particularités des pipelines convertis, ne pas imposer la conversion.
- `compiling-with-metal-shaderconverter` : **référencer** compilateur/converter
  distincts du compilateur Metal, outils/OS/licences propres à vérifier.
- `using-metalfx-temporal-upscaler` : **référencer** option feature-gated.
- `using-metalfx-frame-interpolation` : **référencer** option feature-gated,
  présentation et compatibilité matérielle explicites.
- `setting-up-macos-window` : **référencer** AppKit/fenêtre/événements, pas
  remplacement de la couche plateforme sans décision.
- `using-metal-validation` : **adapter** preuves de validation native.
- `using-gpucapture` : **adapter** capture native, autorisation/bail nécessaires.
- `using-gpudebug` : **adapter** diagnostic natif et données observables.
- `debugging-rendering-issues` : **adapter** progression état/données/image,
  références tier-1/tier-2/tier-3 trouvées.
- `porting-discover` : **référencer** analyse/inventaire, pas routeur ARTIST.
- `porting-execute` : **rejeter** comme workflow automatique importé ; exemples
  d'exécution autonome et commandes système à filtrer, feuille locale obligatoire.
- `porting-validate` : **référencer** critères, pas déclarations de succès Apple.
- `porting-handoff` : **référencer** preuves de transfert, intégré au lead existant.
- `porting-methodology` : **référencer** méthode, sans deuxième orchestration.

`game-porting-skills/agents/porting-assistant.md` : profil/agent, pas skill ;
accessible ; **rejeter** import de superviseur, conserver les références techniques.
Préconditions communes : versions macOS/Xcode/Metal, famille GPU et disponibilité
des features à vérifier individuellement ; Metal 4 n'est pas universel. Effets
potentiels : captures .gputrace, outils Xcode, changement de réglages système dans
certains workflows (`defaults` apparaît), lancement d'apps et fichiers générés.
Rien exécuté. Couverture locale : expertise documentaire Metal à créer ; Windows
ne valide ni compilateur final Metal ni comportement natif.

### Autres compétences Metal

- https://github.com/piemonte/skills ; arbre
  `f26ecec8a55cb84ff464ea82532c9b82a123ee8e` ; updated 2026-08-19 ; MIT.
  `metal-graphics/SKILL.md` vérifié, plugin.json et INSTALL.md présents : vrai
  skill, **fusionner** connaissances pipelines/buffers/pacing avec le spécialiste
  Metal plutôt que dupliquer les contrats Apple. Dépend Metal/macOS ; scripts
  d'installation/plugin non appliqués.
- https://github.com/rudybear/metal-ai-skill ; arbre
  `041d2f4fbd2ba9fd12ddaebd0fa6668b896876fe` ; updated 2026-09-24 ; MIT.
  Chemin réel `.claude/skills/metal-gpu-debug/SKILL.md`, références debugging-recipes
  et xctrace-quick-ref trouvées. Vrai skill, **adapter** capture/profilage via
  `xctrace`, `xcrun metal`, Xcode ; .gputrace produit, outils absents de Windows.
- https://github.com/curiositech/some_claude_skills ; arbre
  `6713fc7a6451c8dee5903b98cd2063ff4cbf7317` ; updated 2026-10-13 ; MIT.
  `.claude/skills/metal-shader-expert/SKILL.md`, reference.md, debug-tools,
  noise-effects et pbr-shaders trouvés. Vrai skill facultatif, **référencer**
  seulement, recouvrement shaders/Metal important ; toute recette numérique doit
  être vérifiée par shader-reviewer, aucun exemple n'est une référence image.

## 4D — shaders et lacune OpenGL

- https://github.com/gamedev-skills/awesome-gamedev-agent-skills ; arbre
  `0a70cfc64672d512c5b5b6006b5dcb5cf90dbdbd` ; updated 2026-10-16 ; Apache-2.0.
  `skills/disciplines/shader-programming/SKILL.md` et
  `skills/disciplines/performance-optimization/SKILL.md` existent tous deux :
  vrais skills. **Fusionner** les connaissances shaders pertinentes ; **référencer**
  performance vers artist-performance plutôt que dupliquer. Dépend du moteur/
  profiler choisi, pas d'une API universelle. Exemples orientés moteurs à filtrer.
- https://github.com/omer-metin/skills-for-antigravity ; arbre
  `e8dcf4e8737921a10088bd5c9eb65e81f74c051f` ; updated 2026-10-15 ; Apache-2.0.
  `skills/shader-programming/SKILL.md` + references patterns/sharp_edges/validations
  trouvés : vrai skill, **fusionner** avec provenance et identifiant ARTIST unique.
  Accès mémoire/divergence/bandwidth sont des hypothèses à mesurer, pas des gains.

Les deux noms `shader-programming` entrent en collision si copiés tels quels.
Leur périmètre n'est pas context ownership, VAO/FBO, stockage, synchronisation ou
debug callbacks : **créer/spécialiser** le skill backend OpenGL à partir des
contrats locaux, sans dupliquer artist-gpu-debug. macOS : OpenGL déprécié et
limites de version à documenter, pas de promesse de parité avec GL Windows.

## 4E — shader commun et capture

- https://github.com/github/awesome-copilot ; arbre
  `cb64ad12e32737ae764becd6885462e9b4c7cb00` ; updated 2026-10-16 ; MIT.
  `skills/slang-shader-engineer/SKILL.md`, language-reference, rules-and-patterns,
  slang-documentation-full trouvés : vrai skill, **adapter prioritairement**
  pour expertise shader partagée (modules, parameter blocks, generics, capabilities).
  Dépend Slang/targets disponibles ; compilation SPIR-V/DXIL/MSL distincte de
  compilation finale et exécution. Local slangc non résolu.
- https://github.com/rudybear/renderdoc-skill ; arbre
  `de33576fa0a4ee739b4b0434d10d710880d604c6` ; updated 2026-10-14 ; MIT.
  `.claude/skills/renderdoc-gpu-debug/SKILL.md` (15 809 caractères), commandes et
  recettes trouvées : vrai skill, **adapter** comme candidat principal de capture.
  Dépend `rdc-cli`/commande `rdc`, API Python RenderDoc, `renderdoc.pyd` et variable
  RENDERDOC_PYTHON_PATH selon environnement. `rdc open` démarre un daemon, capture
  chargée à fermer ; une capture par session. Aucun `rdc doctor` possible ici.
- https://github.com/HKUDS/CLI-Anything ; arbre
  `34f519533bc175d2fe287ab8316b0dd99bb9cc43` ; updated 2026-10-16 ; Apache-2.0.
  `skills/cli-anything-renderdoc/SKILL.md` (6 515 caractères) trouvé : vrai skill,
  **référencer**, alternative surtout analyse hors ligne .rdc. Prescrit
  `pip install cli-anything-renderdoc` et dépend module Python renderdoc ; pas
  exécuté. **Rejeter l'installation redondante** tant que l'avantage face à rdc-cli
  n'est pas démontré. Aucun adaptateur n'est disponible localement dans le PATH.

## 5 — références techniques (pas de faux Agent Skills)

Pour chaque entrée : URL dépôt vérifiée par API GitHub, README lu sauf mention,
date `updated_at` observée et licence signalée ci-dessous. Disposition commune
**référencer**, jamais copier une architecture ou ajouter une dépendance moteur
implicitement. Couverture locale : connaissance à utiliser par le propriétaire
API/ressources/shaders/diagnostic, pas fonctionnalité moteur installée. Les dates
ne sont ni tags de release ni preuves d'actualité de chaque exemple.

### Vulkan

- https://github.com/KhronosGroup/Vulkan-Tutorial — guide, 2026-10-16,
  CC-BY-SA-4.0 ; README.md/.rst/README non trouvés, dépôt accessible ; exemples
  dépendants SDK/GLFW et version ; référence pédagogique, pas norme moderne.
- https://github.com/KhronosGroup/Vulkan-Samples — exemples, 2026-10-15,
  Apache-2.0 ; SDK/device/features, génération/build d'exemples non exécutés.
- https://github.com/KhronosGroup/Vulkan-Guide — guide, 2026-10-15,
  CC-BY-4.0 ; référence de pratiques/features, pas implémentation locale.
- https://github.com/KhronosGroup/Vulkan-Docs — spécification/outils, 2026-10-15,
  NOASSERTION (licences multiples/inconnues pour copie) ; génération documentaire
  non exécutée, référence normative à versions/features explicites.
- https://github.com/KhronosGroup/Vulkan-ValidationLayers — validation/outils,
  2026-10-16, NOASSERTION ; loader/layers compatibles requis, layer Khronos non
  listée localement ; activer seulement dans une campagne autorisée.
- https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator — bibliothèque,
  2026-10-15, MIT ; Vulkan/device/allocations ; étudier suballocation/ownership,
  ne pas ajouter VMA au moteur sans décision.
- https://github.com/KhronosGroup/MoltenVK — couche de portabilité, 2026-10-15,
  Apache-2.0 ; Apple Metal/OS et subset/extensions de portabilité ; pas un backend
  natif Metal et pas exécutable natif Apple validé sous Windows.

### D3D12

- https://github.com/microsoft/DirectX-Graphics-Samples — exemples, 2026-10-13,
  MIT ; Windows SDK/device/features ; référence queues/root signatures/PSO,
  pas preuve de disponibilité de chaque feature locale.
- https://github.com/microsoft/DirectXShaderCompiler — compilateur/bibliothèque,
  2026-10-16, NOASSERTION ; LLVM/DXIL et licences composants ; DXC local 1.8
  observé, ne prouve pas toutes les options du HEAD distant.
- https://github.com/microsoft/DirectX-Headers — headers, 2026-10-15, MIT ; SDK/ABI
  et runtime Windows à distinguer, headers seuls ne fournissent pas le runtime.
- https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator — bibliothèque,
  2026-10-15, MIT ; device D3D12/allocations ; référence residency/suballocation,
  pas remplacement automatique des contrats ARTIST.
- https://github.com/microsoft/DirectXTex — bibliothèque + skill audité ci-dessus,
  2026-10-16, MIT ; traitement offline/textures ; **adapter** au cook seulement.
- https://github.com/microsoft/DirectXMesh — bibliothèque + skill audité ci-dessus,
  2026-10-11, MIT ; géométrie offline ; **adapter** au cook seulement.

### Metal

- https://github.com/apple/game-porting-toolkit — ressources/skills/samples,
  2026-10-16, Apache-2.0 racine et notices samples ; détail section 4C.
  Dépend versions Apple/outils/hardware, pas de support natif Windows déduit.

### OpenGL

- https://github.com/KhronosGroup/OpenGL-Registry — registre/spécifications,
  2026-10-15, licence inconnue API ; vérifier notices des extensions précises.
  Référence normative version/profil/extensions, pas code à recopier.
- https://github.com/KhronosGroup/OpenGL-Refpages — pages de référence,
  2026-10-06, licence inconnue API ; version/profil exacts à vérifier.
- https://github.com/JoeyDeVries/LearnOpenGL — tutoriel/exemples, 2026-10-16,
  NOASSERTION ; bibliothèques/assets et licences distincts, ne pas transformer
  une recette pédagogique en architecture ou oracle de rendu.
- https://github.com/g-truc/ogl-samples — exemples, 2026-10-12, MIT ; contextes
  et extensions variables, valider chaque feature au lieu de copier les patterns.

### Compilation et traduction shaders

- https://github.com/shader-slang/slang — compilateur/langage, 2026-10-16,
  NOASSERTION ; targets et bibliothèques auxiliaires, `slangc` absent PATH.
- https://github.com/shader-slang/slang-rhi — bibliothèque RHI, 2026-10-16,
  NOASSERTION ; devices/backends/Slang ; référence device/lifetime/submission,
  pas architecture à recopier ni backend ARTIST existant.
- https://github.com/KhronosGroup/glslang — compilateur GLSL/HLSL partiel,
  2026-10-16, NOASSERTION ; dépendances SPIR-V éventuelles ;
  `glslangValidator` absent PATH, succès intermédiaire ≠ rendu correct.
- https://github.com/KhronosGroup/SPIRV-Tools — outils IR, 2026-10-16,
  Apache-2.0 ; versions SPIR-V/environment ; `spirv-val/opt/dis` absents PATH.
- https://github.com/KhronosGroup/SPIRV-Cross — traduction/réflexion, 2026-10-16,
  Apache-2.0 ; MSL/GLSL/HLSL générés exigent compilation finale/API layout ;
  `spirv-cross` absent PATH.
- https://github.com/microsoft/DirectXShaderCompiler — même source DXC ci-dessus,
  référence répétée dans la prescription, pas un deuxième outil à installer.

### Abstractions et traduction

- https://github.com/shader-slang/slang-rhi — même entrée ci-dessus ; observer
  compromis dynamic abstraction/capabilities, pas obligation d'adoption.
- https://github.com/bkaradzic/bgfx — bibliothèque RHI, 2026-10-16,
  BSD-2-Clause ; outils shader/build/backends et modèle submission propres ;
  comparaison mécanismes/coûts, pas prétention zero-cost.
- https://github.com/google/filament — moteur/bibliothèques, 2026-10-16,
  Apache-2.0 ; material compiler/assets/backends ; référence render graph/PBR,
  pas extraction automatique d'un système entier.
- https://github.com/doitsujin/dxvk — traduction D3D8/9/10/11 vers Vulkan,
  2026-10-16, Zlib ; Wine/Vulkan selon usage ; complément facultatif,
  pas D3D12 natif et pas backend moteur.
- https://github.com/HansKristian-Work/vkd3d-proton — dépôt officiel actuel vérifié
  API : fork VKD3D, branches Proton D3D12 ; 2026-10-14, LGPL-2.1 signalée ;
  Wine/Vulkan/dépendances et obligations de copie distinctes ; complément
  facultatif **référencer** seulement, ne pas importer sous une licence permissive
  supposée. La prescription ne fournissait pas l'URL : elle est désormais résolue.

### Debugging et profiling

- https://github.com/baldurk/renderdoc — outil/API Python, 2026-10-16,
  MIT ; hooks/capture/replay et GPU/driver compatibles, produit .rdc ; ni outil
  ni bindings résolus localement aux emplacements sondés. Pas support Metal déduit.
- https://github.com/GPUOpen-Tools/radeon_gpu_profiler — profiler, 2026-10-06,
  licence inconnue API ; matériel/driver AMD et traces requises ; GPU local NVIDIA,
  pas de campagne AMD ni performance extrapolée.
- https://github.com/GPUOpen-Tools/radeon_gpu_analyzer — analyseur/compilateur
  offline, 2026-10-09, MIT ; targets ISA/compilateurs requis ; absent PATH,
  analyse offline ne mesure pas le GPU cible.
- https://github.com/apitrace/apitrace — outil de traces, 2026-10-15,
  MIT signalée ; capture/replay API, injection/fichiers potentiels ; absent PATH,
  ne pas activer concurrentiellement sur le même processus sans bail.

## Documents officiels et outils sans URL de dépôt imposée

Date/révision commune : HTTP observé 2026-10-16, version précise non épinglée ;
licence inconnue (pages web, pas attribution de licence logicielle inventée).
Disposition **référencer**, pas copier ni exécuter leurs instructions système.

- https://agentskills.io/ → /home, HTTP 200 : spécification/format, pas skill GPU.
  Dépend compatibilité réelle du runtime ; local SKILL.md statiques, pas catalogue
  runtime éprouvé.
- https://docs.vulkan.org/ → HTTP 403 : **blocked**, référencer dépôt Vulkan-Docs
  accessible comme alternative de lecture, pas contourner l'accès.
- https://learn.microsoft.com/windows/win32/direct3d12/direct3d-12-graphics
  → /en-us/windows/win32/direct3d12/direct3d-12-graphics, HTTP 200 : documentation
  D3D12, dépend versions OS/SDK/device ; aucune création de device locale ici.
- https://devblogs.microsoft.com/pix/ → HTTP 200 : documentation profiler PIX,
  Windows/device/installation à vérifier ; pas Agent Skill. PIX non résolu localement.
- https://developer.apple.com/metal/ → HTTP 200 : documentation API/features,
  familles GPU/OS explicites, expertise ne vaut pas backend moteur.
- https://developer.apple.com/metal/cpp/ → HTTP 200 : wrapper/guide metal-cpp,
  lifetimes et framework Apple, pas port natif de Metal Windows.
- https://developer.apple.com/metal/sample-code/ → HTTP 200 : catalogue exemples,
  chaque sample peut avoir sa licence/version Xcode/OS ; pas téléchargement/build.
- https://developer.nvidia.com/nsight-graphics → HTTP 200 : outil graphics,
  drivers/targets/licences propres ; CLI `nsight` non résolue ne prouve pas que
  toute installation Nsight Graphics est absente.
- https://developer.nvidia.com/nsight-compute → HTTP 200 : profiler kernels
  CUDA, pas substitut à Nsight Graphics ; ncu 2026.1.0 local répond à --version,
  aucune mesure réalisée.
- https://developer.apple.com/xcode/ → HTTP 200 : outils de compilation,
  capture GPU et environnement Apple ; pas disponibles nativement dans ce Windows.
- https://developer.apple.com/instruments/ → HTTP 403 : **blocked** au fetch,
  ne pas en déduire absence d'Instruments sur un hôte Apple ; référence Xcode
  accessible, validation native future requise.
- https://opencode.ai/v2/docs/api → HTTP 403 : **blocked**, hors corpus GPU,
  pertinent pour découverte du catalogue ; aucune requête anonyme à un port deviné.

## Décisions à transmettre à T03

1. Ne pas recopier les six skills communs ; distribuer les 19 responsabilités
   en références croisées, expertise shader commune prioritaire Slang, quatre
   expertises API sans prétention d'implémentation moteur.
2. Conserver orchestration/QA dans le coordinateur actuel ; rejeter les routeurs
   externes importés et les workflows autonomes incompatibles avec les feuilles.
3. Séparer connaissance documentaire et capacité exécutable. DXC/Vulkaninfo/ncu
   observés n'autorisent ni adoption Slang, ni installation SDK, ni score performance.
4. Choisir un adaptateur capture principal après préflight autorisé ; rdc-cli
   candidat pour capture/inspection, CLI-Anything alternative offline. Aucun des
   deux n'est ici validé ; macOS exige la voie Apple, pas RenderDoc supposé universel.
5. Les licences NOASSERTION/inconnues et bibliothèques tierces bloquent la copie,
   pas les liens bibliographiques. Les auteurs doivent vérifier les fichiers
   précis et épingler commits avant réutilisation textuelle/code.
6. Découverte runtime des profils, enforcement permissions et douze scénarios de
   routage demeurent INCONCLUSIVE. Pas de CLI OpenCode PATH, pas d'API agents
   exposée dans le catalogue consulté ; décision/bail d'accès natif requis du lead.

Résultat : PASS inventaire/provenance bornés ; INCONCLUSIVE compilation, tests,
captures, images, performance et enforcement (hors périmètre ou préconditions
manquantes). Aucun défaut moteur reproduit et aucune correction moteur proposée.
