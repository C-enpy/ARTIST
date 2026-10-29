---
title: Registre des ADR ARTIST
category: index
tags: [artist, architecture, adr, index]
aliases: [ADR ARTIST, Décisions ARTIST]
summary: "Audit des huit ADR : 0001/0007 passent à partial ; décisions et preuves distinguées, coverage non prouvée, baseline Windows vérifiée à son périmètre."
created: 2026-10-03
updated: 2026-10-29
sources: [research/sessions/2026-10-29-linux-qa-session/index.json, docs/_meta/session-2026-10-29-linux-qa.json, .opencode/evidence/adr-audit-2026-10-29/inputs.json, .opencode/evidence/adr-audit-2026-10-29/public-deliveries.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Registre des décisions d'architecture

## Audit complet — 29 octobre 2026

[[references/adr-audit-2026-10-29|Obligations, étapes et critères des huit ADR]] :
audit architecture/build, recoupement source et preuves, sans nouvelle campagne.
Le maintien des statuts de décision est motivé par leur adoption documentée,
pas déduit du seul nombre de tests. Deux champs d'implémentation sont corrigés :

- **0001 Proposed/partial** : sélection de composants, N=2 et préparation float réalisées.
- **0002 Proposed/partial** : N=2 encore polymorphe ; bindings float partiellement livrés.
- **0003 Accepted/partial** : owners/cycles testés ; échecs authentiques Reader non tous exercés.
- **0004 Proposed/partial** : seule tranche float adoptée, pas de mesure d'allocation acquise.
- **0005 Accepted/partial** : coverage ON ou rejet ciblé exécuté non prouvé ; pas d'exigence de matrice idéale.
- **0006 Proposed/not-started** : protocole de mesure comparative non exécuté, prérequis QA disponibles.
- **0007 Proposed/partial** : correction initiale C++23 réalisée ; clauses cooker conditionnelles non déclenchées.
- **0008 Accepted/verified** : baseline Windows x64 Debug adoptée et prouvée ; Linux distinct.

`Proposed` ne signifie donc pas « aucun code livré ». Un cooker futur, une API
explicite, Release ou un GPU Linux matériel ne sont pas ajoutés comme conditions
artificielles des contrats actuels. Les gaps effectifs sont nommés dans l'audit.

## Actualisation des preuves — 29 octobre 2026

La [[references/session-2026-10-29-linux-qa|session Linux QA et ses sous-agents]]
enrichit les preuves d'ADR-0004/0005/0008, pas leurs statuts.
Runtime `78e7b74` : six configurations à 589/589 ; contrôleur `f58a49f` renforcé
par l'identité shader, quatre runs WSL/CI distincts également à 589/589.
[[development/linux-qa|Portée : Linux logiciel llvmpipe, sans détection de fuites]].
ADR-0005 reste Accepted/partial ; ADR-0008 reste vérifié pour sa baseline Windows
Debug seulement. La publication de rapports ou la fermeture d'une revue ne vaut
ni adoption globale d'un ADR Proposed, ni nouvelle exécution.

## Premiers contrats de correction

Les ADR-0001 et 0007 sont **Proposed / partial** après audit ; ADR-0006 reste
**Proposed / not-started** pour son protocole de mesure. ADR-0004 reste
**Proposed / partial** : seule la tranche
float scalaire OpenGL est adoptée, source `3b1b102`, 581 normaux et 581 ASan
Windows Debug, oracle pixel borné et générations locales révocables ; pas de
gestionnaire global de handles, de format cooké ni de performance mesurée.
Voir sa preuve et ses limites : ré-audit final PASS borné après deux compléments,
sans requalifier l'historique RED incomplet ou adopter tout l'ADR.
ADR-0002 reste **Proposed / partial** après réexamen de son implémentation :
la tranche d'exécution fermée du 2026-10-20 (`ClosedExecution<API>`, coexistence,
`fdbce15`) est livrée et vérifiée, sans adopter la cible terminale (tuple,
passes concrètes) ; étape de bindings partiellement réalisée par le float.
ADR-0003 est
**Accepted / partial** : la tranche Shader→Pass (2026-10-03) puis la tranche
Pipeline A1–A2/F6 (2026-10-16, C++ `f914d77`) sont vérifiées au périmètre
Windows Debug ; la tranche F7 Uniform/Attribute, Reader et findings F2/F1 est
vérifiée à `6d6dcb7` le2026-10-18 (299natifs, ASan final non exécuté).
ADR-0005 est **Accepted / partial** ; ADR-0008 est **Accepted / verified** dans
son périmètre Windows Debug. Voir les preuves propres à chaque décision.

- [[adr/0001-resolution-multi-etapes|ADR-0001 — Résolution multi-étapes]] : spécialiser au moment où l'information est réellement connue.
- [[adr/0002-preparation-et-execution-specialisee|ADR-0002 — Préparation et exécution spécialisée]] : séparer l'API flexible du chemin fin et migrer une tranche.
- [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 — Ownership et lifetime CPU/GPU]] : owners et cycles de vie vérifiés par tranches ; Accepted/partial, chemins d'échec de préparation encore partiellement prouvés.
- [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004 — Layouts typés et données préparées]] : résoudre noms/types avant la boucle et valider l'invalidation.
- [[adr/0005-build-reproductible-et-validation|ADR-0005 — Build reproductible et validation]] : restaurer une baseline propre avant les comparaisons.
- [[adr/0006-preuves-performance-et-determinisme|ADR-0006 — Performance et déterminisme]] : protocole A/B, coût total et critères distincts CPU/GPU/image.
- [[adr/0007-cpp23-et-cooker-independant|ADR-0007 — C++23 et cooker indépendant]] : conserver le runtime actuel, évaluer un autre langage sur preuves.
- [[adr/0008-conan2-et-baseline-windows|ADR-0008 — Conan 2 et baseline Windows]] : dépendances exactes installées et build/tests exécutés.

L'ordre de travail est dans [[architecture/correction-plan|le plan de correction]].

## Cycle et autorité

- `Proposed` : contrat argumenté à examiner au début de la tâche qui l'applique.
- `Accepted` : choix explicitement adopté ; noter l'origine de cette décision.
- `Superseded` : conserver l'historique et pointer vers la décision remplaçante.

Le champ `implementation` est indépendant : `not-started`, `partial`,
`implemented`, `verified`. `verified` exige un périmètre et des preuves datées ;
une décision acceptée peut encore être totalement absente du code.

Lorsqu'une tâche adopte explicitement un contrat proposé, mettre à jour ce statut
avec sa référence. Ne pas présumer une acceptation parce qu'un agent a écrit l'ADR.
Un correctif local ne nécessite pas une nouvelle ADR.

## Rédaction

Utiliser [[adr/template|le modèle]] et `NNNN-sujet.md`. Distinguer faits source,
hypothèses et décision ; citer chemins/symboles, alternatives, migration, critères
discriminants et limites. Éviter les seuils numériques arbitraires issus des rapports.

Retour : [[index|Accueil]] · [[engineering-principles|Principes]] ·
[[architecture/current-state|État réel]].
