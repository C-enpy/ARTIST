# Rattrapage QA Linux — revue du plan importé

## Objectif et résultat de la revue

La préparation importée propose huit tâches pour qualifier Linux sous WSL2 sur I: puis en CI, sans dupliquer le harnais Windows. Lecture réelle du board par `team_tasks_list` : une tâche en attente, sept bloquées ; aucune exécution constatée dans cette lecture. L'import n'approuve pas la mission.

Le contenu technique est cohérent avec le mandat : outils existants, configurations séparées, tests CPU/OpenGL/pixels, ASan réellement actif et conservation des contrats Windows. QA proportionnée et exigences obligatoires du projet restent applicables.

## Graphe et ownership

1. Contrat de parité : qa-planner fresh, sans worktree ni édition ; le superviseur persiste et commite la remise avant les auteurs.
2. Distribution WSL dédiée : build-engineer fresh, mutation limitée à une nouvelle distribution sous I: après inventaire. Aucune modification des distributions existantes.
3. Build Linux/CPU et extension ASan : deux propriétaires build-engineer fresh, worktrees/sorties distincts. Le premier possède presets/profil/lanceur ; le second module/canari/vérificateur ASan.
4. Après intégration effective des deux producteurs : qualification OpenGL logiciel et CI, commandes communes et contrôle des preuves.
5. Après intégration et exécution : propositions de gaps par test-engineer fresh ; le superviseur choisit, persiste et exécute les compléments.
6. Audit test-reviewer fresh indépendant, puis documentation de clôture.

Tous les contextes importés sont fresh : les briefs et entrées intégrées suffisent, sans importer un état de recherche ou de worktree périmé. Aucun fork nécessaire n'est identifié. Les scores PM de développement respectent le plancher 70 ; ces estimations ne modifient ni le catalogue ni la politique native de routage.

Conan partagé, ressources GPU et sorties de build exigent des baux/sérialisation. Un verrou CTest dans un arbre ne suffit pas à sérialiser les autres worktrees. L'intégration et la vérification du checkout Linux précèdent les campagnes consommatrices.

## Réserve d'admission à corriger

Le projet impose l'audit indépendant hors du board d'implémentation visible au reviewer. Or l'import a déjà créé la tâche d'audit sur ce board. Le texte de sa fiche promettant un audit hors board ne démontre pas la configuration de cette séparation. Les consommateurs ont aussi été publiés par l'import alors que les instructions du coordinateur demandent de les conserver au plan jusqu'à intégration de leurs entrées.

La signature native découverte de `team_tasks_add` permet l'ajout et les décisions d'exécution, mais n'expose ni retrait, ni mise en attente, ni changement de visibilité d'une tâche existante. Ne pas inventer un paramètre, une complétion fictive, une dépendance factice ou une modification de base interne pour contourner cette limite. L'admission concernée reste bloquée tant qu'une voie native conforme n'est pas disponible et vérifiée. Aucun worker n'est lancé pour tester cette hypothèse avant approbation.

Les profils QA sont explicitement importés avec contexte fresh, worktree false et final_response. Ces champs conservés statiquement ne prouvent pas encore la lecture/restitution effective ; leur qualification reste une précondition d'admission.

## Question matérielle résolue au périmètre

Aucune machine Linux native avec GPU et driver n'a été identifiée. Ce jalon est reporté et restera NOT_RUN ; il ne bloque pas la livraison CPU/OpenGL logiciel WSL/CI. Aucune nouvelle question utilisateur sur le matériel n'est nécessaire maintenant.

## Suite

Plan technique accepté pour préparation, avec réserve d'admission native à résoudre avant libération des affectations concernées. Aucune modification de la politique OpenAI/long_run/QA proportionnée n'est requise par cette revue. Seul le start natif de l'utilisateur peut approuver la politique présentée ; ne pas le déduire de l'import.

## Traçabilité

- Préparation : `ses_e7cdc3fa6ffeaCOmqx1gpRuA0J`.
- Contrat : `task_mvsa43rc_0003_5x254n` ; WSL : `task_mvsa43rc_0004_dujtol`.
- Build : `task_mvsa43rc_0005_r040cn` ; ASan : `task_mvsa43rc_0006_6jw879`.
- OpenGL/CI : `task_mvsa43rc_0007_cq2xsm` ; gaps : `task_mvsa43rc_0008_8508em`.
- Audit : `task_mvsa43rc_0009_tr8pgb` ; documentation : `task_mvsa43rc_000a_0yh289`.
- Vérifications de cette étape : lecture du board et de la signature native de gestion des tâches, pas de build/test ou de mutation du board.
