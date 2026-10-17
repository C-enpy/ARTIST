---
title: "Amendement oracle en cours — aucune nouvelle mission sélectionnée"
category: architecture
tags: [artist, architecture, cpp, testing, pipeline]
aliases: [next-mission, Fiche de sélection]
summary: "Socle Pipeline A1+A2 archivé avec ses réserves ; amendement GMock Attribute autorisé le 2026-10-17 et engagé sous la même politique. RED causal recoupé, GREEN intégré b9c90fd, revue/QA indépendantes encore attendues. ADR-0003 reste Accepted/partial et ADR-0004 hors mission."
status: in-progress
implementation: partial
source_revision: 95c2b9e3064a0af4bf1a50f78055ba2d5a78526a
created: 2026-10-17
updated: 2026-10-17
sources: [docs/architecture/missions/2026-10-16-pipeline-ownership.md, docs/_meta/session-2026-10-16-pipeline-final-validation.json, docs/development/known-incidents.md, docs/architecture/correction-plan.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md]
provenance:
  extracted: 0.98
  inferred: 0.02
  ambiguous: 0.0
---

# Aucune nouvelle mission sélectionnée — amendement oracle en cours

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

> [!warning] Amendement GMock intégré — vérifications indépendantes attendues
> Défaut d'oracle **préexistant, hors périmètre de la mission Pipeline, retenu
> FAIL** : `ARTIST/tests/.../AttributeTests.cpp:65` (`AttributeTest.SetTest`) —
> expectation `mockOn` attendue une fois, observée zéro **après summary** ; exit
> 0 du processus et CTest passant malgré le diagnostic. Reproduit en direct
> **et** sous CTest normal/ASan ; visible dans `LastTest.log` (`out/mission-a1a2/
> i0-verification/asan-last-test.log:2494-2501`, filtre individuel + « Test
> Passed ») ; la sortie JUnit tronquée à 1024 octets cachait l'erreur, le témoin
> à 65536 + `LastTest.log` le prouvent.
> L'utilisateur a **autorisé sa correction** le 2026-10-17 (« Alors vas-y »).
> Le plan PM/LLM est validé : G1 `cpp-engineer`, complexité **74**, risque **72**,
> sous la politique de mission inchangée. `AttributeTests.cpp` est le propriétaire
> principal du diff ; le setter moteur reste en lecture seule, sans adoption F7.
> Le checkpoint oracle-only `d1a9ee1` conserve l'ancienne attente et produit le
> **RED causal** : binaire exit1, CTest exit8, JUnit failure1 avant résumé, sans
> crash ; binaire négatif préservé et empreinte recoupée. Ce n'est pas encore
> un PASS global. Le GREEN est maintenant intégré en `b9c90fd` : un seul fichier
> test, production inchangée, producteur 176 CPU et 176 CPU ASan conformes.
> La revue G2 et la QA G3 fresh sont publiées sur ce candidat intégré ; leurs
> résultats restent attendus. Une fragilité Loader/ShaderLoader au shuffle est
> reproduite à l'identique sur le vrai binaire pré-G1 et le GREEN (deux seeds),
> conservée séparément, non corrigée par cet amendement.
> Contrat, scores, IDs et preuves :
> `_meta/session-2026-10-17-attribute-oracle-plan.json`.
> État d'intégration : `_meta/session-2026-10-17-attribute-oracle-validation.json`.
> Les décomptes historiques 215/198 ne deviennent pas « tous oracles valides ».

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
  `.opencode/skills/artist-build-validation/references/validation-levels.md`
  (absence constatée, pas de restauration dans cette mission).

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
