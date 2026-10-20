# Ledger K — brut, UNVERIFIED

- **Mission** : chemin d'exécution Pipeline (`docs/architecture/missions/2026-10-19-pipeline-execution-path.md`)
- **Statut** : VERIFIED — recoupement VER-K rendu (addendum en fin de fichier). Ne pas traiter comme décision adoptée tant que la décision humaine 1–2 n'est pas prononcée.
- **Transport** : sous-agent direct `engine-architect` (profil `.opencode/agents/engine-architect.md`), session `ses_ea74e12adffeJNOKHg61zS4peS`, relevé par le lead le 2026-10-20.
- **Pourquoi ce transport** : deux tentatives via le runtime de membre d'équipe (spawn archivé de la session précédente + spawn de cette session) sont revenues inactives sans réponse ; le système a signalé « no completed final response could be recovered ».
- **Anomalies de forme relevées (texte ci-dessous verbatim, non corrigé)** : artefacts de génération (« Let me start again from the text », « rest întârciate », « Échauds », « garante », « citive », « decision isol objection »-typos) — le recoupement doit trancher si la substance autour est affectée.
- **Note d'infrastructure rapportée par l'agent** : MCP qmd en échec serveur pendant la session ; CLI local en secours, une requête interrompue par erreur CUDA de rerank. Les statuts ADR/fiche ont été vérifiés par lecture directe du frontmatter.

---

## Corps verbatim du ledger produit

# LEDGER de décision K — chemin d'exécution Pipeline
**Mission** : `docs/architecture/missions/2026-10-19-pipeline-execution-path.md` · **HEAD source** : `8f4c815` · **Agent** : `engine-architect`, lecture seule · **Aucune édition de fichier effectuée** (aucune source, aucun ADR, aucune promotion de statut). Livrable : ce ledger statique.

> Constat d'infrastructure vérifié : le MCP **qmd a échoué au niveau serveur** (`qmd_1mcp_query` et `qmd_1mcp_status` → erreur d'appel, pas zéro-résultat). Le CLI local a fonctionné en secours (première requête : la fiche, le journal `docs/log.md`, le plan de correction et `docs/index.md` en tête ; deuxième requête interrompue par une erreur CUDA de rerank local). L'identité du vault est confirmée par `.wiki.json` (`vault_path: docs`, `qmd_collection: artist-wiki`). Les statuts ont été vérifiés par lecture directe du frontmatter : fiche, `architecture/next-mission.md`, `adr/0002…` → `status: proposed` / `implementation: not-started` ; `architecture/current-state.md` → `status: observed` (réponse `6d6dcb7`) ; `adr/README.md` : ADR-0002/0004/0006 **Proposed**, ADR-0003 **Accepted/partial**.

## 1. Faits avec chemins/symboles

### 1.1 Intention (fiche + ADR, pas du code)

- `docs/architecture/missions/2026-10-19-pipeline-execution-path.md` : rendre exécutable **la chaîne à deux passes déjà observée** de `PipelineTest` (OpenGL Classic), « appelle les passes dans le même ordre que `use` / `useNext` », sans rouvrir le curseur, le rollback CPU ni `reset` ≠ unbind (§« Résultat attendu », §K). Largeur bornée à cette topologie ; pas d'autre nombre de passes.
- `docs/adr/0002-preparation-et-execution-specialisee.md` (Proposed/not-started) : « types de passes concrets et composition directe », « pas d'effacement en `IPass` entre ces passes uniquement pour les parcourir », et **tolérance explicite d'une période de coexistence et d'un adaptateur froid**. La tranche ici reste sous cette tolérance : le stockage des passes reste effacé (`IPass`), seule la séquence est fermée.
- `docs/architecture/correction-plan.md` étape 2 : chemin concret en coexistence contrôlée, migrer **un consommateur**, pas toutes les primitives.
- `docs/architecture/next-mission.md` : topologie et forme figées par l'architecte puis décision humaine avant tout auteur.

### 1.2 Code observé (HEAD `8f4c815`)

**`ARTIST/include/graphic/pipeline/Pipeline.hpp`**
- `IPipeline<API>` : `m_context` = `std::unique_ptr<API::PipelineContext>` (`:179`) ; injection rejetant un nul → `ERROR::PIPELINE::NULL_CONTEXT` (`:54`) ; accesseur `getContext()` emprunté → `ERROR::PIPELINE::FREED_CONTEXT` sur wrapper déplacé (`:154-161`, message `:158`).
- `use(const int&)` (`:120-137`) : garde d'index **avant** mutation via `getPass` (`:123`, commentaire « use(-1) is invalid »), mémorisation du curseur, mutation, puis hook ; **catch → restauration du curseur + rethrow identique** (`:126-136` — rollback CPU uniquement, jamais backend).
- `useNext()` (`:104-113`) : hasNext → `use(curseur+1)` ; sinon `reset()` + `false`. `hasNext()` (`:92-97`) : comparaison protégée sans `c+1` non borné.
- `reset()` (`:143-146`) : hook uniquement ; machine curseur gardée par le contexte.
- `Pipeline<API, PROFILE>` (`:192-242`) : `requires(API::Validator::validatePipeline<API, PROFILE>())` (`:193`) ; hooks concrets `use(Context&)`/`reset(Context&)` par **`if constexpr`** appelant `API::PipelineContext::User<PROFILE>::on(context)` (`:219-225`) et `Resetter<PROFILE>::on(context)` (`:235-241`). Un degré par appel, pas un lot.
- Chargement des passes : constructeur consommentant `vector<unique_ptr<IPass<API>>>` vers le contexte (`:48-65`).

**`ARTIST/include/graphic/context/PipelineContext.hpp`**
- Stockage : `std::vector<std::unique_ptr<pipeline::IPass<API>>> m_passes` (`:96`), `m_currentPass = -1` (`:97`).
- `getPass(const int&)` (`:67-74`) : garde `index < 0 || size_type(index) >= size` avant tout indexing → `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` (`:71`) ; `vector::at` écarté volontairement ; retours par emprunt, jamais un owner.
- `setCurrentPass` (`:81-88`) : `-1` accepté, tout autre entier validé par `getPass` **avant** mutation. `getCurrentPass` (`:90-93`). Cardinalité isolée `validatePassCountForAddition` (`:31-38`).

**`ARTIST/include/graphic/pipeline/Pass.hpp`**
- `IPass::use()` public → `use(ctx())` (`:112-115`) ; `ctx()` protège le moved-from → `ERROR::PASS::FREED_CONTEXT` (`:244-251`).
- `Pass<API, PROFILE> final` (`:260-324`) : `use(ctx)` → `API::PassContext::User<PROFILE>::on(context)` sous `HasComponent` (`:317-323`). `~Pass` appelle `free()` (`:270-277`).

**`ARTIST/include/graphic/opengl/context/PipelineContext.hpp`** — `OpenGLPipelineContext` (`:19-26`) expose les alias `User`/`Resetter` Classic (`:22-25`).

**`ARTIST/include/graphic/opengl/pipeline/component/pipeline/User.hpp`** — `OpenGLPipelineUser<Classic>::on(Context&)` (`:20-23`) = `context.getPass(context.getCurrentPass()).use()` : **une passe par invocation d'index**, emprunt du contexte, pas de décision d'ordre. Façade `shared_ptr` (`:26-33`) → `ERROR::SHADER::NON_VALID_CONTEXT`.

**`ARTIST/include/graphic/opengl/pipeline/component/pipeline/Resetter.hpp`** — `OpenGLPipelineResetter<Classic>::on` (`:20-23`) = `context.setCurrentPass(-1)` : **aucun appel GL**.

**`ARTIST/include/graphic/opengl/pipeline/component/pass/User.hpp`** — `OpenGLPassUser<Classic>::on(PassContext&)` (`:30-34`) = `glUseProgram(openglContext.getPassID())`. C'est l'**unique site `glUseProgram(` des includes moteur** (grep vérifié, 1 hit).

**`ARTIST/include/graphic/opengl/validator/Validator.hpp`** — `validatePipeline` (`:56-61`) : validation compile-time de l'appelabilité seulement (mission C).

### 1.3 Comportement exécuté (oracles existants, périmètre Windows x64 Debug)

- **Curseur CPU** : `ARTIST/tests/src/graphic/pipeline/PipelineCursorTests.cpp` — oracles exacts d'erreurs (`:26-36`, `:86-91`) ; `InvalidUseNeverCallsUserOrMutatesCursor` (`:133-143`, inclut −1) ; rollback hook leveur (`:145-160`) ; `TwoPassIterationPreservesOrderAndResetsOnlyAfterExhaustion` (`:187-204`, ordre des deux passes sous `InSequence`) ; rollbacks `useNext` (`:206-230`) ; reset sans activation (`:274-285`).
- **Chaîne GL réelle à deux passes** : `ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp` — fixture `makeClassicPass` (2 shaders réels, `:43-49`), `GlLeaseWitness` (`:58-75`), `LoadedPipeline{ClassicPipeline, program0, program1}` (`:105-121`) ; `UsePassTest` (`:151-170`), `UseNextPassTest` (`:172-192` : program0 puis program1, curseur 0→1→−1 après un 3e appel), `ResetPipelineTest` (`:194-214`, F10 : curseur −1, **programme courant inchangé**), `PassesCountTest` (`:216-226`, N=2).
- **F10 sur driver réel** : `component-tests/src/graphic/pipeline/PipelineLifetimeComponentTests.cpp:173-193` `ResetRewindsCursorWithoutUnbindingOrDestroying`. **F10 unitaire** (programme 0 → `glUseProgram(0)` conservé, unbind explicite valide) : `tests/src/graphic/opengl/pipeline/component/pass/UserTests.cpp:49-65`.
- **FREED_CONTEXT** : pipeline `tests/src/graphic/pipeline/PipelineOwnershipTests.cpp:144` ; pass `tests/src/graphic/pipeline/ShaderLifetimeTests.cpp:212,229`.
- **Consommateurs** : grep `useNext(` sur `ARTIST/` → **14 occurrences, uniquement dans `tests/` et `component-tests/`**. Aucun consumer moteur.
- **Baseline historique** : `71dacbb` — 466/466 unit + 64/64 OpenGL (attestation lifetime). « Aucun nouveau run attendu » de la part de K.
- **Absence de doublon** : grep `class .*(Chain|Executor|Runner)` sous `ARTIST/include` → 0 hit. Aucun type d'exécution fermé n'existe.

### 1.4 Frontières (analyse du skill artist-static-boundary)

- **Sélection statique d'un composant** : réelle (`if constexpr` + requires, `Pipeline.hpp:193,219-241`). **Construction d'un plan** : il n'existe ni plan cooké, ni graphe d'authoring, ni graphe de frame dans ce périmètre — rien à construire. **Exécution effective** : activation par passe (virtuel `IPass::use`, fréquence « passe »), curseur (`PipelineContext`), programme courant (objet du driver, `passID` détenu par `PassContext`). La granularité dynamique restante est *par appel d'index* — c'est précisément ce que la boucle future rendrait « par lot/chaîne ». Aucune donnée de frame n'est en jeu ici.

## 2. Cible

Une topologie fermée **N=2**, une forme de coexistence **par emprunt**, un chemin d'effets GL **réutilisé à l'identique**, des diagnostics **réutilisés**, et un **non-arrêt** de la mission avec S non publiée. La cible ne contient **aucun plan cooké, aucun graphe de frame, aucun handle générationnel** : il s'agit d'un excédent structurel (séquence littéralement fixée) posé au-dessus des emprunts runtime existants, en respectant la frontière compile-time/runtime déjà en place de la fiche (§Binding times).

## 3. Écarts (cible − existant)

1. **Aucun artefact n'expose la chaîne fermée** : la seule façon d'exécuter les deux passes est le chemin d'index/cursor de `IPipeline` (une passe par hook call). C'est l'écart que la mission équipe.
2. **L'ordre intra-chaîne n'est pas observable sur les passes réelles** par readback : après une chaîne complète, `GL_CURRENT_PROGRAM` vaut `program1` même si une seule passe a été activée ; l'ordre exige donc un oracle de séquence (mock), la lisibilité finale exige un readback — la fiche prévoit elle-même les deux familles (« oracles CPU du curseur déjà présents, et readback `GL_CURRENT_PROGRAM` »).
3. **Aucun écart** sur le curseur, le rollback CPU, `reset` ≠ unbind, l'ownership des passes, les diagnostics : spécifiés (`current-state` `:81-97, 161-167`) et couverts par les oracles ci-dessus. Non rouverts.
4. **Aucun écart** sur `Pipeline.hpp`, `User.hpp`, `Resetter.hpp` à figer : K ne fige aucun changement de ces fichiers (les 3 restent inchangés par les auteurs).

## 4. Contrat proposé (les 5 décisions)

### Décision 1 — Topologie : **chaîne à deux passes GL de `PipelineTest` retenue comme base de preuve + fixture CPU compagnon obligatoire pour l'ordre**

Retenir `PipelineTest` (fixture `LoadedPipeline`, deux `ClassicPass` chargés réels, `:105-121`) comme base de preuve GL : c'est la topologie « déjà observée » nommée par la fiche ; elle porte les obligations de liaison réelle (`glIsProgram`, readback `GL_CURRENT_PROGRAM`) et de coexistence sur le même harnais lifetime (`GlLeaseWitness`, unbind explicite pré-déstruction). Pas d'autre nombre de passes dans la mission.

**Justification du compagnon CPU** (répond à « ou une fixture CPU plus petite porte les mêmes obligations » : seule, elle ne les porte pas toutes, mais l'ordre seul est CPU-porteur) :
- Oracles mockés (`MockPass<OpenGL>×2`, `InSequence`), déjà utilisés par `PipelineCursorTests`, prouvent l'ordre strict, le curseur-untouched et l'arrêt en échec — obligations **non observables par readback** (écart 2 ci-dessus).
- La base GL seule est insuffisante pour l'ordre ; la fixture CPU seule est insuffisante pour la liaison programs réels. **Les deux, à N=2 identique.**

- *Compile-time* : la longueur 2 et l'ensemble des obligations sont fixés par cette mission et ce ledger ; aucune autre instanciation.
- *Link* : identique à aujourd'hui (header-only, pas d'instanciation explicite).
- *Runtime* : les deux passes restent dans `vector<unique_ptr<IPass>>` du contexte ; la chaîne ne reçoit que des borrows.

### Décision 2 — Coexistence : **forme proposée acceptée, avec les précisions suivantes**

Forme acceptée : un type d'exécution fermé qui emprunte deux `IPass&` déjà possédées, appelle `use()` dans l'ordre des index, ne possède ni passes, ni contexte, ni programme ; ne remplace pas `Pipeline<API, PROFILE>`. Précisions figées par K :

| Propriété | Décision |
|---|---|
| Paramétrage | `template <typename API>` ; **pas de** paramètre `PROFILE` (aucune sélection de composant au niveau de la chaîne : chaque passe encapsule le sien via le virtuel existant `Pass::use(ctx)`, `Pass.hpp:317-323`). Une seule instanciation pratique : `api::OpenGL`. |
| Membres | Deux références `IPass<API>&` stockées dans l'ordre déclaré. **Pas** de tuple, pas de tableau de pointeurs de fonction, pas de paramètre de longueur N, pas d'effacement supplémentaire. La longueur 2 est **structurelle** (membres fixes + séquence littérale dans la méthode unique publique). |
| Ordre | Une seule invocation de chaîne appelle `p0.use()` **puis** `p1.use()` dans cet ordre littéral ; un échec à l'étape k ne lance jamais l'étape k+1 (garantie de base : les activations déjà réussies restent en GL, aucune annulation). |
| Curseur | Le type **ne lit ni n'écrit** le curseur (`getCurrentPass`/`setCurrentPass` interdits dans ce type) ; la machine curseur reste la seule propriété de `IPipeline`/`PipelineContext`. |
| Validité des borrows | Évaluation **lazy** : pas de sonde `getContext()` à la construction (décision explicite, pas un oubli) ; une passe moved-from échoue à l'activation avec `ERROR::PASS::FREED_CONTEXT` réutilisé ; deux références identiques ne sont pas détectées (devoirs de l'appelant, pas de check runtime). |
| Cache de handle | Interdit : aucune copie de `passID` dans le type ; chaque activation repasse par `pass.use()` (les gardes RELOAD_WITHOUT_FREE restent inchangées). |
| Divers | Type indépendant de `IPipeline` (aucun héritage, aucun membre de contexte) ; marquage de non-ownership dans les commentaires de style voisin. Nom/fichier exacts à l'auteur (style voisin, placement `ARTIST/include/graphic/pipeline/` recommandé puisque le type est API-générique ; une déviation `opengl/` n'est pas acceptable puisque le type n'a aucune dépendance GL). `Pipeline.hpp`, `User.hpp`, `Resetter.hpp` ne changent pas. |

- *Compile-time* : séquence et longueur deviennent statiques — c'est le **seul** nouveau contenu statique de la mission. Aucun concept nouveau requis.
- *Link-time* : rien de figé ; LTO pourrait dévirtualiser le virtuel `IPass::use` du même TU ; **aucun gain revendiqué** à ce stade.
- *Runtime* : borrows, curseur, programme courant restent runtime, conformes aux binding times de la fiche.
- **Alternatives rejetées** : (a) exécuteur pas-à-pas type `use(0)`/`use(1)` — reviendrait à réintroduire l'indexation appartenant à `IPipeline` et à dégrader le type en indirection pure sur `pass.use()` (aucun nouveau contrat) ; (b) « aucun type nouveau, le chemin actuel suffit » — rejeté comme sortie de mission : comportementalement vérifiable, mais la fiche fait de cet artefact coexistence le livrable de l'étape 2, avec un périmètre qui ne contredit aucun invariant et ne demande aucune refonte ; (c) tuple générique N-paramétré — l'alternative mise en garde d'ADR-0002 (« multiplie les variantes »), hors mission.

### Décision 3 — Effets GL : **réutilisation exacte d'`IPass::use()`**

Le chemin concret ne fait que déclencher la pile existante : `IPass::use()` → `Pass::use(ctx)` → `OpenGLPassUser<Classic>::on` → `glUseProgram(getPassID())` (unique site moteur confirmé). Décisions figées :
- **Aucun** nouveau `glUseProgram`, aucun reset, aucun rollback GPU, aucun `glUseProgram(0)` spécialisé ajouté au type ;
- une passe non chargée (passID=0) activée par la chaîne suit la conservation documentée F10 (unbind explicite valide — oracles `UsePass_ZeroPassIDStillCallsGL`, `UserTests.cpp:49-65` ; et driver réel `ResetRewindsCursorWithoutUnbindingOrDestroying`, `:175-193`) ; **ne pas** lui ajouter un rejet ;
- `reset` conserve le `setCurrentPass(-1)` seul (`Resetter.hpp:20-23`) ; F10 reste sur `IPipeline::reset`, non utilisé par le nouveau type ;
- précondition documentée inchangée : un contexte GL courant et compatible est requis pour toute activation/destruction côté driver (déjà documentée côté passes).

### Décision 4 — Erreurs : **réutilisation, aucune enveloppe, aucune sonde**

Liste des canaux réutilisés (tous dotés d'oracles) :
- `ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE` (`PipelineContext.hpp:71` ; oracle `PipelineCursorTests:86-91`) — atteint via les accesseurs du contexte (`getPass`, fixtures) ; **le nouveau type n'introduit aucun accès index** (forme 2, chaîne par refs) ;
- `ERROR::PIPELINE::FREED_CONTEXT` (`Pipeline.hpp:158` ; oracle `PipelineOwnershipTests.cpp:144`) — wrapper déplacé, inchangé ;
- `ERROR::PASS::FREED_CONTEXT` (`Pass.hpp:248` ; oracle `ShaderLifetimeTests.cpp:212,229`) — propage à travers la chaîne en interrompant aux étapes ;
- rejet de `use(-1)` (`Pipeline.hpp:123` ; oracle `PipelineCursorTests:137-142`) — chemin `IPipeline` inchangé ;
- `ERROR::SHADER::NON_VALID_CONTEXT` sur les façades `shared_ptr` (`User.hpp:28-31`) — inchangé.

**Interdictions** : aucun `try/catch` dans le nouveau type, aucun rethrow/enveloppement de l'exception étrangère, aucun nouvel identifiant de message sans oracle préalable (différent d'un RED établi), aucun nouveau type d'exception.

### Décision 5 — Arrêt : **non déclenché ; la mission continue**

Évaluation des conditions d'arrêt de la fiche : la forme acceptée n'est **ni** une refonte du dispatch (aucune table de virtuels `use` modifiée, aucune construction statique via concepts plus forte que l'existant), **ni** un render graph (pas de plan immuable, pas de barrière pronostiquée), **ni** une adoption d'ADR-0002/0004 (restent Proposed, adoption explicite séparée par une nouvelle décision humaine). La chaîne reste dans la période de coexistence tolérée par ADR-0002.

**S non publiée** : aucune obligation du chemin actuel n'est découverte sans oracle — inventaire exhaustif ci-dessus (1.3 + 4). Aucun RED à fabriquer : les tests `X` auront l'ordre et le curseur-untouched du **nouveau type** comme source RED (le type n'existe pas encore, donc « zéro couverture » est l'état attendu, comme la fiche le note explicitement).

## 5. Conséquences et hypothèses

**Conséquences.**
1. Le type fermé est le **seul** nouveau fichier moteur ; `Pipeline.hpp`, `User.hpp`, `Resetter.hpp`, `PipelineContext.hpp`, `Pass.hpp` ne changent pas.
2. Réversibilité : suppression du header + de ses tests pour revenir à l'état actuel ; zéro consumer migré.
3. La granularité : l'ordre fixe et la longueur deviennent structure ; toute topologie ultérieure (N autre, ordre par frame, variété par lot) devra repasser par une mission (largeur stoppée ici par la fiche).
4. Le virtuel par passe (`IPass::use`) demeure — **coût constaté, jamais revendiqué comme gain** ; aucune mesure de performance, aucun test de dévirtualisation, aucun protocole LTO n'est dans le périmètre.
5. ADR-0002/0004/0006 **restent Proposed** ; K n'en modifie aucun statut.
6. La chaîne ne délègue pas : elle n'est **pas** à devenir le hook de `Pipeline::use(Context&)` (changement de comportement dynamique réels : un hook `use(index)` activerait deux passes), pas plus que le `Resetter`.

**Hypothèses (limites de la découverte).**
- Aucun consumer moteur de `useNext`/`use(index)` existe (vérifié par grep, 1.3) — donc aucune surface de migration cachée.
- Chaîne = tranche de topologie ne couvre que N=2 ; tout N≠2 ou toute variabilité de topologie est un cas non couvert par le type (hors mission, mission future).
- Le type fermé API-générique est dit « fermé » par sa **séquence**, pas par son **type de passe concret** (le virtuel demeure) — cela reste dans la tolérance de coexistence d'ADR-0002, pas dans sa cible terminale (tuple/concrets), explicitement hors périmètre.
- LTO/dévirtualisation et coût machine de build sont non revendiqués et non mesurés.
- Baseline `71dacbb` (466+64) reste historique tant que le run candidat n'a pas été refait par la suite de la mission.

## 6. Validation (ce qui reste à prouver par les tests X)

Ordre de cause attendu : le type n'existe pas encore ⇒ **RED causal sur l'ordre/le curseur du nouveau type avant sa création** (fixtures CPU pour l'ordre, puis implémentation minimale GREEN, puis non-régression). Critères :

| # | Obligation | Fixture | Critère observable |
|---|---|---|---|
| V1 | Ordre strict | CPU (`MockPass×2`, `InSequence`) | `use()` appelé exactement une fois par passe, en ordre 0 puis 1 ; unstable sinon |
| V2 | Fin de liaison réelle | GL (fixture `PipelineTest`, mêmes `program0/program1`) | après une chaîne complète `GL_CURRENT_PROGRAM == program1` ; curseur inchangé |
| V3 | Curseur untouched | CPU + GL | `getCurrentPass()` identique avant/après la chaîne |
| V4 | Redémarrabilité | CPU | deuxième invocation = même ordre, aussi valide sans reset de curseur |
| V5 | Échec à l'étape 2 | CPU | passe 1 lève → passe 0 reste activement exercée, exception propagée identique, curseur inchangé |
| V6 | Moved-from | CPU | `run()` lève `ERROR::PASS::FREED_CONTEXT` au premier accès, passe 2 jamais appelée |
| V7 | F10 non-régression | GL (`PipelineLifetimeComponentTests`) | reset = curseur −1, programme courant inchangé, chemin `IPipeline::reset` inchangé |
| V8 | Handles GL réels d'existence | GL (fixture `PipelineTest`) | programmes 0/1 restés valides durant toute la chaîne ; unbind pré-destruction par le test (GlLeaseWitness) |
| V9 | Non-régression | unit + opengl (lead) | suites vertes au même périmètre que la baseline mesurée au début de K (aucun run par K) |

Pas de nouveau concept ⇒ pas de sonde de compilation négative obligatoire ; pas d'oracle manquant ⇒ S non publiée ; pas de nouveau message d'erreur ⇒ aucune obligation de documentation d'erreur.

**Impact sur le graphe de mission** : Q (qa-planner) peut partir de ces critères ; X (cpp-engineer) a un périmètre d'un fichier + tests, avec `Pipeline.hpp`/`User.hpp`/`Resetter.hpp` **gelés sans changement** ; I (lead) intègre X seul ; G (gap tests) en parallèle de R (correctness-reviewer) ; V (test-reviewer) fresh sur le candidat. Les files uniques par fichier sont respectées.

**Séparation des trois plans demandée par la fiche** :
- **Intention** : fiche + ADR-0002 (status `proposed` / `not-started`) — cible, pas encore adoptée.
- **Code observé** : tous les chemins/symboles ci-dessus, à `8f4c815`, attestés par lectures directes (passes, curseur, rollback, F10, ordre, erreurs).
- **Comportement exécuté** : attesté par les suites historiques `71dacbb` (466/466 unit, 64/64 OpenGL) et les oracles GMock/readback listés — K n'a **exécuté aucun run** et n'en réclame aucun.

**Fin du ledger K — aucune autre décision ouverte. Recevable pour une décision humaine sur les points 1 et 2 avant Q.**

---

## Addendum — Recoupement VER-K (2026-10-20)

- **Vérificateur** : sous-agent direct `engine-architect` (contexte frais, lecture seule), session `ses_ea72fc374ffewhmoQT9BlZlHsH`, exécution interrompue puis complétée après reprise. Aucune édition, aucun run. `git rev-parse` vérifié : `8f4c81599b089cca6b38b553873783e609986627`.
- **Verdicts** : C1, C2, C3, C4, C7, C8, C9 **CONFIRMED** (citations exactes) ; C5 **CONFIRMED avec un sous-détail REFUTED** ; C6 **CONFIRMED en substance, comptage corrigé**.
- **Correction 1 (C5)** : `UseNextPassTest` (`ProgramTests.cpp:172-192`) fait exactement 2 appels `useNext()` (`:180`, `:186`), asserte le curseur **1** après le second (`:189`) puis `glUseProgram(0)` `:191` — il ne contient **pas** de « 3e appel → curseur −1 ». Ce comportement est couvert par l'oracle CPU `PipelineCursorTests.cpp:187-204` (`TwoPassIterationPreservesOrderAndResetsOnlyAfterExhaustion`, `:202-203`). Substance de la décision 1 intacte — le besoin du compagnon CPU pour l'ordre est **renforcé** ; attribution corrigée.
- **Correction 2 (C6)** : comptage `useNext(` = **15 occurrences** (12 `tests/`, 2 `component-tests/`, 1 déclaration `include/` `Pipeline.hpp:104`). Conclusion « aucun consumer moteur » inchangée.
- **Recoupements annexes confirmés** : oracles `FREED_CONTEXT` (`PipelineOwnershipTests.cpp:144-145`, `ShaderLifetimeTests.cpp:210-212,227-229`) ; oracle exact `PASS_INDEX_OUT_OF_RANGE` (`PipelineCursorTests.cpp:86-91` ↔ `PipelineContext.hpp:71`) ; rollback hook (`:145-160`) ; reset sans activation (`:274-285`, `Times(0)`) ; citations verbatim ADR-0002 (`:34-36`, `:56-57`) et correction-plan (`:153-158`) ; `.wiki.json` conforme ; baseline `71dacbb` (466+64) présentée comme historique, aucun run revendiqué.
- **Réserves non bloquantes à endosser par la décision humaine** :
  - **(e) Placement** : le ledger durcit (« déviation `opengl/` non acceptable ») alors que la fiche laisse « nom et fichier exacts à l'auteur » (`fiche:106`). Fondement factuel correct (type `template<typename API>` sans include GL, `Pipeline.hpp` déjà dans `graphic/pipeline/`) — un resserrement à endosser consciemment.
  - **(d) Obligation ajoutée** : « échec à l'étape k ⇒ étape k+1 jamais lancée » (V5) n'est pas littéralement dans la fiche — ajout cohérent du ledger, à endosser.
- **Avis** : **RECEVABLE** pour la décision humaine sur la topologie (1) et la forme de coexistence (2). Aucune contradiction fiche↔code↔ADR sur les cinq décisions ; frontmatters fiche/ADR-0002 inchangés (`proposed`/`not-started`) ; les artefacts de génération en en-tête n'affectent aucune affirmation substantielle recoupée.

---

## Décision humaine (2026-10-20) — points 1 et 2

- **Décision 1 (topologie) : APPROUVÉE** — chaîne à deux passes GL de `PipelineTest` comme base de preuve + fixture CPU compagnon obligatoire pour l'ordre ; N=2 exclusif.
- **Décision 2 (forme de coexistence) : APPROUVÉE** — type d'exécution fermé par emprunt tel que corrigé par VER-K, **y compris l'obligation ajoutée (d) « échec à l'étape k ⇒ étape k+1 jamais lancée » (V5)**.
- **Réservation (e) : ENDOSSEE** — placement durci : fichier du nouveau type imposé sous `ARTIST/include/graphic/pipeline/` ; l'auteur garde le nom exact et le style voisin ; une déviation `opengl/` sera refusée.
- **Décision 5 (arrêt) : non déclenchée** — la mission continue ; S non publiée.
- **Corrections intégrées au recoupement** : C5 (oracle « 3e appel → −1 » attribué à `PipelineCursorTests:187-204`) ; C6 (comptage 15, conclusion inchangée).
- **Décision rendue en chat le 2026-10-20 par l'opérateur** (gate de mission distincte de l'approbation de politique) ; suite : Q (qa-planner) → commit du plan QA → X.
