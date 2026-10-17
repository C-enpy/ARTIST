---
title: "Prochaine mission — extension oracle PENDING, aucune nouvelle mission sélectionnée"
category: architecture
tags: [artist, architecture, cpp, testing, pipeline]
aliases: [next-mission, Fiche de sélection]
summary: "La mission Pipeline A1+A2 (ownership du Pipeline et de IPass, curseur, harnais) est terminée et archivée intégralement ; ADR-0003 reste Accepted/partial ; seule est pointée l'EXTENSION ORACLE PENDING : amendement de l'oracle GMock Attribute préexistant autorisé par l'utilisateur le 2026-10-17, contrat en préparation par le PM, distinct de G-FIX PENDING. ADR-0004 reste hors mission."
status: pending
implementation: not-started
source_revision: 95c2b9e3064a0af4bf1a50f78055ba2d5a78526a
created: 2026-10-17
updated: 2026-10-17
sources: [docs/architecture/missions/2026-10-16-pipeline-ownership.md, docs/_meta/session-2026-10-16-pipeline-final-validation.json, docs/development/known-incidents.md, docs/architecture/correction-plan.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md]
provenance:
  extracted: 0.98
  inferred: 0.02
  ambiguous: 0.0
---

# Prochaine mission : rien n'est sélectionné — extension oracle PENDING

La [[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline A1+A2 —
ownership du Pipeline, curseur, contexte unique et restauration du harnais]] est
**terminée dans son périmètre** ; son contrat (G-A, A2/F6, F8-SEAM, découpage,
garde-fous) y est archivé **intégralement**. État au principal : C++ `f914d77`
(les six fichiers A2, suite **215/215** normales = 176 unitaires + 39 OpenGL,
sélection ASan **198/198** = 176 + 22, 17 autres GL non sélectionnés), harnais
`95c2b9e` (P2 fixture corrigé et vérifié indépendamment, cinq binaires
inchangés, **aucun rerun moteur**). QA indépendante finale : **24 critères —
14 PASS, 3 FAIL, 7 INCONCLUSIVE**, aucun PASS global
([[architecture/missions/2026-10-16-pipeline-ownership|détails et chiffres]]).
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] reste **Accepted / partial**
(migration A1+A2 vérifiée à ce périmètre ; F7 resté hors tranche).

**Cette fiche ne sélectionne aucune nouvelle mission** : l'ordre du
[[architecture/correction-plan|plan de correction]] est inchangé et
[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] reste
`Proposed / not-started`, à adoption explicite séparée.

> [!warning] EXTENSION ORACLE PENDING — contrat en préparation, pas exécutable ici
> Défaut d'oracle **préexistant, hors périmètre de la mission Pipeline, retenu
> FAIL** : `ARTIST/tests/.../AttributeTests.cpp:65` (`AttributeTest.SetTest`) —
> expectation `mockOn` attendue une fois, observée zéro **après summary** ; exit
> 0 du processus et CTest passant malgré le diagnostic. Reproduit en direct
> **et** sous CTest normal/ASan ; visible dans `LastTest.log` (`out/mission-a1a2/
> i0-verification/asan-last-test.log:2494-2501`, filtre individuel + « Test
> Passed ») ; la sortie JUnit tronquée à 1024 octets cachait l'erreur, le témoin
> à 65536 + `LastTest.log` le prouvent.
> L'utilisateur a **autorisé sa correction** le 2026-10-17 (« Alors vas-y »). Un
> amendement C++ (code d'oracle, périmètre minimal) est **en préparation par le
> PM**, distinct de G-FIX PENDING : cette fiche ne contient **pas encore** de
> contrat, ni de partition de fichiers, ni de scores. Ne préjuger **ni** le
> périmètre **ni** un GREEN : l'amendement attendra sa fiche, son RED causal,
> son intégration, puis une revue/QA indépendantes. Le FAIL d'oracle reste
> l'historique consigné jusqu'à preuve du contraire, et il empêche de qualifier
> 215/198 comme « tous oracles valides ».

## Ce qui reste ouvert après la mission Pipeline (hérité, non sélectionné ici)

- **F7** : maps uniform/attribute et contextes des wrappers restent **shared**
  (hors tranche).
- **FAIL historiques conservés** : protocole RED A1c (cleanup GL chargé après
  teardown, sonde jamais réexécutée) ; environnement appelant ASan (reproduit
  FAIL, corrigé par H-RESTORE sans effacer la preuve).
- **P3 non prouvées** : `ReaderTests` `removeError` non consulté avec `exists`
  suivant les liens ; fermeture échouée et panne OS d'extraction (F8 partiel) ;
  latent `getStackTraceString` const `libstdc++` hors MSVC ; fenêtre
  signal/état + TOCTOU de H-FIXTURE (exclusivité prouvée, pas atomicité ni
  interruption brutale) — détail dans [[development/known-incidents]].
- **Lifetime GPU non résolu** : emprunts invalidés par assignation/destruction,
  contexte GL courant requis, suppression différée d'un programme courant
  jusqu'à unbind, aucune promesse pour un GPU en vol au-delà du scope C++ ou de
  la frame de soumission — voir l'archive et l'« État réel ».
- **17 autres GL non sélectionnés** sous ASan, sans campagne dédiée.

## Hors mission (repris de l'archive)

ADR-0004 (adoption explicite séparée) ; Q2/Q4/Q5 ; Release/CI distante/
couverture ; performance, équivalence visuelle et
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]] ; F8 exhaustif ;
signature exacte des concepts ; les 17 autres GL sous ASan ; RAII exhaustif ;
toute refonte de render graph ou de backend ; restauration de
`docs/development/validation-levels.md` (absente, aucune source pour la
reconstruire).

## Continuité documentaire

- Archive de la mission Pipeline :
  [[architecture/missions/2026-10-16-pipeline-ownership|contrat, chiffres et échecs retenus]].
- Registre des statuts par finding :
  [[development/known-incidents|Incidents et suivi]].
- Ordre des travaux : [[architecture/correction-plan|plan de correction]].
- Commandes et limites d'exécution : [[development/build-and-test|Build et tests]].

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[architecture/missions/2026-10-16-pipeline-ownership|Mission Pipeline]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
[[development/known-incidents|Incidents]]
