---
title: Continuation QA ARTIST — preuves acquises et board réconcilié
category: reference
tags: [artist, testing, tooling, reference]
summary: "Continuation bornée réconciliée ; scope initial repris : deux causesB1 vérifiées, contratB3 proposé et dispositionC2 encore à décider."
created: 2026-10-25
updated: 2026-10-25
source_revision: 82bf52d52e594fae02593866669a1af715127d1f
sources:
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/21-audit-final-accepte.md
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/22-bilan-final-et-dette-board.md
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/25-reconciliation-native-verifiee.md
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/30-b1-causes-reproduites-et-actuel-vert.md
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/31-jalon-b1-et-decisions-b3-c2.md
provenance:
  extracted: 0.95
  inferred: 0.05
  ambiguous: 0.0
---

# Continuation QA — 2026-10-25

## Deux états qui ne se confondent pas

Les **livrables de continuation sont terminés à leur portée**, avec audit final
indépendant reçu et accepté. Le graphe natif initialement bloqué a ensuite été
**réconcilié7/7completed sous verrou d'admission**, sur choix explicite opérateur.
L'équipe/worktrees restent conservés, non archivés ; politique temporairement
non approuvée, aucun start exécuté par le lead. Aucun FAIL technique n'a été effacé.

### Reprise ultérieure du scope initial

L'opérateur a ensuite réapprouvé la révision3 restaurant B1/B3/B5, avec long_run.
Le7/7précédent est le board de la **continuation bornée**, pas une résolution
complète du scope initial. Trois enquêtes nouvelles ont démarré ; B1 a isolé deux
contaminations ON_CALL historiques par réductions négatives et vérifié leurs
nominaux actuels verts sans modifier les sources. Le contrat B3 reste proposé,
et la disposition C2 exige encore décision explicite ; reprise initiale en cours.

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

## Dette du graphe natif — historique et résolution

La tâche d'audit dédiée était réellement completed après intégration. Cependant,
l'ancienne tâche ASAN restait in_progress/inactive, l'ancien AUDIT était blocked par
ASAN, puis CLOTURE était blocked par AUDIT. La tentative de compléter CLOTURE a
été refusée pour dépendances non résolues. Le nouvel audit intégré n'a pas
remplacé automatiquement l'ancien nœud.

Libérer ASAN avec auto-dispatch actif risque de relancer l'ancien audit sur une
fiche/routage obsolètes. La voie exposée ne permettait pas de remplacer/mettre en
attente ces tâches ni de retargeter leurs dépendances. Aucune modification de base
interne, réadmission en boucle ou fausse complétion n'a été effectuée.

Après enquête complémentaire et choix opérateur A, le lead a ouvert le config
natif **sans changer le fichier de politique**, vérifié le verrou, puis acté
ASAN→ancien AUDIT→CLOTURE dans l'ordre. Chaque résultat était déjà intégré.
Contrôle final :7/7completed, mêmes quatre membres idle, aucun nouvel audit.
Le verrou a empêché l'auto-admission des tâches déverrouillées. Le SHA256 de la
politique est resté identique ; long_run/manual/calls0 conservés. Preuve native :
rapport25 cité dans les sources. Le blocage de statut est résolu, pas les dettes
techniques ; aucune réparation runtime n'a été nécessaire.

La réussite d'un livrable et la transition de son graphe d'orchestration demandent
donc des preuves distinctes ; un résultat reçu n'est pas nécessairement un
consommateur libérable sans duplication. ^[inferred]

## Limites à la sortie de continuation, puis réexamen

F8 catch **INCONCLUSIVE**, close **DEFER/non couvert** ; C2 **OPEN/INCONCLUSIVE** ;
B1 historique non résolu ; GCC/libstdc++ natif et Linux/Release/image/performance
non validés. L'audit O4 ne prouve pas la branche libstdc++ native. Le diagnostic
Windows/Linux du harness est livré, mais les diffs concurrents de profils/oracles
ne sont pas corrigés ni validés par les tests C++. Aucune suite Node/Linux n'a été
exécutée pour ce bilan.

Ce constat de sortie est daté : la reprise a depuis produit les preuves causales
B1 du rapport30, conservées dans `_meta/session-2026-10-25-b1-causal-validation.json`.
Elles ne clôturent pas F8/C2 ni toutes permutations ; leur revue finale reste à faire.

Le contrôle documentaire ciblé passe ; le `git diff --check` global signale un
espace final dans le diff concurrent AGENTS.md116, laissé intact. Aucun PASS global
moteur/harness ni RAII exhaustive. Les [[adr/README|statuts ADR]] sont conservés :
0001/2/4/6/7 Proposed (0002 avec tranche partielle),0003/5 Accepted/partial,
0008 vérifié Windows Debug seulement.
