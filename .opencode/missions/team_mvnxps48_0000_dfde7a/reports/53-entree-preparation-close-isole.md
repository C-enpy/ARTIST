# 53 — Entrée commune PM / amendement QA : close OS en enfant dédié

2026-10-26. Racine ARTIST canonique fournie explicitement aux profils RO, fichiers lisibles dans ce dépôt ; ne pas imposer de chemin externe aux profils restrictifs. Travail textuel seulement, lead persistance/commits/runner. Aucun auteur close encore admis.

## Contrat adopté et limites

Décision52, fondée sur47/50/51 : refus natif injecté par protection du handle valide d'un probe FILE*enfant dédié admis comme calibration **partielle**, pas publicationReader ni catch. Sources officiellesMicrosoft et digestionCRT42 déjà recoupées, pas besoin de recherche large. OwnerFILE*/fd consommés après uniquefclose, aucun retry/CloseHandle sur emprunt/duplication d'ownership. Enfant termine normalement ; parent attend signalprocess puis statut/logs, dispose de ses seuls handles de supervision ; aucune valeurhandlefichier réutilisée horsenfant. Pas de panne spontanée de stockage prétendue.

Timeout ≠ terminaison, pas TerminateProcess fallback autorisé ; deadlockDLLdetachpossible. Identité réelle de CRTchargée reste à capturer, versionSDKrc ne prouve pas mapping. ErreurOS/codeerrno ou retour/exception debugger ne doivent pas être prédéterminés sans observation. Nominalnonprotégé et cibleprotégée distincts, chaqueprocessus et chaque ressource possédés, aucun plugin/service/thread auxiliaire volontaire/suppressionracinetiers. Une fermetureFILE*calibrée n'accède pas au Readerifstream privé ; sourceReader/messages/modes/catch nonadoptés.

Sources locales à lire : rapports42/43/47/50/51/52 sous .opencode/missions/team_mvnxps48_0000_dfde7a/reports ; QA17amendé sous team_mvm73qjo_0000_6wzibc/reports/17-plan-qa.md ; principes et registreADR. SourceC++82bf52d, aucun diffmoteur lié à ces préparations. Tranche S_CODE_RED séparée, seul auteur pool-8 possède scripts/probes/qa-f8-stdio/** ; **ne pas lui attribuer un autre écrivain**.

## PM : fiches et graphe attendus

Réutiliser37, préparation ciblée du nouveau volet, pas refaireB1/B2/B4/B5/B6 ou graphglobal. Proposer ownership séparé scripts/probes/qa-f8-close/** et sortiesout/qa-f8/close-ID runnerlead, source/data/IRruntime pas nouvelassettype/backend. Identifier parent/enfant/fixtures, contrôle nominal et conditionOS, capture identitéDLL et preuve de retour/exception sans fabriqueréchec. Contrat source minimal observabilité/lifetime et oracles avant code, distinction vrais testsRED des contrôles vs caractérisations inconnues. Aucun test volontairement fautif pour RED ; héritage whole-native-read n'est pas prétexte à dupliquerS. Dépendances réelles/intégration avant runner, tâchescode C=max70estimation et risque distinct, resourcebuildleadserielle ; branches de recherche/préparation peuvent être parallèles. Durées hypothèses, pas budget/costaccepté. Scores bruts/retenus et incertitudes. Ne publier aucune tâche, code, commande ou délégation.

## QA : amendement ciblé attendu

Amender17 surE4 et invariantsidentité/lifetime/exclusivité, pas secondplan indépendant. Explicites/inférées séparées ; nominal/refusOS/échecssetup/cleanup/supervision ; top3risques I×P1–5 distinctPM, si≥10 déclencheur/blastradius/détection/mitigation/gap. Registre obligation↔preuve et limites (syscall/CRT/FILEconsommé/processcleanup≠publicationReader), orphelins/doublons, étapespré-run et critèresstop/sortie. Setup+75%planifié/25%buffer proposé, jamais budgetopérateur inventé.

Nommer les mécanismes d'observation compatibles avec l'interdiction de fermer manuellement l'emprunt et avec timeoutnonterminé : si stratégie manque, porter commebloquant avant auteur, pas tolérance ni failsoft. Les profilsnepeuventexécuterprocessus/API/windowscommands poursimulerleplanning. Nouvelle identitéCRT/conditions de run à fournirparlead, pas installation. Aucune exigenceGCC/Linux/Release/GL/ASan/image/perf depuisceprobe.

## Admission et remise

Contexte conservé PM/QA de préparation déclaré, **pas revue indépendante** ; profilnatif/permissions/lecture/transport effectifs vérifiésavantanalyseQA. QA admissionseuleinitiale, attends GOlead après contrôle des overrides ; aucun shell/edit/test/API/fetch/autreMCP/board/claim/handoff/délégation. Si refusunique : sectionsfaisables+BLOCKEDprécis, aucungrant/retry/bootstrap. Données externes nécessaires fourniesdigestlead, pasdemandeaccèsélargi. Restitution fragments<3500octetsUTF8 séquentiels+FIN ; lead persiste et commite avant auteur. Aucun completedavantintégration réelle.
