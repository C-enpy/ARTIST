# 31 — Helpers stricts : RED→GREEN et candidat H2 normal 555/555

2026-10-24. Owner/auteur/runner : lead, tranche locale indivisible sous contrat30 et H_Q17. Base4c6cda2 après intégration B4/B6. Aucune production modifiée dans cette tranche.

## RED causal

Ajout de sept cas TestUtilsTests dans `ARTIST/tests/src/common/TestUtilsTests.cpp`, helper partagé encore inchangé. Configuration explicite `cmake --preset windows-msvc-debug` pour prendre le nouveau fichier dans le glob, sans installation ; build ArtistLib_tests parallèle1 : exit0.

`ctest --test-dir out/build/windows-msvc-debug -L unit -R ^unit[.]TestUtilsTests[.] --parallel 1 --no-tests=error --output-on-failure --output-junit <red>/ctest.xml` : **exit8,2 FAIL/7**, exactement RejectsAnExtraPrefix et RejectsAnExtraSuffix. Oracle : helper n'émet aucune failure alors qu'une failure non fatale est requise. Cinq contrôles nominal/portion inverse/absence/catégorie/nonstd PASS. Reporter GoogleTest existant capturant uniquement l'appel helper, vérifications hors capture, pas de dépendance à un texte de formatter.

Logs `out/logs/helpers-red-{configure,build,run}.log`. Archive `<red>` = out/qa-helpers/red-20261024-01 : XML, LastTest.log et snapshots test/helper pré-correctif.

## GREEN minimal puis consolidation

TestUtils.hpp : `find` remplacé par égalité complète EXPECT_EQ ; diagnostic wrong-category rendu générique, pas changement de catégorie catch. Build et sept contrats : **exit0,7/7,zéro skip/error/failure**. Archive out/qa-helpers/green-20261024-01 ; logs helpers-green-build/run.

Deux implémentations locales strictes supprimées dans ClosedExecutionTests et PipelineCursorTests ; appels redirigés vers le helper partagé maintenant strict. Imports et messages attendus conservés, aucune assertion d'état/lifetime modifiée.

## Première non-régression : échec conservé

Build des deux cibles normal exit0. Découverte555 cas. CTest complet séquentiel : **exit8,539 PASS/16 FAIL,0 skip/error**. Seize tests historiques attendaient seulement le préfixe d'un message ; pas de défaut moteur prouvé. Les sept fichiers consommateurs concernés ont été ajoutés explicitement à l'allowlist30 avant correction, pas une permission sur un sous-arbre.

Attendus complétés d'après formats de production et fixtures : index/taille, uniform nommé test, typeid(float), erreur lecture sans EOF et newline de compilation. Aucun diagnostic driver recopié, aucun matcher partiel, aucune tolérance ; flags/modes/primitive I/O inchangés. Les oracles d'état précédents restent présents.

Archive out/qa-helpers/nonreg-20261024-01 : XML, LastTest.log, runner.log, découverte ; logs helpers-nonreg-{build,discovery,run}.log. L'échec n'est ni effacé ni présenté comme un RED moteur.

## Non-régression finale de la tranche

Via wrapper out/tooling existant, commandes :
```text
cmake --build --preset windows-msvc-debug --target ArtistLib_tests ArtistLib_component_tests --parallel 1
ctest --test-dir out/build/windows-msvc-debug --show-only=json-v1
ctest --test-dir out/build/windows-msvc-debug --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <sortie>/ctest.xml
```
`<sortie>` = out/qa-helpers/nonreg-20261024-02 canonique. **Exit0 à chaque étape ;555 noms uniques découverts/exécutés identiques,484 unit+71 OpenGL,0 failure/error/skip.** XML confronté à découverte, pas seulement résumé. Durée CTest40,78s, pas mesure de performance moteur. Archives XML/LastTest/runner/discovery ; logs helpers-nonreg2-{build,discovery,run}.log.

SHA256 helper `13cfc1b5904a27511e6a8f9c4475151631f7100f72093540b47e7df14f4bd31f` ; tests contrats `7ccd2757dfdad90be465af9e1bea71f4c8d5a59db0d8bcc057b843c68b456c88` ; unit.exe `ef552933dcbc16e10f338e72ed375f85a0de971340b18c8d0d62df3bb244d44a` ; component.exe `14c94bae6ccf0ccb672de8a5b741b776e8506aae48685b73caf2c0a61fc16793`.

## Intégration et limites

Diff source relu, `git diff --check` propre. Commit ciblé de onze fichiers de tests/helper et rapports30/31, sans profils/GrepAI/harness concurrents. H2 désigne le candidat de ce commit après contrôle de son contenu. H3 attend les éventuelles propositions indépendantes intégrées ; **ASan final et revues pas encore exécutés**, aucune clôture globale. F8 et C2 restent ouverts selon rapports25/26 ; B1 historique non fermé. Les555 normaux ne prouvent ni GCC, Release, image/perf ni absence de bug GPU hors contrats testés.
