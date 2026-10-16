# Source, artefact final et ABI

## Parcours reproductible

1. Figer fichiers source/includes, dépendances/modules transitifs et hashes.
   Identifier langage (Slang/HLSL/GLSL/MSL), stage, entry, cible, profile/shader model
   et target environment. Noter compilateur/version, flags, macros, capacités,
   optimisation/précision, spécialisation et mode de layout des matrices.
2. Avec Slang, distinguer chargement de module, sélection des entry points,
   composition, spécialisation/linkage et émission cible. Exploiter sa réflexion
   pour ce programme spécialisé et cette cible ; une réflexion avant spécialisation
   ou pour une autre cible peut décrire un layout différent. Expertise prioritaire,
   pas mandat d'intégrer Slang ni `slang-rhi` dans ARTIST.
3. Faire échouer sur diagnostic pertinent ; conserver commande et log utile.
   Utiliser l'aide/version du compilateur présent plutôt qu'une commande de blog.
   Ne pas prétendre appeler `slangc`, `spirv-val` ou l'outil Apple non découvert.
4. Vérifier l'artefact final et les ressources actives effectivement consommées ;
   dead-code elimination peut supprimer des paramètres, elle ne justifie pas de
   supprimer le contrat fonctionnel. Comparer la réflexion avec le layout hôte
   puis avec l'interface de pipeline. **shader compilé ≠ pipeline valide**.

Avec Slang, identifier paramètres génériques/interfaces, conformances et arguments
de spécialisation réellement résolus ; garder une liste bornée des combinaisons
supportées plutôt que multiplier les types par matériau. Une `ParameterBlock<T>`
n'est pas « une structure C++ donc un binding » : sa réflexion peut porter des
catégories/espaces distincts pour données uniformes et ressources. Vérifier
offsets et strides par catégorie, tableaux de ressources, bindings remappés et
capacités après composition. Ne pas déduire l'ABI MSL de la seule réflexion SPIR-V.
La sélection d'entry et de stage appartient au programme final, pas au seul nom
de fonction trouvé dans un module. Les options et interfaces de réflexion sont
versionnées : vérifier la documentation de la version utilisée avant codage.

## Cibles : preuves différentes

- **Slang** : langage/compilateur multi-cibles ; vérifier capacité de la version
  et du target demandé. Ce n'est ni une API graphique ni une promesse que toutes
  les constructions sont traduisibles sur toutes les cibles.
- **HLSL → DXIL** : DXC, stage/profile/entry et validator compatibles ; le shader
  model et les features du device D3D12 restent à vérifier. DXC installé ≠ device.
- **GLSL/HLSL/Slang → SPIR-V** : options et environment Vulkan ou OpenGL explicites,
  validation IR si outil disponible ; SPIR-V pour Vulkan n'est pas automatiquement
  consommable par OpenGL. Le chemin OpenGL SPIR-V nécessite version/extension et
  spécialisation supportées ; la baseline 3.3 core ne les établit pas.
- **SPIR-V → MSL/GLSL/HLSL** (par exemple SPIRV-Cross) : traduction, pas compilation
  finale. Recontrôler binding remap, ressources et limitations de la cible.
- **MSL** : produire le texte sous Windows ne valide pas Apple. La compilation
  finale avec la toolchain Apple puis le pipeline/device/family/OS cible sont des
  étapes distinctes ; sans elles INCONCLUSIVE pour exécution Metal.
- **GLSL OpenGL** : shader compile puis program link et bindings/états au contexte
  effectif ; un succès offline glslang n'est pas le résultat du compilateur driver.

## Layout et données hôtes

Écrire le contrat à l'octet : taille totale, alignement, offset de chaque champ,
stride d'array/matrice/élément, scalar types, binding/space/set/index/location et
visibilité des stages. Relier `sizeof`, `alignof`, `offsetof` C++ et octets uploadés
à la réflexion finale ; un `alignas` arbitraire ne prouve pas tous les offsets.

- `float3`, tableaux, matrices non carrées, structures imbriquées, scalaires 16/64
  bits et `bool` doivent suivre les règles de **la cible et du storage** ; aucune
  règle « tout est 16 octets » ni copie brute du bool C++ par présomption.
- std140/std430, HLSL cbuffer/StructuredBuffer et MSL ne sont pas des alias. Un
  root signature/descriptor layout/argument buffer doit correspondre à la
  réflexion remappée ; ces noms ne se substituent pas aux bindings OpenGL.
- `row-major`/`column-major` décrivent le stockage, pas à eux seuls `M*v` ou `v*M`.
  Fixer explicitement le mode : defaults Slang API/`slangc` peuvent diverger selon
  version. Utiliser une matrice asymétrique non identitaire et un point connu pour
  discriminer transposition ; vérifier world/view/clip, handedness, depth et unités.
- Documenter normalisation, interpolation, précision, couleur linéaire/sRGB,
  HDR/tonemapping. Un cast, fast-math ou une baisse de précision doit préserver les
  domaines numériques et la qualité attendue, y compris NaN/Inf si contractuels.

## Permutations et invalidation

Axes de permutations bornées : features structurelles réellement connues,
qualité/profil autorisés, pas un binaire/type par objet ou état de frame. Énumérer
les combinaisons supportées et refuser les autres ; budgets temps de cook/build et
taille du code à mesurer, pas simplement compter moins de branches.

Une clé de cache inclut source et includes, compilateur/version/options, cible et
profile/environment, entry/stage, spécialisation, mode matrice, schema et ABI.
Modifier n'importe quelle entrée pertinente invalide l'artefact et sa réflexion.
Cook immuable/versionné ≠ objet driver vivant ; le cache de programme/pipeline
doit en plus respecter l'identité device/driver et les contraintes API. Le reload
publie une génération cohérente binaire+ABI ; le propriétaire ressources retarde
le retrait des anciens objets jusqu'à leur dernière consommation GPU.

## Nominal et négatif

- **Nominal** : shader minimal du stage réellement demandé, buffer de paramètres
  avec valeurs asymétriques, tableau court et matrice non identitaire ; vérifier
  offsets/strides via réflexion, puis résultat numérique connu ou pixels définis.
  Distinguer tolérance flottante motivée et comparaison exacte d'entiers.
- **Négatif compilation** : entry inexistant ou stage/profile incompatible ;
  diagnostic de ce motif, pas « outil absent ». Pas de PASS si le compilateur n'a
  jamais pu lire l'entrée valide équivalente.
- **Négatif ABI** : offset hôte ou type de binding modifié ; contrat hôte/reflection
  doit rejeter avant upload/draw. Le compilateur peut accepter ce shader : son
  succès n'annule pas le rejet attendu et ne prouve pas le pipeline.
- **Négatif cache** : même chemin source mais include/options/layout modifié ;
  l'ancienne entrée ne doit pas être réutilisée. Une permutation interdite est
  refusée, jamais remplacée silencieusement par une version visuellement différente.
- Réussite bornée : artefact/ABI prouvés au niveau testé, nominal correct et
  négatifs causaux. Sans device/image, compilation et réflexion peuvent être PASS
  offline, mais GPU/image restent INCONCLUSIVE. Aucune performance sans A/B.

## Sources et limites

Rédaction originale, liens seulement ; inventaire T02 audité le 2026-10-16.
Licence inconnue ou `NOASSERTION` n'autorise aucune adaptation : ne pas copier
sources, exemples ni notices ; une réutilisation future exige commit exact,
chemin/licence/notice vérifiés (tree SHA ≠ commit). Pas de superviseur importé.

- [Slang, projet et guide officiel](https://github.com/shader-slang/slang) : référence
  du langage, compilation et réflexion ; pas adoption du RHI.
- [Slang, conventions de matrice](https://shader-slang.org/slang/user-guide/a1-01-matrix-layout.html) :
  mode explicite et limites des matrices non carrées.
- [DXC](https://github.com/microsoft/DirectXShaderCompiler),
  [glslang](https://github.com/KhronosGroup/glslang),
  [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools),
  [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross) : outils distincts.
- [Metal Shading Language](https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf).

Audit T02 : DXC observé, Slang/glslang/SPIRV-Tools/SPIRV-Cross non résolus sur PATH,
outils Apple non disponibles dans ce périmètre Windows. Ce relevé daté n'est ni
absence absolue ni préflight de la prochaine session. Aucun outil exécuté ici.
