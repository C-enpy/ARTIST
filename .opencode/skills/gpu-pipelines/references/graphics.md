# Graphics : interface de stages et état de rasterisation

## Contrat réduit avant correction

Une pass, un triangle, un vertex shader et un fragment/pixel shader, une cible de
couleur et éventuellement depth. Fixer caméra, résolution, viewport/scissor,
clear, frame, GPU/driver/API et hashes ; définir l'image et la tolérance avant
le patch. Exemple depth : deux triangles à profondeur connue, ordre inversé,
occlusion identique attendue lorsque depth test/write sont correctement activés.
Le winding et le culling font partie du cas, pas une hypothèse silencieuse.

## Procédure graphics

1. **Interface shader** : stage et entry réels, variantes et ressources actives.
   Relier locations/semantics, types, nombre de composantes et interpolation entre
   stages. Une sortie entière flat, un attribut normalisé ou un format integer
   n'est pas interchangeable avec une entrée floating-point non normalisée.
2. **Entrée géométrique (vertex input)** : buffer/indices et formats, stride,
   offset, alignement, divisor/instance rate, index type et bornes. Vérifier l'octet CPU réellement
   uploadé, pas seulement la déclaration C++. Matrices : ordre en mémoire et
   convention de multiplication distincts ; world/view/clip et handedness écrits.
3. **Cible** : attachments couleur/depth/stencil, formats, usages autorisés et
   sample count compatibles avec la pass/pipeline ; vérifier resolve si MSAA.
   En OpenGL, FBO completeness et paramètres du programme/VAO sont nécessaires,
   pas un « PSO » supposé identique aux API explicites.
4. **États** : topology, raster/cull/front face, viewport/scissor, depth compare,
   depth write, stencil, blend equations/factors et write masks. Noter états
   statiques/dynamiques selon l'API ; un état dynamique doit être effectivement
   fixé avant draw. Reversed-Z exige clear/compare/projection cohérents ; ne pas
   l'activer pour masquer un bug. Clip depth OpenGL par défaut [-1,1] n'est pas
   automatiquement [0,1] ; conventions alternatives exigent capacité et setup.
5. **Couleur** : textures couleur sRGB décodées versus données linéaires ; choix
   du format de sortie et conversion d'encodage cohérents. Ne pas « corriger »
   un écart par gamma arbitraire, tonemapping ou modification du golden.
6. **Exécuter** : valider création/link puis binding et draw, messages pertinents
   si validation/debug réellement activables. Inspecter le premier output fautif,
   puis un seul input. Lire l'image seulement après disponibilité de la sortie ;
   une attente CPU de readback pour le test n'est pas une correction de pipeline.
7. **Retirer** : dernier draw consommateur, génération de pipeline au reload,
   emprunts invalidés au remplacement et contexte de destruction. Un changement
   d'ABI ne doit pas laisser un ancien binding compatible seulement par hasard.

## Nominal, négatif et preuve

- **Nominal** : triangle connu, couleur/coverage et occlusion depth comparés à
  l'oracle fixé. Recréer après resize/reload si cela appartient au contrat ;
  vérifier le nouvel attachment/layout sans conserver des IDs périmés.
- **Négatif ABI** : changer le type/location d'une sortie consommée, ou le sample
  count/format d'un attachment, pour rendre le contrat incompatible. Rejet par
  le validateur d'interface/creation attendu avec diagnostic causal. Si l'API
  laisse le cas indéfini ou non détecté, ne pas soumettre un draw invalide : tester
  le rejet dans le constructeur/contrat CPU autorisé, puis le nominal GPU réel.
- **Négatif visuel contrôlé** : depth test désactivé sur ce cas de triangles,
  sous paramètres fixes, doit violer l'occlusion attendue. Ce témoin discrimine
  un problème d'état sans exiger une barrière universelle.
- Réussite : interface acceptée, aucun diagnostic de validation pertinent non
  résolu, image conforme, cas incompatibles rejetés et retrait sûr si testé.
  Compilation/link seuls → pas PASS image. Sans contexte/device/image de référence
  approprié : INCONCLUSIVE sur cette preuve, avec précondition manquante nommée.

## Sources et limites

Procédure originale ARTIST ; références techniques, pas code adapté. Audit de
provenance : `.opencode/evidence/mission-gpu-qa/t02-external-sources.md`.
Une licence inconnue implique un lien seulement ; aucune notice/implémentation
tierce recopiée.

- [Vulkan, pipelines et états](https://docs.vulkan.org/spec/latest/chapters/pipelines.html).
- [D3D12, gestion de l'état pipeline](https://learn.microsoft.com/en-us/windows/win32/direct3d12/managing-graphics-pipeline-state-in-direct3d-12).
- [Metal, render pipeline descriptor](https://developer.apple.com/documentation/metal/mtlrenderpipelinedescriptor).
- [OpenGL, registre et spécifications](https://registry.khronos.org/OpenGL/).

Ces références ne prouvent ni backend ARTIST multi-API, ni outillage installé.
Choisir la version de spécification correspondant au device et au compilateur ;
une page « latest » n'active aucune feature. SDK, GPU, capture et image non exécutés
restent INCONCLUSIVE, même si le contrat documentaire passe ses contrôles locaux.
