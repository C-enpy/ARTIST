---
title: "Motif d'exécution fermée par emprunt (N fixé)"
category: architecture
tags: [artist, architecture, rendering, ownership]
summary: >-
  Motif validé le 2026-10-20 : type d'exécution fermé pour une topologie fixe,
  membres références en ordre d'activation, séquence littérale, sans
  curseur/cache/ownership ; preuve en deux familles (CPU ordre + GL readback).
status: observed
implementation: done-perimeter
created: 2026-10-20
updated: 2026-10-20
sources:
  - "ARTIST/include/graphic/pipeline/ClosedExecution.hpp"
  - ".opencode/evidence/mission-pipeline-execution-path/ledger-K.md"
  - "docs/adr/0002-preparation-et-execution-specialisee.md"
  - "docs/references/session-2026-10-20-pipeline-execution-path.md"
provenance:
  extracted: 0.6
  inferred: 0.4
  ambiguous: 0.0
---

# Motif d'exécution fermée par emprunt (N fixé)

Le motif livré par la [[architecture/missions/2026-10-19-pipeline-execution-path|mission
Pipeline]] avec [[architecture/current-state|l'état réel]] — description du
**motif** et de ses exclusions motivées ; ce n'est ni une promotion
d'[[adr/0002-preparation-et-execution-specialisee|ADR-0002]] (reste Proposed)
ni une règle générale pour toute topologie.

## Forme

- Type fermé pour une topologie **fixe** : N membres **références** stockés
  dans l'ordre d'activation + **une** méthode publique littérale
  (`first.use(); second.use();`). La longueur et la séquence sont
  **structurelles** — le seul contenu statique ajouté. ^[extracted]
- Ne possède **rien** : ni passes, ni contexte, ni programme, ni curseur.
  Les owners (pipeline) doivent survivre à l'objet. ^[extracted]
- Échec à l'étape k ⇒ étape k+1 jamais lancée ; exception propagée
  **identique** (aucun try/catch/wrap) ; les activations réussies restent en
  GL (aucun rollback GPU — demi-obligation GAP-accept endossée, prouvée par
  revue statique + readback nominal). ^[extracted]

## Exclusions motivées — chaque interdiction touche un invariant existant

- **Aucun accès curseur** : la machine curseur `c ∈ {-1} ∪ [0, N)` est la
  propriété exclusive de `IPipeline`/`PipelineContext` ; un type qui lit ou
  écrit casse la comptabilité des consommateurs.
- **Aucun cache de handle** : chaque activation repasse par `pass.use()` — un
  handle copié court-circuiterait les gardes `RELOAD_WITHOUT_FREE` et
  désynchroniserait le rebind.
- **Pas de paramètre `PROFILE`** : la sélection de composant appartient à
  chaque passe (le virtuel `IPass::use` demeure — coût constaté, jamais
  revendiqué comme gain) ; le type ne redescend pas la sélection au niveau
  de la chaîne.
- **Pas de détection de références identiques** : devoirs de l'appelant —
  décision délibérée, tout test l'assertant contredit le contrat. ^[inferred]

## Coexistence

- Le type **ne remplace pas** `Pipeline<API, PROFILE>` et n'est pas son hook :
  brancher la chaîne sur `use(index)` déclencherait deux passes par appel.
  [[adr/0002-preparation-et-execution-specialisee|ADR-0002]] tolère
  explicitement une période de coexistence et un adaptateur froid
  (verbatim vérifié) ; sa cible terminale (tuple, passes concrètes) reste une
  adoption séparée. ^[extracted]

## Preuve en deux familles — pourquoi les deux sont obligatoires

- Le readback `GL_CURRENT_PROGRAM` **ne prouve pas l'ordre intra-chaîne** :
  après une chaîne complète, la valeur finale est celle de la **dernière**
  activation, identique quel que soit l'ordre réellement suivi. ^[inferred]
- L'**ordre** exige donc des oracles CPU de séquence (mocks `InSequence`
  encodés par invocation) ; la **liaison réelle** exige le GL réel (readback,
  `glIsProgram`, discipline lifetime du harnais). Les deux familles sont
  complémentaires, pas redondantes — la répartition est fixée par le
  [[development/testing-oracle-constraints|contrat d'oracles]]. ^[extracted]

## Frontière d'échelle

- N ≠ 2, ordre par frame, variété par lot : **nouvelle mission**. Le type
  fermé n'est pas un encodeur de scène et ne devient pas un paramètre de
  variante par topologie ; l'alternative tuple N-paramétré multiplie les
  variantes (coût mis en garde par ADR-0002). ^[inferred]

Voir aussi : [[architecture/next-mission|pointeur entre missions]],
[[architecture/correction-plan|ordre de correction]] (l'étape suivante est
candidate, non sélectionnée).
