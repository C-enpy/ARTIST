# QA-PLAN-X — chaîne d'exécution fermée N=2 (livrable Q, brut)

- **Statut** : PERSISTÉ — le lead le commit **avant tout worktree auteur X** (barrière QA). Section « Décisions humaines » en addendum après tranchage chat.
- **Transport** : sous-agent direct `qa-planner` (profil `.opencode/agents/qa-planner.md`, deny-all + read/glob/skill), session `ses_ea70618b5ffeacB8J6s9aZ3xmM`, 2026-10-20, relevé par le lead. Tâche board `task_mvgkmdol_0002_2k23u8` (58/64 → standard).
- **Pourquoi ce transport** : runtime de membre Ensemble en échec 5/5 pour les profils restrictifs `final_response` (4× engine-architect sur cursor, 1× qa-planner sur openai) ; substitution signalée, aucune capture `final_response` d'équipe fiable en l'état.
- **Anomalies de forme** (verbatim, non substantielles) : « glUse Program(0) » (espace parasite, §2 O8), « desctruction » (R3) — le recoupement V relira le plan sur le candidat.

---

## Corps verbatim du livrable Q

# Pin observé

| Point | Vérification faite par moi (lecture seule) | Statut |
|---|---|---|
| HEAD `8f4c815` | `.git/HEAD` → `refs/heads/main` ; `.git/refs/heads/main` → `8f4c81599b089cca6b38b553873783e609986627` — égal au pin et au ledger K (VER-K) | **Match** |
| Répertoire d'évidence | `.opencode/evidence/mission-pipeline-execution-path/` contient uniquement `ledger-K.md` ; `qa-plan-X.md` sera un fichier nouveau (rien à écraser) | Match |
| Statuts ADR | `docs/adr/README.md` : 0001/0002/0004/0006/0007 Proposed, 0003 Accepted/partial, 0005 partial, 0008 verified Windows Debug | Match pin courant |
| Fichiers gelés | Les 5 headers (`Pipeline.hpp`, `User.hpp`, `Resetter.hpp`, `PipelineContext.hpp`, `Pass.hpp`) lus à HEAD, conformes aux citations du ledger | Match |
| Diff docs-only hors `ARTIST/` | **Non vérifiable par moi** (aucun shell/git autorisé) — déclaration du lead, endossée en tant que telle | lead-déclaré |
| Toolchain (MSVC 19.44/14.44, Conan 2.12.2, baseline `71dacbb` 466+64 historique) | Canon documentaire (`docs/development/build-and-test.md`, ADR-0008, skill) — historique, pas re-exécuté ; le lead mesure la baseline du candidat en début de X | lead-déclaré |

Aucun écart de fond entre pin, dépôt et ledger. Le pin périmé du profil (2026-10-16, comptages 109/122/299) est remplacé par celui-ci ; ses totaux restent historiques et ne sont jamais un attendu de cette mission.

---

# PLAN NOMMÉ — « QA-PLAN-X : chaîne d'exécution fermée N=2 »

**Chemin proposé au lead** : `.opencode/evidence/mission-pipeline-execution-path/qa-plan-X.md` — persisté puis **commité avant tout worktree auteur X**. S est non publiée (aucun oracle manquant) : ce plan couvre X seul.

Candidat du lot X (décision humaine 2026-10-20, ledger K décisions 1–2 approuvées) : un **unique nouveau header moteur** sous `ARTIST/include/graphic/pipeline/` (placement imposé, décision (e) endossée — une déviation `opengl/` est refusée), `template<typename API>`, deux membres `IPass<API>&`, séquence littérale `p0.use()` puis `p1.use()`, sans curseur, sans cache `passID`, validité lazy, échec à l'étape k ⇒ étape k+1 jamais lancé. Fichiers gelés vérifiés à HEAD : `ARTIST/include/graphic/pipeline/Pipeline.hpp`, `Pass.hpp`, `ARTIST/include/graphic/context/PipelineContext.hpp`, `ARTIST/include/graphic/opengl/pipeline/component/pipeline/User.hpp` et `Resetter.hpp` — **aucun changement attendu** (empreintes à relever par le lead à l'intégration).

## 1. Obligations — explicites, inférées, non-obligations

**[Fait]** = source directe lue (ledger K recoupé VER-K, fiche, code à HEAD). **[Inférence]** = déduit, pas une spécification. **[Recommandation]** = avis du planner, décision au lead.

### Explicites (V1–V9 du ledger + décisions approuvées)

| # | Obligation (intent) | Source |
|---|---|---|
| O1 (V1) | Ordre strict : une invocation lance `p0.use()` **puis** `p1.use()`, exactement une fois chacun | Ledger déc. 2 « Ordre », V1 ; fiche « même ordre que use/useNext » |
| O2 (V2) | Fin de liaison réelle : après une chaîne complète, `GL_CURRENT_PROGRAM == program1` (mêmes programmes que `PipelineTest`) | Fiche §Preuve ; V2 |
| O3 (V3) | Curseur untouched : `getCurrentPass()` identique avant/après ; le type ne lit ni n'écrit le curseur | Ledger déc. 2 « Curseur » ; V3 |
| O4 (V4) | Redémarrabilité : 2ᵉ invocation = même ordre, valide sans reset de curseur | V4 |
| O5 (V5) | Échec à l'étape k ⇒ étape k+1 jamais lancée ; exception propagée **identique** (aucun catch/enveloppe dans le type) ; obligations (d) endossées | Ledger déc. 2 + décision humaine ; déc. 4 ; V5 |
| O6 (V6) | Validité lazy : passe moved-from ⇒ `ERROR::PASS::FREED_CONTEXT` **au premier accès** (pas de sonde à la construction), étape suivante jamais lancée | Ledger déc. 2 « Validité des borrows » ; V6 |
| O7 (V7) | F10 non-régression : `IPipeline::reset` = curseur −1, programme courant inchangé ; chemin inchangé | Ledger déc. 3 ; V7 |
| O8 (V8) | Handles GL réels valides durant toute la chaîne ; unbind pré-destruction par le test | V8 ; déc. 1 (GlLeaseWitness) |
| O9 (V9) | Non-régression : suites `unit` + `opengl` vertes au périmètre de la baseline mesurée en début de X | Fiche §Preuve ; V9 |
| O10 | Placement : nouveau fichier sous `ARTIST/include/graphic/pipeline/` | Décision humaine (e) endossée |
| O11 | Aucun cache `passID` ; chaque activation repasse par `pass.use()` ; aucun accès curseur dans le type | Ledger déc. 2 « Cache de handle » + « Curseur » |
| O12 | Erreurs réutilisées uniquement : aucun nouvel identifiant de message, aucun type d'exception, aucun try/catch/wrap | Ledger déc. 4 « Interdictions » |
| O13 | Type indépendant d'`IPipeline` (ni héritage, ni membre de contexte) ; pas le hook de `Pipeline::use(Context&)`, pas le Resetter | Ledger déc. 2 « Divers » + conséquence 6 |

### Inférées (pas des spécifications — arbitrages proposés)

| # | Inférence | Source d'inférence |
|---|---|---|
| I-A [Inférence] | « Les activations déjà réussies restent en GL » (pas de rollback GPU) est la moitié **comportementale** de O5 ; à GL réel, aucune injection d'échec n'est possible dans une `ClassicPass` valide (aucun chemin C++ lançant de `glUseProgram`) — preuve par revue statique + readback nominal, pas par oracle runtime | Ledger déc. 2 (littéral) ; constat technique sur `Pass.hpp:112-115`/`User.hpp` (pass) : aucun `throw` possible sur passe valide |
| I-B [Inférence] | Chaîne avec passe non chargée (`passID=0`) : interdiction d'un rejet ajouté ⇒ un test GL discriminant serait possible (p1 non chargée en fin de chaîne ⇒ `GL_CURRENT_PROGRAM == 0`, pas d'exception) | Ledger déc. 3 (« ne pas lui ajouter un rejet ») ; précédent `UserTests.cpp:49-65` |

### Non-obligations (pour empêcher tests orphelins et scope creep)

- **Pas de détection de références identiques** (devoirs de l'appelant) — tout test l'assertant contredit la décision : ledger déc. 2 « Validité des borrows ». **[Fait]**
- **Pas de N ≠ 2**, pas de topologie variable, pas de consumer migré (15 occurrences `useNext(` = 12 tests, 2 component-tests, 1 déclaration `Pipeline.hpp:104` — aucun consumer moteur) : ledger C6 corrigé, fiche §Hors périmètre. **[Fait]**
- Pas d'autre API qu'`api::OpenGL` instanciée en pratique (ledger déc. 2). **[Fait]**
- Pas de sonde de compilation négative obligatoire (aucun concept nouveau), pas de mesure performance/LTO/dévirtualisation, pas d'ASan, pas de Release/autre OS, pas de mocks communs ajoutés : ledger §6 ; fiche §Hors périmètre. **[Fait]**

## 2. Scénario nominal + négatif + oracle discriminant par obligation

Niveaux : **CPU** = `ArtistLib_tests`, unitaires avec `MockPass<OpenGL>` (`ARTIST/tests/mock/graphic/pipeline/MockPass.hpp`, données existantes réutilisées) ; **GL** = `ArtistLib_component_tests`, contexte GLFW/GLEW 3.3 core réel (`OpenGLComponentTests.hpp`), readback `GL_CURRENT_PROGRAM`/`glIsProgram`. Les nouveaux tests sont **écrits par X** (TDD RED→GREEN) — ce plan les spécifie, ne les écrit pas.

| Obligation | Nominal | Négatif (oracle discriminant) |
|---|---|---|
| O1 CPU | Chaîne sur borrows de 2 `MockPass` (via un vrai `Pipeline<OpenGL, Classic>` et ses `forPass(0)/forPass(1)`) : `InSequence`, chaque `use()` `Times(1)` | Toute inversion, double activation ou saut ⇒ violation `InSequence`/`Times` ⇒ GMock FAIL ; un mock-GL sur `OpenGLPassUser` (déjà couvert `UserTests:35-47`) est un doublon interdit |
| O2 GL | `LoadedPipeline` + borrows, une invocation, readback = `program1` | Un ordre inversé aboutirait à `program0` ; « p0 seul » aussi ⇒ readback seul ne prouve pas l'ordre (d'où le compagnon CPU obligatoire — [Fait] ledger §3 écart 2) |
| O3 CPU+GL | Curseur capturé avant l'invocation, identique après. **Deux pré-états** : 0 **et** 1 | Une chaîne déléguant à `use(index)` (alternative rejetée) laisse le curseur à 1 depuis pre=1 (indiscerné) mais le passe de 0→1 depuis pre=0 ⇒ **pré-état 0 indispensable** [Inférence, contrainte d'oracle] |
| O4 CPU | 2ᵉ invocation : même ordre, même validité | ⚠ GMock : `EXPECT_CALL(p0).Times(2)` + `InSequence` naïf encode p0,p0,p1,p1 — **faux RED/GREEN** ; exiger un encodage séquencé par invocation (p0,p1,p0,p1) [Fait sémantique GMock, contrainte d'oracle] |
| O5 CPU (k=1 / k=2) | k=2 : p0 `Times(1)`, p1 `WillOnce(Throw)` ⇒ même exception propagée | k=1 : p0 lève ⇒ **aucune attente sur p1** : tout appel p1 ⇒ GMock FAIL inattendu ; message exact recollé au pattern `expectExactPipelineError`/`expectSpecificError` (`TestUtils.hpp`, `PipelineCursorTests:26-36`) |
| O6 CPU | Passe valide : aucun rejet à la construction de la chaîne (lazy) | `ClassicPass` réel moved-from (p0) ⇒ `ERROR::PASS::FREED_CONTEXT` exact au premier accès, p1 jamais appelée ; variante p1 moved-from (p0 activé via `glUseProgram_mock(0)` — précédent F10 mock) ; ⚠ `MockPass::use()` mocké **ne traverse jamais `ctx()`** ⇒ l'oracle V6 exige des passes réelles, pas des mocks [Inférence vérifiée sur `MockPass.hpp:26` + `Pass.hpp:244-251`] |
| O7 GL | Chaîne puis `pipeline.reset()` : curseur −1, readback `program1` inchangé (interaction chaîne×F10) | Existants : `ProgramTests.ResetPipelineTest:194-214`, `PipelineLifetimeComponentTests.ResetRewindsCursorWithoutUnbindingOrDestroying:175-193` (driver réel) — pas à réécrire |
| O8 GL | `glIsProgram(program0/1)` vrais avant/après ; `GlLeaseWitness` ; `glUse Program(0)` explicite avant destruction des owners | Toute destruction d'owner après `TearDown` ⇒ échec du witness (précédent RED réel A1c) |
| O9 | Lead : baseline mesurée début X, puis `unit` + `opengl` du candidat intégré | Taux : 0 FAIL, 0 skip requis ; `--no-tests=error` actif ; zéro test découvert ≠ PASS |

Rappel périmètre : **MOCK_GL/GMock ne prouvent pas le GPU** — `UserTests.cpp` (`#ifdef __mock_gl__`, `glFunctionMock`) qualifie le comportement composant au niveau mock seulement ; readback **GL réel requis** pour O2/O7/O8 ; labels réels `unit`/`opengl` ; verrou `artist_opengl` exclusif (tests GL sérialisés, `--parallel 1`) ; mocks communs et couverture égale partout hors contrat.

## 3. Registre bidirectionnel

**Sens obligation → preuve** (`fichier:symbole`, raison de la preuve) :

| Obligation | Preuve | Raison | État |
|---|---|---|---|
| O1 | À écrire X (CPU) ; réf. : `PipelineCursorTests.TwoPassIterationPreservesOrderAndResetsOnlyAfterExhaustion:187-204` (chemin `IPipeline`, pas le type) | Ordre non observable par readback | **GAP** (owner X, RED attendu) |
| O2 | À écrire X (GL) ; réf. : `ProgramTests.UsePassTest/UseNextPassTest:151-192` | Readback requis | **GAP** (owner X) |
| O3 | À écrire X (CPU pré-états 0 et 1 + GL) ; réf. machine curseur : `PipelineCursorTests:133-143` | Délégation cursor-ward indétectable sinon | **GAP** (owner X) |
| O4 | À écrire X (CPU) | Idempotence d'invocation | **GAP** (owner X) |
| O5 | À écrire X (CPU k=1/k=2) + revue R statique ; réf. pattern : `PipelineCursorTests:145-160,206-230` | Propagation + non-lancement | **GAP** ; demi « reste en GL » → **GAP-accept proposé** (décision ouverte n°3) |
| O6 | À écrire X (unit `__mock_gl__`, passes réelles) ; réf. : `ShaderLifetimeTests.MovedFromPassAccessThrows:200-213` | Lazy + propagation | **GAP** (owner X) |
| O7 | `ProgramTests.ResetPipelineTest:194-214` + `PipelineLifetimeComponentTests:175-193` (driver réel) ; proposition X : chaîne→reset | F10 gelé, fichiers inchangés | **COVERED** (static-presence à HEAD + executed historique `71dacbb` ; re-confirmation pendante via O9) |
| O8 | À écrire X (GL) ; réf. : `PipelineCreationTest:133-149`, `GlLeaseWitness:58-75` | Existence handles | **GAP** (owner X) |
| O9 | Lead — commandes canon (`build-windows.ps1 -TestLabel unit -Jobs 1` ; `ctest -L unit` ; `ctest -L opengl --parallel 1`, `--no-tests=error`) | Preuve d'exécution du candidat | **GAP-defer** (owner lead ; échéance : début X pour baseline, post-I pour candidat ; impact : verrouille tout PASS global) |
| O10 | Persistance/revue statique lead (fichier sous `graphic/pipeline/`) | Décision (e) | **GAP-defer** (owner lead, à la persistance/commit) |
| O11–O13 | Revue `correctness-reviewer` R fresh post-intégration (statique : pas de cache, pas de try/catch, pas de curseur, pas de membre contexte) | Sans oracle runtime possible | **GAP-defer** (owner R, après I) |
| O14 (I-B) | Proposition : 1 test GL (p1 `passID=0` ⇒ readback 0, pas d'exception) | Enforce « pas de rejet ajouté » (déc. 3) | **Proposition — décision ouverte n°1** |

**Sens preuve → obligation** :

- Aucun oracle existant n'est orphelin : tous couvrent le chemin `IPipeline`/`Pass` de référence ; les fichiers gelés les maintiennent valides — **mais aucun ne couvre le nouveau type** : « zéro couverture de la chaîne » est l'état RED initial attendu (ledger §6), pas un succès.
- `PipelineBorrowedContractTests.cpp:44-57` (concept `OpenGLPipelineFlow` sur composants) — **ne pas le citer comme preuve de la chaîne** : il contracte les composants `User`/`Resetter` gelés, pas le nouveau type (pas de concept requis pour la chaîne).
- Preuves périmées (à ne jamais invoquer comme preuves du candidat) : baseline `71dacbb` 466/466 + 64/64 ; 122/122 (Shader→Pass) ; 299/299 F7 `6d6dcb7` ; 109/109 (2026-10-03) — historiques, candidats différents.
- Doublons faibles interdits pour X : réécrire `use(-1)` (`PipelineCursorTests:133-143`, couvert), doubler `UsePass_ZeroPassIDStillCallsGL` (`UserTests:49-65`), retester la machine curseur seule, un mock-GL de `OpenGLPassUser`. Le sujet discriminant de chaque nouveau test est **le type de chaîne**.
- Citation corrigée (C5, VER-K) : `ProgramTests.UseNextPassTest:172-192` fait **2** appels `useNext()` (curseur 1 après le second, `glUseProgram(0)` :191) ; l'oracle « 3ᵉ appel → −1 » appartient à `PipelineCursorTests:187-204` (:202-203) — ne pas attribuer autrement.

## 4. Top 3 risques

**R1 — Ordre / arrêt / intégrité des erreurs de la chaîne — I=4, P=3 → 12 (haut).** Mode : mauvais ordre, double activation, lancement de k+1 après échec de k, ou exception enveloppée. Déclencheur : implémentation du type par X (boucle, try/catch, séquence non littérale). Blast radius : programme GL courant faux pour tout draw consommateur ; rupture du contrat d'équivalence avec `use`/`useNext` ; rollback CPU amont déguisé. Détection actuelle : **aucune** pour le nouveau type (n'existe pas — « Trou » de la fiche) ; oracles existants = chemin `IPipeline` seulement. Mitigation actuelle : décisions 1–2 gelées, RED causal exigé, compagnon CPU obligatoire. Gap : tests V1/V5 à écrire par X (planifié — se referme au GREEN).

**R2 — Interférence curseur / délégation à `IPipeline` — I=3, P=3 → 9 (haut).** Mode : le type lit/écrit le curseur ou délègue à `pipeline.use(index)` (alternative rejetée (a) du ledger). Déclencheur : commodité d'implémentation. Blast radius : invariants `c ∈ {-1} ∪ [0, N)` brisés pour les consommateurs ; hook `IPipeline` déclenché en double comptabilité. Détection actuelle : aucune pour le type ; V3 (planifié) + revue R statique. Mitigation actuelle : interdiction écrite (déc. 2 « Curseur », conséquence 6). Gap : discrimination V3 exige le pré-état 0 **et** 1 (sinon faux-vert depuis pre=1) — contrainte posée en §2.

**R3 — Lifetime des emprunts et discipline GL du harnais — I=4, P=2 → 8 (haut).** Mode : emprunt pendant un owner détruit, owner GL détruit après `TearDown`, unbind manquant pré-destruction ; AV/UB. Déclencheur : ordre de déclaration/destruction de la fixture (précédent réel : la sonde RED A1c a échoué exactement là, d'où `GlLeaseWitness`). Blast radius : crash de suite, faux signal, flakiness GPU. Détection actuelle : `GlLeaseWitness` + unbind explicite ; ASan **hors périmètre de ce lot** (fiche). Mitigation actuelle : réutiliser la discipline `PipelineTest` (owners dans le corps du test, créés après SetUp, détruits avant TearDown). Gap : discipline à porter par les tests GL de X ; l'absence d'ASan ce lot est un écart de détection assumé → décision ouverte n°3.

## 5. États — récapitulatif qualifié

- **GAP** (executed attendu, owner X, RED) : O1, O2, O3, O4, O5-branches CPU, O6, O8.
- **COVERED** : O7 — static-presence des oracles à HEAD (lus) + executed historique `71dacbb` ; re-confirmation par O9 ; jamais qualifié PASS sur la seule historicité.
- **GAP-defer** : O9 (lead, baseline début X + non-régression post-I) ; O10 (lead, trivial) ; O11–O13 (revue R fresh, post-intégration).
- **GAP-accept proposé** : demi-obligation I-A « bindings réussis restent en GL » (aucun oracle runtime sans injection artificielle ou mocks communs interdits).
- **unassessed** : aucune obligation matérielle laissée non évaluée ; les non-obligations sont listées en §1 pour empêcher le scope creep.
- **blocked** : rien — le lot démarre par RED par conception.

## 6. Budget

**[Inférence sur base déclarée]** Fiche : réalisation X 12–20 h-p. Exemple sur 16 h-p (hypothèse illustrative, à recaler par le lead) :
- **Setup — 2,4 h (15 %)** : fixtures **existantes** réutilisées (`MockPass.hpp`, `TestUtils.expectSpecificError`, fixture `PipelineTest`/`LoadedPipeline`/`makeClassicPass`, CMake/C test discovery) — bas pour un lot : réutiliser, ne pas inventer une API de fixture. Re-glob/re-config CMake si nouveaux fichiers de tests.
- **Restant 13,6 h → planifié 80 % (≈ 10,9 h ; 68 % du total), buffer 20 % (≈ 2,7 h ; 17 % du total)** — dans la fourchette 70–80/20–30.
- Ordre risque×effort : d'abord le **compagnon CPU** (haut risque, peu coûteux : O1, O3×2, O4, O5 k=1/k=2, O6 ≈ 5,5) ; puis **GL** (O2, O8, O7-interaction ≈ 4) ; puis **O14 si acceptée** (≈ 1,4) — coupé en premier si le buffer déborde, puis la variante pre=1 de V3. **Jamais le buffer ni un item haut pour conserver du moyen.**
- Buffer nommé : friction build/re-glob CMake, environnement GPU (driver/GLFW, verrou `artist_opengl`), surprises d'oracle GMock (sémantique `InSequence`), ré-audit post-I.
- **Signal** : budget réalisable, pas d'irréalisabilité détectée ; le principal consommateur du buffer prévu est l'environnement GPU.

## 7. Critères de sortie

1. Plan persisté + commité par le lead **avant** tout worktree auteur X.
2. RED causal observé sur O1 (ordre) et O3 (curseur untouched) avant création du type — l'état « zéro couverture » est le RED attendu ; toolchain absente, contexte GPU indisponible ou zéro test découvert ⇒ **INCONCLUSIVE**, jamais RED ni PASS.
3. GREEN minimal : O1–O6, O8 exécutés PASS (→ COVERED) ; O7 re-confirmé par run ; O9 vert (0 FAIL, 0 skip requis/inexpliqué, découverte non nulle, JUnit recoupés).
4. Zéro message d'erreur nouveau, zéro type d'exception nouveau ; 5 fichiers gelés octet-identiques (empreintes relevées à l'intégration par le lead).
5. Registre à jour dans les deux sens ; aucun PASS sans preuve qualifiée ; chaque state tracé (executed / static-presence / inference).
6. R (revue statique O11–O13) planifié post-I, V fresh ensuite — hors périmètre de ce plan.

## 8. Décisions humaines ouvertes

1. **O14** (chaîne avec `passID=0`, test GL discriminant « pas de rejet ajouté ») — accepter ou GAP-defer. Recommandation : accepter (coût S, enforce littéral de la déc. 3).
2. **Placement des tests GL** : étendre `ProgramTests.cpp` (fixture unique `PipelineTest` réutilisée, X seul écrivain de ce fichier pour ce lot — recommandé) vs nouveau fichier component-test avec fixture dupliquée.
3. **GAP-accept I-A** (« bindings réussis restent en GL » sans oracle runtime) — accepter avec la motivation ci-dessus (injection impossible sans violer « pas de mocks communs »), ou demander une alternative.
4. **Stratégie niveaux/mocks** : la répartition CPU/GL est fixée par le ledger (ordre=CPU, liaison=GL) — aucune stratégie image/performance ni mock supplémentaire n'est héritée ; toute extension (mocks communs, ASan) est une décision humaine distincte, hors ce lot.

## 9. Rappel périmètre (reprise des contraintes transmises)

`MOCK_GL`/GMock ne prouvent pas le GPU ; readback GL réel requis pour V2/V7/V8 ; labels réels `unit`/`opengl` ; verrou `artist_opengl` exclusif ; mocks communs et couverture égale partout hors contrat. ADR-0002/0004/0006 restent Proposed (aucune adoption par ce lot) ; aucun résultat d'index canonique ne prouve le contenu non intégré du worktree ; totaux historiques jamais attendus de cette mission.

---

**Fin du livrable Q.** Aucun fichier écrit, aucun shell/test lancé, aucune session/grant créés ; le lead persiste ce texte sous `.opencode/evidence/mission-pipeline-execution-path/qa-plan-X.md`, le commit avant X, et tranche les 4 décisions ouvertes (n°3 conditionne l'état GAP-accept de O5-demi ; n°1 conditionne O14 ; n°2 conditionne l'implantation des tests GL de X).

---

## Addendum — Décisions humaines (chat, 2026-10-20)

1. **O14 : ACCEPTÉE** — 1 test GL discriminant (chaîne avec p1 non chargée, `passID=0` ⇒ readback 0, pas d'exception) ; coupée en premier si le buffer déborde.
2. **Placement des tests GL : `ProgramTests.cpp` étendu** — fixture unique `PipelineTest` réutilisée ; X seul écrivain de ce fichier pour ce lot.
3. **GAP-accept I-A : ACCEPTÉ** — « bindings réussis restent en GL » prouvé par revue statique + readback nominal ; la demi-obligation de O5 passe à GAP-accept avec la motivation du §6/§8.
4. **Stratégie (n°4)** : rien à trancher maintenant — répartition CPU/GL fixée par le ledger ; toute extension (mocks communs, ASan) reste une décision humaine distincte, hors lot.
5. **Commit pré-worktree : AUTORISÉ** — refresh docs + fiches mission (2 commits docs, puis évidence mission) ; `.opencode/missions/` reste hors commit (état policy).

Le plan est gelé à cet addendum : X démarre sous ces obligations ; toute surprise à l'intégration réévalue le registre (§3), pas le silence.
