# 62 — GREEN exécuté :4contrôles,2garde-fousCLI distincts

2026-10-26. Auteurpool8, intégration961d92ed06904e6d97b0ac5969aba6f8d205e21c, runnerleadseul ; RED58 conservé, comparateurcontrols.cpp byte-identique SHAed9e0e865d2f48c67f9294a7a05bce52bfeadc5c60545bc36f82f3805f8517f8. Aucunefonctionmoteur/Reader modifiée.

## Identité et commandes avant essais

Sortieneuve `out/qa-f8/stdio-green-20261026-01`, manifestecandidate-before-build SHA**271a17dbaa65f3e8e275470bdff4f91035ece1a6049f267152a1f4e41398d418**. Argvbuild/run/safetyRuns intégraux approuvés61avantinvocation ; mêmesflags/outils qu'auRED, seuls outputs/tagssubstitués, aucunconfigure/Conan/install. SourceGreenheaderSHA**c75d648542b27f2511417b27478ad7bb438b85d66469edafbdd7a6609b91a026**, contrôles/note etoutilségalementpinnés.

Buildunique16:55:17–16:55:20Z **exit0**, logcontrols.cpp seulement, stderrwrappervide. Binaire **33aeccde60e06d7313c773d4445a353e275d8c8bbf098c05cba6668928ca0a15**. Manifestecandidate-before-run écrit avantpremierrun, SHA**86f32e40fea73e580142df18921ee0fffe49915073359593725d7278b2119006**, contient sélections/argv/binaire/sources/vecteurs/sentinelles.

Runs réels via `out/tooling/build/Scripts/python.exe out/tooling/run-build.py TAG controls.exe --case CASE --root ROOT`, cwdcanonique, argseffectifsabsolus complets dansmanifestes/resultats ; stdout/stderr fusionnésdocumentés/copiéscomplets. EnvprocessussansGTEST_*/ASAN_OPTIONS/ASAN_ACTIVATION_OPTIONS, Debug /RTC1/-MDd/-Zi/C++latest conservé, pas de changementglobal.

## Observé

- **Lecture complète**,16:56:45–16:56:46Z, tagqa-f8-stdio-green-20261026-01-controls : quatrecas0/4096/8192/**12325** reçoiventexactementleurnombre/octetsexacts,ReadBOOL1/error0, **4PASS/0FAIL/0skip,exit0**. Dernierfichierlit8192puis4133puis0, pas arrêtabusifsurlecturecourte. Closecontrolunefoisparcas, quatreDeleteFile etRemoveDirectoryréussis, racinefixtureabsente/sentinelvoisineintacte, logsfinissentexit0sansdiagnostictardif.
- **Racine déjà existante**,16:56:46–16:56:47Z : tag...-existing-root, execexit**20attendu**, CreateDirectoryrefuseavecerror183 ; aucunecasedelecture/écriture/suppression, rootettémoinSHAintacts. C'est uncontrôleCLIacquisitiondistinct, pas quatrelecturesdeplus. Root/fichiertémoincréésparlead supprimésparleurpropriétaire **après** vérification deleurpréservation.
- **Cas CLI inconnu**,16:56:47–16:56:49Z : tag...-invalid-case, execexit**20attendu**, usage/rejetavantacquisition, aucunelecture/racinecréée. Pas de0testdéclaréPASSduhelper : preuve derejetCLI uniquement.

Troisprocessus séquentiels : **4assertionshelper+2contratsCLI**, pas6GoogleTests/CTest. Source/binaireavantaprès identiques, sentinelleextérieure inchangée, logscomplets/commandes/horodatages/codes dansrun-resultspartenom etverification-summary.json. Pas de rerunidentique.

SHAlogs : whole-read **c80e4a7c2a0c7a5ee7d02c81dba143e2093fdc8f607c02e99b507e44f1269257** ; existing-root **b545d8a5b5c639343bee55ce091a0bb0e761af1ad120647b14a7e5a19d46df72** ; invalid-case **59cb026cb2e2e20f2f004733222feffb4bf133954d0f400d1054db5764c0fef8**.

## Verdict borné et limites

**GREEN acquis du nouveau contrathelperexhaustif**, après RED causal12325→8192 ; codecorrectifminimal/contrôlesinchangés. Garde-fousCLI ciblés conformes. Relecturelead nonindépendante ; revuefreshownership/exception/contrôles enattente avantacceptationfinale decettetranche.

GenuineerreurReadFile/close/allocation nonexécutéesici, modulesCRTFILE*noncalibrés,M0texte/M1ferror/M2recoverystdio encoreàfaire. Pas de nonpublicationReader/catch/M5, GPU/ASan/GCC/Linux/Release/image/perf/CTestnouveau ; ancienASan555 nonreprisenverdictduprobe. SHAphysiquedesfixturesruntime noncapturéindépendammentavantcleanup (vecteurspréhashés/WriteFilecountschecked/comparaisonscomplètes). Les prochainsconsommateurs attendent review/protocole etentréesréellementintégrées ; aucune clôtureglobale B3.
