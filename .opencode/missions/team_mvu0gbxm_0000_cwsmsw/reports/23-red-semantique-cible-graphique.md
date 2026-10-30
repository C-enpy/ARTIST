# 23 — Les tests détectent les fausses correspondances graphiques

## Résultat

Le coordinateur a relu le socle nominal et le diff des scénarios, puis exécuté les douze nouveaux tests de cible graphique : **51 échecs d'assertion avec sous-cas**, aucun error ou skip. Les contrôles atteignent désormais les mauvaises paires, les chaînes invalides, les observations différentes, les renderers logiciels et les contextes non conformes. Les validations correspondantes ne sont pas encore implémentées : c'est le RED attendu, pas une erreur d'environnement.

Les **17 tests historiques passent**, sans modification des assertions. Les sources natives sont inchangées et les empreintes des fichiers contrôlés restent stables pendant les runs.

L'auteur peut maintenant implémenter les validations minimales Python. Le C++ reste soumis à sa revue indépendante et à la future vérification statique de son transport ; aucune compilation ni exécution graphique autorisée.

## Preuves et attribution

Candidat `1834af6f748bbb49eef7bfb227c2af04e923666f`, auteur uniform-scenarios, runner coordinateur. Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_hardware_expectation.py -v`, puis même commande avec `test_contract.py`.

Journaux complets et manifeste : `out/verification-synthetic/scenario-hardware-semantic-red-2026-10-29T23-59-52-502Z/`.

Ce résultat qualifie la sensibilité des tests synthétiques ; il ne démontre aucun matériel réel, rendu ou gain de performance. Le candidat reste intermédiaire et non intégré.
