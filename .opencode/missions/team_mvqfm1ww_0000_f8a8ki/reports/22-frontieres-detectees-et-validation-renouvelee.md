# 22 — Deux frontières détectées, validation renouvelée à 581 cas

## Résultat

Les deux nouveaux tests passent au nominal et détectent chacun la régression
visée. Après restauration exacte des headers, le lead a reconstruit entièrement
les deux configurations et exécuté les cinq commandes convenues : **581 normaux
et 581 ASan**, chacun 506 CPU + 75 OpenGL, mêmes noms uniques, zéro failure/error/skip.
Canari ASan 0/86, aucun marqueur d'erreur ASan tardif dans les logs finaux.
Le ré-audit indépendant doit encore décider la fermeture des deux gaps.

## Expériences et exclusions

F1 : retirer la garde de contexte de prepare fait retourner un accès sous chacun
des deux contextes invalides et ajoute une requête GL. Compilation0/test8, un test
en échec pour ces assertions. Source restaurée et vérifiée avant F2.

F2 : le premier essai recompilant seulement le fichier de test est passé, mais
la définition inline Freer d'origine restait dans d'autres objets. Le lead a
consigné l'hypothèse de définition mixte, puis reconstruit tous les objets unitaires
avec le mutant. Le nouveau binaire détecte le témoin valide dès le premier effet,
deux writes acceptés et les mutations query/upload/mirror : compilation0/test8,
un test en échec. L'essai partiel initial est conservé mais exclu, pas présenté
comme preuve de survie d'un mutant cohérent. Aucun test ou oracle corrigé pour
obtenir l'échec : il s'agissait de corriger la méthode de build expérimental.

Les deux headers retrouvent leurs empreintes initiales ; aucune production
modifiée dans le candidat livré. Le budget healer reste1/2, aucune réparation
supplémentaire nécessaire pour les deux propositions.

## Preuves et prochaine étape

Source finale `3b1b1028dcd64ca46e0a359365ae096be1a38ac4`, quinze empreintes
source et cinq binaires/probe recoupés. Archives nouvelles séparées des579
précédents : `out/qa-prepared-float/lead-followup-20261027/`. C/L/A antérieurs
réutilisés après comparaison exacte de leur test et des headers, pas nouveaux
mutants C/L/A revendiqués. La campagne globale581 est entièrement renouvelée.

[Entrées neutres du ré-audit](../reaudit-input.md), manifeste final-validation.json
SHA256 `29a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b`.
Les commandes convenues et sorties sont les préfixes06/07/09/10/13, JUnit uniques.
Le ré-audit reste hors board, même session indépendante, puis photographie et
clôture documentaires. Aucun nouveau GO ou changement d'objectif requis.
