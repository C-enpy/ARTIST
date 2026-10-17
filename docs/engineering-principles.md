---
title: Cap technique d'ARTIST
category: architecture
tags: [artist, architecture, cpp, performance]
summary: Philosophie multi-étapes, séparation préparation/exécution, choix de langage et limites des promesses de staticité.
created: 2026-10-03
updated: 2026-10-17
---

# Cap technique d'ARTIST

## Intention

Construire un moteur de rendu 3D bas niveau techniquement exigeant, en conservant
l'ambition de spécialisation et de validation précoce du C++23.

> Résoudre toute décision aussi tôt qu'elle est réellement connue, pas avant.
> Structure statique, contenu cooké, état dynamique, frontière amortie par lots.

Cette note traduit les [[references/research-reports|quatre rapports utilisateur]]
fournis le 2026-10-03, y compris les comparaisons C++23/Rust et scene-to-code.
Les originaux sont conservés dans `research/`, hors du vault et de sa collection
QMD. Leurs estimations, bibliothèques suggérées et exemples ne sont ni des mesures
locales, ni une roadmap engagée, ni des fonctionnalités déjà réalisées.

## Quatre moments de résolution

- **Compilation C++** : structure algorithmique fermée, traits backend, layouts
  connus, concepts, kernels SIMD et variantes réellement utiles et bornées.
- **Édition de liens** : instanciations explicites, visibilité inter-TU, élimination
  de code mort, dévirtualisation et LTO selon la toolchain. PGO repose sur un
  workload représentatif ; il ne remplace pas un bon modèle de données.
- **Cook/préparation** : contenu stable, conversion/optimisation de meshes et
  textures, compression, réflexion, IDs, permutations, descriptions de ressources
  et plans dont les entrées sont connues. Le coût est déplacé, pas aboli.
- **Exécution** : visibilité, instances, poses, états des particules et simulation,
  résidence, scheduling et objets propres au GPU/driver. Ces données restent
  dynamiques même lorsqu'un kernel spécialisé les traite.

Un graphe artistique peut être data-driven et aplati avant exécution. Une
configuration réellement fermée peut être entièrement statique. Aucun de ces
deux cas n'oblige à convertir chaque matériau, animation ou asset en type C++.
Les dépendances variant par frame ne sont pas magiquement connaissables au cook.

## Préparer librement, exécuter avec un contrat précis

L'API d'authoring/préparation peut employer noms, descriptions flexibles et
polymorphisme. Le chemin d'exécution migré utilise plans, bindings et données
validés, avec kernels spécialisés et décisions amorties par lots. Le rapport R4
précise ainsi qu'il ne faut pas supprimer brutalement toutes les interfaces.
Voir [[adr/0002-preparation-et-execution-specialisee|ADR-0002]].

La génération d'une fonction par objet peut remplacer de la pression sur les
données par de la pression sur les instructions. Une expérience scene-to-code
est légitime, mais bornée et comparée à un runtime compact, à qualité égale ; son
gain n'est pas présumé. Voir [[adr/0006-preuves-performance-et-determinisme|ADR-0006]].

Le runtime demeure C++23 dans l'orientation proposée ; un futur cooker peut être
évalué séparément en Rust ou C++, avec formats indépendants du langage. Une
réécriture ne se justifie pas avant correction et mesure de la baseline actuelle.
Voir [[adr/0007-cpp23-et-cooker-independant|ADR-0007]].

## Discipline de conception

1. **Spécialiser les algorithmes, borner les variantes.** Conserver templates,
   `requires`, concepts, `constexpr`/`consteval` et `if constexpr` lorsqu'ils
   suppriment du travail ou expriment un invariant réel. Mesurer aussi taille
   binaire, instanciations et build ; l'inlining forcé n'est pas un objectif.
2. **Déplacer le dispatch hors de la boucle fine.** Une sélection par lot suivie
   d'un kernel monomorphe est différente d'un appel virtuel par objet/attribut.
   Décrire fréquence et travail amorti, plutôt qu'interdire un mot-clé partout.
3. **Représenter la propriété réelle.** Valeurs, owner unique, emprunts, spans,
   pools/arenas et handles selon le domaine. Partage seulement s'il est requis.
   Un handle doit avoir une politique d'invalidation ; la destruction GPU doit
   respecter la fin réelle d'utilisation, pas seulement le scope C++.
4. **Préparer le chemin chaud.** Favoriser données denses, accès prévisibles,
   allocations contrôlées et IDs/layouts résolus. Strings, réflexion et parsing
   gardent leur place dans l'outillage et la préparation lorsqu'ils sont utiles.
5. **Rendre les contrats graphiques explicites.** Formats, alignements, espaces,
   unités, précision, colorimétrie, contextes, états et visibilité mémoire font
   partie de l'API. Un appel qui compile peut être faux pour le driver ou l'image.
6. **Évaluer le travail total.** Latence et débit CPU/GPU, qualité visuelle,
   mémoire/cache, synchronisation et soumission comptent davantage que le nombre
   de fonctions. Comparer à une baseline runtime optimisée, à qualité équivalente.
7. **Progresser par tranches verticales en TDD.** Pour une fonctionnalité ou une
   correction : contrat → test en échec observé (RED) → implémentation minimale
   (GREEN) → refactoring et non-régression. Choisir dès le contrat la preuve utile :
   compilation positive/négative, CPU, contexte GPU, image ou mesure. Un refactoring
   pur s'appuie sur les tests existants ou de caractérisation. Voir le
   [[opencode-harness#Missions, évaluation et TDD|workflow du harness]]. Intégrer une
   bibliothèque mature peut servir l'excellence ; la réécriture exhaustive n'est
   pas une preuve.

## Photographie du code après la tranche Pipeline A1+A2

À revérifier lors de chaque intervention : cette section n'impose pas de conserver
les écarts constatés. La photographie détaillée, avec sa révision, est dans
[[architecture/current-state|État réel d'ARTIST]].
Le code moteur observé est celui de `f914d77` ; `95c2b9e` ne change que le
harnais Python. L'amendement de l'oracle Attribute est en cours, pas encore
intégré ni vérifié ; les décomptes CTest ne masquent pas son FAIL préexistant.

- `ARTIST/CMakeLists.txt` crée `ArtistLib` en `INTERFACE` : bibliothèque header-only.
- `ARTIST/include/graphic/pipeline/Pipeline.hpp` combine `IPipeline` virtuel et
  `Pipeline<API, PROFILE>` avec validation/sélection statique de composants.
- `ARTIST/include/graphic/context/PipelineContext.hpp` stocke les passes dans un
  `vector<unique_ptr<IPass<API>>>` ; la collection demeure résolue à l'exécution
  et `forPass` l'emprunte. Les wrappers Pipeline et Pass possèdent aussi leur
  contexte de façon unique ; leurs accesseurs exposent des références empruntées.
- `ARTIST/include/graphic/context/PassContext.hpp` possède ses shaders par
  `unique_ptr`, mais conserve les uniforms/attributes partagés dans des maps
  indexées par chaînes, hors de cette migration.
- `ARTIST/include/graphic/context/AttributeContext.hpp` stocke une valeur dans
  `std::any`, puis récupère un `shared_ptr<T>` par cast runtime.
- `ARTIST/include/graphic/opengl/` fournit le backend présent. Les noms d'autres
  API dans des commentaires ou le README n'établissent pas leur implémentation.
- `ARTIST/tests/` et `ARTIST/component-tests/` distinguent mocks CPU et contexte
  OpenGL réel. Ils ne forment pas à eux seuls une suite de benchmarks ou de
  régression d'images pour un moteur complet.

Donc : la sélection statique d'un composant est réelle ; la promesse d'un pipeline
entièrement aplati et sans overhead reste à démontrer. La présence d'interfaces
ne prouve pas non plus un coût donné sans compilation optimisée et mesure.

## Axes futurs, sans présumer leur réalisation

Cooker et streaming, animation/skinning, particules, physique, hair/cloth peuvent
suivre le même contrat : données préparées, état runtime, kernels spécialisés.
Le choix des formats, dépendances, backend suivant et budgets appartient à des
travaux bornés avec critères observables. Ne pas construire un éditeur ou un
système de plugins généraliste uniquement parce qu'un autre moteur en possède.

Les [[adr/README|premiers ADR]] proposent les contrats de correction ; leur statut
et leur état d'implémentation restent deux informations distinctes. Suivre
[[architecture/correction-plan|l'ordre de correction]] pour établir des preuves
avant de généraliser.
