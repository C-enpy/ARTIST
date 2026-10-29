# 05 — Approbation native reçue ; enquêtes et limite de transport

Date : 2026-10-24. Étape : démarrage de la préparation spécialisée ; auteurs toujours bloqués.

## Approbation et candidat

Approbation native reçue : révision 1, hash `b516ed28f38966db6e9ee7e33c2d6fa6223110cf2bd64ec88a99c39028ec2fdf`. Le catalogue approuvé gouverne les admissions, sans override de modèle. B1–B6 et QA proportionate avec obligations ARTIST confirmés dans le rapport 04.

HEAD reconstaté : `989cb995ef6ae96f2eed8f945e2c6ff29bd90d92`. Statut préalable inchangé : gitignore/GrepAI et répertoires missions non suivis ; aucune source moteur modifiée par le lead. Les 16 tâches importées ont été lues une fois pour valider leurs contrats et estimations PM/LLM ; le board indique classification not_required, pas un classifier lancé.

## Admissions observées et écart

- L'auto-dispatch avait affecté `qa-plan` à `pool-1`, profil **build**, avec worktree, plutôt qu'au profil restrictif `qa-planner` prescrit par le contrat. Le lead lui a demandé de rester strictement en lecture seule, de ne pas compléter/prendre de tâche et de restituer ses actions ; shutdown non forcé demandé. Arrêt effectif et absence de mutation restent à confirmer par son retour. Ce résultat ne sera pas qualifié de plan QA indépendant natif.
- `f8-contract` admis explicitement sans worktree, profil `engine-architect`, contexte fresh, modèle imposé par runtime `openai/gpt-6.1-sol` high. Mandat d'analyse seulement, aucune seam/API ni mutation autorisée.
- `asan-protocol` admis explicitement sans worktree, profil `build-engineer`, contexte fresh. Le runtime a imposé `cursor/claude-opus-5-5` Max, pourtant non préféré dans le chat : aucune substitution effectuée par le lead. Ce modèle figure dans le catalogue approuvé ; le rapport 02 signalait déjà que l'ordre n'était pas une priorité stricte.

## Blocage de transport — aucun élargissement

Une tentative de spawn F8 avec `report_mode: final_response` a été refusée : « Autonomous workers require claim_task and collaborative reporting », malgré le task ID transmis. Aucune admission revendiquée pour cette tentative refusée.

Les deux spawns réussis en `team_message` ont ensuite renvoyé **missing-ensemble-tools** : permissions effectives non allow pour execute, team_message et les outils Ensemble listés par le runtime. Le transport collaboratif n'est donc pas prouvé utilisable. Aucun hook, permission, modèle, classification ou réglage global modifié pour contourner ce constat.

Le catalogue actuel de `team_tasks_add` n'expose ni `require_context` ni mise à jour du contexte des tâches importées. Aucune propriété hors signature n'a été envoyée ; contexte fresh explicite sur les deux spawns, sans prétendre avoir activé le verrou demandé par les instructions projet.

Par précaution, **admissions supplémentaires suspendues par le coordinateur** : C2 et remplacement QA non lancés, auteurs toujours interdits jusqu'à H_Q commité et H_A accepté pour F8. Les workers déjà admis ne sont pas interrompus de force ; attente de leurs restitutions/erreurs natives pour qualifier le blocage réel. Aucun poll de progression.

## Vérification et prochaine étape

Aucun build, test, RED/GREEN ou mesure exécuté. Aucun livrable intégré. Le warning n'est ni une preuve de capture finale ni une preuve de sandbox.

Mise à jour après retour natif de `pool-1` : résultat **failed**, motif profil build incompatible avec qa-planner ; aucune source lue, aucun plan QA produit, aucune mutation déclarée. Le lead a recoupé par `team_merge` : **aucun commit et aucun changement non commité**, rien à intégrer. Son message et son résultat ont bien été capturés : preuve de transport pour ce membre seulement, pas pour les profils F8/ASan/QA restrictifs. Sa demande préalable de contexte n'est pas une nouvelle décision opérateur ; K et HEAD sont déjà établis dans les rapports 03/04/05. Pas de reprise de ce membre comme planner de substitution.

Mise à jour après retour F8 : message collaboratif et résultat final effectivement reçus pour `engine-architect`, malgré le warning initial. Le résultat est **failed documentaire**, pas un défaut du moteur : QMD absent de son catalogue local ; aucun corps/consommateur examiné et aucun contrat H_A produit. Le lead a effectué lui-même une recherche QMD `artist-wiki` avec intent F8/H_A, récupéré `#3b3703:75–109` et `artist-wiki/development/known-incidents.md:230–324`, puis transmis les extraits pertinents, statuts, K et HEAD au même membre par `team_message(force:true)`. Reprise demandée sur la même tâche, strictement lecture seule, sans extension de permissions. Les deux sources confirment seulement les limites historiques F8 ; aucune preuve comportementale nouvelle. Le blocage de catalogue documentaire de cette feuille ne nécessite pas d'arbitrage opérateur lorsque le lead fournit les sources récupérées. Le transport QA restrictif reste non prouvé ; aucune nouvelle admission à ce stade.

Recommandation : recueillir les retours natifs des deux analyses et l'arrêt de pool-1 avant toute nouvelle admission. Si la restitution reste indisponible, signaler BLOCKED et présenter des options bornées à l'opérateur, sans modifier les permissions ni lancer un repli contournant le canal interdit. Les autres branches ne justifient pas de multiplier les admissions sur le même mécanisme signalé défaillant.
