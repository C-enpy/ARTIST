# Reprise de mission — QA Linux ARTIST

## Délégation long run mise à jour le 2026-10-28

Source : décision utilisateur transmise depuis la session de maintenance `longrun-autonomy-default-2026-10-28`, applicable à cette mission déjà approuvée, sans création de mission.

Conduire la mission de bout en bout sans question de routine ni GO intermédiaire. Les décisions techniques et validations opérationnelles nécessaires au mandat sont déléguées par défaut ; une étape nécessaire n'a pas à avoir été énumérée individuellement. Les rapports informent, ils ne demandent pas implicitement une autorisation. La catégorie d'une opération ou la ratification d'un contrat ne crée pas à elle seule une porte d'approbation.

Rechercher, tester lorsque la phase le permet et arbitrer à partir des preuves, instructions et hypothèses raisonnables. N'escalader qu'un dilemme réellement conséquent qui ne peut être résolu raisonnablement, une autorité ou un accès indispensable réellement manquant, ou une contradiction obligatoire ne laissant aucune voie conforme. Préciser alors la décision ou le droit exact ; continuer les travaux indépendants permis.

Cette décision ne change ni objectif, phase, restrictions explicites, modèles, QA ou permissions natives. Ne contourner aucun refus, pause ou contrôle de sécurité. Ne modifier ni révisions approuvées ni politique modèles/QA, ne pas invoquer config/start au nom de l'utilisateur. Une mission documentaire reste documentaire. Les limites explicites d'ARTIST sur les changements d'architecture hors mandat, les branches distantes et les opérations système restent applicables.

La politique native révision1 et son hash approuvé restent inchangés. Cette note précise la délégation opérationnelle ; ce n'est pas une nouvelle révision de politique. Transmettre cette décision aux workers actifs par leur canal permis et aux futurs auteurs dans leur brief. Aucun nouvel agent n'est requis pour cette mise à jour.

## État courant prioritaire — après rapport 24

État prioritaire du 29 octobre : audit final reçu et persisté (`final-test-audit.md`), réserve F1 omission des shaders dans l'identité reproduite en RED puis corrigée à `f58a49f4261e018749c59b0d8aab9dd7e85ba53e`. Test helper 10/10 Windows et Linux, ré-audit indépendant reçu : F1 fermé, aucun nouveau finding, voir `final-test-reaudit.md`. WSL normal/ASan renforcés exécutés code0 (`shader-identity-final-linux/`, deux runs082047/082059), XML/169hashes à recouper. CI renforcée40841460327 en cours ; ne pas lui transférer le succès de40760506988. Runtime C++/shaders/config inchangés depuis78e7b74, Windows589 réutilisable par identité. La documentation finale et PR restent les prochaines actions dès qualification des preuves impactées, sans nouveau GO.

Mise à jour rapport 25 : tests de gaps intégrés à `1532b78d96489923773e7b2a8b355dc91f94345c`, 9/9 Windows + Linux, trois mutants en mémoire détectés ; runtime/config inchangés depuis 78e7b74. Remises du reviewer C++ et generator acceptées/clôturées après persistance réelle. Dossier neutre `final-audit-input.md` préparé. L'utilisateur a autorisé une seule lecture locale GET /api/agent, désormais réussie : profil natif test-reviewer et permissions restrictives vérifiés, snapshot archivé, aucune configuration modifiée. Ne pas refaire cette lecture par rituel.

Audit final lancé en contexte fresh hors board via le profil natif disponible, session `ses_e79498a83ffe3GxE7urS3rUXcG`, background functions.subagent, sans modèle explicite (sélection native de la session). Un essai team_spawn autonome a été refusé avant création car claim_task obligatoire ; ne pas le répéter ni publier l'audit au board pour contourner l'indépendance. Attendre la notification finale, pas de polling. Lecture/restauration effective à recouper sur cette remise. Documentation finale/PR attendent l'audit ; préparation documentaire indépendante en cours, aucune nouvelle campagne moteur nécessaire pour tests Python/docs seuls.

Les sections historiques ci-dessous décrivent les étapes précédentes, pas le statut actuel.

- Candidat moteur final `78e7b74989dd463cddf14080916e03b60405640d`, poussé sans force uniquement sur `mission/linux-qa-attribute-0a4a671`. Aucun push de main.
- Windows, WSL et CI Ubuntu : **six configurations normal/ASan à 589/589 chacune**, 513 CPU + 76 GL, zéro failure/error/skip, populations bijectives. CI réelle `40760506988` success, artefacts téléchargés et recoupés ; 149 hashes sources/config par campagne Linux vérifiés, deux écarts CRLF Windows comparés aux blobs Git exacts.
- Gate GLEW réparé après nouveau RED lead à chaud (513 tests CPU, un seul échec attendu), puis GREEN propre Windows et Linux/CI. Ré-audit indépendant pool-10 reçu : finding fermé, aucun nouveau défaut démontré dans le delta direct. Suspension interne des admissions levée ; aucune pause opérateur ou nouveau GO.
- Auteur pool-9 et producteur pool-5 arrêtés, baux rendus au lead. Ne pas refaire shutdown/merge : nettoyage du worktree pool-9 avait échoué après merge réussi, branches préservées.
- Generator pool-11 : remise publique retrouvée via API native de session, sans relance ou élargissement de permissions. Trois propositions ciblées du contrôleur adoptées dans `gaps-integrated-candidate.md`, à commiter avant code puis exécuter par le lead (seul fichier test_linux_evidence.py). Aucun défaut production supplémentaire proposé. Méthode locale PM/LLM, complexité 70, risque 40.
- Prochaine étape : tests de caractérisation renderer/environnement/identité, vérification de leur détection, intégration puis audit final test-reviewer fresh hors board à qualifier effectivement. Ensuite documentation et PR, sans merge distant.
- Les admissions et restitutions QA doivent rester effectives ; /api/agent a été refusé une fois, ne pas le retenter. API native lecture des messages a permis de récupérer les seules remises publiques. Ne pas confondre défaut de récupération automatique et absence de livrable, ni se substituer au reviewer indépendant.
- Archives : `out/logs/mission-linux-qa/gate-final-reconciliation.json`, `gate-final-linux/`, `ci-40760506988/`, `gate-green-windows-*`, RED et FAIL historiques conservés. Couverture GPU limitée à Mesa llvmpipe/Linux et NVIDIA/Windows ; detect_leaks=0, pas de performance/Release/UBSan/fuites exhaustives.

## Historique — état avant intégration

- Objectif : obligations QA Windows rattrapées sous Ubuntu WSL2 dédié sur I:, puis CI Linux propre ; outils existants mutualisés.
- Dernier HEAD lead d'intégration : `44943016b8b732d843e58b8137044890c03b3ba8`. C++ encore celui de `a4c0f82` ; contrat, plan QA et workflow/helper désormais intégrés. Diff préexistant profils/harness/docs hors ownership à préserver.
- WSL `ARTIST-Ubuntu`, disque `I:/WSL/ARTIST-Ubuntu/ext4.vhdx`, utilisateur artist, sources/builds ext4. Aucun arrêt ou changement de docker-desktop autorisé.
- Windows au candidat a4c0f82 :581/581 normal et581/581ASan,506CPU+75GL, sans skip.
- Linux WSL puis CI propre : mêmes581cas normal/ASan,506CPU réussis et72/75GL, trois failures Attribute sans skip. Parité globale **FAIL**, pas simple INCONCLUSIVE ; canaris0/86 bornés, fuites non qualifiées.
- CI réelle40741589259, branche `mission/linux-qa-parity`, candidat84bacef : deux jobs en échec sur CTestOpenGL8, artefacts téléchargés/recoupés. Ne pas répéter ce run sans entrée ou hypothèse nouvelle.
- Sonde Mesa finalisée/copée dans `out/logs/mission-linux-qa/attribute-name-probe/` : legacy ne distingue pas scalaire/array1 ; requête location[0] invalidée ; nom moderne discriminant, disponible sur Mesa4.5. Aucun correctif de production appliqué.
- Contrat de correction compatible adopté/commité38d03b7 : `.opencode/evidence/mission-linux-qa/attribute-correction-contract.md`. Modernité conditionnelle utilisée seulement pour isGLArray, clés natives legacy exactes, API inchangée, fallback historique borné ; réponse moderne incohérente fait échouer le staging. Pas de nouvelle provenance source/cook ni minimum global4.3.
- Avis architecte accepté ; complément QA réellement récupéré via API native après défaut d'autorestitution, relu et commitéa41b8c0 dans `.opencode/evidence/mission-linux-qa/qa-plan.md`. Tâches architecte et plan QA clôturées après intégration ; auteur C++ admis ensuite, état ci-dessous.

## Prochaine action autorisée

Le producteur workflow/helpers pool-5 a rendu son livrable et confirmé l'arrêt normal de ses compilations/contextes GPU. Le workflow/helper est intégré à4494301,6tests rejoués par le lead sur Windows et Linux, aucune sourceC++ changée. Les originauxINCONCLUSIVE restent intacts ; le reporting corrigé conserveFAIL et comptesGL. La version finale du helper n'est pas encore rejouée en CI distante, contrôle prévu sur le candidat combiné. Pool-5 arrêté, tâche de raccordement clôturée pour son livrable réel, sans annoncer la parité.

La tranche auteur OpenGL est publiée et admise après ces préconditions : `task_mvsggywg_0028_23u2s9`, pool-9/gpu-engineer, contextefresh et worktree isolé, basef09d58a, complexité84/risque76 PM/LLM. Fichiers réservés : Reader, testsReader, `ARTIST/tests/mock/opengl/glFunctionMock.hpp`, testscomposantsAttribute ; pas workflow/helpers. CycleREDcausal sur classification array1 sous nomlegacy indépendant, GREENminimal puis non-régression. Pas de garantie nouvelle de rollbackCPU dans IAttribute::set.

Les baux build/GPU Linux et Windows sont désormais à l'auteur dans ses seuls arbres privés. Les vrais producteurs architecte et QA sont intégrés, son brief contient contexte/contrat/tests/essais/limites/scores. La correction reste en cours ; RED natif recoupé ci-dessous, aucun GREEN encore accepté. Gaps et revue finale restent différés jusqu'à sa livraison intégrée. Voir rapports21–22. Ne pas créer de doublon auteur ni attendre un GO utilisateur. Aucun worker n'a été lancé uniquement pour la mise à jour de délégation.

L'auteur a confirmé lecture du contrat/QA avant toute édition. Sa session initiale voyait le canonique ; refus externe constaté une seule fois, sans mutation. Après transition native implementation/high, session_move l'a placé dans son worktree et la politique Context Mode locale a été lue. Baux effectivement reçus et plan local conforme. Le lead a clarifié l'ancienne mention DEFERRED ; aucun nouveau GO nécessaire. Le RED natif est maintenant acquis ; le GREEN et les contrôles CPU restent à recouper.

Mise à jour RED : le test natif de classification a été exécuté avant le Reader et recoupé par le lead,1cas sélectionné/exécuté en échec,0skip,CTest8,shaderscompilés/liés. La clé native indépendante passe, le scalaire comparable passe, array1/2 ontisGLArrayfalse attendu true. Copies dans `out/logs/mission-linux-qa/attribute-correction-red/`, rapport22. L'auteur annonce aussi unREDCPU compilé, détails encore à recouper. Il poursuitGREENminimal ; aucunGREEN ouPASSglobal accepté et correctif pas encore intégré.

Parallélisme : remise/intégration workflow/helper et préparation/écriture auteur peuvent avancer sans conflit de fichiers ; sérialiser le bailGPU et les compilations d'un même arbre. Les consommateurs ne reçoivent une branche qu'après intégration et relecture du lead. Le QA generator ne remplace pas le RED auteur. Revue sensible ciblée et test-reviewer fresh hors board à qualifier effectivement avant admission ; ne pas fabriquer l'audit ou ouvrir des droits si cette voie manque.

## Limites et conservation

### État courant après intégration et revue

La correction auteur est intégrée à `0a4a671` et ses quatre archives588/588 ont été recoupées, mais n'est PAS acceptée globalement : le reviewer a établi un gateGLEW expérimental non conforme. Les nouvelles admissions sont suspendues, les reviewersRO existants continuent. Le lead prend une réparation locale bornée enTDD, couverte par A2, Reader/mock/tests uniquement, complexité70/risque72 ; ré-audit requis après correction. Ne pas annoncer la parité depuis les seules archives auteur.

Le premier passage Windows intégré a échoué (binaire incrémental : deuxCPP seulement recompilés). À sources identiques, clean de projet puis63CPP reconstruits et588/588réussis. Écart de cache archivé, mécanisme de dépendances non entièrement expliqué. Tous prochains passages de la réparation doivent reconstruire les TU de façon cohérente. Le gate reste un défaut distinct à corriger. Voir rapport23.

Workflow+correction0a4a671 publiés sur branche distante nouvelle `mission/linux-qa-attribute-0a4a671`, sans toucher main. Sa CI ne peut prouver la future réparation du gate. Recherche de gaps pool-11 active enRO sur sources intégrées et ledgerneutre ; revueC++pool-10 terminée avecfinding, ré-audit à demander sans nouveauworker tant que suspension. Le test-reviewer final hors board reste à qualifier après gaps et preuvefinale.

OpenGL seul backend implémenté. StatutsADR inchangés :0001/2/4/6/7Proposed,0003/5Acceptedpartial,0008WindowsDebugseulement. PasGPUmatérielLinux, Release, UBSan, performance ou rechercheexhaustivedefuites dans cette livraison. Un pilote3.3 sans route moderne à nom ambigu reste un gap explicite de qualification, jamais skip/PASS implicite.

Conserver les archives rouges, les identités et les limites de récupération des remises. Les notifications retardées peuvent rendre « No unread messages » ; ne pas relancer les workers arrêtés ni répéter les cycles shutdown/merge qui ont déjà laissé des worktrees préservés. Les anciens worktrees sont conservés sans suppression forcée.

Rapports18–20 : contrat, preuveCI et complémentQA ; rapports14–17 : normes, campagnes locales et sonde. Artefacts volumineux sous `out/logs/mission-linux-qa/`, analyses via Context Mode. La présente note doit accompagner toute reprise ou compaction.
