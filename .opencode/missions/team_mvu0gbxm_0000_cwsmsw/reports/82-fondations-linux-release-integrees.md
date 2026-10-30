# 82 — Préparation Linux Release intégrée, sans exécution Linux native

Les trois fichiers de préparation Release sont intégrés par merge natif et correspondent exactement au candidat déjà vérifié. Le helper transmet explicitement Debug/Release aux profils host/build Conan avant CMake ; les presets configure/build/test restent cohérents et séparés de Debug et ASan. Aucun adaptateur commun n'est ajouté à cette étape.

Le coordinateur a relu le diff et le confinement des tests, puis vérifié l'intégration : **6/6 tests Release et 19/19 contrôles historiques satisfaits**, sans échec, erreur ou skip. Les historiques sont exécutés sur des copies exactes dans une fixture possédée pour isoler leurs créations de répertoires. Empreintes sources et copies stables.

Il s'agit de tests Python sous Windows, avec commandes Conan/CMake/CTest doublées, et d'un enfant Python fini retournant 7 dans un test historique. Aucun outil moteur ni environnement Linux réel n'est lancé : la préparation native Linux Release reste NON_EXECUTE.

Le raccordement Linux attend encore les interfaces communes intégrées. Après arrêt de l'ancien auteur, le runtime a repris la tâche originale sur pool-19 ; le coordinateur a immédiatement limité cette reprise à lecture/plan et restitution, sans écriture ni tests avant disponibilité des entrées. La tâche reste ouverte, sans fausse complétion ou cycle d'arrêt répété.

## Preuves

- Candidat reçu `42828c3a01f997783a3d86da4013a99bf4ef5309` ; commit intégré `d2d7be70b5d1b82816c03faab7135a9f1405eb4b`.
- Fichiers : `CMakePresets.json`, `scripts/build-linux.py`, `scripts/tests/verification_linux/test_release_contract.py`.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s <suite> -p <filtre> -v`, suite intégrée `scripts/tests/verification_linux` avec `test_release_contract.py`, puis fixture historique `scripts/tests` avec `test_build_linux.py` et `test_linux_evidence.py`.
- Auteur Release : linux-pipeline ; runner réel : coordinateur. Journaux, cwd exacts, empreintes et copies : `out/verification-synthetic/linux-foundations-integrated-2026-10-30T04-46-03-680Z/`.
- RED/GREEN antérieurs réutilisés : `linux-release-red-fixture-2026-10-29T23-13-12-017Z`, `linux-release-green-2026-10-29T23-55-14-687Z`, non-régression historique `linux-helper-nonregression-2026-10-29T23-57-13-844Z`, sous le même dossier de preuves.

Aucune compilation, suite moteur, OpenGL, rendu, benchmark, installation ou ressource cloud exécuté ou réservé. Le pipeline complet reste à réceptionner.
