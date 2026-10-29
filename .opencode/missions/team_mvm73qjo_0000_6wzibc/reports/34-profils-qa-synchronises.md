# 34 — Profils QA relus et synchronisés ; vérification runtime demandée

2026-10-24. Décision opérateur après rapport33 : « Recharge les profils vérifie normalement ils ont été corrigés ». AGENTS mis à jour : les trois profils QA doivent partager les mêmes permissions. Pas d'autorisation de réparer le plugin ou de changer modèles/catalogue.

## État disque réellement relu

Canonique : qa-planner communicationv2 complet31 règles ; test-engineer/test-reviewer avaient été partiellement corrigés (execute allow) mais restaient27 règles, sans team_message explicitement autorisé. Leurs méthodes maintenaient les outils d'équipe deny et le marqueur final_response.

Worktree pool-15 : anciennes copies27/execute deny pour les trois profils. La correction canonique partielle n'y était pas transmise. Le constat historique du rapport33 était donc dépassé sur execute canonique, mais le canal restait interdit par défaut et la copie active était bien périmée.

## Corrections bornées effectuées

Dans la base d'abord, test-engineer et test-reviewer alignés sur le bloc permissions **identique** du qa-planner : ajout grep/team_message et deux contextes externes déjà nommés, retrait des consignes contradictoires de restitution et marqueurs final_response. Pas d'ouverture de shell, édition, sous-agent, board/claim/spawn ou autres MCP. Code Mode reste non sandbox ; fetch/API/utilitaires hors rôle interdits.

Instruction orchestrator contradictoire27 règles corrigée pour les trois profils. Puis copies des trois profils + AGENTS + orchestrator synchronisées dans le worktree pool-15, sans source moteur ou base interne runtime modifiée. Les deltas canoniques concurrents sont conservés, pas committés aveuglément.

Comparaison octet pour octet base/worktree et blocs frontmatter : **31 règles identiques pour les trois profils**. SHA256 :
- qa-planner `504268d5539769aa85fd69158b592eb4c9b63cdf7fc23fd488d904ad0d43f1c3` ;
- test-engineer `cbc0ea3c6e92f555b8c15f8ebf8a77935fb292a4e2817eb350eb4278265ea427` ;
- test-reviewer `14c5b6fa14d091c5012eec518fe133cf26e80e221bf583ce31ea7dadfad33591`.

## Rechargement et preuve runtime : portée exacte

Aucun outil opencode reload exposé dans le catalogue découvert. Aucun restart service, changement de session/provider ou appel d'API supposé effectué. La synchronisation n'est **pas** une preuve de hot reload.

La même session pool-15 a reçu la correction explicite de ses anciens briefs puis une unique vérification ciblée : lecture locale du profil, découverte et envoi `QA-H2-TRANSPORT` par execute→team_message. Pas de nouvelle collecte/tests/écriture. Un refus donne une finale unique et l'inactivité, pas une nouvelle boucle. Le lead doit recouper la réception avant de déclarer le transport fonctionnel et demander le texte QA déjà prêt.

## Limites statiques restantes

Le validateur local `.opencode/scripts/validate-agentic.mjs:451–481` distingue encore planner31 des autres QA27. Il n'a pas été modifié ou exécuté pour fabriquer un PASS : il doit être réconcilié avec la politique opérateur lors d'une tranche harness bornée et testée, si retenue. Présence statique, permissions effectives et restitution restent trois preuves distinctes.

À cette étape : profils disque synchronisés ; prise en compte runtime **à vérifier**, livrable generator non reçu, H3/campagne ASan finale toujours en attente. Les résultats moteur du rapport32 restent valides à leur seule portée.
