# 27 — Pool-5 : arrêt demandé après boucle signalée

2026-10-24. Objectif : arrêter l'activité devenue inutile de pool-5 sans perdre ses fichiers ni réadmettre son affectation.

## Observations

L'opérateur signale une boucle. Contrôle natif : pool-5 `working`, profil qa-planner, aucune tâche affichée pour ce membre, dernier message ancien alors que H_Q est déjà intégré au commit64d97dc. Les notifications précédentes sans contenu récupérable ne prouvaient pas son inactivité ; le lead aurait dû distinguer ces deux faits. Le mécanisme exact de répétition n'est pas établi par ce seul état.

## Action et résultat

Un unique `team_shutdown(pool-5)` non forcé a été demandé. Retour effectivement reçu ensuite : pool-5 confirme H_Q livré/intégré et **fin d'activité conformément au shutdown**, sans nouveau test, édition, commande ou auto-attribution déclaré. Il mentionne un smoke de lecture/communication, pas une preuve moteur ni une nouvelle obligation.

Contrôle natif après cet accusé : **`shutdown_requested`, `state: preserved`**, branche préservée et worktree toujours présent. Le statut terminal `shutdown` n'est pas encore confirmé ; ne pas le déduire du seul accusé. Aucun nouveau spawn, aucune réactivation, aucune fausse complétion de tâche ni nouvelle boucle shutdown/réadmission.

## Conservation

`git status --short`, `git diff --stat` et `git log -1` exécutés dans son worktree : HEAD989cb995 ; quatre fichiers modifiés non committés (AGENTS.md, orchestrator.md, project-manager.md, qa-planner.md),126 insertions/33 suppressions selon Git. Ces deltas ne sont pas attribués au planner et ne sont pas automatiquement intégrés : des corrections lead/concurrentes existent. Aucun cleanup, suppression, reset ou merge aveugle. H_Q lui-même est déjà persisté dans le canonique.

## Limites et suite

Activité déclarée terminée et état natif préservé observé ; finalisation du statut runtime encore inconnue. Si une réactivation survient, traiter la mise en attente par une voie native et une preuve nouvelle, sans répéter l'ancien cycle. Aucun diagnostic de cause racine ou correctif du plugin revendiqué. Aucun test moteur exécuté dans cette intervention.
