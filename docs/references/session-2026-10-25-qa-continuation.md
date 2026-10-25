---
title: Continuation QA ARTIST — preuves acquises et clôture native bloquée
category: reference
tags: [artist, testing, tooling, reference]
summary: "Audit indépendant accepté et ASan555 vérifié ; livrables bornés terminés, clôture native bloquée par le graphe hérité, dettes F8/C2/B1/GCC conservées."
created: 2026-10-25
updated: 2026-10-25
source_revision: 82bf52d52e594fae02593866669a1af715127d1f
sources:
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/21-audit-final-accepte.md
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/22-bilan-final-et-dette-board.md
provenance:
  extracted: 0.95
  inferred: 0.05
  ambiguous: 0.0
---

# Continuation QA — 2026-10-25

## Deux états qui ne se confondent pas

Les **livrables de continuation sont terminés à leur portée**, avec audit final
indépendant reçu et accepté. **La mission native n'est pas clôturée** : son graphe
importé conserve des dépendances obsolètes. Cette différence ne constitue ni un
FAIL des tests ni une permission de fabriquer des statuts completed.

La [[architecture/current-state|photographie source]] reste `82bf52d` ;
la documentation/audit a été intégrée à `fc830a7`, puis le bilan de dette native
à `c840a64`. Aucune production n'a changé durant cette continuation ; le correctif
local est dans un seul test lifetime. Voir [[development/known-incidents|incidents]]
et [[architecture/correction-plan|plan de correction]].

## Ce que les preuves couvrent réellement

- **ASan final555/555** :484CPU+71OpenGL, zéro failure/error/skip ; mêmes noms
  découverte/sélection/exécution/JUnit,17historiques chacun une fois. Runner lead,
  sélection pré-run, parallèle1, bail GL et diagnostics tardifs contrôlés.
- **Normal** :484CPU réexécutés,71GL historiques réutilisés après contrôle
  d'identité. Ce n'est pas une nouvelle campagne normale555.
- **Archives** : le rouge initial554/555 reste FAIL à son candidat ; le vert est
  `out/qa-b2-h3/final-h4-20261025-01/`. Préflight/canari réutilisés par identité,
  pas rejoués pour remplir un bilan. Commande et environnement dans
  [[development/build-and-test|build et tests]], identités dans
  `_meta/session-2026-10-25-qa-continuation.json`.
- **Audit final** : test-reviewer fresh, PASS borné O1–O6 sans finding bloquant
  démontré. Il relit helper strict, consolidation des attendus, ProgramUnbinder,
  const/fallback Windows, assertions lifetime et preuves d'intégration.
  Assertions = présence statique ; mutants mentaux = inférence ; exécutions =
  archives du lead. Le reviewer n'a ni modifié ni exécuté de tests.

Profil natif, lectures canoniques, contexte et restitution ont été vérifiés avant
analyse. L'adaptation approuvée autorise une tâche dédiée et un worktree RO ; elle
ne prouve ni invisibilité technique du board ni sandbox. Les pins anciens du
profil/skill n'ont pas été appliqués au candidat courant.

## Dette du graphe natif

La tâche d'audit dédiée est réellement completed après intégration. Cependant,
l'ancienne tâche ASAN reste in_progress/inactive, l'ancien AUDIT est blocked par
ASAN, puis CLOTURE est blocked par AUDIT. La tentative de compléter CLOTURE a
été refusée pour dépendances non résolues. Le nouvel audit intégré n'a pas
remplacé automatiquement l'ancien nœud.

Libérer ASAN avec auto-dispatch actif risque de relancer l'ancien audit sur une
fiche/routage obsolètes. La voie exposée ne permettait pas de remplacer/mettre en
attente ces tâches ni de retargeter leurs dépendances. Aucune modification de base
interne, réadmission en boucle ou fausse complétion n'a été effectuée.

La réussite d'un livrable et la transition de son graphe d'orchestration demandent
donc des preuves distinctes ; un résultat reçu n'est pas nécessairement un
consommateur libérable sans duplication. ^[inferred]

## Limites non absorbées par le PASS

F8 catch **INCONCLUSIVE**, close **DEFER/non couvert** ; C2 **OPEN/INCONCLUSIVE** ;
B1 historique non résolu ; GCC/libstdc++ natif et Linux/Release/image/performance
non validés. L'audit O4 ne prouve pas la branche libstdc++ native. Le diagnostic
Windows/Linux du harness est livré, mais les diffs concurrents de profils/oracles
ne sont pas corrigés ni validés par les tests C++. Aucune suite Node/Linux n'a été
exécutée pour ce bilan.

Le contrôle documentaire ciblé passe ; le `git diff --check` global signale un
espace final dans le diff concurrent AGENTS.md116, laissé intact. Aucun PASS global
moteur/harness ni RAII exhaustive. Les [[adr/README|statuts ADR]] sont conservés :
0001/2/4/6/7 Proposed (0002 avec tranche partielle),0003/5 Accepted/partial,
0008 vérifié Windows Debug seulement.
