# 45 — Conserver la couverture matérielle des tests historiques

## Constat et recommandation retenue

L'architecte confirme que les nouveaux workloads qualifient leur contexte réel, mais que les trois cas OpenGL historiques sélectionnés ne produisent que des chaînes fabricant/renderer/version. Le refus actuel du collecteur survient après leur éventuelle exécution ; il ne remplace pas une garde avant les programmes et uploads.

Le coordinateur retient la solution minimale recommandée : **observation et garde additives, activées explicitement dans la fixture commune des tests historiques**, avant le corps du test. Aucun changement des assertions ou des cas négatifs intentionnels. Les observations doivent appartenir au contexte réellement utilisé ; un préflight dans un autre exécutable ne suffit pas.

Les deux sélections mélangent trois cas OpenGL au total et des cas CPU mockés : ces derniers ne deviennent pas des preuves GPU. Le collecteur devra demander l'observation structurée à chaque cas OpenGL matériel, pas aux cas CPU.

## Préparation avant source

Cette recommandation n'est pas encore une implémentation autorisée. Le format versionné, son attribution par cas, son transport explicite depuis le plan, l'ownership du fichier historique et les critères QA seront persistés avant toute écriture auteur. Hors activation, le comportement historique reste inchangé. Les préconditions refusées, échecs d'assertion et pannes de protocole devront rester distincts, avec leurs sorties brutes.

La solution n'ajoute ni backend, ni dépendance, ni campagne, ni nouvelle variante CMake. Compilation et exécution C++/GL resteront NON_EXECUTE ; les contrôles exécutables porteront uniquement sur les données synthétiques et programmes factices.

Réduire les deux sélections au logiciel serait plus simple, mais supprimerait leur route Windows v1, où le logiciel n'est pas pris en charge. Cette réduction n'est pas retenue silencieusement. Le mandat de livraison Windows/Linux est conservé.

## Preuve documentaire

Proposition indépendante `result_mvuc90ln_001q_zom79m`, profil engine-architect, lecture seule. Sources : `ARTIST/component-tests/include/OpenGLComponentTests.hpp`, les trois cas sélectionnés, descriptions des scénarios et `evidence.py::_junit`. Aucun test, build, rendu ou édition par l'architecte. La validation du cœur reste 124 succès et un symlink non exécuté, pas un PASS global.
