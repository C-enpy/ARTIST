# Textures : DDS, compression et sous-ressources

## Préconditions et procédure

DDS est un conteneur, pas une sélection D3D12. Demander dimensions/type (2D,
volume, array, cube), mip count, format source/cible, alpha, orientation et usage.
BC et ASTC sont des familles de formats à blocs, pas des formats interchangeables.
ASTC n'est pas supposé représentable dans n'importe quel DDS/DXGI : refuser le
couple conteneur/format inconnu, ou utiliser une conversion explicite documentée.

1. Lire les entiers DDS little-endian sans cast de struct ; vérifier magic,
   `DDS_HEADER` de 124 octets, pixel-format de 32 octets, flags/caps cohérents.
   En-tête total historique 128 octets (magic inclus) ; extension DX10 de 20
   octets → début payload 148. Vérifier sa présence AVANT lecture. Le FourCC
   seul ne donne pas toute la sémantique ; valider format DXGI, resourceDimension,
   arraySize, cubes/faces et depth. Troncature, dimensions nulles ou combinaison
   invalide → erreur avant allocation. Définir la politique du mip count legacy
   absent ; vérifier la borne `1 + floor(log2(max(width,height,depth)))`.
2. Recalculer chaque mip avec dimensions `max(1, dimension >> niveau)` dans
   une arithmétique bornée ; limiter niveaux, faces/layers et bytes cumulés.
   Pour BC : blocs 4×4, 8 octets (BC1/BC4) ou 16 (autres BC usuels) ; pour ASTC
   2D : footprint explicite et 16 octets/bloc. Ne pas généraliser à ASTC 3D
   sans profil défini. Exemple : `blocksX = ceil(width / blockWidth)` et
   `blocksY = ceil(height / blockHeight)` avec minimum un bloc par mip.
3. `rowPitch = blocksX * blockBytes`, `slicePitch = rowPitch * blocksY` pour
   un payload compressé compact ; dimensions non multiples exigent des blocs
   complets. Produit depth/layers/faces et somme des mips checked. Pour un
   format non compressé, utiliser ses règles bits/pixel/packing/planes ; ces
   formules BC ne s'appliquent pas à YUV/planar ni à tous les formats legacy.
4. Construire la table des sous-ressources dans l'ordre du conteneur (array,
   face, mip, profondeur selon type). Vérifier offset et longueur contre taille
   restante, overflow et overlaps. Ne pas faire confiance à pitch/linearSize
   annoncé sans le confronter au format. Le rowPitch du fichier n'est pas celui
   de staging GPU : alignement requis par l'API/device traité par les ressources.
5. Déclarer sRGB vs linéaire vs données : filtrer les couleurs en linéaire puis
   réencoder ; normales/roughness/masques ne deviennent pas sRGB. Mips de normales
   renormalisés avec convention de signe Y ; règles spécifiques pour alpha
   coverage/premultiplied, roughness et HDR. Pas de gamma appliqué deux fois.
6. Cuisiner mips/compression/cibles offline à qualité mesurée ; paramètres codec,
   schéma, sRGB/alpha/orientation et dépendances entrent dans la clé de cache.
   Un format non supporté au runtime exige fallback préparé/transcode explicite
   ou refus, jamais réinterprétation silencieuse en RGBA. Vérifier capacités.

## Fixtures et oracle (proposés, pas décodage exécuté)

- **D-01 nominal** : DDS legacy BC1, 5×3, trois mips (5×3, 2×1, 1×1) :
  payloads 16 + 8 + 8 = 32 octets ; rowPitch 16/8/8 et slicePitch 16/8/8 ;
  offsets 128/144/152, fichier complet 160 octets. Header valide et faces/layers
  cohérents requis en plus de cette arithmétique. Une fixture DX10 analogue
  commence à 148 et nécessite sa propre taille totale, pas 160.
- **D-02 négatif** : le même fichier à 159 octets est tronqué ; refuser le dernier
  mip avant lecture/upload. Tester aussi header DX10 incomplet, offset débordant,
  cube incomplet et produit arraySize énorme avant toute allocation.
- **D-03 négatif** : format non supporté ou feature absente → refus/fallback
  explicite. Le suffixe DDS ne prouve ni BC supporté ni backend natif disponible.
- Round-trip sans perte : metadata et bytes de blocs conservés dans le schéma
  canonique ; réencoder avec codec avec perte requiert une borne d'erreur/image
  fixée avant test. Deux cooks déterministes ont mêmes hashes dans leur enveloppe.
  Changer codec/options/mips/colorimétrie/ABI doit invalider le cache.

CPU offline vérifie conteneur/tailles, pas sampler/image GPU. Mesurer I/O,
validation, décompression/transcode, staging et pic mémoire séparément ; stocker
le produit prêt à charger déplace le coût au cook sans le rendre nul.
`DEFER_TO` lead → `gpu-resource-engineer` avec table validée pour l'upload,
`gpu-portability-engineer` pour formats/capacités et stratégie de fallback.

## Bibliothèques et provenance

Rédaction originale ; liens officiels seulement, aucun code adapté ou copié.
- [DDS guide Microsoft](https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide) : conteneur et règles de lecture.
- [DirectXTex](https://github.com/microsoft/DirectXTex) : candidat mature pour
  DDS/mips/compression ; vérifier formats exacts, plateforme, colorimétrie et
  déterminisme. L'audit T02 cite MIT, pas une dépendance/version sélectionnée.
- [astc-encoder](https://github.com/ARM-software/astc-encoder) : candidat codec
  offline ASTC ; comparer qualité, temps de cook, profils HDR/LDR et plateformes.

Résoudre commit exact et notices/licence par fichier avant intégration ; arbre
SHA ≠ commit, licence inconnue = lien seulement. Aucun SDK/install au chargement.
