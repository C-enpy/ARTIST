# 100 — Bloquer l'héritage logiciel dans une exécution matérielle

La revue du raccordement Linux bloque son intégration : les commandes matérielles
déclarent correctement leur cible, mais le runner copie l'environnement parent.
Un lancement depuis `LIBGL_ALWAYS_SOFTWARE=true` et `GALLIUM_DRIVER=llvmpipe`
transmet donc encore ce forçage aux enfants matériels. Une machine équipée peut
échouer en contexte logiciel ; aucun contournement du contrôle de cible exacte
ou faux succès n'est démontré.

Le coordinateur a recoupé les lignes du runner et le modèle de commande. Le modèle
accepte seulement des valeurs texte : simuler une suppression avec None ou une
valeur vide inventée n'est pas une correction admise. Lire le parent dans les
descriptions violerait aussi leur pureté.

L'auteur Linux prépare une régression sur l'environnement effectivement transmis
à un faux processus, sans exécution. Une consultation d'architecture est demandée
sur une correction privée du runner limitée au contexte matériel explicitement
demandé, sans nouveau schéma ni mutation de l'environnement parent. Le contrat
et l'ownership du petit delta commun seront fixés avant implémentation ; aucune
permission d'éditer le runner n'est encore donnée à l'auteur Linux.

Les 59 tests verts restent acquis mais ne couvrent pas cet héritage effectif.
L'intégration seule est bloquée ; les travaux Windows indépendants continuent.

## Références et portée

- Avis indépendant `result_mvum8csu_005b_84umaw`, lecture seule, aucun test.
- Candidat Linux `3e697590c03869f53b64aec8dde0cd706bceff52`.
- `scripts/verification/adapters/linux.py::_environment` ; consommateur
  `scripts/verification/runner.py::SubprocessRunner.run`, lignes44–47 ;
  `scripts/verification/model.py::CommandSpec`, lignes159–167.
- Résultats verts antérieurs : rapport99 et manifeste cité, sans relance.

Reproduction dynamique NON_EXECUTE à ce stade. Aucun outil natif, moteur,
OpenGL, rendu, benchmark, installation ou cloud lancé.
