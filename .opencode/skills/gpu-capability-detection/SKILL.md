---
name: gpu-capability-detection
description: Établit capacités observées, supportées, activées et compatibles avec un workload sur OpenGL/Vulkan/D3D12/Metal ; vérifie limites, formats et fallbacks. À charger pour mesh shaders disponibles, jamais pour déduire une feature du seul nom RTX.
---

# Capacités : observer avant de sélectionner une variante

## Déclencheurs et exclusions

Principal `gpu-portability-engineer` pour feature optionnelle, limite, format,
profil/device inconnu ou fallback. Scénario mesh shaders : commencer par ce seul
skill et demander API/device, pas charger tout le catalogue. Pas pour optimiser
un shader, réparer un hazard ou supposer quatre backends déjà implémentés.
Un artefact cross-backend appelle `gpu-backend-portability` + `artist-gpu-debug`.

## Préconditions et outils

Relever OS/runtime/SDK séparément, API/version/profil, device choisi/driver et
extensions/features requises, modes effectivement activés, shader target/options,
dimensions/samples/formats/usages et limites du workload. Outils autorisés :
requêtes API et compilateurs/validateurs réellement découverts, jamais installés
par le skill. Avant exécution : `artist-build-validation`, bail et contexte réel.

Le backend présent ARTIST est OpenGL avec fixture 3.3 core. Une RTX observée
historiquement n'est pas la preuve d'une feature dans le contexte courant.
Lecture documentaire possible partout ; exécution conditionnée par plateforme :
Vulkan avec runtime/device, D3D12 Windows approprié, Metal hôte Apple/SDK/device,
OpenGL avec contexte/profil. Pas de test natif Metal déduit d'un MSL généré Windows.

## Procédure

1. **Définir l'exigence.** Feature exacte + usage (ex. mesh shader avec payload,
   workgroup et formats donnés), oracle nominal et qualité du fallback. Éviter
   « GPU moderne » ou version minimale seule comme spécification.
2. **Identifier l'appareil.** En multi-GPU, les propriétés du device sélectionné
   font foi. Conserver résultat brut/version de chaque requête, pas la disponibilité
   d'un outil ou le nom RTX comme conclusion.
3. **Séparer les états.** Observé = interrogation effectuée. Supporté = propriété
   device annoncée. Activé = chemin disponible et sélectionné dans la création/
   configuration applicable. Compatible = toutes exigences de ce workload satisfaites.
   Non observé = inconnu, jamais automatiquement false. Certaines API n'ont pas de
   bit d'activation globale : noter « non applicable » et la sélection effective.
4. **Vérifier le produit des contraintes.** Feature + extension/target + limites
   + format/usages/samples + bindings/ABI + queues disponibles. Une compilation
   réussie ne prouve pas ce produit, encore moins le résultat d'un pipeline.
5. **Choisir la variante.** Feature complète → chemin spécialisé ; absente ou
   incompatible → fallback explicite ou refus. Ne pas imposer le plus petit
   dénominateur commun. Borne les variantes, identifie clé de cache et invalidation
   sur device/driver/ABI/capacité ; pas de type C++ par GPU ou asset.
6. **Valider en niveaux séparés.** Compilation, offline, runtime, GPU, capture,
   profiling, image : statut et preuve propres. Un outil présent n'est pas un run.
   Consigner NOT_AVAILABLE/SKIPPED sans promouvoir le critère requis en PASS.

## Nominal et négatif

- Nominal conceptuel Vulkan : support query meshShader true, extension/feature
  activées, limites et formats compatibles, entry/target valides → variante éligible,
  puis test device distinct. Pas de PASS rendu à la seule éligibilité.
- Négatif : « RTX » sans API/device interrogé → demander les préconditions,
  capacité inconnue. D3D12 MeshShaderTier non supporté → fallback vertex/index
  compatible ou refus explicite, jamais l'appel de DispatchMesh supposé disponible.
- Négatif : support true mais limite workgroup dépassée, format non supporté pour
  storage ou extension non activée → incompatible, même si le shader compile.

## Pièges

Meshlets ≠ mesh shaders, ray tracing ≠ mesh shading, runtime ≠ GPU, header ≠
fonction disponible. Une version Metal/SDK ou D3D feature level ne remplace pas
les requêtes par fonctionnalité. Ne pas tester un flag disponible dans un header
sans vérifier d'abord que cette interrogation existe sur la version cible.

## Réussite et preuves

PASS pour le relevé ou test effectivement exécuté et son périmètre ; FAIL si
exigence contredite ; INCONCLUSIVE si information requise inconnue ou plateforme
absente. Résultat : matrice conceptuelle ou relevé horodaté explicitement nommé,
device/driver/API, requêtes brutes, source/options, candidat HEAD/delta/hashes,
variant/fallback, commandes et nombres/skips. Aucun gain CPU/GPU inféré.

## Références et transfert

[Matrice conceptuelle des quatre API et sources](references/api-capabilities.md)
à ouvrir pour l'API visée seulement. Aucun fichier partagé platform-capabilities
ne découle de ce skill. Rédaction originale, sans nouvelle adoption ADR.
DEFER_TO au lead : expert API si interrogation/mapping ambigu, shader-engineer
pour target/ABI, gpu-pipeline-engineer pour pipeline, rendering-verifier pour
device/image, performance-reviewer pour mesures. Joindre fichiers/ownership,
binding times, capacités/oracle/essais/inconnues et bail ; aucun spawn.
