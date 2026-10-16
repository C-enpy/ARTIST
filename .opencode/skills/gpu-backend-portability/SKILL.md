---
name: gpu-backend-portability
description: Isole les écarts entre backends, définit conventions clip/depth, coordonnées, matrices, formats et couleur puis mappings/fallbacks explicites. À utiliser pour un artefact spécifique à une API ou une frontière portable, sans imposer un plus petit dénominateur commun.
---

# Portabilité avec contrat visuel explicite

## Déclencheurs et exclusions

Principal `gpu-portability-engineer` pour artefact sur une API, conventions
divergentes, ressource non représentable ou stratégie de variantes. Charger
`artist-gpu-debug` pour l'image ; `gpu-capability-detection` seulement si une feature
ou limite inconnue l'exige. Traduction/compilation de shader seule : shader-engineer
principal, avec ce skill comme complément, pas ownership automatique du pipeline.

Pas de création d'un RHI à partir d'un bug d'image, ni migration pour masquer
un contexte OpenGL incorrect. ARTIST implémente actuellement OpenGL ; Vulkan,
D3D12 et Metal restent des expertises. ADR-0001/0002/0004 non adoptées ici.

## Préconditions et outils

Demander backends/device/driver, OS/profil/extensions, images attendu/observé,
scène/caméra/seed/frame, résolutions, shaders/entry/options et formats effectifs.
Sans ces entrées : clarification et INCONCLUSIVE, pas matrice de résultats inventée.
Établir convention de référence avant conversion ; examiner matrices, packing et
states, pas uniquement source shader. Exécutions après `artist-build-validation`,
outils disponibles et bail ; aucun SDK/driver/capture installé implicitement.

Compilation/validation offline possibles selon outils ; runtime/GPU/image/
profiling propres à chaque plateforme. MSL généré Windows ne valide pas Metal
natif ; capture et benchmark n'émergent pas de cette compétence documentaire.

## Procédure

1. **Réduire.** Trouver première pass divergente, figer entrées et sorties. Une
   primitive asymétrique colorée avec profondeur connue discrimine mieux qu'une
   scène mouvante. Conserver mêmes options et qualité, pas de wait-idle global.
2. **Écrire la convention moteur.** Clip/depth et reversed-Z, viewport/scissor et
   origine, handedness, winding/front-face, matrices et multiplication, normales,
   espaces couleur, unités/exposure. Ne pas attribuer row-major à une API entière.
3. **Décrire chaque mapping.** Transformer une seule fois à la frontière retenue,
   justifier correction Y/Z, front-face et ranges. Vérifier shader ABI/stride/offset/
   alignement, bindings, sampler/filter/compare et formats/usages/samples réels.
4. **Qualifier l'écart.** Représentable exactement, représentable via variante, ou
   non représentable sous ce contrat. Choisir fallback avec qualité/limitations
   déclarées ou refus explicite ; jamais un plus petit dénominateur imposé.
5. **Préparer et invalider.** Choix de variante quand connu au cook/device ; état
   frame vivant séparé. Clé source/options/ABI + mapping/capacités pertinents,
   invalidation si conventions/format changent. Aucun type par asset/instance.
6. **Vérifier aux frontières.** Tests CPU pour matrices/layouts, compilation et
   réflexion pour ABI, validation API réelle puis pass/image avec oracle fixé.
   Accéder aux pixels linéaires/HDR avant tonemapping si c'est le premier écart.
   Ne pas élargir la tolérance pour masquer la différence.

## Nominal et négatif

- Nominal : triangle asymétrique aux profondeurs near/mid/far, winding connu,
  cible linéaire puis sRGB ; transformations documentées, silhouettes/depth et
  couleurs attendues selon oracle. La matrice de références détaille les écarts.
- Négatif : conversion Z appliquée deux fois ou flip Y sans mise à jour du
  front-face → image/culling différents ; isoler la première transformation fautive.
- Négatif : vue sRGB pour données de normales, stride CPU différent du shader,
  format UAV/attachment non supporté → rejet de compatibilité ou erreur reproduite,
  pas synchronisation globale. Support inconnu → relevé, pas fallback supposé valide.

## Pièges

Convention clip ≠ stockage des matrices ; texture origine ≠ framebuffer origine ;
handedness ≠ règle universelle d'API. Changer depth range sans projection/clear/
compare cohérents n'est pas un portage. sRGB decode/encode ≠ gamma approximatif
ajouté partout ; exporter une image déjà transformée peut doubler la conversion.
Pas d'hypothèse de format compressé, filtre ou atomic identique sur tous devices.

## Réussite et preuves

PASS pour le contrat réellement exercé et métrique/tolérance fixées avant patch,
FAIL si écart reproduit, INCONCLUSIVE pour backend/image/outils manquants. Séparer
texte/compilation/API/image/performance ; aucune matrice exécutée quatre API ici.
Retour : conventions/mappings, candidat HEAD/delta/hashes, workload, fichiers,
images intermédiaires, commandes/comptages/skips, capacités/fallbacks et limites.
Impact CPU/GPU seulement mesuré ou explicitement estimé, jamais gain présumé.

## Références et transfert

[Conventions et cas discriminants](references/conventions.md), rédaction originale.
DEFER_TO au lead : shader-engineer pour compilation/ABI, expert API pour écart
non représentable, engine-architect pour changement durable, rendering-verifier
pour image indépendante. Fournir backend/ownership/binding times, oracle/essais,
inconnues/bail. Ni spawn ni import d'un superviseur ou d'un backend tiers.
