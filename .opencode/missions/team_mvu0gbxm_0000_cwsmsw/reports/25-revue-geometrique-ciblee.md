# 25 — Vérification indépendante des références pixels

La dernière remise complète des scénarios reste le RED sémantique déjà vérifié. Le canal natif n'a pas livré de nouveau candidat ; la réponse publique la plus récente confirme que l'auteur implémente les validations Python autorisées, sans modification native ni exécution.

En parallèle, le renvoi du reviewer C++ est pris en charge par une revue shader indépendante, en lecture seule. Elle doit vérifier les calculs de géométrie, les coordonnées des points pixels et leurs marges, puis la détection d'une valeur figée, d'un draw absent ou d'une mauvaise cellule. Elle complète la revue des durées de vie ; elle ne la répète pas.

Le mandat est limité aux sources et shaders existants du candidat : aucune compilation, image, mesure ou infrastructure supplémentaire. Les données de cible matérielle en cours de modification restent hors de son axe. Les corrections éventuelles reviendront au propriétaire et devront être réauditées ; aucune preuve graphique exécutée ne peut être déduite de cette analyse.

Fiche native : `task_mvu7jrsp_000q_k359o2`, profil `shader-reviewer`, contexte fresh, sans worktree d'écriture, restitution finale, acceptation réservée au lead. Évaluation locale PM/LLM de revue : complexité 55, risque 60 ; pas de développement ni de modification de politique modèle.
