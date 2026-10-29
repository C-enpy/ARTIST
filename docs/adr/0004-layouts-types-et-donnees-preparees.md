---
title: "ADR-0004 — Layouts typés et données préparées"
adr_id: ADR-0004
category: adr
tags: [artist, rendering, assets, cpp, adr]
aliases: [ADR-0004]
status: Proposed
implementation: partial
decision_origin: Correction proposée des maps par noms et valeurs effacées ; seule la tranche float OpenGL a été adoptée explicitement par l'utilisateur le 2026-10-27.
summary: "Proposed globalement ; première tranche float scalaire préparé livrée, emprunts révocables et consommateur pixel vérifiés Windows Debug."
created: 2026-10-03
updated: 2026-10-29
audit_date: 2026-10-29
sources: [docs/_meta/session-2026-10-27-prepared-float.json, docs/_meta/session-2026-10-29-linux-qa.json, research/sessions/2026-10-29-linux-qa-session/index.json, .opencode/evidence/adr-audit-2026-10-29/inputs.json, .opencode/evidence/adr-audit-2026-10-29/public-deliveries.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# ADR-0004 — Layouts typés et données préparées

## Audit au 29 octobre 2026

**Proposed / partial** confirmé : seule la tranche float est adoptée. Ses types,
métadonnées, révocations et pixels sont exercés ; les layouts généralisés et
bindings Attribute préparés ne sont pas livrés. Le critère de mesure des
lookups/allocations n'est pas entièrement prouvé : l'absence de lookup/any_cast
d'update est observée dans la source, mais pas de mesure d'allocation acquise.
Un format cooké futur et un gestionnaire global d'IDs absents ne sont pas des
obligations ajoutées au produit actuel.
[[references/adr-audit-2026-10-29#ADR-0004|Chaque obligation et critère audités]].

## Non-régression Linux et classification — 29 octobre 2026

Statut **Proposed/partial inchangé**. Les campagnes de [[development/linux-qa]]
exercent la tranche existante dans les 589 tests Windows, WSL et CI normal/ASan.
La correction Attribute conserve les clés natives legacy et n'utilise un nom
moderne que temporairement pour classifier scalaire/array1. Route conditionnée
par capacité effectivement annoncée et fonctions disponibles ; incohérence rejetée
avant publication du staging. Pas de binding Attribute préparé, résolution cookée,
rollback CPU supplémentaire ou gain mesuré ajouté par ce correctif.

Voir [[references/session-2026-10-29-linux-qa|décision de compatibilité et revue GLEW]].
Le fallback 3.3 ambigu reste non universellement qualifié ;
les nombres et limites de la tranche Windows du 27 octobre ci-dessous gardent
leur date et ne sont pas réécrits en nouvelle campagne.

## Tranche adoptée et exécutée — 2026-10-27

Le statut global reste **Proposed**, l'implémentation devient **partial**. Seule
la tranche `PreparedFloatUniform` OpenGL est adoptée ; aucun format cooké, binding
d'attribute, gestionnaire global de handles ou suppression générale de `std::any`.

Source finale `3b1b1028dcd64ca46e0a359365ae096be1a38ac4` :
`graphic/opengl/pipeline/PreparedFloatUniform.hpp` sous `ARTIST/include/` expose
`prepareFloatUniform(pass, name)` puis `set(float)`. Nom et métadonnées de réflexion
sont résolus **runtime après link**, pas au cook. Seul `GL_FLOAT` strictement
scalaire est admis ; arrays de taille 1 compris refusés, nom absent/optimisé requis
introuvable, métadonnées manuelles ou altérées rejetées.

Les owners restent uniques ; l'accès emprunte avec témoins faibles locaux et
générations vérifiés avant pointees. Maps, métadonnées, retrait, destruction et
nouvelle incarnation révoquent l'ancien accès. Les moves suivent le contexte
transféré ; pas de copie implicite d'identité. Contrat mono-thread, sérialisé,
non réentrant, fenêtre vivante, sans relink/delete natif externe aux owners.

`set` valide contexte/programme, conserve le miroir CPU `std::any` historique,
puis transmet directement le float à `glUniform1f`, sans lookup par nom,
résolution native ou `any_cast` dans les updates. L'API par nom est conservée,
avec ses propres compatibilités. Aucune promesse de zéro allocation ou gain mesuré.

Consommateur : `ReflectedProgramGuardCrossTests.cpp`, target préparé une fois,
decoy par nom. `PreparedFloatUniformRenderTests.cpp` vérifie un triangle dans
un FBO RGBA8 linéaire 32×32 : attendus fixes non vides pour chaque route, égalité
des buffers et changement de valeur discriminant. Omission d'update et rendu
tout noir détectés séparément ; mutants C/L/A détectés puis sources restaurées.

Runner lead : **581/581 normaux et 581/581 ASan**, chacun 506 CPU + 75 OpenGL,
mêmes noms, zéro failure/error/skip, canari 0/86 ; clean-first intégral des deux
configurations. Revue C++ indépendante sans défaut bloquant démontré ; audit
fresh des tests ayant demandé deux compléments, puis **ré-audit PASS borné**, F1/F2
fermés, aucun finding bloquant maintenu. Historique RED toujours INCONCLUSIVE.
Les nouveaux tests détectent la préparation sans contexte autorisé et la révocation
tardive après détachement partiel ; leurs mutants séparés échouent avant restauration.
Preuve portable :
`_meta/session-2026-10-27-prepared-float.json` ; [[usage/uniform-attribute|usage]],
[[development/build-and-test|commandes]] et [[development/known-incidents|limites]].
Certains tests lifetime ont été ajoutés après les gardes et l'empreinte source RED
initiale manque : aucune preuve historique reconstituée. Fuites, Release/Linux,
autres API et performances non validés ; aucune retraite GPU générale démontrée.

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

**Portée restante** : hors de la tranche float ci-dessus, les layouts généralisés
et le format cooké ne sont pas implémentés ; leurs types exacts restent à préciser.

## Références

[[adr/0001-resolution-multi-etapes|Frontières]] ·
[[adr/0003-ownership-et-lifetime-cpu-gpu|Ownership]] ·
[[adr/0007-cpp23-et-cooker-independant|Cooker]] · [[adr/README|Registre]].
