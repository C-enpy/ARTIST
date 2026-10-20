---
title: Pipeline et passes
category: usage
tags: [artist, guide, cpp, ownership]
aliases: [pipeline-and-passes]
summary: "IPipeline/IPass : ownership, emprunts, curseur, rollback CPU et erreurs."
status: source-checked
source_revision: 1005dff0804e18c412129843e944d7f1f55a0f48
created: 2026-10-20
updated: 2026-10-20
sources: [ARTIST/include/graphic/pipeline/Pipeline.hpp, ARTIST/include/graphic/pipeline/Pass.hpp, ARTIST/include/graphic/context/PipelineContext.hpp, ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp]
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Pipeline et passes

[[index|Accueil]] · [[usage/getting-started|Premiers pas et snippets sources]] ·
[[usage/uniform-attribute|Uniform et Attribute]] · [[usage/closed-execution|Exécution fermée]].

## Construire des propriétaires, puis emprunter

`Pipeline<api::OpenGL, profile::Pipeline::Classic>` implémente `IPipeline`.
Le constructeur consomme un `vector<unique_ptr<IPass<API>>>` et possède
son `PipelineContext` par `unique_ptr`. Le contexte possède le registre des
passes ; chaque passe possède son contexte et ses shaders. Copie interdite,
déplacement transférant la propriété : voir les fabriques à deux passes dans
[[usage/getting-started]].

`forPass(i)` retourne un **`IPass&` emprunté**, pas un nouvel owner.
La croissance du registre ne déplace pas les objets pointés ; déplacer le
pipeline conserve l'adresse du contexte et des passes transférées.
La destruction ou le remplacement de l'owner invalide les emprunts concernés ;
l'assignation move détruit d'abord les anciennes passes de destination.
`getContext()` est également un emprunt, même depuis un pipeline const :
aucune prolongation de lifetime, aucune garantie d'accès concurrent sûr.

Sérialiser les accès côté appelant. Garder le contexte OpenGL requis vivant
et courant pour opérations et destructions. Un handle copié pour observation
n'est pas un owner. Scope C++ et frame de soumission ne démontrent pas
la fin d'utilisation GPU ; aucun protocole général de retraite GPU n'est promis.

## Le curseur et l'itération

Le registre impose `N ≤ INT_MAX` et `c ∈ {-1} ∪ [0, N)`.
Initialement `c = -1` ; `use(-1)` reste invalide : cette sentinelle sert au
reset, pas à sélectionner une passe. `hasNext()` signifie `c < N - 1`.

- `use(i)` valide l'index, sélectionne `i`, puis appelle le backend.
- Si le backend lève, `use` restaure le **curseur CPU précédent** et relance
  l'exception. Aucun rollback des effets GPU déjà produits n'est promis.
- `useNext()` exécute la suivante et retourne **s'il en reste après celle-ci**.
  La dernière passe est donc exécutée avec un retour `false`.
- Un appel supplémentaire sans suivante appelle `reset()` et retourne
  `false` ; un pipeline vide suit aussi cette branche.

Les readbacks `PipelineTest.UsePassTest` et `PipelineTest.UseNextPassTest`
de `ProgramTests.cpp`, reproduits dans [[usage/getting-started]], distinguent
le curseur du programme lié.

## Reset n'est pas unbind (F10)

Classic remet le curseur à −1 sans détruire la passe ni appeler
`glUseProgram(0)`. `PipelineTest.ResetPipelineTest` vérifie `glIsProgram`
et `GL_CURRENT_PROGRAM` ; extrait dans [[usage/getting-started]]. Les tests
délient explicitement avant destruction. Séparément, utiliser une passe
OpenGL au handle zéro conserve l'unbind `glUseProgram(0)` (F10) : ce n'est
pas le comportement de `reset`.

## Les cinq canaux d'erreur du pipeline

Diagnostics existants, pas nouveau registre ; les exceptions sont des
`TraceableException<T>` avec la base standard suivante :

- `ERROR::PIPELINE::NULL_CONTEXT` : `invalid_argument`, contexte constructeur nul.
- `ERROR::PIPELINE::NULL_PASS` : `invalid_argument`, ajout d'une passe nulle.
- `ERROR::PIPELINE::PASS_COUNT_LIMIT_EXCEEDED` : `length_error`, ajout au-delà
  de la limite représentable du curseur.
- `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` : `out_of_range`, index négatif
  ou hors registre (`forPass`, sélection ou mutation du curseur).
- `ERROR::PIPELINE::FREED_CONTEXT` : `runtime_error`, accès après transfert
  du contexte depuis un owner déplacé.

Les erreurs des passes se propagent aussi, sans conversion en `false`.
Un ajout rejeté conserve les entrées existantes, leurs adresses et le curseur,
mais son argument `unique_ptr` a déjà été consommé. Un échec de construction
détruit les owners déjà acquis : pas de restitution à l'appelant.

## Preuves et limites

Contrats **décrits par lecture du code** au `source_revision` ; snippets liés
extraits 1:1 des tests présents à cette révision. Dernière suite tracée :
**546/546** (477 unitaires + 69 OpenGL), Windows Debug, au SHA
`fdbce15dcc05663cc58618a844d931b07b84750f`, sans relance ici.
Compilation et mocks seuls ne prouvent ni rendu ni absence d'UB.
Voir [[development/build-and-test]], [[development/known-incidents]] et
[[usage/closed-execution]] pour la chaîne empruntée sans déplacement du curseur.
