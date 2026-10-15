---
name: artist-gpu-debug
description: Diagnostic et validation du rendu GPU d'ARTIST. À utiliser pour contexte OpenGL, états, ressources, synchronisation, shaders, image incorrecte ou divergence CPU/GPU.
---

# Diagnostic GPU reproductible

## Préconditions

Relever OS, GPU, driver, API/version/profil/extensions, mode Debug/Release,
contexte courant et thread. Le dépôt contient OpenGL/GLFW/GLEW ; un mock valide
un contrat CPU, pas un appel driver. Une VM ou Mesa logiciel ne représente pas
nécessairement le GPU cible.

Pour les tests présents : `ARTIST/component-tests/include/OpenGLComponentTests.hpp`
demande OpenGL 3.3 core. Examiner l'initialisation GLFW/GLEW et le traitement des
échecs avant de conclure à un bug shader. Charger `artist-build-validation`.

## Réduction du problème

1. Geler seed, caméra, résolution, inputs, paramètres et frame/temps.
2. Passer d'une image incorrecte au premier pass incorrect, puis à ses entrées.
3. Suivre shader compile/link/logs → attributs/uniforms → buffers/textures →
   FBO/attachments → viewport/scissor → depth/stencil/blend → draw/dispatch.
4. Vérifier tailles, usages, strides/offsets, formats, bindings, capacités et
   lifetime jusqu'à la dernière consommation GPU.
5. Isoler la visibilité mémoire, l'ordre et les attentes CPU. Utiliser une attente
   globale temporaire uniquement comme expérience discriminante ; ne pas la
   conserver comme « correction » sans établir le mécanisme nécessaire.
6. Vérifier destruction, rechargement, resize et contexte perdu si le contrat les
   inclut. Tester aussi après plusieurs frames et lors de ressources remplacées.

Activer debug output et labels si l'API et les extensions le permettent.
Pour RenderDoc, Nsight, PIX ou équivalent, découvrir d'abord l'outil/version/API
pris en charge. Aucune capture ni analyse GPU ne doit être annoncée si elle n'a
pas eu lieu. Un MCP de capture n'est pas installé par ce harness.

## Mathématiques et image

- Écrire conventions d'espaces, unités, matrices, normales, handedness et depth.
- Vérifier packing CPU/GPU, interpolation, précision et domaines numériques.
- Séparer texture de couleur sRGB, données linéaires, HDR, exposure et tonemapping.
- Comparer à paramètres constants ; capturer les sorties intermédiaires utiles.
  Définir métrique et tolérance avant de modifier les références, avec attention
  aux différences driver et aux rendus temporels.
- Compiler un shader ne valide ni l'équation, ni le rendu, ni sa stabilité.

## Rapport

Symptôme → hypothèse → expérience → observé → conclusion. Joindre commande,
messages utiles et chemin de capture/image. **FAIL** pour l'écart reproduit ;
**INCONCLUSIVE** pour l'absence de contexte/capture/précondition. Distinguer
validation API, rendu visuel et performance.
