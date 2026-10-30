# 76 — Préparer deux raccordements Windows sans coder prématurément

## Plan reçu

Le propriétaire Windows a lu les interfaces réellement intégrées du cœur et remis un plan, sans copie de branche ni implémentation d'adaptateur. Deux coutures doivent être réglées : les arguments workload doivent rester visibles à l'inspection du cœur, et la préparation pure ne reçoit pas actuellement l'identité/racine du bundle nécessaire au fichier privé du helper.

La préférence du coordinateur est d'exécuter directement workloads et CTest via le runner commun, depuis les exécutables admis du BuildReceipt, plutôt que de cacher leurs options dans un helper-run et dupliquer la capture. Ce choix et la propriété du payload de préparation sont soumis à une consultation architecte minimale, en lecture seule, avant ratification.

## Dépendances et parallélisme

La préparation architecturale peut avancer pendant que le collecteur de contexte termine sa tranche TDD et que Windows remet son correctif d'observation. Les changements de signature ou de préparation touchant le cœur devront attendre l'intégration du collecteur pour éviter deux écrivains sur `evidence.py`. Aucun nouveau schéma Request/receipt, mini-framework de commandes ou fichier écrit par list/plan n'est autorisé implicitement.

La notification reçue mentionne le candidat d'observation `6a19ad7…`, mais ne fournit que le plan d'adaptateur. Une restitution autonome du correctif déjà annoncé est demandée avant relance des tests ; aucune nouvelle écriture ou enquête générale n'est demandée au worker pour cela.

## Références et limites

- Plan `WINDOWS_ADAPTER_READONLY_PLAN.md`, commit `a5628d462e07f4f804610aa14d22586f9e17ee92`.
- Fondations lues `107a2d3bbd88ba95a2eb70e904085911b71662bd`, producteur/parser contexte `91c5ac23bf17d4d88dd36699cb7d15a6d6835872`.
- Consultation native `task_mvugu1co_003p_e3u2rs`, engine-architect fresh, sans worktree d'écriture, évaluation PM/LLM de lecture 65/65.

Aucun nouveau test, adaptateur de production, helper complet, MSVC, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté à cette étape. Le plan seul n'autorise aucun câblage source.
