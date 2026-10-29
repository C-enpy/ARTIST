# QA Linux — contrat intégré, création WSL autorisée au propriétaire

## Progression vérifiée

Le plan QA récupéré en lecture native est maintenant relu, persisté et commité. La tâche de contrat est clôturée par le superviseur pour son livrable réel, pas sur le seul statut du worker. Seule la tâche WSL a ensuite été publiée ; les six consommateurs suivants restent deferred.

Commit vérifié : `9bf085075e44cd299e80aca9128af4fd0f7c7777`, « docs: establish Linux QA parity contract before authors ». Deux fichiers seulement : `.opencode/evidence/mission-linux-qa/qa-plan.md` et rapport 07. La règle ignore du dossier evidence a nécessité un ajout forcé limité à ce fichier. Le renommage GrepAI déjà indexé et les autres travaux préexistants n'ont pas été inclus.

## Affectation et barrières

Le runtime approuvé a affecté la tâche WSL à pool-2, profil build-engineer, nouveau worktree basé sur l'entrée intégrée. Le lead a transmis commit et chemin du plan et demandé leur vérification avant mutation, ownership exclusif de la nouvelle distribution sur I:, préflights observés, restrictions de sécurité, statut des ADR et exigences de preuve.

Il n'est pas encore démontré que la distribution est créée ou la toolchain qualifiée. Le worker doit réexaminer destination/espace/options disponibles, ne toucher à aucune distribution existante, ne pas changer de réglage WSL global, puis remettre son manifeste. Les sources/builds/caches seront dans ext4 ; un transfert en lecture du dépôt Windows peut fournir le commit local non poussé.

Les branches build et ASan attendent encore cette entrée WSL. Pas de lancement artificiellement parallèle d'un consommateur sans prérequis. Le lead conserve la clôture après vérification effective.

Message de progression WSL reçu : préflights concordants, destination libre, options locales --location/--name/--version/--no-launch disponibles ; aucun changement système encore à cette remise. Le worker a signalé que le changement de cwd vers son worktree retire Context Mode/Serena/QMD de son catalogue. Il est autorisé à retrouver le contexte canonique pour les MCP avec chemins absolus vers son worktree et ses seules ressources possédées ; aucune réparation de configuration MCP/profil ni édition du canonique. Le lead a vérifié directement le HEAD du worktree `9bf0850`, son diff suivi vide et la présence du plan identique au canonique : SHA256 `b5fb473588c6cdd06f383593a329629aad4ee2adffd8e1dad6e3bb5bb0b3d731` des deux côtés. Cette limitation de découverte MCP ne signifie pas un défaut moteur ou une mauvaise racine Serena.

## Baseline indépendante utile pendant cette attente

Le lead, runner distinct, a exécuté sous Windows :

`C:/Python313/python.exe -B scripts/tests/test_verify_asan.py -v`

Résultat : **8/8 tests du vérificateur Python réussis**, code 0. Ils contrôlent les oracles sain/faute, diagnostic attendu et mauvais code ; ils n'exécutent ni compilateur, canari instrumenté, moteur ou GPU. Ce n'est ni un RED du portage ni un PASS ASan/Linux.

Log : `out/logs/mission-linux-qa/baseline-verify-asan-windows.txt`.

Identités des sources testées :
- `scripts/verify_asan.py` : `16fed95db7cb2fd6ea37ca40fa77ef9507040d886f3d5af2e6e4c0b349aad2d2`.
- `scripts/tests/test_verify_asan.py` : `d7dd7b77d2dfe24bcc2a6cfcc99c5473258ae1b8d87d9e0db6a3c1b32214d045`.

Deuxième baseline ciblée, indépendante de l'installation WSL : `C:/Python313/python.exe -B -m unittest discover -s scripts -p test_asan_env_fixture.py -v` : **9/9 réussis**, code 0. Les fixtures opèrent dans des répliques temporaires privées (TEMP/TMP du processus enfant vers le répertoire opencode approuvé) ; elles ne lancent pas les bras PowerShell réels et n'altèrent pas le build principal. Les fautes sont injectées via unittest.mock : pas preuve d'erreur OS authentique, de restauration du shell natif, d'ASan actif ou de GPU. Log : `out/logs/mission-linux-qa/baseline-asan-env-fixtures-windows.txt`. Sources : `test_asan_env.py` SHA256 `6004f567c3eede39018100c21c4ee3f043b5e8dc4c14e95466c96507856b4396`, `test_asan_env_fixture.py` SHA256 `2052876fae622890d7ec05c7984d1c534300e4d768539869df687ce032287ae6`.

## Limites conservées

La récupération automatique du qa-planner n'est pas réparée ; son livrable a été réellement récupéré via l'API native en lecture seule. Pas d'élargissement de permissions, de substitution de profil ou de modèle.

Audit indépendant encore deferred et conditionnel à son contrat hors board/permissions/restitution. WSL/CI, GPU matériel, performance, UBSan et fuites exhaustives ne sont pas présentés comme validés.

## Traçabilité native

- Contrat QA clôturé : `task_mvsa43rc_0003_5x254n`.
- Seule nouvelle publication : `task_mvsa43rc_0004_dujtol`, completion lead conservée.
- `team_status` après publication : approved v1, long_run, pool-1 idle et pool-2 build-engineer working, une tâche completed, une in_progress, six blocked.
- Worktree pool-2 : `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-ql1erh-pool-2`.
