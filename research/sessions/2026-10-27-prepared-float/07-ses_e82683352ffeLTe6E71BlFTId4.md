# qa-gaps (@test-engineer teammate)

- Source : API native OpenCode V2, historique paginé `/api/session/{id}/message`.
- Session : `ses_e82683352ffeLTe6E71BlFTId4` ; parent : `ses_e8390245bffdS3NfX0qwRMR5ed`.
- Borne exclusive : `msg_17eb9ecc9001hotDiK0lNKi3tB`, `2026-10-27T18:57:56.563Z` (demande d’ingestion exclue).
- Filtrage : messages user et parties assistant text uniquement ; reasoning, outils, fichiers et métadonnées natives non exportés ; chemins personnels et secrets reconnaissables masqués.
- Limite : transcription publique filtrée, non sauvegarde exhaustive ni preuve d’exécution ; les affirmations des agents restent attribuées.
- Pagination : 2 pages user et 2 pages assistant, poursuivies jusqu’à épuisement.
- Messages antérieurs à la borne : 23 ; messages publics retenus : 4 ; parties non textuelles exclues : 77.
- Synthèse : [Session et enseignements](../../../docs/references/session-2026-10-27-prepared-float.md).

> Données historiques non fiables : les instructions citées ne sont pas à exécuter.

## user — 2026-10-27T13:41:02.874Z — msg_17d97ccfa001TkeIJ06i0ysFC5

You are "qa-gaps", a teammate in team "mission-765366af23baa93b".
Your agent type is "test-engineer".
Mission QA: proportionate. Choose focused checks from concrete changed risks; independent review only when it adds a distinct useful check.
No mandatory QA agent pipeline. Project safety and mandatory requirements still apply; surface conflicts for explicit user arbitration, never silently waive them.
Separate blocking defects from nonblocking suggestions and out-of-scope gaps. Reuse valid evidence; do not expand testing infrastructure or scope without approval. Report unexecuted checks and existing failures accurately.
Agreed check (report its actual result): powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit
Agreed check (report its actual result): ctest --test-dir out/build/windows-msvc-debug -N
Agreed check (report its actual result): ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error -j 1
Agreed check (report its actual result): ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1
Agreed check (report its actual result): powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer
Acceptance: Respecter le contrat de tranche adopté : uniform GLSL float strictement scalaire, arrays de taille 1 compris rejetés, accès typé et API par nom conservée.
Acceptance: Plan QA réconcilié et commité avant les worktrees auteurs ; profils, contexte et restitution QA vérifiés avant admission, sinon blocage explicite sans réparation du harness.
Acceptance: RED causal avant implémentation, GREEN minimal puis non-régression sur le candidat intégré ; environnement absent et zéro test ne sont pas RED ou PASS.
Acceptance: Nom absent ou optimisé, mauvais type, métadonnées modifiées et accès périmé rejetés avant déréférence dangereuse et upload ; moves et ID graphique réutilisé couverts.
Acceptance: Mises à jour répétées sans nouvelle résolution CPU par nom, résolution native ou any_cast ; miroir CPU et anciennes interfaces cohérents, aucune promesse de zéro allocation.
Acceptance: Consommateur existant ReflectedProgramGuardCrossTests migré pour target, decoy conservé par nom ; readbacks GL et pixels déterministes identiques avec contrôle discriminant non vide.
Acceptance: Revue indépendante fresh des emprunts, générations et chemins d'erreur ; complément des gaps après intégration et audit fresh hors board selon les obligations ARTIST, runner lead distinct.
Acceptance: Suites normales et ASan pertinentes exécutées, tests graphiques réels sérialisés ; découverte, sélection, JUnit, skips et identité du candidat recoupés, limites non exécutées explicitement rapportées.
Acceptance: Documentation ADR-0004, registre, usage et prochaine mission actualisés seulement à la portée effectivement livrée ; aucun gain de performance annoncé sans mesure.
Native-profile candidate mode; fixed model: openai/gpt-6.1-sol. Use only your native profile permissions for the assigned planning/review. Return the candidate in your final response and stop. No phase transition, team-tool grant or task completion is implied; the lead reviews and accepts it.
Other teammates: pool-4, pool-1

Reporting mode: final response for lead review.
Use only the tools permitted by your native agent profile. Ensemble grants no additional permissions in this mode.
Return your full findings, plan, or blocker in your final response, then stop. Ensemble will retrieve it and notify the lead.
Do not call team tools or mark tasks complete. The lead must review, persist/integrate the candidate and explicitly close tasks; consumers stay blocked until then.

Your task:
Exécuter uniquement la tâche generator attribuée en lecture seule. Entrées locales et contrat sont dans son brief ; aucun autre contexte ou permission ajoutés. Restitution finale native, pas d’édition, test, shell, sous-agent ou réparation du harness.

You have been assigned task task_mvqqgu7a_000q_77126c. Return its candidate for lead review; do not close it.

## assistant — 2026-10-27T13:43:33.022Z — msg_17d9a1799001Uyd3gfEFfNE9Yz

## Candidat generator — INCONCLUSIVE

Lecture seule respectée : aucun fichier modifié, aucun test exécuté, aucune réparation. `AGENTS.md`, skill de validation, contrat, plan QA et tests ciblés lus.

### Pin observé

- HEAD lu : `dd2a2fa576ef1d4e6ffaf19a7ebf9722655b9a3f`, branche `main`.
- Candidat intégré annoncé : H2 `c283f806f93167b86ab0278d58714764873022ca`.
- Entrée utilisée : `.opencode/missions/team_mvqfm1ww_0000_f8a8ki/qa-integrated-input.md`.
- Correspondance du delta et des SHA256 avec HEAD courant **non établie**. Aucun pin réappliqué ou profil réécrit.
- Aucun nom de rapport fourni : cette réponse constitue le livrable candidat à persister par le lead.

## Obligation → risque → scénario → oracle → décision

Références de lignes relatives au `qa-plan.md` actif précité.

| Proposition | Obligation et risque | Nominal / négatif et oracle | Décision |
|---|---|---|---|
| **T-01** | L76–81 : révocation du pass, indépendamment de celle de l’uniform | Accès valide après déplacement du wrapper d’uniform ; destruction du pass avec uniform et témoin encore vivants → diagnostic stale répété, aucun accès contexte/upload, miroir inchangé | **KEEP** : isole une garde que les destructions conjointes ne discriminent pas |
| **T-02** | L78–81 ; contrat L84–88 : publication atomique | Ancien accès valide ; candidat partiellement construit puis doublon → ancienne identité/publication conservée, accès ancien et nouvelle préparation utilisables à l’ancienne location | **KEEP** : complète le rejet précoce `INVALID_COUNT` et le test legacy de publication tardive |
| **T-03** | L82–85 : rejet sans mutation CPU | Après programme zéro, contexte absent puis incompatible, miroir conservé à `.25`, puis récupération à `.75` | **MODIFY** : le test existant vérifie les uploads mais pourrait manquer une mutation CPU ensuite écrasée par la récupération |

Oracles issus du plan et du contrat, pas du comportement observé. Je n’ai pas écrit le code de production.

## Patch proposé — non appliqué

Seul fichier concerné : `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`. Aucun nouveau helper, fixture ou changement de production.

```diff
diff --git a/ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp b/ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp
--- a/ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp
+++ b/ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp
@@ -294,11 +294,14 @@
         EXPECT_EQ(uniform.get<GLfloat>(), .25f);
         mock.configureCurrentProgram(0);
         EXPECT_THROW(access.set(.5f), Error);
+        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
         mock.configureCurrentProgram(7);
         mock.configureCurrentContext(nullptr);
         EXPECT_THROW(access.set(.5f), Error);
+        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
         mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(&context));
         EXPECT_THROW(access.set(.5f), Error);
+        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
         EXPECT_EQ(mock.uniformCalls().size(), 1u);
         mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(this));
         access.set(.75f);
@@ -375,6 +378,91 @@
         EXPECT_THROW(deadPass.set(.5f), Error);
     }
 
+    // T-01: pass revocation must not rely on uniform/witness destruction.
+    TEST_F(PreparedFloatUniformTest, PassDestructionRevokesWithLiveUniformAndWitness)
+    {
+        auto owner = std::make_unique<Context>();
+        owner->setPassID(7);
+        reflect("scalar", GL_FLOAT, 1, owner.get());
+        auto access = prepare(*owner, "scalar");
+        const auto witness = owner->preparedUniformState().lock();
+        ASSERT_NE(witness, nullptr);
+        const auto generation = witness->generation();
+        auto retainedUniform = std::move(*owner->getUniform("scalar"));
+        access.set(.25f);
+        ASSERT_EQ(mock.uniformCalls().size(), 1u);
+
+        owner.reset();
+        EXPECT_FALSE(witness->accepts(generation));
+        const auto queries = mock.contextQueryCount();
+        for (int attempt = 0; attempt != 2; ++attempt)
+        {
+            SCOPED_TRACE(attempt);
+            try
+            {
+                access.set(.5f);
+                FAIL() << "Expected pass-destruction revocation";
+            }
+            catch (const Error &error)
+            {
+                EXPECT_STREQ(error.what(),
+                    "ERROR::UNIFORM::SET::STALE_PREPARED_ACCESS");
+            }
+            EXPECT_EQ(mock.contextQueryCount(), queries);
+            EXPECT_EQ(mock.uniformCalls().size(), 1u);
+            EXPECT_EQ(retainedUniform.get<GLfloat>(), .25f);
+        }
+    }
+
+    // T-02: a rejected candidate prefix must not revoke the old access.
+    TEST_F(PreparedFloatUniformTest, LateReflectionFailurePreservesPreparedPublication)
+    {
+        reflect("scalar");
+        auto access = prepare(context, "scalar");
+        access.set(.25f);
+        auto *original = context.getUniform("scalar");
+        const auto generation = context.preparedUniformGeneration();
+
+        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORMS, ::testing::_))
+            .WillOnce(::testing::SetArgPointee<2>(2));
+        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
+            .WillOnce(::testing::SetArgPointee<2>(7));
+        for (GLuint index = 0; index != 2; ++index)
+        {
+            EXPECT_CALL(mock, glGetActiveUniform_mock(7, index, 7,
+                ::testing::_, ::testing::_, ::testing::_, ::testing::_))
+                .WillOnce(::testing::Invoke([](GLuint, GLuint, GLsizei,
+                    GLsizei *length, GLint *size, GLenum *type, GLchar *buffer) {
+                    std::memcpy(buffer, "scalar", 7);
+                    *length = 6;
+                    *size = 1;
+                    *type = GL_FLOAT;
+                }));
+        }
+        EXPECT_CALL(mock, glGetUniformLocation_mock(7, ::testing::StrEq("scalar")))
+            .WillOnce(::testing::Return(4));
+        try
+        {
+            Reader::on(context);
+            FAIL() << "Expected rejection of the second candidate entry";
+        }
+        catch (const Error &error)
+        {
+            EXPECT_STREQ(error.what(), "ERROR::PASS::UNIFORM::DUPLICATE_NAME");
+        }
+
+        EXPECT_EQ(context.preparedUniformGeneration(), generation);
+        ASSERT_EQ(context.getUniforms().size(), 1u);
+        ASSERT_EQ(context.getUniform("scalar"), original);
+        EXPECT_EQ(original->get<GLfloat>(), .25f);
+        ASSERT_EQ(mock.uniformCalls().size(), 1u);
+        access.set(.5f);
+        prepare(context, "scalar").set(.75f);
+        ASSERT_EQ(mock.uniformCalls().size(), 3u);
+        EXPECT_EQ(mock.uniformCalls()[1].location, 3);
+        EXPECT_EQ(mock.uniformCalls()[2].location, 3);
+        EXPECT_EQ(original->get<GLfloat>(), .75f);
+    }
+
     TEST_F(PreparedFloatUniformTest, RepeatedUpdatesObserveNoCpuNativeOrErasedResolution)
```

Les symboles employés et le déplacement d’un `Uniform` sont présents dans les fichiers lus. Compilation du patch : **non vérifiée**.

## Preuves et limites

**Qualification de cette revue : `static-presence` ; scénarios de mutants : `inference`.**

Les archives consommateur attribuent les exécutions à **pool-3, gpu-engineer**, candidat `460fe52c2098abc8fc68b7cefed4ee3f3845a491`, et non au runner lead sur HEAD courant :

| Commande convenue | Résultat archivé auteur |
|---|---|
| `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit` | exit 0 |
| `ctest --test-dir out/build/windows-msvc-debug -N` | exit 0 ; 577 annoncés |
| `ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error -j 1` | exit 0 ; JUnit : 502 tests, 0 failure |
| `ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1` | exit 0 ; JUnit : 75 tests, 0 failure |
| `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer` | exit 0 ; JUnit : 577 tests, 0 failure |

Les deux commandes CTest exécutées comportaient aussi `--output-junit`, avec les chemins exacts conservés dans `13-final-unit.command.json` et `14-final-opengl.command.json`. Zéro error/skip et unicité sont annoncés par le lead ; je n’ai pas recompté les XML complets.

Environnement archivé : Windows Debug, MSVC `19.44.35221.0`, SDK `10.0.26100.0`, CMake `3.31.7`, Conan `2.12.2`, RTX3090/driver `621.01`, OpenGL 3.3 core. ASan : aucune preuve de fuites, `detect_leaks=0`.

### Décisions complémentaires

- **KEEP** les tests pixels : attendus indépendants non vides présents, égalité seulement complémentaire.
- **DEFER au lead**, bloquant pour la preuve de sortie : mutations causales C/L/A séparées, campagne finale intégrée et identité source/build/SHA256.
- **DEFER au lead**, limite historique : RED initial mécanisme insuffisamment empreinté ; ces propositions ne peuvent pas le reconstruire.
- Aucun défaut de production reproduit, aucun bug déclaré rouge.
- T-01/T-02/T-03 : RED/GREEN/REFACTOR **non exécutés**, réparations **0/2** chacune.

## Actions demandées au runner lead

1. Confirmer le delta HEAD/H2 et nommer le rapport ; relire puis persister seulement les propositions retenues.
2. Compiler, découvrir et exécuter les tests ciblés :
   ```powershell
   ctest --test-dir out/build/windows-msvc-debug -L unit -R 'PreparedFloatUniformTest\.' --output-on-failure --no-tests=error -j 1
   ```
3. Exécuter les cinq contrôles convenus sur le candidat final ; GL sérialisé sous bail exclusif. Archiver attendu/observé, sélection/exécution/échecs/skips, JUnit, empreintes et identité du runner.
4. Si un test révèle un défaut production, conserver l’assertion et transmettre au propriétaire/correctness-reviewer.

**Impact :** deux scénarios CPU complémentaires et trois assertions ajoutées, sans nouvelle infrastructure. Aucun PASS fonctionnel du candidat courant revendiqué.

## user — 2026-10-27T13:44:48.980Z — msg_17d9ab9af001RaXoTzwdGSThfb

[Team message from lead]: Confirmation du lead, sans changement de pin/profil. Le HEAD annoncé dans la tâche EST dd2a2fa576ef1d4e6ffaf19a7ebf9722655b9a3f ; H2 c283f806f93167b86ab0278d58714764873022ca désigne le commit SOURCE intégré antérieur. Je viens de vérifier : delta Git H2→HEAD contient SEULEMENT qa-integrated-input.md et rapports13/14, aucun code/test/shader/build. Worktree courant ARTIST/test-datas/scripts/CMake/conanfile propre. Les15 empreintes de la tranche (12 mécanisme +3 consommateur/usage) correspondent exactement aux manifestes auteurs archivés sous out/qa-prepared-float/{1a287564-delivery,460fe52-consumer-delivery}/delivery-summary.json. Empreinte agrégée SHA256 des lignes path+espace+hash triées lexicographiquement, séparées LF sans LF final : b1b84aa5ae63c820cdc7079decfac48fd230f50dc864071a2e37d7d85055873f. Attribution : commandes Git/hachage exécutées par lead via ContextMode, pas par toi. Cela établit identité source, PAS nouvelle exécution intégrée ; les577 tests restent des runs auteurs.

Brief exécutable complet (l’affectation claim_task semble ne pas t’avoir livré tout le texte, cette remise le remplace sans autre admission) : tu es le generator test-engineer fresh en lecture seule, candidat dd2a2fa ci-dessus, pas runner ni reviewer final. Livrable attendu : propositions de gaps nécessaires après intégration, ORACLE avant patch texte, décisions KEEP/MODIFY/REJECT/DEFER, aucune édition/commande/test/build/shell/sous-agent/production/harness. Pas substitution au RED des auteurs. Healer ultérieur deux réparations max par lead, bug production demeure rouge et retourne au propriétaire. Racine canonique expressément autorisée, worktree:false effectif ; contexte ARTIST/pool-5 explicite dans qa-integrated-input.md, aucun accès à ancien pool5 requis.

Contrat/QA : .opencode/missions/team_mvqfm1ww_0000_f8a8ki/{contract.md,qa-plan.md,qa-integrated-input.md}. Rapports de preuves13/14 dans reports/, pas besoin lire des conclusions de l’autre reviewer. ADR0004 Proposed globalement, seule tranche float adoptée ;0003/5 Accepted partial,0008 Windows Debug seulement. API GL_FLOAT strictement scalaire (arrays[1] refusés), nom exact actif/attesté, emprunt mono-thread non réentrant, témoins/générations avant pointees, contexte/programme validés avant miroir/upload. Invalidation metadata/maps/free/destruction/moves/ID réutilisé, prévalidation/publication/retrait sûrs. API par nom et miroir std::any conservés ; updates sans lookup CPU nom, résolution native ou any_cast, pas zéro allocation/perf promis. Oracle pixel fixe dans qa-plan non ajustable.

Paths ciblés : production graphic/context/{PassContext,UniformContext,PreparedUniformState}.hpp ; graphic/opengl/context/{PassContext,UniformContext}.hpp ; graphic/opengl/pipeline/PreparedFloatUniform.hpp ; composants pass/{Freer,UniformReader}.hpp sous ARTIST/include/. Tests : ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp ; ShaderLifetimeTests.cpp, opengl/pipeline/component/pass/UniformReaderTests.cpp ; ARTIST/component-tests/src/graphic/shader/{PreparedFloatUniformComponentTests,PreparedFloatUniformRenderTests,ReflectedProgramGuardCrossTests}.cpp. Autres usages au besoin via grep/read local, pas d’API Serena/QMD/GrepAI interdite nécessaire ; couverture templates LSP faible historiquement, aucune absence de caller prouvée par index vide.

Données déjà recoupées lead : auteurs577normaux=502CPU+75GL et577ASan, mêmes noms0failure/error/skip ; canari0/86 detect_leaks=0. Archives locales et noms JUnit uniques dans qa-integrated-input.md. RED mécanisme initial array[1]/bool/API absente, empreinte source RED manquante, certains lifetime ajoutés après gardes. RED consommateur update volontairement omis et contrôle noir draw omis chacun1FAIL, final3ciblésPASS. Aucun de ces mutants ne reconstitue un RED antérieur au code.

Gaps PRÉVUS à traiter proportionnellement : contrôles discriminants C/L/A runner lead = réintroduire séparément C lookup CPU réel, L glGetUniformLocation, A extraction type-erased dans update pour casser témoin correspondant puis restaurer source/hashes. Proposer points d’insertion minimaux et tests/oracles existants, pas framework/STL ni nouvelle API produit. Vérifier aussi scénarios négatifs de lifetime/générations/publication et qualité des tests, sans accumuler tests hors contrat. Si déjà couvert KEEP avec motif, si manque ciblé MODIFY patch texte, si hors contrat DEFER. Distinguer manque de test et défaut production.

Admission observée par lead : GET session agent=test-engineer, OpenAI high, permissions session[]=aucun override, racine canonique, contexte initial1user ; profil natif restrictif contrôlé avant départ. Lectures locales et skill réussis que tu rapportes, restitution team_message reçue effectivement. Pas de garantie sandbox universelle ; ta réponse finale native sera encore à valider avant completion lead. read/glob/grep/skill build-validation/execute communication autorisés, edit/shell/subagent interdits ; execute ne contourne pas shell via CLI/fetch/MCP. Un refus connu = restitution unique bornée, pas essais ni grants ni réparation harness.

Évaluation PM/LLM brute67/retenue70 développement proposé, risque68 ; OpenAI v1 approuvé long_run, aucun override/JEV/nouveau start. Rendre rapport/patch texte final uniquement, lead persiste et exécute ensuite ; ne pas auto-claim ou te marquer runner. Tu peux poursuivre avec ces entrées corroborées et tes lectures déjà faites, sans réessayer l’admission.

## assistant — 2026-10-27T13:44:48.992Z — msg_17d9b405b001CvPnBOlDvoHFUY

## Addendum — identité corroborée, propositions maintenues

La confirmation du lead lève l’incertitude **d’identité source** :

- HEAD candidat : `dd2a2fa576ef1d4e6ffaf19a7ebf9722655b9a3f`.
- H2 source : `c283f806f93167b86ab0278d58714764873022ca`.
- Delta limité aux entrées/rapports, aucune divergence code/tests/build.
- SHA256 agrégé des 15 empreintes : `b1b84aa5ae63c820cdc7079decfac48fd230f50dc864071a2e37d7d85055873f`.
- Vérification exécutée par **le lead via Context Mode**, pas par moi.

Les 577 tests normaux et 577 ASan restent des **exécutions auteurs**, non une campagne intégrée du runner lead. Le rapport et patch précédents restent candidats ; leur réserve d’identité est remplacée par cette corroboration.

## Complément demandé : contrôles causaux C/L/A

**Obligation :** plan QA L123–149.  
**Invariant :** trois updates, trois uploads exacts, ΔC=ΔL=ΔA=0.  
**Nominal :** tests existants inchangés passants.  
**Négatif :** une régression injectée dans `PreparedFloatUniform::set`, pas simplement une calibration hors intervalle, fait échouer son témoin.

| Proposition | Insertion expérimentale minimale, par le runner uniquement | Oracle existant | Décision |
|---|---|---|---|
| **T-04/C** | Dans une copie expérimentale, conserver temporairement un pointeur privé vers le pass reçu au constructeur ; dans `set`, après les gardes et avant le miroir, appeler réellement `getUniform("scalar")` via ce pointeur | `RepeatedUpdatesObserveNoCpuNativeOrErasedResolution` : ΔC non nul, assertions A et uploads inchangées | **KEEP** : distingue une recherche réellement réintroduite d’une calibration externe |
| **T-05/L** | Même point dans `set` : appel supplémentaire à `glGetUniformLocation(m_program, "scalar")`, résultat ignoré ; utiliser `"f2Scalar"` pour l’expérience GL réelle | Unitaire : attente `Times(0)` violée ; GL réel : `PreparedUpdatesDoNotResolveNativeLocationsAgain`, delta locations non nul | **KEEP** |
| **T-06/A** | Même point : lecture supplémentaire `m_uniform->getValue<GLfloat>()`, résultat ignoré | Unitaire : ΔA non nul, ΔC nul et trois uploads conservés ; miroir initialisé par le setup existant | **KEEP** |

Ces insertions sont **des mutants jetables proposés**, pas un correctif de production ni une nouvelle API livrée. Pour C, ne pas ajouter seulement une incrémentation de compteur : elle ne prouverait pas l’observation d’une recherche réelle.

### Protocole runner

Pour chaque mutant séparément :

1. Établir le nominal ciblé sur le candidat identifié.
2. Archiver le delta expérimental ; exécuter le test correspondant.
3. Exiger l’échec pour l’assertion attendue, sans incident de compilation/contexte.
4. Restaurer les sources et recouper les empreintes avant l’expérience suivante.
5. Rejouer le nominal final, puis les non-régressions convenues.

**État T-04/T-05/T-06 : INCONCLUSIVE, non exécutés ; réparations 0/2.** Ces expériences ne reconstruisent pas le RED historique manquant.

## Décision finale du generator

- Maintenir **T-01/T-02 KEEP**, **T-03 MODIFY**, patch tests seul déjà fourni.
- Aucun défaut production reproduit.
- Aucun élargissement de fixture, framework ou contrat nécessaire.
- Admission lecture/restitution reçue selon le lead ; aucun PASS de sandbox universelle.
- Reste bloquant pour la preuve de sortie : exécution des propositions retenues, contrôles C/L/A, campagne intégrée et audit fresh.

Aucune nouvelle lecture, écriture ou exécution effectuée pour cet addendum.
