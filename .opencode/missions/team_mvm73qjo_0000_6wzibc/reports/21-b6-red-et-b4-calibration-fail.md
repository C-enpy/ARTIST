# 21 — RED B6 transposé exécuté ; caractérisation B4 : 8 PASS / 2 FAIL d'oracle

Date : 2026-10-24. Runner réel : lead. Propositions : pool-11 (B6), pool-10 (B4-U). Aucun PASS global ni GCC annoncé.

## B6 — contrôle de langage causal exécuté

Générateur PowerShell proposé par l'auteur relu/persisté dans `out/qa-b6/generate-const-probe.ps1`, puis sortie exclusivement allouée `out/qa-b6/run-20261024-01`. Source réelle confrontée au header du pin64d97dc (seules fins de lignes normalisées pour cette comparaison). Extraction entre deuxième garde #if et #else, quatre substitutions de type std::stacktrace→ProbeStackTrace ; corps autrement identique. Main instancie et appelle réellement sur objet const.

Le lead a vérifié que les deux TU diffèrent uniquement du const ajouté au helper dans le contrôle positif jetable. Inclusions explicites propres au TU, pas de macro GCC forcée, faux header standard ou changement CheckCxx23.

Commandes réellement exécutées, via python local + `out/tooling/run-build.py`, toolset MSVC14.44 :
```text
cl.exe /nologo /std:c++latest /permissive- /EHsc /W4 /c /Fo<dir>/b6-positive.obj <dir>/b6-positive.cpp
cl.exe /nologo /std:c++latest /permissive- /EHsc /W4 /c /Fo<dir>/b6-negative.obj <dir>/b6-negative.cpp
```
`<dir>` = `C:/Users/djden/source/repos/ARTIST/out/qa-b6/run-20261024-01`. Positif exécuté **en premier**, exit0. Négatif mêmes options, exit2, **C2662** à format_stacktrace appelé depuis getStackTraceString const ; diagnostic explicite conversion de this const et perte des qualificateurs, aucun autre motif d'erreur relevé.

**RED causal obtenu dans la frontière transposée** : compilateur fonctionnel via positif, appel const-this rejeté dans le corps négatif. Ce n'est ni une compilation ArtistLib/GCC/libstdc++ ni un test runtime.

Manifestes/empreintes : `b6-manifest.json` sous ce répertoire ; source SHA256 `a277ebe97c530b180ad81f068087a2735a44f3aa514e5ef8f7c337c714f01ee7` ; négatif `45437914372d4202c064b556abc5031ce0b08c301ee56c6686784bb000d3199f` ; positif `a2b89ee39572c9457325449ec3bd4bc35c587b9b5dc3a193ad5d53f9d72c71a7`. Logs complets `out/logs/b6-const-positive.log` et `b6-const-negative.log`.

Résultat transmis à pool-11, autorisation du seul ajout const au helper privé dans son worktree ; pas d'incidental sstream/oracle GCC. GREEN à produire depuis le header réellement fixé, sans réutiliser naïvement le générateur baseline qui ajouterait const deux fois. Source production canonique non modifiée par ce contrôle ; non-régression MSVC intégrée encore à faire.

## B4-U — patch test uniquement et build

Proposition pool-10 relue et appliquée par le **lead** dans ProgramTests.cpp canonique : ajout gtest-spi/string et deux caractérisations, sans changer les trois anciens tests, moteur, TestUtils ou garde existante. Capture étroite de FAIL intentionnel ; nombre/type/message vérifiés hors capture ; binding/cleanup hors capture et garde de sécurité externe après oracle. Contrôle sans garde attend programme0 encore lié, variante garde attend zéro ; propriétaires encore vivants puis glIsProgram false après destruction.

Avant build : CMake/presets/Conan/CI/test-CMake/CheckCxx23 lus dans le sandbox ; cache existant identifié Debug/Ninja/ASanOFF/racine ARTIST et toolchain Conan existante. Aucun pip/Conan install ni reconfiguration implicite de plateforme.

Exécution via runner existant : `cmake --build --preset windows-msvc-debug --target ArtistLib_component_tests --parallel 1` : exit0, deux étapes compile ProgramTests puis link, log `out/logs/b4-characterization-build.log`.

Découverte fraîche `ctest --test-dir out/build/windows-msvc-debug -L opengl -R <sélection PipelineTest> --show-only=json-v1` : **10 noms exacts/uniques**, 3 historiques + 5 ClosedExecution + 2 nouveaux. Répertoire de test ARTIST canonique vérifié. Log JSON `out/logs/b4-characterization-discovery.log`.

## B4-U — run réel, résultat non vert

Commande réellement exécutée :
```text
ctest --test-dir out/build/windows-msvc-debug -L opengl -R <sélection PipelineTest> --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b4/characterization-20261024-01/ctest.xml
```
Sélection exacte : UsePassTest/UseNextPassTest/ResetPipelineTest, cinq ClosedExecution nommés par l'auteur, TerminalUnbindIsSkippedOnFatalReturn, ProgramUnbinderUnbindsOnFatalReturnBeforeOwnerCleanup. Motif CTest ancré et liste résultante comparée aux dix noms avant run.

**Exit8 ; JUnit recoupé :10 exécutés,2 failures,0 errors,0 skipped.** Les huit cas préexistants PASS. Deux nouveaux FAIL uniquement sur le littéral de message capturé : obtenu `Failed\nB4-U deliberate early return\n`, attendu sans la dernière newline. Nombre1/type fatal et oracles GL n'ont pas d'autre écart dans les logs ; cela ne rend pas ces deux cas PASS.

Contexte réel journalisé : NVIDIA Corporation, GeForce RTX3090/PCIe/SSE2, OpenGL3.3.0 NVIDIA617.42. Preuve API ciblée/binding, pas image/performance/simulation. ProgramTests SHA256 stable avant/après run : `f847c1bf4f7936d5ed7506ab412ffed26215bb44354539c8483e50825a399701`.

**Ce FAIL est une mauvaise calibration du test proposé, pas un RED moteur.** Ne pas modifier production/les trois anciens tests sur cette base. Archives du premier échec conservées sous `out/qa-b4/characterization-20261024-01/` : ctest.xml, LastTest.log complet, runner.log, discovery.json et snapshot ProgramTests.cpp. Aucun résultat écrasé par une relance à cette étape.

## Hypothèse nouvelle et recoupement framework

Le lead a lu le GoogleTest effectivement disponible sous `_deps/googletest-src` :
- `gtest.cc:2235–2245`, AppendUserMessage joint message généré et user par newline ;
- `gtest.cc:5332–5336`, AddTestPartResult ajoute explicitement une newline si os_stack_trace vide.

Cela explique le suffixe observé et fournit une justification distincte d'un changement aveugle d'assertion. Retour demandé à l'auteur : delta des seules constantes exactes, sans find/trim/tolérance ni changement GTest. Aucune correction appliquée ou relance à ce stade. La portée est celle du framework local ; ne pas prétendre portabilité d'oracle non testée.

## État et suite

B6 : RED transposé exécuté, correction minimale autorisée, GREEN final/MSVC/GCC non obtenus. B4 : huit anciens PASS, caractérisation globale FAIL tant que deux oracles erronés non corrigés/rejoués ; aucun refactor des trois tests encore autorisé. Sources canoniques : seulement delta test B4 non commité dans cette étape ; les changements de profils préexistants sont préservés hors ownership.

Après retour auteur : corriger l'oracle test avec égalité complète puis un run discriminant, pas répétition coûteuse ; qualifier ensuite le refactor. Revue indépendante et intégration restent dues avant consommateurs/QA finale. B1/B3 pas qualifiés par ces preuves ; C2 OPEN/INCONCLUSIVE, close réel DEFER, campagne H3 NOT_RUN.
