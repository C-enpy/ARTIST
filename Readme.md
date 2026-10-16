![Build, Test](https://github.com/C-enpy/ARTIST/actions/workflows/cmake-single-platform.yml/badge.svg) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=C-enpy_ARTIST&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=C-enpy_ARTIST)

# A.R.T.I.S.T.

**Abstract Rendering Toolkit & Interface for Shader Technology** est actuellement
une bibliothèque de rendu **C++23 header-only**, avec un backend **OpenGL** et des
tests GoogleTest/GoogleMock. Le projet vise un moteur 3D bas niveau techniquement
exigeant.

## Ce qui existe et ce qui est visé

Les paramètres `API`/`PROFILE`, concepts et `if constexpr` sélectionnent et
valident des composants à la compilation. L'exécution actuelle reste hybride :
interfaces virtuelles, contexts partagés, collections dynamiques, lookups par
chaînes et valeurs effacées coexistent avec cette sélection statique.

La présence de templates ne démontre donc ni un pipeline entièrement aplati,
ni l'absence d'overhead. Vulkan, D3D12, Metal, un cooker, le streaming, la physique
et un moteur de scène complet ne sont pas des fonctionnalités livrées ici.

Le cap : **spécialiser les comportements, préparer les données stables, conserver
l'état vivant en données compactes et amortir le dispatch par lots**. Les
performances doivent être établies par des mesures à qualité équivalente.

## Documentation Obsidian

Ouvrir **`docs/` comme vault** et commencer par [l'accueil](docs/index.md).

- [État réel et preuves source](docs/architecture/current-state.md)
- [Principes techniques](docs/engineering-principles.md)
- [Premiers ADR](docs/adr/README.md) et [ordre de correction](docs/architecture/correction-plan.md)
- [Build et tests](docs/development/build-and-test.md)
- [Conventions C++](docs/development/cpp-guidelines.md) et [contribution](docs/development/contributing.md)
- [Harness OpenCode](docs/opencode-harness.md)

La collection QMD est **`artist-wiki`**, limitée à `docs/**/*.md`. Son identité
portable est dans [`.wiki.json`](.wiki.json) et son usage dans
[`AGENTS.md`](AGENTS.md). Les quatre rapports sources sont conservés dans
`research/`, avec leur provenance documentée dans le vault.

## Compilation

Baseline vérifiée sous **Windows x64, MSVC 19.44, C++23, CMake 3.31.7,
Ninja 1.13.0 et Conan 2.12.2** : build complet et **109/109 tests réussis**,
dont 22 tests sur un contexte OpenGL NVIDIA réel.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
```

Le script utilise MSVC 14.44 et le SDK Windows installés, prépare Conan/Ninja
dans `out/tooling/build`, puis installe les dépendances, configure, compile et
exécute CTest. Il restaure les variables d'environnement à la sortie.
Voir [le guide de build](docs/development/build-and-test.md) pour les prérequis,
les versions, les commandes détaillées et les limites de validation.

## Remerciements et sources du harness

Notre harness s'appuie sur les outils et les travaux ci-dessous. Merci à leurs
auteurs, aux organisations qui les maintiennent et à l'ensemble de leurs
contributeurs. Les noms de comptes sont conservés lorsqu'aucun nom public n'est
indiqué.

### Outillage du harness

- [OpenCode](https://github.com/anomalyco/opencode) — [Anomaly](https://github.com/anomalyco) : runtime des agents, outils et permissions.
- [OpenCode Ensemble](https://github.com/hueyexe/opencode-ensemble) — [hueyexe](https://github.com/hueyexe) : coordination des agents, tâches partagées et worktrees.
- [GrepAI](https://github.com/yoanbernabeu/grepai) — [Yoan Bernabeu](https://github.com/yoanbernabeu) : recherche sémantique et graphes d'appels.
- [Serena](https://github.com/oraios/serena) — [Oraios AI](https://github.com/oraios) : navigation et manipulation symboliques du code.
- [QMD](https://github.com/tobi/qmd) — [Tobias Lütke](https://github.com/tobi) : recherche dans la documentation et le wiki.
- [Context Mode](https://github.com/mksglu/context-mode) — [Mert Köseoğlu](https://github.com/mksglu) : traitement des sorties volumineuses et recherche dans celles-ci.
- [1MCP](https://github.com/1mcp-app/agent) — [1mcp-app](https://github.com/1mcp-app) : agrégation et accès aux serveurs MCP.

### Corpus de compétences étudié pour les profils GPU

Ces dépôts ont servi à l'étude des compétences, de leurs frontières et des
procédures de diagnostic. Ils ne sont pas importés en bloc : le harness conserve
son coordinateur et ses contrats ARTIST.

- [CppStudio](https://github.com/tarkansarim/CppStudio) — [tarkansarim](https://github.com/tarkansarim) : modules C++/Vulkan, synchronisation et contrôle observable.
- [gpu-info-skill](https://github.com/sawickiap/gpu-info-skill) — [Adam Sawicki](https://github.com/sawickiap) : interrogation des capacités GPU.
- [DirectXTex](https://github.com/microsoft/DirectXTex) et [DirectXMesh](https://github.com/microsoft/DirectXMesh) — [Microsoft](https://github.com/microsoft) : compétences et références sur les textures et la géométrie préparées au cook.
- [Game Porting Toolkit](https://github.com/apple/game-porting-toolkit) — [Apple](https://github.com/apple) : ressources, synchronisation, pipelines et lifetime Metal/metal-cpp.
- [skills / metal-graphics](https://github.com/piemonte/skills) — [Patrick Piemonte](https://github.com/piemonte) : pipelines et ressources Metal.
- [metal-ai-skill](https://github.com/rudybear/metal-ai-skill) et [renderdoc-skill](https://github.com/rudybear/renderdoc-skill) — [Alexey Medvedev](https://github.com/rudybear) : recettes de diagnostic Metal et RenderDoc.
- [some_claude_skills](https://github.com/curiositech/some_claude_skills) — [Curiositech](https://github.com/curiositech) : références de compétences Metal/shaders.
- [awesome-gamedev-agent-skills](https://github.com/gamedev-skills/awesome-gamedev-agent-skills) — [gamedev-skills](https://github.com/gamedev-skills) : périmètres shaders et performance.
- [skills-for-antigravity](https://github.com/omer-metin/skills-for-antigravity) — [Ömer Metin](https://github.com/omer-metin) : pratiques et pièges de programmation shader.
- [awesome-copilot](https://github.com/github/awesome-copilot) — [GitHub](https://github.com/github) : compétence Slang et références de compilation shader.
- [CLI-Anything](https://github.com/HKUDS/CLI-Anything) — [Data Intelligence Lab, Université de Hong Kong (HKUDS)](https://github.com/HKUDS) : alternative étudiée pour l'analyse de captures RenderDoc, non installée par cette mission.

### Références techniques des procédures GPU

- [Khronos Group](https://github.com/KhronosGroup) :
  [Vulkan-Docs](https://github.com/KhronosGroup/Vulkan-Docs),
  [Vulkan-Guide](https://github.com/KhronosGroup/Vulkan-Guide),
  [Vulkan-Tutorial](https://github.com/KhronosGroup/Vulkan-Tutorial),
  [Vulkan-Samples](https://github.com/KhronosGroup/Vulkan-Samples),
  [Vulkan-ValidationLayers](https://github.com/KhronosGroup/Vulkan-ValidationLayers),
  [MoltenVK](https://github.com/KhronosGroup/MoltenVK),
  [OpenGL-Registry](https://github.com/KhronosGroup/OpenGL-Registry),
  [OpenGL-Refpages](https://github.com/KhronosGroup/OpenGL-Refpages),
  [glslang](https://github.com/KhronosGroup/glslang),
  [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools) et
  [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross).
- [Microsoft](https://github.com/microsoft) :
  [DirectX-Graphics-Samples](https://github.com/microsoft/DirectX-Graphics-Samples),
  [DirectXShaderCompiler](https://github.com/microsoft/DirectXShaderCompiler) et
  [DirectX-Headers](https://github.com/microsoft/DirectX-Headers).
- [AMD GPUOpen](https://github.com/GPUOpen-LibrariesAndSDKs) :
  [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator),
  [D3D12MemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator),
  [Radeon GPU Profiler](https://github.com/GPUOpen-Tools/radeon_gpu_profiler) et
  [Radeon GPU Analyzer](https://github.com/GPUOpen-Tools/radeon_gpu_analyzer).
- [Slang](https://github.com/shader-slang/slang) et [slang-rhi](https://github.com/shader-slang/slang-rhi) — [projet Slang](https://github.com/shader-slang).
- [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL) — [JoeyDeVries](https://github.com/JoeyDeVries).
- [ogl-samples](https://github.com/g-truc/ogl-samples) — [g-truc](https://github.com/g-truc).
- [bgfx](https://github.com/bkaradzic/bgfx) — [bkaradzic](https://github.com/bkaradzic).
- [Filament](https://github.com/google/filament) et [Benchmark](https://github.com/google/benchmark) — [Google](https://github.com/google).
- [DXVK](https://github.com/doitsujin/dxvk) — [doitsujin](https://github.com/doitsujin).
- [vkd3d-proton](https://github.com/HansKristian-Work/vkd3d-proton) — [HansKristian-Work](https://github.com/HansKristian-Work).
- [RenderDoc](https://github.com/baldurk/renderdoc) — [Baldur Karlsson](https://github.com/baldurk).
- [apitrace](https://github.com/apitrace/apitrace) — [projet apitrace](https://github.com/apitrace).
- [ASTC Encoder](https://github.com/ARM-software/astc-encoder) — [Arm](https://github.com/ARM-software).
- [meshoptimizer](https://github.com/zeux/meshoptimizer) — [Arseny Kapoulkine](https://github.com/zeux).
- [MikkTSpace](https://github.com/mmikk/MikkTSpace) — [mmikk](https://github.com/mmikk).
- [PositionBasedDynamics](https://github.com/InteractiveComputerGraphics/PositionBasedDynamics) — [Interactive Computer Graphics](https://github.com/InteractiveComputerGraphics).

Consultation, inspiration et utilisation d'outils ne signifient ni copie de code,
ni ajout de dépendance au moteur, ni validation GPU exécutée. Chaque projet garde
sa licence et ses notices ; ces remerciements ne les remplacent pas. Le détail
du corpus audité et de ses dispositions est conservé dans
[l'inventaire des sources](.opencode/evidence/mission-gpu-qa/t02-external-sources.md).
