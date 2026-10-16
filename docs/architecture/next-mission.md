---
title: "Prochaine mission — bornage des contrats d'échec (loaders et accesseurs)"
category: architecture
tags: [artist, cpp, testing, ownership]
summary: Contrats d'échec figés (longueur de log aberrante, publication de l'attacher, bornes des accesseurs, lecture fichier et glUseProgram(0)) avant la tranche Pipeline.
status: completed
implementation: verified
source_revision: e736bba
created: 2026-10-15
updated: 2026-10-16
sources: [docs/development/known-incidents.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/_meta/next-mission-2026-10-15-c-selection.json, docs/architecture/missions/2026-10-15-f5-gl-diagnostics.md, docs/architecture/missions/2026-10-15-failure-contracts.md, docs/_meta/session-2026-10-15-failure-contracts-validation.json]
provenance:
  extracted: 0.95
  inferred: 0.05
  ambiguous: 0.0
---

# Prochaine mission : les contrats d'échec doivent être figés, pas assumés

**Statut : terminée dans son périmètre Windows x64 Debug à `e736bba`, le 2026-10-15.**
Sélectionnée à la demande de l'utilisateur le même jour. L'archive de mission est
[[architecture/missions/2026-10-15-failure-contracts|2026-10-15-failure-contracts]]
(résultats recomptés, statuts par finding, limites) ; le manifeste est
`_meta/session-2026-10-15-failure-contracts-validation.json`. Le texte ci-dessous est
le contrat initial, conservé sans réécriture. Statuts réels : Q1 et Q3 traités, F4
traité pour l'accesseur, F8 traité partiellement, F10 conservé, concept
`OpenGLPassFlow` en appelabilité seulement, `getStackTraceString` ouvert. La mission F5 est archivée dans
[[architecture/missions/2026-10-15-f5-gl-diagnostics|2026-10-15-f5-gl-diagnostics]].

## Pourquoi cette mission avant la tranche Pipeline

Les reviews indépendantes de F5 ont déposé un défaut réel du chemin d'échec
(**Q1**) et un contrat latent (**Q3**). L'architecte a répondu au DEFER Q3 et a
ordonné la séquence : figer les contrats d'échec **avant** le churn structurel
de la tranche Pipeline (qui va multiplier les fixtures exerçant ces chemins),
puis seulement [[adr/0004-layouts-types-et-donnees-preparees|la tranche ADR-0004]]
avec adoption explicite. C'est la même logique que la sélection F5 avant
Pipeline : petits chefs de contrats opposables, coupes causales, preuve à la
frontière.

## Contenu de la mission

### C1 — Loaders et attacher (fichiers : `component/{shader,pass}/Loader.hpp`,
`component/pass/ShaderAttacher.hpp`, leurs tests)

- **Q1, longueur aberrante** : `GL_INFO_LOG_LENGTH` aberrant (ex. `INT_MAX`)
  fait lever `std::vector` depuis `checkCompileErrors`/`checkLinkErrors` —
  erreur non métier et nettoyage manquant. Vérifié contre le code : chemin
  shader, le handle créé n'est **ni stocké ni supprimé** ; chemin pass, le
  handle est **déjà publié** dans le contexte et le contrat
  `glDeleteProgram + setPassID(0)` du link-fail testé n'est pas exécuté.
  Contrat attendu : capacité bornée (clamp), exception métier
  `COMPILATION_FAILED` / `LINK_FAILED` au public invariant, `glDelete*` une fois
  en séquence, pas de handle source d'ambiguïté conservé.
- **Q3, ruling publication différée** : variante préférée = un seul `setPassID`
  après succès (`glCreateProgram` → attacher en forme **empruntée** — handle
  passé en valeur, pas d'extension de lifetime → `glLinkProgram` → check →
  publication unique) ; tout échec pré-publication = `glDeleteProgram` + throw,
  contexte tel quel. Variante de repli admissible : non-rollback **documenté et
  testé** (handle conservé + `free()` idempotent + rejet strict B intact). Les
  deux variantes sont fermées par des tests RED ; la preuve décide, la revue
  confie. Delta API contenu : concept `OpenGLPassFlow` + `MockShaderAttacher` +
  loaders + leurs tests. La sonde encodeant l'ordre actuel
  (`LoaderTests.cpp`, `passID==7` pendant le rappel info-log) change de sens :
  point de revue explicite, pas une retouche silencieuse.

### C2 — Bornes et contrats de lecture (hors loaders, fichiers disjoints)

- **F4** : borne/pass index (`getShader(int)` de `PassContext`, `getPass` de
  `PipelineContext`) — rejet hors bornes au motif documenté (erreur métier), pas
  d'indexation non bornée.
- **F8** : fichier absent / `badbit` de lecture source — erreur de lecture
  distincte de `ERROR::SHADER::COMPILATION_FAILED` (nature et message), pas d'un
  masquage du défaut GL.
- **F10** : contrat de `use()` sur un `passID == 0` (pilote `glUseProgram(0)`
  valide en OpenGL mais désarme le programme) — comportement documenté et testé,
  décidé explicitement, pas de changement silencieux sans consommateurs migrés.

## Base, invariants et périmètre

- **B0 résolu** : le diff F5 est commité avec provenance dans `b80c353`
  (tags `f5-red` / `f5-green` sur les commits auteur ; empreintes des 4 fichiers
  conservées `20139b4a…`, `9e26477a…`, `a91aa012…`, `daca4475…`). Un worktree
  frais transporte donc la base intégrale ; plus de transport de diff non stagé.
- Baseline : 143/143 normaux (112 unit + 31 GL) et ASan séparé 121 distincts à
  préserver, sans les reformuler ; baselines historiques `4d29842` (122/122) et
  loaded-owner (`137/137`, ASan 115) intactes.
- Invariants [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] : rejet strict B
  avant tout `glCreate*`, contexte inchangé au rejet, jamais de remplacement
  implicite ni de rollback transactionnel général ; publication unique — jamais
  de handle d'objet détruit publié, toute ressource abandonnée libérée à son
  site de création ; composants en emprunt dans le chemin migré ; mock GL
  `glFunctionMock.hpp` inchangé (stubs explicites dans les tests) ; pas de
  changement d'ownership, `noexcept`, profil, dépendance ; erreurs publiques
  préservées.
- QMD MCP pouvait être indisponible pendant la consultation : repli documentaire
  sur le vault versionné `docs/` ; la précondition d'exécution reste un index
  frais via `node .opencode/scripts/update-wiki.mjs` puis `qmd embed -c artist-wiki`.

## Critères observables

1. **RED causal avant chaque tranche** (C1 puis C2), exécuté isolément — mécanisme
   exact documenté ; une panne d'environnement/timeout non expliqué n'est pas un RED.
2. GREEN minimal par tranche, sans affaiblir assertions/mocks ; REFACTOR puis
   suites normales CPU/GL complètes sérialisées avec JUnit neufs et nombres réels
   (143 + ajoutés).
3. ASan séparé au modèle F5 : options strictes par processus, suite CPU complète
   + GL lifetime ciblés ; pas d'extension aux 22 autres GL.
4. Changes de sens de la sonde de présenté (Q3) et la variante retenue
   consignées dans la fiche de mission et connues d'`adr/0003` au DOC.
5. Revue indépendante (correctness-reviewer) des contrats d'échec et QA
   indépendante (rendering-verifier) sur le diff intégré, en contexte frais.

## Graphe prévisionnel et fiches — non publiés

**B0 (fait, `b80c353`) → D-C1 → I-C1 → D-C2 → I0 → (R ∥ V) → DOC → I1.**
Références locales, pas IDs Ensemble. PM/LLM, deux scores entiers, heuristiques.

- **D-C1** (gpu-engineer) : brut C64 → retenu **70** (plancher dev), R66.
  TDD RED→GREEN→REFACTOR, écrivain unique des fichiers C1. `fresh`, `worktree:true`.
- **D-C2** (cpp-engineerOrGpu) : brut C44 → retenu **70** (plancher dev), R58.
  Files disjointes de C1 ; publication après intégration C1 (une ressource
  d'arbre de build, une barrière d'intégration ; le refuge architecte
  C1 ∥ C2 reste admissible si un deuxième arbre est disponible, sinon
  séquentiel). `fresh`, `worktree:true`, après I-C1.
- **R** (correctness-reviewer) : C50/R66, lecture seule du diff intégré et des
  preuves. `fresh`, `worktree:false`, après I0.
- **V** (rendering-verifier) : C45/R62, QA exécution sans code nouveau, baux
  sérialisés. `fresh`, `worktree:false`, après I0.
- **DOC** : C30/R33, clôture documentaire bornée (fiche/archive, ruling passé
  dans `adr/0003`, incidents, plan, manifeste). `fresh`, publication après R/V.

Barrières du coordinateur : **I-C1 / I0 / I1** intégrations vérifiées
(fichiers + empreintes + diff), jamais la fin de travail d'un auteur seule.

## Hors mission

Q2/Q4/Q5 (info, consignes de limite de preuve) restent au statut de warnings ;
22 autres GL sous ASan ; Q1 du script ASan ; workload/équivalence visuelle et
ADR-0006 ; **tranche Pipeline** (A1a stockage/emprunts `pipeline/{User,Resetter}`,
A1b `m_passes` unique_ptr + `forPass` emprunt + bornes/curseur, A1c fixtures
chargées avant teardown du contexte, A2 F6 contexte unique, avec fiche de
contrat préalable de l'architecte) ; **tranche ADR-0004** (uniforms préparés +
`UniformReader` durci avec test de concept négatif, F7) ; Release/CI/coverage.

## Autorité et limites de la sélection

PM/LLM classé de la mission : A1 d'abord (74/76) sur avis PM, ordre inverse
(arguments architecte : congeler les contrats d'échec avant churn, défaut Q1
réel) adopté par le coordinateur ; les consultations fresh ont remplacé les
sessions historiques non rattachées à cette session (`ses_ebf7537c9ffeSOEv…
` pour le PM, `ses_ebf7537c8ffeN6p5…` pour l'architecte — substitution
signalée). L'utilisateur a choisi C parmi les candidates et ordonné le commit
B0 de F5. Filenames C1/C2 documentées après exécution dans `known-incidents`
(ruling Q3 déjà consigné). **C est sélectionnée, pas corrigée** ; aucun PASS
global ne résulte de cette sélection.

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[development/known-incidents|Incidents]] ·
[[architecture/current-state|État réel]]
