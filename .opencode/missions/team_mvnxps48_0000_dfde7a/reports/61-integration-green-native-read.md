# 61 — GREEN source intégrée avant runner, assertions inchangées

2026-10-26. Même auteurpool8/cpp-engineer/même trancheTDD, commit **2bbc85c4d53612e1dc5a668ae1871ffcda62cda3**, worktreepropre rapporté ; aucun build/test auteur. Les blocked d'admission antérieurs auGO58/59 sont historiques et ne remplacent pas cette remise.

## Intégration et portée

Deux blobs intégrés par patch exact, pas gitmerge manuel ni shutdown libérant une tâche avant preuve :
- native_read.hpp b1ec9817be308180f347209d4922698a094b4582.
- RED-PROTOCOL.md e3b6ebffd78101e47dd023aa22a294fb887d54c3.

Identités comparées au commit auteur ; diffciblé81insertions/12suppressions, diffcheckexit0, aucun diffARTIST. Controls.cpp **strictement immuable** SHAed9e0e865d2f48c67f9294a7a05bce52bfeadc5c60545bc36f82f3805f8517f8, comparateur/attendusinchangés. AncienREDpréservécommitc77/artefacts58.

Lecture lead : accumulechaqueblocReadFile réussi jusqu'à vrai succès/n0, lecturecourtnevalantpasEOF ; erroreurcaptureavantdiagnostic,pasbufferdureadéchoué publié. Native.n devient size_t cohérentstring ; garde max_size-size avantappend, pasfstat/sizecheck/statiqueasset. HANDLElocal noncopiable, tentativecloseunefois normal/unwind, slotinvalidéavanttentative ; diagnostic Cfprintf et erreurReadFileprimairepréservée si closeéchoueaussi, closeéchouéaprèslecturecorrecte invalidecontrôle. Ces observations sont **relecture lead**, pas revueindépendante ou exécutionbrancheerreur/allocation/close.

SourceS uniquementhelper etannotationhistoriqueRED ; aucunprobeCRT,parentclose,Reader,moteur,CMake,installation/service/dépendance/GL/ASan. SerenaauteurcanonicalARTISTneprouvaitpasrefsduworktree ; grepworktreeciblétrouveuniqueconsommateurcontrols::check compatiblecompte64bits, limiteindexsignalée sansréactivation.

## Runner GREEN prévu, pas résultat présumé

Sortie **out/qa-f8/stdio-green-20261026-01** neuve, tagsbuild/rununiques correspondants ; aucuneanciennearchiveREDécrasée. MêmeoutilsetentréeCLpinnés54/57/58, mêmesflagsDebug/includes/executablecompilateur, mêmesargumentssauf cheminsobj/PDB/exe/log/rootneufs ; sourcecontrolsidentique, header/noteaucommitGREENintégré. Pas CMake/ConanconfigureoucampagneCTest/ASan pourseulhelper autonome.

Manifesteprébuild puis pré-run avecSHAexebuild0, sélection4tailles0/4096/8192/12325 etvecteursattendus exacts identiquesRED ; sentinelvoisin/owned-controlsabsentavantprocessus. ArgvCOMPLETS contrôlés etapprovéslead avantinvocation. Oracle **4PASS/exeexit0**,ReadBOOL1/error0,tousoctets/tailles exacts, cleanup4fichiers/racine etvoisinsintacts, SHAavantaprès/diagnosticsfinaux ; sinonFAILouINCONCLUSIVEselonmotif, pasassertionsaffaiblies ni retrycosmétique.

NominalGREENneprouvepasgenuineerreurReadFile/close/alloc ; revuefreshsensibledusourceintégré après cette tranche avantconclusion finale. M0traductions/M1stdioferror/M2recovery nonencorecodés/exécutés, M5Reader/catchrestentnonprouvés, pasPASSglobal.

Sélection sécurité ciblée avant exécution : les deux garantiesCLI/acquisition déjàspécifiées44/48/49 seront vérifiées avec **deux processus supplémentaires bornés**, pas nouveaux essaisCRT : racine préexistante créée/possédéeparlead contenantunfichiertémoin (attenduexit20/racine+témoinintacts,aucuncleanupparcontrols), et nomdecasinconnu (exit20/aucuneracineacquise). Manifeste inclut cesargv/tagsséparés etétatspré/post. Les quatre assertions duhelper etcesdeuxcontrôlesCLI sont comptés séparément ; aucun0casdelecture dansuncontrôleCLI n'est renomméPASSdeslectures. Aucunrootextérieuràl'outputprivé n'estexposé.

**Approbation exacte avantbuild/run** : manifeste out/qa-f8/stdio-green-20261026-01/candidate-before-build.json écrit, SHA271a17dbaa65f3e8e275470bdff4f91035ece1a6049f267152a1f4e41398d418. HEADsource961d92ed06904e6d97b0ac5969aba6f8d205e21c ; trois sourceshashées/controlimmuable, mêmesoutilspinnés/compiler etargumentsnonoutputsidentiquesRED. Argvbuild/run/safetyRuns complets approuvéslead telsqu'enregistrés, sansinterpolationshell ni nouvellesoptions ; seulesdifférences outputs/tags/sentinelneufs. Sortie/logsvérifiésabsentsavantacquisition ; owned-controls/owned-invalidabsents, preexisting-root/keep créés ethashés parlead pourlenégatif. Aucunbuild/testencoreexécuté à cette approbation. Build0/binairehashé etmanifestepre-runprécéderontchaqueinvocation ; surprise=STOPpasrerunidentique.
