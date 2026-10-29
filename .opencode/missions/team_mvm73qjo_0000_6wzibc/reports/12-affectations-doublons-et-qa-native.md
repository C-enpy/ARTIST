# 12 — Doublons d'affectation et QA native observée

Date : 2026-10-24. Étape : préparation ; aucun auteur débloqué.

## Diagnostic déclenché par le retour de pool-6

`pool-6` signale être auto-affecté à l'ancien plan QA, profil build/worktree, alors que la reprise Q est déjà affectée à pool-5. Il déclare n'avoir exécuté aucune commande/test/écriture et refuse de produire un doublon. Ce constat est cohérent avec les substitutions QA précédemment arrêtées.

Un contrôle `team_status` motivé par cette divergence, et non un poll d'attente, confirme :
- **pool-5 : profil natif qa-planner**, tâche de reprise `task_mvmc5s53_000v_ct63sq`, en cours ; worktree auto-créé. Cette admission automatique est distincte du spawn final_response refusé du rapport10 ; ce refus ne prouvait pas l'impossibilité de toute admission native.
- pool-6 : build, idle, ancienne tâche QA ;
- pool-7 : build, ancienne tâche C2 réattribuée automatiquement après shutdown de pool-3 ;
- asan-sol-reprise encore en cours ; politiques et critères F8 v2 inchangés, zéro classifier.

## Mesures bornées du coordinateur

- Pool-5 désigné seul propriétaire H_Q ; brief complémentaire envoyé avec K, H_A commité, sources QMD déjà récupérées et HEAD source. Rappel strict read/glob/skill, sans execute/shell/edit/subagent ; rendu natif texte, persistance et runner au lead. Le worktree automatique est celui d'un analyste en lecture seule, pas une autorisation de création de worktree auteur ni d'écriture. Le fonctionnement effectif de sa restitution reste à constater, sans grant.
- Pool-6 : doublon refusé, aucune production H_Q ou autre tâche autorisée. Message stocké sans réveil (membre déjà idle/completed côté transport). Pas de nouveau shutdown immédiatement : les arrêts précédents ont permis la réattribution de la même tâche historique non terminée. Aucune commande de pause du runtime invoquée implicitement.
- Pool-7 : instruction de ne pas refaire la collecte C2 déjà acquise ni lancer de replay ; maintenir l'affectation sans activité, restitution limitée à l'état et absence de mutation. Aucun nouveau mandat d'enquête ou changement de modèle.

Il ne s'agit pas d'un mécanisme technique de hold du board : l'API exposée ne fournit pas de mise en attente/annulation des anciennes tâches. Aucune tâche QA n'est marquée completed fictivement pour arrêter la boucle ; aucune modification de l'auto-dispatch/global ou de la base runtime. Les consignes d'inactivité doivent être distinguées d'un verrou natif effectif.

## Preuve C2 persistée

Rapport11 seul commité par le lead : **`49b5dadd99c1a2872e8de4224ab3137914c4a810`**, commande `git commit --only` sur ce fichier puis `git show --format=oneline --stat HEAD -- <rapport11>` : 1 fichier/43 lignes. Aucun diff moteur ni changement GrepAI embarqué. Cela préserve le diagnostic borné, sans fermer C2, terminer sa comparaison primaire ou déclarer sa tâche réussie.

H_A reste intégré au commit354b53f. Les nouveaux commits ne changent que les rapports, pas les sources moteur fournies aux analystes.

## État et prochaine étape

Attendre les livrables H_Q natif et protocole ASan Sol, sans multiplier les spawns ni reproduire les refus. H_Q non reçu à ce stade ; aucun test exécuté. Les décisions sur les archives C2 et le statut des tâches historiques seront traitées distinctement de la réussite de la QA. Toute impossibilité réelle de restitution restrictive restera BLOCKED, pas une autorisation d'élargir les permissions.
