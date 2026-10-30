# 28 — Références pixels cohérentes, limites de détection conservées

## Résultat de la revue indépendante

Le shader-reviewer ne démontre aucun défaut bloquant de géométrie ou de référence couleur dans les quatre points contrôlés. Le coordinateur a recoupé les sommets, le shader, les viewports, les coordonnées lues et les fenêtres chronométrées.

Dans une cellule de 32 pixels, les sommets sont `(4,4), (28,4), (16,28)` à zéro et `(12,12), (36,12), (24,36)` au déplacement `.5`, avant clipping. Les centres des pixels contrôlés sont éloignés des arêtes ; la plus petite marge intérieure est d'environ 2,01 pixels. Le blanc et le noir opaques sont cohérents avec le framebuffer linéaire et les états explicitement désactivés. L'égalité entre routes complète ces références indépendantes, sans les remplacer.

## Deux limites non bloquantes

Une permutation complète des cellules identiques ne change pas l'image : l'oracle détecte omissions et duplications, pas toute erreur d'identité logique de cellule. Ajouter un marqueur spécifique serait une extension non requise de cette livraison.

Les fenêtres chronométrées ne lisent que leur dernière phase. Le contrôle distinct examine la séquence complète, mais les pixels finaux ne prouvent pas chaque draw ou image intermédiaire du chemin mesuré. Aucun scintillement absent ni exhaustivité temporelle n'est donc revendiqué.

Ces limites sont retournées au propriétaire pour la documentation, sans nouveau banc ni instrumentation dans la fenêtre mesurée. Le défaut C++ du producteur CPU et le risque d'encodage Windows restent séparément ouverts.

## Preuves et portée

Revue native `result_mvu7m96w_000s_r34ufc`, profil shader-reviewer, contexte indépendant. Candidat observé `cbf39ce183c61cd62c2b3a8a48ab220e5b4214af` ; bloc géométrie/oracle inchangé pendant la revue.

SHA-256 `OpenGLFloatWorkload.cpp` : `2db9c10b7393ae100bb47ae5f01ffcb2d939c382d583ebd4446782d2ec133f80`. Fichiers et points examinés : lignes 255–275, 294–340, 490–518 ; shaders `f2-single-values.vert` et `minimal.frag`. Référence historique : `PreparedFloatUniformRenderTests.cpp:132–139`, présence statique seulement.

Aucun build, rendu ou benchmark exécuté. Cohérence mathématique observée ; preuve d'image et qualification Windows/Linux NON_EXECUTE. Cet avis ne vaut pas PASS graphique global.
