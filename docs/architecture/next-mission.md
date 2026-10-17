---
title: "Prochaine mission — sélection en attente : F7 uniform/attribute (A) ou ADR-0004 (B)"
category: architecture
tags: [artist, architecture, cpp, ownership, testing, pipeline, uniforms, attributes]
summary: "Fiche de sélection PM/LLM du 2026-10-17 après Pipeline archivé et amendement GMock b9c90fd vérifié : quatre options comparées (A F7 recommandé, B ADR-0004 adoption explicite, C protocole ADR-0006, D réserves), observations à convertir en RED, graphes de tranches, décisions ouvertes. Aucune mission engagée avant choix utilisateur."
status: pending-selection
implementation: not-started
source_revision: b9c90fd51b0ff8a85b1db1d0674c1243ccdd0951
created: 2026-10-17
updated: 2026-10-17
sources: [docs/architecture/correction-plan.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/adr/0004-layouts-types-et-donnees-preparees.md, docs/adr/0006-preuves-performance-et-determinisme.md, docs/architecture/current-state.md, docs/development/known-incidents.md, docs/architecture/missions/2026-10-16-pipeline-ownership.md]
provenance:
  extracted: 0.95
  inferred: 0.05
  ambiguous: 0.0
---

# Prochaine mission : fiche de sélection PM/LLM (aucune mission engagée)

Le [[architecture/missions/2026-10-16-pipeline-ownership|Pipeline A1+A2]] est
archivé et vérifié ; l'amendement GMock Attribute (`b9c90fd`) est terminé au
périmètre, avec revue fresh PASS et QA indépendante (176 CPU normaux +
176 CPU ASan, contrôles négatifs réels). ADR-0003 reste **Accepted / partial** :
`Uniform`/`Attribute` (F7) restent partagés, hors coupe à ce jour.
ADR-0004 et ADR-0006 restent **Proposed / not-started**.

Cette fiche est un **plan de sélection** : elle ne lance ni code ni campagne ;
l'option et les préconditions restent des décisions utilisateur. Le contenu
précédent de cette page est conservé dans
[[architecture/missions/2026-10-16-pipeline-pointer-2026-10-17-pre-selection-backup-20261017-095916|la sauvegarde
du pointeur]] et dans le manifeste machine `out/mission-a1a2/pre-selection-backup/`.

## Observations de lecture — à convertir en RED, non confirmées

1. `ARTIST/include/graphic/context/UniformContext.hpp:24,31,39` —
   `std::any_cast<T>(&m_value)` en **forme pointeur** : ne lève jamais
   `bad_any_cast` ; le contrat `TYPE_MISMATCH` documenté n'est pas testé, et
   `*std::any_cast<T>(&m_value)` peut déréférencer `nullptr` (UB potentiel).
2. `ARTIST/include/graphic/pipeline/Attribute.hpp:88,90` — `if constexpr`
   testant la spécialisation **hardcodée** `Setter<std::shared_ptr<int>>`
   (motif de l'oracle GMock récemment corrigé côté test).
3. `ARTIST/include/graphic/opengl/pipeline/component/attribute/Setter.hpp:31-32` —
   `sizeof(attrValue.get())` mesure un **pointeur**, pas `sizeof(T)` ; suspect
   pour `glBufferData`.

Ces constats proviennent d'une lecture statique (index LSP incomplet sur
templates, greps recoupés) ; aucune exécution ne les prouve encore.

## Options comparées (PM/LLM, heuristiques — pas des probabilités)

| | **A — F7 uniform/attribute + durcir UniformReader** | **B — ADR-0004 layouts typés** | **C — Protocole ADR-0006 A/B** | **D — Réserves séparées** |
|---|---|---|---|---|
| Ordre du plan | reste étape 1/2, sillage explicite de l'avis A2 | **étape 3 formelle** | étape 4, mieux après B | hors séquence |
| Valeur moteur | achève la migration ownership ; ferme les trois observations ; réduit le churn de B | bindings résolus à la préparation, invalidation relink/reload | premières mesures réelles MEASURED vs HYPOTHESIS | fiabilise harnais/plateforme |
| Complexité brute → retenue | 62 → **70** | 78 → **78** | 66 → **70** | 35–60 → **70** |
| Risque | **68** | **74** | **62** | 38–58 |
| Prérequis | aucun (RED causal dédié pour rouvrir `IAttribute::set`) | **adoption explicite ADR-0004** | workload/seuils à figer | selon branche |

**Recommandation du PM : A d'abord.** Faire B avant A signifie migrer des
données préparées dans des maps `shared_ptr`/`std::any` qu'A changera ensuite
(double churn). B reste l'étape 3 canonique si l'utilisateur suit l'ordre
strict. C sans B n'aurait qu'une valeur ambiguë (sauf figage de workload
parallélisable). D ne bloque rien.

## Option A — tranches proposées

```
T1 Uniform (cpp-engineer, worktree): ownership unique wrapper, getContext() emprunt,
   RED TYPE_MISMATCH/deref-null, static_asserts +/−            [C70 R62]
T2 Attribute (cpp-engineer, dépend T1): idem + correction Setter<std::shared_ptr<T>>
   (remplace hardcode int), oracle GMock adapté RED causal     [C70 R66]
T3 UniformReader durci + concept négatif (gpu-engineer)        [C70 R55]
T4 BARRIÈRE intégration (orchestrateur): merge, build un-job,
   suites normales + ASan sélection déclarée, diff final       [—]
T5 revue correctness-reviewer fresh (lecture seule)            [C58 R45]
T6 QA rendering-verifier fresh (lecture seule)                 [C55 R40]
```

Sérialisation T1→T2→T3 : `ComponentConcept.hpp` et le pattern d'invalidation
sont partagés, un seul écrivain ; campagnes GL/ASan sérialisées.

## Option B — tranches (si adoption ADR-0004 obtenue)

```
B0 adoption explicite ADR-0004 (engine-architect, statut Proposed→Accepted) [C60 R55]
B1 bindings résolus à la préparation + adaptateur par nom conservé
   (cpp-engineer; PassContext/Uniform/Attribute/contexts+tests) [C72 R70]
B2 invalidation relink/reload + politique required/optional uniforms
   optimisés driver (gpu-engineer; Readers, Freer/Loader, GL tests) [C70 R66]
→ B3 barrière + revue (correctness + shader fresh) + QA fresh
```

Preuves : compat CPU/GPU layout, rejets type/handle périmé au bon moment,
invalidation CPU+GL, image à paramètres identiques, profil allocation/lookup
du chemin migré. Les types exacts de B « restent à préciser » (ADR-0004) : B1
doit figer les types avant B2.

## Option C — protocole ADR-0006

```
C1 workload/fixtures figés + instrumentation (build-engineer)   [C70 R55]
C2 campagne A/B old-path vs specialized-path + code size/build  [C70 R60]
C3 rapport ADR-0006 + docs                                      [C50 R40]
```

Release non validée : étendre ADR-0005 ou mesurer Debug-only avec limite
documentée ; dispersion/warm-up/répétitions imposées, seuils fixés **avant**
conclusion ; verdicts MEASURED/HYPOTHESIS/INCONCLUSIVE séparés.

## Option D — branches séparées (indépendantes entre elles)

| Branche | Contenu | C brute→retenue / Risque | Propriétaire | Limite |
|---|---|---|---|---|
| D1 | shuffle Loader/ShaderLoader : isolation lifecycle, singletons mocks, campagne multi-seeds | 45→70 / 48 | build-engineer | fixtures/harnais |
| D2 | 17 autres GL sous ASan, verdicts individuels | 55→70 / 52 | gpu-engineer + QA | GPU sérialisé |
| D3 | preset Release + workflow CI | 60→70 / 58 | build-engineer | CI jamais validée |
| D4 | F8 : `close()` échouée + `ifstream::failure` réel (Reader) | 58→70 / 55 | cpp-engineer | panne OS possiblement INCONCLUSIVE |
| D5 | latent libstdc++ `#if` const (TraceableException) | 35→70 / 38 | cpp-engineer | **bloqué** sans GCC `std::stacktrace` |

## Preuves attendues (toutes options)

- RED causal par tranche (échec pour la raison attendue, pas toolchain absente) ;
  GREEN minimal ; REFACTOR et non-régression sur le diff final.
- Compile-time : static_asserts positifs **et** négatifs, motif de rejet vérifié.
- CPU (unit) : null/moved-from/index ; GL (contexte GLFW/GLEW 3.3) : bind/set
  réels ; ASan sélection déclarée avant run, nouveaux cas lifetime.
- Interdits : gain revendiqué sans protocole exécuté, tolérance élargie,
  mock à la place d'une preuve GPU.

## Ressources exclusives

Un seul build par arbre (`scripts/build-windows.ps1`, un job) ; GPU sérialisé ;
**aucun alias `subst`** (tous retirés) ; sorties courtes + témoin JUnit 65536 ;
cache Conan/tooling existants réutilisés sous bail, jamais copiés.

## Décisions ouvertes (tranchées par l'utilisateur)

1. **Choix de l'option** A / B / C / D (A recommandée).
2. **Adoption explicite ADR-0004** — précondition de B, via le cycle natif
   d'une nouvelle mission ; un GO en prose ne le remplace pas.
3. Périmètre moteur de A : rouvrir `IAttribute::set` exige son RED causal
   (l'amendement précédent avait laissé le setter en lecture seule).
4. B : politique required/facultative des uniforms optimisés.
5. C : workload/matériel et seuils d'acceptation.
6. D5 : conditionner à une baseline GCC fournissant `std::stacktrace`,
   ou classer INCONCLUSIVE.

## Limites

- Consommateurs établis par grep exact (`Pass.hpp:151,159`,
  `context/PassContext.hpp:138,147`, tests unitaires et composants,
  `UniformReaderTests.cpp:51-52`, `AttributeReaderTests.cpp:75-76`) ;
  à revérifier à l'implémentation (LSP incomplet sur templates).
- Scores = heuristiques PM/LLM de routage, pas des probabilités de succès ;
  estimations indicatives, re-scoring si les fiches sont modifiées.
- Aucune mesure de performance n'existe encore dans le dépôt : rien n'est
  présumé sur le coût réel ; les observations §Observations restent
  INCONCLUSIVE jusqu'à leur RED exécuté.

## Continuité

- Archive Pipeline et chiffres vérifiés :
  [[architecture/missions/2026-10-16-pipeline-ownership|contrat, preuves et limites]].
- Registre des statuts : [[development/known-incidents|incidents]] ;
  ordre des travaux : [[architecture/correction-plan|plan de correction]].
- Commandes : [[development/build-and-test|build et tests]].

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] ·
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]]
