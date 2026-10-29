# 10 — Reprise v2, H_A intégré, transport QA bloqué

Date : 2026-10-24. Étape : prérequis des auteurs ; aucun test moteur exécuté.

## Politique et admissions

Révision 2 approuvée, hash `ea11ffaf8fea9a336d7d8df5ac2a68ef90415d9c62c741135f27290c66784d56`, confirmée par `team_status` : supervised/running, 16 tâches historiques not_required, zéro appel classifier. Le retrait de Cursor et les trois critères F8 sont applicables aux nouvelles admissions, pas réécrits dans les tâches passées.

Deux remplacements de recherche ont été publiés avec contexte/ownership/critères dans leur contenu et affinité explicite, scores PM/LLM validés (Q C48/R50 ; protocole C55/R50). Tous deux sont recherche documentaire sans production de code. Décision directe native autorisée : `openai/gpt-6.1-sol` high, sans modèle au spawn.

- **Q** : `task_mvmc5s53_000v_ct63sq`, profil `qa-planner`, lecture seule stricte, sans worktree. Tentative native `report_mode: final_response` refusée par le runtime : « Autonomous workers require claim_task and collaborative reporting », malgré task ID et décision directe fournis. C'est une nouvelle expérience discriminante par rapport au refus de l'admission planifiée : la décision directe ne résout pas le transport. Aucun worker QA admis par ce spawn ; aucun H_Q livré. **BLOCKED**, aucune permission execute/team_* ajoutée ni repli de modèle/profil pour contourner la restriction.
- **ASan** : `task_mvmc5s53_000w_rbrmkl`, membre `asan-sol-reprise`, `build-engineer`, fresh, sans worktree. Runtime confirme admission **openai/gpt-6.1-sol high**, conformément à la correction utilisateur. Analyse/protocole seulement, aucun build/test/installation autorisé. Warning de permissions Ensemble toujours présent ; la capture effective reste à constater, sans conclure au blocage à partir du warning seul puisque F8 a déjà restitué.
- **C2** : reprise demandée de la même enquête de `pool-3`, sans changer modèle/session ou permissions ; profil build générique signalé comme substitution pour analyse seulement. Sources/HEAD/K et constat historique fournis ; aucune nouvelle tâche à prendre. Résultat courant attendu.

## Arrêt des substitutions QA

`pool-2` a reçu demande d'arrêt non forcé, sans reprise QA ni auto-attribution. `team_merge` a confirmé **aucun commit et aucun changement non commité**, rien à intégrer. Ses observations restent partielles et ne valent pas le plan H_Q. L'absence de mutations de pool-1 avait déjà été recoupée.

## H_A accepté et réellement intégré

Commit ciblé du lead **`354b53f2d4cbdf427aad54069bfc6ef43a5f90f7`** : uniquement les rapports 08 (contrat) et 09 (décision), 2 fichiers/83 lignes. Les mentions « non commité » de ces rapports décrivent leur état lors de leur rédaction ; le présent checkpoint établit leur intégration ultérieure.

Commandes réellement exécutées via Context Mode, cwd ARTIST :
- `git diff --check -- <rapport08> <rapport09>` : aucune erreur affichée ;
- `git add -- <rapport08> <rapport09>` ;
- `git commit --only -m "docs: record accepted F8 evidence boundary before authors" -- <rapport08> <rapport09>` : commit créé ;
- `git show --format=fuller --stat HEAD -- <rapport08> <rapport09>` : seuls les deux livrables H_A présents ;
- `git status --short` : changements gitignore/GrepAI préexistants conservés, autres rapports/politique non embarqués.

La tâche architecturale `task_mvmbkkq8_0002_5aeiej` a été complétée **après** cette intégration/acceptation, pas sur le seul statut auteur. Shutdown non forcé de `f8-contract` demandé. Cela ferme uniquement la livraison du contrat de preuve, pas F8 ni la preuve d'une panne réelle.

## Barrières, limites et suite

H_A est maintenant commité ; **H_Q reste absent**. Aucun auteur n'est débloqué par le coordinateur et aucune sonde F8 n'a été exécutée. Le protocole ASan et C2 restent attendus. Les anciennes tâches sans livrable restent historiques, non marquées réussies pour masquer un échec.

Le contrat natif QA restrictif et le transport collaboratif imposé par le runtime sont actuellement incompatibles avec la voie essayée. Il faut conserver BLOCKED plutôt qu'ajouter execute/team_message à QA ou qualifier un profil build de qa-planner. Aucun changement de plugin, de permissions globales ou de sandbox engagé.

Recommandation : recueillir les enquêtes indépendantes déjà admises, puis présenter un arbitrage borné si aucune voie native QA autorisée n'est disponible. Pas de répétition du même spawn refusé et pas de test/infra supplémentaires pour remplir un rapport.
