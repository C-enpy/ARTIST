# Usage borné — uniform float OpenGL préparé

Tranche adoptée du contrat de mission, pas adoption globale de l'ADR-0004.
Préparation **runtime après link/réflexion**, pas au cook offline. Seul un
`GL_FLOAT` strictement scalaire est accepté ; même un array de taille 1 est refusé.

Extrait compilé dans `PreparedFloatUniformRenderTests.cpp` :

```cpp
#include <graphic/opengl/pipeline/PreparedFloatUniform.hpp>

// pass est un Pass<OpenGL, Classic> chargé et réfléchi ; son contexte GLFW est courant.
pass.use();
const auto prepared = artist::graphic::opengl::pipeline::prepareFloatUniform(pass, "f2Scalar");
for (float value : {0.f, .5f, -.25f}) prepared.set(value);
```

L'accès est un **emprunt** mono-thread, sérialisé et non réentrant, sans ownership
du pass, des uniforms ou de la fenêtre. Garder les owners et le contexte GLFW
vivants jusqu'au retrait des ressources. Les témoins/générations sont contrôlés
avant les pointees ; un accès périmé ou le mauvais programme/contexte est rejeté
avant miroir CPU/upload. Après retrait, nouvelle publication de map, mutation des
métadonnées ou remplacement du programme, préparer un nouvel accès. Les moves
suivent le contexte unique transféré ; ne pas relink/delete par des appels GL
externes contournant les owners.

`set(float)` garde le miroir CPU historique (`get<GLfloat>()`), sans recherche
de nom, nouvelle résolution native ni `any_cast` pendant l'update. Le miroir
reste un `std::any` : aucune promesse de zéro allocation ou de gain de vitesse.
Le nom absent/optimisé et les mauvais types sont des erreurs, pas des accès optionnels.
L'API historique `pass.withUniform(name, value)` reste disponible avec ses propres
compatibilités (dont bool legacy), sans extension des setters d'arrays.

## Consommateurs et limites de preuve

- `ReflectedOwnerRejectsOtherCurrentProgramAndRecovers` prépare `target` une seule
  fois : `2.5 → rejet 17 sous decoy → 5.5`, `decoy=8.5` restant par nom. Le miroir
  CPU et les readbacks `glGetUniformfv` contrôlent les deux programmes.
- `LegacyAndPreparedMatchFixedNonemptyPixels` initialise les autres uniforms,
  dessine le triangle contractuel dans un FBO RGBA8 linéaire 32×32 et contrôle
  séparément les quatre pixels fixés avant code pour chaque route à `0` puis `.5`.
  L'égalité des images complètes complète cet oracle non vide, sans le remplacer.
- `PreparedUpdatesDoNotResolveNativeLocationsAgain` calibre un espion GLEW
  pass-through local puis observe trois updates : seulement les résolutions
  natives sont comptées ici. Les recherches CPU et extractions effacées sont
  observées séparément par les tests unitaires du mécanisme.

Il s'agit de vrais consommateurs du backend dans des component-tests, pas d'une
application produit, d'un benchmark ni d'une preuve de performance. Les commandes,
JUnit, contrôles négatifs et empreintes auteur sont remis sous
`out/logs/prepared-float-consumer/` ; la validation du candidat intégré, les revues
indépendantes et les mutations C/L/A restent au runner lead.
