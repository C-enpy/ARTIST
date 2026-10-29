# Corriger le séquencement et l'audit — limite native confirmée

## Objectif et constat

La demande est de corriger le départ des consommateurs et la publication de l'audit, pas de modifier le moteur Linux ni de lancer la mission. La recherche dans les outils effectivement exposés et dans le plugin installé n'a pas identifié de commande native permettant de retirer, différer ou masquer les tâches déjà importées.

La correction complète du board n'a donc pas été réalisée. Aucun démarrage, fausse complétion, nouvelle équipe, édition de base interne ou modification du plugin n'a été effectué.

## Preuves vérifiées

- Le catalogue expose `team_tasks_list`, `team_tasks_add`, `team_tasks_complete`, `team_claim` et les outils de membres. La signature d'ajout permet ajout/décisions d'exécution, pas retrait ni suspension d'une tâche existante.
- Dans le bundle du déploiement Ensemble consulté, `parseMissionCommand` reconnaît `prepare`, `config` et `start` ; aucune opération de correction du board n'y est déclarée.
- Les fonctions de gestion de tâches et les références locales consultées ne fournissent pas de voie publique documentée de retrait/différé. Ce constat porte sur cette installation et ces interfaces, pas sur toutes les versions d'Ensemble.
- Un document de conception d'évolution des tâches existe, mais sa présence ne démontre pas une fonctionnalité déployée.

## Conséquences et mesures possibles

La règle de clôture reste applicable : les producteurs ne sont achevés au board qu'après intégration et contrôle par le superviseur. Cela permet de respecter la dépendance réelle sans confondre fin d'auteur et intégration. Cette mesure ne retire pas les consommateurs déjà publiés et ne corrige pas à elle seule la séparation de l'audit.

L'audit exige une préparation indépendante avec contexte et restitution qualifiés. Modifier sa description ou lui donner le contexte fresh ne suffit pas à prouver une invisibilité du board. Ne pas qualifier ce problème de résolu par une promesse de briefing.

## Options

1. Corriger la capacité native de l'outillage, dans un périmètre distinct et autorisé : retrait/différé des tâches non admises avec traitement cohérent des dépendances, puis vérification de l'état réellement obtenu. Ne pas développer cette capacité directement dans la mission Linux sans autorisation. Un éventuel changement de déploiement partagé doit être présenté séparément.
2. Reporter cette correction : conserver la mission non démarrée. Aucun travail Linux perdu, mais le départ attend une voie native conforme.

Recommandation : traiter un correctif natif ciblé plutôt que contourner la base ou recommencer la mission. Cette proposition n'est ni une implémentation ni une estimation de durée garantie.

## Références

- [Revue du plan importé](03-revue-du-plan-importe.md).
- Plugin inspecté en lecture seule : `C:/Users/djden/.config/opencode/local-plugins/ensemble/20261027-preparation-admission-ac2bbe7eb142/index.js`.
- Recherches de catalogue, lectures ciblées et analyse du bundle via Context Mode ; aucun build ni test du plugin exécuté.
