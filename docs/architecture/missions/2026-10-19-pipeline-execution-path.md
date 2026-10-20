---
title: "Ordre de mission — chemin d'exécution Pipeline"
category: architecture
tags: [artist, architecture, rendering, cpp]
aliases: [pipeline-execution-path, mission-execution]
summary: "EXÉCUTÉE et close le 2026-10-20 au périmètre Windows Debug : type fermé ClosedExecution N=2 en coexistence avec IPipeline, 546/546 natifs (fdbce15). ADR-0002 reste Proposed."
status: closed
implementation: done-perimeter
source_revision: 8f4c81599b089cca6b38b553873783e609986627
created: 2026-10-19
updated: 2026-10-20
sources:
  - "architecture/correction-plan.md"
  - "adr/0002-preparation-et-execution-specialisee.md"
  - "architecture/current-state.md"
  - "ARTIST/include/graphic/pipeline/Pipeline.hpp"
  - "ARTIST/include/graphic/opengl/pipeline/component/pipeline/User.hpp"
provenance:
  extracted: 0.6
  inferred: 0.4
  ambiguous: 0.0
---

# Ordre de mission — chemin d'exécution Pipeline

> [!important] EXÉCUTÉE et close le 2026-10-20
> Cette fiche a été exécutée au périmètre Windows x64 Debug et close : type
> fermé `ClosedExecution<API>` (52 lignes) + 15 tests (10 unitaires, 5 OpenGL),
> **546/546 natifs** à `fdbce15` (477 unit + 69 OpenGL), RED→GREEN tracé
> (`36c6a13` → `ac692db`), revue R PASS (aucun bloquant), audit V « suite
> discriminante ». Décisions humaines du 2026-10-20 : topologie et forme
> approuvées, obligation (d) endossée, placement durci, O14 acceptée,
> GAP-accept I-A endossé, P1/P2/F1 acceptés. Preuves :
> [[references/session-2026-10-20-pipeline-execution-path|session de mission]],
> évidence sous `.opencode/evidence/mission-pipeline-execution-path/`.
> ADR-0002, ADR-0004 et ADR-0006 restent **Proposed** ; ADR-0003 reste
> **Accepted/partial**.
>
> Pointeur vivant : [[architecture/next-mission]] (candidat étape 3,
> non sélectionné). Pointeur lifetime archivé :
> [[architecture/missions/2026-10-19-next-mission-lifetime-closed]].

## Résultat attendu

Rendre exécutable, à côté de `IPipeline`, **une chaîne fermée déjà observée** :
les deux passes chargées du test composant `PipelineTest`, profil OpenGL
Classic. Le chemin concret appelle les passes dans le même ordre que
`use` / `useNext`, produit le même programme GL courant, et laisse inchangés
le curseur, le rollback CPU et la règle `reset` ≠ unbind.

La largeur s'arrête à cette topologie. Une scène, un lot ou un nombre de
passes différent attend une mission ultérieure.

## Contrainte structurante

[[adr/0002-preparation-et-execution-specialisee|ADR-0002]] décrit la cible et
reste Proposed. [[architecture/correction-plan|L'étape 2 du plan]] est la
prochaine tranche réversible après la clôture lifetime. [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]]
se migre ensuite sur cette tranche, avec une adoption explicite qui n'est
pas donnée ici.

Le code actuel sélectionne déjà `User<PROFILE>` et `Resetter<PROFILE>` par
`if constexpr`. `OpenGLPipelineUser<Classic>::on` emprunte le contexte et
appelle `getPass(current).use()`. Les passes restent un
`vector<unique_ptr<IPass>>`. Le curseur `c ∈ {-1} ∪ [0, N)` , le rollback
CPU de `IPipeline::use` et l'absence de `glUseProgram(0)` dans `reset` sont
déjà spécifiés dans [[architecture/current-state|l'état réel]] et couverts
par `PipelineCursorTests` ainsi que `PipelineTest.UsePassTest`,
`UseNextPassTest` et `ResetPipelineTest`. Cette mission ne rouvre pas ces
contrats.

## Preuve de réussite

Sur le candidat intégré, Windows x64 Debug :

- le chemin concret, alimenté par les deux passes déjà possédées par le
  pipeline de `PipelineTest`, lie `program0` puis `program1` dans le même
  ordre que `UsePassTest` / `UseNextPassTest` ;
- `IPipeline::reset` remet le curseur à −1 et laisse le programme GL courant
  lié, comme `ResetPipelineTest` (F10 conservé) ;
- un index illégal, `use(-1)` et un contexte libéré échouent comme aujourd'hui,
  sans mutation du curseur en cas d'échec du hook ;
- les suites `unit` et `opengl` du candidat restent vertes au même périmètre
  que la baseline qui sera mesurée au début de K ;
- aucun critère requis n'est FAIL ou INCONCLUSIVE.

Un constat « le chemin actuel suffit, aucun type nouveau » est une sortie
valide si K le démontre. Ce n'est pas un échec à combler par un RED fabriqué.

## Hors périmètre

Nouveaux backends, render graph, cook/assets, handles générationnels,
suppression globale des virtuels, tuple par scène, remplacement de
`IPipeline` ou de `vector<unique_ptr<IPass>>`, layouts et maps de
[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]], protocole de
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]], image de référence,
ASan, Release, autre OS, lifetime VBO, loaders, Uniform, Attribute, mocks
communs. Pas de promotion d'ADR.

## K — décisions avant tout auteur

Principal : `engine-architect`, lecture seule. Le spécialiste OpenGL reste
`gpu-engineer` seulement si K touche la séquence d'appels de
`OpenGLPipelineUser` ou `OpenGLPipelineResetter`.

1. **Topologie.** Retenir la chaîne à deux passes de `PipelineTest`
   (`ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp`), ou expliquer
   pourquoi une fixture CPU plus petite porte les mêmes obligations. Pas
   d'autre nombre de passes dans cette mission.
2. **Coexistence.** Forme proposée, à accepter ou à remplacer : un type
   d'exécution fermé qui emprunte deux `IPass&` déjà possédées, appelle
   `use()` dans l'ordre des index, et ne possède ni passes, ni contexte, ni
   programme. Nom et fichier exacts restent à l'auteur, dans le style voisin.
   Le type ne remplace pas `Pipeline<API, PROFILE>`.
3. **Effets GL.** Le chemin concret réutilise `IPass::use()`. Il n'ajoute pas
   de `glUseProgram`, de reset ou de rollback GPU. `reset` du pipeline
   existant conserve F10.
4. **Erreurs.** Réutiliser les diagnostics déjà émis (`PASS_INDEX_OUT_OF_RANGE`,
   `FREED_CONTEXT`, rejet de `use(-1)`). Aucun message nouveau sans oracle.
5. **Arrêt.** Si la seule forme correcte est une refonte du dispatch, un
   render graph ou l'adoption d'ADR-0002/0004, K s'arrête et rend la main.
   La mission ne continue pas sous un autre nom.

Le lead persiste le choix et attend la décision humaine sur 1 et 2 avant
le plan QA et les auteurs.

### Binding times

`API`, `PROFILE` et le composant `User`/`Resetter` Classic : compile-time,
déjà en place. Index, curseur, programme lié et emprunts de passes : runtime.
La topologie à deux passes est connue du fixture de test, pas de chaque
scène du moteur : le type fermé ne devient pas un encodage de scène.

## Graphe préparatoire

Évaluation **PM/LLM locale du lead**, pas un passage JEV. Complexité et risque
sont distincts, sur 100, et ne sont pas des probabilités de succès. Pour tout
développement, complexité retenue = `max(70, estimation)`. Ces scores ne
publient aucune tâche et n'imposent aucun modèle.

| Ref | Rôle | Brute | Retenue | Risque | Notes |
| --- | --- | --- | --- | --- | --- |
| K | engine-architect, RO | 74 | 74 | 72 | Ledger, forme de coexistence, arrêt éventuel. Aucune source. |
| Q | qa-planner, RO, après K accepté | 58 | 58 | 64 | Plan avant auteur. |
| S | cpp-engineer, conditionnel | 46 | 70 | 52 | Seulement si K trouve une obligation du chemin actuel sans oracle. Sinon non publiée. |
| X | cpp-engineer | 78 | 78 | 74 | Type fermé, tests du contrat, GREEN minimal. |
| I | lead | 60 | 70 | 66 | Intégration réelle de S (s'il existe) et de X avant la suite. |
| R | correctness-reviewer, fresh RO | 62 | 62 | 70 | En parallèle de G, après I. |
| G | test-engineer, RO | 52 | 52 | 48 | Gaps en propositions. Pas le RED de X. |
| F | owner du gap accepté | 60 | 70 | 58 | Publiée seulement avec chemins et scores réévalués. |
| V | test-reviewer, fresh hors board | 66 | 66 | 68 | Candidat intégré, contrat, preuves. |

```text
K → décision humaine → Q → commit du plan QA
                              ↓
                    S seulement si un oracle manque, sinon rien
                              ↓
                         X (après S intégré, s'il a existé)
                              ↓
                    I → G || R → corrections bornées
                              ↓
                         V fresh → documentation
```

Un seul écrivain pour le nouveau type et ses tests. `Pipeline.hpp`,
`User.hpp` et `Resetter.hpp` ne changent que si K le fige ; dans ce cas un
seul owner et une barrière, et X ne démarre pas en parallèle de cette
édition. Builds sérialisés sur le même arbre. Un bail GPU pour les tests
OpenGL.

QA de la proposition, produit impact × probabilité : comportement d'ordre de
passes, d'erreur et de non-unbind, impact 4, probabilité 3, produit 12.

- Déclencheur : contrat d'exécution et risque K 72.
- Rayon : `use` / `useNext` / `reset` et le programme GL courant de la chaîne
  à deux passes.
- Détection : oracles CPU du curseur déjà présents, et readback
  `GL_CURRENT_PROGRAM` sur le même modèle que `PipelineTest`.
- Mitigation : coexistence, arrêt de K si la forme devient une refonte.
- Trou : le type concret n'existe pas encore, donc aucun test ne le couvre.

## Sortie et suite

Baseline au début de K : `scripts/build-windows.ps1 -Configuration Debug -TestLabel unit|opengl -Jobs 1`,
avec `--test-dir` sur le build réel. Les 466+64 de `71dacbb` restent
historiques tant que ce run n'a pas été refait sur le candidat du moment.

RED causal de X avant le type, GREEN minimal, puis non-régression des suites
touchées. Environnement absent ou zéro test découvert : INCONCLUSIVE, jamais
RED ni PASS. Pas de revendication de coût, de dévirtualisation ou d'image.

Budget indicatif, pas une durée promise : préparation 2–4 h-personnes,
réalisation 12–20, réserve 4–8. La réserve ne sert pas à élargir la topologie.

Quand l'exécution sera voulue : `/ensemble-mission` avec cet objectif, puis
`/ensemble-mission config` et le `start` demandé par l'utilisateur. Cette
fiche ne lance aucune de ces commandes.
