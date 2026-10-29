# 80 — Stdio intégré ; deux commandes COMPILE approuvées, aucun RUN

2026-10-26. RemisecomplèteS_PROBE_SOURCE/FIN de pool8, auteurcommit **c189858b804ddfb69de5e3b4b861ab8374aba327**,3fichiers508insertions. Sourcefixée/worktreepropre rapportés, aucuncompilateur/expérienceauteur. GAPREDcomportemental75 reste explicite, testsdéterministes nonexécutés aujalon.

## Intégration

Lecturelead des3corps : probe.cpp360l, probe_contracts.hpp86l, PROBE-PROTOCOL.md62l ; portéeM0/M1/M2,baselineproductiondirecte,owners/cleanup/recovery/contrôles/lignesissues. Pasrevueindépendante ou verdictcorrectnessdepuisnoms. Patch exact puis comparaisondesblobs aucommit auteur : probe215fccc163f05f0c876d6d53b251ff42c5af5a8d ; contrats95c83b64b5a7d132910d2bf36859ba9cf855e37f ; note471f5506cb2ed46f387ee31a59df2c30d267e13e. Commit intégré **d03f9964b71d21b5b2893c734c39e900be3e892a**, aucun diffARTIST.

SHAimmutables recontrôlés : helperc75d648542b27f2511417b27478ad7bb438b85d66469edafbdd7a6609b91a026, controlsed9e0e865d2f48c67f9294a7a05bce52bfeadc5c60545bc36f82f3805f8517f8, Readerce97d9e3ecfcf163ab757bb9c1dbf29116fb52505443c17a8725da5c0a90a284. Blobsclose56296feintégrés inchangés ; dossiersdiffsconcurrentspréservés. Auteurs gardent leursbranches sansshutdownlibéranttâcheactive.

## Manifeste et approbation avant compilateur

Sorties/logs neufs, aucunecollisionavantcréation :
- `out/qa-f8/stdio-cal-20261026-01/candidate-before-build.json`, SHA **9a4ed3dbd72a4ef218341ebcb9e75c88b51b845f19985dbcf47bf3298b2f619b**.
- `out/qa-f8/close-cal-20261026-01/candidate-before-build.json`, SHA **4cdc080a2908af705b8ee3091be6889d091cf0b1556885d3d7c0a7c9177702b4**.

Chaque manifestecontient argvbuild COMPLÈTE, entréeCLréelleoriginale, cinqsubstitutionsstrictes source/obj/compilerPDB/exe/linkPDB, source/header/config/tool/compilerSHA etenvprévu. DérivationdepuiscommandecontrôlesGREENpinnée, pasinventiondechemin : mêmesMSVC14.44Debug /RTC1/-MDd/-Zi/C++latest/includes/macros/protections, tousoutilsetcompile_commands identiquesauxSHAacquis. PasconfigureConan/CMake/install, pasnouvellelib dehash/tracing. FcloseUCRT et Win32seulementpourclose, Readerproductionincluspourcebaseline stdio sansappelGPU.

**Lead approuve les deux argvbuild exactes enregistrées avant invocation**, stdioPUISclose en série, unbuildparsortie. Attenduexit0/link0/binaireidentifié, logbrutcomplet etsource/configinchangées. Erreurcompiler=FAILbuildàdiagnostiquer, pasREDcomportementalOS oupanneenvironnementrenomméeRED. Aucunretryaveugle/flagassoupli, aucuneassertionmodifiée.

Le wrappertimeout1200 autorisé **pour ces seuls builds** ; toutRUNclose par ce wrapper resteinterdit. Aucunprobe/contrôleCLI/flag/processusdecalibration n'estlancéparlesdeux commandescompile/link. AucunmanifesteargvRUNapprouvéici. Exécutiondescaractérisations ensuite attendcandidats/binaire/protocoles/pre-run/sélections/fixtures/bail ; close attend aussi revuefresh79 en cours.

Revueindépendantesensible stdio à organiser avantacceptationfinale, pas auto-revuepreuve. RuntimeWAIT_FAILED/deadlock/close/authenticerrors nonexécutés,pasASan/GPU/Reader/catch/nonpublication/M5/PASSglobal déduitsdebuild. Compteurbuild2n'estpas2tests ; aucune revueauteur ou suiteacquise rejouée pourcejalon.
