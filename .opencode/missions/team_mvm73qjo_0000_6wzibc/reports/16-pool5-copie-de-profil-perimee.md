# 16 — Pool-5 : copie worktree périmée identifiée et synchronisée

Date : 2026-10-24. Étape : diagnostic demandé par l'opérateur, sans nouveau worker ni relance de mission.

## Observation actuelle, différente des rapports 14–15

Les instructions projet ont depuis été révisées : qa-planner utilise la politique communication v2 ; ses27 règles historiques ne sont plus un invariant. Les deux autres profils QA restent inchangés.

`team_status` confirme pool-5 au profil natif qa-planner, tâche de reprise H_Q, worktree `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-6wzibc-pool-5`. Aucun résultat de ce membre récupéré au début de cette étape ; les messages remontés étaient d'anciens briefs du lead, pas ses refus effectifs. « working » ne prouve pas une progression.

Comparaison directe des fichiers :
- **canonique** `.opencode/agents/qa-planner.md` :31 règles, execute/team_message allow, grep et contextes externes nommés autorisés ;
- **worktree pool-5 avant correction** :27 anciennes règles, execute deny, pas d'autorisation team_message, ancien marqueur `ensemble-report: final_response` ;
- AGENTS/orchestrator du worktree conservaient aussi les anciennes consignes27/execute fermé.

Cette divergence est prouvée sur disque. Qu'elle explique toutes les tentatives runtime reste une hypothèse tant que le membre n'a pas rapporté sa permission effective. Le contraste avec les rapports14/15 s'explique par une révision canonique ultérieure : ne pas présenter leurs contrôles passés comme actuels.

## Correction effectuée

Sans réécrire le canonique révisé ni ses modifications concurrentes, le lead a appliqué son contenu actuel dans les trois copies worktree :
- `.opencode/agents/qa-planner.md` : permissions communicationv2, contexte nommé, restrictions et avertissement Code Mode non sandbox, retrait du marqueur final_response périmé ;
- `AGENTS.md` : politique qa-planner révisée, sans imposer27règles ;
- `.opencode/agents/orchestrator.md` : distinction qa-planner communicationv2 / deux autres QA restrictifs.

`edit`, `shell`, `subagent` demeurent deny. Aucun droit de test, de délégation, de modification de session ou d'appel des autres MCP accordé par cette synchronisation. Les utilitaires/fetch accessibles dans Code Mode ne sont pas une sandbox ; leur emploi hors communication reste interdit par le rôle.

## Vérifications effectuées

Comparaison des trois fichiers : **identiques octet pour octet base/worktree**. Profil SHA256 commun **`504268d5539769aa85fd69158b592eb4c9b63cdf7fc23fd488d904ad0d43f1c3`**. Lecture structurée des triplets :31 règles, execute/team_message allow ; edit/shell/subagent deny. `git diff --check` sur ces trois fichiers dans le worktree : propre.

Un premier script de détection regex a mal conservé les échappements et affiché des faux négatifs allow ; ces valeurs ont été écartées. Les permissions ci-dessus sont recoupées par lecture directe puis extraction des triplets sans cette regex. Aucun diagnostic de permissions runtime n'est déduit de ce script.

## Vérification runtime demandée, non encore obtenue

Message envoyé au **même** pool-5 : les anciens briefs du lead imposant27/execute deny sont remplacés par la politiquev2 ; une lecture locale puis un accusé unique `H_Q-TRANSPORT` via execute→team_message sont demandés. Pas de bootstrap, nouvelle collecte, fetch, API session, commande ou test. Si outil absent/refusé, restitution finale unique avec refus exact ; aucune répétition de la même expérience.

L'édition sur disque ne prouve pas le hot reload de la session ni l'adoption des nouvelles permissions. Aucun restart/shutdown/changement de modèle ou nouvelle admission effectué. Retour runtime et H_Q toujours attendus ; auteurs toujours bloqués. Les modifications worktree sont celles du lead ; le canonique modifié dans l'autre intervention n'est pas commité par ce diagnostic.

## Mise à jour — accusé runtime reçu

Résultat natif `pool-5`, statut `H_Q-TRANSPORT`, effectivement récupéré par le lead : lecture locale du profil réussie (execute allow77–79, team_message allow89–91), découverte de tools.team_message et envoi unique déclarés par le membre. L'accusé est bien arrivé ici. Cela vérifie lecture et restitution sur cet essai ciblé, pas la composition complète des permissions, la sandbox ou le mécanisme de hot reload. Le SHA reste celui calculé par le lead, pas une empreinte recalculée par le worker.

Le membre précise qu'aucun refus runtime antérieur n'avait été rencontré : les anciennes limites étaient instructionnelles, sans tentative du canal à ce moment-là. Ne pas requalifier les warnings de spawn ou les interdictions dans le brief en refus natifs exécutés de pool-5.

Le plan H_Q complet est déclaré rédigé par le membre, mais son texte n'est pas présent dans la restitution récupérée. Le lead en a demandé la retransmission complète, sans nouvelle collecte, via le canal qui fonctionne, en parties numérotées si nécessaire et marqueur FIN_H_Q. Chemin de persistance désormais nommé : `reports/17-plan-qa.md` dans cette mission, pour ne pas écraser le plan historique mission-gpu-qa cité par le profil. Auteurs toujours bloqués jusqu'à réception, contrôle et commit du plan réel.
