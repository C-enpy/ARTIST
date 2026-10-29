# 03 — Arbitrages après préparation PM bloquée

2026-10-25. Aucune tâche importée ; aucune exécution autorisée.

## État et nouvelles preuves

Objectif inchangé : finir le lot existant sans refaire les preuves valides ni élargir implicitement Linux/F8. Le PM a remis un blocage, pas un plan exécutable. Sa session sans équipe ne renseigne pas le rôle de ce superviseur.

Contrôle natif effectué ici : `team_status()` confirme le rôle lead de **mission-91cc703bafaaebfc**, politique draft ; aucune autorité établie sur **mission-aaae4c9ca30bb8be**. Aucun teammate actif. Ce constat remplace toute inférence depuis la session PM.

Le catalogue effectif de `team_spawn` accepte `agent`, `context`, `worktree` et `report_mode`. L'absence de ces champs dans `team_tasks_add` n'interdit donc pas à elle seule leur transmission au spawn. Mais cela ne prouve pas une admission réussie ni un transport compatible.

Lecture ciblée du plugin installé, `index.js` lignes14853–14903 : `executeTeamSpawn` refuse effectivement un worker d'exécution autonome sans `claim_task`, ou avec `report_mode: final_response` (14896–14897). Cette condition explique le refus historique. **Aucune voie native compatible avec l'audit strictement hors board n'est démontrée à ce stade.** Ne pas lancer un spawn voué au refus, une fausse tâche, ou un profil de substitution. Aucune modification du plugin proposée dans le lot.

Catalogue corrigé au rapport02 : validation locale PASS ; cette preuve ne vaut ni approbation de mission ni résolution des autres blocages.

## Choix et recommandation

1. Autorité : soit rétablir le rattachement historique par une voie native autorisée, soit autoriser explicitement une **continuation distincte dans l'équipe actuelle**, sans transférer l'approbation ni compléter les anciennes tâches. Recommandation pratique si le rattachement n'est pas disponible : continuation distincte, avec références de provenance et nouvelle config/start par l'utilisateur.
2. Linux : recommandation pour ce lot borné, **analyse critique Windows/Linux des chemins QA existants et de leurs gaps seulement**. Ne pas promettre un PASS Linux. Une campagne Linux demande environnement existant identifié, toolchain C++23 fonctionnelle, choix CPU/GL/harness et critères explicites ; aucune installation implicite.
3. Audit : conserver le gate final **BLOCKED** tant qu'une voie native conforme n'est pas disponible. Les autres travaux indépendants pourront être proposés après les arbitrages et approbations nécessaires, mais aucune clôture ne pourra prétendre cet audit exécuté. Accepter une continuation ne supprime pas le gate et n'autorise pas une correction du runtime.

QA proposée maintenue proportionate : réutilisation du plan17/generator39, campagne ASan prévue, audit distinct requis par le handover, pas de nouvelle chaîne obligatoire. Toutes les dettes F8/C2/B1/GCC restent explicites.

## Décision demandée

Autorisez-vous la continuation distincte dans l'équipe actuelle, Linux limité à l'analyse pour ce lot, en conservant l'audit final bloqué jusqu'à admission native conforme ? Sinon préciser le rattachement autorisé et/ou l'environnement et les critères d'exécution Linux voulus.

Après décision, transmettre les clarifications au PM par `/ensemble-mission prepare --resume <clarification>`. Ne pas présenter la préparation comme prête avant traitement explicite du gate audit ; `start` demeure réservé à l'utilisateur après config. Aucun worker, classifier, test, build, commit, mutation de board ou modification source effectué ici.
