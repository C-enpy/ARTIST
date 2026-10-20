---
title: "Session lifetime OpenGL — politique, exécution et enseignements"
category: reference
tags: [artist, rendering, testing, reference, architecture]
summary: "Session root ses_eace2b30 et 30 descendants : contrats K ratifiés, preflightGate, exécution TDD, correctifs R-F1/R-Q1, audit V borné COVERED sur 71dacbb."
created: 2026-10-19
updated: 2026-10-19
source_revision: 71dacbb40ad8746fce5e57cd3cbecc2e1fa8a493
sources:
  - "opencode:ses_eace2b30bffd3si1XqGDa0rysS"
  - "research/sessions/opencode-lifetime-20261019/index.json"
  - "docs/architecture/missions/2026-10-19-opengl-lifetime-contracts.md"
provenance:
  extracted: 0.85
  inferred: 0.15
  ambiguous: 0.0
---

# Session lifetime OpenGL

## Périmètre de l'ingestion

La session racine `ses_eace2b30bffd3si1XqGDa0rysS` (« OpenGL resource lifetime
mission policy and plan review », orchestrator) et **30 descendants
accessibles** (15 pools + 11 forks + 4 sessions de préparation du jour) sont
exportés verbatim sous
[[research-sessions-opencode-lifetime|`research/sessions/opencode-lifetime-20261019/`]].
Le premier export (2026-10-19T11:41Z) est conservé pour les 29 sessions dont le
compte de messages n'a pas bougé. La racine et `ses_eac13149b` (fichier encore
nommé pool-25 fork, agent devenu test-reviewer, audit V) sont réexportées
intégralement à la clôture ; le compte et l'horodatage sont dans `index.json`
(`refreshed`). Les entrées comprennent idle, synthetic et compaction : ce n'est
pas un verbatim du service hors de ces sessions. Audit V clos : PASS borné,
registre COVERED, candidat `result_mvf8gjec_008t_9t7qc0`. Détail :
[[architecture/missions/2026-10-19-opengl-lifetime-audit-v|l'audit V]].

Chaîne de préparation incluse : orchestration native
(`ses_eacea7ee`/`ses_eace9372`) et les deux PM en lecture seule
(`ses_eacf31fb` « Cadrer mission long run », `ses_eacf2ea5` « Évaluer
alternatives parallélisables ») sur `070e20d`.

## Politique de routage — le preflightGate en production

- **Grille approuvée révision 1** (hash `13d34967…`) : matrice 4×4
  risk×complexity, groupes `light: openai/gpt-6-luna (high)`,
  `standard: opencode-go/glm-5.3-flash (max)`,
  `strong: openai/gpt-6.1-sol (xhigh)`, cellule dédiée
  `preflight: openai/gpt-6.1-sol (medium)` à [risque 55-66 × complexité 34-54].
  Les variants natifs recoupés avec les politiques sœurs : « extra high » =
  `xhigh` ; Luna `max` reste rejetée par le prédicat opérateur.
- **`preflightGate: { strong: ["preflight"] }`** appliqué à TOUTES les tâches
  strong : une tâche support de lecture seule (Sol medium) précède chaque
  exécution strong (Sol xhigh), l'agent de développement est **forké sur la
  session support** (fork natif strictement postérieur à la complétion) —
  fresh/reuse/checkpoint ne prouvent pas la découverte en v1.
- Conséquence structurelle : **toute tâche de développement est strong**
  (plancher `max(70, brute)`) donc gated — les micro-tranches aussi. La paire
  support n'est pas factice quand elle produit un dossier d'oracle ; un
  arbitrage de contention de fichier (un seul écrivain) sérialise les
  correctifs qui touchent le même header.

## Contrats adoptés (gate humain du 2026-10-19)

Voir [[architecture/missions/2026-10-19-opengl-lifetime-contracts|la note de
contrat]]. Les cinq ratifications d'origine : atomicité de réflexion **par
Reader** (pas de transaction Pass) ; refus Uniform **avant mutation CPU** ;
owner natif VBO **au contexte Attribute** (`getBufferID` = emprunt seul) ;
destructeur owner non vide sans contexte = **fail-fast sans appel GL** ; tranche
runtime = contexte courant exact GLFW/GLEW, sans gestionnaire global.

Deux décisions complémentaires marquantes de la session :

- **`setBufferID` = adoption exclusive** (décision lead dans le cadre ratifié) :
  ID transféré, même-ID no-op, ID différent = prévalidation puis release de
  l'ancien avant adoption, zéro = release. L'alternative `adoptBufferID`
  séparée ajouterait une API publique non ratifiée.
- **Extension R-Q1 ratifiée par le décideur** (K3-parité) : `Attribute::set`
  prévalide le contexte **avant toute mutation CPU** — sous mauvais contexte,
  l'ancienne valeur CPU est conservée (l'historique stockait la CPU avant le
  garde du Setter). Committée `6847523`.

## Rectifications et défaites d'illusions de la session

- **Le « zéro `glDeleteBuffers` dépôt entier » était faux.** Absence confirmée
  dans `ARTIST/include`, mais deux cleanups de fixtures existent côté
  component-tests (`AttributeComponentTests.cpp` ~95 et ~538). L'absence ne
  doit pas être annoncée au-delà de sa portée réelle.
- **`HasComponent` ne vérifie pas `::on`** : `set<bool>` est potentiellement
  ill-formed, pas une exception `UNSUPPORTED_TYPE` prouvée — corrigé par le
  garde `HasBorrowedOnMethod` existant (aucun edit du header partagé).
- **Un binaire périmé produit de faux verdicts.** Deux fois, un build en échec
  a laissé l'exe précédent en place ; les runs de vérification H ont alors
  « reproduit » une contamination inexistante. Le hash du binaire est le
  discriminant : hash identique = preuve périmée, à exclure du verdict.
- **La contamination réelle (H) a deux schémas causaux** : le sticky ON_CALL
  ciblé (`glGetShaderiv(1, GL_COMPILE_STATUS)→GL_FALSE`) en seed 61219, et un
  poseur amont différent en seed 7 (`ShaderLifetimeTests.cpp:359-364`,
  ON_CALL persistants `glCreateShader→8`/`glCreateProgram→5`). Corrigés par
  patch fixture-scopé (`c4df08b`) — jamais de reset global modifié.
- **Le comptage « zéro émission » natif n'est pas comptable** avec les helpers
  actuels : les readbacks prouvent l'isolation d'état, pas l'absence littérale
  d'appel ; le zéro strict est prouvé par le journal mock. Ne pas présenter un
  readback comme compteur.
- **Piège PowerShell 5.1** : un pipeline `2>&1 | Out-String` autour d'un script
  transforme la première ligne stderr d'un exe natif en `NativeCommandError`
  terminal et peut produire un log vide. Relancer conforme à la doc (`-File`,
  sans pipeline enveloppant) ; rediriger au niveau `cmd /c`.

## Preuves exécutées (registre, binaire `0a952ff3…`)

- **Baseline fraîche** : 266/266 unit + 60/60 GL sur `cd1b598` (manifeste
  `docs/_meta/session-2026-10-19-opengl-lifetime-baseline.json`) ;
- **RED/GREEN par tranche** (logs `out/`, hashés dans l'inventaire) : mock
  (7/273 → 273/273), A (14+3 → 446/446+63/63), B (19+terminate → 462/462),
  MC (précédence → 463/463), RQ1 (CPU mutée → 464/464), RF1 (2/2 → 466/466) ;
- **Croisée B→C native : PASS** (test #500, `ReflectedProgramGuardCrossTest`) —
  programID réfléchi → garde → refus au mismatch → reprise au rebinding ;
- **Contiguïté** : les logs `shuffle-*-rf1.log` et `pair-seed*-rf1.log` sont
  verts, mais l'audit ne les requalifie pas seuls par le hash du binaire. Bornes
  H acceptées, même exe `0a952ff3…`, 2026-10-19T14:24–14:25 : graines 1 et 3
  échec→création 2/2, graine 4 création→échec 2/2, `LoadShaderFailureTest`
  isolé 1/1, shuffles 7 et 61219 466/466.

## Enseignements harness

- Les exécutions centralisées au lead (modèle AGENTS) coûtent un aller-retour
  par itération de compilation ; en répertoire partagé sans worktree (dirty
  externe non transférable), le build d'un auteur en cours **bloque** la
  vérification des autres — sérialiser ou scoper strictement les fichiers.
- Les agents signalent correctement leurs blocages procéduraux quand on leur
  laisse le droit de ne pas écrire dans l'arbre partagé (pool-4, pool-13) ;
  le refus de mutation hors périmètre (pool-11 sur `ShaderLifetimeTests`) est
  exactement le comportement attendu du protocol.
- Le relais d'autorisation via l'historique fork (message envoyé au support
  avant la liaison du fork) fonctionne pour passer un brief à l'exécution
  forkée.

Voir [[architecture/missions/2026-10-19-next-mission-lifetime-closed|la mission archivée]], [[architecture/missions/2026-10-19-opengl-lifetime-qa-plan|le plan QA]] et [[architecture/missions/2026-10-19-attribute-session-ingestion|l'ingestion précédente]] pour le contexte.
