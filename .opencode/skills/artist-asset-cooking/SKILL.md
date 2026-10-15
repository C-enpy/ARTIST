---
name: artist-asset-cooking
description: Définit les données importées, cookées et runtime d'ARTIST. À utiliser pour meshes, textures, matériaux, animation, particules, formats, cache, streaming et chargement GPU.
---

# Compiler le contenu sans en faire des types C++

Lire `docs/engineering-principles.md` et identifier d'abord le sous-système
réellement présent. Ne pas supposer un cooker ou un cache déjà implémenté.

## Contrat minimal

1. **Source** : format, provenance, unité/espace, version et dépendances externes.
2. **Import** : validation des tailles, indices, ranges, encodages et limites de
   ressources ; les données malformées doivent produire une erreur explicite.
3. **Cook** : transformations déterministes, profils cibles, optimisations offline,
   formats, layout et permutations bornées ; hashes et version des outils/options.
4. **Stockage** : schéma versionné, endianness, alignement, offsets vérifiés,
   intégrité et compatibilité. Ne pas sérialiser directement un objet C++ avec
   vtable, pointeurs ou padding indéterminé.
5. **Runtime** : ID logique → handle générationnel → résidence → upload →
   consommateur ; budgets mémoire, erreurs, fallback, annulation et libération.

## Points de vigilance

- Meshes : topology, indices, tangentes, handedness, unités et layout GPU.
- Textures : colorimétrie vs données, mips, compression cible et résidence.
- Matériaux : noms résolus hors frame, paramètres en données, vocabulaire borné
  de kernels/shaders. Distinguer bytecode portable et cache dépendant du driver.
- Animation : squelette/tracks cookés ; sampling/blending/skinning spécialisés,
  poses et temps en données. Tester conventions, interpolation et précision.
- Particules/cloth/hair : opérateurs/kernels bornés, buffers d'état dynamiques,
  scheduling et synchronisation explicites ; pas un template par instance.
- Streaming : invalidation de cache, hot reload, double-buffering éventuel,
  ressources encore utilisées par CPU/GPU, version et génération des handles.

Évaluer importeurs, compression et autres bibliothèques matures selon un contrat
borné. Leurs noms dans un rapport de recherche ne les rendent pas déjà intégrés.

## Validation

Petites fixtures connues, erreurs représentatives, déterminisme, changement de
version/options et invalidation, round-trip lorsque pertinent, puis upload et
consommation GPU réels si le contrat le demande. Mesurer cook/load séparément du
temps de frame. Les benchmarks et images doivent utiliser les mêmes données.

Retour : schéma et invariants, dépendances choisies/proposées, preuve, limites.
