---
title: Plan de correction de l'existant
category: architecture
tags: [artist, architecture, testing]
summary: Séquence de travaux réversibles et critères de passage des ADR vers une implémentation vérifiée.
status: proposed
created: 2026-10-03
updated: 2026-10-15
---

# Plan de correction de l'existant

Ce plan est dérivé des [[references/research-reports|quatre rapports]] et de
[[architecture/current-state|la photographie source]]. Ce n'est pas un calendrier
de livraison. Les durées et pourcentages des rapports ne sont pas des budgets ratifiés.

## Mission sélectionnée — 2026-10-15

[[architecture/next-mission|Assignation move vers un Shader/Pass chargé, puis ASan]].
Fermer ce risque avant la migration Pipeline. Contrat, périmètre autorisé et
critères de preuve définis ; **implémentation non commencée**. Baseline rejouée :
122/122 sous Windows Debug, code committé en `4d29842`.

## 0 — Obtenir une baseline exécutable

**Réalisé sous Windows Debug le 2026-10-03** : build complet et 109/109 tests
(87 CPU, 22 OpenGL). Voir [[adr/0008-conan2-et-baseline-windows|ADR-0008]] et
[[development/build-and-test|les preuves]]. Coverage et autres plateformes
restent à qualifier. L'étape 1 peut désormais partir de cette baseline.

Appliquer le contrat de [[adr/0005-build-reproductible-et-validation|ADR-0005]] :
Conan compatible et toolchain identifiée, coverage optionnel, découverte CTest
fiable. Capturer les résultats CPU, puis les préconditions OpenGL. Un contexte
absent ne bloque pas les preuves CPU, mais laisse le critère GPU INCONCLUSIVE.

**Sortie** : commandes propres reproductibles, nombre non nul de tests, premier
écart causal documenté. Aucun changement d'architecture attribué à un simple
problème de dépendances.

## 1 — Fermer les contrats de la tranche

Choisir une petite chaîne OpenGL représentative et ses fixtures. Définir
comportement, ordre des passes, erreurs, ownership et équivalence visuelle selon
[[adr/0001-resolution-multi-etapes|ADR-0001]],
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] et
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]].

**Tranche Shader→Pass — réalisée le 2026-10-03** : migration franche d'ownership
(owner unique `unique_ptr` sur `IShader`/`PassContext::m_shaders`, emprunts en
références, composants GL `on(Context&)`, `Pass::shared()` supprimé). Suites :
95/95 unit + 27/27 OpenGL PASS, après les corrections de revue et le rejet des
re-loads sur handles vivants. Voir [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
et [[development/build-and-test|les preuves]]. Restent dans la tranche : le contrat
visuel/équivalence (ADR-0006, images figées) et le relevé workload — la tranche
couvre les cas d'ownership et d'erreurs testés, pas tous les chemins de lifetime.
ASan et l'assignation par déplacement vers un owner chargé restent à qualifier.
Les [[development/known-incidents|findings non traités]] ne sont pas annulés par
ces suites vertes. L'avis d'architecture propose ensuite `PipelineContext::m_passes`
en ownership unique et `forPass` par emprunt, puis le contexte de `IPass` ; ce
travail n'est pas encore implémenté. Durcir `UniformReader` lors de la tranche
uniform/attribute, avec un test de concept négatif.

**Sortie** : baseline caractérisée, configurations valides/invalides, données et
workload figés ; aucune performance inventée faute de mesure.

## 2 — Introduire un chemin d'exécution spécialisé

Suivre [[adr/0002-preparation-et-execution-specialisee|ADR-0002]] : coexistence
contrôlée avec l'API existante, types concrets pour la topologie fermée, context
emprunté, dispatch externe par famille/lot si nécessaire. Migrer un consommateur,
pas toutes les primitives en une seule réécriture.

**Sortie** : mêmes comportements voulus, erreurs vérifiées, appels générés inspectés
si une suppression de dispatch est revendiquée. Les anomalies de l'ancien
comportement sont traitées explicitement, pas figées parce qu'un mock les reproduit.

## 3 — Préparer les données et bindings

Suivre [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] : noms/réflexion à
la préparation, layouts typés et IDs validés, invalidation lors du relink/reload.
Définir les coûts autorisés lors d'une création et ceux autorisés à chaque frame.

**Sortie** : preuve de compatibilité CPU/GPU, erreurs de type et handle périmé,
profil d'allocation/lookup du chemin migré.

## 4 — Décider à partir du coût total

Comparer ancien chemin, runtime optimisé par lots et spécialisation statique
selon [[adr/0006-preuves-performance-et-determinisme|ADR-0006]]. Conserver les
données brutes, qualité, code size et coûts de build/cook. Une expérience
scene-to-code est bornée ; elle n'est pas automatiquement le produit livré.

**Sortie** : gains et régressions observés, ou INCONCLUSIVE. La question d'un
prototype Rust ne s'ouvre que sur une baseline C++ corrigée et selon
[[adr/0007-cpp23-et-cooker-independant|ADR-0007]].

## À chaque étape

- Relier le diff et les résultats à l'ADR, mettre à jour `implementation` seulement
  pour le périmètre prouvé et corriger [[architecture/current-state|l'état réel]].
- Garder un chemin de comparaison/retour ; ne retirer une compatibilité qu'après
  migration de ses consommateurs et décision explicite sur l'API publique.
- Mettre à jour [[log|le journal]] et l'index QMD selon
  [[_meta/wiki-maintenance|la procédure]].
