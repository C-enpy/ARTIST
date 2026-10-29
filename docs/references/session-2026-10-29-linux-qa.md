---
title: "Parité QA Linux — session, sous-agents et identité des preuves"
category: reference
tags: [artist, reference, testing, build, tooling]
summary: "Mission Linux QA, contributions de quatorze sous-agents, revues et publication : résultats attribués, identité shader et limites conservées."
created: 2026-10-29
updated: 2026-10-29
source_revision: f80c7cb19ec8a0b551e272bb60814adcb7cbec83
sources:
  - research/sessions/2026-10-29-linux-qa-session/index.json
  - docs/_meta/session-2026-10-29-linux-qa.json
  - .opencode/evidence/mission-linux-qa/candidate-validation.md
  - .opencode/evidence/mission-linux-qa/final-test-audit.md
  - .opencode/evidence/mission-linux-qa/final-test-reaudit.md
  - .opencode/evidence/context-publication/qa-review.md
  - .opencode/missions/team_mvs9ispa_0000_ql1erh/reports/34-publication-du-contexte-sur-main.md
provenance:
  extracted: 0.92
  inferred: 0.08
  ambiguous: 0.0
---

# Parité QA Linux — décisions et enseignements

## Mandat et décisions utilisateur

La session part d'une question de prochaine mission, puis de la dette Linux
accumulée pendant les développements Windows. L'utilisateur demande de rattraper
la QA Windows, de mutualiser les outils et patterns existants, et de créer une
distribution WSL2 dédiée dont le disque est sur `I:`. WSL ne doit pas devenir une
preuve présumée de GPU matériel Linux. La procédure et les environnements
effectivement qualifiés sont dans [[development/linux-qa]].

Le mandat long run a permis les arbitrages courants dans ce périmètre, pas une
extension implicite des permissions ou objectifs. L'utilisateur a ensuite demandé
la fusion de la PR sur `main`, puis la publication du contexte restant hors
`.tmp/`. La configuration GrepAI contenant l'identifiant local reste sur disque,
ignorée et non suivie ; seul son exemple sans secret est publié.

La PR [3](https://github.com/C-enpy/ARTIST/pull/3), incluant les soixante commits
prérequis explicitement autorisés, a été fusionnée à `e170d71`. La publication
ultérieure du contexte arrive à `f80c7cb`. Ces commits d'intégration/documentation
ne sont pas rebaptisés candidats des campagnes moteur antérieures.

## Résultats, avec leurs frontières

- **Runtime `78e7b74`** : six configurations Windows, Ubuntu WSL et Ubuntu CI,
  normales et AddressSanitizer (ASan, détection d'erreurs mémoire), passent chacune
  **589/589** : 513 CPU et 76 OpenGL, zéro failure/error/skip. Les ensembles de noms
  concordent après le seul cast GLM documenté ; cardinalité seule ne suffisait pas.
- **Adaptateur `f58a49f`** : les shaders rejoignent les entrées hashées après
  RED causal, GREEN Python **10/10 sur chaque OS** et ré-audit indépendant. Quatre
  campagnes supplémentaires WSL/CI, normales/ASan, passent chacune 589/589 avec
  **169 empreintes dont 20 shaders**. CI finale
  [40841460327](https://github.com/C-enpy/ARTIST/actions/runs/40841460327).
  Les deux campagnes Windows antérieures sont réutilisées par identité, non
  réexécutées sous ce nouveau nom.
- **Harnais agentic** : la publication a révélé cinq échecs sur 594 tests, causés
  par la reconnaissance v2 limitée au planner alors que les trois profils QA
  partagent déjà ce contrat. Raccordement `7132c9a`, RED ciblé causal puis 633/633.
  La revue détecte ensuite une interdiction de runner non protégée dans l'oracle
  planner : négatif causal puis correction `be888ff`, **634/634**, aucun skip.
  Syntaxe/schéma/hors ligne : 41/81/215 contrôles PASS ; `missionVerdict` reste
  **INCONCLUSIVE**. Ce n'est ni un résultat moteur ni une preuve de confinement.

Les preuves exécutées sont celles des runners identifiés dans les dossiers et
réconciliées par le lead ; les reviewers n'ont pas exécuté ces campagnes.
Cette ingestion ne relance aucun de ces tests.

## Contributions attribuées

L'index des sources relie chaque rôle à son identifiant de session et son fichier,
avec hash et pagination. Les numéros ci-dessous désignent ces exports, pas des
verdicts ni des exigences de lancement de nouveaux workers.

- **Planification QA initiale et complément Attribute** (exports 01 et 08,
  qa-planner) : exigences, populations, nominal/négatif, baux de build/GPU et
  distinction rollback natif/CPU. Plans proposés, aucune exécution par ces agents.
- **Environnement WSL** (02, build-engineer) : distribution dédiée et sonde C++23 ;
  **adaptateur Linux** (03) : profil, presets et correctif d'oracle stacktrace ;
  **ASan** (04) : flags compile/link et canari sain/faute. Un fichier de lanceur
  absent n'est pas à lui seul un RED comportemental.
- **Intégration Linux et CI** (05, build-engineer) : contrôleur commun et campagnes
  ayant d'abord reproduit les trois échecs Attribute. Ces FAIL restent des données,
  pas des essais effacés par les runs finaux.
- **Diagnostic OpenGL** (06, gpu-engineer) et **frontière de compatibilité**
  (07, engine-architect) : lecture seule ; insuffisance du suffixe de nom pour
  classer array1, route moderne conditionnelle, clés legacy conservées et aucun
  rollback CPU du wrapper promis.
- **Correction Attribute** (09, gpu-engineer) : livraison auteur `ddab33a`, quatre
  campagnes auteur à **588/588** ; preuves recoupées après intégration, non
  confondues avec les six campagnes finales à 589.
- **Revue C++** (10, correctness-reviewer) : défaut bloquant de gate GLEW démontré
  en lecture ; correction causale et ré-audit favorable borné. Le flag expérimental
  GLEW ne prouve pas l'annonce effective de l'extension.
- **Recherche de gaps** (11, test-engineer) : propositions sur le candidat intégré,
  notamment renderer, options ASan hostiles et fraîcheur des entrées. Proposition,
  persistance et exécution restent attribuées séparément.
- **Audit final** (12, test-reviewer fresh) : omission des shaders dans l'identité
  du contrôleur, puis ré-audit fermant le finding après correction. L'audit n'a
  pas prétendu qu'un shader avait effectivement changé pendant les premiers runs.
- **Plan de publication** (13, qa-planner) et **revue/ré-audit des tests QA**
  (14, test-reviewer fresh) : exclusions des secrets et temporaires, détection de
  la restriction runner non verrouillée puis fermeture bornée. Finales natives
  reçues ; un transport `team_message` hors équipe refusé ne signifie pas que
  le travail manque, ni qu'il faut relancer un agent terminé.

Certaines remises d'équipe ne figurent que dans les rapports persistés, car les
appels/résultats d'outils ne sont pas exportés. Les deux anciennes entrées de plan
audit/documentation restent deferred/inactives malgré les livrables intégrés :
pas de complétion administrative fabriquée pour arrêter une relance automatique.

## Enseignements réutilisables

1. **L'identité d'une campagne inclut les entrées ouvertes à l'exécution.** Hasher
   code et binaire sans shader ne ferme pas ce contrat ; ajouter leur hash renforce
   la preuve suivante sans transformer les précédentes en essais du nouveau helper.
2. **Une capacité observée a plusieurs conditions.** Version ou extension
   effectivement annoncée et fonctions nécessaires doivent être distinguées des
   flags d'un loader et du nom de la carte. La classification moderne interne
   n'oblige pas à renommer les clés publiques historiques.
3. **La protection d'une consigne et son enforcement sont deux preuves.** Un test
   textuel peut détecter la suppression de « pas de runner », mais ne confine pas
   les capacités exposées par Code Mode.

Ces trois constats suggèrent de préférer un adaptateur de preuve commun et des
oracles ciblés à deux harnais d'OS divergents. ^[inferred]
Conserver l'identité des populations et des entrées rend plus utile la réutilisation
de preuves que l'addition de simples compteurs verts. ^[inferred]

## Limites et ADR

Linux graphique qualifié : **Mesa llvmpipe seulement**. ASan avec `detect_leaks=0`,
dépendances Conan/système/driver non instrumentées. Aucun GPU matériel Linux,
Release, UBSan, coverage, performance ou recherche exhaustive de fuites validé.
Le premier delta RED classification n'a pas été hashé complètement à chaud ;
l'incident d'incrémentalité Windows a disparu après clean à sources inchangées,
sans cause du suivi de dépendances entièrement démontrée. Un pilote OpenGL 3.3
ambigu sans réflexion moderne reste non universellement qualifié.

[[adr/0005-build-reproductible-et-validation|ADR-0005]] reste Accepted/partial ;
[[adr/0008-conan2-et-baseline-windows|ADR-0008]] reste vérifié dans sa baseline
Windows Debug, avec preuve Linux distincte. [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]]
reste Proposed/partial : corriger la classification n'ajoute ni binding Attribute
préparé ni format cooké. [[adr/README|Les autres statuts]] ne changent pas.

## Provenance de cette ingestion

Archive `research/sessions/2026-10-29-linux-qa-session/` : **15 transcriptions**,
250 messages publics, 236188 octets ; principal et **14 enfants directs** retournés
par l'API native. Pagination jusqu'à épuisement, SHA256 par transcription ; borne
exclusive `msg_187a2d6b5001tBMoEtVELgbnOt`, 29 octobre 2026 à 12:29:20.478 UTC.
Messages humains du principal et texte assistant seulement ; briefs des workers,
messages user synthétiques, raisonnement, outils, pièces jointes et enveloppes
privées exclus. Secrets reconnaissables et chemins personnels masqués.

Pas de revendication d'inventaire récursif ni de récupération des préparations
séparément parentées. Ce n'est pas un backup exhaustif ou une preuve d'exécution.
Les onze exports de la [[references/session-2026-10-27-prepared-float|session float préparé]]
restent inchangés, sans nouvelle ingestion de leur contenu.

Retour : [[development/linux-qa|Procédure]] · [[development/known-incidents|Incidents]] ·
[[opencode-harness|Coordination et permissions]] · [[index|Accueil]].
