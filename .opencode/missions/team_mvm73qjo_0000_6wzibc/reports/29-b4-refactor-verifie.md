# 29 — B4-U : patch appliqué, sélection GL finale 10/10

2026-10-24. Reprise autorisée par l'opérateur « go ». Proposition : pool-10 ; application/revue/exécution : lead. Base HEAD ecb548b. Aucun worker réactivé.

## Delta et preuve préalable

`ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp` : deux caractérisations de sortie fatale déjà validées avant refactor (rapports21/22), puis remplacement des trois débinds terminaux de UsePassTest/UseNextPassTest/ResetPipelineTest par ProgramUnbinder existant, déclaré après loaded et avant toute activation. Destruction garde→loaded→lease, contexte encore vivant. Toutes les assertions, dont F10 reset/cursor/programme vivant et lié, sont conservées.

Le lead a comparé le fichier entier au snapshot caractérisé02 : seuls les trois ajouts de garde et trois retraits de glUseProgram terminaux diffèrent. Aucun changement production, helper ou dépendance. Refactoring de tests sur caractérisation, pas RED moteur fabriqué. `git diff --check` propre.

## Commandes exécutées et résultats

Via python local et out/tooling/run-build.py, cache existant identifié Debug/Ninja/ASanOFF/racine canonique :
```text
cmake --build --preset windows-msvc-debug --target ArtistLib_component_tests --parallel 1
ctest --test-dir out/build/windows-msvc-debug -L opengl -R <sélection10> --show-only=json-v1
ctest --test-dir out/build/windows-msvc-debug -L opengl -R <sélection10> --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <sortie>/ctest.xml
```
Sélection ancrée PipelineTest : trois tests nommés, cinq ClosedExecution et deux caractérisations (liste exacte dans discovery.json). Découverte fraîche : dix noms uniques identiques à la caractérisation. **Build exit0 ; CTest exit0 ; JUnit10/10,0 failure,0 error,0 skip**, liste confrontée à la découverte. Contrôle du source inchangé pendant run.

Logs `out/logs/b4-refactor-{build,discovery,run}.log`. Sortie canonique possédée `out/qa-b4/refactor-20261024-01` : XML, LastTest.log, runner.log, discovery.json, snapshot ProgramTests.cpp. Commande intégrale journalisée par wrapper. Aucune installation ni nouveau contexte mock substitué au GL réel ; CTest conserve RESOURCE_LOCK artist_opengl et exécution séquentielle.

SHA256 ProgramTests : `3a1fff8c95bff4a27dcd978a62aec5adbf2363dc4eae4a6b0e4c6275f50ce503` ; component binaire : `fa7b517eda202adfea23fe21b657c34f481e6afea74a5d3e090d55149f7f102e`. Build11 étapes dont recompilation après header B6 ; warnings hors diff conservés, sans correction incidente.

## Portée et suite

**PASS ciblé B4-U** binding/cleanup et non-régression F10 ; pas image/performance ni suite globale. Le commit ciblé contient ce rapport et ProgramTests seulement, indépendamment des deltas concurrents profils/GrepAI/harness. Revue indépendante et campagne finale restent à faire sur le candidat intégré ; helper strict pas encore traité, F8 non corrigé (rapport26), C2 ouvert et B1 historique non fermé. Aucun PASS global.
