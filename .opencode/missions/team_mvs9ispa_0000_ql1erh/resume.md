# Reprise de mission — QA Linux ARTIST

## Délégation long run mise à jour le 2026-10-28

Source : décision utilisateur transmise depuis la session de maintenance `longrun-autonomy-default-2026-10-28`, applicable à cette mission déjà approuvée, sans création de mission.

Conduire la mission de bout en bout sans question de routine ni GO intermédiaire. Les décisions techniques et validations opérationnelles nécessaires au mandat sont déléguées par défaut ; une étape nécessaire n'a pas à avoir été énumérée individuellement. Les rapports informent, ils ne demandent pas implicitement une autorisation. La catégorie d'une opération ou la ratification d'un contrat ne crée pas à elle seule une porte d'approbation.

Rechercher, tester lorsque la phase le permet et arbitrer à partir des preuves, instructions et hypothèses raisonnables. N'escalader qu'un dilemme réellement conséquent qui ne peut être résolu raisonnablement, une autorité ou un accès indispensable réellement manquant, ou une contradiction obligatoire ne laissant aucune voie conforme. Préciser alors la décision ou le droit exact ; continuer les travaux indépendants permis.

Cette décision ne change ni objectif, phase, restrictions explicites, modèles, QA ou permissions natives. Ne contourner aucun refus, pause ou contrôle de sécurité. Ne modifier ni révisions approuvées ni politique modèles/QA, ne pas invoquer config/start au nom de l'utilisateur. Une mission documentaire reste documentaire. Les limites explicites d'ARTIST sur les changements d'architecture hors mandat, les branches distantes et les opérations système restent applicables.

La politique native révision1 et son hash approuvé restent inchangés. Cette note précise la délégation opérationnelle ; ce n'est pas une nouvelle révision de politique. Transmettre cette décision aux workers actifs par leur canal permis et aux futurs auteurs dans leur brief. Aucun nouvel agent n'est requis pour cette mise à jour.

## État à reprendre

- Objectif : obligations QA Windows rattrapées sous Ubuntu WSL2 dédié sur I:, puis CI Linux propre ; outils existants mutualisés.
- Dernier HEAD lead avant cette note : `a41b8c0866f115b37e1c9532c89141532ee14888`. Sources exécutables encore celles de `a4c0f82` ; ajouts depuis essentiellement documentaires. Diff préexistant profils/harness/docs hors ownership à préserver.
- WSL `ARTIST-Ubuntu`, disque `I:/WSL/ARTIST-Ubuntu/ext4.vhdx`, utilisateur artist, sources/builds ext4. Aucun arrêt ou changement de docker-desktop autorisé.
- Windows au candidat a4c0f82 :581/581 normal et581/581ASan,506CPU+75GL, sans skip.
- Linux WSL puis CI propre : mêmes581cas normal/ASan,506CPU réussis et72/75GL, trois failures Attribute sans skip. Parité globale **FAIL**, pas simple INCONCLUSIVE ; canaris0/86 bornés, fuites non qualifiées.
- CI réelle40741589259, branche `mission/linux-qa-parity`, candidat84bacef : deux jobs en échec sur CTestOpenGL8, artefacts téléchargés/recoupés. Ne pas répéter ce run sans entrée ou hypothèse nouvelle.
- Sonde Mesa finalisée/copée dans `out/logs/mission-linux-qa/attribute-name-probe/` : legacy ne distingue pas scalaire/array1 ; requête location[0] invalidée ; nom moderne discriminant, disponible sur Mesa4.5. Aucun correctif de production appliqué.
- Contrat de correction compatible adopté/commité38d03b7 : `.opencode/evidence/mission-linux-qa/attribute-correction-contract.md`. Modernité conditionnelle utilisée seulement pour isGLArray, clés natives legacy exactes, API inchangée, fallback historique borné ; réponse moderne incohérente fait échouer le staging. Pas de nouvelle provenance source/cook ni minimum global4.3.
- Avis architecte accepté ; complément QA réellement récupéré via API native après défaut d'autorestitution, relu et commitéa41b8c0 dans `.opencode/evidence/mission-linux-qa/qa-plan.md`. Tâches architecte et plan QA clôturées après intégration, pas d'auteur C++ admis à cet instant.

## Prochaine action autorisée

Le producteur workflow/helpers pool-5 doit rendre son livrable et confirmer la libération des compilations/contextes GPU Linux. Le lead a demandé ce transfert de bail ; ce n'est pas un GO utilisateur. Son helper corrigé conserve le FAIL et les comptesGL, les originaux INCONCLUSIVE restant intacts. Le workflow/helper n'est pas encore intégré au lead.

Après préconditions : publier une seule tranche auteur OpenGL, complexité84/risque76 PM/LLM, contexte et ownership explicites, sur contrat et planQA intégrés. Fichiers réservés : Reader, testsReader, `ARTIST/tests/mock/opengl/glFunctionMock.hpp`, testscomposantsAttribute ; pas workflow/helpers. CycleREDcausal sur classification array1 sous nomlegacy indépendant, GREENminimal puis non-régression. Pas de garantie nouvelle de rollbackCPU dans IAttribute::set.

Cette tranche est maintenant préparée nativement sous `task_mvsggywg_0028_23u2s9`, **deferred et non admise**, avec les vrais IDs des producteurs architecte et QA déjà intégrés. Son texte donne HEAD/diff, ownership, invariants, liberté locale, oraclesRED/GREEN, commandes/outils possibles, essais antérieurs, limites et scores84/76. Publier cette tâche existante après transfert du bail, ne pas en créer un doublon. Aucun nouveau worker n'a été lancé pour la mise à jour de délégation.

Parallélisme : remise/intégration workflow/helper et préparation/écriture auteur peuvent avancer sans conflit de fichiers ; sérialiser le bailGPU et les compilations d'un même arbre. Les consommateurs ne reçoivent une branche qu'après intégration et relecture du lead. Le QA generator ne remplace pas le RED auteur. Revue sensible ciblée et test-reviewer fresh hors board à qualifier effectivement avant admission ; ne pas fabriquer l'audit ou ouvrir des droits si cette voie manque.

## Limites et conservation

OpenGL seul backend implémenté. StatutsADR inchangés :0001/2/4/6/7Proposed,0003/5Acceptedpartial,0008WindowsDebugseulement. PasGPUmatérielLinux, Release, UBSan, performance ou rechercheexhaustivedefuites dans cette livraison. Un pilote3.3 sans route moderne à nom ambigu reste un gap explicite de qualification, jamais skip/PASS implicite.

Conserver les archives rouges, les identités et les limites de récupération des remises. Les notifications retardées peuvent rendre « No unread messages » ; ne pas relancer les workers arrêtés ni répéter les cycles shutdown/merge qui ont déjà laissé des worktrees préservés. Les anciens worktrees sont conservés sans suppression forcée.

Rapports18–20 : contrat, preuveCI et complémentQA ; rapports14–17 : normes, campagnes locales et sonde. Artefacts volumineux sous `out/logs/mission-linux-qa/`, analyses via Context Mode. La présente note doit accompagner toute reprise ou compaction.
