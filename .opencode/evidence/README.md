# Preuves de travail locales

Les fichiers voisins sont ignorés par Git ; ce README est versionné.
Utiliser un sous-répertoire par tâche, sans écraser les preuves d'une autre session.

Une transmission durable contient : objectif et périmètre, HEAD et état du diff,
chemins/symboles, contrat attendu, findings localisés, commandes et environnement,
extraits de résultats, verdicts, tentatives déjà faites et prochain test discriminant.

Captures, images et mesures brutes restent locales. Une conclusion utile à long
terme va dans la documentation ou une ADR avec son protocole reproductible.
Revérifier les références après tout changement de HEAD/diff ; une ancienne preuve
ne valide pas automatiquement le nouveau code.
