# Checklist ciblée OpenGL : état, accès, durée de vie

Lire uniquement pour l'API OpenGL. Procédures proposées, pas résultats exécutés.
Les commandes/appels mentionnés ne sont autorisés que par le périmètre de la tâche.

## 1. Contexte et capacités obtenus

Faire réussir GLFW et la création, rendre le contexte courant sur le thread
appelant, puis initialiser GLEW/chargeur. Vérifier chaque échec avant le premier
appel GL. Un contexte n'est courant que sur un thread à la fois ; migration =
détachement de l'ancien thread puis attachement maîtrisé, pas accès simultané.
Identifier contexte, share group, surface et ordre du teardown.

Après ce préflight seulement : `GL_VENDOR`, `GL_RENDERER`, `GL_VERSION`,
`GL_SHADING_LANGUAGE_VERSION`, version majeure/mineure, `GL_CONTEXT_PROFILE_MASK`
et flags. En core 3.3, énumérer extensions avec `GL_NUM_EXTENSIONS`/`glGetStringi`,
pas la vieille chaîne globale `glGetString(GL_EXTENSIONS)`. Relever limites des
formats, unités, tailles et bindings utilisés, pas toutes les extensions en dump.

- Compute : OpenGL **4.3** ou `ARB_compute_shader`, points d'entrée disponibles,
  limites workgroups/invocations et autres fonctions effectivement employées.
- SSBO : OpenGL **4.3** ou `ARB_shader_storage_buffer_object`, binding/alignement,
  tailles et support GLSL requis. Compute et SSBO sont deux gates distinctes.
- Persistent mapping : OpenGL **4.4** ou `ARB_buffer_storage`, flags de storage/map
  cohérents et politique de visibilité/reuse. Non impliqué par compute.
- Debug output/groupes/labels : **4.3** ou `KHR_debug`. `ARB_debug_output` peut
  fournir un callback plus ancien, pas tous les labels/groupes KHR. Tout est
  conditionnel ; vérifier les fonctions et utiliser les logs compile/link/FBO
  et erreurs GL ciblées comme fallback. Ne pas prétendre voir un callback absent.

Un système macOS OpenGL déprécié et limité en version n'acquiert pas 4.3/4.4 par
ce document. VM/Mesa logiciel ≠ matériel cible ; contexte créé ≠ pass valide.

## 2. Création → upload → binding → draw → consommation

- **VAO/VBO/EBO** : VAO non nul en core, attributs activés, buffer de chaque
  attribut, type/composantes/normalisation/divisor, stride et offset en **octets**.
  `glVertexAttribPointer` capture le VBO lié à ce moment ; changer ensuite
  `GL_ARRAY_BUFFER` ne remplace pas cette association. `GL_ELEMENT_ARRAY_BUFFER`
  appartient au VAO : le délier peut casser le draw indexé. Entiers GLSL :
  `glVertexAttribIPointer`, pas conversion flottante. Vérifier indice maximal,
  type d'indice, plage et taille du buffer ; VAO/FBO sont **non partageables**
  entre contextes, même d'un même share group.
- **Upload** : dimensions, format interne/externe, type, tailles et padding CPU,
  `GL_UNPACK_ALIGNMENT`/row length et PBO éventuellement lié. Readback : pack
  alignment/PBO distincts ; ne pas lire le pointeur d'un transfert encore en vol.
  Uniform blocks : layout std140, offsets et alignement UBO vérifiés, pas un
  `memcpy` supposant le packing C++. Les données de texture ne sont pas toutes sRGB.
- **GLSL** : source/hash, `#version`, defines, stages/entry attendus. Lire
  `GL_COMPILE_STATUS`/`glGetShaderInfoLog` puis `GL_LINK_STATUS`/`glGetProgramInfoLog`.
  Un link échoué bloque le pass. Après relink, revalider attributs/uniforms et leurs
  locations ; `-1` peut signifier inactif/absent. En 3.3, `glUniform*` modifie le
  programme courant : vérifier `glUseProgram`. Le sampler contient un indice
  d'unité, pas un nom de texture ; vérifier cible, unité, sampler, format et mips.
- **FBO** : attachments, dimensions effectives, niveaux/couches, formats,
  sample counts, draw/read buffers. Tester `glCheckFramebufferStatus` sur le FBO
  concerné : exiger `GL_FRAMEBUFFER_COMPLETE` avant draw/readback. Un FBO incomplet
  n'est pas une image « noire correcte ». Complet ne prouve ni bons attachments
  ni absence de feedback : ne pas échantillonner une image simultanément écrite
  sans mécanisme légal explicitement supporté.
- **États** : viewport et scissor, framebuffer draw/read, clear/masks, rasterizer
  discard, culling/front face, depth test/write/function/range, stencil et blend.
  Contrôler multisample/resolve et sortie vers framebuffer par défaut/swap.
  En OpenGL 3.3, NDC z est par défaut [-1,1], sans supposer `glClipControl`.
  Vérifier matrices/transpose, espaces, handedness et winding. `GL_FRAMEBUFFER_SRGB`
  convertit sur attachment sRGB ; texture couleur sRGB, données linéaires, HDR,
  exposure et tonemapping doivent rester cohérents, sans double gamma.
- **Consommation** : vérifier sortie du pass avant le post-process/present :
  attachment/canal/plage/orientation, image attendue et tolérance décidée avant
  patch. Un clear visible mais triangle absent oriente vers draw/états/inputs,
  pas automatiquement vers la synchronisation. Ne pas réécrire le pipeline entier.

## 3. Synchronisation : domaine et portée explicites

Écrire le ledger producteur → consommateur (RAW/WAR/WAW), ressource/sous-plage,
contexte/share group, accès et dernière utilisation. Séparer :

1. **Ordre** : le flux d'un contexte et les opérations GL ordinaires ont leurs
   règles d'ordonnancement. Un upload ordinaire suivi d'un draw n'exige pas une
   barrière inventée de style Vulkan/D3D12. Certains accès shader sont incohérents.
2. **Visibilité** : `glMemoryBarrier`, si supporté et requis par le type d'accès,
   choisit les bits selon le **consommateur** : écriture shader puis lecture vertex
   → `GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT` ; texture fetch ou SSBO exigent leurs
   domaines propres. Justifier accès, range logique, contexte et portée ; ce
   n'est **pas une barrière universelle**, ni une attente CPU. Ne pas l'appeler
   dans un contexte 3.3 où son support n'a pas été établi.
3. **Complétion/attente CPU** : `glFenceSync` après la dernière utilisation et
   `glClientWaitSync` avec résultat/timeout contrôlés peuvent autoriser réutilisation
   d'une zone ou lecture CPU. `glWaitSync` ordonne l'attente côté serveur, pas CPU.
   `glFlush` soumet le travail mais n'attend pas son achèvement ; ce n'est pas une
   barrière mémoire. Avec plusieurs contextes partageant des objets, le producteur
   doit soumettre le fence (flush), le consommateur attendre et respecter les
   règles de rebinding/visibilité des objets partagés ; le partage seul ne suffit pas.

Persistent mapping, seulement si gate satisfaite : pour CPU→GPU non-coherent,
mapper avec `GL_MAP_FLUSH_EXPLICIT_BIT` puis `glFlushMappedBufferRange` sur les
plages écrites avant leur consommation. Pour écritures shader→CPU non-coherent,
le domaine `GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT` et une preuve de complétion
(fence puis attente CPU réussie) remplissent des rôles différents. Vérifier les
flags et le sens exact de l'accès avant de choisir la recette. Coherent ne
supprime ni le hazard d'écraser une zone en vol ni l'attente de complétion avant
lecture CPU. Une génération ne prouve pas la complétion.
`glFinish` peut discriminer temporairement une hypothèse, jamais devenir une
correction de performance non mesurée. Il ne rend pas un mauvais binding valide.

## 4. Destruction et erreurs reproductibles

L'owner C++ doit détruire sur un thread ayant un contexte valide compatible avec
l'objet, avant teardown GLFW. Contexte exact pour VAO/FBO ; objets partageables
et share group à vérifier pour buffers/textures/programmes/sync. Conserver un
ledger owner/emprunt/génération ; remplacement/resize/relink invalide les emprunts
et caches concernés. Aucun GLuint recyclé ne valide un ancien emprunt.

`glDelete*` peut différer la libération effective tant que l'objet reste utilisé
ou référencé selon les règles GL ; ne pas ajouter une attente globale à chaque
destructeur. Distinguer libération du nom, références GL et mémoire encore en vol.
Réutiliser une zone CPU/persistante impose sa propre preuve de dernière complétion ;
la destruction RAII ou un compteur de génération ne l'apportent pas. Couvrir move,
rechargement, resize, multi-frame et destruction après usage quand concernés.

**Nominal** : triangle déterministe + readback défini, teardown avant fermeture
du contexte, aucun message d'erreur ciblé. **Négatif** : sans contexte courant,
refuser avant appel ; FBO incomplet/link échoué, conserver statut/log et FAIL ;
capacité absente, refuser/fallback documenté. Ces oracles sont à exécuter, pas des
observations acquises. Avec KHR_debug disponible, garder source/type/ID/sévérité
et frame/event ; callback synchrone seulement pour diagnostic, coût non nul.

## Références officielles, liens seulement

Rédaction originale ; aucun code ou texte tiers importé. Version/profil et notices
à vérifier avant adaptation (licence inconnue = lien seulement).

- [OpenGL 3.3 core](https://registry.khronos.org/OpenGL/specs/gl/glspec33.core.pdf)
  : contexte/objets/états, partage et sync ; règles précises priment sur la checklist.
- [Refpages Khronos](https://registry.khronos.org/OpenGL-Refpages/gl4/) : vérifier
  les versions supportées de chaque appel, pas appliquer tout gl4 au profil 3.3.
- [KHR_debug](https://registry.khronos.org/OpenGL/extensions/KHR/KHR_debug.txt),
  [ARB_compute_shader](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_compute_shader.txt),
  [ARB_shader_storage_buffer_object](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shader_storage_buffer_object.txt),
  [ARB_buffer_storage](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_buffer_storage.txt).
- [Guide contexte GLFW](https://www.glfw.org/docs/latest/context_guide.html).
