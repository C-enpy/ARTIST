# 112 — Adaptateur Linux intégré, raccordement effectif vert

Après avis indépendant favorable sur la composition avec le runner corrigé, le
coordinateur a intégré les seuls fichiers Linux par merge natif. Les **dix fichiers
reçus correspondent exactement au candidat revu** ; le runner corrigé de l'arbre
principal est conservé, sans écrasement par l'ancien runner du worktree auteur.

Le GREEN du consommateur est obtenu : **4/4 tests d'environnement effectif**, puis
**34/34 binding, 6/6 Release et 19/19 historiques**. Cœur **201 succès et trois SKIP
sur204**, contexte **28/28**, scénarios **31/31**. Aucun échec ou erreur ; empreintes
et copies historiques confinées stables. Les trois liens symboliques restent
non exécutés faute de droits Windows.

Le finding d'héritage logiciel est donc fermé à cette portée : description Linux
réelle et runner commun corrigé, factory en mémoire. Les tâches d'auteur et revue
Linux peuvent être closes sur leurs livrables réellement intégrés. Le brouillon
historique inactif reste préservé et n'a pas été consommé.

La livraison complète attend encore Windows, le raccordement optionnel CMake/CI/
documentation et la QA finale proportionnée. Aucun consommateur de ces deux
adaptateurs n'est libéré tant que Windows n'est pas intégré ; les préparations
documentaires indépendantes restent possibles sans mutation de fichiers partagés.

## Preuves

- Ré-audit `result_mvunkxkt_0000_v5a53t` ; candidat Linux `fa78654d4d62e8a0b31ef2b2c6cc4be183db3505`.
- Commit intégré `7a1acbffd7c8d050f02fb4cdaf928a154dbc8c3f`, runner blob `cdf2408b1f83267cfe147c03cfe1a0cc9cc6c2bd`.
- Auteur pool-24 ; runner coordinateur. Commandes `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/<suite> -p <filtre> -v` ; filtres effectif, binding, Release puis historiques confinés ; suites core/contexte/scénarios. Commandes/cwd exacts dans le manifeste.
- Logs et empreintes : `out/verification-synthetic/linux-binding-integrated-2026-10-30T07-32-22-960Z/`.

Tests Python exécutés sous Windows, pas sous Linux. Aucun outil de build réel,
moteur, OpenGL, rendu, benchmark, installation ou cloud exécuté. Linux natif et
GPU restent NON_EXECUTE : livré pour qualification, pas fonctionnement démontré.
