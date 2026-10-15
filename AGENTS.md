# ARTIST — instructions OpenCode

## Cap et preuves

ARTIST vise un moteur de rendu 3D bas niveau, C++23, techniquement exigeant.
**Résoudre une décision aussi tôt qu'elle est réellement connue, pas avant.**
Structure et contrats statiques ; contenu préparé au cook ; état vivant en données
compactes ; dispatch dynamique amorti à la frontière des lots.

- Lire `docs/engineering-principles.md` pour les frontières et invariants.
- Pour corriger l'existant, consulter `docs/adr/README.md` et
  `docs/architecture/correction-plan.md` ; un ADR `Proposed` n'est pas encore
  une décision adoptée ni une fonctionnalité implémentée.
- Distinguer **intention**, **code observé**, **comportement exécuté** et **mesure**.
  Les rapports fournis et le README éclairent la cible, ils ne prouvent ni
  l'implémentation ni la performance. Le code actuel peut lui-même être un défaut
  à corriger, pas une architecture à perpétuer.
- `Pipeline<API, PROFILE>` ne prouve pas une exécution entièrement statique :
  suivre les appels, le stockage, l'ownership et, pour le coût réel, le code généré.
- Conserver les usages utiles de templates, concepts et `if constexpr`.
  Ne pas encoder chaque asset ou état de scène dans un type C++.
- Correctness, lifetime CPU/GPU et qualité du rendu précèdent les gains de vitesse.
  Aucun gain « zero-cost », SIMD ou GPU sans expérience reproductible.

## Style de travail

- Échanges en français ; identifiants et code cohérents avec les fichiers voisins.
  Conserver les conventions locales plutôt que renommer tout le dépôt pour
  appliquer les exemples parfois divergents de `GUIDELINE.md`.
- Partir du symptôme/diff, identifier les invariants, puis limiter l'exploration.
  Donner des chemins, symboles, commandes et extraits utiles, pas des dumps.
- Définir le périmètre et la preuve attendue avant une modification difficile.
  Corriger la cause ; ne pas masquer une régression par un mock ou une tolérance.
- Les hypothèses sont explicites. Après deux essais sans information nouvelle,
  changer d'expérience ou transmettre un diagnostic compact au spécialiste.
- Un travail local simple reste local. Une décision transversale mérite un contrat
  et, si elle doit durer, une note dans `docs/adr/`.

## Développement piloté par les tests (TDD)

- Fonctionnalités et corrections suivent **RED → GREEN → REFACTOR**, par petite
  tranche observable. Définir le contrat et les critères avant de modifier le code.
- **RED** : écrire le test de régression ou de contrat, puis constater son échec
  pour la raison attendue avant l'implémentation. Une toolchain absente, un contexte
  GPU indisponible ou zéro test découvert ne constituent pas cette preuve.
- **GREEN** : implémenter le minimum correct pour satisfaire le contrat. Ne pas
  affaiblir les assertions, tolérances ou références pour faire passer le test.
- **REFACTOR** : améliorer la structure lorsque nécessaire, puis relancer les tests
  concernés et les contrôles d'intégration pertinents sur le diff final.
- Choisir la preuve à la frontière testée : contrats de compilation positifs et
  négatifs pour concepts/types ; tests CPU pour le comportement ; contexte OpenGL
  réel pour le backend ; image ou mesure lorsqu'elles font partie du critère.
  Un test de compilation négatif doit rejeter le cas pour le motif attendu, avec
  une toolchain fonctionnelle ; les mocks ne remplacent pas une preuve GPU.
- Conserver les commandes, environnement, attendu/observé, nombres de tests et
  extraits utiles des phases RED/GREEN. Une phase non exécutée reste INCONCLUSIVE.
- Un refactoring pur s'appuie sur les tests existants ou de caractérisation avant
  restructuration. Documentation et configuration utilisent les contrôles adaptés
  (syntaxe, cohérence, découverte) sans fabriquer un échec comportemental.
- Le TDD ne démontre pas une performance ni un coût nul : ces critères exigent
  leurs expériences reproductibles.

## Documentation Obsidian et QMD

- **Collection QMD : `artist-wiki`** ; vault versionné : `docs/` ; accueil :
  `docs/index.md`. Identité portable de référence : `.wiki.json`.
- Toute recherche documentaire ARTIST utilise `collections: ["artist-wiki"]`
  et un `intent` explicite. Lire les notes via `get`/`multi_get` après la recherche.
- Maintenance : `docs/_meta/wiki-maintenance.md` ; vocabulaire :
  `docs/_meta/taxonomy.md`. Préserver frontmatter, wikilinks, sources et dates.
- Les quatre rapports bruts sont dans `research/`. Le code reste la preuve du
  comportement présent ; les rapports et ADR définissent ou proposent la cible.
- Actualiser cette documentation et les preuves des ADR avec chaque correction.
  Pour réindexer uniquement ce vault : `node .opencode/scripts/update-wiki.mjs`,
  puis `qmd embed -c artist-wiki`. Ne pas substituer un index `.qmd` local au MCP global.

## Routage

`orchestrator` coordonne et intègre ; il ne se redélègue jamais à lui-même.
La délégation est autorisée lorsqu'elle apporte une spécialisation ou une revue
indépendante. Les autres agents sont des feuilles, sans sous-délégation.

- `project-manager` : découpage exécutable, dépendances, parallélisme, estimation
  de complexité/risque ; planificateur en lecture seule.
- `engine-architect` : frontière statique/cook/runtime, API, render graph, ownership.
- `cpp-engineer` : implémentation C++23, concepts, templates, mémoire, concurrence.
- `gpu-engineer` : backend, passes, shaders, états et synchronisation GPU.
- `asset-engineer` : import/cook, formats, layouts, streaming et contrats de données.
- `build-engineer` : CMake, Conan, toolchains, compilation et outillage de tests.
- `correctness-reviewer` : invariants, UB, lifetime, erreurs et contrats observables.
- `shader-reviewer` : espaces, unités, précision, interfaces CPU/GPU et image.
- `performance-reviewer` : coût CPU/GPU, allocations, cache, spécialisation et mesures.
- `rendering-verifier` : vérification indépendante avec PASS / FAIL / INCONCLUSIVE.

Pour une mission Ensemble, charger `opencode-ensemble` avant la coordination.
Faire préparer le plan par `project-manager` ; pour une petite intervention
indivisible, l'orchestrateur peut appliquer directement son contrat de planification.
Rechercher systématiquement les branches indépendantes, y compris pour le harness,
les tests et la QA. Exécuter en parallèle les tâches prêtes sans conflit d'ownership
ni ressource partagée ; justifier les dépendances qui imposent une séquence.
La composition et la taille de l'équipe dépendent de la mission.

L'évaluation **PM/LLM** est le défaut ; **JEV direct** est un choix explicite par
mission. Le PM évalue avant publication et conserve méthode, estimation brute,
justification, incertitudes et scores retenus. Pour toute tâche de développement
(implémentation, correction, refactoring, code de test, shader, script ou harness
exécutable), appliquer **`complexity = max(70, estimation)`**, sur 100. Le risque
reste une estimation distincte ; ces scores ne sont pas des probabilités de succès.
Ne pas abaisser le plancher en rebaptisant une tâche « tests », « QA » ou « outillage ».

L'orchestrateur valide les fiches puis publie **les deux entiers `complexity` et
`risk` (0–100)** dans `team_tasks_add`. Ils évitent la classification automatique ;
ne pas attendre une notification JEV pour une tâche marquée `not_required`.
Fournir une affinité `agent`, les vrais IDs des dépendances déjà créées et une
décision `context` explicite. Le routage Ensemble choisit le modèle/effort autorisé
dans le catalogue opérateur ; ne pas le court-circuiter par un `model` explicite
sans demande utilisateur, ni modifier les scores du catalogue. Le PM et le travail
hors routage Ensemble conservent la sélection native de la session.

`autoDispatch` et `autoScale` sont activés dans `.opencode/ensemble.json` ; les
tâches publiées peuvent donc démarrer immédiatement. Publier seulement des tâches
exécutables et évaluées, avec un contexte décidé (`fresh` par défaut, requis pour
une revue indépendante). Réserver `pending` aux décisions de contexte ouvertes.
Les tâches dépendantes attendent les livrables réellement intégrés : le coordinateur
doit organiser cette barrière, pas seulement la fin du travail d'un auteur.

Un brief contient : objectif, fichiers/symboles, état du diff, invariants, liberté
locale, critères de réussite, commandes possibles, tentatives déjà faites, preuves
RED/GREEN attendues et ressources exclusives. Une tranche TDD garde un propriétaire
pour son cycle ; ses phases ne sont pas des tâches indépendantes à lancer en parallèle.
Des lectures indépendantes peuvent être parallèles. Un seul écrivain par fichier
et un seul build/indexeur par répertoire de sortie ; sérialiser les tests qui
partagent un contexte GPU ou des fixtures. Le coordinateur vérifie l'intégration.
Vérifier ce que reçoit chaque worktree : un diff non commité n'y est pas transmis
automatiquement. Les tests sont écrits par les implémenteurs ; les reviewers et
`rendering-verifier` conservent leurs restrictions de modification.
Un changement sensible (ownership/lifetime, concurrence, backend ou shader) exige
une revue ou vérification indépendante, avec contrat, diff et critères observables.
Si elle est indisponible, signaler la limite sans qualifier une auto-revue d'indépendante.

## Navigation et MCP

- **Context Mode** : utiliser `ctx_execute` / `ctx_execute_file` pour logs, builds,
  tests, réponses API et lectures volumineuses ; `ctx_batch_execute` pour les
  commandes indépendantes ; indexer les documents réutilisés. Renvoyer seulement
  les conclusions et les preuves nécessaires. Lire les petites portions à modifier
  directement et éditer avec les outils dédiés.
- Découvrir les signatures MCP dans le catalogue. Corriger les erreurs de script
  dans cette session. Si Context Mode est réellement indisponible, le signaler et
  demander l'accord avant de poursuivre sans lui. Ne pas installer son ancien plugin.
- **Serena** : `initial_instructions`, puis `get_current_config` ; confirmer la
  racine ARTIST avant les symboles. Ne jamais appeler `activate_project` sur une
  instance 1MCP partagée. Une connexion par racine ; pas de résultats AIForAll.
  Sans base de compilation fiable, qualifier les limites de l'analyse C++.
- **GrepAI** : recherche sémantique en anglais et traces sur l'index ARTIST.
  Vérifier fraîcheur et limites des traces de templates. Sans index/service,
  signaler la limite et revenir aux recherches textuelles ciblées.
- **QMD**, hérité globalement : collection documentaire `artist-wiki`.
  Toujours vérifier projet, fraîcheur, statut des décisions et preuves ; les
  autres collections ne sont utilisées que pour une comparaison explicitement utile.
- Configuration et utilisation : `docs/opencode-harness.md`. Les outils GPU
  externes ne sont considérés disponibles qu'après découverte effective.

## Repères réels et validation

- Bibliothèque header-only `ArtistLib` : `ARTIST/include/` ; backend implémenté
  OpenGL : `ARTIST/include/graphic/opengl/`.
- Tests unitaires GoogleTest/GoogleMock : `ARTIST/tests/`, cible `ArtistLib_tests`.
- Tests OpenGL réels : `ARTIST/component-tests/`, cible
  `ArtistLib_component_tests`, contexte GLFW/GLEW 3.3 core.
- Build : `CMakeLists.txt`, `CMakePresets.json`, `conanfile.txt` et
  `.github/workflows/cmake-single-platform.yml` font foi pour les commandes présentes.
  Charger `artist-build-validation` avant de configurer/compiler/tester.
- C++23, CMake >= 3.25, Conan **2** (`CMakeDeps` / `CMakeToolchain`), installé
  avant CMake. Baseline vérifiée : Windows x64 Debug, MSVC 19.44 / toolset 14.44,
  Conan 2.12.2, 109/109 tests dont 22 OpenGL. Voir `docs/development/build-and-test.md`
  et ADR-0008. Relance : `scripts/build-windows.ps1` (un job par défaut).
- Après la migration Shader→Pass : 122/122 tests Windows Debug (95 unit, 27
  OpenGL), JUnit recoupés. ADR-0003 reste Accepted/partial ; ASan et l'assignation
  move vers owner déjà chargé ne sont pas validés. Reprise :
  `docs/references/session-2026-10-03-shader-pass.md` et
  `docs/development/known-incidents.md` ; empreintes et commandes dans
  `docs/_meta/session-2026-10-03-validation.json`. Ne pas confondre ces résultats
  avec une preuve de RAII exhaustive ou de performance.
- `cmake/CheckCxx23.cmake` vérifie `std::format` et `std::stacktrace` au compile/link.
  GCC MSYS2 13.2 installé ne fournit pas ce dernier ; autres plateformes, Release
  et CI distante ne sont pas encore validés. Ne pas déduire la STL du numéro GCC.
- Choisir les tests qui prouvent le changement : contrats compile-time,
  comportement CPU, contexte GPU réel, image ou mesures selon le périmètre.
  Un test mocké ne prouve pas un rendu ; zéro test découvert n'est pas un succès.
- **PASS** : critère vérifié par une preuve ; **FAIL** : écart reproduit ;
  **INCONCLUSIVE** : précondition ou preuve manquante. Toujours préciser le périmètre.
