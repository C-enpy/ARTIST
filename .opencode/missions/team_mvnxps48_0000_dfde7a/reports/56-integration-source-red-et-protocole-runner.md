# 56 — Source RED intégrée : identité auteur et protocole runner figés

2026-10-26. Auteur pool-8/cpp-engineer fresh isolé, commit source **fc02b9481de736fcde722acc83dbbe00cebb46cf**, trois fichiers/188insertions. Remise reçue, lecture lead des trois corps effectuée ; zéro build/test auteur, aucune boucleGREEN/probe/Reader/moteur/CMake modifiés. Compte4contrôles attendu, pas RED exécuté à ce jalon.

## Intégration avant exécution, sans relâcher une tâche active

Le native shutdown libérerait la tâche S_CODE_RED encorein_progress avant preuveRED et risquerait un double auteur ; pas de cycle shutdown/réadmission. Auteur source à l'arrêt d'édition, worktreecommitpropre conservé, HOLD envoyé. Intégration lead par outilpatch des **trois blobs exacts** (pas gitmerge manuel, pas réécriture de correctif) ; identités Git comparées à fc02b9 :
- controls.cpp : 33d4c58d48386bac0421ef70f78d822c2cda883d.
- native_read.hpp : 44cc342efa0ce313e7fd2fbee52c6e140ad6a36c.
- RED-PROTOCOL.md : 0bc4dd95ad8a4cafb3b0145825f4e432ec17cae9.

Les trois hash-object correspondent au commit auteur, contrôleciblédiffcheck exit0. Aucun diffARTIST ; diffsconcurrents horsscope préservés. Propriétaire de tranche demeure pool-8 ; code/GREEN interdit jusqu'au RED réellement exécuté, lead runner séparé. Native merge/arrêt final seulement une fois le vrai livrable intégré et tâche traitée, pas fausse complétion pour éviter le verrou.

## Relecture de sécurité avant contrôle possédé

CLI48/racineabsolue distincte nonpréexistante ; acquisitionexclusive CreateDirectoryA et CREATE_NEW, quatre vecteurs attendus indépendants complets incluantNUL, ReadFilehérité inchangé8192/unelecture. Pas de courtcircuit des quatre comparaisons. Supprime seulement noms acquis puis racine acquise, aucun parcours/récursion/suppressionvoisins. Parent de fixtures doit être fiable/possédé/sansmodificateurconcurrent ; runner le réserve sous nouveauoutput. Issues20setup/30cleanup distinctes du rouge1. Relecture lead non indépendante ; revue finale des changements ownership/contrôles prévue sur candidatGREEN intégré, pas verdict de revueindépendante inventé.

## Préconditions et sélection avant RUN

Niveau : contrôles autonomes CPU/Win32, pas GoogleTest/CTest/GL/ASan, conformément44/46. Aucun candidat moteur changé ; campagnes normales/ASan acquises nonrejouées pour ce probe. Charger artist-build-validation et réutiliser toolchain/wrapper54, aucune installation ou CMake/Conanconfigure.

Réserver **out/qa-f8/stdio-red-20261026-01** neuf (STOPsi existe, ne pas écraser), logs wrapper tag **qa-f8-stdio-red-20261026-01-build-controls** etrun **qa-f8-stdio-red-20261026-01-controls**, tous absentsavantécriture. Sentinel voisin possédé hors owned-controls, octets/hashavant/après ; seul controls acquiert puis supprime owned-controls. Un seulbuild dans cette sortie et aucun contexteGPU/bailGL nécessaire.

Manifeste AVANTbuild : HEADintégré/delta source, SHA sources/header/note/compiler/entrycompile_commands/wrappers/env, argvCLintégral, argvRUNintégral, sélectiontailles0/4096/8192/12325, vecteur(i*37+11)%256 etSHApourchaqueattendu, cwd/envwrapper, setup/cleanup/oracles. LigneCL dérivée viaCommandLineToArgvW de l'uniqueTraceableExceptionTests.cpp actuel, conserve protectionsDebug etincludes ; remplace seulement -c,/Fo,/Fd/source pour linker controls.exe avecobj/PDBdanssortie neuve. Aucunfichierbuildprincipal écrasé. Les argv exacts de ce manifeste sont approuvés par lead avant invocation ; forme de dérivation seule n'est pas commande exécutée.

RUNaprèsbuildexit0/binaireidentifié : controls.exe --case native-read-whole --root CHEMIN_ABSOLU_NEUF/owned-controls, viawrapperexistant dont stdout/stderr fusionnés logcomplet. Pas codeprotectionsclose/enfantdanscecontrôle ; timeout1200 duwrapper n'est pas adopté pour futur RUNcloseparent, questiondistincte55. Contrôleexécuté unefois, aucune retryidentique.

## Oracle RED causal et contrôles post-run

4lignes de cas exactement, aucunskip :0/4096/8192PASS tailles+octetsexacts,12325FAILreçu8192avecReadFileBOOL1/error0 et comparaisoncomplète false, processexit1. Setupcréation/écriture/flush/close doivent réussir ; cleanup dequatre fichiers/racine réussit ; sousracineabsente, sentinelle/parentsortieintacts. Sinon pasREDattendu : FAILexpérience/INCONCLUSIVEprécondition, aucune assertionaffaiblie ou correctionspéculative. Logscomplets/horodatages/codesdiagnosticsaprèsdernièreligne, hashesavant/après. Build/linkcassé ou0test n'est pasRED.

Aprèspreuve : mêmeauteur adaptehelper minimumlecturecomplete/EOF/erreur, comparator/attenduscontrolsimmuables, puis mêmescontrôlesGREEN/nonrégression et revuesensiblerefresh. Pas d'autorisation implicite d'écrireprobeCRT ou de corrigerReader, pas de nouveauPASSglobal.
