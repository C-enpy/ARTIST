# 71 — PC1_PARENT_RO complet ; source bloquée par limite TDD explicite

2026-10-26. Cinq fragments et FIN_PC1_PARENT_RO reçus de pool9, sans édition/recherche/commande/build/test/processus/flag depuis GO_RO. Snapshot auteur0e70b002 inchangé attribué au worker ; preuve policyGREEN68 fournie par lead. Cette proposition n'est pas une implémentation ni un résultat runtime. PC1 global reste in_progress.

## Interface proposée, non adoptée

Exécutable privé unique close_probe.cpp/close_probe.exe, utilisant réellement wait_policy.hpp. Parent : --root ABS_NOUVEAU --run-id TOKEN_ASCII, sans option de saut du nominal. Enfants mêmeexe : --internal-child --arm nominal|protected --root ABS --run-id TOKEN --parent-pid DEC --parent-ctime HEX16. Rejet strict arguments manquants/dupliqués/inconnus/relatifs avant acquisition/spawn. Token [A-Za-z0-9_-],1..64 ; root≤512UTF16, cheminDLL≤1024 ; bornes propres au probe, pas aux assets/Reader, troncature rejetée.

Root acquis exclusivementCreateDirectoryW, manifeste/bailCREATE_NEW avant spawn. Fichiers : fixture/target.dat4608ASCIIbyte[i]=A+i%26, neighbor.keep, parent.log, nominal.log, protected.log, lease.txt, result.txt. Écriture/flush exacts, handleparent fermé avantenfant. Logs conservés, cleanup seulementtarget/fixture, pas suppressionrécursive/roottiers ; sentinelle voisine lead possible horsroot.

Codes proposés :0=caractérisation complète+cleanup ;10=inconclusif/exceptioncible ;20=usage/setup/identité/nominalinvalide ;30=cleanupéchec ;40=fin tardiveaprèstimeout. HOLDvivant ne fournit pas de codefinal. **Point à revoir avant adoption** : ciblefclose0 pourrait rendre instrumentationsuccès0 tout en infirmant hypothèserefus ; il faut résultat distinct explicite, ne jamais confondre ce0 avec confirmation de l'hypothèse ou M5couvert. Aucun code n'est ici adopté pour RUN.

## Journal et épilogue proposés

Logs Win32 exclusifs nonhéritables, pas pipes : recordsUTF8TABkey=value, échappement%HH pour%,TAB,CR,LF ; version1/run-id/bras/PID/ctimeparent+child, seqcroissant. Bornes8192octets/record,128KiB/fichier ; débordement/écritureincomplète/flushéchoué invalident l'observation, aucune troncature. Buffers préparés avantacquisition, pas formatteur allouant critique.

Phases enfant : CHILD_START, CRT_IDENTITY, OPEN, READ_COMPLETE(taille+octets), HANDLE_FLAGS_INITIAL, PROTECTION_CONFIRMED, FCLOSE_ENTER(purposecalibration|setup_cleanup), FCLOSE_RETURN(ret/errno/GLEcontextuel), CHILD_MAIN_RETURN. Phases critiques persistées avant suite. Lecturefreadrb exacte4608avecEOF/ferror distincts ; mismatch/excès/erreur=setupinvalide. Protectionseulementaprèslecture ; étatinitialprotection/héritageabsents. Set/readbackéchoué peutlaisserprotectioninconnue : pasdéprotection/fermeturemanuelle.

Fopenéchoué=0fclose ; aprèsacquisitiontoutes sortiesnormales rejoignent uniqueépilogue, capacitéretiréeavanttentative. Tentative engagée≠retourconsommant, aucunretry/gardeaveugle/usageFILEfdemprunt ensuite. Setupcleanup ne devient pas calibration. AucunhandlerSEH/vectored/debuggerinventant consommation ; phasesmanquantes/exceptionstatut réels conservés. CHILD_MAIN_RETURN ne prouve pas finDLLdetach.

## Supervision proposée conforme aux décisions60/64

Ownerpréspawn et stockage/bail/log/commandline préalloués. CreateProcessW bInheritHandlesFALSE ; hProcess/hThread acquis immédiatement sans allocationjetante. hThread distinct ne prouve pas fin. Ctime depuis mêmehProcess ; échecpostspawn=HOLDidentitépartielle sansabandon. Childlogcorrèleidentités.

Une porte commune reçoit le résultat wait surhProcess réel et appelle **can_reap_process** ; seulementtrue metterminal_observed puis permetGetExitCodeProcess/logsfinaux/fermeturehProcess/cleanup. Callers réels : premièrewait10000ms, waitINFINITEaprèsHOLD et gardeunwind. Fauxnefinalisejamais. AucunSTILL_ACTIVE/mainreturn/log/shellnotification remplaçant signal.

Timeout marqueHOLD_CHILD_ALIVE,stopcampagne,pasbras suivant ; attendINFINITEmêmehandle, finultérieurepermetcleanup maiscode40/pasPASSrétroactif. WAIT_FAILED sauveGetLastErrorimmédiat ; inattendu/ABANDONED marqueHOLD_SUPERVISION_FAILURE,conservehandles/lease puisSleepINFINITEquiescent sansretryloop/kill/service. Markeréchoué dégradevisibilité sansthrow abandonnantchild. GardeC++ ne détruit pas supervisionactive : waitINFINITE,puis mêmeporte/HOLD. /EHscnegarantitpasSEH ; crashfatal/kill/hôtearrêté horsgarantie, paswatchdog/TerminateProcess ajouté.

## Identité et commandes proposées

Adresse fclose réellementappelée, GetModuleHandleExFROM_ADDRESS/base/GetModuleFileNameW, debuggerparent+enfant. EXEthunk/forwarder/chemintronqué=identitéabsente etstopavantprotection, pasautreCRTchargée pourfaciliterattribution. Leadrecoupeimports etversion/SHAfichierDLL, nominal/protégé/prépost ; hashfichier≠mémoire/sourceSDKcertifiée. GLEaprèsCRT=contextuel, CloseHandleinterne reste documentaire/inféré.

RUN proposé seulementaprèssourceintégrée/commandesexactes : functions.shell backgroundtrue/timeout0, exeprivé --root output/owned-runabsent --run-id fixé ; **jamais run-build.py pour RUNparent**. COMPILEwrapper/outilsDebugpinnés possible, argv/linkàfiger après source. LeadpréenregistreSHA/source/exe/imports/identité/envdebugger/bail/sentinelles ; signalchild puisstatut/logs/seq/ctime/cleanup etvoisins, codeparentobservable sinonpreuveabsente. Notificationbackgroundneprouvepasexit0. Nominalreadexact/flagabsent/fclose0/finsignalée exigés avantprotégé, sinonSTOP. Cibleobservée sanserrno/codeimposé ; résultatdéfavorable conservé.

## Contrôles déterministes et limite structurante

Proposésavantcode : CLIinvalide/rootpréexistant=>20,zérochild/altération ; nominalcompletavantprotégé ; setup/logincomplet=>pasprotégé ; cleanupuniquementviawaitpolicy+revuecallers ; limiteslogs/sentinelles/aprèsfinsignalée. WAIT_FAILED/deadlock nonprovoqués artificiellement, branchesrevue statique+GAPruntime.

**Aucun RED comportemental causal honnête identifié pour superviseur absent.** Programmeabsent/compilationcassée n'enconstituentpasun ; squelettevolontairementfaux/mockduwait ne seraient pas une preuveOS. PolicycompileRED/GREEN68 n'exemptepasTDDparent. CRTinconnue reste caractérisation52, pasRED parcequefclose refuse.

Worker soumet deux suites, n'en choisit aucune : adoption **explicite** de caractérisations/invariants+revuestatique avec GAPREDcomportemental conservé, ou scénario causal supplémentaire avantcode. Le lead n'accorde aucune exemption implicite : **sourceparentbloquée**, futuredécisionopérateur doit nommer l'écart au contratTDD et sa portée exacte. Tout défaut révélé ensuite garderait vraiRED/oracleinchangé/correctifminimal/GREEN. Pasframework/seamnouveau pourfabriquerrouge.

## État et regroupement des décisions

ContratReader/modes/messages/catch nonadopté, M5Reader/nonpublicationnonprouvés, aucunessaiclose. Remise complète à intégrer, taskglobale noncomplétée. StdioROindépendantencours ; regrouper les arbitrages compatibles après sa remise pour éviter demandes fragmentées. Pas de codeparent ni de nouvellecampagne en attendant ; diffsconcurrents/archives/identitéspréservés.
