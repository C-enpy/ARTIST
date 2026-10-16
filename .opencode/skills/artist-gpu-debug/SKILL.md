---
name: artist-gpu-debug
description: Isole un rendu noir, une image incorrecte ou une frame RenderDoc par pass, entrées et sorties. Diagnostic GPU reproductible, contexte OpenGL, états, ressources, shaders, synchronisation et capture conditionnelle ; distinct du profiling.
---

# Diagnostic GPU reproductible

## Déclencheurs et Exclusions

Utiliser pour rendu noir, artefact, divergence CPU/GPU, frame/capture et validation
visuelle. `gpu-engineer` porte le correctif OpenGL ; `rendering-verifier` porte
diagnostic/capture indépendants, sans modifier code/tests/goldens. Pour OpenGL
noir, charger ensuite [opengl-backend](../opengl-backend/SKILL.md).

Exclusions : optimisation sans défaut d'image → `artist-performance` ; CPU seul
→ `artist-cpp-contracts` ; API non donnée → clarification ; conception globale
du graphe → `artist-static-boundary`. Ne pas charger toutes les API par défaut.
Le présent skill n'accorde aucun droit d'écriture, de capture ou d'installation.

## Préconditions et pin ARTIST

Relever HEAD, hash du delta et des shaders/assets, OS, GPU, driver,
API/version/profil/extensions, mode Debug/Release, contexte courant et thread.
Le dépôt contient OpenGL/GLFW/GLEW ; un mock valide
un contrat CPU, pas un appel driver. Une VM ou Mesa logiciel ne représente pas
nécessairement le GPU cible.

Pour les tests présents : `ARTIST/component-tests/include/OpenGLComponentTests.hpp`
demande OpenGL 3.3 core. Examiner l'initialisation GLFW/GLEW et le traitement des
échecs avant de conclure à un bug shader. Avant d'exécuter, charger
[artist-build-validation](../artist-build-validation/SKILL.md) ; bail et chemin
d'artefacts explicites. Le pin est une observation, pas une preuve du candidat.
Les autres API restent des expertises ; ne pas inventer un backend ARTIST.

## Réduction du problème

1. Geler seed, caméra, résolution, inputs, paramètres et frame/temps.
2. Passer d'une image incorrecte au premier pass incorrect, puis à ses entrées
   ET à sa sortie attendue (attachment, canal, plage). Définir un oracle falsifiable.
3. Suivre shader compile/link/logs → attributs/uniforms → buffers/textures →
   FBO/attachments → viewport/scissor → depth/stencil/blend → draw/dispatch.
4. Vérifier tailles, usages, strides/offsets, formats, bindings, capacités et
   lifetime jusqu'à la dernière consommation GPU.
5. Isoler la visibilité mémoire, l'ordre et les attentes CPU. Utiliser une attente
   globale temporaire uniquement comme expérience discriminante ; ne pas la
   conserver comme « correction » sans établir le mécanisme nécessaire.
6. Vérifier destruction, rechargement, resize et contexte perdu si le contrat les
   inclut. Tester aussi après plusieurs frames et lors de ressources remplacées.

Activer debug output et labels si l'API/extensions et le périmètre l'autorisent ;
conserver source/type/ID/sévérité, événement et message, pas seulement « aucun
warning ». OpenGL 3.3 core ne garantit ni KHR_debug ni compute/SSBO/persistent mapping.
Pour frame RenderDoc, lire [capture et fallback](references/frame-capture.md).
Découvrir d'abord l'outil/version, support API et compatibilité de replay. Aucun
MCP de capture n'est installé par ce harness ; une capture non faite reste absente.

## Mathématiques et image

- Écrire conventions d'espaces, unités, matrices, normales, handedness et depth.
- Vérifier packing CPU/GPU, interpolation, précision et domaines numériques.
- Séparer texture de couleur sRGB, données linéaires, HDR, exposure et tonemapping.
- Comparer à paramètres constants ; capturer les sorties intermédiaires utiles.
  Définir métrique et tolérance avant de modifier les références, avec attention
  aux différences driver et aux rendus temporels.
- Compiler un shader ne valide ni l'équation, ni le rendu, ni sa stabilité.

## Oracle nominal et négatif

- **Nominal** : scène minimale déterministe, pass et inputs identifiés, sortie
  intermédiaire puis image finale conformes à l'oracle préétabli ; états/API et
  destruction vérifiés sur le contexte cible. Qualifier chaque étape exercée.
- **Négatif** : image finale noire mais attachment HDR non nul → inspecter plage,
  tonemapping, canal et présentation avant d'accuser le shader. Entrée invalide
  ou FBO incomplet → conserver FAIL reproduit ; ne pas « réparer » par glFinish.
- Capture absente, replay incompatible, zéro test ou GPU skipped → INCONCLUSIVE
  pour ce critère, jamais PASS par substitution d'un mock. Un autre FAIL demeure.
- Un shader compilé ne prouve pas le link, les bindings, l'image ou la performance.

## Rapport et Handoff

Symptôme → hypothèse → expérience → observé → conclusion. Joindre candidat
HEAD/delta/hashes, commande, messages utiles et chemin/hash de capture/image.
Compter découverts, exécutés, échoués, skipped ; distinguer `executed`,
`static-presence` et `inference`. **FAIL** pour l'écart reproduit ;
**INCONCLUSIVE** pour l'absence de contexte/capture/précondition. Distinguer
validation API, rendu visuel et performance ; capture ≠ benchmark. Aucun gain
CPU/GPU déduit d'une inspection ou d'un test textuel.

`DEFER_TO` au lead : auteur API pour correctif, `shader-reviewer` pour mathématiques,
`gpu-resource-engineer` pour hazards, `performance-reviewer` pour mesures. Remettre
reproduction, première divergence, expérience suivante, limites et bail ; ne pas
spawn/handoff pour élargir des permissions. Les références détaillent les sources,
outils facultatifs et pièges ; aucune commande tierce n'est exécutée au chargement.
