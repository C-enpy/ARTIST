---
title: Incidents connus et suivi Shader→Pass
category: development
tags: [artist, cpp, ownership, testing, tooling]
summary: "Incidents Shader→Pass/Pipeline et harnais, oracle Attribute corrigé b9c90fd, fragilités de shuffle Loader/ShaderLoader préexistantes conservées ; preuves historiques et chemins encore non couverts distingués."
created: 2026-10-03
updated: 2026-10-19
sources: [docs/references/session-2026-10-03-shader-pass.md, docs/_meta/session-2026-10-03-validation.json, research/sessions/2026-10-15-loaded-owner-move/session-capture.md, research/sessions/2026-10-15-loaded-owner-move/R-review.md, docs/_meta/next-mission-2026-10-15-selection.json, docs/references/session-2026-10-15-f5-gl-diagnostics.md, docs/_meta/session-2026-10-15-f5-gl-diagnostics-validation.json, docs/architecture/missions/2026-10-15-failure-contracts.md, docs/_meta/session-2026-10-15-failure-contracts-validation.json, docs/architecture/missions/2026-10-16-pipeline-ownership.md, docs/_meta/session-2026-10-16-pipeline-i-a1-validation.json, docs/_meta/session-2026-10-16-pipeline-a1c-validation.json, docs/_meta/session-2026-10-16-pipeline-i0-validation.json, docs/_meta/session-2026-10-16-pipeline-final-validation.json, docs/_meta/session-2026-10-16-asan-caller-restoration.json]
provenance:
  extracted: 0.96
  inferred: 0.03
  ambiguous: 0.01
---

# Incidents, diagnostics et suivi

## Clôture F7 et findings — 2026-10-18

[[architecture/missions/2026-10-18-f7-uniform-attribute|F7]] est clôturée au
périmètre `6d6dcb7` (299natifs Windows Debug, aucun runASanfinal).

- T1-1 : oracle agrégé de destruction insuffisant sur fuiteancien+destruction
  transféré ; correction `1db306a` à compteurs distincts et ASSERT avant lectures,
  revuefresh KEEP après invalidation du premier KEEP. Mutation raisonnée par
  lecture, pas mutantproduction exécuté ; cas Loaded→Loaded seulement.
- F2 : après publication de size par T3, count=N pouvait faire lire Nobjets au
  driver depuis un seulobjet. Corrigé `1690bac` :34gardes single-value/30count1,
  init déterministe. REDmock sûrs puis readbacks natifs, deux revuesfresh sans
  FAIL. Pas de lecture OOBdriver volontaire, ASan non probant pour ce hazard.
- F1 : `6d6dcb7` corrige deux expressions diagnostiques AttributeContext (type
  réellement stocké au lieu de std::any). SET::TYPE_MISMATCH reste public même
  dans GET, freeze-type inchangé ; RED2/2 puis251CPU et revuefresh sansfinding.
  La prétendue inatteignabilité du mismatch après T2 était fausse : int→float
  reste rejeté. Manifeste T4 rectifié sans réécrire ses campagnes historiques.
- Baux et chemins : C1083chemins longs dans les worktrees résolu par sortiecourte
  dédiée sous out/mission-f7 du principal, sources isolées, pas subst ; vcvars
  dans le même processus de build. Erreurs d'invocation horsRED conservées.
- Providers : capacity_limit Cursor et clé opencode-go expirée étaient des
  échecsinfra, jamais des livraisons ni RED. Révision native3 approuvée place
  Solxhigh primaire strong ; aucun override/fallback automatique retenu.
- TE a été livré par substitution cpp-engineer signalée. L'export public natif
  ultérieur retrouve les deux réponses locales **BLOCKED** : les workers déclaraient
  ne pouvoir utiliser execute/team_message ni satisfaire le mandat d'écriture.
  L'absence de message reçu ne signifiait donc pas absence de tour. Métadonnées de
  permissions ≠ enforcement démontré ; aucun nouveau test de refus natif exécuté,
  aucune règle QA modifiée. Voir la [[references/session-2026-10-18-f7|session complète]].

Restent horscoupe : gatefp64 en production sans extension (les tests natifs
détectent leurs capacités), VBOBinder nonlibéré, noms périmés au reload et
publication Reader partielle, lifetimeGPUen vol, fragilités shuffle historiques,
28spécialisationsF2 sans preuve drivernative. Aucune RAIIexhaustive/perf/image
annoncée. Registre détaillé et limites :
`.opencode/evidence/mission-f7/closure-findings-2026-10-18.md`.

## Règle de lecture

Les **165/165** normaux de la suite courante à `e736bba` (134 unit + 31 OpenGL) et les
**143** cas ASan distincts (archive [[architecture/missions/2026-10-15-failure-contracts|mission C]])
sont une preuve de ces tests Windows Debug, pas une absence exhaustive d'UB ou de fuite.
Les 143/143 de F5 cités plus bas sont historiques. Le
[[references/session-2026-10-03-shader-pass|journal de session]] attribue les avis
aux sous-agents ; les expériences proposées ici ne sont pas des tests exécutés.

## Incidents corrigés pendant la session

### ODR des fixtures et corruption heap

Symptômes : dialogue CRT « HEAP CORRUPTION DETECTED », code de corruption heap
rapporté, timeouts. Défaut trouvé : des fixtures globales homonymes définies
différemment dans plusieurs TU. Renommages `ShaderLoaderTests`,
`PipelineUserTests`, `PassFreerTests` ; suites vertes après corrections.
Un même nom de classe peut être partagé dans un header commun : la règle n'est
pas d'interdire ce partage, mais d'éviter des définitions divergentes sous le même nom.

Si cela récidive : isoler un filtre GTest, rechercher les définitions de fixtures,
capturer le crash/dialogue et les PID. Ne pas conclure à partir du seul code exit 3.
La correspondance « tous les hangs = dialogue CRT » n'a pas une preuve exhaustive.

### Purecall dans la destruction

Un destructeur de base qui appelle une opération virtuelle pure de lifecycle ne
redescend pas dans le dérivé : `_purecall`/abort. La libération est maintenant
dans `~Shader` et `~Pass`, avec destructeurs de base sans ce dispatch.
Le contexte GL doit être courant et valide pendant la destruction ; RAII C++ ne
crée pas cette précondition. Le test ordonne detach → deleteProgram → deleteShader.

### GMock et contexte emprunté — correction d'une fausse piste

`MockShader::getContext()` a été supprimé au profit du véritable contexte possédé.
Mais l'explication initiale « absence de default action ⇒ référence pendante »
est **fausse**. Dans GMock 1.14, `gmock-spec-builders.h` vérifie
`DefaultValue<Result>::Exists()` et lève une erreur ou arrête suivant la config ;
`DefaultValue<T&>` dans `gmock-actions.h` requiert une référence utilisateur.
Un mock par référence est possible avec `ReturnRef(objetStable)` ; il devient
dangling seulement si le lifetime réel de cet objet est invalide.

### Compilation et pression mémoire

C1060 « espace du tas insuffisant » observé dans `out/logs/build-guards.log`,
pendant une forte pression mémoire ; relance séquentielle réussie ensuite.
Mesurer mémoire engagée/limite de commit Windows et RAM, fermer seulement les
processus identifiés et autorisés, garder `--parallel 1`. Après deux tentatives
sans information nouvelle, changer d'expérience ; un délai seul n'est pas un diagnostic.
Le GCC installé sans `std::stacktrace` est une précondition toolchain manquante,
pas un échec de l'implémentation moteur.

### Tests de lifetime mal préparés

- `PassAccessorsOnLiveObject` : vecteur local créé mais shader non inséré dans
  `PassContext`, donc count 0 ; insertion corrigée, attente count 1 conservée.
- Test de destruction : IDs seedés puis `load()` interdits par le contrat strict.
  Retirer les seeds et laisser `glCreateShader`/`glCreateProgram` mockés produire
  les IDs ; conserver l'ordre de suppression et vérifier les IDs après load.
- Include-order de `AttributeReader.hpp` : inclure les composants utilisés par
  son instanciation concrète d'`Attribute`, pas dépendre d'un include du consommateur.
- Context contenant `unique_ptr` non copiable : déplacer le context plutôt que
  rétablir un alias partagé pour faire compiler le test.

### QMD, commandes et outils asynchrones

`artist-wiki*` était une collection inexistante causée par la commande.
Commande corrigée : `qmd embed -c artist-wiki`. Log conservé :
`out/logs/qmd-adr3-embed.log`, CUDA `NoBinaryFoundError`, fallback CPU puis allocation
538443776 octets refusée. Ce log **ne prouve pas** une isolation mémoire sandbox.
CUDA fonctionne selon l'utilisateur en console, et un embedding standard avait
réussi avant la migration. Garder les faits séparés, pas de restriction CPU durable.

Le MCP QMD n'est pas exposé pendant cette ingestion : CLI lexicale et `qmd get`
utilisés avec collection `artist-wiki` et intention de réconciliation documentaire.
Indexation ciblée via le SDK, pas `qmd update -c` ni index `.qmd` de substitution.
À l'issue de cet incident, embedding et recherche vectorielle restaient à prouver.
Pendant l'ingestion documentaire, une expérience avec mémoire disponible mesurée
(10802 MiB RAM, 10502 MiB commit disponible) a réussi : commande standard
`qmd embed -c artist-wiki`, **code 0**, 38 chunks de 10 documents en 6 s.
`qmd vsearch` ciblé a ensuite retrouvé ADR-0003, et recherche lexicale + `get`
ont retrouvé la nouvelle synthèse de session. Preuve locale :
`out/logs/qmd-session-ingest-embed.log`. Ce succès ne prouve pas rétrospectivement
la cause exacte de l'échec précédent, ni une validation de toutes les collections.

Un worker peut finir alors qu'une commande outil est encore en arrière-plan.
La sortie bufferisée de `run-build.py` ne supprime pas les notifications de l'outil.
Attendre la notification, sans polling ni build concurrent. Identifier un processus
orphelin par PID/parent/commande avant de le stopper ; ne pas utiliser un kill
global de tous les `ctest`/`ArtistLib_tests` comme procédure normale.

## Findings de revue : état et prochaine expérience

- **F1 traité** : re-load interdit par les loaders avant création native,
  handles inchangés en rejet. Tests mockés shader/pass + shader driver.
  **Limite** : `IPass::load()` charge d'abord les shaders ; pas de rollback global.
  Prochain test utile : rejet du wrapper pass chargé, puis nouveau cycle complet
  après libération du programme et de tous ses shaders.
- **F2 traité** : `GLuint m_shaderID = 0`, `m_passID = 0`.
- **F3 traité partiellement au périmètre prévu** : accès qui déréférencent le
  contexte Pass gardés par `ctx()` ; `free()` moved-from no-op, `load()` gardé.
  `getContext()` reste une copie de `shared_ptr`, vide sur moved-from, pas une
  référence gardée. Ne pas décrire tous les accesseurs comme throwing.
- **F4 traité pour l'accesseur seul (mission C)** : `PassContext::getShader(index)` et
  `PipelineContext::getPass(index)` employaient `operator[]` sans garde (`30a46b5`,
  `PassContext.hpp:72`, `PipelineContext.hpp:25`). Garde `index < 0 || size_type(index) >= size()`
  avant indexation, exception `TraceableException<std::out_of_range>`, messages
  `ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE` et `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE`. Le RED
  est un plantage CRT `0xc0000409` (STL debug), pas un rejet métier observé : réserve α de R.
  Le curseur `use(index)` reste hors coupe (Pipeline A1).
- **F5 corrigé dans le périmètre 2026-10-15** : `&infoLog[0]` avec longueur de
  log zéro dans les loaders GL. RED causal mocké (`GL_INFO_LOG_LENGTH = 0`)
  avant fix : CTest exit 8, natif `0xc0000409`,
  « vector subscript out of range » sur les deux loaders — la précondition du
  vecteur vide était violée avant `glGet*InfoLog`, ce qui empêchait diagnostic,
  delete unique et exception métier. GREEN minimal : capacité
  `max(maxLength, 1)`, `written` initialisé, clamp `[0, capacity-1]`, terminaison
  explicite, `string_view` borné ; exceptions publiques, préfixes stderr et corps
  `on()` inchangés. Suites indépendantes : **143/143** distincts (137 + 6
  nouveaux ; 112 unit + 31 OpenGL) et **121 distincts sous ASan** (112 + 9 GL
  lifetime ; 22 autres GL hors sélection de mission, pas abandonnés). Revue
  indépendante PASS 7/7 ; QA indépendante PASS 10/10 critères.
  Contrat, empreintes, commandes et limites :
  [[architecture/missions/2026-10-15-f5-gl-diagnostics|mission archivée]] et
  [[references/session-2026-10-15-f5-gl-diagnostics|synthèse de session]].
  F4/F8/Q1 et la migration Pipeline restent hors de cette coupe.

### Suite de revue F5 — Q1, exposition INT_MAX préexistante (traité en mission C)

Item **Q1** de la revue indépendante, faible, préexistant et hors du diff corrigé :
la capacité du buffer vient du pilote ; une valeur aberrante
(`GL_INFO_LOG_LENGTH` = INT_MAX) fait lever `std::vector`
(`bad_alloc`/`length_error`) depuis `checkCompileErrors`/`checkLinkErrors`.
L'exception n'est alors pas l'exception métier et le handle natif n'est pas
supprimé (`shader Loader.hpp:74`, `pass Loader.hpp:73`). L'ancien code avait la
même exposition (il allait aussi allouer `maxLength`). Vérification minimale
proposée — mocker `INFO_LOG_LENGTH` = INT_MAX puis contrater le contrat attendu
(exception métier ? fuite native ?) — **planifiée dans la mission C
([[architecture/next-mission|bornage contrats d'échec]], sélectionnée le
2026-10-15)** : RED propre d'abord, capacité bornée, exception métier et cleanup
une fois attendus en séquence ; pas rétro-ajoutée à F5. Précision vérifiée
contre le code : chemin shader, le handle créé n'est ni stocké ni supprimé ;
chemin pass, le handle est déjà publié dans le contexte et le contrat
`glDeleteProgram + setPassID(0)` du link-fail testé n'est pas exécuté.

Traitement (mission C, `e736bba`) : capacité de log bornée à `[1, 4096]` dans les deux loaders
(`shader/Loader.hpp:82-83`, `pass/Loader.hpp:97-98`), exceptions métier inchangées ; 10 cas
`InfoLogLengths` (dont INT_MAX et -5) passent en normal et en ASan. Limite : le clamp borne
l'allocation, pas la pénurie mémoire du processus. Archive : [[architecture/missions/2026-10-15-failure-contracts|mission C]].

### Suite de revue F5 — Q2 à Q5 consignées

- **Q2, faible, limite de preuve** : si un pilote renvoie
  `GL_INFO_LOG_LENGTH = 0` alors qu'un log existe, le diagnostic est vide
  silencieusement — conforme à la spec, pas une régression (l'ancien code
  crashait). Non observé sur RTX 3090 / driver 617.42 ; pilote réel rapportant 0
  indisponible sur cette machine.
- **Q3, info, hors diff — ruling appliqué en mission C (publication différée)** : si `ShaderAttacher::on` lève après
  `setPassID(passID)` (`pass Loader.hpp:42-46`), le handle programme reste publié
  sans rollback. C'est la limite **F1** déjà consignée ci-dessus. L'architecte a
  livré le ruling le 2026-10-15 : variante préférée = **publication différée**
  (un seul `setPassID` après lien vérifié ; création → attach en forme empruntée
  → link → check ; échec pré-publication = `glDeleteProgram` + throw, contexte
  tel quel) ; variante de repli = non-rollback **documenté et testé** (handle
  conservé + `free()` idempotent + B intact). Les deux variantes sont fermées
  par des tests RED, la preuve décide — aucune des deux reste en doc seule.
  Exécuté en mission C (C1), la sonde
  encodeant l'ordre actuel (`LoaderTests.cpp` : `passID==7` pendant le
  rappel de information-log) conformée comme point de revue explicite.
- **Q4, info** : `std::string_view` imprime désormais un éventuel NUL interne du
  log, l'ancien code s'arrêtait au premier NUL. Cas de pilote pathologique, non
  testé.
- **Q5, info, provenance** : le build GREEN a été réalisé à HEAD `5dfafd1` avec
  le correctif non committé ; `final-validation.json` (HEAD `4154780`,
  `sourceHashesMatchGreen: true`) est cohérent — même octets vérifiés. Le lien
  source → binaire repose sur l'intégrité SHA-256 et les métadonnées, pas sur un
  rebuild.
- **F6 nommé, non migré** : `IPass` garde un contexte partagé et l'expose par copie ;
  lifetime extensible hors wrapper. Architecture propose de migrer d'abord le
  stockage/les consommateurs du pipeline ; aucune refonte faite aujourd'hui.
- **F7 différé** : `UniformReader` absent d'`OpenGLPassFlow` malgré appel nominal,
  absence silencieuse sous `if constexpr`. Test négatif de concept à écrire dans
  la tranche uniform/attribute ; ce n'est pas un composant aujourd'hui garanti.
- **F8 traité partiellement (mission C)** : le reader n'activait que `badbit` ; un fichier
  absent devenait une compilation de chaîne vide (`30a46b5`, `Reader.hpp` l.31 et l.38). Désormais :
  extraction par `read()`/`gcount()`, `failbit` sans EOF = `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ`,
  fermeture vérifiée avant publication, fichier vide lisible = chaîne vide. Non exercées : la branche
  de fermeture échouée et la branche `ifstream::failure` réelle (badbit). V : INCONCLUSIVE ; réserve
  β de R. La formule « les chemins réels couvrent chaque branche » est écartée.
- **F9 amélioré** : tests supplémentaires d'accès shader/pass moved-from ; couverture
  non exhaustive de toutes les méthodes. Les traits move/noexcept ne prouvent pas RAII.
- **F10 décision : conservation documentée (mission C)** : `use()` sur programme zéro appelle
  `glUseProgram(0)`, unbind explicite valide en OpenGL plutôt que rejet d'un pass non chargé.
  Caractérisé par `UsePass_ZeroPassIDStillCallsGL` (`UserTests.cpp:49`), sans migration de
  consommateur. Ce n'est pas une correction ; aucun test moved-from dédié.
- **F11 faux positif écarté** : un `shared_ptr<T>` peut posséder un T non copiable ;
  aucune copie de `Pass` n'est nécessaire pour le stocker par pointeur partagé.

### Pipeline — garde-fous d'une future tranche, pas un fix exécuté

Consultation architecturale pendant la sélection F5, en lecture seule :

- `IPipeline` partage encore son contexte et les passes, accesseurs compris.
  Une coupe d'ownership cohérente doit aussi choisir copies/moves et emprunts,
  pas seulement remplacer le vecteur. Un move généré pourrait copier le sous-objet
  de base dont le destructeur est déclaré : exiger des probes, pas une déduction
  de « move » à partir de sa seule syntaxe. ^[inferred]
- `PipelineContext` accepte un pass nul et un curseur arbitraire, indexe sans
  contrôle ; `hasNext()` additionne un au curseur. Bornes/overflow/état après
  échec restent à reproduire et définir avant migration. `reset()` remet le
  curseur à −1, ce n'est pas un unbind `glUseProgram(0)`.
- Les passes de `ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp` ne sont pas
  chargées ; leur destruction comme membres de fixture arrive après le teardown
  du contexte. Le succès actuel sur handles zéro ne qualifie donc pas la
  destruction de programmes chargés par un Pipeline. Les nouvelles fixtures
  devront détruire les ressources avant leur contexte.

Ces observations ne modifient pas les résultats antérieurs, ne sélectionnent pas
Pipeline à la place de F5 et ne prouvent aucun défaut reproduit dans cette sélection.

## Assignation move vers owner chargé — risque reproduit et corrigé

Constat initial : les opérateurs de base remplaçaient `m_context` sans cleanup
natif destination ; les tests de construction move ne le couvraient pas.
RED causal CPU et GL le 2026-10-15, puis cleanup avant transfert, opérateurs de
base `protected`, owners concrets `final`, assignations potentiellement levantes.
Revue indépendante du final : PASS de lecture ; **137/137 normaux** et
**106 CPU + 9 lifetime GL sous ASan**, sans échec/skip dans ces exécutions.
Voir [[references/session-2026-10-15-loaded-owner-move|commandes, empreintes et limites]].
Ce correctif ne promet pas un rollback après detach partiel, ni un destructeur
capable de récupérer d'un cleanup throwing. Le partage du contexte Pass est
conservé et testé par aliases ; F6 n'est pas clos. Les autres findings ci-dessus
restent à leur statut antérieur.

### Réserve du harness ASan : environnement hérité (Q1 — traité en mission A1+A2, H-ASan)

`verify_asan.py` fixe `ASAN_OPTIONS` pour le canari seulement ; `build-windows.ps1`
ne normalise pas l'environnement arbitraire de CTest. Un parent permissif
(`exitcode=0`, diagnostic redirigé) pourrait fausser une autre campagne. Aucun faux
PASS n'a été constaté ici : les métadonnées S05/S06/S10 consignent toutes
`halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`, identique au canari,
sans `log_path`. Réserve non bloquante documentée, pas un durcissement implémenté.

- Traitement (mission A1+A2, H-ASan, `eb8659e` intégré en `760f4f8`) : normalisation
  par le harnais sous `-AddressSanitizer` AVANT tout enfant instrumenté (découverte
  GoogleTest, préflight canari, ctest et ses processus) ; canonique
  `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`, sans `log_path` ;
  canari **fail-close** sur héritage hostile et `inherited_asan_options` consigné
  dans `summary.json`. Nouveaux `scripts/asan-env.ps1` + `scripts/test_asan_env.py`
  (contrat module), `verify_asan.py`/`build-windows.ps1` modifiés. Aucun comportement
  hors ASan changé. PREUVE RED : shim observateur au point réel d'appel ctest — env
  hostile VALIDE et permissif (`exitcode=0`, `log_path` redirigé) transmis tel quel,
  campagne 134/134 « PASSED » et harnais exit 0, `artist_asan_probe` use-after-free →
  exit 0 (masquage mécaniquement prouvé) ; un `log_path` malformé (deux-points non
  échappés) est rejeté PAR LE PARSING du runtime (fail-close, pas un faux PASS). GREEN :
  contrat module FAIL→PASS ; intégration complète avec env hostile au parent →
  « reset to canonical », observateur canonique au point ctest, campagne 134/134
  honnête. Empreintes sha256 (16 hex) : `out/logs/mission-hasan-pool2/validation-summary.json`.

### Incidents d'outillage de cette mission

Le worktree H était détaché et sa référence Ensemble absente : premier
`team_merge` refusé avant fusion. Après vérification du commit livré et du
worktree propre, seule la référence possédée manquante a été restaurée avec
garde d'absence ; `team_merge` a ensuite intégré H. Aucun merge manuel ni commit
des changements utilisateur. Les limites catalogue Code Mode ont été traitées
par les fonctions MCP directes effectivement disponibles, sans réinstaller de plugin.

Une dérive de `watch.last_index_time` dans `.grepai/config.yaml` a été isolée par
V/S et recoupée par le coordinateur : reste du fichier identique au snapshot
utilisateur, attribution exacte non établie, aucune réversion. Les 15 entrées
source/build intégrées restent identiques ; ne pas affirmer une immutabilité
globale du checkout. ^[ambiguous]

## Mission C — incidents, anomalies et statuts (2026-10-15, `e736bba`)

Archive : [[architecture/missions/2026-10-15-failure-contracts|mission C]] ; manifeste
`_meta/session-2026-10-15-failure-contracts-validation.json`. Les anomalies ci-dessous ne sont
pas des preuves et ne sont pas comptées dans les totaux.

### getStackTraceString — référence pendante (traité en mission A1+A2, S-GSS)

- Constat : `ARTIST/include/common/exception/TraceableException.hpp:46-57`, branche `#else`
  (active lorsque la condition `#if` des lignes 5 et 17 est fausse, par exemple sous MSVC) :
  `const std::string &getStackTraceString() const` retourne `"Stacktrace not available"`, soit
  une référence vers un temporaire.
- Portée : membre de template, instancié seulement à l'usage. Grep à `e736bba` : deux
  déclarations (l.29 et l.53), zéro appelant dans `ARTIST/`. Défaut dormant.
- Décision : **non corrigé** en mission C (fichier hors périmètre, interdit par la fiche). Coupe
  séparée avant toute exposition ou tout appel. R (point δ) confirme : défaut dormant, pas un
  fixed en C.
- Traitement (mission A1+A2, S-GSS, `433bdbc`) : retour par valeur `std::string` sur la branche
  `#else` (l.56 après correction) branches alignées (l.29 déjà par valeur). RED causal double,
  observé avant fix : runtime ASan strict — `stack-buffer-overflow` sur « compiler temporary »
  détruit (temporaire de 23 caractères, hors SSO 15), lu depuis le test
  `TraceableExceptionTests.cpp`, options `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`,
  log `out/logs/s-gss-red-run.log` ; sonde compile-time hors dépôt — `C2338` sur la signature
  `const&`, `out/logs/s-gss-red-compile.log`. GREEN : 2/2 sous ASan ; suite normale 167/167
  (136 unit + 31 OpenGL, RTX 3090 / driver 617.42, 0 skip, `ctest-s-gss.xml`) ; ASan unit
  136/136 avec options strictes (`ctest-unit-s-gss-asan.xml`) ; canari 0/86 `verify_asan.py`
  PASS. Branche de travail : `befb5a8` (RED, test seul) → `36e4a34` (fix +
  `static_assert` branch-agnostique). Premier consommateur de l'accesseur : tests uniquement,
  zéro appelant de production ajouté — le défaut reste dormant en production, exposition inchangée.
- Défaut latent découvert par S-GSS (**OPEN**, hors périmètre, non corrigé — à arbitrer) : dans
  la branche `#if` (libstdc++), `getStackTraceString() const` (l.29) appelle `format_stacktrace`,
  membre **non const** (l.37) — la première instanciation réelle de cette branche ne compilerait
  pas (const-correctness). Branche inatteignable sous MSVC (exige des macros libstdc++ ET
  `__cplusplus > 202002L`) ; non compilable ni exécutable sur cette baseline (GCC MSYS2 13.2 sans
  `std::stacktrace`). Coupe dédiée recommandée avant toute exposition de l'accesseur sous
  GCC/libstdc++. Les références OPEN résiduelles dans l'index, ADR-0003 et l'archive mission C
  sont à réconcilier en DOC-F/I1.

### F8 — preuve de lecture incomplète (traité partiellement)

- Non exercés : la branche « fermeture échouée » (`Reader.hpp:72-77`) et la branche
  `catch (std::ifstream::failure)` (`Reader.hpp:82-88`, badbit réel). Preuve manquante : une seam
  d'extraction (`streambuf` de test) ou un échec réel reproductible. Non exécutée ; V : INCONCLUSIVE.
- Ne pas écrire que « les chemins réels couvrent chaque branche » : formule du message de commit
  `e736bba`, écartée avec la réserve β de R.

### Anomalie RED de développement — baseline C2 du 20:35 (OPEN, cause non établie)

- `mission-c2-baseline-ctest.xml` (20:35) : 165 cas, 13 échecs : 10 F4 (`0xc0000409`) et 3
  `ReaderTests`.
- `EmptyReadableFileYieldsEmptyCode` échoue sur `std::filesystem::exists(emptyPath)` faux
  (`ReaderTests.cpp:114` dans le log) ; `MultiBlockReadPreservesContent` échoue à
  `ReaderTests.cpp:146`.
- `mission-c2-red-exact.xml` (20:38) : 16 cas, 11 échecs (10 F4 et `AbsentFileRejected`). Ces
  deux tests Reader y passent. Les numéros de ligne diffèrent : version antérieure possible du
  fichier de tests, non vérifiée. Aucun rapport n'attribue ces échecs.
- Statut : la baseline 20:35 **n'est pas un RED causal** et n'est citée nulle part comme tel. Seul
  `red-exact` (20:38) sert de RED F8 causal.

### Campagne ASan incrémentale — FAIL 133/134 (mécanique expliquée en mission A1+A2, H-ASan)

- `asan-unit.xml` (21:00) : 134 cas, 1 échec, `ReadShaderTest_AbsentFileRejected` (« Expected an
  exception to be thrown »). `asan-reader-reproduce.xml` (21:00) : 1/1 échec.
- Sur le même HEAD, après rebuild propre (`asan-clean-*`, 21:07-21:08) : 134/134 et 9/9.
- Recommandation du QA : clean-first pour toute campagne de candidat ; enquête séparée sur la
  fraîcheur des artefacts (objets obsolètes ou COMDAT, non démontré). Ne pas masquer le premier
  échec.
- Enquête (mission A1+A2, H-ASan — hypothèse H-TEMP-LOCK + expérience discriminante) : la
  « garantie d'absence » du test est silencieuse — `absentTempShaderPath()` appelle
  `std::filesystem::remove(path, ec)` et **ignore `ec`** (`ReaderTests.cpp:48-54`) sur un chemin
  `%TEMP%`, hors portée d'un clean rebuild. Un verrou externe transitoire (type Defender,
  ouvert sans `FILE_SHARE_DELETE`) fait échouer le `remove` en silence, puis la lecture réussit →
  aucune exception → exactement `TestUtils.hpp(16)`. Expérience sur le binaire post-clean 21:07
  (aligné au code actuel, `git diff e736bba..a50f466 -- ARTIST/` vide) : E1 baseline PASS ;
  E2 (fichier vide au lieu « absent » + verrou `ReadWrite/FileShare.Read` tenu pendant
  l'exécution) → FAIL, lignes identiques au caractère près au XML 21:00 ; E3 (verrou levé,
  résidu supprimé, MÊME binaire sans rebuild) → PASS. **Établi** : la mécanique état-fichier est
  SUFFISANTE pour expliquer les observations ; la cause binaire (objets périmés/COMDAT) n'est
  PAS nécessaire ; le rétablissement après rebuild n'implique pas la causalité du rebuild.
  **Limite** : le déclencheur réel du 21:00 reste INCONCLUSIVE (verrou non observable a
  posteriori, résidu nettoyé par le dernier TearDown). Le FAIL 21:00 n'a pas été masqué ;
  clean-first systématique réaffirmé ; sonde reconstruite après clean (canari 0 puis 86).
- Finding consigné pour coupe dédiée (renforcement de test, pas un défaut moteur) :
  `ReaderTests.cpp` `absentTempShaderPath` devrait vérifier l'échec du `remove` ou utiliser un
  nom unique par exécution. Lié au périmètre F8/Reader — à intégrer au ruling F8-SEAM ou à une
  coupe test-hygiène séparée.

### Harnais et environnement (clos)

- Découverte `GoogleTestAddTests` expirée à 5 s pendant le lien (`normal-clean-build.log`, 21:06) ;
  relance identique, sans échec.
- La sonde `artist_asan_probe` supprimée par le clean-first donnait `FileNotFoundError` ; rebuild
  explicite, canari 0 puis 86.
- Re-glob CMake après intégration : le premier run C2 à 153/153 était pré-glob et invalide ; 165/165
  après re-glob.
- Chemins longs (C1083) : `subst Z:` (C1) et `subst Y:` (C2) autorisés, sans modification Git ni
  CMake. Ce ne sont pas des RED.

## Intégration Pipeline A1 — archive GoogleTest préexistante incompatible

À `d319096`, le premier build de l'union D-A1 + S-GSS + H-ASan compile les
sources puis échoue au lien : `LNK2019 __std_rotate` dans `gmock_main.lib`.
Cette archive est construite localement sous `out/build/windows-msvc-debug/_deps`,
pas fournie par Conan. L'archive précédente est conservée ; `dumpbin /symbols`
y trouve une référence `UNDEF __std_rotate`. Reconstruction ciblée de
`gmock_main` dans l'environnement courant, sans changement de source moteur :
la référence disparaît, le lien réussit et les **167/167 unitaires distincts**
de l'union passent (134 baseline + 31 D-A1 + 2 S-GSS), sans skip/disabled.

La substitution de l'archive préexistante suffit à résoudre cet incident ;
son historique de compilation exact reste inconnu. Le répertoire du toolset
`14.44.35207` et la version binaire `19.44.35221` ne démontrent pas, à eux seuls,
une installation Visual Studio incohérente. Premier FAIL conservé, non classé
RED moteur ; aucune preuve GL ou ASan de l'union n'est tirée de cette expérience.
Commandes, empreintes et JUnit :
`docs/_meta/session-2026-10-16-pipeline-i-a1-validation.json`.

## Pipeline A1c — réserve du protocole RED historique

Le GREEN producteur `3526ead`, intégré en `3c670c9`, contient les owners GPU
locaux aux corps de tests ; 39/39 GL ont été rejoués sur le principal après
reconfiguration/découverte fraîche. Les 13 cas GL ASan du producteur sont les
5 `PipelineTest` devenus chargés et les 8 nouveaux `PipelineLifetimeTest` :
ils ne couvrent pas les 9 `ShaderLifetimeComponentTests` historiques.

L'inspection du RED `df35b3f` a trouvé un écart confirmé par l'auteur :
`ContextCheckedOwner::~ContextCheckedOwner` (`ProgramTests.cpp:69-80`) vérifie
le drapeau CPU puis appelle `m_owned.reset()` après `TearDown()` ; le destructeur
de `Pass` appelle `free()`, donc le cleanup GL chargé hors contexte. L'assertion
CPU préalable ne qualifie pas la suite comme CPU-only. Cette sonde concernait
les 5 `PipelineTest`, pas les 8 nouveaux tests body-local non chargés du RED.
Son comportement driver/WGL silencieux n'est pas une preuve valable.

**FAIL du protocole historique** « aucun appel GL volontaire après teardown »,
pas un défaut conservé dans le GREEN. Aucune réexécution de la sonde interdite ;
originaux/manifeste préservés et addendum séparé. La réserve reste à transmettre
à la revue fresh et à la QA I0, sans transformer les succès GREEN en conformité
TDD historique globale. Preuves et empreintes :
`docs/_meta/session-2026-10-16-pipeline-a1c-validation.json`.

## Mission Pipeline A1+A2 — clôture documentaire (2026-10-16, retouches 2026-10-17)

Archive : [[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline
A1+A2]] ; manifeste figé `_meta/session-2026-10-16-pipeline-final-validation.json`
(24 critères : **14 PASS, 3 FAIL, 7 INCONCLUSIVE**, 85/85 artefacts recoupés,
129/129 empreintes sources identiques). Cette clôture **ne modifie aucun code ni
test** et **ne réexécute rien** ; les statuts ci-dessous complètent, sans
écraser, les sections ci-dessus.

### Oracle GMock Attribute — FAIL historique, corrigé et vérifié le 2026-10-17

- Constat conservé : `AttributeTests.cpp:65` (`AttributeTest.SetTest`), diagnostic
  `mockOn` attendu une fois / observé zéro **après summary** ; exit 0 du
  processus et CTest passant malgré tout. Reproduit **directement** (toutes
  suites) **et** sous CTest normal/ASan ; la revue l'a trouvé dans
  `out/mission-a1a2/i0-verification/asan-last-test.log:2494-2501` avec filtre
  individuel et « Test Passed ». Les sorties JUnit des campagnes tronquées à
  1024 octets cachaient l'erreur ; un témoin à 65536 conserve le diagnostic dans
  `normal-attribute-settest.xml` ; croisement `LastTest.log`.
- Arbitrage historique : défaut d'oracle **préexistant hors périmètre A2**, pas
  une régression A2 ; corrigé ensuite par amendement explicite, sans effacer le
  FAIL initial. L'absence dans les JUnit tronqués ne prouve pas l'absence dans
  les logs bruts.
- **Interdits** : qualifier le défaut « direct-only », qualifier 215/198 comme
  « tous les oracles valides » ou annoncer un PASS global ; ne pas transformer
  un exit 0 en PASS sans réserve.
- **Amendement oracle terminé au périmètre** : l'utilisateur a autorisé la
  correction le 2026-10-17 (« Alors vas-y »). Checkpoint oracle-only
  `d1a9ee1` : ancien test réellement RED (binaire1, CTest8, JUnit failure1 avant
  résumé), binaire négatif préservé et recoupé. GREEN intégré `b9c90fd`, un seul
  fichier test : TearDown vérifie/efface les quatre singletons sans court-circuit,
  attente sur la spécialisation effective `MockSetter<shared_ptr<int>>`, Times(1)
  conservé, contexte et valeur resserrés. Production et mocks inchangés.
  Revue G2 **PASS sans finding** ; QA G3 indépendante **176 CPU + 176 CPU ASan**,
  contrôles négatifs répétés1/8/failure1 et positifs0/0/failure0 sans diagnostic
  tardif, 137 artefacts et 130 empreintes recoupés. TearDown après assertion
  fatale vérifié statiquement dans GoogleTest v1.14.0, pas de scénario dédié
  exécuté. Le FAIL historique n'est pas requalifié. Manifeste :
  `_meta/session-2026-10-17-attribute-oracle-validation.json` ; contrat :
  `_meta/session-2026-10-17-attribute-oracle-plan.json`. La fiche courte du
  jour a depuis été remplacée.

#### Addendum F7 T2 (2026-10-17) — oracle `SetTest` réaligné sur `Setter<T>`

Ajout postérieur : la section ci-dessus reste l'état vérifié à `b9c90fd`, sans
réécriture. **Sortie auteur** de la tâche T2 de F7 option A
(`task_mvcebdf3_001s_58nvmt`, base `cb9f268`, branche
`ensemble-artist-mcp-lifecycle-mission-5caa4a18f43401c7#eiyn0g-pool-6`) :
**ni intégrée ni revue indépendamment** à la rédaction ; le lead relit le diff
avant T3.

- Cause : `MockSetter<shared_ptr<int>>`, observé par `b9c90fd`, venait de la
  clé figée de `IAttribute::set` (`Setter<std::shared_ptr<int>>`,
  `Attribute.hpp:88,90` à `cb9f268`), pas d'un choix d'oracle. Le correctif de
  production D2 indexe le dispatch sur `Setter<T>` : l'observateur effectif de
  `SetTest` devient `MockSetter<int>`.
- RED-B, oracle seul, production intacte : `MockSetter<int>` `Times(1)` jamais
  appelé et `MockSetter<shared_ptr<int>>` `Times(0)` appelé une fois, tous deux
  signalés avant le résumé GoogleTest ; CTest exit 8, JUnit 4 cas / 1 échec.
  Binaire préservé `out/mission-f7/t2/red-b/ArtistLib_tests.exe`
  (`D72AF41C…1611`). Sur GL réel, la même clé levait `TYPE_MISMATCH` pour
  `int` et `glm::vec3` (2/2).
- Oracle final : `MockSetter<int>::mockOn(Ref(contexte possédé))` `Times(1)`,
  `MockSetter<shared_ptr<int>>` `Times(0)`, `ASSERT_NO_THROW`, valeur stockée
  non nulle, même objet, égale à 1. Le `TearDown` de `b9c90fd` est inchangé ;
  son commentaire de code est réécrit sans historique, conservé ici.
- Résultats auteur sur le diff final : 207/207 unit et 42/42 OpenGL réels,
  découverte fraîche 249, limite de sortie JUnit 65536 ; ni ASan ni shuffle.
  Le FAIL historique n'est pas requalifié ; les réserves Loader/ShaderLoader
  ci-dessous restent ouvertes. Les autres défauts établis par RED causal dans
  T2 (taille d'un pointeur transmise à `glBufferData`, valeur nulle transmise
  au driver, déplacement implicite supprimé par le destructeur virtuel) sont
  décrits dans `out/mission-f7/t2/t2-slice-note.md`. Manifeste : clé
  `f7_t2_followup` de `_meta/session-2026-10-17-attribute-oracle-validation.json`.

### Ordre des fixtures Loader/ShaderLoader — FAIL préexistant, non corrigé

L'essai G1 de shuffle du binaire CPU complet révèle neuf cas Loader/ShaderLoader
en échec sous seed20261017 (effectif61219), et `LoadShaderTest_callCreate` sous
seed7. Auteur puis QA indépendante ont rejoué **PRE_G1, RED et GREEN** : mêmes
176 tests dans le même ordre par seed ; les ensembles d'échecs PRE_G1/GREEN
sont exactement identiques, RED ajoute seulement `AttributeTest.SetTest` attendu.
Le vrai binaire PRE_G1 est lié à la QA `f914d77` par SHA, pas au checkpoint RED
déjà modifié. Préexistence indépendante de tout l'amendement établie pour ces
deux ordres, pas une garantie toutes permutations. Pas de défaut moteur nouveau
prouvé par cette expérience ; enquête lifecycle/isolation des autres mocks à
autoriser séparément. Aucun correctif ou nouveau chantier implicite. Preuves :
`out/mission-a1a2/gmock-amendment/qa/shuffle-tripartite-results.json`.
La suite en ordre par défaut et la fixture Attribute corrigée restent conformes
dans les campagnes déclarées ; **pas de PASS global toutes configurations**.

### Clôtures des correctifs harnais

- **H-RESTORE (`1a75c75`)** : restauration de l'env appelant vérifiée
  indépendamment avant clôture — cinq contrôles réels **séparés** du compte 198
  (4 bras appelant + Q1), 0 échec ; module normaliseur inchangé ; manifeste
  `_meta/session-2026-10-16-asan-caller-restoration.json`. Le FAIL de
  reproduction I0 reste l'historique de la découverte.
- **H-FIXTURE (`95c2b9e`, deux fichiers Python uniquement)** : P2 du staging
  corrigé après RED d'injection isolé **avant** fix ; vérification indépendante
  (producteur `4d1b655`, `out/mission-a1a2/post-fixture-verification/report.json`,
  SHA-256 `cac855ff…00e8a`) : **14 contrôles harnais séparés** (9 tests de fixture
  TEMP + 5 contrôles du harnais, dont 4 bras réels), 21/21 artefacts recoupés, 130/130 empreintes
  (128 inchangées + 1 runner + 1 test), **cinq binaires identiques** avant/après
  et état du build sans hold. **Aucun rerun 215/198/GPU** : C++ et configuration
  inchangés, comptage antérieur conservé avec l'oracle FAIL — pas une
  recertification, pas une nouvelle campagne. Les fenêtres signal/état + TOCTOU
  ne prouvent qu'une exclusivité, pas une atomicité ni une interruption
  brutale.

### Distinctions d'invocation et d'empreintes (leçons de la campagne)

- **Premier appel ASan CTest exit 8 avant tout test** : chemin **relatif** du
  `--tests-from-file` résolu contre le répertoire de build. Erreur
  d'invocation, **pas un RED** ; l'invocation suivante avec chemin absolu donne
  les 198. Le log original est conservé ; seul le chemin a changé.
- **Empreintes binaires** relevées **après** les campagnes, avant la
  vérification finale : elles ne sont pas des empreintes pré-build rétrodatées
  ; l'identité binaire de `95c2b9e` provient d'une comparaison
  avant/après wrapper dans la même session.
- **`validation-levels.md`** : absente du vault, **non restaurée** par cette
  coupe ; aucune reconstruction sans source.
- **Ancien LNK `__std_rotate`** : livré par la reconstruction ciblée des
  archives gmock (`_deps` locales), **pas par le moteur** ; voir la section
  « Intégration Pipeline A1 » ci-dessus.
- **Réserve H-TEMP-LOCK** : l'expérience E1–E3 prouve la **suffisance du
  mécanisme** état-fichier ; le déclencheur réel du FAIL 21:00 reste
  INCONCLUSIVE — voir « Campagne ASan incrémentale » (mission C) : mécanisme
  prouvé, cause historique jamais établie.
- **Reste non résolu** (hors périmètre de cette mission) : tant `ReaderTests`
  `removeError` non consulté + `exists` suivant les liens (P3, INCONCLUSIVE),
  qu'une panne OS d'extraction ou de fermeture échouée (F8 partiel), le latent
  `getStackTraceString` const `libstdc++` hors MSVC, la fenêtre d'un contexte GL
  courant invalide, un GPU en vol au-delà du scope C++ et de la frame de
  soumission — et l'absence exhaustive de preuve RAII/image/perf
  (Release, Linux, CI distante).

## Expériences encore absentes

### Revue finale A2 — P2 fixture et oracle GMock préexistant

R-A2 fresh (`f914d77`, huit fichiers, 129 empreintes conformes), lecture seule :
aucun défaut nouveau identifié dans le C++ A2 ; aucune nouvelle exécution.
Le nouveau runner Python H-RESTORE comporte en revanche un **P2** :
`scripts/test_asan_env.py:154-170` déplace le build sous `.hold-hrestore`, puis
crée un fichier vide. Si cette création lève avant le retour du helper,
`check_wrapper_restoration` garde `held = None` et son finally ne remet pas le
build original en place. Défaut de contrôle de flux établi ; injection causale
à exécuter avant correction dans H-FIXTURE, branche isolée et temporaire privé.
Il concerne la fixture du test, pas la restauration des variables PowerShell.
**Clôture intervenue** : correctif intégré `95c2b9e`, vérification indépendante
reçue (14 contrôles harnais séparés, voir la section « Mission Pipeline A1+A2 —
clôture documentaire ») ; le constat ci-dessus reste l'historique de la
découverte.

**Correction de qualification GMock** : le diagnostic `AttributeTests.cpp:65`
(`AttributeTest.SetTest`, `mockOn` attendu une fois, observé zéro après résumé)
n'apparaît pas seulement en exécution directe de toutes les suites. La revue
l'a retrouvé dans `out/mission-a1a2/i0-verification/asan-last-test.log:2494-2501`
avec filtre individuel et `Test Passed`. C'est un faux négatif d'oracle
préexistant, pas une régression A2. Les JUnit scannés sans diagnostic ne
suffisent pas à certifier stdout ; QA finale doit contrôler le LastTest.log.
Conserver séparément les décomptes CTest, ce défaut d'oracle et le FAIL du
protocole historique RED A1c. Ne pas transformer un exit0 en PASS sans réserve.

V-A2 l'a ensuite **reproduit fraîchement** sur le candidat intégré `f914d77` :
direct, CTest normal individuel et LastTest.log ASan final. La sortie JUnit des
campagnes était tronquée à 1024 octets ; un témoin avec limite 65536 conserve
le diagnostic dans `normal-attribute-settest.xml`. Les décomptes exécutés
restent 176 CPU + 39 GL et sélection ASan 198, zéro échec/skip **rapporté par
CTest**, pas une conformité de tous les oracles. Bilan indépendant : 14 PASS,
3 FAIL (oracle actuel + deux historiques), 7 INCONCLUSIVE. 85 artefacts et
129 empreintes source recoupés. Cinq contrôles H-RESTORE réels passent et le
build a été restauré sans hold ; la fenêtre d'échec du staging n'est pas
couverte par ces cinq contrôles et attend H-FIXTURE. Aucun PASS global.
Voir `_meta/session-2026-10-16-pipeline-final-validation.json`.

### Revue I0 — réserves mineures distinctes des verdicts exécutés

Revue fresh de `18edc72`, 24 fichiers du diff et 129/129 empreintes recoupées :
aucun défaut bloquant du code A1 final identifié, pas une preuve exhaustive ni
une nouvelle exécution de tests par le reviewer. QA indépendante ensuite reçue :
214/214 normaux et sélection ASan exacte 197/197, sans échec/skip dans ces suites.

- **P3 H-ASan, contrôle de flux confirmé par reproduction QA** :
  `build-windows.ps1:86-90,115-118` et `asan-env.ps1:31-32` : une variable
  ASAN_OPTIONS initialement absente est créée par le normaliseur sans entrée
  dans `savedEnvironment`. Elle peut rester définie au retour dans le même
  PowerShell appelant. V-I0 l'a reproduit par appel réel de
  `build-windows.ps1 -AddressSanitizer -ConfigureOnly` dans le même processus :
  variable absente avant, canonique encore présente après retour. FAIL exécuté,
  non corrigé dans I0 ; correction H-RESTORE intégrée ensuite en `1a75c75`.
  Producteur : deux bras absence échouent avant fix, quatre bras restauration
  et Q1 passent après fix (5/5 JUnit) ; source/artefacts recoupés, module de
  normalisation inchangé. Vérification indépendante finale du correctif encore
  attendue, pas de relance du principal pendant le bail Conan A2. Ce point
  concerne la restauration après campagne, pas la normalisation stricte/fail-close
  de la campagne courante, dont les 197 tests sélectionnés ont passé. Le
  correctif a ensuite reçu sa vérification indépendante (cinq contrôles réels
  séparés du compte 198 lors de la QA finale) et le harnais a été re-vérifié à
  `95c2b9e`, sans relance du moteur.
- **P3 fixture TEMP, question non reproduite** :
  `ReaderTests.cpp:136-150` calcule `removeError` sans la consulter, et `exists`
  suit les liens. Le cas suppression en erreur puis absence, ou lien pendant,
  n'est pas qualifié par le test de répertoire résiduel. Le contrat littéral
  exige le rejet de l'erreur ; ne pas prétendre une couverture acquise de ce
  cas, ni affaiblir le contrat pour obtenir un PASS. Aucun défaut d'extraction
  moteur exécuté n'est démontré par cette question.

Le **FAIL du protocole RED historique A1c** est confirmé séparément et demeure
consigné. Ces réserves sont transmises à V-I0 sans réexécuter la sonde interdite.

État I0 de la mission Pipeline (`18edc72`) : F8 a reçu une seam interne testant
l'extracteur réel sous badbit (4096 octets consommés, deuxième demande levant
une exception sentinel, traduction exacte et contexte non publié), plus une
preuve de non-traduction de `bad_alloc` et de fermeture avant publication sur
Windows. Le refactoring est caractérisé, pas présenté comme un RED moteur
historique nouveau. Les fixtures ont des racines possédées hors du parent
hérité : première version sous ce parent refusée à l'intégration, test RED
d'ascendance causal puis correction `b44ba0b`.

Les originaux et corrections sont conservés sous `out/mission-a1a2/pool-7-*`.
Producteur F8 : 175 unitaires et 16 cas au filtre ASan Reader (12 dans la suite
ReaderTests, 4 autres correspondances), canari 0/86 recoupé. Union normale
intégrée : 175 CPU + 39 GL ; revue/QA fresh reçues et ASan intégré 197/197.
F8 reste **partiel** : fermeture échouée et panne filesystem réelle pendant
l'extraction non prouvées. Les comptages des campagnes historiques ci-dessous
restent datés ; manifeste courant :
`docs/_meta/session-2026-10-16-pipeline-i0-validation.json`.

ASan Windows Debug est exécuté séparément et instrumenté ; après F5, la sélection
ASan couvre **121 cas distincts** (112 unit — dont les 6 nouveaux diagnostics —
plus 9 GL lifetime) contre 115 lors de la mission loaded-owner move, toujours
sans résultat pour les 22 GL non sélectionnés, UBSan/TSan ou les autres
plateformes. Les 22 ne sont ni des tests échoués ni des tests abandonnés : la
sélection ASan était ciblée dès le plan ; la suite normale couvre bien les 31 GL. Après la
mission C (`e736bba`), la sélection couvre 143 cas distincts (134 unit + 9 GL lifetime) ; les 22
autres GL restent non exécutés sous ASan.
L'entrée d'environnement ASan héritée (Q1 du harness) reste ouverte ; l'exposition INT_MAX
(Q1 ci-dessus) est traitée en mission C, dans la limite d'allocation documentée. Les dépendances Conan GLFW/GLEW et le driver
restent non instrumentés. Release, Linux, CI distante, couverture, images et
performance restent non validés.

Navigation : [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
[[architecture/correction-plan|Plan de correction]] · [[index|Accueil]].
