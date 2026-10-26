# 60 — PC0 : contrat close minimal adopté, étape initiale bornée

2026-10-26. Décision52, PM55 et QA17amendementclose intégrésc77fb5a ; transport59 vérifié sur processusinoffensif3s. Ce contrat concerne seulement le probeclose, **pas Reader/catch/nonpublication**, aucun résultat close encore exécuté.

## Ownership et isolement

Auteurcpp fresh dans worktree neuf après ce commit et QA17, admissionréelle avant édition ; sourceunique scripts/probes/qa-f8-close/**. Pool8 garde seul stdio. Aucun moteur/CMake/test moteur/sharedhelper/install/service/dépendance/tracingnouveau. Source/protocoles intégrés avant runnerlead ; unbuildparsortie, compilation/runstdiosérialisésaveclose, auteursparallèles possibles seulementfichiersdisjoints. PMPC1 **brut79/retenu79/R84** validés, pas changement de modèle/provider/effort imposé. Les étapesPC2/PC3/PC4 attendront leurs entrées.

## Transport et conduite adoptés, sans garantie impossible

Buildcl peut utiliserwrapperexistant. **RUN ne passe pas par run-build.py(timeout1200)**. Le lead utilisera functions.shell **background=true,timeout=0**, dans son rootpossédé, pour lancer le futur parentprobe (commandeintégrale à figer après source). Référence59 : retourbackground puis END3s observés sur commandebénigne, pas persistanceuniverselle/défaillancehôte démontrées.

Parentprobe garde ses propres handlesnatifsprocessus et bail/PID+instantcréation/bras/logs au-delà de toutdélai. Première attenteProcessHandlebornée **10000ms**, pas ThreadHandle ; WAIT_OBJECT_0 autorisecollectestatuts/cleanup, WAIT_TIMEOUT non. Surtimeout : markerpersistant unique **HOLD_CHILD_ALIVE**, STOPcampagne/pasbrasprotégési nominaltimeout,pascleanup/pasrerun ; parentrestevivantenattente native du **même processhandle** jusqu'à signal. Aucunbusyloop/service/TerminateProcessfallback ; le lead conserve responsabilité/bail et porte à opérateur toute attenteexceptionnelle. Pas de libérationdebail ou prétention derecoveryavantfin.

WAIT_FAILED/étatinattendu : conserveridentités/bail, marker **HOLD_SUPERVISION_FAILURE**, aucunecollecteexit/cleanup/rerun fondée surcewait. Parent ne doit pas disparaître en abandonnant son enfant ; stratégiequiescente/conservationhandles à préciser par l'auteur **avant codeparent**, toute incapacité est BLOCKED/retourlead. Ni codeextérieur/timeout du transport ni marker ne prouvent fin. Panne/restartservice n'est pas couverteparpréflight : journalPID+instantcréation permetaulead dequalifier l'état, aucune récupérationprésumée. Interventionfuture éventuelle sur processusowned demandera décision distincte, pas fallbackautomatique.

## Contrat fonctionnel futur parent/enfant

Parentnominal puis protégé en enfantsdistincts, mêmes4,608octetsASCIIstables/fopenrblectureseule, lectureexhaustivevérifiéeavantfermeture. Enfant seul ownertargetFILE/fd/handlenatif ; pas transmission/héritage/duplicationhorsenfant. Instrumentation_get_osfhandle etSet/GetHandleInformation seulementpourtarget52, pas deCloseHandletémoin surl'emprunt. Exactementune fclose cible ; résultat/errno/infosdisponiblescapturésavantlogging, aucune codeOS/errno/absenceexception forcés. Aprèsretourconsommant,pasréutilisationFILE/fd/emprunt ; aucunegarde/atexit/destructeurduprobe ne réessaie. Valeursdiagnostic jamais handlesutilisablesparparent.

Identifier moduleeffectivementciblefclose, chemin/version/base/hash (thunk seul insuffisant) et debuggerprésent/absent ; détailsd'observation à proposerpuisvalider avant codeparent. CapturerretourOUexception viafrontière observable existante/statut+phases, sans handlerqui inventerait consommation ; pas nouveauobservateursyscall requis. CloseHandleinterne reste documentaire/inféré sanstrace directe ; pas executed revendiqué. Sourcedll42≠implémentationruntimecertifiée.

Logs fichierspossédés bornés, paspipenondrainé ; corrélationparent/childPID+instantcréation/bras/ressource. Enfantretourmain/finOS ; parentattendsignal puisstatut/logs et nettoiesesulementressourcespropres/fixtureprivée/voisinssentinelles. Filepathownershipdistinctd'ownershiphandle ; aucune suppressionracinetiers/récursion parent. Protectionciblée=fauteOSdélibérée, pas stockage spontanémentdéfectueux. RetourEOF/exitstatusseul n'est pas syscallobservé ou preuvepublicationReader. Nominalnonconforme/setup/exception/timeout/logsincomplets=FAILouINCONCLUSIVEseloncontrat,pasassertionsaffaiblies. Pas deprogrammeparentavantpropositiondétaillée lifetimes/exception/WAIT_FAILED approuvéelead.

## Première tranche source proposée : contrat statique de collecte, pas faux RED

Pour éviterduplicationduhelpernativeRead etunbogusREDOS, proposer **un tout petit prédicat privé réellement utilisé par le futur parent** : il autorise collecte/cleanup **uniquement** sur WAIT_OBJECT_0, jamais WAIT_TIMEOUT/WAIT_FAILED/WAIT_ABANDONED_0. Aucun assettype/template/dispatcher ; structure connuecompilation et donnéeWait runtime.

TDDcompilecontract possible : déclarer nouvelle signatureconstexpr `can_reap_process(DWORD) noexcept` sans corps d'implémentation dansheaderprivé et écrire d'abord4static_assert de cescas. BaselineTU incluantledéclaration sansl'appeler etmacrosSDK doit compiler ; TUcontrat doit être rejetépour **définitionconstexprmanquantedecettenouvellefonction**, pas headerabsent/toolchaincassée ou valeurvolontairementfausse. C'est RED d'une capacitéstatiqueencorenonimplémentée, **pas échec comportemental du parent ni erreurOS**. Après preuvelead, mêmeauteur définitleprédicatminimal et mêmesassertionsGREEN ; pas de mauvaiseimplémentation/comparateur ajouté pourrougir.

Ce design est proposé au spécialisteen **premièrephaseRO** : confirmer pertinence/diagnostic/préconditions ou remonter une limite précise avantsource. Aucun droitde prétendreREDdisponible sans compilationeffectivebaselinepuiscontrat, ni decréerunframeworkjustepourafficherdurouge. Si rejeté/nonpertinent, garderPC1auRO etporter lemanque, pas exemptionTDD implicite.

## Admission et sorties

PC1initial : profilcpp frais/nouveauworktree/GETdirectory+Git/lecture+transport, puis propositionROpolicytest etsupervision. Sourcebaseline/headerdéclaration/testsonly aprèsGOsource distinct ; **pas encore parent/enfant/flags**. Runnerlead exécute testscompilation après vraieintégration etcommandeapprouvée. Partiecalibration inconnue caractérisée honnêtement selon52 ; autrescontrôlesdéterministes à TDDpertinent, pas globalREDrenommé.

CLIparent/enfant/codeslogetligneRUNfinales serontfigés avecproposition avantcodeparent. Les étapesPC1complète/PC2/PC3/PC4serontaffinées surcandidat/protocole réels ; aucune nouvelleexpérimentation débloquéeparcequePC0estcommité. Auditfinalreviewerfresh inchangé, M5Reader/catchrestentGAP.

## Publication/admission réellement observées

PC1_CLOSE task_mvphg6fn_0028_ear29g publiée sur dépendances P_CLOSE/Q_CLOSE réellementintégrées ; nouveau pool-9/cpp-engineer, session ses_e86ea4cc2ffezLKs4XAq2Hdky1, contexte initial limité à son affectation/lectures (pas histoireSTDIO). Nouveauworktree pool9 propre, HEADaddb7cccbc42d040ea25733be06009c5942b9382, contrat60 présent. GETinitialencorecanonique : aucuneadmissioncodeaccordée. Native opencode.session_move vers **ce propreworktree**, puis GETpost confirme directory/profilcpp et modèleSol/xhigh sélectionnéparpolitique79/84, aucunoverride demandé. Lecture .git/60 **relatives** +transport demandés avantGO_ROconception ; ni parent/enfant, ni flags/tests/codeautorisés à ce jalon. Ancien architecte b3-contract a reçu shutdown après étudesintégrées/tâchescomplétées, pas libération d'une sourceactive ni nouvelleenquête doublon.
