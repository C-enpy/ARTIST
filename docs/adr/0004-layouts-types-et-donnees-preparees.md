---
title: "ADR-0004 — Layouts typés et données préparées"
adr_id: ADR-0004
category: adr
tags: [artist, rendering, assets, cpp, adr]
aliases: [ADR-0004]
status: Proposed
implementation: not-started
decision_origin: Correction proposée des maps par noms et valeurs effacées observées dans les contexts.
summary: Résoudre noms, formats et bindings à la préparation, puis exécuter sur IDs et layouts validés avec invalidation explicite.
created: 2026-10-03
updated: 2026-10-03
---

# ADR-0004 — Layouts typés et données préparées

## Contexte et faits

`ARTIST/include/graphic/context/PassContext.hpp` contient des maps par `string`.
`ARTIST/include/graphic/context/AttributeContext.hpp` utilise `std::any` et des
casts runtime. Les fonctions de configuration par nom ont une utilité, mais
elles ne doivent pas devenir une obligation répétée pour chaque donnée exécutée.

## Décision proposée

1. Garder les noms dans l'API de préparation/outillage et les messages d'erreur.
   Les résoudre vers des bindings/indices/offsets avant le chemin chaud migré.
2. Décrire les layouts par sémantique, format, taille, stride, alignement,
   offset, espace et fréquence de mise à jour. Les faits connus en C++ peuvent
   être vérifiés statiquement ; les faits provenant d'assets/reflection sont
   validés à la préparation. Pas de faux `static_assert` sur une information runtime.
3. Séparer une description immutable et les valeurs de frame. Employer des
   représentations typées ou des enregistrements de formats bornés, plutôt que
   récupérer le type d'une valeur avec `any_cast` à chaque utilisation.
4. Associer les bindings à leur programme/layout et version. Un relink, reload
   ou changement de layout invalide les bindings correspondants. Une location
   OpenGL n'est ni un ID d'asset stable ni une valeur cookée portable.
5. Distinguer ID logique, index dense, handle générationnel et objet natif.
   Un hash de chaîne seul n'est pas une preuve d'unicité : définir les collisions.

Pour un futur format cooké : schéma versionné indépendant des pointeurs/vtables
et du padding C++, tailles/offsets bornés, endianness/alignement, dépendances et
clé de cache. C'est un prolongement du contrat, pas l'annonce d'un cooker existant.

## Alternatives et conséquences

Les maps conservées dans la préparation sont simples et utiles. Les remplacer
partout par des nombres opaques ferait perdre du diagnostic sans prouver un gain.
Un template par valeur/material multiplie les instanciations. Le contrat proposé
concentre validation et conversion, mais nécessite gestion d'invalidation et
métadonnées de debug.

## Migration

Sur la tranche de [[adr/0002-preparation-et-execution-specialisee|ADR-0002]],
résoudre quelques uniforms/attributs représentatifs. Garder un adaptateur par nom
pour les consommateurs existants ; il produit une vue/binding valide plutôt que
réimposer les lookups au kernel. Étendre les formats après cette preuve.

## Critères de validation

Layout CPU/GPU compatible ; types/formats incompatibles rejetés au bon moment ;
noms absents et uniforms optimisés par le driver traités selon une politique
requise/facultative explicite ; binding périmé rejeté ou reconstruit ; bornes et
alignements validés. Pour la tranche migrée, mesurer les lookups/allocations
restants et vérifier l'image à paramètres identiques.

**Preuves actuelles** : les nouvelles représentations et le format cooké ne sont
pas implémentés ; leurs types exacts restent à préciser dans le travail borné.

## Références

[[adr/0001-resolution-multi-etapes|Frontières]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|Ownership]] ·
[[adr/0007-cpp23-et-cooker-independant|Cooker]] · [[adr/README|Registre]].
