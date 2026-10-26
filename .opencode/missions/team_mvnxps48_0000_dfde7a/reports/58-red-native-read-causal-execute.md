# 58 — RED causal nativeRead exécuté par le lead

2026-10-26. Contrat44 adopté46, source49/56 intégréec77fb5a ; commandes intégrales57 approuvées avant invocation. Auteurpool8 écrit, **runnerlead exécute**, aucun témoignage d'auteur pris pour test.

## Préconditions et commandes réellement exécutées

Build unique clMSVC14.44Debug dérivé de la véritable entréeTraceableExceptionTests.cpp, viawrapperexistant ; argvexact/source/header/compiler/env/SHA dans `out/qa-f8/stdio-red-20261026-01/candidate-before-build.json` (SHA098224db652798ce1d4baf3bf2c9bb4b7fc0566e7a8c1607a81576cbf5b52c14), build-result.json et logscomplets. Build exit0,16:12:22–16:12:25Z, sortiecontrols.cpp seulement, stderrwrappervide. Sourcecontrôle/helper SHApréservées, protections /RTC1/-MDd/-Zi/C++latest conservées. Binaire **233932deb7b52796cce60787c62963ad9f2e8e59cd81b8a83ec279f15572996a**.

Manifeste candidate-before-run.json écrit **avant** exécution, SHA163188835205fb86894c74a97c94f57443f7d2247c8b5d8894b5b563dac31f4c. Run réel16:14:05–16:14:06Z :

`out/tooling/build/Scripts/python.exe out/tooling/run-build.py qa-f8-stdio-red-20261026-01-controls out/qa-f8/stdio-red-20261026-01/controls.exe --case native-read-whole --root C:/Users/djden/source/repos/ARTIST/out/qa-f8/stdio-red-20261026-01/owned-controls`

Arguments effectifs absolus complets au manifeste/run-result ; cwdcanoniqueARTIST, environnementprocessus dédié sansGTEST_*/ASAN_OPTIONS/ASAN_ACTIVATION_OPTIONS, aucune modificationglobale. Wrapperstdout/stderrfusionnés explicitement, copieslogcomplètes danssortie etout/logs/tag.log, pas de pipeline masquantcode.

## Résultat observé, pas seulement attendu

- cas0 : reçu0,ReadFileBOOL1/error0,tailles/octetsexacts,PASS.
- cas4096 : reçu4096,ReadFileBOOL1/error0,tailles/octetsexacts,PASS.
- cas8192 : reçu8192,ReadFileBOOL1/error0,tailles/octetsexacts,PASS.
- cas12325 : reçu**8192**,ReadFileBOOL1/error0,comparaisonlongueur ET stringcomplète échouent,FAIL.
- **Processus exit1**, quatrecontrôles exactement exécutés,3PASS/1FAILattendu,0skip. Ce n'est pas un motifsetup20/cleanup30 ni compilationcassée.

Créationexclusive/racine et écritures/flush/closefixtures ont réussi. Quatre suppressions et RemoveDirectory réussis ; owned-controls absent, sentinelvoisinetparentoutputintacts, hashssource/binaireavantaprès identiques. Aucunstderrwrapper, log finit exactement `exit=1`, aucun diagnostic tardif ignoré. Toutes vérificationspostrun ont réussi.

run.logSHA **9251f4473aa8cacfa5bcc4ab5b9c82b09d6125d2fb9d5d7299c52e95a1aa2cfc**, run-result.json contient codes/cas/checks/oracles/horodatages et commandes ; sources et protocoleconservés, aucune réexécutionidentique.

## Portée et prochaine tranche

**RED causal acquis pour nouveau contrat nativeRead exhaustif12325**, parceque helperhérité ne litqu'unefois8192. Pas un défaut de son ancienusage4608, pas REDReader ni preuveerreurCRT/close/catch. Compte4contrôlesautonomesWin32, pas CTest/GTest/JUnit/ASan/GPU ou nouveaucampagne555. SHAphysique desfixtures nonindépendammentcapturéavantcleanup : vecteursattendusSHApréenregistrés,WriteFilecountscheckedetcomparaisons complètes attestées, limiteconsignée.

Mêmepropriétairepool8 peut effectuer GREENminimum duhelper après GOlead, contrôles/comparateur/attendus **immuables**. Sourcefixée réellement intégrée avant runnerGREEN mêmes4cas etrevueindépendante sensible du candidatfinal. Aucune sondeexpérimentaleM0/M1/M2 ni correctionReader/OSclose admise par ce RED seul.
