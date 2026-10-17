---
title: "Prochaine mission — aucune sélection, amendement oracle terminé"
category: architecture
tags: [artist, architecture, cpp, testing, pipeline]
aliases: [next-mission, Fiche de sélection]
summary: "Pipeline A1+A2 archivé et amendement GMock Attribute b9c90fd terminé au périmètre : revue indépendante PASS, QA176CPU/176ASan et contrôles négatifs/positifs conformes. Fragilité de shuffle préexistante conservée ; aucune nouvelle mission choisie, aucun PASS global moteur."
status: completed
implementation: verified
source_revision: b9c90fd51b0ff8a85b1db1d0674c1243ccdd0951
created: 2026-10-17
updated: 2026-10-17
sources: [docs/architecture/missions/2026-10-16-pipeline-ownership.md, docs/_meta/session-2026-10-16-pipeline-final-validation.json, docs/development/known-incidents.md, docs/architecture/correction-plan.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md]
provenance:
  extracted: 0.98
  inferred: 0.02
  ambiguous: 0.0
---

# Aucune nouvelle mission sélectionnée — amendement oracle terminé au périmètre

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

> [!success] Amendement GMock corrigé et vérifié, sans PASS global moteur
> Défaut d'oracle **préexistant conservé historiquement comme FAIL**, puis
> corrigé dans l'amendement autorisé : ancien `AttributeTests.cpp:65`
> (`AttributeTest.SetTest`) —
> expectation `mockOn` attendue une fois, observée zéro **après summary** ; exit
> 0 du processus et CTest passant malgré le diagnostic. Reproduit en direct
> **et** sous CTest normal/ASan ; visible dans `LastTest.log` (`out/mission-a1a2/
> i0-verification/asan-last-test.log:2494-2501`, filtre individuel + « Test
> Passed ») ; la sortie JUnit tronquée à 1024 octets cachait l'erreur, le témoin
> à 65536 + `LastTest.log` le prouvent.
> L'utilisateur a **autorisé sa correction** le 2026-10-17 (« Alors vas-y »).
> Le plan PM/LLM est exécuté : G1 `cpp-engineer`, complexité **74**, risque **72**,
> sous la politique de mission inchangée. `AttributeTests.cpp` est le propriétaire
> principal du diff ; le setter moteur reste en lecture seule, sans adoption F7.
> Le checkpoint oracle-only `d1a9ee1` conserve l'ancienne attente et produit le
> **RED causal** : binaire exit1, CTest exit8, JUnit failure1 avant résumé, sans
> crash ; binaire négatif préservé et empreinte recoupée. Ce n'est pas encore
> un PASS global. Le GREEN est intégré en `b9c90fd` : un seul fichier test,
> production inchangée. La revue G2 fresh est **PASS au périmètre, zéro finding**.
> La QA G3 indépendante confirme 176 CPU normaux, 176 CPU ASan, contrôles
> négatifs réels répétés (binaire1/CTest8/XML failure1) et positifs propres.
> 137/137 artefacts et 130/130 empreintes recoupés. GoogleTest v1.14.0 garantit
> statiquement le TearDown après assertion fatale, sans scénario exécuté dédié.
> Une fragilité Loader/ShaderLoader au shuffle est
> reproduite à l'identique sur le vrai binaire pré-G1 et le GREEN (deux seeds),
> conservée séparément, non corrigée par cet amendement.
> Contrat, scores, IDs et preuves :
> `_meta/session-2026-10-17-attribute-oracle-plan.json`.
> État d'intégration : `_meta/session-2026-10-17-attribute-oracle-validation.json`.
> Les décomptes historiques 215/198 ne deviennent pas « tous oracles valides ».

## Ce qui reste ouvert après la mission Pipeline (hérité, non sélectionné ici)

- **F7** : maps uniform/attribute et contextes des wrappers restent **shared**
  (hors tranche).
- **Ordre des fixtures Loader/ShaderLoader** : échecs sous deux seeds reproduits
  à l'identique sur le binaire pré-G1 et le GREEN ; sujet de suivi séparé,
  aucune correction ou nouvelle mission choisie implicitement.
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
