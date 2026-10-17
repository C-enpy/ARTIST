---
name: opengl-backend
description: Expertise OpenGL pour contexte courant/thread, version/profil/extensions, VAO/VBO/FBO, GLSL compile/link, états, synchronisation et destruction. À charger pour le backend ARTIST 3.3 core ou un rendu OpenGL noir, sans supposer compute/SSBO/persistent mapping.
---

# Backend OpenGL : contrat avant appel driver

## Déclencheurs et Exclusions

Principal : `gpu-engineer`, ID historique conservé. Charger ce skill pour API,
états implicites, erreurs GLSL, binding, FBO, upload et lifetime OpenGL. Pour un
rendu noir/artefact, commencer par
[artist-gpu-debug](../artist-gpu-debug/SKILL.md), puis isoler le pass OpenGL.

Exclusions : pas de RHI Vulkan/D3D12/Metal à inventer, pas de réécriture de graphe
pour un mauvais binding, pas de profiling sans protocole `artist-performance`.
Une demande sans API demande clarification. La capture indépendante relève de
`rendering-verifier`, jamais d'un correctif implicite dans son rôle.

## Préconditions et pin observé

- OS, device/GPU, driver, API/version/profil/GLSL/extensions **obtenus**, contexte
  courant/thread, share group, chargeur de fonctions et capacités réellement
  disponibles. Ne pas confondre version demandée et contexte obtenu.
- ARTIST : `ARTIST/include/graphic/opengl/`, fixture
  `ARTIST/component-tests/include/OpenGLComponentTests.hpp` GLFW/GLEW **3.3 core**.
  Sur cette base, compute, SSBO, persistent mapping et KHR_debug sont **non garantis**.
  Vérifier les gates précisés dans la référence, pas le seul nom du GPU.
- Pin au contrat T03 du 2026-10-16 : `PipelineContext` possède
  `vector<unique_ptr<IPass<API>>>`, `getPass` retourne un **emprunt**, pas un owner
  partagé. Relire le candidat : HEAD, hash du delta, shaders et fixtures. Une
  divergence donne « Pin observé », pas une réécriture silencieuse du contrat.
- ADR-0001/0002/0004/0006/0007 **Proposed** ; ADR-0003/0005 Accepted/**partial** ;
  ADR-0008 vérifié Windows Debug seulement. Aucune nouvelle adoption/performance.
- Avant tout build/test, charger `artist-build-validation`. Un contexte réel,
  des entrées connues et un bail GPU sont requis pour une preuve driver/image.
  Pas d'installation de SDK/driver, pas de capture automatique au chargement.

## Procédure

1. Geler scène, seed, caméra, résolution, frame et attendu. Écrire une hypothèse
   falsifiable, puis isoler un pass et ses entrées/sorties avant synchronisation.
2. Relever contexte/profil/limites. Suivre création → upload → binding → draw
   (dispatch seulement si supporté) → consommation → destruction sur le bon thread.
3. Vérifier interfaces CPU/GPU, GLSL compile/link/logs, VAO/VBO/EBO, textures,
   FBO/completeness et états. Le détail et les gates sont dans
   [contexte, état et lifetime](references/context-lifetime.md).
4. Distinguer ordre d'exécution, visibilité et attente CPU ; lister producteur,
   consommateur, accès/domaine, contexte et dernière utilisation. Aucune barrière
   universelle, aucun glFinish permanent pour masquer un défaut.
5. Valider matrices, espaces/handedness, clip/depth, formats/strides/offsets,
   alignement et colorimétrie linéaire/sRGB. Refaire resize/remplacement et plusieurs
   frames si le contrat les inclut ; préserver qualité et ownership.

## Oracle : nominal, négatif, réussite

- **Nominal** : triangle minimal en 3.3 core avec shader GLSL compatible, VAO/EBO
  valides et FBO complet ; readback/image conforme à l'attendu fixé avant correction,
  absence des erreurs API recherchées, libération dans le contexte autorisé.
- **Négatif** : FBO incomplet ou link échoué → statut/log explicite, aucun draw
  présenté comme réussi. Appel sans contexte courant → précondition refusée avant
  les appels driver, pas essai dangereux pour fabriquer une erreur GL.
- Autres contre-exemples : VAO d'un autre contexte, offset/stride hors bornes,
  handle de génération périmée, compute demandé en 3.3 sans extension → refus ou
  fallback explicite avec qualité attendue, jamais succès simulé.
- Réussite : cause isolée, patch borné et tests réels adaptés au critère. Un mock
  prouve au plus le contrat CPU ; précondition absente = **INCONCLUSIVE**, écart
  reproduit = **FAIL**, résultat conforme au critère exécuté = **PASS**.

## Handoff et retour

`DEFER_TO` au lead : `gpu-resource-engineer` pour ownership/hazards transverses,
`render-graph-engineer` pour scheduling, `gpu-portability-engineer` pour écart
entre API, `shader-reviewer` pour équations/ABI et `rendering-verifier` pour preuve
indépendante. Feuille sans sous-délégation ; fournir reproduction et limites.

Rapport : candidat, hypothèse/cause, attendu/observé, commandes, messages GL utiles,
nombre de tests/skips, patch et impact CPU/GPU **estimé ou mesuré** identifié.
Séparer validation API, image/capture et performance. Les références officielles
sont des liens documentaires : aucune copie tierce ni preuve d'exécution.
