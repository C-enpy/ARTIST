---
title: "ADR-0003 — Ownership et lifetime CPU/GPU explicites"
adr_id: ADR-0003
category: adr
tags: [artist, cpp, ownership, rendering, adr]
aliases: [ADR-0003]
status: Accepted
implementation: partial
decision_origin: Analyse des contexts partagés et recommandation commune des rapports ; adoption explicite par l'utilisateur le 2026-10-03 avec une migration franche (rien n'est en production).
summary: Séparer propriétaires et emprunts du chemin d'appel, définir invalidation et libération conforme aux règles du backend.
created: 2026-10-03
updated: 2026-10-15
---

# ADR-0003 — Ownership et lifetime CPU/GPU

## Contexte et faits

Au commit de départ `887548b`, les contexts de `Pipeline`, `Pass`, `Shader`, `Uniform` et `Attribute`, dans
`ARTIST/include/graphic/pipeline/`, utilisent des `shared_ptr`. Plusieurs
composants OpenGL reçoivent également leur context par `shared_ptr` en valeur.
`IShader` dispose d'un destructeur par défaut et d'un `free()` distinct : la
gestion d'un pointeur C++ ne prouve pas la libération correcte d'un objet driver.

Ce constat ne démontre pas que chaque partage est inutile ou coûte une allocation
à chaque appel. Il exige d'expliciter le graphe des propriétaires et des emprunts.

## Décision adoptée

- Un owner identifiable pour chaque ressource native et chaque stockage mutable.
  Utiliser valeur/owner unique quand le domaine le permet ; garder un partage
  seulement si plusieurs propriétaires indépendants sont nécessaires.
- Les opérations qui n'étendent pas le lifetime empruntent `Context&`, `const&`,
  vue/span ou handle validé selon le contrat. Documenter toute rétention asynchrone.
- Définir copies, déplacements, état vide/moved-from, échecs partiels et répétition
  de libération. Un owner natif ne doit pas se dupliquer implicitement avec un handle.
- Définir l'invalidation des vues et handles : réallocation, reload, fin de frame,
  réutilisation d'un slot ; utiliser une génération lorsque ce risque existe.
- La destruction respecte contexte/thread et règles du backend. Pour OpenGL,
  tenir compte de ses règles propres de suppression/références différées ; pour
  une API explicite future, établir les conditions de retraite/fence requises.
  Ne pas ajouter une attente GPU globale à chaque destruction par défaut.
- Un wrapper RAII peut posséder la ressource ou déléguer une requête de libération
  à un owner de device dont le lifetime est garanti. Son destructeur ne suppose
  pas magiquement un contexte valide et ne laisse pas échapper d'exception.

## Migration

Choisir la tranche de [[adr/0002-preparation-et-execution-specialisee|ADR-0002]],
dessiner création → emprunts → dernières utilisations CPU/GPU → destruction,
puis migrer les signatures internes avant les adaptateurs publics. Traiter
explicitement les copies de wrappers et contexts qui partagent déjà une ressource.

### Tranche Shader → Pass — réalisée le 2026-10-03

Migration **franche** (décision explicite : rien n'est en production, aucune
compatibilité conservée). Le relevé a tranché sur le point ouvert : le programme
GL appartient au `Pass`, l'objet shader au `Shader`.

- `IShader<API>` possède son contexte via `std::unique_ptr<typename API::ShaderContext>` ;
  non-copiable, movable ; `getContext()` retourne une **référence** d'objet possédé
  (jamais null, jamais sur un objet moved-from) ; le destructeur de la base ne
  redescend pas dans l'override pur virtuel — la libération RAII est portée par
  `~Shader` (côté dérivé).
- `PassContext::m_shaders` : `std::vector<std::unique_ptr<IShader<API>>>` — owner
  unique : le conteneur ; `addShader(std::unique_ptr<…>)` avec rejet de `nullptr` ;
  `getShader(i)`/`getShaders()` retournent des emprunts.
- Composants OpenGL `Loader/Freer/Reader/ShaderAttacher/UniformReader/AttributeReader/User`
  : signature `on(Context&)` (emprunt) ; `ShaderFreer` remet `shaderID=0` ;
  `PassFreer` remet `passID=0` ; `Loader` remet `passID=0` à l'échec de link.
- `Pass::shared()` supprimé (un clonage implicite de `Pass` partageait un seul
  programme GL) ; `withUniform` retourne `IPass&` ; wrappers non-copiables, movables.
- `HasBorrowedOnMethod` (concept dédié, référence) pour les concepts Shader/Pass GL ;
  Uniform/Attribute restent sur `HasOnMethod` (shared_ptr), hors tranche.
- `IPass::m_context` reste un **`shared_ptr`**, malgré l'entrée `unique_ptr` de
  son constructeur. `getContext()` peut étendre le lifetime du contexte et des
  shaders au-delà du wrapper ; cette déviation est nommée, pas une cible adoptée.

### Rechargement — contrat strict B

Avis `engine-architect` retenu : les loaders OpenGL rejettent un handle non nul
avant leur `glCreate*`, avec `ERROR::{SHADER,PASS}::RELOAD_WITHOUT_FREE`.
Ils ne remplacent pas implicitement les ressources. La valeur du handle est une
précondition **runtime de préparation**, pas un état résolu à la compilation.
L'alternative de remplacement automatique est écartée : elle exige de choisir
destroy-first ou remplacement transactionnel avec rollback, non requis ici.

`shader.free(); shader.load()` permet une nouvelle compilation. `pass.free()`
ne libère pas les shaders : pour un cycle complet, libérer le programme et
chaque shader, puis appeler `pass.load()` sous un contexte GL valide.
La non-mutation testée concerne **chaque loader**, pas l'ensemble d'`IPass::load()` :
ce dernier charge d'abord les shaders et peut rejeter avec l'erreur shader avant
l'erreur pass. Aucun rollback global ou relink transactionnel n'est prouvé.

## Alternatives et conséquences

Le partage généralisé simplifie certaines signatures mais masque les owners et
peut ajouter refcounts/aliasing. Les pointeurs bruts généralisés suppriment ces
coûts visibles sans résoudre use-after-free ou invalidation. Les handles rendent
le stockage plus libre, au prix d'un contrat de validation et de réutilisation.

## Critères de validation

- Création échouée à chaque étape : état cohérent et nettoyage des acquisitions.
- Déplacement, destruction, tentative d'accès périmé et libération répétée :
  comportement documenté et testé ; pas de double libération.
- Pour les usages asynchrones introduits : ressource valide jusqu'à sa dernière
  consommation ; réutilisation des IDs sans alias accidentel.
- Vérification CPU/sanitizers supportés et test driver du cycle de vie pertinent.
  Les mocks ne prouvent pas la conformité du contexte de destruction.

**Preuves du 2026-10-03 — tranche Shader→Pass, Windows Debug (MSVC 19.44)** :

- `ShaderLifetimeTests.cpp` : 14 tests mockés — non-copiables/movables (`static_assert`),
  `free()` idempotent (double `glDeleteShader` impossible), moved-from sans libération,
  contexte null rejeté à la construction (`ERROR::SHADER::NULL_CONTEXT`,
  `ERROR::PASS::NULL_CONTEXT`, `ERROR::PASS::NULL_SHADER`), détach uniquement des
  shaders encore vivants, idempotence après `passID=0`, ordre de destruction
  detach → deleteProgram → deleteShader.
- `ShaderLifetimeComponentTests.cpp` : 5 tests sur le driver réel — load/free
  idempotent, move transfère le handle, compile en échec sans handle résiduel,
  cycle complet du pass (attach, use, free idempotent), rejet de re-load shader
  et nouveau chargement après `free()`. Les tests de déplacement portent sur
  le move-constructor, pas sur l'assignation vers un objet déjà chargé.
- Suites : **95/95 unit** et **27/27 OpenGL** PASS (0 fail, 0 skip, 0 timeout,
  0 hang) ; anti-ODR corrigé (fixtures renommées `ShaderLoaderTests`,
  `PipelineUserTests` ; collisions ODR de fixtures corrigées).
- Revue indépendante (`correctness-reviewer`) : **verdict PASS de lecture** sur
  l'état précédant les corrections F1/F2/F3 ; pas de re-test indépendant. Should-fix
  appliqués et verrouillés par tests — handles GL `= 0` par défaut (défaut
  indéterminé éliminé), gardes moved-from unifiées (`ERROR::PASS::FREED_CONTEXT`
  côté `Pass`), re-load **variante B** (avis `engine-architect`) : un `load()`
  sur handle vivant rejette `ERROR::SHADER::RELOAD_WITHOUT_FREE` /
  `ERROR::PASS::RELOAD_WITHOUT_FREE` avant toute création (`glCreate* Times(0)`
  testé, handle non muté) ; recompiler = `free(); load()`.
- Sanitizers : **INCONCLUSIVE** — pas exécutés dans ce périmètre ; l'expérience ASan
  `/fsanitize=address` MSVC restent à instruire (cf. [[development/build-and-test|guide]]).
- Prochaine tranche (avis `engine-architect`, à instruire) : la conversion
  `PipelineContext::m_passes` → `vector<unique_ptr<IPass>>` + `forPass` par
  emprunt d'abord, puis `IPass::m_context` → `unique_ptr` ; durcir
  `UniformReader` dans `OpenGLPassFlow` compile-time lors de la sous-tranche
  uniform/attribute. `Uniform`, `Attribute`, `Pipeline` restent `shared_ptr` hors périmètre.
- Limites supplémentaires : assignation par déplacement vers un owner chargé,
  bornes d'index, logs GL vides et lecture de fichier manquant à qualifier.
  La validité du contexte/thread de destruction reste une précondition du
  consommateur, pas une garantie automatiquement fournie par RAII.

Sources de reprise : [[references/session-2026-10-03-shader-pass|conversation et sous-agents]],
[[development/known-incidents|incidents, corrections des diagnostics et suivi]].
Empreintes des JUnit et logs finaux : `_meta/session-2026-10-03-validation.json`.

**Revérification du 2026-10-15** : code committé en `4d29842`, build existant et
122/122 tests PASS ; voir `_meta/precommit-2026-10-15-validation.json`.

### Assignation move vers owner chargé — vérifiée le 2026-10-15

La [[architecture/missions/2026-10-15-loaded-owner-move|mission loaded-owner move]] est terminée dans son périmètre Windows
x64 Debug. HEAD principal `e589aee` + diff D `0bbca504` et H `a423fc25` intégrés,
15 fichiers empreintés ; voir la
[[references/session-2026-10-15-loaded-owner-move|synthèse et les commandes]] et
`_meta/session-2026-10-15-loaded-owner-move-validation.json`.

- Contrat approuvé après avis `engine-architect` : assignation publique seulement
  entre owners concrets identiques ; bases `protected`, `Shader`/`Pass` `final`.
  Cette restriction d'API est volontaire, sans shim ni héritier concret trouvé.
- Self-move strictement sans effet ; cleanup de l'ancienne destination par son
  hook concret avant transfert. Source vidée, pas de double libération ni de
  redescente virtuelle depuis une base détruite. Constructeurs move `noexcept`,
  assignations potentiellement levantes : les composants n'imposent pas `noexcept`.
- Échec cleanup : source non consommée, pointeurs conservés, garantie de base
  seulement ; un detach déjà effectué n'est pas annulé. Les destructeurs restent
  non-throwing et un cleanup qui y jette peut toujours terminer le processus.
- `IPass::m_context` reste partagé : un alias de l'ancien contexte conserve
  stockage/shaders avec `passID=0`, pas l'ancien programme vivant ni le nouveau
  contexte. Dernier alias et cleanup exigent un contexte GL compatible courant.
  La suppression d'un programme courant peut être différée jusqu'à l'unbind.
- RED causal CPU et driver avant fix ; 7 probes de compilation positives/négatives.
  Revue finale indépendante : PASS de lecture. Validation indépendante intégrée :
  **106 unit + 31 OpenGL = 137/137**, zéro skip/échec ; lifetime **25 + 9**.
- ASan séparé : probe sain 0/faute 86 identifiée, **36/36 TU instrumentées**,
  **106/106 CPU + 9/9 lifetime GL**, 115 cas uniques. Les 22 autres GL ne sont pas
  qualifiés sous ASan. Conan GLFW/GLEW, système et driver non instrumentés ; pas
  de preuve de fuite driver, d'image ou de performance. Options moteur fail-closed
  consignées ; le script ne normalise pas un environnement ASan parent arbitraire.

`implementation: partial` reste inchangé : Pipeline, Uniform/Attribute, contexte
Pass encore partagé et autres chemins/plateformes ne sont pas fermés par ces tests.
Aucun autre ADR `Proposed` n'est adopté implicitement.

**F5 exécuté puis archivé** : [[architecture/missions/2026-10-15-f5-gl-diagnostics|diagnostics GL vides et nettoyage sûr]],
sélection des chemins d'erreur après avis PM/architecte — RED causal, correction
bornée des deux loaders et validation indépendante (143/143 distincts, 121 ASan
distincts). Cette coupe ne modifie pas `implementation: partial` et ne démarre
pas Pipeline.

## Références

[[development/cpp-guidelines|Conventions]] · [[architecture/current-state|État réel]] ·
[[adr/0004-layouts-types-et-donnees-preparees|Bindings]] · [[adr/README|Registre]].
