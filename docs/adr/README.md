---
title: Registre des ADR ARTIST
category: index
tags: [artist, architecture, adr, index]
aliases: [ADR ARTIST, Décisions ARTIST]
summary: Registre des huit ADR, des propositions architecturales à la baseline Windows Conan 2 vérifiée.
created: 2026-10-03
updated: 2026-10-03
---

# Registre des décisions d'architecture

## Premiers contrats de correction

Les ADR-0001, 0002, 0004, 0006 et 0007 restent **Proposed / not-started**.
ADR-0003 est **Accepted / partial** (tranche Shader→Pass vérifiée Windows Debug).
ADR-0005 est **Accepted / partial** ; ADR-0008 est **Accepted / verified** dans
son périmètre Windows Debug. Voir les preuves propres à chaque décision.

- [[adr/0001-resolution-multi-etapes|ADR-0001 — Résolution multi-étapes]] : spécialiser au moment où l'information est réellement connue.
- [[adr/0002-preparation-et-execution-specialisee|ADR-0002 — Préparation et exécution spécialisée]] : séparer l'API flexible du chemin fin et migrer une tranche.
- [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 — Ownership et lifetime CPU/GPU]] : migration franche Shader→Pass exécutée ; tranches suivantes à venir.
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
