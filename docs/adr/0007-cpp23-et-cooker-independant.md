---
title: "ADR-0007 — Runtime C++23 et préparation indépendante du langage"
adr_id: ADR-0007
category: adr
tags: [artist, architecture, cpp, assets, adr]
aliases: [ADR-0007]
status: Proposed
implementation: partial
implementation_note: "Audit du 2026-10-29 : correction initiale compilée/testée en C++23 ; sous-contrats cooker, formats et pilote de langage conditionnels non déclenchés."
decision_origin: Recommandations explicites des rapports R3 et R4, appliquées au périmètre actuel.
summary: "Proposed/partial : correction initiale C++23 réalisée ; futur cooker et format versionné conditionnels, aucun changement de langage adopté."
created: 2026-10-03
updated: 2026-10-29
audit_date: 2026-10-29
sources: [.opencode/evidence/adr-audit-2026-10-29/inputs.json, .opencode/evidence/adr-audit-2026-10-29/public-deliveries.json, docs/_meta/session-2026-10-29-linux-qa.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# ADR-0007 — C++23 et cooker indépendant

## Audit au 29 octobre 2026

**Proposed / partial**, au lieu de not-started : le premier sous-contrat est
réalisé. La correction compile/teste en C++23, sans réécriture, notamment dans les
six configurations de la QA Windows/WSL/CI (`78e7b74`, 589 chacune).
Cette qualification ne ratifie pas globalement l'ADR. Le cooker, son schéma et
un pilote éventuel d'un autre langage n'étant pas engagés, leurs clauses restent
conditionnelles et ne deviennent pas des chantiers exigés pour la QA actuelle.
[[references/adr-audit-2026-10-29#ADR-0007|Toutes les obligations et leur applicabilité]].

## Contexte et faits

Le dépôt est une bibliothèque C++23/OpenGL ; aucun runtime Rust ou cooker ne
constitue la baseline observée. Les rapports R3/R4 recommandent de corriger
l'architecture C++ avant de prendre une décision de migration. Rust est une piste
pour parsing, ownership et concurrence, pas une accélération GPU automatique.

## Décision proposée

- Garder C++23 comme langage de la correction du runtime et de ses kernels.
  Cette préférence ne vaut pas rejet permanent d'un autre langage.
- Si un cooker est introduit, définir d'abord des représentations et formats
  versionnés indépendants des pointeurs, vtables et types propres au langage.
  Les noms `SceneIR`/`RenderIR` sont conceptuels, pas des API déjà existantes.
- Choisir Rust ou C++ pour cet outil selon contrat, qualité du parsing, concurrence,
  dépendances, déterminisme et maintenance. Le langage du cooker n'impose pas de
  FFI dans chaque frame lorsque son interface est un produit de données offline.
- Limiter la génération aux layouts et familles de kernels réellement utiles.
  La scène complète en code reste une expérience évaluée par
  [[adr/0006-preuves-performance-et-determinisme|ADR-0006]].
- Évaluer les bibliothèques d'import/animation/physique derrière un contrat étroit
  plutôt que réimplémenter tout par principe. Aucun nom de bibliothèque des
  rapports n'est ratifié comme dépendance par cet ADR.

## Si une migration est étudiée

Partir de la meilleure baseline C++ corrigée, pas du graphe dynamique actuel.
Comparer une tranche équivalente, y compris backend, workload, code généré,
ownership, erreurs et coût d'itération. Un prototype Rust/Vulkan ne doit pas être
comparé naïvement à C++/OpenGL pour attribuer l'écart au langage.

Traduire `shared_ptr` en `Arc` et `virtual` en `dyn Trait` ne résout pas les
frontières du modèle. La sûreté, la maintenabilité et la concurrence sont des
axes légitimes à évaluer séparément des cycles CPU et du temps GPU.

## Alternatives et conséquences

Une réécriture immédiate cumule changements de langage, backend, ownership et
outillage. Tout imposer en C++ au cooker peut perdre des avantages d'un outil
séparé. La proposition conserve l'investissement et laisse un pilote borné
trancher lorsque la baseline rend la comparaison interprétable.

## Critères de validation

La correction initiale compile/teste en C++ sans réécriture de langage.
Un futur produit cooké a un schéma, une version et des règles de compatibilité
testables. Un pilote éventuel documente hypothèses, critères préalables et
résultats ; aucune échéance en personne-mois n'est importée comme engagement.

**Preuves actuelles** : correction initiale compilée et testée en C++23 ;
préparation float OpenGL runtime réalisée séparément (ADR-0004). Aucun produit
cooké, format d'échange versionné ou pilote de comparaison de langages introduit.

## Références

[[references/research-reports|R3 et R4]] ·
[[adr/0001-resolution-multi-etapes|Frontières]] ·
[[adr/0004-layouts-types-et-donnees-preparees|Données préparées]] · [[adr/README|Registre]].
