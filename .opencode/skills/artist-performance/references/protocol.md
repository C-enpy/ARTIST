# Protocole A/B et diagnostic du goulot

## Définir l'expérience avant instrumentation

Écrire « si X est la cause, changer Y à travail constant doit modifier Z ».
Définir scène, caméra, résolution, seed, durée, charge CPU/GPU, nombre de draws/
dispatches/instances/triangles, formats, LOD et qualité. Identifier baseline A,
candidat B, HEAD + delta + hashes, toolchain/STL, OS, CPU, GPU, driver, flags,
LTO/PGO, bibliothèques, paramètres runtime, fréquence et mode alimentation.

Même workload, mêmes options et qualité hors variable explicitement testée.
Ne pas opposer shader Debug à Release, qualité réduite à référence complète,
matériel différent, caches chauds à froids ou vsync désactivée à activée.
Un output observable/checksum empêche l'élimination du travail ; ne pas mettre
un lock/print ou un `volatile` arbitraire dans le kernel pour « sécuriser » le test.

Déclarer warm-up, nombre de répétitions indépendantes, durée d'échantillonnage,
ordre A/B alterné ou randomisé reproductible, température/charge de fond, état
des caches et règle de rejet des outliers avant collecte. Conserver tous les
runs, y compris échecs et exclusions motivées. Nombre adapté à la dispersion et
à l'effet recherché, pas seuil universel inventé ; un run unique ne suffit pas.
Rapporter médiane, dispersion (IQR/écart-type selon distribution), percentiles
et unité ; un percentile extrême exige assez d'échantillons. Distinguer runs et
frames corrélées d'un même run ; intervalle de confiance si méthodologie adaptée.

## Isoler les coûts, puis tester une cause

- **CPU submit** : préparation/traversée, allocations et octets copiés, appels
  driver/soumission, locks et sleeps/waits. Chronométrer les scopes exacts avec
  horloge monotone ; un appel submit rapide ne mesure pas la fin du travail GPU.
- **GPU** : temps de passes/dispatchs et bornes de timestamps ; relever unités,
  fréquence, résolution, validité/disjoint, wrap et disponibilité selon API.
  Lecture différée des queries/résultats : ne pas introduire un wait forcé dans
  chaque frame. Calibrer avant corrélation CPU/GPU ; clocks non interchangeables.
- **waits** : dépendances, queue idle, fences, stalls driver, transferts et
  backpressure. Une attente visible peut être le symptôme d'un travail en amont,
  pas la cause. Carte des dépendances/queues et intervalles avant de proposer
  synchronisation différente. Temps CPU et GPU peuvent se chevaucher.
- **spacing / cadence** : intervalle entre soumissions/frames, frames en vol,
  pacing, present/vsync, compositor et latence bout-en-bout. FPS moyen masque
  jitter/stutters ; ne pas confondre débit, latence et temps d'un draw.
- **compteurs** : IPC/cache/branches, bandwidth/octets, occupancy, waves actives,
  utilisation ALU/TEX ou causes de stall si outil/API/device les expose. Relever
  définitions, unités, normalisation, portée et multiplexage/replay. Occupancy
  haute n'est pas une preuve de débit, pourcentage busy n'est pas temps critique.
  Compteur indisponible → non mesuré, pas zéro ni estimation présentée comme fait.

Mesurer overhead instrumentation en contrôle séparé ; debug layers/capture/
sanitizers changent le workload. Résultats instrumentés servent au diagnostic,
pas directement au chiffre de livraison sans comparaison contrôlée.

## Frontières et coûts déplacés

Ordre utile : algorithme/travail → layout/mouvements → allocations/ownership →
synchronisation/soumission → dispatch → micro-optimisation. `virtual`, templates,
`shared_ptr` ou `std::any` seuls ne prouvent pas un goulot. Donner site, fréquence,
granularité et lot ; comparer une spécialisation à une baseline runtime dense
optimisée, pas à une hiérarchie artificiellement mauvaise.

Inspecter désassemblage et optimization remarks pour inlining/vectorisation ;
compiler avec les mêmes options pertinentes. Mesurer code-size/code généré,
nombre d'instanciations/permutations, pression i-cache, build clean/incrémental,
temps/link et pic mémoire du compilateur. Import, cook, load et création PSO ont
des coûts séparés du temps de frame, et caches froid/chaud identifiés. Un cache
driver ne devient pas portable parce que sa création est déplacée hors frame.

## Cas discriminants et verdict

- **P-01 nominal** : « goulot GPU » sans mesures → `performance-reviewer` et
  `artist-performance` seulement ; demander baseline/workload/cible, fournir
  HYPOTHESIS et expérience discriminante. Aucun gain chiffré et aucun correctif.
  Si mesures répétées sont autorisées et présentes, conclure MEASURED avec portée.
- **P-02 négatif** : qualité ou travail A/B différents → INCONCLUSIVE pour le gain
  à qualité égale ; établir une comparaison correcte plutôt que retoucher l'oracle.
- **P-03 négatif** : capture seule ou run unique → localisation/hypothèse possible,
  pas gain moteur. Outillage absent/inaccessible : protocole seulement, pas PASS.

Succès d'un budget = résultat compatible avec seuil préfixé et incertitude,
qualité préservée, candidat valide. Dégradation reproduite → FAIL du budget,
même si la campagne est MEASURED. Sans budget : rapporter mesure, pas succès inventé.
`DEFER_TO` lead puis propriétaire réel du goulot ; le reviewer ne modifie ni
sources ni tests. Auteur corrige en TDD ; reviewer fresh recontrôle après intégration.

## Sources et limites d'outillage

Rédaction originale, liens officiels seulement :
- [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html) : corrélation CPU/GPU, captures et limites plateforme.
- [GPUOpen performance](https://gpuopen.com/performance/) : méthodes/outils à contextualiser, pas preuve ARTIST.
- [Google Benchmark](https://github.com/google/benchmark) : candidat pour mesures
  CPU bornées, pas remplacement d'un benchmark rendu/image ou d'un oracle GPU.

Aucun profiler n'est supposé installé ; aucun install/download ni benchmark
automatique. Avant adaptation/copie d'un outil : version/commit exact, licence,
notices, plateforme, métriques et permissions vérifiés ; tree SHA ≠ commit.
Licence inconnue = lien seulement ; nom dans T02 n'est pas sélection de dépendance.
