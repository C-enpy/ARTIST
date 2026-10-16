# Contrat de graphe et expériences discriminantes

Rédaction originale, 2026-10-16 ; recette conceptuelle, pas résultat d'un compilateur
de graphe ARTIST. Aucune adoption nouvelle des ADR-0001/0002/0004.

## Trois objets et quatre moments de décision

- **Graphe authoring** : passes, accès versionnés, imports/exports, effets externes,
  contraintes et liberté d'édition. Les noms servent au diagnostic ; ne pas
  présumer qu'ils deviennent tous des types C++ ou des lookups de frame.
- **Plan cook immuable** : schéma/versions de ressources, indices/bindings préparés,
  dépendances, variantes et stratégie d'allocation. Ce qui dépend du device ou de
  la résolution inconnus reste paramétrique ou est préparé plus tard. Un cooker
  offline ne peut fabriquer des handles driver universels.
- **Instance runtime-frame** : imports effectifs, dimensions, paramètres, objets
  driver, complétions, résidences, événements et ressources encore en vol.

Compile : structure/traits bornés et validation de types. Link : instanciations,
visibilité et élimination de code selon toolchain, sans performance présumée.
Cook : calcul stable connu. Runtime : état vivant et choix tardif par lots.
Cette séparation ne signifie pas qu'un render graph est toujours entièrement
précompilable. Invariants et contenu de scène ne doivent pas être confondus.

## Exemple nominal à spécifier avant implémentation

Entrées : scène/caméra fixées, depth D0 non initialisé, HDR H0 non initialisé,
sortie E importée, frame N. Une pass shadow écrit D1 ; opaque lit D1 et écrit H1 ;
post lit H1 et écrit E. Export E ; aucun accès implicite à H0.

Oracle : edges shadow → opaque (depth sampled), opaque → post (HDR sampled) ;
format/usage de D1 et H1 compatibles. Résultat observable E conservé. Le plan
indique chaque sous-ressource/stage et dernier utilisateur, pas « barrière partout ».
Dans OpenGL, la traduction ne ressemble pas nécessairement aux transitions d'une
API explicite : vérifier l'opération réellement productrice et le type d'accès.

Tests négatifs proposés, à écrire dans le vrai composant lorsqu'il existe :

1. post lit H1, opaque absent, aucun import H1 : erreur `read-before-produce`,
   identifiant post/H1 dans le diagnostic, aucune soumission GPU.
2. shadow lit X1 produit par post, post dépend de opaque, opaque de shadow :
   erreur de cycle, chemin témoin shadow → opaque → post → shadow.
3. Deux writers de H1 sans ordre/version distincte : ambiguïté rejetée, pas ordre
   arbitraire choisi par hash-map. Un read-modify-write exige son entrée initialisée.
4. Historique Hprev correctement importé : accepté ; import absent : rejeté.
   Le cycle temporel est entre frames, pas une arête cyclique du DAG de la frame.
5. Pass marquée sans sortie mais écrivant une query/callback/export : son effet
   interdit le culling. Oracle : effet conservé ; pas simple nombre de passes.

## Scheduling, fusion et allocation

Établir les contraintes puis choisir le scheduling. Queue unique : un intervalle
de vie entre première et dernière utilisation peut suffire au modèle. Multi-queue :
utiliser l'ordre partiel **happens-before**, pas les positions d'une liste
topologique. Pour aliasing A/B, tous les derniers utilisateurs de A doivent
précéder la première utilisation de B, ou inversement, et l'API doit permettre
les allocations aliasées (taille, alignement, type mémoire, usage, format).

Contre-exemple : G0 écrit A, K1 lit A en async compute ; G2 écrit B sur le même
heap sans arête K1 → G2. L'intervalle CPU G0..G1 est trompeur : aliasing interdit.
Deux voies à examiner : allocation distincte ou arête K1 → G2 avec coût mesuré.
Pas de fallback « wait all » silencieux ; pas d'aliasing d'un export encore vivant.
Après aliasing légal, initialiser B selon contrat, effectuer la transition/barrière
API nécessaire ; l'ancien contenu n'est pas garanti par le simple partage de heap.

Fusion de passes : mêmes contraintes d'attachments/samples et sous-ressources,
load/store/clear/resolve équivalents, pas de dépendance externe brisée. Examiner
lectures intermédiaires, queries et debug labels. Une fusion ne prouve un gain
qu'avec workload/image identiques et protocole `artist-performance`.

## Clé de cache, rebuild et retrait

Clé minimale justifiée : schéma du graphe, source/options/ABI shaders, versions
des cookers, capacités/format/usage cibles, contraintes de scheduling et dimensions
qui affectent réellement le plan. Définir quoi invalide quelle couche, plutôt que
tout reconstruire par réflexe. Paramètre d'exposure ≠ nouveau schéma de graphe.

- Résolution change : redimensionner ressources dépendantes ; invalider le plan
  physique concerné. Conserver le plan et allocations de N tant que N est en vol.
- ABI/binding change : invalidation shader/pipeline et bindings/plans consommateurs.
  Ne pas réutiliser un cache uniquement parce que les noms de ressources concordent.
- Device recréé/capacité change : objets driver et tokens de complétion anciens
  invalides ; revalider variantes/allocations, expliciter ce qui doit être recooké.
- Publication atomique du nouveau plan prêt ; retirer l'ancien après dernières
  utilisations sur toutes queues et expiration des emprunts CPU. Génération utile
  contre les handles périmés, jamais comme attente GPU.

## Sources et preuve à produire

Contrats locaux : `docs/engineering-principles.md`, registre ADR et correction-plan
font foi pour intention/statuts ; inspection du code requise pour l'existant.
Liens officiels sur les mécanismes traduits, pas import d'un graphe tiers :

- [Vulkan, synchronization](https://docs.vulkan.org/spec/latest/chapters/synchronization.html).
- [D3D12, aliasing et autres resource barriers](https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12).

Aucune licence tierce attribuée à cette rédaction. Copie/adaptation future exige
licence/notices, chemin et commit exact (pas tree SHA). Le corpus T02 est un audit,
pas une autorisation de copie. Les cas ci-dessus sont des oracles proposés :
preuves CPU du compilateur, traduction GPU, image et performance restent à obtenir
séparément. Pas de faux test de graphe annoncé parce que le Markdown contient DAG.
