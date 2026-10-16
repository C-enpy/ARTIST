# Matrice conceptuelle des conventions et oracles visuels

Rédaction originale, 2026-10-16. Pas de matrice exécutée quatre API ; pas de RHI
ARTIST implémenté par cette référence. Lire seulement les axes du cas rencontré.

## Clip, profondeur et orientation

- **OpenGL** par défaut : clip Z dans `[-w,+w]`, NDC Z dans [-1,+1], conversion
  par depth range ensuite. `glClipControl` peut changer origine et convention de
  profondeur avec version/extension adaptées (core 4.5), pas garanti par 3.3.
- **Vulkan** par défaut : clip Z dans `[0,w]`, NDC Z dans [0,1]. Des extensions
  peuvent modifier la convention ; ne les utiliser que si supportées/activées.
  Les réglages de viewport (dont signe de hauteur si permis) changent l'orientation
  effective ; écrire le mapping image/winding au lieu de compter les flips Y.
- **D3D12** : clip Z `[0,w]`, NDC Z [0,1], viewport/framebuffer selon sa convention.
  Depth clipping configurable dans le rasterizer ; ne pas déduire l'espace monde
  de ce range. Documenter front-face et application de la transformation Y.
- **Metal** : clip Z `[0,w]`, NDC Z [0,1] ; convention viewport/image à raccorder
  explicitement à la projection et aux textures, pas via une copie du shader GL.

Handedness du monde/vue, stockage row-major/column-major et ordre de multiplication
relèvent du contrat mathématique et de l'ABI, pas d'une obligation absolue de l'API.
Pour la comparaison, consigner **handedness**, winding et front-face après toutes
les transformations. Vérifier normales (inverse-transposée si nécessaire), unités,
jitter et reversed-Z : projection, clear, compare, format et résolution associés.
Un changement de range n'autorise pas la perte silencieuse de précision depth.

## Origines et matrices : expérience minimale

Scène : une seule pass, triangle non symétrique avec trois couleurs distinctes,
near/mid/far connus, culling contrôlé, texture 2×2 à quadrants nommés ; frame 0,
caméra fixe, pas de temporal jitter. Fournir positions CPU, matrices finales,
ordre de multiplication, éléments transmis au shader, viewport et front-face.

Oracle CPU : coordonnées clip/NDC calculées pour les trois sommets, clipping et
orientation attendus, sans tolérance choisie après observation. Oracle GPU/image :
triangle présent au bon endroit, quadrant correct, depth vérifiée en cible
appropriée/readback. Ces oracles proposés restent à exécuter sur chaque backend.

Négatifs : flip Y double ; projection Z [0,1] reconvertie comme [-1,1] ; transpose
appliquée deux fois ; culling inversé après changement viewport ; CPU stride ou
offset omettant un padding. L'image en sortie de pass doit discriminer lequel
échoue, pas seulement « frame différente ». Un rapport « shader compilé » ne suffit pas.

## Formats, ABI et couleur

- Inventorier format logique et format physique/view, type numérique, normalisation,
  compression/blocs, aspect depth/stencil, dimensions/mips/couches/samples, usages
  et restrictions read/write/filter/blend. `RGBA8` seul est un contrat incomplet.
- Packing CPU/GPU : stride, offset, alignement, matrices/vecteurs/arrays, booléens
  et enums ; utiliser la réflexion réelle du target/layout. Ne pas supposer std140
  identique aux constant buffers HLSL ou aux structs MSL. Bornes et binding indices
  restent valides après spécialisation/traduction.
- Couleur : texture albedo sRGB décodée une fois vers linéaire, textures de données
  sans conversion gamma, illumination/blending dans le domaine défini, sortie
  encodée une fois selon attachment/surface. OpenGL exige notamment état et cible
  compatibles pour la conversion framebuffer ; ne pas copier son état aux autres API.
- Distinguer framebuffer linéaire/HDR, image tonemappée, présentation et fichier
  exporté. Exposure et tonemapping figés avant comparaison. Un fichier screenshot
  et un readback float ne portent pas nécessairement le même domaine colorimétrique.

Expérience : patch gris linéaire connu, rampe et normales marquées ; lire la cible
avant tonemapping, puis sortie finale encodée. Négatif : double sRGB encode ou
normal map décodée comme couleur ; mesure dans le domaine annoncé. Métrique et
tolérance justifiées avant code, pixels/edges sensibles explicités ; ne pas
faire passer une régression en changeant les golden images ou la tolérance.

## Écarts représentables et stratégie de fallback

Une abstraction exprime l'intention et les exigences, pas seulement une union
d'enums. Classer chaque écart : mapping exact, variante explicitement équivalente,
fallback à qualité contractualisée, ou **non représentable** → refus documenté.
Pas de plus petit dénominateur commun imposé à toutes les cibles ; conserver un
chemin spécialisé quand disponible. Ne pas cacher coût/qualité des fallbacks.

Exemples : format compressé non disponible → asset recooké dans un format valide
avec critère de qualité et budget mémoire, pas reinterpretation des octets ;
mesh shading absent → vertex/index si géométrie équivalente ; fonctionnalité de
blending/atomic sans équivalent acceptable → erreur explicite, pas rendu approximé
silencieusement. Ne pas sélectionner un fallback dont les propres capacités ne
sont pas vérifiées. Cache : clés de mappings/ABI/capacités versionnées, état frame
et objets driver séparés ; une convention changée invalide les consommateurs.

## Sources et limites

Liens officiels, aucune copie de shader/exemple ; licence de réutilisation non
inférée. Avant adaptation tierce : notice/fichier et commit exact, pas tree SHA.

- [OpenGL, clip control](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClipControl.xhtml).
- [Vulkan, vertex post-processing](https://docs.vulkan.org/spec/latest/chapters/vertexpostproc.html).
- [Direct3D, rasterizer](https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage) : conventions partagées à recouper avec le state D3D12 cible, pas API D3D11 à appeler depuis D3D12.
- [Metal Shading Language specification](https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf).

Versions évolutives ; relever la cible et le texte applicable. Lecture des liens,
tests CPU et compilation ne prouvent ni image correcte, ni couverture multi-API,
ni performance. Si outil/plateforme manquant : INCONCLUSIVE à ce niveau précis.
