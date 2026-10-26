# 91 — Nouveau candidat validé Windows Debug et ASan ; revues indépendantes

2026-10-26. Candidat source intégré **a96104c7aaeede7dd6915bfeac86f53565615114**. QA89 avant code, caractérisations90 avant refactor ; auteur/runner lead. Dernier contrôle identité : aucun changement des sources/binaries/config capturés pendant les campagnes.

## Résultats réellement exécutés

- Build normal unit+component : exit0,31,9s,15 unités de build/relink, un job. Build ASan séparé même cible : exit0,32,3s,15 unités. Warnings préexistants macro GL C4005 et nodiscard C4834 conservés, pas traités hors scope.
- Normal **555/555 =484CPU+71OpenGL**,0failure/error/skip/notrun, découverte=sélection=exécution en noms et ordre,555 uniques,12 Reader.31,0s ; vrais tests component GL, pas mocks substitués.
- ASan **555/555 =484CPU+71OpenGL**,0failure/error/skip/notrun, mêmes noms/ordre que découverte et sélection,17historiques chacun une fois.103,7s. Normal et ASan ont la même sélection courante.
- Les555 actuels **ne sont pas l'ancienne population héritée** :3 tests seam/publication Windows retirés,3 caractérisations stables ajoutées ; même cardinalité par coïncidence. Les15 tests pré-refactor incluaient les3 anciens et3 nouveaux ;12 Reader subsistent.
- Exécutions sérielles `--parallel 1 --no-tests=error`, sélection-from-file pré-run, logs complets/JUnit, limiteoutput65536. Bail interprocessus coopératif par handle exclusif `out/artist-opengl.lock`, libéré en finally. Aucun autre runner lancé.
- ASAN_OPTIONS canonique via module existant avant build/découverte/exécution ; activation secondaire absente. Debug et /RTC1 conservés. Préflight0/86 antérieur réutilisé **après** contrôle identité probe+summary+configuration et6DLL runtime ; aucun nouveau probe expérimental.
- Recoupement indépendant du résumé CTest par script d'analyse lead : JUnit555 uniques, LastTest555 `Test Passed.`,0 `Test Failed.`,0diagnosticASan/runtime/DEADLYSIGNAL et0marqueurtroncature ; sorties max17126octets normal/17129ASan. Hashes avant/après inchangés, dont15entrées ASan (sources,EXE,PDB,DLL,cache/ninja). Pas de résultat déduit du seul code retour.

## Commandes et preuves

Wrapper partagé `python out/tooling/run-build.py`, non modifié. Build : `cmake --build --preset windows-msvc-debug[-asan] --target ArtistLib_tests ArtistLib_component_tests --parallel 1`. Découverte `ctest --preset ... --show-only=json-v1`.

CTest : `ctest --preset windows-msvc-debug[-asan] --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-f8/stable-20261026-01/{normal,asan}-selected.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-f8/stable-20261026-01/{normal,asan}.xml`. Les deux variantes ont été exécutées séparément, pas des placeholders au runner : argv exactes dans `{normal,asan}-command.txt`.

Archives `out/qa-f8/stable-20261026-01/` : plans pré-run, préReader.xml,discovery/selected/before/verification par campagne,LastTest et CTest complets,logsrunner,preflight-reuse,b1-relevance. Logs build/découverte/CTest aussi `out/logs/stable-*.log`. JUnitSHA256 normal `b0d2e976908df2ad6b8b1d633d2e76c9a88cb7e2c22373d045b23b12c3581a5d`, ASan `590e62422f83829263994a676d1b3c65c38fb5b6418b8324bc75eff2a167bc24`.

## Revue indépendante reçue

Appel natif direct `correctness-reviewer`, contexte fresh initial confirmé par GET session et5messages/lecture+skill, pas d'histoire auteur ni de rapports/board. Session `ses_e864a6b90ffeWqUmBQrIGdpqpl` ; profil/parent/racine confirmés. Les permissions session renvoyées ne certifient pas le ruleset complet de profil ; cette limite est conservée, aucun grant ajouté. Revue RO sans tests/shell/édition/délégation. Cette voie est celle de l'organisation locale simplifiée, **pas un audit natif test-reviewer maquillé**.

Revue Reader : aucun finding démontré au diff ; lifetime/local accumulation/catch/fermeture-vérification-publication recoupés, consommateurs ShaderContext/Shader/Loader suivis ; scénarios discriminants troncature/EOFexact/stale/NUL identifiés. Limites : erreurs authentiques extraction/close et ordre portable non démontrés en exécution,GLaprèsNUL hors tranche,consommateurs externesdetail non exclus.

Revue finale B1 : défaut historique d'isolation des defaults ON_CALL confirmé. A : compilation fausse héritée,1échec/4 exécutions historiques vs4/4current ; B : handles8/5 hérités,2échecs/6 historiques vs6/6current. `reset()` efface expectations mais pasdefaults ; le code actuel borne les overrides par EXPECT_CALL/WillOnce, sans resetexhaustif revendiqué. Reviewer recommande clôture **pour ces deux contaminations et populations précises**, pas replay176 ni résolutionuniverselle. CodeB1 comparé82bf52d→a96104c : aucun diff dans les4 fichiers concernés ; vieuxbinarySHA3d4fb31e… vérifié identique. Preuvescurrent réduites restent au pin82bf52d, pas nouveau runrépété a961.

Audit final de cohérence des archives transmis au même reviewer indépendant, encore en cours à l'écriture de ce jalon ; aucune nouvelle campagne demandée. Reste : intégrer son retour, gap-check local explicite, photographie documentaire/QMD et disposition finale B1–B6. Aucune recherche OS/calibration ou correction spéculative reprise. Pas de PASS global moteur, ni GCC/Linux/Release/image/performance.
