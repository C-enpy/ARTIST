# 35 — Aligner les cibles non vides entre Python et C++

## Défaut et décision

Le ré-audit indépendant démontre un écart ciblé : le parseur natif rejette une cible composée d'un espace avant création du résultat, alors que le contrat et le protocole Python admettent une chaîne non vide sans contrôle. Une cible syntaxiquement admissible mais discordante doit produire une précondition indisponible avec observations, pas une erreur de protocole sans JSON.

La lecture du modèle du cœur montre également un filtre `strip` plus restrictif. Le coordinateur conserve le contrat ratifié : **non vide, chaîne exacte, sans normalisation**. Espaces seuls et espaces Unicode sans contrôle restent admissibles ; ils ne qualifient aucun matériel par eux-mêmes. Aucun nouveau champ ou élargissement de campagne.

## Corrections demandées

Le propriétaire des scénarios corrige uniquement le filtre des deux options cibles natives, puis remet le diff pour ré-audit statique. Le propriétaire du cœur ajoute d'abord un test positif discriminant sur espace et espace Unicode : le rejet actuel doit être observé avant suppression du filtre trop strict. Les contrôles de chaînes vides, caractères interdits et limite 4096 restent inchangés.

Les tests verts antérieurs sont conservés comme preuves des cas couverts, pas comme preuve exhaustive de conformité. La réception source reste bloquée sur cet écart ; les autres travaux autorisés continuent.

## Autres résultats de la revue

Aucun autre défaut démontré sur décodeurs Unicode, ownership des arguments, chemins natifs, options CMake locales, contrôles de contexte/cible et ordre de destruction. Ce constat ne prouve ni compilation ni exécution. La géométrie déjà revue n'a pas été réanalysée.

Avis natif `result_mvu8p34c_0000_6de7kv`, candidat `fa828094e67f9ec17eb7b76e38537627d04b0810`. Localisation : `WorkloadProtocol.hpp:190`, `_target_text` dans le protocole Python, `_hardware_target` dans le modèle du cœur. La note `hardware-expectation-v1.md` explicite la distinction existante non vide/non blanc. Aucun build, moteur, rendu ou benchmark lancé.
