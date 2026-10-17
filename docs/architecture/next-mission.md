---
title: "Mission sélectionnée — F7 Uniform/Attribute (option A) : préflights P1–P3, tranches T1–T3, barrière T4, revues T5/T6"
category: architecture
tags: [artist, architecture, cpp, ownership, testing]
aliases: [F7 uniform attribute, Mission F7, next-mission]
summary: "Option A enregistrée le 2026-10-17 par l'opérateur (PM/LLM) : préflights lecture seule 45/25, 45/25, 40/25 ; tranches de développement 70/62, 70/66, 70/55 ; barrière lead après T3 ; revues fresh GLM 58/45 et 55/40 ; concurrentiel max 2. Relance en cycle natif dans une nouvelle session depuis la racine ARTIST, grille light/standard/strong (GLM standard, Opus strong), scores manuels publiés avec chaque tâche → classification not_required, aucun appel JEV. Observations restent à convertir en RED causal ; B (ADR-0004), C et D reportées. Le brouillon natif abandonné team_mvc96rxl_0030_yrrzrg (champ policy.classification invalide, racine re-ff8) ne doit pas être relancé."
status: selected
implementation: not-started
source_revision: eb0dd14fcf1d7ee51f550186953211553d3cc4b1
created: 2026-10-17
updated: 2026-10-17
sources: [docs/architecture/correction-plan.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/adr/0004-layouts-types-et-donnees-preparees.md, docs/adr/0006-preuves-performance-et-determinisme.md, docs/architecture/current-state.md, docs/development/known-incidents.md, docs/architecture/missions/2026-10-16-pipeline-ownership.md]
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Mission sélectionnée (2026-10-17) : F7 Uniform/Attribute, option A

L'opérateur a retenu l'option A le 2026-10-17 après l'avis PM/LLM du matin
(aucune mission engagée ce jour-là, fiche de sélection conservée pour trace dans
[[architecture/missions/2026-10-16-pipeline-pointer-2026-10-17-pre-selection-backup-20261017-095916|la
sauvegarde du pointeur]]). Le
[[architecture/missions/2026-10-16-pipeline-ownership|Pipeline A1+A2]] est
archivé et vérifié ; l'amendement GMock Attribute (`b9c90fd`) est terminé au
périmètre avec revue fresh PASS et QA indépendante (176 CPU normaux + 176 CPU
ASan, contrôles négatifs réels). ADR-0003 reste **Accepted / partial** ;
ADR-0004, ADR-0006 et les autres ADR restent **Proposed**.
Option A : préflights lecture seule **P1–P3**, tranches de développement
**T1 → T2 → T3**, barrière **T4** du lead, revues indépendantes **T5/T6**.
B (ADR-0004), C (protocole ADR-0006) et D (réserves) sont **reportées** avec
leurs conditions inchangées (§Décisions ouvertes).

## Cycle natif à relancer (nouvelle session, racine ARTIST)

1. Ouvrir la session sur **`C:/Users/djden/source/repos/ARTIST`** (racine
   canonique ; un `model-policy.json` créé depuis une autre racine pointe vers
   cet autre dépôt, comme l'a montré l'abandon ci-dessous).
2. `/ensemble-mission` avec un objectif du type : « Mettre en œuvre l'option A
   de docs/architecture/next-mission.md : ownership Uniform/Attribute (F7) et
   UniformReader durci ; préflights P1–P3 lecture seule, tranches T1–T3, barrière
   T4, revues T5/T6 fresh ; PM/LLM, scores manuels, aucun appel JEV. »
3. `/ensemble-mission config` : appliquer la grille du §Politique de modèles
   (le brouillon racine accepte **exactement** `version`, `providers`,
   `assessment`, `complexityBands`, `riskBands`, `groups`, `matrix` — aucun
   champ `classification` au niveau politique), publier le plan du §Plan validé
   avec **les deux entiers pour chaque tâche**.
4. `/ensemble-mission start` : approbation de l'opérateur uniquement ; aucune
   exécution avant ce GO natif.

**Orphelin abandonné :** `team_mvc96rxl_0030_yrrzrg` (préparé le 2026-10-17
depuis une session ouverte sur `retro-eng\re-ff8`) — champ
`policy.classification` invalide et `policy_path` hors racine ARTIST. Ne pas le
configurer ni le démarrer ; une purge optionnelle via `team_cleanup` reste une
décision de l'opérateur.

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

## Plan validé (PM/LLM — publication après config, exécution après start)

| Tâche | Groupe → modèle | C/R | Contexte | Déps |
|---|---|---|---|---|
| **P1** préflight Uniform (lecture seule : observations vérifiées, consommateurs, plan RED) | standard → GLM max | 45/25 | fresh | — |
| **P2** préflight Attribute (Setter hardcodé, oracle GMock b9c90fd, plan RED) | standard → GLM max | 45/25 | fresh | — (parallèle P1/T1) |
| **P3** préflight UniformReader/GL (patterns tests composants, concept négatif) | standard → GLM max | 40/25 | fresh | — (parallèle T2) |
| **T1** Uniform implémentation | strong → Opus Max | 70/62 | fork P1 | P1 |
| **T2** Attribute implémentation | strong → Opus Max | 70/66 | fork P2 + diff T1 intégré | T1, P2 |
| **T3** UniformReader implémentation | strong → Opus Max | 70/55 | fork P3 | T2, P3 |
| **T4** barrière (merge, build un-job, ASan déclarée, diff final) | — lead | — | — | T3 |
| **T5** revue correctness fresh (lecture seule) | standard → GLM max | 58/45 | fresh — jamais fork | publication après T4 |
| **T6** QA rendering fresh (lecture seule) | standard → GLM max | 55/40 | fresh — jamais fork | publication après T4 |

Règles d'exécution :

- **Mainline sérialisée T1→T2→T3** : `ComponentConcept.hpp` et le pattern
  d'invalidation sont partagés, un seul écrivain ; campagnes GL/ASan
  sérialisées. Les préflights restent parallèles car lecture seule (papier,
  consommateurs, patterns testés) ; aucune mutation de fichier partagé.
- **Squelettes RED** : les préflights rédigent le plan RED (squelettes notés
  dans leur note de préflight) ; le code de test réel et le RED causal
  appartiennent à l'auteur de la tranche T1–T3.
- **Fork** = point de départ worktree du prédécesseur, diff intégré relu par le
  lead avant publication de la tâche suivante ; les contextes de session des
  teammates restent fresh. **T5/T6 ne forkent jamais** et partent du diff final
  de T4 ; leur publication attend l'intégration réelle (merge, contrôle du
  diff, HEAD/diff et empreintes).
- **Concurrentiel max 2** members actifs ; budget fixé par l'opérateur dans la
  session de configuration (rapporté à l'approbation si nécessaire).
- Les noms P/T ne sont pas des IDs Ensemble ; les fiches exécutables conservent
  objectif, fichiers/symboles, invariants, liberté locale, critères observables
  et preuves RED/GREEN attendues.

## Politique de modèles (config) — grille validée le 2026-10-17

Groupes approuvés (identités vérifiées disponibles dans le catalogue opérateur) :

- `light` → `opencode-go/gpt-6-luna` @ `none`
- `standard` → `zai-coding-plan/glm-5.3-flash` @ `max` (GLM)
- `strong` → `cursor/claude-opus-5-5` @ `«Claude Opus 5.5 Max»` (Opus)

`providers` : `cursor`, `openai`, `opencode`, `opencode-go`,
`zai-coding-plan` ; bandes complexité et risque `[0, 34, 67, 101]` ; matrice
3×3 `[[light, standard, strong], [standard, standard, strong],
[strong, strong, strong]]` (T1–T3 à C70 ; T5/T6 et préflights en standard).

- **Économie d'Opus** : Opus ne tourne que sur T1→T2→T3 (cœur ownership/UB/GL
  qui le justifie) ; exploration, préflights, revues et QA tournent sur GLM.
- **Classification** : PM/LLM. Le lead publie les deux entiers complexity/risk
  avec chaque tâche → `classification_status` = `not_required` immédiat.
  **Aucun appel JEV** pour cette mission (choix explicite de l'opérateur du
  2026-10-17) ; ne pas activer `classification.mode: jev` (le réglage mission,
  s'il est jamais posé, exprime un mode manuel). Aucun spawn avec `model`
  explicite ; la grille approuvée seule décide.
- **Option non appliquée (décision ouverte, mot opérateur requis)** : ajouter
  `openai/gpt-6.1-sol` @ `xhigh` comme second candidat de `strong` (identité
  vérifiée ; Sol = modèle standard/review historique d'ARTIST) pour réduire
  l'exposition d'Opus, voire re-bander la complexité en 4 tranches comme la
  mission `team_mvb24znz`. Non appliqué ce jour.

## Contrats attendus par tranche (liberté locale bornée)

- **T1 Uniform** : ownership unique wrapper, `getContext()` emprunt ; RED
  `TYPE_MISMATCH` et déréférencement null (forme pointeur de
  `std::any_cast`) ; static_asserts positifs et négatifs au motif de rejet
  vérifié.
- **T2 Attribute** : idem T1 pour le bloc Attribute + correction du
  `if constexpr` spécialisé sur `Setter<std::shared_ptr<int>>` →
  `Setter<std::shared_ptr<T>>` générique (remplace le hardcode) ; corrige au
  passage le `sizeof` mesurant un pointeur (observation 3) si le RED causal
  l'établit ; oracle GMock `b9c90fd` adapté, RED causal avant réécriture.
- **T3 UniformReader** : durcissement du Reader + concept négatif ; patterns
  issus du préflight P3 (tests composants, OpenGL réel) sans élargir le
  périmètre.
- **T4 barrière** (lead) : merge, build un-job, suites normales + ASan
  sélection **déclarée** avant run, diff final relu.
- **T5/T6** : revue fresh lecture seule (correctness-reviewer /
  rendering-verifier), critères observables, verdicts PASS/FAIL/INCONCLUSIVE
  par critère.

## Preuves attendues (toutes tranches)

- RED causal par tranche (échec pour la raison attendue, pas toolchain absente) ;
  GREEN minimal ; REFACTOR et non-régression sur le diff final.
- Compile-time : static_asserts positifs **et** négatifs, motif de rejet vérifié.
- CPU (unit) : null/moved-from/index ; GL (contexte GLFW/GLEW 3.3) : bind/set
  réels ; ASan sélection déclarée avant run, nouveaux cas lifetime.
- Interdits : gain revendiqué sans protocole exécuté, tolérance élargie, mock à
  la place d'une preuve GPU.

## Ressources exclusives

Un seul build par arbre (`scripts/build-windows.ps1`, un job) ; GPU sérialisé ;
**aucun alias `subst`** (tous retirés) ; sorties courtes + témoin JUnit 65536 ;
cache Conan/tooling existants réutilisés sous bail, jamais copiés.

## Décisions ouvertes (tranchées par l'opérateur)

1. Choix de l'option : **résolu** — A le 2026-10-17 ; B/C/D reportées. B reste
   l'étape 3 canonique et exige l'adoption explicite ADR-0004 via le cycle
   natif d'une mission (un GO en prose ne le remplace pas) ; C devient plus
   utile après B (workload/seuils à figer sinon) ; D reste hors séquence,
   branche par branche (voir la sauvegarde du pointeur liée en tête de page).
2. Second candidat `strong` (`gpt-6.1-sol @ xhigh`) ou rebanding 4 tranches :
   uniquement sur mot de l'opérateur, à poser en config.
3. Budget et concurrentiel exacts : fixés par l'opérateur dans la session de
   configuration ; rappelés à l'approbation si besoin.
4. Périmètre moteur de A : rouvrir `IAttribute::set` exige son RED causal
   (l'amendement précédent avait laissé le setter en lecture seule).
5. B : politique required/facultative des uniforms optimisés (plus tard).
6. C : workload/matériel et seuils d'acceptation (plus tard).

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
- Le brouillon natif du 2026-10-17 (`team_mvc96rxl_0030_yrrzrg`) est abandonné,
  non présenté, non approuvé : son fichier politique n'est ni corrigé ni
  réutilisé.

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
