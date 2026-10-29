# Paquet neutre — revue fresh supervision et close probe

2026-10-26. Candidat intégré **56296fedd759c6ab6a7be018b87260b0f7cb1fa5**. Profil correctness-reviewer, nouveau contexte natif **fresh**, aucune histoire auteur/ancienne revue. Admission : lire ce paquet, confirmer lecture/profil/contexte/localisation/transport puis attendre GOlead de fond. Source canoniqueARTIST explicitement disponible ; aucun accès externe requis. Readonly, pas édition/artefact/shell/build/test/API/delegation/claim/board/handoff. Refusunique précis, pasbootstrap/grant.

## Contrat neutre

ProgrammeWin32privé d'observation, pas moteur/Reader/GPU. Un parent acquiert rootnouveau/lease/fixture4608bytesASCII `A+i%26`, lance nominal puis enfantprotégé distinct ; stopavantautrebras si nominal/setup/identitéinvalide. Sous-modechildinterne n'est pas sécurité ; rootparentsfiables/possédés/sansmodificateurconcurrent, pas adversaire. CLIstrict/rootabsoluneuf/tokenASCII1..64, bornesroot512UTF16/module1024/log128KiB/record8192, pastroncature. Cleanup uniquementnomsacquis/fixtureaprèsfin, logs/rootconservés, voisinsintacts.

Child : FILE*lectureseule valide, lecture complète+EOFdistinguéerreur, fd/handlenatif seulement emprunt pourinstrumentation avantuniquefclose. ProtectionOS délibérée admise uniquementdanscetoutil, paspannestockage. Tentativeengagée≠consommationprouvéesurexception ; aucunefermeturemanuelleemprunt/déprotection/retry/_close/handlerSEH quiinventeraitretour. Primairepréservée/cleanupsecondaire ; toutes sortiesaprèsacquisitiondevraientaboutiràuniquetentative. Adressefcloseeffectivementappelée/module/base/chemin pourrecoupementimports/version/SHAlead, thunkambigu=identitéabsente. GLEaprèsCRTcontextuel,pastracesyscallinterne. Enfantterminenormalement, OSrécupèreressources ; DLLdetachpeutbloquer.

Parent : owneruniquehProcess/hThread acquisimmédiatement aprèsspawn, superviseuravantacquisition ; aucuneallocationjetanteperdantowner. Seul **WaitForSingleObject sur mêmehProcess retournantWAIT_OBJECT_0**, via can_reap_process, permet collecteGetExitCodeProcess/logsfinaux/CloseHandleprocess/cleanup. Troisportes : attente10000ms, attenteINFINITEaprèstimeout, destructionunwind. TimeoutmarqueHOLD/bailvivant,pasbras suivant/cleanupanticipé ; finultérieurecode40sansPASSrétroactif. WAIT_FAILED/inattendu captureGLEimmédiat,marker/identités/handlesretenus,SleepINFINITEquiescentsansretryloop/kill. Markeréchoué n'autorise pasabandon. CatchC++/garde ne doiventpaslaisserchildnonsignalé sanssupervision. Crashfatal/kill/hôte horsgarantie, pasnouveauservice/TerminateProcess.

Phases/seq/runid/PIDctime, lecture/flag/entrée+retourfclose persistés et résultatsimmédiats, logs incomplets ne prouvent pasobservationcomplète. Codes20usage/setup/identité/nominal,30instrumentation/cleanup,40retard,10cibleexception/nonconclusive/fclose0protégé,0seulementnominalconforme+protectionconfirmée+fcloseEOF+fin/cleanupvalides. Champsindividuelsobservation_complete/protected_fclose_return/refusal_confirmed_at_crt_boundary/cleanup_complete/reader_proven=false, pasPASSReader/catch/nonpublication.

## Sources autorisées et identité

- scripts/probes/qa-f8-close/close_probe.cpp (452l), blob39f5b311f820be2506877887450f6dfad39f3223.
- controls.ps1(36l), blob69ec076fce825e5332a5501363f523d791f81caf, mêmesousarbre.
- wait_policy.hpp(18l), blobb119275c1908b5777ab1c99e2d4a7161d46ec92e, deuxTU wait_policy_baseline.cpp/wait_policy_contract.cpp courts siutile.

Ne pas lire PC1-PROTOCOL.md/PARENT-ORACLES.md, rapports/justificationsauteurs, boardimplementation ouremisesautresreviewers. Ce paquetfournitcontratnécessaire, pas besoin de QMD/ContextModeCLI/rechercheexterne. IndexSerenacanoniquepeutnepasvoirnouveaufichier, lectures/grepexacts locaux bornés suffisent ; pasréactivation.

## Preuves disponibles et attendus

Lead a comparé les blobs intégrés aucommit6a6af844auteur ; policy/deuxTU inchangées. Anciencontratstatique : baselineSDK0 etcontratRED4C2131définitionabsente, puis2TUGREEN0avec4assertionspolicyidentiques ; **paspreuveparent/runtime**. Aucunecompilationparent/parsePS/CLI/runcloseauprésentpaquet. GAPREDcomportementalparentexplicitementacceptéopérateur pourceprobeobservatoire, pasTDDcausalglobal accompli. Testsoraclesetcaractérisationsdoiventresterfidèles, toutfuturcorrectif gardeREDcausal.

## Livrable

Findings prouvés avecfichier:ligne/symbole, scénario discriminant compatiblepérimètre, impact/attenduobservé et expérience minimale proposée, pasexécutée. Priorité ownership/nonabandon/synchronisation/observabilité/codes/cleanup/contrôles ; distinguer risqueactuel, preuveabsente, suggestionhorsscope. Pasdequota/findingsspéculatifs oucouvertureuniverselle. Revue statiqueneprouvepaswaitdéfaillant/exceptionnative/retourCRT. Fragments<3500octetsUTF8séquentiels puisFIN_REVIEW_CLOSE_PARENT, leadpersiste/complèteaprèsintégration. Aucun test exécuté par reviewer.
