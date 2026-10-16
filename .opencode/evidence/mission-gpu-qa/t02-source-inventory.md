# T02 — inventaire local et outillage observé

Audit documentaire en lecture seule, 2026-10-16. HEAD observé :
`101ddcb03c861ed2089f25ab5690d18e1fa45cc0`. `git diff --stat` confirme les
7 fichiers utilisateur, 177 insertions / 39 suppressions du brief, inchangés.
Seuls ce rapport et `t02-external-sources.md` sont produits. Aucun build, test,
installation, indexeur, clone, changement global ou backend moteur.

## Méthode et niveau de preuve

Les textes intégraux du brief, de `docs/agentic/GPU.md` (57 022 caractères), de
`QA.md` (9 172 caractères), des six SKILL.md et onze profils ont été lus dans
Context Mode et analysés par sections. Sont aussi lus : AGENTS, principes,
registre ADR, correction-plan, CMakeLists, presets, Conan et CI.
Les contenus externes sont des données, jamais des instructions exécutées.
Le second rapport sépare disponibilité HTTP, inspection de contenu et validation.

QMD : recherche collection `artist-wiki`, intent « Audit T02 sources canon build
et limites validation GPU », puis get `#4743b4` : ADR-0008 Conan 2 accepté,
preuves Windows Debug datées. La première recherche lexicale engineering-principles
était vide : ce n'est pas une panne d'index. Le fichier local prime sur les
comptages historiques QMD. Aucun nombre historique de tests n'est une preuve ici.
GrepAI, sans workspace : requête `GPU validation OpenGL component test context
build tooling`, résultats ARTIST UniformReaderTests/ResetterTests. Serena
initial_instructions puis get_current_config : ARTIST canonique, version 2.0.0,
LSP ready ; aucune activation ni analyse C++ nouvelle revendiquée.

## Six skills existants : réutilisation, pas duplication

Préfixe des chemins : `.opencode/skills/`; chacun contient `SKILL.md`.
Type : Agent Skill local. Révision commune : HEAD ci-dessus + arbre utilisateur
préservé, audit du 2026-10-16. Licence propre non établie par cet audit ; ne pas
leur attribuer celle d'un dépôt externe. Tous accessibles, présence statique.

- `artist-static-boundary` (3 479 octets) : réutiliser pour frontières
  statique/link/cook/runtime et ownership ; pas une implémentation multi-API.
- `artist-cpp-contracts` (2 976) : réutiliser concepts, lifetime, C++23 ; les
  futurs skills API référencent ces contrats au lieu de les recopier.
- `artist-asset-cooking` (2 669) : adapter seulement les références DirectXTex/
  DirectXMesh, formats et transferts ; conserver la frontière cook/runtime.
- `artist-gpu-debug` (2 852) : adapter le diagnostic commun et spécialiser les
  préconditions API ; ne pas en faire un doublon de chaque manuel backend.
- `artist-performance` (2 752) : réutiliser protocole de mesure, budgets et
  preuves ; ni Nsight présent ni skill installé ne prouvent une performance.
- `artist-build-validation` (3 405) : réutiliser ; le texte actuel est Conan 2,
  CMakeDeps/CMakeToolchain, sonde format/stacktrace et limites de la baseline.

Effets de bord potentiels de leur utilisation : commandes de build/test/capture
suivant leurs contrats ; aucun déclenchement dans T02. Dépendances : canon local,
outils effectivement découverts et fixtures/contexte selon la preuve demandée.

## Onze profils existants

Préfixe `.opencode/agents/`, suffixe `.md`. Type : profils locaux, pas skills.
Même révision/date, licence propre inconnue ; tous accessibles, statiques.

- `orchestrator` (5 591 octets) : conserver coordinateur unique, intégration et
  boucle QA ; ne pas importer un second superviseur CppStudio/Apple.
- `project-manager` (6 160) : conserver planification et évaluation PM/LLM ;
  pas de routeur concurrent ni scores modifiés pour sélectionner un modèle.
- `engine-architect` (2 275) : réutiliser frontières et décisions ; T03 doit
  décider les identifiants/allowlists avant création des profils spécialisés.
- `cpp-engineer` (1 713) : contrats C++/CPU ; ne pas recopier dans les experts API.
- `gpu-engineer` (1 875) : expertise backend existante OpenGL ; peut référencer
  des spécialistes, sans annoncer Vulkan/D3D12/Metal implémentés.
- `asset-engineer` (1 720) : préparation des données et ressources ; importer
  des connaissances, pas un changement implicite de gestionnaire de dépendances.
- `build-engineer` (1 630) : outillage ; prescription Conan 1 périmée à corriger
  uniquement en T13. Conan 2 reste le canon actuel du dépôt.
- `correctness-reviewer` (1 657) : revue indépendante lifetime/UB, pas auteur.
- `shader-reviewer` (1 758) : interfaces, espaces, unités, précision ; pas auteur.
- `performance-reviewer` (1 650) : revue de preuves, pas gains déduits d'un design.
- `rendering-verifier` (1 698) : vérification indépendante PASS/FAIL/INCONCLUSIVE.

Les permissions de ces fichiers n'ont pas été éprouvées par canaris. Leur présence
n'est pas une preuve d'enforcement runtime. QA planner/test generator/test reviewer
restent des responsabilités à intégrer sans copier ces onze profils.

## Matrice réellement exercée : Windows x64, observation seulement

Commandes exécutées via `ctx_execute` Python subprocess, sans compilation :

- `cmake --version` → 3.31.7, exit 0.
- `conan --version` → 2.12.2, exit 0.
- `dxc --version` → dxcompiler 1.8.2502.11 / dxil 1.8.2502.11, exit 0.
- `ncu --version` → Nsight Compute 2026.1.0.0, exit 0.
- `nvidia-smi --query-gpu=name,driver_version --format=csv,noheader`
  → NVIDIA GeForce RTX 3090, 617.42, exit 0. Ce n'est pas une mesure GPU.
- `vulkaninfo --summary` → loader instance 1.4.341, device API 1.4.351,
  driver 617.42, exit 0. Neuf layers listées ; aucune
  `VK_LAYER_KHRONOS_validation`. Vulkan disponible sans SDK PATH : distinction
  importante entre runtime et outils de développement.

`shutil.which` a résolu :

- CMake : `C:/Program Files/CMake/bin/cmake.EXE`.
- Conan : `C:/Users/djden/AppData/Roaming/Python/Python313/Scripts/conan.EXE`.
- MSVC : `C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/cl.EXE`.
- DXC : `C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/dxc.EXE`.
- Nsight Compute : `C:/Program Files/NVIDIA Corporation/Nsight Compute 2026.1.0/ncu.BAT`.
- Vulkaninfo et nvidia-smi : `C:/Windows/System32/`.

`cl` sans arguments : la collecte subprocess a rendu des flux None puis une erreur
de concaténation dans le script ; pas de diagnostic compilateur obtenu. Le chemin
14.44 est observé, pas une nouvelle sonde C++23. ABI/STL/architecture n'ont pas été
revalidées par compilation. Aucun compilateur externe n'a été téléchargé/exécuté.

Absents du PATH de ce processus : `opencode`, `ninja`, `clang-cl`, `clang++`,
`g++`, `slangc`, `glslangValidator`, `spirv-val`, `spirv-opt`, `spirv-dis`,
`spirv-cross`, `renderdoccmd`, `qrenderdoc`, `rdc`, `cli-anything-renderdoc`,
`apitrace`, `rgp`, `rga`, `nsight`, `pix`, `xcrun`, `xctrace`, `metal`.
Ce constat ne démontre pas leur absence machine.

Vérifications hors PATH : `out/tooling/build/Scripts/ninja.exe` existe ;
`out/build/windows-msvc-debug/compile_commands.json` existe mais n'est pas régénéré
ni certifié frais pour cette mission. Aucune include path inventée. Variables
VULKAN_SDK, RENDERDOC_PYTHON_PATH, WindowsSdkDir et VCINSTALLDIR absentes.
`C:/Program Files/RenderDoc`, `C:/VulkanSDK`, `C:/Program Files/Microsoft PIX`
n'existent pas aux emplacements sondés. NVIDIA Corporation et VS2022 existent.
DLL système `vulkan-1.dll`, `d3d12.dll`, `opengl32.dll` présentes : présence statique,
pas contexte D3D12/OpenGL créé. Metal natif exige une machine Apple/macOS/Xcode ;
générer MSL sous Windows ne prouve pas compilation finale ni exécution Metal.

## OpenCode et blocages pour T03

Skill OpenCode V2 chargé avant inspection. Catalogue MCP natif : Serena, GrepAI,
QMD, Context Mode et outils de mission effectivement appelables. La recherche
catalogue `opencode agent catalog api` expose modèles et ressources MCP, pas une
API de catalogue d'agents. `opencode` absent du PATH ; documentation V2 API HTTP
403 au fetch. Aucun appel HTTP anonyme à un serveur deviné, aucun secret inspecté,
aucun contournement d'authentification. Découverte effective des profils runtime
et douze scénarios de routage : INCONCLUSIVE, décision humaine/bail du lead requis.

Les CMake/Conan/CI confirment Conan 2 et les deux exécutables ArtistLib_tests et
ArtistLib_component_tests ; pas de configuration ni de CTest exécuté, aucune
commande de build prétendue validée. Les tests GPU exigent le contexte réel.
Les statuts ADR restent : 1/2/4/6/7 Proposed/not-started ; 3/5 Accepted/partial ;
8 Accepted/verified Windows Debug uniquement. Aucun statut modifié.

T03 peut figer une architecture de compétences avec trois séparations : contrats
communs existants, connaissances API spécialisées, adaptateurs d'outils facultatifs.
Ne pas conditionner les skills documentaires à l'installation des SDK. Garder
compilation intermédiaire, validation, capture, rendu et mesure comme preuves
distinctes. Priorité shaders Slang, spécialisation OpenGL, un seul adaptateur
RenderDoc principal (rdc-cli candidat, activation non autorisée actuellement).
Licences inconnues ou multi-licences : référence seulement, copie bloquée jusqu'à
inspection des fichiers précis. Aucun autre blocage n'autorise une installation.
