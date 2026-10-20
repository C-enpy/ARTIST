---
title: "Prochaine mission — chemin d'exécution Pipeline"
category: architecture
tags: [artist, architecture, rendering, cpp]
aliases: [next-mission]
summary: "Mission proposée : chemin d'exécution concret pour la chaîne OpenGL Classic à deux passes, en coexistence avec IPipeline. Non démarrée. ADR-0002 reste Proposed."
status: proposed
implementation: not-started
source_revision: 8f4c81599b089cca6b38b553873783e609986627
created: 2026-10-19
updated: 2026-10-19
sources:
  - "architecture/missions/2026-10-19-pipeline-execution-path.md"
  - "architecture/correction-plan.md"
provenance:
  extracted: 0.7
  inferred: 0.3
  ambiguous: 0.0
---

# Prochaine mission — chemin d'exécution Pipeline

> [!important] Proposée le 2026-10-19, non démarrée
> L'ordre complet est
> [[architecture/missions/2026-10-19-pipeline-execution-path|la fiche Pipeline]].
> Préparation documentaire seulement : pas de worker, pas de modification de
> `ARTIST/`, pas de `/ensemble-mission start`. ADR-0002, ADR-0004 et ADR-0006
> restent Proposed. ADR-0003 reste Accepted/partial.

La lifetime OpenGL est close. Son pointeur est archivé :
[[architecture/missions/2026-10-19-next-mission-lifetime-closed|mission lifetime]].
La tranche suivante du [[architecture/correction-plan|plan de correction]] est
le chemin d'exécution spécialisé.

**Résultat attendu.** Un type d'exécution fermé, à côté de `IPipeline`, pour
les deux passes déjà exercées par `PipelineTest` (OpenGL Classic). Même ordre
de programmes GL que `use` / `useNext`. Même règle `reset` ≠ unbind. Le
curseur et les erreurs actuelles restent ceux déjà testés.

**Contrainte.** La topologie et la forme de coexistence sont figées par
`engine-architect` puis par une décision humaine avant tout auteur. Si la
forme correcte est une refonte du dispatch, la mission s'arrête.

**Preuve.** Readback `GL_CURRENT_PROGRAM` aligné sur `UsePassTest`,
`UseNextPassTest` et `ResetPipelineTest`, plus les suites `unit` et `opengl`
du candidat Windows Debug. Pas de gain de performance revendiqué.

Le graphe, les scores PM/LLM, le registre QA et les commandes sont dans la
fiche. Le démarrage ultérieur passe par `/ensemble-mission`, `config`, puis
le `start` de l'utilisateur.
