---
name: artist-build-validation
description: Configure, compile et vérifie ARTIST avec CMake/Conan et les toolchains réellement présentes. À utiliser avant un build, CTest, tests de concepts, sanitizers ou validation GPU.
---

# Build et preuves graduées

## Baseline et préflight

Lire `docs/development/build-and-test.md`, ADR-0005/0008, les CMake, presets,
`conanfile.txt` et la CI. Examiner le diff avant de modifier. Exécuter commandes
et analyse de logs via Context Mode ; conserver les logs sous `out/`.

Le flux actuel est **Conan 2**, avec installation avant configuration CMake :
`CMakeDeps` / `CMakeToolchain`. Les versions directes historiques sont conservées.
Ne pas rétablir `conan_cmake_run`, Conan 1 ou ses générateurs.

Baseline exécutée : Windows x64 Debug, MSVC 19.44 / toolset 14.44, SDK 10.0.26100,
CMake 3.31.7, Conan 2.12.2, Ninja 1.13.0. Procédure reproductible :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
```

Le script isole tooling/cache sous `out/`, restaure l'environnement et utilise
un seul job par défaut. Vérifier la mémoire engagée Windows en cas de C1060 ;
la RAM physique libre seule ne suffit pas. Aucun arrêt d'application étrangère
ni changement système de mémoire sans périmètre approprié.

`cmake/CheckCxx23.cmake` teste compilation et linkage de `std::format` et
`std::stacktrace`, avec détection des bibliothèques libstdc++ éventuelles. Le GCC
MSYS2 13.2 présent n'expose pas stacktrace. Ne pas contourner la sonde pour obtenir
un build artificiellement vert. Une autre toolchain doit établir ses propres preuves.

## Exécution et portée

CTest découvre les tests depuis la racine de build, avec préfixes/labels distincts :

```powershell
ctest --test-dir out/build/windows-msvc-debug -N
ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error
ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error
```

Le 2026-10-03 : **87 tests unitaires + 22 OpenGL**, sans skip. Le contexte observé
était NVIDIA RTX 3090 / OpenGL 3.3.0 / driver 616.92. Ce résultat est une baseline
datée, pas une garantie pour le diff suivant. La fixture GPU échoue explicitement
sur init manquante ; elle ne doit pas être remplacée par un mock pour conclure PASS.

Les fixtures sont lues depuis la racine source, configurée dans CTest.
`MOCK_APPDATA` était inutilisé et a été retiré. Coverage est optionnel, réservé
au flux GCC/gcov ; son module téléchargé inconditionnel a été supprimé.

Choisir la preuve selon le changement : types positifs/négatifs, CPU, sanitizers,
API GPU, images ou performance. Ne pas tout exécuter par défaut. Ni zéro test,
ni un build mocké ne prouvent le rendu. Sérialiser les tests partageant un contexte.

Une base `compile_commands.json` est générée dans le build réel. La présence de
ce fichier ne garantit pas que clangd/Serena connaît le SDK et l'environnement
MSVC : qualifier les limites de l'analyse symbolique.

## Retour

Rapporter commandes, versions, périmètre, nombre de tests et skips, et première
erreur utile. **PASS** pour une preuve obtenue, **FAIL** pour un défaut reproduit,
**INCONCLUSIVE** pour une précondition manquante. La baseline Windows ne valide
pas automatiquement Release, Linux/WSL, macOS, CI distante, coverage ou performances.
