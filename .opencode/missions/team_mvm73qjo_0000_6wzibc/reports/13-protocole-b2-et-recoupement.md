# 13 — Protocole B2 ASan livré et recoupé, campagne non exécutée

Date : 2026-10-24. Auteur : `asan-sol-reprise`, build-engineer, OpenAI GPT-6.1 Sol high. Persistance/recoupement : lead. Analyse documentaire seulement.

## Provenance et contrôles réellement effectués

Résultat auteur `completed`, sans édition/build/test/installation. Le lead a recoupé par dérivations de fichiers dans Context Mode :

- `out/mission-a1a2/final-asan-selection.json`, pin historique `f914d77b17b1c3013bf06679c0f94537ee7ba24a` ;
- `final-verification/normal-discovery.json` (UTF16LE/BOM) : 39 noms GL historiques ; 22 présents dans la sélection, différence exacte **17** ;
- SHA256 de `normal-discovery.json`, `discovery-check.json`, `declared-asan-selection.json` recalculés et concordants avec `final-verification/outputs-sha256.json` ;
- copie `declared-asan-selection.json` identique octet pour octet à `final-asan-selection.json` ;
- chaque nom exclu correspond exactement à **un TEST_F** dans les deux fichiers source courants ci-dessous.

Ces contrôles établissent la provenance documentaire et la présence source, **pas** une découverte CTest fraîche, l'équivalence des corps historiques/courants ou un résultat H3. Zéro test du candidat exécuté. La liste documentée des 17 n'est pas la liste finale complète de la campagne.

## Les 17 cas à retrouver dans la découverte H3

`ARTIST/component-tests/src/graphic/shader/PassTests.cpp` :

```text
opengl.PassTest.PassCreationTest
opengl.PassTest.UniformManagementTest
opengl.PassTest.InvalidUniformTest
opengl.PassTest.ShaderAttachmentTest
opengl.PassTest.UniformReadingTest
opengl.PassTest.AttributeReadingTest
opengl.PassTest.FreeResourcesTest
```

`ARTIST/component-tests/src/graphic/shader/ShaderTests.cpp` :

```text
opengl.ShaderTest.CreateShaderVert
opengl.ShaderTest.CreateShaderBadVert
opengl.ShaderTest.CreateShaderFrag
opengl.ShaderTest.CreateShaderBadFrag
opengl.ShaderTest.CreateShaderGeom
opengl.ShaderTest.CreateShaderBadGeom
opengl.ShaderTest.CreateShaderTesselaionControl
opengl.ShaderTest.CreateShaderBadTesselaionControl
opengl.ShaderTest.CreateShaderCompute
opengl.ShaderTest.CreateShaderBadCompute
```

Préserver la graphie `Tesselaion`. Les noms de tests ne prouvent pas une capacité compute native du backend 3.3.

## Préconditions et ressources, avant campagne

Runner : lead distinct, depuis la racine ARTIST. Figer H3, delta admis et empreintes sources/config/tests. H_Q et H_A commis avant les auteurs ; H_A354b53f acquis, H_Q toujours attendu. Arbres normal et ASan distincts ; bail exclusif GPU/fixtures entre processus, un build par arbre, un job explicite. RESOURCE_LOCK CTest ne coordonne pas plusieurs processus indépendants.

Versions/outillage installés à contrôler, pas supposés par les notes : CMake/Conan2/Ninja, MSVC/toolset/SDK, CheckCxx23 compile/link format+stacktrace, DLL ASan, chemins des runners. Scripts inspectés par l'auteur : build-windows.ps1, asan-env.ps1, presets, cmake ASan, out/tooling/run-build.py et msvc-env.cmd existants. Sonde/non-GPU saine puis faute intentionnelle requises avant verdict ASan. Aucun contournement C++23, /RTC1 ou CRT/assertion.

**Attention installation :** build-windows.ps1 appelle pip install et Conan --build=missing. Pas d'installation dans cette analyse. Avant tout appel ultérieur, établir que les opérations nécessaires sont dans le mandat ; composant absent ou nouvelle dépendance non autorisée = blocage/décision, pas bootstrap implicite. Le runner local out/tooling est non versionné et son chemin Visual Studio doit être revérifié ; ne pas le recréer par hypothèse.

## Commandes proposées, non exécutées

Après chaque commande native, conserver exitcode et première erreur causale ; arrêt si non-zéro. Ces exemples dépendent du setup Windows réel validé et de H3. Le lead possédera `out/qa-b2-h3` et figera la sélection avant tout run de campagne.

```powershell
powershell -NoProfile -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel all
powershell -NoProfile -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer -AsanPreflightOnly

$py = Join-Path $PWD 'out/tooling/build/Scripts/python.exe'
. ./scripts/asan-env.ps1
Set-AsanTestEnvironment
& $py out/tooling/run-build.py b2-h3-asan-clean cmake --build --preset windows-msvc-debug-asan --clean-first --target ArtistLib_tests ArtistLib_component_tests artist_asan_probe --parallel 1
& $py out/tooling/run-build.py b2-h3-asan-canary $py scripts/verify_asan.py out/build/windows-msvc-debug-asan/artist_asan_probe.exe --log-dir out/qa-b2-h3/asan-canary
& $py out/tooling/run-build.py b2-h3-asan-gl-discovery ctest --test-dir out/build/windows-msvc-debug-asan -N -L opengl
& $py out/tooling/run-build.py b2-h3-asan-discovery ctest --preset windows-msvc-debug-asan --show-only=json-v1
```

Le préflight wrapper configure --fresh mais ne fait pas clean-first ; il ne remplace pas la reconstruction propre ni le canari reconstruit après clean. Le runner local fixe un parallélisme de 2 par défaut ; `--parallel 1` explicite doit s'appliquer aux commandes de campagne.

Sélection proposée : tous les noms unit+opengl fraîchement découverts sur H3, aucune exclusion, assertion 17/17 chacun exactement une fois. **À arrêter et persister avant exécution**, pas à générer silencieusement pendant le run. Si une autre sélection est retenue, justifier chaque exclusion et conserver les noms exacts. Dénominateur déterminé sur H3, jamais 198/215/299/245 recyclé.

```powershell
& $py out/tooling/run-build.py b2-h3-asan-selected ctest --preset windows-msvc-debug-asan --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/asan-selected-names.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/asan-selected.xml
```

Le chemin tests-from-file doit être absolu : un échec de résolution avant tests serait un défaut de dispositif, pas RED produit. Vérifier que le CTest installé supporte ces options avant de retenir la commande. Pas de test exécuté par ce document.

## Oracles et preuves à conserver

`Set-AsanTestEnvironment` doit imposer `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0` et supprimer ASAN_ACTIVATION_OPTIONS avant build/discovery/tests, pas uniquement canari. Processus PowerShell dédié ; environnement/delta conservés.

- Canari sain : exit0, marqueur ARTIST_ASAN_HEALTHY, pas de diagnostic AddressSanitizer.
- Canari faute : exit86 **et** diagnostic AddressSanitizer heap-use-after-free ; un crash seul ne suffit pas. Canari hors dénominateur CTest ; detect_leaks=0 ne prouve pas les fuites driver.
- Découverte/sélection/exécution/JUnit : mêmes noms distincts, pas de manque/doublon/inattendu, CPU+GL concordants et 17/17 retrouvés ; zéro FAIL/error/skip requis, zéro test jamais PASS.
- Conserver sorties runner complètes, LastTest.log, XML/JUnit, discovery, sélection déclarée, commandes, environnement, HEAD/delta, hashes sources/binaire/DLL/PDB/config. Archiver aussitôt avant écrasement au run suivant.
- Examiner les logs au-delà du résumé : un échec GMock post-summary peut être masqué par une sortie tronquée même si exit0/Test Passed. La limite de sortie65536 ne garantit pas à elle seule l'absence de troncature.
- Mutation pertinente ultérieure : invalider les preuves concernées. Échec reproductible = FAIL ; précondition/découverte/coverage manquante = INCONCLUSIVE ; aucune qualification globale de PASS induite.

## Limites et intégration

Campagne Windows Debug/MSVC/ASan H3 **NOT_RUN** ; candidat INCONCLUSIVE à ce périmètre. Ni Release, GCC/autres OS, image ou performance. ADR0005 Accepted/partial,0008 Windows Debug seulement inchangés. F8 close réel DEFER, sans verdict lecture anticipé.

Ce protocole est un livrable producteur, pas la campagne B2 exécutée. Les tâches historiques et de reprise devront être liées explicitement à son commit avant de libérer les consommateurs. Aucun changement source ou d'infrastructure.
