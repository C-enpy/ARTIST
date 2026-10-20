---
title: "Ordre de mission — couche documentation d'usage"
category: architecture
tags: [artist, documentation, rendering, cpp]
aliases: [usage-doc-layer, mission-usage-doc]
summary: "Proposition sélectionnée par l'opérateur le 2026-10-20 : une couche documentation d'usage (getting-started, walkthroughs, snippets sourcés des tests) ajoutée à la mémoire d'ingénierie existante, sans la réécrire. Non démarrée."
status: proposed
implementation: not-started
source_revision: e43504a337cab6aadfc73e0eb836670db2d91bd1
created: 2026-10-20
updated: 2026-10-20
sources:
  - "Readme.md"
  - "docs/index.md"
  - "docs/development/build-and-test.md"
  - "docs/development/cpp-guidelines.md"
  - "docs/references/session-2026-10-20-pipeline-execution-path.md"
  - "ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp"
provenance:
  extracted: 0.7
  inferred: 0.3
  ambiguous: 0.0
---

# Ordre de mission — couche documentation d'usage

> [!important] Proposition documentaire, pas un démarrage
> Écrite le 2026-10-20 contre HEAD `e43504a`, à la demande de l'opérateur
> (audit d'audience du vault). Sélectionnée pour la **session suivante** ;
> le démarrage passera par `/ensemble-mission`, `config`, puis le `start`
> de l'utilisateur. Cette fiche ne lance aucune commande.

## Contexte — l'audit qui motive la mission

Le vault `docs/` (49 notes) est une **mémoire d'ingénierie** fraîche et
sourcée (états observés, ADR, fiches de mission, sessions, journal) : cette
fonction marche et reste inchangée. En revanche **aucune note n'explique
comment consommer ArtistLib** : zéro `#include <graphic/...>` dans le vault,
pas d'exemple minimal, pas de walkthrough ; les seuls exemples exécutables du
dépôt sont les tests. Les touchpoints face utilisateur ont dérivé : Readme
« Compilation » affiche **109/109** (état du 03/10, aujourd'hui 546/546),
`build-and-test.md` résume encore « F7 final : 299/299 » (deux missions de
retard), `cpp-guidelines.md` est à jour de frontmatter (03/10) et sa surface
citées s'arrête à `Pipeline.hpp`.

## Résultat attendu

Une couche **usage** ajoutée, distincte de la mémoire d'ingénierie :

1. **`docs/usage/getting-started.md`** — prérequis (Windows x64 Debug, MSVC
   19.44, CMake ≥ 3.25, Conan 2, GLFW/GLEW), commande de build canonique, et
   un **premier exemple minimal exécutable** : un `Pipeline<api::OpenGL,
   Classic>` à deux passes chargées, `use`/`useNext`/`reset`, readback
   `GL_CURRENT_PROGRAM` — code extrait des tests composant qui font foi, pas
   inventé.
2. **`docs/usage/pipeline-and-passes.md`** — walkthrough `IPipeline`/`IPass` :
   ownership `unique_ptr`, `forPass` par emprunt, curseur `c ∈ {-1} ∪ [0, N)`,
   rollback CPU de `use`, `reset` ≠ unbind (F10), les cinq canaux d'erreur
   réutilisés.
3. **`docs/usage/uniform-attribute.md`** — consommation des maps
   Uniform/Attribute possédées en `unique_ptr` : réflexion des noms, `set`
   single-value, rejets documentés (arrays en single-value, concept
   UniformReader, location/doublons).
4. **`docs/usage/closed-execution.md`** — quand utiliser
   `ClosedExecution<API>` contre `IPipeline`, en renvoyant au motif décrit
   dans [[architecture/closed-execution-pattern]].
5. **Refresh des touchpoints** — Readme : comptages au SHA du moment, section
   « Documentation d'usage » ; `build-and-test.md` : résumé à jour ; la
   mémoire d'ingénierie elle-même n'est **pas** réécrite.

Nom exact du répertoire (`docs/usage/`) et découpage précis des pages :
décision K ci-dessous, à valider par l'opérateur.

## Contraintes structurantes

- **Chaque snippet provient d'un test passant au SHA du moment** — fichier et
  tests d'origine cités à côté du bloc ; aucun code de démonstration inventé.
  Si un exemple doit diverger d'un test, le test est d'abord corrigé, puis
  l'exemple en est extrait.
- La mémoire d'ingénierie existante n'est **pas** réécrite ni réorganisée ;
  la couche usage s'ajoute à côté, avec sa propre provenance.
- La navigation : accueil du vault et Readme pointent la couche ; wikilinks
  réciproques ; reindex QMD à la clôture.
- Aucune promotion d'ADR, aucune revendication de performance, aucun
  changement de source moteur (`ARTIST/`).
- La documentation d'usage distingue explicitement **ce qui est testé** (avec
  SHA) de ce qui est seulement décrit — pas de promesse d'usage au-delà du
  périmètre Windows Debug.

## Preuve de réussite

- Les pages existent avec leurs provenances ; leurs snippets sont recoupés
  contre les tests du SHA courant (extraction 1:1 ou run de vérification) ;
  accueil + Readme naviguent vers la couche ; reindex QMD PASS ;
  découverte des wikilinks sans lien brisé (contrôle wiki-lint ciblé).

## K — décisions avant tout auteur

Léger, lead + décision humaine (pas de frontière moteur) :

1. **Structure** : `docs/usage/` avec les cinq livrables ci-dessus — accepter
   ou ajuster la table.
2. **Exemples retenus** : la chaîne Classic à deux passes de `PipelineTest`
   comme fil rouge des pages 1–2 et 4 ; la réflexion Uniform/Attribute
   canonique pour la page 3.
3. **Ton et langue** : français, style voisin du vault, snippets commentés en
   anglais comme le code (cohérence avec les conventions locales).
4. **Ce qui est explicitement hors mission** : API reference exhaustive,
   génération de site, traductions, changements de `Readme.md` au-delà des
   touchpoints listés, promotion d'ADR, tout run Release/ASan.

## Graphe préparatoire

Évaluation **PM/LLM locale du lead**, pas un passage JEV ; les scores ne
publient aucune tâche et n'imposent aucun modèle. Documentation pure : le
plancher `max(70, estimation)` ne s'applique pas aux notes (il vise le
développement exécutable) ; le risque porte sur la **dérive documentaire**
constatée, pas sur un comportement moteur.

| Ref | Rôle | Brute | Risque | Notes |
| --- | --- | --- | --- | --- |
| K | lead + humain : structure/table | 30 | 35 | Décision K ci-dessus, puis commit du choix. |
| U1 | rédaction pages 1–2 + refresh build-and-test (un seul écrivain) | 55 | 45 | Snippets extraits 1:1 des tests ; run de vérification si divergence. |
| U2 | rédaction pages 3–4 (parallèle U1, fichiers disjoints) | 50 | 45 | Idem, périmètre Uniform/Attribute. |
| V-D | contrôle : wikilinks, découverte, QMD reindex + embed (lead) | 40 | 40 | Contrôle wiki-lint ciblé, pas de QA de comportement. |
| R-D | revue fresh ciblée : snippets = tests ? (correctness-reviewer ou cpp-engineer RO) | 45 | 45 | Vérifie 1:1 et les citations d'origine. |

```text
K (décision humaine) → U1 || U2 → V-D (reindex) → R-D fresh → clôture
```

Un seul écrivain par fichier ; builds sérialisés (aucun run moteur n'est
requis si tous les snippets sont extraits 1:1 de tests passants du HEAD).

## QA de la proposition

Documentation d'usage : pas de comportement moteur sous contrat ; risque de
dérive déjà qualifié et borné par les contraintes. Produit impact ×
probabilité : impact 2 (note fausse = confusion, pas de crash), probabilité 3
(dérive démontrée), produit **6 < 10** → boucle QA non déclenchée ; contrôles
adaptés = cohérence/découverte/reindex (V-D).

## Sortie et suite

Budget indicatif : préparation 1 h, réalisation 4–8 h-personnes, réserve 2–4.
La réserve ne sert pas à élargir la table des pages.

Quand l'exécution sera voulue : `/ensemble-mission` avec cet objectif, puis
`/ensemble-mission config` et le `start` de l'utilisateur. Cette fiche ne
lance aucune de ces commandes.
