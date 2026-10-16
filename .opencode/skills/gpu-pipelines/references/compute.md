# Compute : layout, groupes et consommateurs

## Préconditions spécifiques

Identifier API/device, queue capable de compute, limites de workgroup par axe,
nombre total d'invocations et mémoire partagée, limites de dispatch, formats et
bindings. OpenGL 4.3 core fournit compute/SSBO ; une voie par extensions demande
de vérifier chaque fonctionnalité utilisée. La fixture ARTIST 3.3 ne les garantit
pas. Sans capacité → refus/fallback explicitement approuvé, pas dispatch supposé.
Les autres API demandent leurs limites/features réellement activées.

## Procédure compute

1. Déclarer source/entry compute, spécialisation, layout et accès par binding ;
   refléter l'artefact final, tailles/offsets/strides/alignements inclus. Pas de
   vertex input, attachments ou depth state dans l'oracle de ce kernel.
2. Fixer L, taille locale, puis nombre de groupes `ceil(N / L)` pour un domaine
   linéaire ; vérifier overflow/limites du calcul hôte avant conversion. L fixé
   ou spécialisé doit correspondre au binaire. Relever les axes pour un domaine
   2D/3D ; ne pas confondre groupes, invocations et nombre d'éléments.
3. Lire/écrire seulement les indices valides. Dans un kernel coopératif, les
   invocations inactives restent participantes à toute `barrier` de workgroup :
   un early return divergent avant cette barrière peut bloquer ou être invalide.
   Une barrière locale ne synchronise jamais tous les workgroups d'un dispatch.
4. Relever upload CPU→entrée GPU, sorties et chaque consommateur (dispatch,
   vertex fetch, texture, indirect draw, readback). Ordre d'exécution, visibilité
   mémoire et attente CPU sont distincts. Exprimer domaine/stage/access/portée
   au spécialiste ressources ; le graphe ordonne, le backend traduit.
5. Exemple OpenGL si disponible : écriture SSBO puis lecture par vertex fetch →
   domaine consommateur `GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT` ; SSBO→SSBO →
   `GL_SHADER_STORAGE_BARRIER_BIT`. Ajouter les domaines effectivement requis,
   pas `GL_ALL_BARRIER_BITS` systématique. `glMemoryBarrier` n'est pas une attente
   CPU. Readback/mapping, notamment persistant, exige son propre contrat de
   visibilité/cohérence et de complétion ; pas de règle universelle dérivée du cas.
6. Pour plusieurs queues, transmettre dépendance et propriété éventuelle selon
   l'API ; une fence CPU ne remplace pas automatiquement l'attente GPU. Ne pas
   transposer les resource states D3D12 ou queue-family transfers à OpenGL.
7. Vérifier ping-pong, version des bindings après remplacement et retrait après
   dernière consommation de toutes les queues. Génération CPU ≠ complétion GPU.

## Nominal et négatif

- **Nominal minimal** : N = 257, L = 64 si supporté ; entrée entière `i`, sortie
  `2*i+1`, petite plage sans overflow. Allouer aussi une zone sentinelle hors du
  domaine logique et vérifier qu'elle reste intacte. Comparaison exacte des entiers
  après readback correctement synchronisé. Ce cas sans barrière locale exerce la
  garde de tail ; un kernel coopératif demande un autre oracle.
- **Consommation** : faire lire cette sortie par un second kernel ou par un draw
  contrôlé, selon l'usage réel, et vérifier le résultat. Réussir le readback seul
  ne prouve pas une dépendance compute→graphics ni les transferts inter-queues.
- **N = 0** : contrat hôte explicite, ici ne pas soumettre et ne rien modifier.
  L'API peut accepter une dimension nulle ; ce n'est pas un traitement uniforme
  imposé à toutes les API. N=1, N=L, N=L+1 discriminent les tailles limites.
- **Négatif** : mauvais type/binding, buffer trop court ou L au-delà des limites →
  rejet avant soumission, message causal, sortie/sentinelle inchangée. Ne pas
  dépendre d'une écriture GPU hors limites pour prouver ce rejet. Une mutation
  de garde se teste seulement sur fixture dédiée autorisée et bornée.
- Réussite : résultat exact, sentinelle intacte, consommateur valide et négatifs
  rejetés. Sans contexte compute réel, compiler le kernel ou simuler la formule
  ne prouve pas le GPU : INCONCLUSIVE. Mesurer séparément CPU submit/GPU/attentes
  seulement si une optimisation est demandée, via `artist-performance`.

## Sources et limites

Procédure originale ; licence inconnue implique lien seulement, aucune copie de
shader/helper tiers. Références à confronter à la version réellement supportée :

- [OpenGL, compute dispatch](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDispatchCompute.xhtml).
- [OpenGL, domaines de visibilité](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml).
- [Vulkan, dispatch et limites](https://docs.vulkan.org/spec/latest/chapters/dispatch.html).
- [D3D12, Dispatch](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-dispatch).
- [Metal, compute command encoder](https://developer.apple.com/documentation/metal/mtlcomputecommandencoder).

Ce sont des procédures, pas des dispatches exécutés par ce skill. Driver, device,
validation API et synchronisation réelle non observés restent INCONCLUSIVE.
