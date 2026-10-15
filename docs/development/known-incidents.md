---
title: Incidents connus et suivi Shader→Pass
category: development
tags: [artist, cpp, ownership, testing, tooling]
summary: Incidents corrigés, hypothèses rectifiées et expériences discriminantes pour les chemins encore non couverts de Shader→Pass.
created: 2026-10-03
updated: 2026-10-15
sources: [docs/references/session-2026-10-03-shader-pass.md, docs/_meta/session-2026-10-03-validation.json, research/sessions/2026-10-15-loaded-owner-move/session-capture.md, research/sessions/2026-10-15-loaded-owner-move/R-review.md, docs/_meta/next-mission-2026-10-15-selection.json]
provenance:
  extracted: 0.96
  inferred: 0.03
  ambiguous: 0.01
---

# Incidents, diagnostics et suivi

## Règle de lecture

Les **137/137** normaux de [[development/build-and-test|la suite courante]] sont une preuve
de ces tests Windows Debug, pas une absence exhaustive d'UB ou de fuite. Le
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
- **F4 ouvert** : `PassContext::getShader(index)` emploie `operator[]`, donc
  indice négatif/hors borne à qualifier ; test de rejet et contrat de bornes requis.
- **F5 ouvert** : `&infoLog[0]` avec longueur de log zéro dans les loaders GL ;
  expérience mockée `GL_INFO_LOG_LENGTH = 0`, sous sanitizer si disponible.
  **[[architecture/next-mission|Sélectionné pour la prochaine mission]]**, avant
  Pipeline ; source recoupé, aucun RED/fix exécuté à ce stade. F4/F8/Q1 hors coupe.
- **F6 nommé, non migré** : `IPass` garde un contexte partagé et l'expose par copie ;
  lifetime extensible hors wrapper. Architecture propose de migrer d'abord le
  stockage/les consommateurs du pipeline ; aucune refonte faite aujourd'hui.
- **F7 différé** : `UniformReader` absent d'`OpenGLPassFlow` malgré appel nominal,
  absence silencieuse sous `if constexpr`. Test négatif de concept à écrire dans
  la tranche uniform/attribute ; ce n'est pas un composant aujourd'hui garanti.
- **F8 ouvert** : reader de fichier n'active que `badbit` ; fichier absent peut
  devenir compilation d'une chaîne vide. Test de fichier absent, erreur de lecture
  distincte et contrat `failbit` à instruire.
- **F9 amélioré** : tests supplémentaires d'accès shader/pass moved-from ; couverture
  non exhaustive de toutes les méthodes. Les traits move/noexcept ne prouvent pas RAII.
- **F10 choix ouvert** : `use()` sur programme zéro appelle `glUseProgram(0)` ;
  GL-valide mais désarme le programme plutôt que signaler un pass non chargé.
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

### Réserve du harness ASan : environnement hérité (Q1)

`verify_asan.py` fixe `ASAN_OPTIONS` pour le canari seulement ; `build-windows.ps1`
ne normalise pas l'environnement arbitraire de CTest. Un parent permissif
(`exitcode=0`, diagnostic redirigé) pourrait fausser une autre campagne. Aucun faux
PASS n'a été constaté ici : les métadonnées S05/S06/S10 consignent toutes
`halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`, identique au canari,
sans `log_path`. Réserve non bloquante documentée, pas un durcissement implémenté.

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

## Expériences encore absentes

ASan Windows Debug est exécuté séparément et instrumenté ; pas de résultat
pour les 22 GL non sélectionnés sous ASan, UBSan/TSan ou les autres plateformes.
Les 22 ne sont ni des tests échoués ni des tests abandonnés : la sélection ASan
était ciblée dès le plan ; la suite normale couvre bien les 31 GL.
Les dépendances Conan GLFW/GLEW et le driver restent non instrumentés. Release,
Linux, CI distante, couverture, images et performance restent non validés.

Navigation : [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
[[architecture/correction-plan|Plan de correction]] · [[index|Accueil]].
