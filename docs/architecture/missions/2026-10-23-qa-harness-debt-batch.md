---
title: "Ordre de mission — dette QA/harnais consolidée (B + D)"
category: architecture
tags: [artist, testing, cpp, tooling]
aliases: [qa-harness-debt-batch, mission-dette]
summary: "Proposition sélectionnée le 2026-10-23 : fermer six findings conservés avant le churn de l'étape 3 (ADR-0004) — shuffle, ASan final+17 GL, F8 réel, unbind/consolidation, C2, getStackTraceString ; ADR-0002/0003 rafraîchis. Non démarrée."
status: proposed
implementation: not-started
source_revision: b368a359cbc996c66613f03884948b81447caa39
created: 2026-10-23
updated: 2026-10-23
sources:
  - "docs/development/known-incidents.md"
  - "docs/architecture/correction-plan.md"
  - "docs/adr/0003-ownership-et-lifetime-cpu-gpu.md"
  - "docs/adr/0005-build-reproductible-et-validation.md"
  - "docs/architecture/missions/2026-10-19-pipeline-execution-path.md"
provenance:
  extracted: 0.75
  inferred: 0.25
  ambiguous: 0.0
---

# Ordre de mission — dette QA/harnais consolidée

> [!important] Proposition, pas un démarrage
> Écrite le 2026-10-23 contre HEAD `b368a35`. L'opérateur a arbitré : **B puis A**
> — cette mission (B+D) précède l'étape 3 (ADR-0004), qui remplira la session
> d'après. Démarrage ultérieur via `/ensemble-mission`, `config`, `start` de
> l'utilisateur. Cette fiche ne lance aucune commande.

## Logique

La logique de la mission C (2026-10-15) reste valable : **fermer les findings
avant le churn des contrats suivants**. Six findings conservés dans
[[development/known-incidents]] sont bornés, pour la plupart parallélisables,
et aucune tranche moteur n'est requise pour les instruire. Le rafraîchi
documentaire (D) du 2026-10-20 est plié ici : la tranche d'exécution fermée
n'est toujours pas reflétée dans les ADR.

## Les six branches

| Ref | Contenu | Brute | Retenue | Risque | Nature |
| --- | --- | --- | --- | --- | --- |
| B1 | Fragilité shuffle Loader/ShaderLoader — enquête lifecycle/isolation des mocks (2 seeds démontrées, seed20261017/seed7), cause établie ou harnais corrigé ; pas de défaut moteur prouvé par l'expérience G1 actuelle | 70 | 70 | 55 | Diagnostic puis correctif borné si RED |
| B2 | ASan final sur le candidat courant + les 17 GL non sélectionnés (sélection déclarée avant run) | 65 | 70 | 50 | Campagne exécutable |
| B3 | F8 branches réelles : `ifstream::failure` authentique + fermeture échouée — RED causal, correction bornée si défaut réel | 70 | 70 | 50 | TDD |
| B4 | Unbind pattern des 3 tests préexistants (`UsePassTest`/`UseNextPassTest`/`ResetPipelineTest`, garde RAII existant réutilisé) + consolidation des helpers d'erreur **sans affaiblissement** (exiger l'égalité stricte partout) | 70 | 70 | 45 | Tests uniquement |
| B5 | Anomalie C2 (baseline du 20:35, OPEN) — reproduction ou fermeture motivée avec preuve | 65 | 70 | 45 | Diagnostic |
| B6 | `getStackTraceString` : coupe de la branche `#if` libstdc++ (appel const vers membre non const) avant toute exposition GCC — preuve MSVC inchangée + argument statique ; validation GCC réelle absente = limite consignée | 70 | 70 | 50 | Coupe compile-time |

Le plancher `max(70, estimation)` s'applique aux branches exécutables.

**D — fait en préparation le 2026-10-23** (hors graphe de la mission) :
ADR-0002 → `implementation: partial` avec note datée + addendum « Preuves
actuelles » (tranche close `fdbce15`, cible terminale non implémentée, ligne
de contexte explicite — `vector<shared_ptr>` décrit l'état de décision) ;
registre (README) nuancé ; ADR-0003 → addendum tranche d'exécution. Les
sections historiques des ADR ne sont pas réécrites.

## Ressources et conflits

- **ASan séquentiel** (bail exclusif, un build ASan par arbre) ; les tests GL
  partagent le verrou `artist_opengl` — B2 sérialise les GPU ; les autres
  branches n'exécutent que des suites CPU. Builds sérialisés sur le même
  arbre : un job par défaut, pas de builds concurrents.
- Un seul écrivain par fichier : B3 (loaders), B4 (ProgramTests.cpp/ViewTests),
  B1 (fixtures mocks) touchent des fichiers disjoints — confirmé au K.
- Transport : le correctif `ac5e0f5` (allow explicit correctness-reviewer) est
  un changement de permissions ; la panne de capture `final_response` n'est
  **pas** prouvée résolue. Première admission de chaque nouveau profil = sonde
  à résultat mesuré, avec repli sous-agent direct signalé (doctrine
  établie).

## Preuve de réussite

- Chaque branch : RED causal avant correctif (les branches 1/3/4 quand un
  défaut est trouvé), sinon constat reproduit-ou-fermé motivé ; jamais de RED
  fabriqué ni de tolérance ajoutée.
- Le candidat intégré reste vert au périmètre courant (unit + opengl, 0 FAIL,
  0 skip requis) ; les campagnes ASan exécutées rapportent leurs bornes.
- Registre [[development/known-incidents|incidents]] mis à jour : chaque
  finding reclassé (traité/fermé-motivé/DEFER avec propriétaire).
- ADR-0002/0003/README rafraîchis et recoupés (D) ; reindex QMD PASS.

## K — décisions avant tout auteur

1. **Périmètre par branche** : confirmer la liste ci-dessus, ou couper/ajouter
   (aucune branche implicite).
2. **Autorisation de l'enquête shuffle** (B1) : la lifecycle/isolation des
   autres mocks est explicitement « à autoriser séparément » dans les
   incidents — c'est cette autorisation.
3. **B5** : fermer C2 exige soit la reproduction, soit une motivation
   documentée acceptée par l'opérateur (pas une clôture silencieuse).
4. **Hors périmètre** : Release/plateformes (c'est le gate K de la mission C
   d'après), ADR-0004 (gate K de la mission A d'après), tout changement de
   contrat moteur au-delà des correctifs RED, promotion d'ADR, image/perf.

## QA de la proposition

Branche B1 et B3 touchent un comportement sous contrat si un défaut est
trouvé ; B2 étend la suite d'intégration (ASan) ; risque PM par branche ≥ 50
pour 1–3 → **boucle QA déclenchée par branche** : plan QA avant les auteurs,
persisté/commité avant leurs worktrees. Impact × probabilité par branch :
B1 3×3=9 (haut), B3 2×3=6, B2 2×3=6, B4 2×2=4 — le plan QA traite les
branches à produit ≥ 6 avec déclencheur/blast radius/détection/mitigation/gap
explicites.

## Graphe

```text
K (décisions humaines) → Q (qa-planner, plan par branche) → commit du plan
        ↓
B1 ∥ B2 ∥ B3 ∥ B4 ∥ B5 ∥ B6 (parallèles, conflits de fichiers et bail ASan sérialisés)
        ↓
I (lead : intégration réelle, diff, suites) → R ∥ G → F? → V fresh hors board
        ↓
clôture documentaire + reindex
```

Budget indicatif : préparation 1–2 h, réalisation 10–18 h-personnes, réserve
4–6. La réserve ne sert pas à ouvrir des branches hors K.

## Sortie et suite

L'étape 3 (ADR-0004) remplira la session d'après — son adoption explicite est
le gate K de cette future mission. Le protocole de mesure (C) suit, avec la
question Release tranchée dans son K. Démarrage de cette mission :
`/ensemble-mission`, `config`, `start` de l'utilisateur.

```text
K → Q → commit du plan → B1 ∥ B2 ∥ B3 ∥ B4 ∥ B5 ∥ B6 → I → R ∥ G → F? → V → clôture
```

(Ancien graphe avec branche D retiré : D est fait en préparation, voir plus haut.)
