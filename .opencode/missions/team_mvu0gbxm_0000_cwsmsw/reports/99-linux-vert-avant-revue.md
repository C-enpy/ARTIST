# 99 — Raccordement Linux vert, revue avant intégration

Le pipeline Linux doit séparer préparation, construction et exécution sans
déclencher de campagne. Le coordinateur a relu les deux fichiers de production :
descriptions directes, métadonnées séparées et modes helper ratifiés. Le candidat
est propre ; tests nouveaux et historiques sont inchangés depuis le RED.

Résultats exécutés par le coordinateur : **34/34 nouveaux tests, 6/6 Release et
19/19 historiques**, aucun échec, erreur ou skip. Les historiques utilisent des
copies exactes confinées ; empreintes des sources et copies stables. Les appels
d'outils sont doublés ; seul l'enfant Python factice fini déjà prévu par les
tests historiques est lancé.

Une revue indépendante fresh est publiée sur les chemins, reçus, commandes,
profils, collecte de métadonnées et préservation du comportement historique.
Le candidat reste stable et non intégré. Les consommateurs attendent sa réception
effective après avis, merge natif et vérification dans l'arbre principal.

## Preuves

- Candidat `3e697590c03869f53b64aec8dde0cd706bceff52`, auteur pool-24 ; runner : coordinateur.
- Fichiers de production : `scripts/verification/adapters/linux.py`, `scripts/build-linux.py`.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p <filtre> -v`, filtres `test_binding_*.py` puis `test_release_contract.py` ; historiques en fixture avec `-s scripts/tests`, filtres `test_build_linux.py` et `test_linux_evidence.py`.
- Logs, cwd et empreintes : `out/verification-synthetic/linux-binding-green-2026-10-30T06-46-39-327Z/`.
- Revue `task_mvum0xq5_0059_mkoad4`, contexte fresh, lecture seule ; PM/LLM67/71.

Ces tests Python sont exécutés sous Windows. Linux natif, outils de build réels,
moteur, OpenGL, rendu et performances restent NON_EXECUTE. Aucune installation
ou réservation cloud ; aucun fonctionnement natif démontré.
