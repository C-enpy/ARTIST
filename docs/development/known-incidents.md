---
title: Incidents connus et suivi Shader→Pass
category: development
tags: [artist, cpp, ownership, testing, tooling]
summary: Incidents corrigés, hypothèses rectifiées et expériences discriminantes pour les chemins encore non couverts de Shader→Pass.
created: 2026-10-03
updated: 2026-10-03
sources: [docs/references/session-2026-10-03-shader-pass.md, docs/_meta/session-2026-10-03-validation.json]
---

# Incidents, diagnostics et suivi

## Règle de lecture

Les **122/122** de [[development/build-and-test|la suite finale]] sont une preuve
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

## Contrôle complémentaire prioritaire : assignation move vers owner chargé

Observation de code lors de l'ingestion : `IShader::operator=(IShader&&)` et
`IPass::operator=(IPass&&)` remplacent `m_context` sans libération native explicite
de la destination. Les opérateurs dérivés sont defaulted. Le test de
move-construction ne couvre pas ce chemin ; un handle destination peut être perdu.
Ce point était déjà signalé comme risque dans le checkpoint et n'est pas fermé
par le PASS de la revue. **À reproduire avant de déclarer les moves corrects** :
charger source et destination, assigner par move, vérifier la suppression unique
de l'ancien handle destination puis du handle transféré (mock GL et driver).
Ne pas ajouter un dispatch virtuel de destruction dans la base sans analyser son
ordre et la validité du contexte ; contrat et fix bornés nécessaires.

## Expériences encore absentes

ASan demandé, pas exécuté ; vérifier instrumentation dans un arbre séparé et
ne pas traiter les suites ordinaires comme une preuve sanitizer. Release, Linux,
CI distante, couverture, images et performance restent non validés.

Navigation : [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] ·
[[architecture/correction-plan|Plan de correction]] · [[index|Accueil]].
