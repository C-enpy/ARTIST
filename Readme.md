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
