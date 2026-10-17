# Contrat source → IR → cook → runtime

Méthode de conception, pas format binaire ni API ARTIST déjà adopté. Spécifier
les valeurs retenues par tâche avant code ; refuser les conventions implicites.

## Source et import

Conserver identité persistante, chemin logique/provenance/licence, bytes source
et hash, format/version, dépendances externes et leur contenu. Définir unités
(longueur, secondes, radians), repère/handedness/axes, winding, espace objet/world,
convention UV/origine, colorimétrie et précision. Convertir une seule fois ;
signaler l'unité/espace inconnu plutôt que supposer une convention moteur.

L'import produit une **représentation intermédiaire** explicite, indépendante
du driver : tableaux de données, topologie, attributs typés et métadonnées.
Valider nombres finis, indices/ranges/strides, profondeur/compteurs et budgets
avant allocation. Bloquer références cycliques ou externes hors périmètre ; ne
pas exécuter scripts/chemins fournis par l'asset. Décompression sous limites de
bytes décodés et travail, pas seulement sous limite de taille compressée.

## Cook, stockage et clé de cache

- Cook immuable : ordre de parcours stable, options canonisées, seed déclarée,
  compression/quantification déterminées, règles NaN/-0/padding explicites.
  Ne pas inclure timestamp, adresse, chemin machine ou ordre de hash-map dans
  le produit. Borner les permutations ; aucune spécialisation par asset C++.
- Proposer un schéma indépendant de l'ABI C++ : magic/type, version majeure et
  mineure, taille d'en-tête, taille totale, table de sections typées, format et
  taille/compte/stride de chaque payload, dépendances et empreintes d'intégrité.
  Pour un nouveau profil, proposer endianness little-endian, entiers u32/u64 à
  largeur fixe et IEEE-754 explicite, puis faire adopter ces choix. Une lecture
  big-endian convertit ou refuse ; jamais un dump de struct/pointeurs/vtable.
- Déclarer alignement de chaque section/élément dans le schéma (puissance de
  deux, padding initialisé). Ce n'est pas l'alignement/pitch d'un upload device.
  Offsets relatifs au début du fichier, représentables sur la plateforme ; aucun
  cast direct non aligné. Versions incompatibles/sections obligatoires inconnues
  refusées, migration explicite hors hot path.
- Valider `offset <= taille` puis `longueur <= taille - offset`, additions,
  produits `compte * stride`, arrondis d'alignement et conversions checked avant
  usage. Rejeter overlap interdit, section tronquée, compteur incohérent, tailles
  au-delà du budget. Une checksum correcte ne remplace pas ces validations.
- La clé de cache identifie le contenu source ET les dépendances transitives,
  version importeur/cooker/codec, options canonisées, profil cible, format/ABI
  du consommateur, conventions et version de schéma. Changement d'un élément
  invalide les produits dépendants ; l'identité persistante peut rester stable.
  Écriture temporaire puis publication atomique : interruption ne rend jamais
  visible un produit partiel. Vérifier intégrité/compatibilité à la lecture.
- Réflexion/bindings, indices/mips/layouts stables sont préparés ici. Les PSO
  natifs gardent les clés/contraintes de cache du device, driver et de l'API :
  un bytecode portable n'est pas un PSO réutilisable partout.

## Runtime : frontière avec les ressources GPU

Séparer identité persistante (asset logique), révision cookée (contenu),
handle générationnel (référence runtime invalidable), résidence (non résident/en attente/
partiel/prêt/échec/retrait) et objet GPU (allocation/binding propriétaire device).
Ces états sont un modèle proposé, pas des noms de méthodes existantes.

Transmettre au propriétaire ressources payloads validés, tailles/pitch, formats,
usages, sous-ressources, dépendances, budgets CPU/GPU et conditions de publication.
Ne pas publier « prêt » avant complétion/visibilité requises pour le consommateur.
L'asset-engineer ne choisit pas une barrière universelle ni un allocateur GPU.

Une requête porte identité/révision/génération/token : l'annulation avant
soumission retire le travail ; après soumission, elle interdit sa publication
mais ne libère pas la staging memory encore lue. Le remplacement prépare une
nouvelle révision, publie au point sûr, et écarte les complétions anciennes.
La libération différée de staging et anciens objets dépend de la fin réelle des
dernières utilisations sur **toutes les queues** concernées et des emprunts CPU.
Une génération changée n'attend pas le GPU. Traiter erreurs et destruction du
device explicitement avec le propriétaire, pas en recyclant un slot en vol.

Poses/temps/poids, particules, cloth/hair et physique restent état runtime ;
sampling/blending/skinning et solveurs ont kernels bornés, scheduling et
synchronisation explicites ; pas un type par clip, emitter ou instance.

## Oracle, coûts et transfert

Nominal : fixture minuscule versionnée, import→IR→cook→lecture avec mêmes valeurs
dans la précision annoncée ; round-trip bytes seulement si le format est sans
perte et canonique. Deux cooks mêmes entrées/options donnent le même hash dans
l'enveloppe toolchain/codec déclarée ; déterminisme inter-plateformes à tester
séparément. Test négatif : overflow, section hors fichier, version inconnue,
annulation/remplacement en vol, dépendance changée : rejet/invalidation attendu.

Coûts à relever : I/O + validation O(sections + éléments), décompression/transcode,
copies de staging, création device et publication ; pic CPU/GPU incluant ancienne
et nouvelle révision. Aucune promesse zero-copy, temps constant ou gain sans mesure.
`DEFER_TO` lead → ressources pour fixtures en vol réelles ; sans loader/cooker,
ces oracles sont proposés, non exécutés et INCONCLUSIVE pour le moteur.
