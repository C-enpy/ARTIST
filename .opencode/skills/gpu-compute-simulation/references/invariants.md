# Invariants numériques et frontières simulation/rendu

## Fixture commune

Fixer modèle, domaine, unités, seed et état initial, dt, substeps, itérations,
coefficients, conditions aux bords, collisions, N et frame. Préfixer précision,
tolérance absolue/relative et horizon temporel avec justification, pas après voir
l'image. Inspecter NaN/Inf, bornes mémoire, positions/vitesses et compteurs, puis
invariants physiques appropriés. Même seed n'impose pas l'ordre des atomiques ou
des réductions flottantes sur différentes queues/devices ; déclarer déterminisme
exact ou seulement numérique/visuel et le tester comme tel.

Nominal petit cas analytique/CPU → dispatch réel → séquence multi-frames → lecture
du rendu. Négatif avant soumission pour paramètres invalides, puis scénarios
numériques contrôlés valides ; jamais une écriture hors allocation en production.
N=0, N=1, tail non multiple du workgroup, capacité atteinte et remplacement du
buffer font partie des limites. Le test CPU et l'image seule ne remplacent pas
la vérification des données GPU.

## Particules

- **Nominal** : particules indépendantes sans collisions/forces, vitesses connues :
  après k pas, position initiale + vitesse * k*dt ; count borné, lifetime selon
  convention fixée. Ajouter ensuite émission/mort/compaction avec seed et IDs
  stables ; aucun doublon/perte hors la politique prévue.
- **Négatif** : capacité saturée, lifetime négative, dt non fini ou entrée NaN →
  rejet/diagnostic selon contrat préfixé, pas propagation silencieuse. Le protocole
  de saturation doit borner l'écriture, pas seulement incrémenter puis clamp count.
- **Invariants** : count ≤ capacité, index valide, valeurs finies, masses/lifetimes
  admissibles. Avec gravité/collision, vérifier le modèle d'intégration choisi ;
  ne pas exiger conservation d'énergie d'un système dissipatif avec restitution.

## Tissu (cloth)

- **Nominal** : petit patch, rest lengths connus, sommets pinned immobiles, sans
  force ; puis gravité faible sous dt/itérations fixés. Mesurer erreur de contrainte
  longueur/rest length et pénétration aux collisions selon le modèle retenu.
- **Négatif** : masse inverse négative/non finie, indices hors topologie ou triangle
  dégénéré → validation des données avant dispatch. Un dt trop grand pour le régime
  convenu est refusé/subdivisé selon politique, pas « réparé » par un clamp caché.
- **Invariants** : pinned stables, bornes des contraintes, positions finies, stretch
  et tolérance définis. PBD/XPBD et intégrateur force-based ont des critères
  différents ; une dissipation ou un changement de dt n'est pas gratuit.

## Cheveux (hair)

- **Nominal** : une mèche, racine pinned, segments au repos ; ajouter force douce.
  Longueurs et contraintes de flexion restent dans la tolérance, orientation et
  attaches suivent le repère voulu ; tester le passage du root animé au solveur.
- **Négatif** : segment de longueur nulle ou root absent → rejet ou traitement
  explicitement spécifié avant normalisation/division. Collisions/coïncidences
  ne doivent pas engendrer NaN ou indices voisins hors domaine.
- **Invariants** : racine attachée, longueurs positives, positions finies et
  continuité exigée. Ne pas réclamer un invariant de twist si le modèle n'en porte
  pas ; ne pas diminuer les itérations pour obtenir un faux gain égal-qualité.

## Modèles fluides

- **Nominal** : choisir un modèle (grille incompressible, SPH, autre), pas tous à
  la fois. Grille au repos en domaine fermé → vitesse/divergence proches de zéro,
  masse constante sans sources ni sinks. Pour projection, résidu/divergence doivent
  satisfaire le seuil fixé après les itérations prévues ; pour SPH, contrôler
  densité et erreur admissible sous masses/voisinages connus.
- **Négatif** : cellule/taille de support nulle, densité négative/non finie ou
  vitesse incompatible avec la stabilité du schéma → refus/substeps explicites.
  CFL (dt, vitesse, taille de cellule) est pertinente selon le schéma ; ce n'est
  ni une garantie universelle ni une excuse pour ignorer diffusion/viscosité et
  solveur de pression. Pas de clamp cachant une divergence croissante.
- **Invariants** : bornes et valeurs finies, masse/densité selon sources et modèle,
  conditions aux bords et convergence. Un rendu de surface lissé peut masquer un
  mauvais champ ; lire les champs/résidus avant de toucher au shading.

## Passes, queues et rendu

Pour chaque ressource : owner, emprunts, génération, allocation/capacité,
producteur (pass/queue/domaine d'écriture), consommateurs (pass/queue/lecture),
dernière utilisation et condition de retrait. Le solveur exprime RAW/WAR/WAW ;
`gpu-resource-engineer` traite visibilité/réemploi, `render-graph-engineer` calcule
ordre et dépendances, le spécialiste API traduit stages/access/queues. Ordre GPU,
visibilité mémoire et attente CPU distincts ; attendre le CPU partout détruit
potentiellement le chevauchement sans corriger un mauvais binding.

- État t lu, état t+dt écrit : ping-pong et swap CPU seulement après publication
  cohérente des bindings. Interpolation rendering entre deux états exige les deux
  versions vivantes jusqu'à leur dernière lecture, sur toutes les queues.
- Le rendu lit positions/counters/indirect arguments avec layout, espaces et
  frame convenus ; vérifier bounds/culling, formats et couleur s'il y a shading.
  Ne pas injecter les états depth/blend du rendu dans l'oracle physique.
- Une dépendance workgroup ne vaut pas rendez-vous global ; décomposer le solveur
  en dispatches si les passes nécessitent visibilité de tous les groupes.
- dt=0 peut être défini ici comme état gelé sans émission ni intégration ; dt<0
  et non fini rejetés. Si le produit exige un autre comportement, le faire décider
  avant test. Dépassement du budget substeps/temps accumulé : rendre la politique
  visible, ne pas jeter du temps sans le signaler.

Réussite : nominal dans la tolérance préfixée, négatifs diagnostiqués, stabilité
sur horizon représentatif, consommation rendering correcte et retrait sûr si
testés. Capacité, contexte compute ou observation manquants → INCONCLUSIVE sur le
GPU, indépendamment d'une référence CPU réussie. Performance via protocole A/B,
pas déduite du nombre de passes/branches ni de l'async compute activé.

## Sources et limites

Procédure originale de validation, sans code ni algorithme tiers copié. Une licence
inconnue implique un lien seulement ; réutiliser un solveur exige
source/commit/notice exacts, choix de modèle approuvé et nouveau contrat de données/qualité.

- [GPUGems, simulation fluide sur GPU](https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-38-fast-fluid-dynamics-simulation-gpu) :
  référence historique, ni schéma imposé ni performance pour le GPU courant.
- [Position Based Dynamics, projet de référence](https://github.com/InteractiveComputerGraphics/PositionBasedDynamics) :
  lecture conceptuelle seulement, aucun import de code ou choix de solveur ARTIST.
- [Vulkan, synchronisation](https://docs.vulkan.org/spec/latest/chapters/synchronization.html).
- [OpenGL, compute et domaines mémoire](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml).

Ces références ne sont pas des simulations exécutées. Qualifier le modèle précis,
la version des outils, le domaine de stabilité et les limites de déterminisme.
