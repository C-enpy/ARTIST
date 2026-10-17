---
title: "Prochaine mission — AUCUNE : Pipeline A1+A2 terminée et archivée"
category: architecture
tags: [artist, architecture, ownership]
status: none
implementation: none
source_revision: 738891fb98e23ee0dbe4afb1f1009c225105d894
created: 2026-10-15
updated: 2026-10-16
sources: [docs/architecture/missions/2026-10-16-pipeline-ownership.md, docs/architecture/correction-plan.md, docs/development/known-incidents.md]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Prochaine mission : AUCUNE — Pipeline A1+A2 terminée et archivée

> [!important] Lecture de cette fiche
> La fiche de sélection **Pipeline A1+A2** a été exécutée le 2026-10-16 dans le périmètre
> Windows x64 Debug et son contrat intégral est préservé — non modifié — dans l'archive
> [[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline A1+A2 (ownership
> du Pipeline, curseur et contexte unique)]], avec le journal des filières, les statuts
> par finding et les manifestes `_meta/session-2026-10-16-pipeline-{i-a1,a1c,i0,final}-validation.json`
> et `_meta/session-2026-10-16-asan-caller-restoration.json`. Les preuves non versionnées
> restent sous `C:/Users/djden/source/repos/ARTIST/out/mission-a1a2/`.

**État de la sélection : AUCUNE nouvelle mission sélectionnée par l'utilisateur.** Cette page
reste un pointeur ; toute nouvelle sélection passe par une décision explicite de l'utilisateur
et une coupe PM/LLM fresh. ADR-0003 reste **Accepted / partial** (tranches Shader→Pass puis
Pipeline A1+A2 vérifiées dans leur périmètre) ; ADR-0004 reste **Proposed / not-started**,
à adoption explicite séparée.

## Résumé de la mission terminée

- Sous-tranches A1a/A1b/A1c et A2/F6 intégrées en `d319096`, `3c670c9`, `18edc72` (I0),
  `f914d77` (A2 + H-RESTORE) ; harnais corrigé en `95c2b9e` ; branches latérales
  H-ASan, S-GSS, DOC-REF, F8-SEAM, H-RESTORE et H-FIXTURE intégrées.
- Preuves recoupées : I0 **214/214** normales (175 CPU + 39 GL) et **197 ASan** exacts ;
  candidat combiné **215/215** (176 CPU + 39 GL) et **198 ASan** exacts (176 + 22 GL ;
  17 autres GL non sélectionnés) ; sondes getter 0/2/0 ; H-RESTORE 5 cas réels séparés ;
  H-FIXTURE vérifiée indépendamment (9 + 5 harnais, binaires identiques, moteur non relancé).
- **Aucun PASS global** : oracle GMock préexistant `AttributeTest.SetTest`
  (`AttributeTests.cpp:65`) reproduit direct ET CTest, FAIL conservé ; deux FAIL historiques
  conservés (protocole RED A1c, pollution d'environnement I0) ; bilan final V-A2
  **14 PASS / 3 FAIL / 7 INCONCLUSIVE**.

## Findings et décisions laissés ouverts (non sélectionnés)

- **Oracle GMock `AttributeTest.SetTest`** : défaut de suite préexistant hors mission
  (diagnostic post-résumé, 1024 tronqué / témoin 65536) — à ouvrir et suivre dans
  [[development/known-incidents|les incidents]], correction hors de cette mission.
- **Reader `removeError`/liens** : question INCONCLUSIVE ; close-failure et panne filesystem
  réelle non prouvées (F8 partiel).
- **Latent `#if` libstdc++** de `getStackTraceString` (const-correctness), GCC inatteignable
  ici — coupe dédiée avant toute exposition.
- **ADR-0004** (layouts typés et bindings), **F7** (`UniformReader` absent d'`OpenGLPassFlow`,
  concept négatif à écrire), **les 17 autres GL non sélectionnés sous ASan**, Release/CI/couverture,
  performance/équivalence visuelle (ADR-0006), Q2/Q4/Q5 : tous hors mission, à sélectionner
  explicitement.

Retour : [[architecture/correction-plan|Plan de correction]] ·
[[development/known-incidents|Incidents]] ·
[[architecture/missions/2026-10-16-pipeline-ownership|Archive Pipeline A1+A2]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
[[adr/README|Registre des ADR]]
