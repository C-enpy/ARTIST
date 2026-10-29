# 02 — Correction du catalogue refusé

2026-10-25. Intervention locale indivisible de configuration ; mission toujours non approuvée, aucune exécution libérée.

## Objectif et cause

Corriger `Execution catalogue contains a disabled, invalid or unauthorized model` sans réautoriser Cursor/opencode-go. La correction du rapport01 était incomplète : seuls les providers avaient été retirés, laissant15 couples modèle/variante de ces providers dans `models`.

Cause vérifiée dans le plugin installé `C:/Users/djden/.config/opencode/local-plugins/ensemble/20261025-pm-preparation-fdbbe025cbf1/index.js`, `validateExecutionPolicy`, lignes13550–13570 : **chaque entrée** doit être activée, avoir un ID valide et appartenir à un provider autorisé. Ce validateur ne filtre pas silencieusement les entrées interdites.

## Changements et preuves

- Modification du seul brouillon courant `../model-policy.json` : suppression des15 entrées Cursor/opencode-go, conservation des14 entrées autorisées, des scores, seuil50, beta0.35 et critères QA proportionate. Aucun fichier global/plugin/base native modifié.
- RED : fonctions du validateur installé et dépendances extraites en mémoire, sans importer le plugin ni ses effets de bord ; application au brouillon avant correction reproduisant exactement l'erreur affichée.
- GREEN : même validateur appliqué au fichier corrigé sur disque → PASS ;14 couples restants,7 admissibles au seuil de planification.
- Contrôle négatif en mémoire : ajout d'un modèle opencode-go fictif → rejet avec la même erreur attendue. Aucun modèle fictif écrit sur disque ni appel provider.
- Commandes réelles : lectures ciblées par grep/read et scripts JavaScript `ctx_execute` (fs/vm) pour exécuter les fonctions de validation. Aucun build/test C++ ou worker.
- SHA256 corrigé : `742bd7cc2b28aa91f5c5c1de946ab9af3ebcebdbe319886c87b301bbe866b6c5`.

## Limites et suite

PASS de validation locale du brouillon, pas preuve d'une commande UI réussie, d'une disponibilité provider native ou d'une approbation. Aucun redémarrage ou installation nécessaire établi. Faire présenter le fichier par `/ensemble-mission config` ; ne lancer `start` qu'après examen et résolution du rattachement décrit au rapport01. Le problème d'équipe historique est distinct et reste ouvert.

Cette réparation corrige ma configuration précédente ; elle n'élargit ni le périmètre ARTIST ni la QA, et ne change pas la politique de modèles autorisés. Pas de commit ni de modification des diffs concurrents.
