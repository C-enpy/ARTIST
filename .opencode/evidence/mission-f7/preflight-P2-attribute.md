# P2 — Préflight Attribute (mission F7 option A)

Transmis par pool-3 (cpp-engineer), relecture lead 2026-10-17. Persistance : pool-1
lecture seule, zéro exécution — tous les constats restent **INCONCLUSIVE** jusqu'au RED
exécuté par l'auteur T2. HEAD lu : `b4335f0` (docs-only sur `eb0dd14`, code identique) ;
au commit lead `4f64a2b` (docs-only) le code reste identique à `eb0dd14`.

## A. État

- HEAD réel b4335f0 « docs: record F7 option A » (parent eb0dd14, docs/log.md seuls
  changés) : code identique à eb0dd14, fiche à recaler. Untracked :
  `.opencode/missions/…/` (artefact Ensemble, hors source). Aucune source modifiée.

## B. Observations vérifiées (statique, non exécutée — INCONCLUSIVE sans RED)

### 1) Hardcode obs 2 — CONFIRMÉ, plus grave que l'énoncé

- `Attribute.hpp:88,90` : clé constante `Setter<std::shared_ptr<int>>` indépendante du T
  de `set<T>`. `HasComponent (ComponentConcept.hpp:28-31)` n'exige que typename C → pour
  OpenGL (alias template `Setter=OpenGLSetter<A>`,
  `opengl/context/AttributeContext.hpp:27-28`) et le mock
  (`MockAttributeContext.hpp:20-21`) la branche TRUE est toujours prise : la branche
  `UNSUPPORTED_TYPE` (Attribute.hpp:92-95) est **morte** pour ces deux contextes. La
  porte `OpenGLAttributeFlow` (opengl/validator/AttributeConcept.hpp:29,33) valide
  `Setter<int>` — clé différente de celle dispatchée, deux endroits incohérents.
- **FLUX** : `setValue<T>` stocke `any(shared_ptr<T>)` (Attribute.hpp:86 →
  context/AttributeContext.hpp:36-50) ; le dispatch appelle `on<shared_ptr<int>>` →
  `getValue<shared_ptr<int>>()` = `any_cast<shared_ptr<shared_ptr<int>>>(m_value)` —
  l'any contient `shared_ptr<int>` → `bad_any_cast` → TraceableException TYPE_MISMATCH
  (AttributeContext.hpp:27-30). **Sur OpenGL réel, set<int>(make_shared<int>(1)) au
  niveau pipeline JETTERAIT TYPE_MISMATCH**, valeur persistée après le throw
  (Attribute.hpp:86 stocke AVANT le dispatch : écriture partielle survivant à
  l'exception). Invisible aux tests actuels : l'unitaire moque (MockSetter ne lit pas
  l'any) et les composants appellent `OpenGLSetter<float>::on` directement (A=T).
- Cas dégénéré : `set<std::shared_ptr<int>>` (T=shared_ptr<int>) matche et uploaderait
  8 octets de la capsule shared_ptr elle-même (données corrompues, pas d'UB C++).

### 2) sizeof obs 3 — CONFIRMÉ, mesure sizeof(A*)=8 (x64) pour tout T

- `Setter.hpp:31` glBufferData et `:32` stride sur `sizeof(attrValue.get())` avec
  attrValue = shared_ptr<A> → sizeof(A*). Chemin composant réel A=T=float
  (SetterTests.cpp:29-44) : buffer de 8 pour objet 4 octets = lecture OOB dès un GL non
  moqué ; glFunctionMock enregistre les args, défaut silencieux.
- **LE TEST COMPOSANT RÉPLIQUE LE BUG** : attentes SetterTests.cpp:39 et 59 utilisent
  aussi sizeof(attrValue.get())=8 — faux vert par miroir ; fix sizeof ⇒ ces deux
  lignes-oracle à adapter (motif b9c90fd).
- Hors périmètre : buffer multi-éléments (N) non représentable (m_size = composants
  1..4, pas count) ; T2 ne traite que sizeof(T)/élément.

### 3) Précédent interne

- `Uniform.hpp:70-73` dispatche déjà `Setter<T>` (même T que le stockage) et place
  setValue dans le if constexpr (Attribute : stocke avant le check).

## C. Consommateurs

- Include : **AUCUN appel de IAttribute::set hors tests**. Pass.hpp:159,
  context/PassContext.hpp:61,107,147,155 = storage ownership (carte
  shared_ptr<IAttribute>). AttributeReader.hpp:58-65 crée
  AttributeContext+Attribute<OpenGL,Classic> et n'appelle JAMAIS set.
- Unitaire : AttributeTests.cpp TearDown:33-36 reset les 4 singletons (`MockSetter<int>`
  ET `MockSetter<shared_ptr<int>>` déjà resetés) ; SetTest:68-93 attend
  `MockSetter<shared_ptr<int>>::mockOn` Times(1) + stored==value via getValue<int>.
  Bind/Unbind/GetContext non touchés (Binder/Unbinder paramétrés par PROFILE).
- Composants (__mock_gl__ + glFunctionMock) : SetterTests.cpp:29-64 à adapter (sizeof
  8→4) ; 66-83 (null/buffer) insensibles. AttributeReaderTests.cpp:75-76 VÉRIFIÉS :
  population de carte seulement — non affectés par T2.
- **Blast radius** : changement de clé observable uniquement par les oracles de test ;
  aucun caller moteur réel de set() dans l'arbre.

## D. Invariants T2 (à relire au diff)

- Ownership : IAttribute possède shared_ptr<AttributeContext> (Attribute.hpp:125) ; la
  carte Pass possède shared_ptr<IAttribute> — T2 ne change ni l'un ni l'autre ;
  getContext() (115-118) reste copie propriétaire (hors périmètre).
- Emprunt : on(shared_ptr<AttributeContext>) = contrat SHARED conservé pour
  Uniform/Attribute (ComponentConcept.hpp:18-26) — T2 NE migre PAS vers on(C&) (décision
  T1 ; pas d'échange d'effacement sous la même boucle sans coût justifié).
- Lifetime GPU : attrValue.get() n'est retenu par aucun appel GL (copie synchrone
  driver) ; le shared_ptr reste vivant dans m_value et localement pendant le dispatch —
  aucun hazard nouveau à forme conservée. Pas de barrière nouvelle (single-thread).
- Exception safety : TYPE_MISMATCH / NON_OPENGL_CONTEXT / BUFFER_ID_NOT_SET inchangés.
  « Stockage avant dispatch ⇒ valeur persistée si throw » : ne PAS corriger en T2 sauf
  décision explicite (alignement Uniform = 1 ligne + RED/GREEN dédiée) ; sinon conserver
  et documenter.

## E. Plan RED T2 (squelettes — l'auteur T2 possède le RED causal et le code)

Séquence obligatoire (précédent d1a9ee1/b9c90fd,
_meta/session-2026-10-17-attribute-oracle-{plan,validation}.json) : RED oracle-only
d'abord, échec constaté pour la raison causale, PUIS source. Interdits : sonde
historique hors protocole (sélection déclarée, négatif préservé, pas de WILL_FAIL),
tolérance élargie.

1. **RED-B (clé, obs 2)** — AttributeTests SetTest :
   EXPECT_CALL(*mock::MockSetter<int>::instance(), mockOn(attributeContext)).Times(1) ;
   ASSERT_NO_THROW(attribute.set(value)) — échec attendu aujourd'hui : dispatch va à
   MockSetter<shared_ptr<int>> (unexpected call + expectation non satisfaite, visible
   avant summary grâce au TearDown d1a9ee1) ⇒ causal hardcode.
2. **RED-A (sizeof, obs 3)** — SetterTests :
   glBufferData_mock(GL_ARRAY_BUFFER, sizeof(float) /*4, pas sizeof(float*)=8*/,
   attrValue.get(), GL_STATIC_DRAW) Times(1) et
   glVertexAttribPointer_mock(1,1,GL_FLOAT,GL_FALSE,sizeof(float),(void*)nullptr)
   Times(1) — échec attendu aujourd'hui (8 mesuré) ⇒ causal sizeof pointeur.
3. **GREEN minimal** : Attribute.hpp:88,90 Setter<std::shared_ptr<int>> → Setter<T>
   (aligné stockage + Uniform.hpp:70-73 ; la porte AttributeConcept (Setter<int>) devient
   cohérente sans changement) ; Setter.hpp:31-32 sizeof(attrValue.get()) → sizeof(A)
   (élément pointé) pour glBufferData ET stride. **PRÉREQUIS : l'alignement
   clé/stockage DOIT précéder le fix sizeof** — sans clé générique, le chemin nominal
   n'atteint jamais glBufferData (TYPE_MISMATCH) : un fix sizeof seul serait
   inatteignable.
4. **Adaptation oracle finale** : SetTest attente effective sur MockSetter<int>,
   commentaire b9c90fd réécrit pour la nouvelle réalité, assertion stored conservée ;
   known-incidents.md + _meta à jour (nouvel amendement, historique 440-471 immuable).
5. **Preuve GPU recommandée** (décision auteur/lead) : un composant test réel
   (GLFW/GLEW 3.3, ArtistLib_component_tests) sur set<int> pipeline prouverait
   TYPE_MISMATCH actuel au RED et sa disparition au GREEN ; sinon la chaîne any_cast
   reste statique.
6. Static_asserts optionnels : identité de clé (mock+contexte réel), négatif sur un stub
   sans Setter<T> (rejet au motif vérifié, précédent HasBorrowedOnMethod),
   sizeof(A)!=sizeof(A*) pour A court.

## F. Décisions à trier

- **(i) Clé Setter<T> vs Setter<shared_ptr<T>> — TRANCHÉE par le lead au commit (D2)** :
  **Setter<T>** (recommandation motivée P2 : stockage any(shared_ptr<T>), getValue
  consomme shared_ptr<T>, Uniform.hpp:70-73 dispatch déjà Setter<T>, AttributeConcept
  valide Setter<int> → clé T rend la porte concept cohérente sans autre changement ;
  l'alternative casserait contexte/Uniform/concept). Oracle A1 du plan QA se réécrit en
  conséquence (set<float> → MockSetter<float> Times(1) identity) — contrainte D2 ferme :
  jamais de réécriture silencieuse, RED conservé avant.
- (ii) Position de setValue (avant/if constexpr) — décision contract, 1 ligne (T2).
- (iii) UNSUPPORTED_TYPE vivant par T : exigerait spécialisations explicites par type
  (motif uniform OpenGLUniformSetter<GLfloat>…) — élargissement hors T2 ; utile à
  ADR-0004.
- (iv) Risque adjacent (sans chantier) : opengl/context/AttributeContext.hpp:87-91
  membres NON INITIALISÉS (m_bufferId/m_usage indéterminés ⇒ BUFFER_ID_NOT_SET non
  garanti si unset) — hors T2.

## G. Limites

- Statique seulement : la chaîne any_cast est démontrée par le code, mais l'échec
  runtime réel n'est pas reproduit — INCONCLUSIVE jusqu'au RED de l'auteur T2. LSP
  incomplet sur templates (greps exacts) ; à revérifier à l'implémentation.
- Cibles T2 : ARTIST/include/graphic/pipeline/Attribute.hpp (IAttribute::set 83-96) ;
  ARTIST/include/graphic/opengl/pipeline/component/attribute/Setter.hpp
  (OpenGLSetter::on 20-33) ; oracles AttributeTests.cpp + SetterTests.cpp ; docs
  next-mission.md, known-incidents.md, _meta attribute-oracle-{plan,validation}.json.
  Scores PM/LLM 45/25 = heuristique, non probabilités.
