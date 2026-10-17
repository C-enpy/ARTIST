# P3 — Préflight UniformReader/GL (mission F7 option A)

Transmis par pool-4 (gpu-engineer), relecture lead 2026-10-17. Lecture seule pure
(aucun build/test/GPU exécuté) — tous les « actuel » = statique, **INCONCLUSIVE** jusqu'au
RED exécuté par l'auteur T3. HEAD lu eb0dd14, arbre propre ; bases de fork réelles à la
publication (docs-only 4f64a2b / 25ced63). Les arbitrages D6–D8 (annexe) sont tranchés
par le lead au commit — ils figurent dans la fiche T3.

## 1. État reconnu du Reader (lecture)

- `OpenGLPassUniformReader` (opengl/pipeline/component/pass/UniformReader.hpp:29-62) :
  spécialisation unique Pass::Classic, static void on(OpenGL::PassContext&) — forme
  EMPRUNTÉE void déjà conforme HasBorrowedOnMethod (validator/ComponentConcept.hpp:12-16).
  Garde passID==0 → ERROR::PASS::INVALID_PROGRAM_ID : unique garde métier, NON testée.
- **Non durci (statique ; RED à établir par T3)** :
  (a) glGetProgramiv(GL_ACTIVE_UNIFORMS) sans badge GL error ;
  (b) glGetActiveUniform sur buffer local uniformName[256] sans
  GL_ACTIVE_UNIFORM_MAX_LENGTH ni nameLength/GL error → nom >=256 tronqué ;
  (c) location -1 non vérifiée → setUniformID(GLuint(-1)) wrap silencieux ;
  (d) addUniform écrase silencieusement (context/PassContext.hpp:50-53) — deux noms
  longs tronqués identiques collident ;
  (e) boucle GLint sûre si numUniforms<0 (zéro itération) — pas un RED.
- Flux : Pass.hpp:97-110 IPass::load() → shaders → Loader → readUniforms
  (Pass.hpp:293-299, if constexpr HasComponent) → readAttributes. Reader bien appelé.
- **Gap F7 CONFIRMÉ** : OpenGLPassFlow (opengl/validator/PassConcept.hpp:41-53) exige
  Loader/Freer/ShaderAttacher/AttributeReader/User (HasComponent + forme empruntée) mais
  **PAS UniformReader** (ni présence ni forme). validatePass = requires-clause sur
  OpenGLPassFlow (Validator.hpp:43-48) → durcissement propage à tout
  Pass<API,PROFILE> (Pass.hpp:261).
- **Couplage T1** : Reader construit make_shared<UniformContext> +
  make_shared<Uniform<OpenGL>> (Uniform.hpp:47-103, getContext() shared) ; T1 (owner
  unique, getContext() emprunt) changera cette forme → construction du Reader réglée sur
  diff T1/T2 intégré au fork T3 ; P3 ne tranche pas.
- Unit mocké : tests/src/graphic/opengl/pipeline/component/pass/UniformReaderTests.cpp,
  garde __mock_gl__ (glFunctionMock = tests/mock/opengl/glFunctionMock.hpp) : UN cas
  ReadUniforms_ValidContext. MockPassContext:29 mappe UniformReader→MockUniformReader
  (on(C&) void, conforme).
- Fixtures GL : test-datas/shaders/{vertex,fragment}/good/minimal.* = uniform int
  testUniform unique + aPos vec3 ⇒ base nominal réelle.
- Addendum A2 (P1) absorbé : le `size` de glGetActiveUniform jamais transmis
  (UniformReader.hpp:49) se replie dans les durcissements (b)/(c) ci-dessus.

## 2. Patterns component-tests (ArtistLib_component_tests)

- Fixture OpenGLComponentTest : GLFW 3.3 core + GLFW_VISIBLE off, 640x480,
  makeContextCurrent, glewExperimental/glewInit ; log VENDOR|RENDERER|VERSION à chaque
  SetUp ; échec explicite si init impossible (pas de skip) ; TearDown
  destroyWindow+terminate.
- Bail GPU GlLeaseWitness (ProgramTests.cpp:51-76 =
  PipelineLifetimeComponentTests.cpp:56-74) : témoin CPU déclaré AVANT les owners,
  détruit DERNIER, EXPECT_TRUE(contextAlive) → échec si owner GPU survit TearDown ;
  zéro appel GL (complément, jamais substitut). Probe RED « owner détruit après
  tearDown » a échoué causalement sur ce witness — pattern validé.
- Owners locaux post SetUp/pré TearDown ; passes chargées avant transfert ; handles GL
  copiés PAR VALEUR (jamais owners) ; glUseProgram(0) explicite AVANT destruction d'un
  bound (suppression native différée sinon) ; observation glIsProgram/glIsShader,
  GL_CURRENT_PROGRAM, GL_DELETE_STATUS ; pas de glFinish. Chaîne complète des
  composants incluse par TU (« full component chain so the profile constraints
  resolve »).
- CMake component-tests : GLOB_RECURSE + gtest_discover_tests prefix "opengl." +
  RESOURCE_LOCK "artist_opengl" + WORKING_DIRECTORY=racine repo +
  artist_copy_asan_runtime. Fixtures globales UNIQUEes entre TU (incident ODR
  LoaderTests/UserTests) ; re-glob CMake OBLIGATOIRE après tout ajout de fichier
  (incident C2 : 153/153 pré-glob invalide). Noms globaux pris : OpenGLComponentTest,
  PipelineTest, PipelineLifetimeTest, PassTest, GlLeaseWitness.

## 3. Précédent concept négatif

- tests/src/graphic/pipeline/PipelineBorrowedContractTests.cpp:15-57 : dummy API
  PipelineContractAPI<U,R> ; comps dummy Borrowed on(C&) / SharedOnly on(shared_ptr<C>)
  / NonVoid int on(C&) ; static_assert POSITIF OpenGLPipelineFlow<BorrowedAPI> +
  NÉGATIFS !…<SharedUserAPI>/<SharedResetterAPI>/<NonVoidAPI> (44-47) + TEST runtime du
  prédicat (50-57) + comps réels (59-65). Forme à répliquer.
- Traits : ShaderLifetimeTests.cpp:113-152 (!copyable/movable/nothrow/final),
  PipelineContextTests.cpp:38-39, UniformContractTests.cpp:27-30 (!HasUniform<int>…) ;
  MSVC 19.44/19.50 : concept NOMMÉ conservant le prédicat, pas requires inline
  (build-and-test.md:409-411).
- Borne (mission failure-contracts γ) : concept contrôle l'APPELABILITÉ, pas la
  signature exacte — T3 s'en tient là (absence + forme).
- RED exige toolchain fonctionnelle + motif de rejet VÉRIFIÉ (C2338 + message
  personnalisé). Sonde compilation négative HORS build normale possible (précédent
  Pipeline : probe getter-owner 0/2/0) — forme au choix de l'auteur T3.

## 4. Plan RED T3 (squelettes NOTE ; écriture = auteur T3, fork P3 + diff T1/T2 relu.

Périmètre : PassConcept.hpp + UniformReader.hpp + tests. Hors périmètre : Shader/Loader
(sh+pass)/contexts/F8/Pass.hpp dispatch inchangé/glFunctionMock sauf arbitrage D8.)

- **A. Concept négatif** — unit CPU pur, NOUVEAU
  tests/src/graphic/pipeline/UniformReaderFlowContractTests.cpp (fixture unique),
  pattern PipelineBorrowedContractTests : comps dummy AbsentUniformReader (struct vide)
  / SharedOnlyUniformReader on(shared_ptr<C>) / NonVoidUniformReader int on(C&) ; dummy
  API = 5 comps existants Borrowed + UniformReader=R ; static_assert positif + 3
  négatifs (« F7 absence », « F7 forme empruntée », « F7 void »). RED : C2338 sur les 3
  (concept actuel laxies). GREEN : PassConcept.hpp ajouter requires HasComponent +
  requires HasBorrowedOnMethod pour UniformReader (~l.46/52) ; GREEN compile asserts
  unit+opengl complètes (MockOpenGL::PassContext conforme). Runtime gtest du prédicat
  en réplique EXPECT_TRUE/FALSE.
- **B. Durcissement mocké** — ÉTENDRE UniformReaderTests.cpp (unit, __mock_gl__), RED
  paramétré à la F5 : 1) nom long : glGetActiveUniform_mock nameLength>255 +
  numUniforms=1 + glGetUniformLocation_mock -1 sur le nom tronqué ; RED actuel : entrée
  ajoutée tronquée + ID wrap -1 ; 2) collision : deux noms longs distincts tronqués
  identiques → getUniforms().size()==1 (RED) ; 3) passID==0 : EXPECT_THROW
  INVALID_PROGRAM_ID (nouvelle couverture) ; 4) numUniforms=0 : zéro itération, aucun
  addUniform (caractérisation).
- **C. Driver réel GL** — NOUVEAU
  component-tests/src/graphic/shader/UniformReaderComponentTests.cpp, fixture unique
  UniformReaderComponentTest : OpenGLComponentTest. Squelette : makeLoadedPass()
  (chaîne complète incluse, pattern PassTests) ; pass.load() ; nominal conservé (suite
  de PassTest::UniformReadingTest) : getUniforms().contains("testUniform"), location !=
  GLuint(-1), getGLType()==GL_INT ; RED nom long : nouvelle fixture
  test-datas/shaders/vertex/good/longUniform.vert (uniform nommé >255 car. actif) +
  frag ; attendu écrit AVANT run = uniform non-tronqué présent, location valide → FAIL
  attendu vs actuel tronqué. GlLeaseWitness déclarée avant le pass ; observation
  GL_CURRENT_PROGRAM puis glUseProgram(0) avant destruction owner — pattern exact des
  TU existants.
- **ASan** : sélection déclarée AVANT run dans le manifeste T3 (nouveaux cas
  non-lifetime hors sélection comme les autres GL non-lifetime mission Pipeline :
  176+22 GL lifetime, 17 autres explicitement NON sélectionnés ; pas de requalification
  implicite) ; options PAR PROCESSUS
  halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0 ; canari
  artist_asan_probe reconstruit après clean build, 0 puis 86 ; premier FAIL conservé ;
  --tests-from-file chemins ABSOLUS (incident Pipeline : relatif résolu sur arbre build
  = erreur d'invocation, pas un RED) ; ASan ne remplace jamais une preuve GL native.

## 5. Couplage T1/T2 + arbitrages lead

T3 après T2, fork P3 ; adaptation Reader sur (a) diff T1/T2 relu, (b) patterns §2-4,
(c) bornes ADR-0003/0004/next-mission. Pas de politique required/facultative uniforms
optimisés (déc. ADR-0004 B plus tard) : T3 règle le RED driver observable, pas une
politique future. Pas d'ouverture IAttribute::set (déc. 4) ; pas de re-design T1/T2.

**Arbitrages tranchés par le lead (D6–D8, à emporter dans la fiche T3)** :

- **D6 — TRANCHÉ** : location==-1 et nom tronqué → **rejet strict + exception typée au
  motif** (pas de tolérance élargie, cohérent U1/plan QA) ; le comportement « document
  conserv » n'est pas l'option retenue. addUniform silencieux écrase → garde au Reader
  (rejet nom dupliqué) ou à PassContext selon le RED causal de l'auteur (colision),
  consigné dans la note T3.
- **D7 — TRANCHÉ** : fixture longUniform créée par l'auteur T3 sous test-datas/
  (minimal.* intouchés, noms uniques, re-glob après ajout).
- **D8 — TRANCHÉ** : extension de glFunctionMock PERMISE pour T3, bornée : uniquement
  si le durcissement introduit des badges GL erreur nécessitant une observation mockée
  (glGetError) ; toute extension est justifiée dans la note T3 et visible au diff ;
  « glFunctionMock intact » était la contrainte de la mission Pipeline archivée, pas de
  F7. Pas de re-design de glFunctionMock.

## 6. INCONCLUSIVE explicités

- Aucune exécution ; chaque « actuel » = statique. RED driver réel (nom long, NVIDIA
  617.42/GL 3.3.0) probable mais non reproduit — à établir par T3, attendu écrit avant
  run ; la valeur GL_ACTIVE_UNIFORM_MAX_LENGTH (attente du driver) se confirme à
  l'occasion, jamais déduite du nom GPU.
- Chaîne complète des consumers Reader/location wrap non exhaustive (templates/mocks) ;
  l'auteur T3 re-grep exact (précédent F5 : pass/Loader.hpp:97-98, setPassID publié
  après link vérifié — analogie sans état repris).

## 7. Campagne T3 (commandes)

- cmake preset windows-msvc-debug + re-glob après tout ajout ; VsDevCmd dans le même
  processus que cmake/ctest (sinon C1083/LNK1104).
- ctest --test-dir out/build/windows-msvc-debug -L unit -R UniformReader ; -L opengl
  idem ; comptage final consigné (baseline 176+39 normal, ASan 176+22) avec nombres,
  noms, skips.
- ASan : preset windows-msvc-debug-asan ; scripts/build-windows.ps1 -AddressSanitizer
  -AsanPreflightOnly -Jobs 1 ; cmake --build --preset windows-msvc-debug-asan
  --parallel 1 ; canari 0/86 puis sélection ABSOLUE via fichier.

Récap : gap F7 vivant (concept sans UniformReader), forme Reader déjà empruntée,
durcissements 1-4, précédent PipelineBorrowedContract réutilisable, patterns GL validés
(LeaseWitness/glUseProgram(0)/handles copie/fixture unique/re-glob/lock/ASan
clean-first), couplage T1/T2 borné, arbitrages D6-D8 fermés au commit.

— pool-4 / gpu-engineer / préflight P3 (persisté par le lead)
