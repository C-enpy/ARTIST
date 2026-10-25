# Complément neutre à l'audit — delta et archives

2026-10-25. Digestion par le lead, aucune nouvelle exécution de tests. Pins et empreintes inchangés au contrôle19. Les anciens résultats blocked/failed de l'admission précèdent le GO reçu ; aucune conclusion sur les tests n'en est déduite.

## Références du plan préalable

Plan QA commité : .opencode/missions/team_mvm73qjo_0000_6wzibc/reports/17-plan-qa.md. Extraits contractuels sans justifications auteurs :
- lignes37–45 : E2 ASan final/GL ; E5 unbind trois tests avec garde existante ; E6 consolidation après inventaire/allowlist, égalité stricte ; E8 const sans revendication GCC ; E9 RED causal/GREEN/non-régression ; E10 documentation/statuts honnêtes.
- lignes48–51 : I1 diagnostics tardifs, I2 cas uniques/répétitions, I4 requalification après intégration.
- lignes59–64 : types/messages/cardinalités non affaiblis, baux exclusifs/GL sérialisé, mocks≠GPU, zéro test/skip/précondition manquante≠PASS, aucune installation.
- lignes95–98 : canari, découverte et présence effective17noms ; recouper sélection/découverte/exécution/JUnit/logs/skips/uniques.
- lignes124–131 : unbind avant destruction/contexte vivant, sortie anticipée ; helper type/message complet exact, préfixe/suffixe/portion/mauvais type/absence rejetés.
- lignes139–143 : const-correctness/retour possédé, pas de macros GCC forcées ; sans GCC réel aucun PASS GCC.
- lignes234–240 : sortie bornée, registres/limites et audit fresh après intégration.

## Delta intégré des attendus et consolidation

Source : commande lead réellement exécutée `git diff 318bf7a^ 318bf7a --unified=1 -- <neuf chemins ci-dessous>`, exit0. Aucun delta production de ce commit. Les numéros de hunks sont avant→après ; lire les corps canoniques pour les assertions voisines.

- component-tests/src/graphic/shader/ShaderTests.cpp52/82/112/142/172 : dans cinq tests BadVert/Frag/Geom/TesselaionControl/Compute, littéral `ERROR::SHADER::COMPILATION_FAILED` → `ERROR::SHADER::COMPILATION_FAILED\n`.
- tests/src/graphic/context/PassContextTests.cpp124→124–125 : `ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\n` → même préfixe puis `Shader index ` + to_string(rejectedIndex) + ` out of range (size 1)`.
- tests/src/graphic/context/PipelineContextTests.cpp128→128–129 : `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\n` → même préfixe puis `Pass index ` + to_string(rejectedIndex) + ` out of range (size 1)`.
- tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp141 et212 : `ERROR::SHADER::COMPILATION_FAILED` → même chaîne avec newline finale, pour LoadShaderFailureTest et CompileFailurePreservesErrorAndCleansUp.
- tests/src/graphic/opengl/pipeline/component/shader/ReaderTests.cpp399 : `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ` → `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\nReading stopped without reaching end-of-file.`.
- tests/src/graphic/pipeline/PassTests.cpp107 : `ERROR::SHADER::UNIFORM_NOT_FOUND` → `ERROR::SHADER::UNIFORM_NOT_FOUND\nUniform test not found`.
- tests/src/graphic/pipeline/UniformOwnershipTests.cpp107 : `ERROR::UNIFORM::GET::UNINITIALIZED` → `ERROR::UNIFORM::GET::UNINITIALIZED: No value was set for the uniform variable (requested type ` + typeid(float).name() + `)`.
- tests/src/graphic/pipeline/ClosedExecutionTests.cpp98–120→98–100 : helper local expectExactError supprimé, using artist::test::utils::expectSpecificError. Ancien helper : appel forward ; FAIL si absence ; catch(const E&) EXPECT_STREQ(actual.what(),expected.what()) ; catch std::exception FAIL wrapped/retargeted ; catch(...) FAIL category. Appels renommés aux anciens209/225/269/307/326→nouveaux189/205/249/287/306 ; attendus inchangés, dont FREED_CONTEXT.
- tests/src/graphic/pipeline/PipelineCursorTests.cpp25–37→25–27 : helper expectExactPipelineError supprimé, même using. Ancien helper : appel forward ; FAIL absence ; catch(const E&) EXPECT_STREQ ; catch(...) FAIL category. Appels anciens87/100/256→nouveaux77/90/246 ; attendus inchangés (format bornes, NULL_PASS, cardinalité).

Tous ces chemins commencent par ARTIST/. Le delta helper/meta-tests lui-même est directement lisible dans les deux sources fournies. Le delta ultérieur82bf52d ajoute seulement EXPECT_CALL glGetActiveAttrib à ShaderLifetimeTests.cpp, déjà fourni au contrat ; aucune action ON_CALL ou assertion de destruction supprimée.

## Campagne archivée et recoupement actuel

Archive : out/qa-b2-h3/final-h4-20261025-01/. Manifeste créé16:59:05Z avant run16:59:27Z–17:01:12Z. Runner lead, processus49572, mutex LocalARTIST_QA_OPENGL ; run-end atteste bail détenu, exit0.

Commande archivée : `ctest --preset windows-msvc-debug-asan --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-h4-20261025-01/selected-names.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-h4-20261025-01/asan-selected.xml`.

Environnement archivé : ASAN_OPTIONS=halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0 ; ASAN_ACTIVATION_OPTIONS null. Windows x64 Debug, MSVC14.44.35207/19.44, CMake3.31.7, Conan2.12.2, Ninja1.13.0.git.kitware.jobserver-pipe-1 enregistrés dans les preuves de campagne ; pas de nouvelle invocation de ces outils pour cette digestion. /RTC1/MDd/assertions conservés. Les configs/binaires/canari ont été rehashés au19, sans rebuild ni canari rejoué.

Recoupement lead actuel par analyse XML/JSON/logs uniquement :555découverts,555sélectionnés,555testcase JUnit,555noms uniques ; ensembles identiques.0failure/error/skipped. LastTest :555marqueurs Test Passed.,0Test Failed. Fin ctest-full :100% tests passed,0tests failed out of555 ; labels71OpenGL/484unit. Aucun nouveau test exécuté par ce recoupement.

Scan actuel JUnit : aucune ligne non vide après dernier marqueur PASSED par testcase ; aucun ERROR/SUMMARY AddressSanitizer ou AddressSanitizer: heap, aucun output truncated/output has been truncated. Maximum17127octets en concaténant **tout itertext testcase**, sous65536 ; ce calcul inclut davantage de texte que l'ancien maximum17112 des seules sorties, ce n'est pas une mutation de l'archive. Contrôle historique complémentaire des diagnostics tardifs dans les logs : lateOutputs=[] et asanDiagnostics=[] du summary dont neuf empreintes ont été recoupées. Ces recherches bornées ne sont pas un détecteur universel de tout diagnostic possible.

## Les17historiques

Chacun ci-dessous est présent **une fois dans le JUnit actuel**, aucun remapping inventé ; découverte et sélection ont les mêmes555noms uniques.

- opengl.PassTest.PassCreationTest
- opengl.PassTest.UniformManagementTest
- opengl.PassTest.InvalidUniformTest
- opengl.PassTest.ShaderAttachmentTest
- opengl.PassTest.UniformReadingTest
- opengl.PassTest.AttributeReadingTest
- opengl.PassTest.FreeResourcesTest
- opengl.ShaderTest.CreateShaderVert
- opengl.ShaderTest.CreateShaderBadVert
- opengl.ShaderTest.CreateShaderFrag
- opengl.ShaderTest.CreateShaderBadFrag
- opengl.ShaderTest.CreateShaderGeom
- opengl.ShaderTest.CreateShaderBadGeom
- opengl.ShaderTest.CreateShaderTesselaionControl
- opengl.ShaderTest.CreateShaderBadTesselaionControl
- opengl.ShaderTest.CreateShaderCompute
- opengl.ShaderTest.CreateShaderBadCompute

Limites toujours ouvertes : F8 catch INCONCLUSIVE/close DEFER, C2 OPEN/INCONCLUSIVE, B1 historique non résolu, GCC natif non validé ; pas de Linux/Release/image/performance. Ce texte ne fournit aucun verdict d'audit à adopter.
