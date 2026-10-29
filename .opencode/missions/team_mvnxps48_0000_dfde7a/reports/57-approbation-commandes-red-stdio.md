# 57 — Commandes exactes RED stdio approuvées avant exécution

2026-10-26. Lead runner, protocole56 et sélection44/46/49. Source intégrée **c77fb5a10168b2a9dace0a3e8bea2002cae49705**, identique aux trois blobs auteurfc02b9 ; pas de diffARTIST. Manifestes/sentinelle créés, **aucun compilateur/contrôle encore exécuté à cette approbation**.

Manifeste intégral : `out/qa-f8/stdio-red-20261026-01/candidate-before-build.json`, SHA256 **098224db652798ce1d4baf3bf2c9bb4b7fc0566e7a8c1607a81576cbf5b52c14**. Contient buildArgv/runArgv COMPLETS, entréeCLoriginale/substitutions4tokens, tousSHA sources/compiler/wrappers/compile_commands/env et vecteursattendus. Sortie/tagneufs vérifiésabsentsavantcréation, owned-controls absent, sentinelvoisinécritpossédé/hashé.

**Build approuvé** : argvexact `buildArgv` de ce manifeste, sans interpolationshell ni optionajoutée. CommandLineToArgvW a conservéexecutableclactuel, tousincludes etflagsDebug /RTC1,-MDd,-Zi,-std:c++latest ; suppriméseulement /Foancienneobj,/Fdanciennepdb,-c,sourceTestancienne ; nouveaux sourcecontrols etobj/PDB/exe soussortieneuve,/link /DEBUG /PDB. Aucune sortiebuildnormalécrasée, pas de configuration/Conan/install/CMakebuild. Un seulcl direct, pas buildsconcurrents.

**Run approuvé conditionnel** : uniquementaprèsbuildexit0/binairehashé etidentitésourceinchangées, argvexact `runArgv` (wrapperexistanttagunique puis controls.exe --case native-read-whole --root owned-controlsabsoluneuf). Avantinvocation, manifester candidate-before-run avecbinaire/SHAlogbuild etsource ; attendu4cas exactement,3PASS et12325FAIL/received8192/read_ok1/error0/exit1. Setup20/cleanup30/builderror ne sont pasRED ; pas de deuxième essai identique en cas de surprise.

Processus dédiés, environnementhérité sansGTEST_*,ASAN_OPTIONS,ASAN_ACTIVATION_OPTIONS ; wrapper conserve sonenvMSVC/VSLANG, stdout/stderr fusionnés archivésexplicitement. LogsCOMPLETS sourcewrapper/out/logs et copiesdanssortieavantécrasement. Hash physique desfixtures noncapturéavantleurcréationdanscontrôle ; SHA vecteursattendus préenregistrés, WriteFile/flush/closecounts etcomparaisons exactes contrôlés au run, sansrevendiquerdirecthashphysiquemanquant.

Source/notes/wrappers etsentinelle vérifiésavant/après, cleanupdequatrechemins acquisetroot, voisin/parentsortieintacts, diagnosticsfinaux conservés. Niveau CPUWin32autonome, pas CTest/GTest/JUnit niASan/GPU ; aucunPASSmoteur/close/catch déduit. Anciennespreuvesrestentdatées, aucunecampagneacquise relancée. RevueindépendantedusourceGREEN sensible à organiser après vraieintégration, pas auto-revue diteindépendante.
