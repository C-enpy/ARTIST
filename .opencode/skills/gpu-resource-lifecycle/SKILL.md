---
name: gpu-resource-lifecycle
description: Trace ownership, borrow, générations, allocations, upload/readback, résidence et retrait des ressources GPU ; analyse RAW/WAR/WAW, ordre, visibilité et synchronisation CPU/GPU avant de choisir une barrière API.
---

# Ressources et synchronisation jusqu'à la dernière utilisation

## Déclencheurs et exclusions

Utiliser pour une ressource périmée, aliasing illégal, ring buffer écrasé, fuite,
readback incohérent, descripteur réutilisé ou resource barrier. Principal :
`gpu-resource-engineer`. Pas pour concevoir tout un DAG, cuire une texture,
compiler un shader ou optimiser sans baseline. Un souci CPU seul relève de
`artist-cpp-contracts` ; un scheduling global de `gpu-render-graph`.

## Préconditions et outils

Obtenir API/version/profil/extensions activées, OS/GPU/driver, contexte courant
et thread, frame/seed, commandes et ressource/sous-plage. Relever tailles, formats,
usages, limites d'alignement CPU/GPU, placements et chemin shader. Le backend
ARTIST présent est OpenGL, tests 3.3 core ; compute/SSBO/mapping persistant ne
sont pas garantis. Expertise documentaire des autres API ≠ implémentation.

Lire [ledger et traduction](references/ledger-and-hazards.md) seulement pour le
cas concerné ; charger ensuite le skill API correspondant, pas tous les quatre.
`artist-build-validation` avant exécution. Aucun SDK/driver/capture installé
implicitement ; debug output/layers, capture ou profiler seulement si réellement
disponibles et sous bail. Réflexion/compilation offline n'est pas un test GPU.

## Procédure

1. **Définir le contrat.** Décrire owner exclusif ou partage explicitement justifié,
   borrow non propriétaire, invalidation, erreur observable et attendu nominal.
   Lire le storage réel ; le pin PipelineContext utilise `unique_ptr` et emprunt.
2. **Construire le ledger.** Identité + génération CPU ; allocation, offset, taille,
   sous-ressource, format/stride ; usage producteur/consommateur, stage/domaine,
   queue/contexte, état/layout ; dernière utilisation et complétion par queue.
   Inclure mémoire staging, descripteurs, vues et objets de commande dépendants.
3. **Suivre la chaîne.** Création → upload → binding → draw/dispatch → consommation
   → retrait → destruction/réutilisation. L'upload peut finir avant la dernière
   lecture de la destination. Le propriétaire CPU ne suffit pas à prouver la fin GPU.
4. **Établir le hazard.** Pour chaque plage chevauchante, identifier RAW/WAR/WAW.
   Justifier séparément ordre d'exécution, disponibilité/visibilité mémoire et
   attente CPU ; pas de barrière universelle. Pour un WAR pur, ordonner le dernier
   lecteur avant l'écrasement ; ne pas inventer de valeur à rendre visible.
5. **Traduire au backend.** Définir domaine source/destination et portée exacte,
   ownership de queue si pertinent, flush/invalidate host si requis. La barrière
   ne répare ni un mauvais binding/layout CPU/GPU, ni un emprunt pendant.
6. **Retirer.** Invalider les nouveaux emprunts puis attendre la complétion de
   toutes les dernières utilisations GPU avant réemploi physique. Conserver la
   ressource en file de retrait, avec le contexte/device correct pour la destruction.
   Une génération changée n'est pas une fence. Traiter échec d'upload et device loss.
7. **Prouver.** Fixer oracle avant patch, exécuter RED causal puis GREEN minimal
   si du code est modifié ; revoir le diff final. Mesurer séparément CPU submit,
   GPU et attente. Un wait-idle provisoire peut discriminer, pas devenir le correctif.

## Nominal et négatif

- Nominal : copy produit un buffer, graphics le consomme ; rendre visible le bon
  intervalle et retirer staging après copy, destination après graphics. Oracle :
  données vérifiées et aucun réemploi précoce à la frontière testée.
- Négatif : génération CPU renouvelée mais graphics encore en vol ; refuser le
  réemploi. Autre négatif : borrow ancienne génération, ou retrait depuis un thread
  sans contexte OpenGL valide → erreur définie/route de retrait, jamais appel aveugle.
- D3D12 barrier : ledger puis `d3d12-backend` ; un mauvais state et une queue qui
  n'attend pas sont deux hypothèses différentes, à tester séparément.

## Pièges

RAII CPU ≠ complétion GPU ; cohérence ≠ ordre ; une fence ≠ visibilité de tout
accès shader ; version d'objet ≠ synchronisation ; maxima de compteurs de queues
indépendantes incomparables. Ne pas transposer resource states D3D12 à OpenGL.
Aliasing exige plus que des durées de vie CPU disjointes. Ne pas vider toutes
les queues pour masquer un usage/format/stride incorrect.

## Réussite et preuves

PASS seulement pour l'oracle exécuté : contrat CPU, validation API, données/image
et coût sont distincts. FAIL si écart reproduit ; INCONCLUSIVE si contexte/outils
ou dernières utilisations inconnus. Zéro test ou skip requis ne valide rien.
Conserver candidat HEAD/delta/hashes, ledger, commande/versions, comptages et logs
pertinents ; aucun gain de performance déduit de cette recette documentaire.

## Références et transfert

[Ledger, exemples et sources officielles](references/ledger-and-hazards.md).
Rédaction originale ; aucune nouvelle adoption des ADR-0001/0002/0004.
DEFER_TO au lead : expert API pour traduction, render-graph-engineer pour dépendance
globale, correctness-reviewer pour lifetime, rendering-verifier pour preuve GPU
indépendante. Transmettre backend/capacités, fichiers possédés, binding times,
oracle/essais/inconnues et bail requis. Aucune sous-délégation depuis ce skill.
