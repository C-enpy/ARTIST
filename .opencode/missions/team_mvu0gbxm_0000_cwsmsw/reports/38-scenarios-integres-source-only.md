# 38 — Scénarios intégrés, sans qualification native

## Livrable effectivement intégré

Après avis indépendant favorable et correction des deux défauts identifiés, le coordinateur a arrêté l'auteur puis utilisé le merge natif. Les **20 fichiers intégrés correspondent exactement au candidat revu**, comparaison SHA-256 des objets Git contre les fichiers reçus, sans divergence.

Le commit intégré contient les producteurs CPU et OpenGL, le protocole natif borné, le CMake local et le manifeste Windows, les descriptions des scénarios, le protocole Python et ses tests. Les rapports d'étapes conservés dans le sous-dossier sont historiques, pas des résultats natifs supplémentaires.

## Vérification après intégration

Dans l'arbre principal, **31/31 tests synthétiques Python passent**, aucun error ou skip. Les empreintes des 20 fichiers restent stables pendant le run. Les revues indépendantes C++ et géométrique sont acquises dans leur portée ; le dernier ré-audit lève statiquement le filtre trop restrictif des cibles.

La livraison reste **SOURCE_ONLY / pour qualification ultérieure** : aucun C++ compilé, aucun manifeste incorporé dans un exécutable testé, aucun moteur ou rendu exécuté et aucune mesure. Les limites de permutation des cellules et d'observation de la dernière phase chronométrée restent documentées.

## Dépendances restantes

Le cœur et les adaptateurs Windows/Linux ne sont pas encore intégrés. Les options CMake parentes, le catalogue central composé, la CLI, les phases réelles et les documents de livraison complète restent aux producteurs/raccordements prévus. La fin de cette branche n'est pas un PASS du pipeline global. Les consommateurs d'intégration globale et QA finale restent différés jusqu'à leurs véritables entrées.

Les changements utilisateur des profils d'agents, les rapports de l'autre mission et `.tmp` sont restés hors commits.

## Annexe de preuves

- Candidat auteur revu : `1eaa1875cd3757fc94ab84f4c4992a5124ce6781`.
- Intégration native, puis commit : `cc26104d1ccc6c064218d84ebd22f35db389e3d2`.
- Dernier avis C++ : `result_mvu8vwk1_0000_ge4483` ; revue géométrique : `result_mvu7m96w_000s_r34ufc`.
- Commande après intégration : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v`.
- Auteur des tests : uniform-scenarios ; runner réel : coordinateur.
- Logs, commande et 20 empreintes avant/après : `out/verification-synthetic/scenarios-integrated-2026-10-30T00-41-19-139Z/`.
