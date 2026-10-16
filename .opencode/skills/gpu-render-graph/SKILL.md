---
name: gpu-render-graph
description: Conçoit et vérifie DAG de passes, versions de ressources, scheduling, aliasing/transients, fusion et invalidation ; sépare graphe authoring, plan cook immuable et état runtime-frame. À utiliser pour shadow/post ou dépendances globales, pas une barrière isolée.
---

# Render graph : dépendances avant optimisation

## Déclencheurs et exclusions

Principal `render-graph-engineer` pour composition shadow/post, cycle, allocation
transient, scheduling async ou rebuild. Charger `artist-static-boundary` avec ce
skill. Pas pour une équation shader, un import de mesh, un layout API ou un hazard
local : transférer via lead au spécialiste, ne pas créer un second orchestrateur.
Une image incorrecte demande d'abord une pass isolée et ses entrées.

## Préconditions et outils

Exiger pass/ressources nommées, versions et plages lues/écrites, imports/exports,
effets externes, sorties attendues, capacités, queues et contrat de frame/resize.
Relever HEAD/delta/hashes et source/options/ABI. Lire le code : ARTIST a un backend
OpenGL ; aucune implémentation de render graph multi-API n'est affirmée ici.
ADR-0001/0002/0004 restent Proposed ; la méthode ne les adopte pas.

La validation de DAG/plan peut être CPU si ce contrat existe réellement. Le
backend et les images exigent leur contexte réel, après `artist-build-validation`.
Pas d'installation, SDK ou capture automatique ; outils API/trace conditionnels.
[Contrat détaillé et exemples](references/graph-contract.md) sur besoin seulement.

## Procédure

1. **Authoring.** Déclarer les passes flexibles, versions logiques, accès, effets
   externes et ressources importées. Pas de lecture implicite d'un futur producteur.
   Un historique temporel est un import de frame précédente, pas une boucle DAG.
2. **Validation.** Construire dépendances de données et anti-dépendances ; détecter
   cycle avec chemin témoin, read-before-produce, writer ambigu, plage/layout/format
   incompatible et import manquant. Définir les erreurs attendues avant code.
3. **Cook/préparation.** Résoudre seulement ce qui est connu : validation, indices,
   variantes bornées, lifetimes et plan. Plan cook immuable/versionné, distinct des
   allocations, objets driver, complétions et handles de la frame courante.
4. **Scheduling.** Ordre partiel et contraintes de queues explicites. Éliminer une
   pass uniquement si ses sorties ET effets sont inutiles. Async compute n'est
   utile que si dépendances et ressources autorisent un recouvrement mesurable.
5. **Transients/aliasing.** Calculer bornes de vie à partir du scheduling effectif,
   pas de la seule liste CPU. Prouver non-recouvrement happens-before, compatibilité
   des allocations, états/init et usages ; sinon allocations distinctes. La fusion
   de passes conserve load/store, clear, samples, resolves et effets observables.
6. **Traduction API.** Produire le ledger RAW/WAR/WAW et domaines. L'expert API
   traduit les dépendances, transitions et visibilité ; le graphe ne transforme pas
   toutes les arêtes en attentes CPU. Charger `gpu-resource-lifecycle` seulement
   lorsqu'un hazard/retrait concret doit être résolu.
7. **Runtime-frame.** Instancier imports, paramètres/caméra et ressources de frame,
   soumettre les lots puis enregistrer leurs complétions. Un dispatch par lot amortit
   la sélection d'implémentation/bindings homogènes ; il ne rend pas chaque draw
   statique et n'abolit pas la soumission driver. Pas de type par asset/instance.
8. **Invalidation.** Définir clés et déclencheurs de rebuild, conserver les plans
   encore en vol jusqu'au retrait. Tester resize, changement d'ABI/capacité et
   recompilation ; aucun cache vivant publié à moitié reconstruit.

## Nominal et négatif

- Nominal : shadow produit depth, opaque lit depth et produit HDR, post lit HDR et
  écrit la sortie importée. Oracle CPU : producteurs/ranges/dépendances exacts ;
  oracle GPU/image séparé : shadow/post attendus à scène/frame figées.
- Négatif : post lit HDR sans producteur ni import → read-before-produce, rejet
  avant allocation/soumission. Cycle shadow → opaque → shadow → chemin témoin.
- Négatif de scheduling : réutiliser une allocation alors qu'un lecteur compute
  non ordonné reste actif → aliasing refusé, même si l'ordre topologique semble bon.

## Pièges

Plan immuable ≠ graphe runtime ni device objects. Une liste de passes n'est pas
une preuve multi-queue ; une génération ne termine pas le GPU. Les ressources
importées, exportées, histories et callbacks ne sont pas des transients ordinaires.
Ne pas fusionner à l'aveugle ni ajouter un wait-idle pour « stabiliser » le graphe.
Pas d'optimisation qui supprime une sortie visible ou réduit la qualité sans contrat.

## Réussite et preuves

PASS borné au contrat exécuté (DAG/erreurs CPU, API, image ou mesure). INCONCLUSIVE
si graphe moteur absent, traduction non exercée ou contexte indisponible ; FAIL
si écart reproduit. Les diagrammes ne prouvent ni rendu ni gain. Conserver cas,
oracle, candidat, options/capacités, commandes/comptages/skips, plan avant/après et
ressources en vol. TDD pour l'exécutable, cohérence/liens pour la documentation.

## Références et transfert

[Graphes, cache et contre-exemples](references/graph-contract.md), rédaction
originale ; `artist-static-boundary` reste le contrat des frontières.
DEFER_TO au lead avec backend/HEAD/delta/hashes, chemins/ownership, binding times,
capacité/oracle/essais/inconnues : engine-architect pour décision durable, expert
API pour mapping, gpu-resource-engineer pour retrait, performance-reviewer pour
protocole A/B et rendering-verifier pour preuve indépendante. Aucun spawn.
