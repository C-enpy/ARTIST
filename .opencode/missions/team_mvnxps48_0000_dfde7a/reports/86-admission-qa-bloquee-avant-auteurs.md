# 86 — Admission QA bloquée ; aucun changement source

2026-10-26. Objectif : nettoyage portable du chargement de fichiers stables, contrat/inventaire adoptés au [85](85-contrat-stable-et-inventaire-adoptes.md), commit `26182b5ebacf4de466086bd90e60a2df55051c01`. Étape : amendement QA préalable aux auteurs.

## Depuis85 : faits vérifiés

- Tentative native de préparation QA fresh/worktree:false sans tâche : refus `Autonomous workers require claim_task and collaborative reporting`. Aucun worker créé par cette tentative.
- Publication de la seule tâche d'**admission RO** `task_mvply25m_003x_kerfs2`, affinité qa-planner, scores61/R72, sans autoriser l'analyse. Auto-dispatch immédiat vers pool-6. Tentative spawn explicite avec claim/fresh/worktree:false refusée, tâche déjà affectée ; aucun deuxième worker créé. Pas de nouvelle tentative.
- `team_view(pool-6)` confirme la session `ses_e8ba57542ffea1tdYPD3SqBw3x`. GET natifs session et contexte, via CLI existante : agent qa-planner et répertoire canonique ARTIST, **1143 messages historiques**, pas fresh.
- Permissions effectivement renvoyées par GET : deny-all initial, mais permissions allow additionnelles comprenant board (`team_tasks_add/list`, claim), handoff, broadcast, skills génériques et MCP externes. Elles ne correspondent pas au profil restrictif canonique lu sur disque. Aucune hypothèse de hot reload ni promesse de sandbox ; aucun changement de profil/permission tenté.
- Pool-6 a seulement lu AGENTS/85 et transmis son admission. Son message signale lui-même contexte historique et absence de contrôle effectif ; réception recoupée dans le contexte natif. Aucun amendement QA produit, aucune analyse autorisée. L'écart relève de l'admission, pas d'un test Reader ni d'une faute du planner.
- HOLD transmis et stocké sans réveiller le worker inactif ; pas de completion fictive, shutdown/réadmission en boucle, autre tâche ou substitution générique. Aucun nouveau code, build, test, probe, suppression ou backend de présentation.

## Conséquence et limites

**Admission QA BLOCKED**, donc amendement non commité et barrière avant auteurs toujours fermée. Le contrat85 n'est pas une validation ni une permission de contourner cette barrière. Les13 probes et Reader sont encore intacts. Diffs étrangers/archives préservés ; dernière modification de cette reprise est documentaire uniquement.

Le schéma `team_tasks_add` découvert n'expose pas `require_context`, contexte ou worktree ; le spawn expose ces champs, mais exige une tâche libre que l'auto-dispatch a déjà affectée. Aucun outil de mise en attente/réaffectation n'a été supposé, aucune base interne éditée. Ce diagnostic n'établit ni une impossibilité universelle du runtime, ni le correctif à lui appliquer.

## Options et recommandation

1. **Admission native restrictive bornée (recommandée)** : autoriser une intervention dédiée sur ce seul blocage pour obtenir planner au bon profil/contexte/transport, en privilégiant les commandes natives supportées. Si un changement exécutable s'avère nécessaire, proposer d'abord ownership/contrat/PM et RED causal ; aucune implémentation de harness implicitement autorisée. Coût : détour de coordination, aucun élargissement du chantier Reader/probes.
2. **Rester en attente** : conserver les admissions et sources gelées jusqu'à résolution externe du routage natif. Aucun détour d'outillage ; nettoyage non livré pendant l'attente.

Pas d'option de droits élargis, de profil générique, d'auto-QA présentée comme indépendante, ni de suppression avant QA. Décision demandée : autoriser l'intervention native bornée, ou rester en attente. Les garanties/exceptions historiques ne sont pas modifiées par cette décision.
