# Géométrie : préparation et qualité

## Préconditions et procédure

Source/IR/profil cible identifiés ; vérifier outils et consommateur avant de
choisir un layout. Fixer unités, repère, winding, espace objet et convention UV.
Importer topologie explicite ; trianguler seulement avec règle et oracle définis.

1. Rejeter position non finie, index hors bornes (`index < vertexCount`), stride
   insuffisant, nombre d'indices incompatible avec la topologie et compteurs
   dépassant le budget. Distinguer dégénérescence tolérée/documentée et erreur.
   Bounds (AABB/sphère) doivent envelopper les positions décodées, après conversion.
2. Normales unitaires, tangentes orthogonalisées, `tangent.w` signe de handedness :
   convention déclarée `B = tangent.w * cross(N, T)`. Distinguer signe UV miroir
   et changement de repère ; scinder les sommets aux coutures UV/normales si requis.
   Une échelle non uniforme demande inverse-transposée pour N et réorthogonalisation
   de T ; déterminant négatif oblige à traiter winding et signe de base tangentielle.
   Décider tangentes importées ou générées, algorithme/version et UV utilisés.
3. Dédupliquer sur tous les attributs pertinents, pas position seule. Toute
   permutation remappe indices, skin weights/joints, morphs et métadonnées ; garder
   les sous-meshes/matériaux et seams. Définir u16/u32, limite du maximum index et
   éventuel index réservé primitive-restart ; ne pas choisir u16 par vertexCount seul.
4. Optimiser offline cache post-transform puis accès vertex-fetch, et overdraw
   seulement si ordre/alpha/sémantique le permettent. Mesurer coût/qualité sur un
   workload réel ; un modèle de cache n'est pas un gain GPU prouvé.
5. Layout AoS/SoA, alignement, formats normalisés, quantification et décodage sont
   un contrat consommateur. Borner erreur position/angle et bounds après décodage ;
   préparer offsets/strides une fois, pas reconstruire les bindings par sommet.
6. Meshlets : limites de sommets/primitives selon profil/capacités, indices locaux
   valides, table de remap, bounds et cônes conservatifs. Les mesh shaders ne sont
   pas garantis par des meshlets : fournir chemin indexé classique ou refuser le
   profil non supporté. Les capacités relèvent de `gpu-portability-engineer`.
7. LOD : métrique d'erreur et écran/résolution/distance déclarées, silhouettes,
   seams, matériaux, skin/morph préservés. Vérifier seuils et transitions ; ne pas
   comparer performances en diminuant silencieusement la qualité ou les triangles.

## Nominal, négatif et oracle

- Nominal : triangle indexé puis quad UV miroir à deux matériaux ; positions,
  indices et tangentes attendus, bounds conservatifs. Transformation de repère
  et aller-retour sans perte conservent la surface orientée ; version quantifiée
  respecte les erreurs publiées. Deux cooks mêmes options donnent mêmes bytes.
- Négatif : index égal à vertexCount, UV dégénérés, NaN, stride/offset invalides,
  permutation ne remappant pas le skin, meshlet au-delà d'une limite, LOD dépassant
  l'erreur : rejet ou fallback précontracté, jamais mémoire lue hors tableau.
- Invalidation : UV/tangentes, codec, version, seuil LOD ou profil modifié → clé
  différente. Changement de nom logique sans changement de contenu suit la
  politique d'identité, pas une recuisson aveugle non définie.

Coûts séparés : construction/remap/simplification au cook ; lecture/validation,
décompression et transfert au chargement ; culling/skinning au runtime. Relever
octets source/cook/runtime, pic mémoire, durée et qualité, sans gain présumé.
Upload/consommation GPU requièrent leurs preuves ; CPU offline seul ne les prouve pas.

## Bibliothèques et provenance

Rédaction originale ; liens officiels, aucun extrait ou code tiers adapté.
- [meshoptimizer](https://github.com/zeux/meshoptimizer) : candidat pour cache,
  fetch, simplification, meshlets et codecs ; évaluer qualité/limites et coût
  de décodage. Pas importeur universel ni dépendance ARTIST sélectionnée.
- [DirectXMesh](https://github.com/microsoft/DirectXMesh) : candidat pour nettoyage,
  adjacency, normales/tangentes et meshlets ; évaluer plateforme/conventions et
  intégration offline. Le nom DirectX ne choisit pas un backend runtime.
- [MikkTSpace](https://github.com/mmikk/MikkTSpace) : candidat si le pipeline
  d'authoring exige sa convention de tangentes ; ne pas remplacer au jugé par
  un algorithme « équivalent » sans comparaison sur seams et normal maps.

Avant sélection/copie : résoudre commit exact, chemins/notices/licence et
dépendances de la version retenue (tree SHA ≠ commit), documenter disposition.
T02 a audité DirectXMesh comme candidat MIT ; cela n'adopte pas une version.
Licence inconnue = lien seulement. Aucun install/clone automatique.
Retour au lead avec contrat, fixtures, décision de bibliothèque et limites ;
`DEFER_TO` ressources pour chargement, shader pour décodage, portabilité pour features.
