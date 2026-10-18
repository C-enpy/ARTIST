---
title: "Mission F7 clôturée — Uniform/Attribute et findings de cardinalité/diagnostic"
category: architecture
tags: [artist, architecture, cpp, ownership, testing]
aliases: [Archive F7 uniform attribute]
summary: "F7 T1–T3 intégrées, TE et ré-audit T1-1, puis F2 uploads single-value et F1 diagnostics corrigés/revus. Candidat final 6d6dcb7 : 299 tests natifs Windows Debug (251 CPU, 48 OpenGL), zéro skip ; ASan245 historique T4, non rejoué sur le final. Contrat initial conservé ci-dessous, limites et reports explicites."
status: completed
implementation: verified
source_revision: 6d6dcb79ef7d249dd1061e09f20e5514268fad9f
created: 2026-10-17
updated: 2026-10-18
sources: [docs/architecture/correction-plan.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/adr/0004-layouts-types-et-donnees-preparees.md, docs/adr/0006-preuves-performance-et-determinisme.md, docs/architecture/current-state.md, docs/development/known-incidents.md, docs/architecture/missions/2026-10-16-pipeline-ownership.md]
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Mission F7 clôturée au périmètre, 2026-10-18

## Résultat vérifié

T1 `cb9f268` : Uniform et son contexte propriétaires uniques, getContext emprunté,
erreurs GET typées au lieu de deref nul, maps uniform possédées par le Pass.
T2 `05fef28` : Attribute/context/maps propriétaires uniques, composants empruntés,
clé de dispatch **Setter<T>**, sizeof(T) pour upload/stride et rejet NULL_VALUE.
T3 `18dcf31` : UniformReader exigé par concept, noms dimensionnés par le driver,
rejets nom/location/size/doublon intra-réflexion et taille réelle publiée.

TE `1dd7cd1` ajoute trois gaps CPU (move-assign, set<float>, publication partielle).
T1-1 final `1db306a` : compteurs distincts et guards avant lectures, ré-audit
indépendant KEEP ; précédent KEEP du compteur agrégé invalidé et conservé.
F2 `1690bac` : les34setters OpenGL rejettent les tableaux avant GL,30counts=1,
contexte initialisé. RED sûrs mocks avant patch, GPU sans altération après rejet,
deux revues fresh concordantes. F1 `6d6dcb7` : deux expressions de diagnostic
AttributeContext corrigées (type réellement stocké), préfixes publics et freeze
conservés ; RED2/2 avant patch, GREEN251CPU et revuefresh sans finding.

**Campagne finale réellement exécutée sur `6d6dcb7` : 299/299** (251CPU +48OpenGL),
failure0/disabled0/skipped0, CTest exit0 le2026-10-18. Logs/JUnit sous
`out/mission-f7/final/`, manifeste versionné `_meta/session-2026-10-18-f7-final.json`.
ASan : **245/245 à T4** (`17019e4`,223CPU+22GL sélectionnés), antérieur à TE/F2/F1 ;
**aucune nouvelle campagne ASan du candidat final**. Ne pas combiner ces campagnes.

Preuves et revues : `.opencode/evidence/mission-f7/closure-findings-2026-10-18.md`,
plans QA et notes d'intégration F1/F2 ; logs ignorés sous out/ copiés au principal
avant shutdown des auteurs. Reviewers en lecture seule, exécutions attribuées aux
auteurs/lead ; comparaison de preuves distincte d'une réexécution indépendante.

## Limites conservées et suites non sélectionnées

- Windows x64 Debug/MSVC14.44, OpenGL3.3 core/NVIDIA617.42. Pas de preuve
  image/perf/Release/CI distante/autreOS/RAII exhaustive/lifetimeGPU en vol.
- Six spécialisations F2 vérifiées nativement (float/vec3/mat3 et fp64 observé
  double/dvec3/dmat2) ;28autres INCONCLUSIVE driver, couvertes par inventaire/revue.
  Gate fp64 production sur appareil sans extension reste une question préexistante.
- Reload conserve les anciens noms absents ; publication Reader partielle après
  erreur, pas d'atomicité/purge nouvelle. Emprunts invalidés au remplacement/reload.
- VBO créé par Binder non libéré dans cette tranche ; aucun gain de performance.
- Fragilités de shuffle Loader/ShaderLoader historiques non corrigées.
- Incidents provider capacity_limit/auth qualifiés infrastructure, jamais RED.
  Politique native v3 approuvée : strong Sol xhigh primaire, standard GLM
  opencode-go max ; aucun fallback automatique. Substitution TE cpp-engineer
  signalée ; cause du silence des profils QA non démontrée par transcript natif,
  aucune permission QA modifiée. Audit/ré-audit test-reviewer hors board.

ADR-0003 reste **Accepted/partial**, ADR-0004/0006 **Proposed**. Pas d'ADR-0009.
B/C/D reportées ; aucune nouvelle mission sélectionnée implicitement.

## Contrat initial conservé (historique du 2026-10-17)

Les sections suivantes sont la fiche de préparation initiale, pas des instructions
de relance actuelles. Les observations désormais exécutées, changements de clé
Setter<T>, décisions D1–D9 et politique v3 sont réconciliés ci-dessus et dans les
preuves versionnées. Les scores/modèles initiaux décrivent l'intention à leur date.

### Sélection initiale : F7 Uniform/Attribute, option A

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
