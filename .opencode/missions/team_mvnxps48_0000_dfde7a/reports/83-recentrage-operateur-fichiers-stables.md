# 83 — Recentrage opérateur : fichiers stables, portable, fin de l'exploration

2026-10-26. Nouvelle demande explicite : chargement shader au cas normal **fichier stable pendant sa lecture, sans écriture concurrente** ; solution portable, contrôles d'erreur, contenu accepté seulement après réussite. **Pas de synchronisation ajoutée pour garantir un scénario concurrent hors contrat.** Inventorier puis retirer méthodiquement surcouches/instrumentation/auxiliaires/supervision/tests devenus sans objet, préserver correctifs utiles/régressions pertinentes/preuves historiques/diffs étrangers ; vérifier consommateurs avant suppression. Valider nettoyage et correctionciblée puis reprendre mission normale, sans prolonger l'expérimental.

## Arrêt effectif des admissions/essais expérimentaux

Instructions HOLD envoyées aux propriétairespool8/pool9 et revuepool12 : aucun nouveaucode/build/test/probe/flag/processus/rechercheexpérimentale, pasd'autonouvelleaffectation. Messagesstockés pour workersdéjàinactifs sanslesréveillerinutilement. Aucune complétionfictive/shutdownlibérantPC1globale ni cleanupworktrees/branches/archives. La demandeopérateur remplace la poursuite75/76, ce n'est pas une reprise d'une pause antérieure.

**Manifeste stdio préparé mais NON exécuté** : out/qa-f8/stdio-cal-20261026-01/candidate-before-run.json, SHAd073c6be20c7947c417c35dc28ad290d665607018f859c1c9d1df78b198104c7. Trois argv sélectionnées, zéroinvocation. Outputcontientbinaire/logsbuild/sentinelle etmanifestes, aucunm0/m1runtime-result oufixtureprobecréée. Close : compile/linkseulement, pasmanifestRUNniinvocation. Le leadn'a lancé aucundesprobes ; auteursrapportentaucuneexpérience. Pasenfantclose/bailOSàterminer, aucun kill/cleanupglobal nécessaire.

## État vérifié

HEAD **db36204047a7f995271752d8335d7a80b4baeac3**, pasdiffARTIST. Sources moteur82bf52d etcorrectifsacquis restentrepères, pas preuve futureaprèsnettoyage. Nouveauxprobesintégrés56296feclose/d03f996stdio, buildsexit0; closewarningC4244historiqueconservé. HelperRED/GREEN etpolicycompileRED/GREEN/revues restent preuveshistoriques deleurspérimètres, jamaisvalidationReaderclose/catch ou problèmeconcurrentcontractuel.

Diffsétrangers observés : .gitignore/.grepai, agentsorchestrator/QA, evidenceQA/scriptsvalidate/testsagentic, AGENTS.md ; renameGrepAI seulstaged. **Aucuneannexion/édition** deceszonespourcettenettoyage sansownershipdémontré. Worktrees/branches/out/docs/_meta/reports conservés par défaut, paspurge/gitreset/revertglobal. Premier scriptd'observationavaituneaccoladeenexcès,corrigéavantlectureeffective ; paspanneharness/rerunbuild.

## Nouvelle boucle bornée

PréparationRO en parallèle : PM inventaire/consommateurs/ownership/graphe/scores/validation ; architecte contratportablelecture/EOF/erreur/acceptation/fermeture au casstable et dispositiondessurcouches. Aucune nouvelleexpérienceOS ou recherchearchives. Les analyses doivent séparer REMOVE/KEEP/HISTORICAL/FOREIGN/OPEN avecpreuvesderefs ; codeettestsReaderseulementaprèscontrat/adoption/QAcommis etTDDpertinent (l'écart75 desoutilsretirés ne dispensepas correctionmoteur).

Succès futur : productionportablecohérenteaucontratstable,surcouchesretiréesaprèsrefs,testsnominal/négatif/publicationutilepréservés/corrigés etexécutés, build/testsnettoyage + nonrégressionWindowsDebug selonrisque etASanprotocoleexistant sipertinent, preuvehonnêtepérimètre/comptes/limites. PasclaimGCC/Linux/Release/GPUimage/perf,nizérotestPASS. ÉvaluationPM/LLM etplancherdéveloppement70/routageexistants inchangés ; pasnouveau modèle/provider/dépendance/backend.

## Préparations effectivement admises, aucune mutation source

Entrées83/84 commitées **45d1884c51233a5985c771d141ee64293cd736d5** avant publication. P_STABLE_CLEANUP task_mvplhnrc_003i_i89r5z autoaffectée pool7/project-manager (analyse64/72), A_STABLE_READER task_mvplhnrc_003j_9e6doh à nouveaupool13/engine-architect (analyse66/74). ScoresRO leadPM/LLM validés, pasréévaluationJEV oumodèlemuté. GETsessions confirmeprofils/canoniqueARTIST/parentlead ; architectecontexteneufsanshistoireauteur, PMcontextepréparationconservénonindépendant déclaré. GOanalysesRO transmis,aucuncode/build/test/expérienceadmis. Deuxlecturesindépendantes enparallèle, pas deux écrivains.

QASTABLE/auteurs/nettoyage/tests serontconsommateurs du contrat/inventaire réellementintégrés, pas publiés prématurément ; QAplancommisavantauthor, TDDmoteurcontratstableetrevueindépendanteselonrisque. Anciennesaffectationsprobes conservéesinactives, aucunecompletedfictive pourlesarrêter. DéclarationNOT_RUN écrite danschacunedesdeuxarchivescalibration sansmodifiermanifestes/logs acquis ; donnéeshorsGitignoréesconservées, reportversionnéfournitstatutportable.
