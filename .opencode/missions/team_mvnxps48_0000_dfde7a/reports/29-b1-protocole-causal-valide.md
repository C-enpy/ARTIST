# 29 — B1 : protocole causal réduit validé avant exécution

2026-10-25. Proposition pool-5 reçue, lead runner seul. Enquête sous révision3 ; aucun développement/source/build. Ce protocole utilise des exécutables préservés et l'existant, pas un nouveau harnais versionné.

## Cause proposée et preuve déjà présente

Les primaires PRE_G1 attribuent explicitement des defaults de tests antérieurs : seed7, callCreate réutilise glGetShaderiv de LoadShaderFailureTest et reçoit COMPILATION_FAILED ; seed61219, observers reçoivent shader8/program5 depuis PassDestructorReleasesProgramThenShaders alors qu'ils attendent1. Singleton mock partagé ; reset VerifyAndClearExpectations ne supprime pas les ON_CALL. Source actuelle : ces trois sites sont déjà EXPECT_CALL/WillOnce. Cela ne justifie pas un correctif spéculatif ni le simple replay de seeds.

## Préconditions effectivement vérifiées par le lead

- H, PRE_G1 f914d77-era : out/mission-a1a2/gmock-amendment/author-final-2c7af8e/shuffle-preexisting-check/ArtistLib_tests-PRE_G1.exe,6433280octets, SHA2563d4fb31ee463a1c44370e9ba051e584784bf2f5becbf584341169d71d2ea1388, concordant manifeste historique lu.
- N, normal actuel source82bf52d : out/build/windows-msvc-debug/ARTIST/tests/ArtistLib_tests.exe,11142144octets, SHA2561b0fc02956d424f7fa6de02a91211b8e14f4295784d3563342be7c064e3950b5, concordant manifeste H4 pré-run.
- CWD canonique C:/Users/djden/source/repos/ARTIST, fixture test-datas/shaders/vertex/good/minimal.vert présente. Pas d'environnement GTEST_* hérité observé. Processus enfants dédiés avec GTEST_* supprimés et arguments explicites, aucun environnement système modifié.
- Découvertes filtrées H/N --gtest_list_tests exit0 chacune : A contient exactement callCreate puis LoadShaderFailureTest ; B contient LoaderTests.LoadPassTest_validContext puis ShaderLoaderTests.LoadShaderTest_setContextShaderId puis ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders. L'ordre n'est pas imposé par l'ordre des noms dans le filtre.
- Sortie réservée out/qa-b1/causal-20261025-01 absente au contrôle, créée avec manifeste avant run ; aucun artefact antérieur écrasé. CPU mocks seulement, aucun contexte OpenGL réel/ASan/build sollicité. Pas de ressource GPU partagée avec les enquêtes B3/B5.

## Commandes exactes et oracles

Appeler H puis N, **nouveau processus pour chaque bras**, racine canonique comme cwd, sorties stdout/stderr complètes et XML distincts. Arguments communs : `--gtest_shuffle=0 --gtest_repeat=2 --gtest_color=no`. Timeout60s par bras ; pas de retry identique.

A : `--gtest_filter=ShaderLoaderTests.LoadShaderTest_callCreate:ShaderLoaderTests.LoadShaderFailureTest` et `--gtest_output=xml:C:/Users/djden/source/repos/ARTIST/out/qa-b1/causal-20261025-01/A-historical.xml` (H) ou A-current.xml (N). Stdout/stderr A-historical.*.log / A-current.*.log. Oracle H : premier callCreate passe, puis Failure écrit le default, second callCreate échoue pour compile-status contaminé attribué au writer. Oracle N :4exécutions,2cas uniques,exit0,aucun diagnostic tardif ; XML final2testcases seulement.

B : `--gtest_filter=ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders:ShaderLoaderTests.LoadShaderTest_setContextShaderId:LoaderTests.LoadPassTest_validContext` et output xml absolu B-historical.xml ou B-current.xml, logs distincts. Ordre découvert observers puis writer ; répéter2fois permet d'observer son effet en seconde itération. Oracle H : observers shader8/program5 au lieu de1 après writer, exit1, attribution des defaults au test lifetime ; N :6exécutions,3uniques,exit0, aucun diagnostic tardif ; XML final3testcases.

H négatif doit échouer pour le mécanisme attendu, pas un open échoué/crash pretty-printer. En cas de panne ou autre raison : FAIL d'expérience/INCONCLUSIVE selon frontière, pas RED causal fabriqué et pas de nouvel essai sans hypothèse. Les deux expériences ne reproduisent pas l'ancien ordre176 et ne prétendent pas le faire ; elles isolent deux writers/observers. Compter RUN par itération et XML de dernière itération séparément, pas4/6nouveaux cas uniques.

## Décision lead

Protocole A/B approuvé pour ces quatre bras seulement, dans le contrat QA préalable17(B1)/révision3. Oracles et sélection archivés **avant exécution**. Aucun changement de test ni assouplissement. Recouper logs avant/après PASSED, diagnostics stderr, identité H/N avant/après ; disposition B1 après preuves, pas sur seul avis auteur. La restitution de pool-5 reste lecture statique, les exécutions à venir sont attribuées au lead.
