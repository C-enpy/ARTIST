---
description: Prépare une mission ARTIST avec le PM et le cycle natif Ensemble, avant approbation explicite et exécution TDD/QA.
agent: orchestrator
subagent: false
---

Prépare la mission ARTIST $ARGUMENTS. Si l'objectif manque, demande-le avant de
choisir un travail arbitraire. Applique AGENTS.md et le contrat de project-manager :
découpage, ownership, dépendances, scores et critères observables.

Ce raccourci ne remplace pas le cycle natif `/ensemble-mission`. Pour une nouvelle
mission Ensemble, si aucune équipe préparée par cette commande n'est rattachée à
la session, donne à l'utilisateur la commande exacte `/ensemble-mission <objectif>`
avec son objectif, puis arrête-toi. Ne crée pas une équipe legacy avec `team_create`
pour contourner cette étape. Une demande de plan seul peut être traitée en lecture
seule, sans équipe ni exécution. Une équipe legacy existante conserve sa politique ;
ne la recrée pas et ne la convertis pas implicitement.

Le mode d'évaluation est PM/LLM par défaut ; `evaluation=jev` demande JEV direct,
`evaluation=llm` choisit explicitement le PM. Conserve ce choix dans le brief et le
plan transmis au cycle natif. Une demande de plan seul s'arrête après le plan.

Pour une mission préparée, présente le plan et la sélection issue de la grille
complexité × risque de `.opencode/missions/<team-id>/model-policy.json`. Écris les
corrections providers/modèles/variantes/équivalents et critères dans ce fichier,
pas seulement dans le chat. Demande ensuite `/ensemble-mission config` pour
présenter la version corrigée, puis attends `/ensemble-mission start` de l'utilisateur.
Ne lance pas `start` à sa place ; ni un GO en prose, ni `/engine-mission start`,
ni une approbation de plan de teammate ne valent approbation native de la politique.
Les arguments `config` ou `start` de ce raccourci orientent vers la commande native
correspondante, sans approuver ni lancer eux-mêmes une mission.

Avant approbation, reste en préparation : aucun worker d'exécution ni changement
de source. Après approbation, publie seulement les branches prêtes, évaluées avec
les deux scores, le plancher développement à 70 et les contextes explicites.
Laisse Ensemble imposer la grille approuvée et les restrictions opérateur, sans
override de modèle ni auto-claim en cours de tour. Intègre les livrables avant leurs
consommateurs, conduis les tranches en TDD puis la QA indépendante, et rapporte les
preuves et limites. Une pause opérateur exige un GO de reprise distinct ; ce
raccourci ne la retire jamais.
