# 24 — Cause du blocage et solutions à décider

2026-10-25. Demande opérateur : enquêter sur la dette et proposer les solutions avant action. Diagnostic lecture seule ; aucun changement de politique/source/runtime/base interne ni nouvelle complétion effectués.

## Objectif et état courant

Terminer réellement le flux approuvé, sans doublon d'audit, faux completed ou nouvelle campagne. `team_status` actuel : approved v2, long_run/running, classifier manual/calls0 ;7tâches,4completed,1in_progress,2blocked ; quatre membres idle. Audit dédié réellement intégré/completed, documentation et ingestion publiées. Les dettes F8/C2/B1/GCC/harness sont distinctes du blocage de statut.

## Causes établies et responsabilité

1. Le graphe importé contenait ASAN→AUDIT→CLOTURE. Le lead a réalisé/intégré la campagne, mais a laissé ASAN in_progress pour éviter l'auto-admission d'un audit dont l'ancien routage était inadéquat.
2. Après adaptation opérateur approuvée, l'audit a été livré sur une **nouvelle tâche dédiée**, sans remplacement des dépendances de l'ancien AUDIT. Il existe donc un livrable valide et un ancien nœud encore bloquant ; ce ne sont pas deux audits à refaire.
3. Le lead a tenté de compléter CLOTURE avant ses dépendances : refus natif exact `blocked by unresolved dependencies`. Ce refus est cohérent avec le graphe, pas une preuve de panne moteur.
4. Le lead a traité l'absence d'outil retarget/supersede comme une impasse et s'est arrêté trop tôt. Une voie par le verrou d'admission n'avait pas été examinée. Cette lacune relève de la coordination ; aucun nouveau bug runtime n'est démontré.

## Nouvelle lecture discriminante du runtime installé

Fichier installé observé : C:/Users/djden/.config/opencode/local-plugins/ensemble/20261025-pm-review-0f9902409237/index.js.

- openMissionConfiguration13694–13714 : le config natif retire approved_hash sans modifier le catalogue si le brouillon est inchangé.
- assertMissionAdmission13812–13821 : absence d'approbation bloque les nouvelles admissions.
- pumpTeam15332–15340 : vérifie ce verrou **avant** dispatch/scaling.
- executeTeamTasksComplete16022–16091 : refuse une tâche blocked, mais ne requiert pas une politique approuvée pour acter un résultat déjà intégré ; complète le producteur, débloque ses dépendants, puis appelle pumpTeam.

**Déduction à vérifier en exécution, pas encore une preuve runtime de ce chemin :** ouvrir config sans changer la politique doit permettre de réconcilier ASAN puis ancien AUDIT puis CLOTURE, tout en bloquant leur auto-admission. Les trois livrables sont déjà intégrés ; leur completion ne serait pas fictive. Le check préalable sera le statut natif draft/nonapproved et l'absence de worker actif ; si une précondition diverge, arrêt des mutations et restitution précise. Aucun start à la place de l'utilisateur.

## Solutions et compromis

**A — Réconciliation sous verrou natif (recommandée).** Ouvrir config sans changer le JSON/catalogue/QA/long_run ; confirmer le verrou effectif. Puis compléter ASAN avec sa preuve, l'ancien AUDIT avec le rapport indépendant déjà intégré et l'adaptation approuvée, puis CLOTURE avec le bilan publié. Contrôler7/7completed et aucun audit doublon. Présenter la même politique ; l'utilisateur reste seul à demander start pour réapprouver ensuite s'il veut restituer l'état approuvé. Aucun test/source/runtime nouveau ; inconvénient : passage transitoire au brouillon et réapprobation native utilisateur. Ce n'est pas une levée de pause opérateur par le lead.

**B — Correctif durable de supersession du runtime.** Nouvelle tranche explicitement autorisée, propriétaire harness/build, contrat et RED causal sur ancien graphe/nouveau livrable, opération native atomique de remplacement/retarget, tests négatifs empêchant fausse completion et double admission, GREEN/revue indépendante. Plus coûteux et hors correction exécutable approuvée actuellement ; utile seulement si ce cas doit devenir une capacité récurrente. Pas de mutation directe de base ni de correctif global improvisé.

**C — Conserver le blocage documenté.** Aucune mutation ; livrables restent disponibles mais mission native non close. Ce n'est pas une terminaison de mission, et ce n'est pas la recommandation.

Interdits inchangés : désactiver globalement autoDispatch/autoScale, marquer completed une dette non livrée, relancer l'audit/generator, faire une course de completions avec dispatch actif, modifier directement la base interne, étendre Reader/F8/GCC/Linux ou réparer le harness sans contrat.

## Décision demandée

Autoriser A (verrou natif et réconciliation des seuls résultats intégrés), ou choisir B (préparer le correctif durable avant toute implémentation), ou C (laisser la mission bloquée). Les dettes techniques F8/C2/B1/GCC/harness ne seraient fermées par aucune simple opération de statut.

## Suite vérifiée

L'utilisateur a choisi A. Exécution native réussie :7/7completed, aucun doublon et politique fichier inchangée. Preuves, commandes, état conservé de l'équipe et limites au rapport25-reconciliation-native-verifiee.md. Les hypothèses de ce diagnostic restent datées ; ne plus présenter le blocage comme actif après cette résolution.
