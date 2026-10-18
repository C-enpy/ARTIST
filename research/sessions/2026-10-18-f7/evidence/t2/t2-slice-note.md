# T2 — note de tranche (pool-6, mission F7 option A)

Base : `cb9f2688dcb268a430c37cb66de3bd3aa31dd843` (merge T1). G1 confirmé
2026-10-17T15:00:37+02:00 (`g1-identity-start.txt` : 0 commit, 0 delta, 0 untracked ; octets
bruts = blobs HEAD ; `AttributeTests.cpp` = empreinte validée `b9c90fd`).

## P0 (base, avant toute modification)

Arbre `out/b`, toolchain Conan de `out/build/windows-msvc-debug`, MSVC 14.44, un job.
conan install / configure / build exit 0 (build 50 étapes, 149,3 s). Découverte 231 = 192 unit +
39 opengl. `p0-unit.junit.xml` 192/192 (SHA256 `1AFBAC4C…10AA`), `p0-opengl.junit.xml` 39/39 en
15,74 s (SHA256 `90CBE116…DFF2`). Précondition GL : pool-1..4 idle, pool-5 shutdown, aucun
processus ArtistLib/ctest.

Arbitrage reçu du lead : **D9 fermé (commit `67cbb02`), option (E)** ; interprétations (a), (b),
(c) validées. La base du fork reste `cb9f268` (le plan QA de `67cbb02` est lu depuis le dépôt
principal).

## Décisions consignées AVANT l'écriture des tests (2026-10-17T15:23:54+02:00)

### D9 (E) — contrat composant emprunté

- `OpenGLSetter<A>`, `OpenGLBinder<Classic>`, `OpenGLUnbinder<Classic>` : `static void
  on(OpenGLAttributeContext &)`. Mocks `MockSetter`/`MockBinder`/`MockUnbinder` : `on(C&)` et
  `mockOn(C&)`. `OpenGLAttributeFlow` : `HasBorrowedOnMethod` sur ses trois composants.
  `HasOnMethod` conservé (négatif utilisé par `UniformContractTests`), commentaire seul mis à jour.
- `NON_OPENGL_CONTEXT` supprimé des trois composants (contrôle inatteignable sur une référence,
  précédent `4d29842`). Les trois tests composant nul (`SetterTests:66-72`, `BinderTests:73-82`,
  `UnbinderTests:45-54`) sont remplacés par des static_asserts dans le nouveau
  `ARTIST/tests/src/graphic/pipeline/AttributeContractTests.cpp` : pour chaque composant, `on(C&)`
  invocable (positif) et `on(nullptr)`, `on(shared_ptr<C>)` non invocables (négatifs) — paire
  minimale, seul le type d'argument change.
- Le rejet typé du « pas de contexte » est porté par le wrapper : `ERROR::ATTRIBUTE::NULL_CONTEXT`
  (`std::invalid_argument`) à la construction, `ERROR::ATTRIBUTE::FREED_CONTEXT`
  (`std::runtime_error`) pour tout accès (`bind`/`unbind`/`set`/`getContext`) moved-from.
- `BUFFER_ID_NOT_SET` (SET et UNBIND) conservé tel quel.

### A4 — ownership unique de `IAttribute`

- Contexte en `std::unique_ptr<API::AttributeContext>` (constructeur explicite, défaut par
  `make_unique`), copie `= delete`, déplacement `= default` (aucun `noexcept` écrit : vérifié par
  static_assert), `ctx()` privé → `FREED_CONTEXT`. Ordre de `set` conservé : garde `ctx()`,
  `setValue`, dispatch.
- Virtuels protégés `bind`/`unbind(API::AttributeContext &)` ; `Attribute` et `MockAttribute`
  suivent. `MockAttribute` reste une sous-classe : `IAttribute` a un destructeur virtuel, la
  destruction via `unique_ptr<IAttribute>` est définie (différence assumée avec l'alias T1
  `MockUniform`).
- **(b) `getContext()` → `API::AttributeContext &`** (reste virtuel, surface conservée).
  Inventaire des consommateurs : `AttributeTests.cpp:103` (identité) et composant
  `PassTests.cpp:120-123` (appel chaîné, aucune copie conservée) ; aucun consommateur de
  production. La référence ne doit pas survivre à l'attribut.
- **(a) carte d'attributs du pass** (miroir T1 `NULL_UNIFORM`) : `PassContext::m_attributes` →
  `unique_ptr<IAttribute<API>>` ; `addAttribute(name, unique_ptr)` rejette un nul par
  `ERROR::PASS::NULL_ATTRIBUTE` (`std::invalid_argument`) avant mutation ; `getAttribute` → emprunt
  nullable `IAttribute<API> *` ; `getAttributes()` → référence sur la carte. `Pass::getAttributes`
  (type de retour) et commentaire `Pass::getContext` suivent ; `AttributeReader.hpp:58-65` :
  `make_unique` + `std::move`, mécanique, sans durcissement. Ceci remplace, sur validation
  explicite du lead, la consigne initiale « cartes `shared_ptr<IAttribute>` inchangées ».
  RED attendu : `addAttribute(name, nullptr)` accepté aujourd'hui (entrée nulle insérée).

### A3 — valeur stockée nulle

- Le texte A3 réécrit à `67cbb02` ne mentionne plus ce point ; D9 n'arbitre que le « pas de
  contexte » et ne retire rien. L'obligation du brief est donc maintenue (signalée au lead) :
  `OpenGLSetter<A>::on` rejette une valeur nulle (nul explicite ou jamais posée) par
  `ERROR::ATTRIBUTE::SET::NULL_VALUE` (`std::runtime_error`, famille des erreurs Setter), après
  `BUFFER_ID_NOT_SET`, **avant tout appel GL**. Un seul point de contrôle, au dernier composant
  avant le driver, couvre les deux chemins (`getValue` rend `nullptr` dans les deux cas).
- Écriture avant dispatch **conservée** (pas de décision explicite contraire) : si le Setter lève
  (`BUFFER_ID_NOT_SET`, `NULL_VALUE`), la valeur reste stockée. Documenté, non corrigé.
- Cycle propre : les tests `NULL_VALUE` sont écrits sous la forme `on(C&)` après la migration D9 ;
  leur RED est observé sur le build GREEN-C (sans le contrôle), puis GREEN-N.

### (c) `UNSUPPORTED_TYPE`

- Branche morte pour OpenGL et le mock (`OpenGLSetter<A>`/`MockSetter<A>` génériques →
  `HasComponent<Setter<T>>` toujours vrai) ; une API sans `Setter` est rejetée par la porte
  `OpenGLAttributeFlow`, et `IAttribute<API>::set` pour une telle API serait une erreur de
  compilation dure, jamais un throw. **GAP documenté** + négatif statique sur un stub sans `Setter`
  (paire minimale avec la fausse API acceptée). `set()` n'est pas rouvert.

### A5 — GL réel

- Nouveau `ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp` (glob CMake →
  re-glob ; label `opengl` et `RESOURCE_LOCK artist_opengl` hérités de `gtest_discover_tests`).
  Contexte 3.3 core réel, VAO lié : `bind` → `set` → `unbind` ; oracles : aucun throw,
  `glGetError() == GL_NO_ERROR`, `GL_BUFFER_SIZE == sizeof(T)`,
  `GL_VERTEX_ATTRIB_ARRAY_STRIDE == sizeof(T)`, `GL_ARRAY_BUFFER_BINDING == 0` après `unbind` ;
  cas `int` et `glm::vec3`. Requêtes d'état seulement, ni draw, ni image, ni readback (D4).
- Attendus : RED-B `TYPE_MISMATCH` levé par `set` (clé figée) ; après GREEN-B, 8 ≠ 4 et 8 ≠ 12
  (RED-A sur GL réel) ; vert après GREEN-A.

### Hors périmètre (laissé tel quel)

- `context/AttributeContext.hpp` : jumeau C4834 ligne 42 (`any_cast` valeur dont le résultat est
  jeté ; comportement correct) et libellé « SET » du mismatch de `getValue` — aucune réécriture
  opportuniste.
- Le VBO généré par `OpenGLBinder` n'est possédé ni libéré par personne (préexistant).

### Adaptations d'oracle prévues

- `SetTest` : RED-B = `MockSetter<int>` `Times(1)` identité + `MockSetter<shared_ptr<int>>`
  `Times(0)` (strictement plus fort) ; commentaire `b9c90fd` réécrit au GREEN-B ; `stored == value`
  conservé ; au GREEN-C, matcher `Ref(context)` (identité conservée, contexte emprunté).
- `GetContextTest` : égalité `shared_ptr` → identité d'adresse (même force).
- `SetterTests:39,59` : `sizeof(float)` (RED-A) + paire vec3.
- Supprimés : 3 tests composant nul (D9) → static_asserts.
- Flips mécaniques (D3) : `on(x)` → `on(*x)` ; `make_shared` → `make_unique` + `std::move` ;
  références dans `PassTests` composant.

### Ordre des cycles

1. RED-B (`SetTest` + A5) → GREEN-B (clé `Setter<T>`).
2. RED-A (`SetterTests` sizeof + vec3) → GREEN-A (`sizeof(A)`).
3. RED-C exécution (`AttributeOwnershipTests`) puis RED-C compilation (`AttributeContractTests`,
   objet seul) → GREEN-C (A4 + D9 + (a)), qui porte le RED-N.
4. GREEN-N (`NULL_VALUE`), puis non-régression complète et documentation.

## Exécution (2026-10-17, arbre `out/b`, HEAD `cb9f268`, un job, `run.py`)

Chaque log porte en en-tête HEAD, état du worktree et commande, en pied `# exit= seconds=`.
Avant chaque campagne GL : aucun autre membre au travail (`team_status`) et 0 processus
ArtistLib/ctest ; CTest séquentiel ; `--test-output-size-passed/failed 65536`.

### RED-B → GREEN-B : clé de dispatch (A1), 15:25–15:28

- Worktree au RED : seuls `AttributeTests.cpp` (oracle) et le nouveau
  `AttributeComponentTests.cpp` diffèrent ; **production intacte**. Re-glob
  `red-b-configure.log` exit 0 ; `red-b-build.log` exit 0 (4 étapes, 6,1 s) ; découverte 233
  (= 231 + 2 GL).
- Unit `-R ^unit\.AttributeTest\.` : exit 8, 1 échec sur 4 (`SetTest`) ; JUnit 4 cas / 1 échec,
  `A86A4072…0B77`. Diagnostics dans le test, avant le résumé GoogleTest :
  `AttributeTests.cpp(84)` « Mock function called more times than expected - returning
  directly. … Expected: to be never called / Actual: called once - over-saturated and active »
  (`MockSetter<shared_ptr<int>>` `Times(0)`) ; `(83)` « Actual function call count doesn't
  match EXPECT_CALL(*mock::MockSetter<int>::instance(), … / Expected: to be called once /
  Actual: never called - unsatisfied and active ».
- GL réel `-R ^opengl\.AttributeComponentTest\.` : exit 8, 2/2 échecs, JUnit `6FE7A4E6…5024`.
  `set` lève « ERROR::ATTRIBUTE::SET::TYPE_MISMATCH: The type of the value (class
  std::shared_ptr<int>) does not match… » pour `int` et `glm::vec3` (lignes 81 et 98 du fichier
  d'alors) : la clé figée lit un `shared_ptr<shared_ptr<int>>` dans `any(shared_ptr<T>)`.
  Confirme P2 §B.1 ; l'échec de lecture est signalé sous le libellé « SET ».
- Binaires RED préservés : `red-b/ArtistLib_tests.exe` `D72AF41C…1611`,
  `red-b/ArtistLib_component_tests.exe` `6DDB72EE…ECE2`.
- GREEN-B : `Attribute.hpp` `Setter<std::shared_ptr<int>>` → `Setter<T>` (garde et appel) ;
  commentaire `b9c90fd` de `SetTest` réécrit. Build exit 0 (25 étapes, 87,0 s) ;
  `-R ^unit\.Attribute` 17/17 (`DF689A1B…755B`). Le GL réel échoue encore 2/2, désormais pour
  A2 : `GL_BUFFER_SIZE` et `GL_VERTEX_ATTRIB_ARRAY_STRIDE` valent 8 contre `sizeof(int)` 4
  (lignes 84-85) et contre `sizeof(glm::vec3)` 12 (lignes 101-102) ; JUnit `B2E391B4…1DB5`.
  Le correctif sizeof était inatteignable avant la clé, comme l'annonçait P2.

### RED-A → GREEN-A : taille de l'élément (A2), 15:29–15:30

- Worktree au RED : `Attribute.hpp` (GREEN-B) et tests ; **`Setter.hpp` intact**. Oracles
  `SetterTests` : `glBufferData_mock(GL_ARRAY_BUFFER, sizeof(float), attrValue.get(),
  GL_STATIC_DRAW)` et `glVertexAttribPointer_mock(1, 1, GL_FLOAT, GL_FALSE, sizeof(float),
  nullptr)` `Times(1)`, constante indépendante du code testé à la place du
  `sizeof(attrValue.get())` tautologique, plus la paire `glm::vec3` (`sizeof(glm::vec3)`,
  GLSize 3).
- `red-a-build.log` exit 0 (2 étapes, 6,1 s). `-R ^unit\.AttributeSetterTests\.` : exit 8,
  4 échecs sur 6 (`CallBufferData`, `CallBufferDataWithVec3Size`, `CallVertexAttribPointer`,
  `CallVertexAttribPointerWithVec3Stride`) : « glBufferData_mock(34962, 8, …, 35044) / Expected
  arg #1: is equal to 4 [12] / Actual: 8 » et « glVertexAttribPointer_mock(1, 1, 5126, '\0', 8,
  NULL) / Expected arg #4: is equal to 4 [12] / Actual: 8 ». JUnit `1B09AE74…A555` ; binaire
  `red-a/ArtistLib_tests.exe` `6297889A…BDF0`.
- GREEN-A : `Setter.hpp` `sizeof(attrValue.get())` → `sizeof(A)` pour la taille et le stride.
  Build exit 0 (11 étapes, 31,3 s) ; `-R ^unit\.Attribute` 19/19 (`8B6C3EC6…3375`) ; GL réel
  2/2 (`E046E7F2…5802`).

### RED-C : ownership (A4), D9, (a), (b), 15:31 et 15:42

Exécution : nouveau `AttributeOwnershipTests.cpp` (14 cas), re-glob `red-c-configure.log`
exit 0, build exit 0 (2 étapes, 6,7 s), découverte 249. Production au RED = GREEN-B + GREEN-A
seulement. `-R AttributeOwner` : exit 8, 6 échecs sur 14, JUnit `B82CF98D…DFAB` :
- `NullContextIsRejected` : « Expected an exception to be thrown » ; `unique_ptr{}` se convertit
  silencieusement en `shared_ptr` nul, aucun `NULL_CONTEXT`.
- `PassContextRejectsNullAttribute` : aucune exception et `contains("a")` vrai (entrée nulle
  insérée).
- `MovedFrom` des 4 `TEST_P` : mocks « called more times than expected » (lignes 98, 117, 136)
  et « Expected an exception to be thrown ». Le destructeur virtuel supprime le déplacement
  implicite : le « move » copiait le `shared_ptr` et la source restait utilisable.
- `Loaded`/`Empty` (8 cas) verts : le comportement nominal était déjà correct.

Compilation : nouveau `AttributeContractTests.cpp`, re-glob `reglob-contract.log` exit 0, objet
seul (`--target …/AttributeContractTests.cpp.obj`) : `red-c-contract.log` exit 2, **exactement
19 C2607** (« échec de l'assertion statique ») aux lignes prévues : 30-31 (copie), 35
(construction depuis `shared_ptr`), 36 (`getContext()` ne rend pas `Context&`), 46-48 (carte du
pass), 56-64 (composants : `on(C&)` non invocable, `on(nullptr)` et `on(shared_ptr<C>)`
invocables ; note MSVC « impossible de convertir l'argument 1 de 'Context' en
'std::shared_ptr<…AttributeContext>' »), 104-106 (la porte accepte un composant
`on(shared_ptr<C>)`). Verts dès le RED, comme prévu : 32-34 (move nothrow, move-assign et
construction depuis `unique_ptr` sont vrais par copie ou conversion, d'où la preuve d'exécution
MovedFrom), 38, 45, 67, 103, 107-110 (motifs des paires minimales), 122.

### GREEN-C : A4 + D9 + (a) + (b), 15:55–15:56

Production : `Attribute.hpp` (`unique_ptr`, `NULL_CONTEXT` `invalid_argument`, copie supprimée,
déplacement par défaut, `ctx()` → `FREED_CONTEXT` `runtime_error`, `getContext()` → `Context&`,
virtuels protégés `bind`/`unbind(Context&)`) ; `Setter`/`Binder`/`Unbinder`
`on(OpenGLAttributeContext&)`, `NON_OPENGL_CONTEXT` supprimé ; `AttributeConcept.hpp` →
`HasBorrowedOnMethod` ; `ComponentConcept.hpp` (commentaires) ; `PassContext.hpp` (carte
`unique_ptr`, `NULL_ATTRIBUTE` avant mutation, `getAttribute` emprunt nullable) ; `Pass.hpp`
(type de retour, commentaire) ; `AttributeReader.hpp` (`make_unique` + `std::move`). Mocks :
`MockSetter`/`MockBinder`/`MockUnbinder` `on(C&)`/`mockOn(C&)`, `MockAttribute` emprunté.
Tests : flips mécaniques ; 3 tests composant nul supprimés
(`AttributeSetterTests.SetAttributeTest_ThrowExceptionWhenContextIsNullptr`,
`AttributeBinderTests.BindAttributeTest_invalidContext`,
`AttributeUnbinderTests.BindAttributeTest_nullContext`) et includes morts de `BinderTests`
retirés ; tests `NULL_VALUE` écrits (RED-N ci-dessous).

`green-c-build.log` exit 0 (34 étapes, 127,2 s) : `AttributeContractTests.cpp.obj` compile, les
19 assertions passent. Découverte fraîche 249 = 207 unit + 42 opengl. Unit complet 205/207
(`A306487F…3FB5`), GL complet 41/42 (`B93B2877…5762`) : **les 3 seuls échecs sont les tests
RED-N** ; les 14 tests d'ownership passent.

### RED-N → GREEN-N : valeur nulle (A3), 15:56–15:58

RED observé sur le build GREEN-C, qui ne contient pas encore le contrôle :
- Unit `SetAttributeTest_ThrowExceptionWhenValueIsNull` et `…IsNotSet` : `SetterTests.cpp(124)`
  et `(143)` « Mock function called more times than expected … glBufferData_mock(34962, 4,
  NULL, 35044) / Expected: to be never called » ; `(126)` et `(145)` idem pour
  `glVertexAttribPointer_mock(0, 0, 0, '\0', 4, NULL)` ; `TestUtils.hpp(16)` « Expected an
  exception to be thrown ».
- GL réel `SetNullValueIsRejectedBeforeUpload` : aucune exception ; `boundArrayBufferSize()` et
  `attributeStride(0)` valent 4 au lieu de 0 (lignes 125-126), sans erreur GL. `glBufferData`
  avec un pointeur nul est légal (stockage non initialisé) : seul un rejet C++ empêche
  d'allouer et de décrire un attribut sans contenu.
- Binaires RED-N préservés (= build GREEN-C) : `red-n/ArtistLib_tests.exe` `85B5618B…BE3B`,
  `red-n/ArtistLib_component_tests.exe` `9B33A906…09B5`.
- GREEN-N : `OpenGLSetter<A>::on` lève `ERROR::ATTRIBUTE::SET::NULL_VALUE` après
  `BUFFER_ID_NOT_SET` et avant tout appel GL. Build exit 0 (12 étapes, 34,2 s) ;
  `-R ^unit\.(Attribute|OwnerStates/AttributeOwnerStateTests)` 32/32 (`50B17E17…29FF`) ; le cas
  GL passe dans la campagne finale.

## REFACTOR et non-régression finale (15:58–15:59)

- Aucune restructuration au-delà des GREEN ; diff de production relu (callers, ownership,
  chemins d'exception) ; `git diff --check` exit 0.
- `final-configure.log` exit 0 ; `final-build.log` « ninja: no work to do. » ; découverte
  fraîche 249 = 207 unit + 42 opengl.
- Unit `-L unit` 207/207 (3,81 s) ; JUnit 207 cas / 207 distincts / 0 échec / 0 skip,
  `F0E5FC04FD0C6D986005A031612AEDD2D752D9BC0FF1DB7B509E98EEE86C0789`.
- GL réel `-L opengl` 42/42 (17,34 s) ; JUnit 42/42/0/0,
  `24944197DDA25DD025C78262E5844D250F45B9F995A8F4F680B7DB21CCB2E1FD` ; témoin
  « NVIDIA GeForce RTX 3090/PCIe/SSE2 | 3.3.0 NVIDIA 617.42 » dans les 42 cas. Précondition :
  pool-1..4 idle, pool-5 arrêté, 0 processus ArtistLib/ctest.
- Avertissements : aucun nouveau par rapport à P0 dans aucun build (P0 : 63 uniques, C4834
  `AttributeContext.hpp(42)`, `ShaderLifetimeTests.cpp(194)`, `PassContextTests`,
  `PipelineContextTests`, C4005 `glFunctionMock.hpp(39-89)`) ; avertissement CMake de longueur
  de chemin objet présent depuis P0.
- Comptages : unit 192 → 207 (+2 vec3 Setter, +14 ownership, +2 `NULL_VALUE`, −3 tests
  composant nul) ; opengl 39 → 42 (+3 `AttributeComponentTest`) ; le contrat statique n'ajoute
  aucun cas.
- Identités : `g1-identity-start.txt` (15:00:37), `g2-identity-tested.txt` (16:00:31, sources
  testées : 19 modifiés + 3 non suivis), `g2-identity-end.txt` après le commit.

## Obligations → preuves

| Obligation | Nominal | Négatif | Tag |
|---|---|---|---|
| A1 clé `Setter<T>` | `SetTest` (`MockSetter<int>` `Times(1)`, `Ref` du contexte possédé) ; GL réel `int`, `glm::vec3` | `SetTest` `MockSetter<shared_ptr<int>>` `Times(0)` ; RED-B 1/4 unit, 2/2 GL | executed |
| A2 `sizeof(A)` | `CallBufferData`, `CallVertexAttribPointer` (+ vec3) ; GL réel taille et stride = `sizeof(T)` | RED-A 4/6 (8 ≠ 4, 8 ≠ 12) ; GL après GREEN-B 2/2 (8 ≠ 4, 8 ≠ 12) | executed |
| A3 valeur nulle | GL réel `int`/`vec3` non nuls | `…ValueIsNull`, `…ValueIsNotSet` (aucun appel GL) ; GL `SetNullValueIsRejectedBeforeUpload` (taille 0, stride 0, `GL_NO_ERROR`) | executed |
| A3 écriture avant dispatch | — | — | documenté, conservé |
| A4 ownership | OwnerState Loaded/Empty ×4 ; 32-34, 38 | 30/31/35 ; OwnerState MovedFrom ×4 (`FREED_CONTEXT`) ; `NullContextIsRejected` | executed ; static (30/31/35) ; static-presence (32-34, 38) |
| A5 GL réel | `SetIntUploadsOneIntWithoutGLError`, `SetVec3UploadsOneVec3WithoutGLError` | `SetNullValueIsRejectedBeforeUpload` | executed (contexte 3.3 core réel) |
| D9 composants empruntés | 56/59/62 ; 67 | 57/58/60/61/63/64 (paires minimales) ; 104-106 (motif 107) | static ; static-presence (67, 107) |
| D9 « pas de contexte » | OwnerState Loaded/Empty | `NullContextIsRejected` (`NULL_CONTEXT`) ; MovedFrom (`FREED_CONTEXT`) | executed |
| (a) carte du pass | `PassTest.GetAttributes`, `AttributeReaderTests`, composant `AttributeReadingTest` ; 45 | 46-48 ; `PassContextRejectsNullAttribute` (carte inchangée) | executed ; static (46-48) ; static-presence (45) |
| (b) `getContext()` emprunt | 36 ; `GetContextTest` (identité d'adresse) ; `ContextBorrowIsRefusedOnlyWhenMovedFrom` Loaded/Empty | `…/MovedFrom` (`FREED_CONTEXT`) | executed ; static (36) |
| (c) `UNSUPPORTED_TYPE` | 67, 103 (APIs avec `Setter` acceptées) | 122 (API sans `Setter` refusée par la porte) | static-presence (vert dès le RED) + GAP |
| Non-régression | 207/207 unit, 42/42 GL | — | executed |

Numéros = lignes de `AttributeContractTests.cpp`. `static` : échec C2607 au RED de compilation
pour le motif attendu, puis vert ; `static-presence` : déjà vert au RED, non causal.

Registre inverse : `AttributeTests.SetTest` (`b9c90fd`, static-presence de l'oracle du bug) →
RED dédié puis mise à jour strictement plus forte (executed) ; `SetterTests`
`CallBufferData`/`CallVertexAttribPointer` (static-presence tautologique) → oracle littéral et
RED-A (executed) ; 3 tests composant nul → contrat statique (D9). Aucun orphelin connu.

## Adaptations d'oracle réalisées (toutes après RED conservé)

- `SetTest` : `MockSetter<int>` `Times(1)` (au RED-B `mockOn(attributeContext)` sur le
  `shared_ptr` ; au GREEN-C `Ref(ownedContext)`, référence prise avant `std::move`, même
  identité) et `MockSetter<shared_ptr<int>>` `Times(0)` (nouveau, plus fort) ;
  `ASSERT_NO_THROW` et valeur stockée non nulle, même objet, égale à 1, conservés et lus via
  `ownedContext`. `TearDown` `b9c90fd` inchangé.
- `GetContextTest` : égalité de `shared_ptr` → identité d'adresse `&getContext()` (même force).
- `SetterTests` (lignes 39 et 59 d'origine) : `sizeof(float)` (RED-A) + 2 cas vec3.
- Supprimés (D9) : 3 tests composant nul, remplacés par les static_asserts 56-64.
- Composant `PassTest.AttributeReadingTest` : miroir de `UniformReadingTest` (`const auto &`,
  `ASSERT_NO_THROW((void)getContext())`, `.getGLType()`).
- Flips mécaniques (D3) sans changement d'attente : `on(x)` → `on(*x)`, `make_shared` →
  `make_unique` + `std::move` (`AttributeTests`, `PassTests` unit, tests Binder/Unbinder/Setter).
- `_` n'apparaît qu'avec `Times(0)` (`NULL_VALUE`, `MockSetter<shared_ptr<int>>`), soit le
  négatif le plus strict (aucun appel toléré), dans les nouveaux tests OwnerState où l'observable
  est le compte 1/0 selon l'état (l'identité du contexte est prouvée par `SetTest`), et dans
  `BindTest`/`UnbindTest` inchangés. Aucune tolérance, aucun `WILL_FAIL`, aucune assertion
  retirée hors des 3 tests D9.

## Critères de sortie du plan QA (auto-évaluation auteur, non indépendante)

1. Nominal + négatif + tag par obligation : PASS au périmètre (tableau) ; (c) reste un GAP
   documenté (static-presence).
2. Comptages non nuls, 0 skip, artefacts RED avant réécriture d'oracle : PASS. ASan (C3) :
   INCONCLUSIVE, non exécuté (sélection déclarée par le lead à T4, D5).
3. Pas de verdict global : respecté.
4. Registre : mis à jour ci-dessus, aucun doublon faible connu.
5. Artefacts sous `out/mission-f7/t2/` : PASS.

## Documentation

- `docs/development/known-incidents.md` : addendum « Addendum F7 T2 » inséré après la
  ligne 472 (+33/−0). Lignes 440-471 identiques au blob `cb9f268` : SHA256 `FF4E27B5…1523` sur
  2310 octets, avant et après (référence `known-incidents-440-471-head.bin`).
- `docs/_meta/session-2026-10-17-attribute-oracle-{plan,validation}.json` : clé
  `f7_t2_followup` ajoutée en fin d'objet ; clés existantes et ordre inchangés (vérifié contre
  `HEAD`), JSON valide, LF ; empreintes citées recalculées sur les artefacts.
- Non modifiés (hors périmètre T2, pour le lead) : `docs/architecture/current-state.md:141-142`
  décrit encore des cartes `shared_ptr<Uniform>/shared_ptr<IAttribute>` (périmé pour Uniform
  depuis T1) ; ADR-0003 reste Accepted/partial ; `next-mission.md` décrit l'état avant mission.

## Incidents et réserves

- Branche d'abord créée et extraite par erreur dans le dépôt principal ; principal remis sur
  `main` (`cb9f268`, aucun fichier modifié), puis branche extraite dans le worktree pool-6.
- `known-incidents-440-471-before.txt` (premier instantané, 2447 octets) est **infidèle** :
  transcodage PowerShell 5.1 (BOM, mojibake cp1252, CRLF final). Son empreinte `F67277E5…`
  n'est pas une référence ; remplacé par `known-incidents-440-471-head.bin`, extrait en octets
  bruts du blob `HEAD`.
- Réserve (précédent T1) : arbre `out/b` dans le worktree (ignoré par Git), non attribué par le
  lead. `out/mission-f7/t2/` n'est pas versionné et ne suit pas le merge : à copier avant tout
  nettoyage du worktree.
- Écarts de libellé : la fiche cite « A1-A6 », le plan persisté s'arrête à A5. A1/D2 du plan
  nomme `set<float>` → `MockSetter<float>` ; la fiche impose `MockSetter<int>`, suivie : le
  `SetTest` unitaire ne couvre que `int`. La généricité est prouvée par `glm::vec3` sur GL réel
  (TYPE_MISMATCH → 8 ≠ 12 → vert) et par les cas `OpenGLSetter<glm::vec3>` ; aucun cas
  unitaire de dispatch `set<float>`, à arbitrer.
- Limites : ni ASan (T4) ni shuffle ; move-assign prouvé statiquement avec la même garde
  `ctx()`, sans test d'exécution dédié (dont l'assignation vers un owner déjà chargé) ;
  `nothrow` du move non causal (vrai par copie au RED) ; (c) non causal ; écriture avant
  dispatch : après un throw du Setter (`BUFFER_ID_NOT_SET`, `NULL_VALUE`) la valeur reste
  stockée ; ni draw, ni image, ni readback, ni perf, ni Release, ni autre plateforme.
- Hors périmètre, constaté : jumeau C4834 `AttributeContext.hpp(42)` ; libellé « SET » et
  `typeid(m_value).name()` (affiche `std::any`, pas le type stocké) dans le mismatch de
  `getValue` ; VBO créé par `OpenGLBinder` jamais libéré.

## Contrats pour les consommateurs

- `PassContext::addAttribute` prend un `unique_ptr`, rejette un nul (`NULL_ATTRIBUTE`, carte
  inchangée) et remplace un nom existant (l'ancien attribut est détruit) ; `getAttribute` rend
  un emprunt nullable ; `getAttributes()` et `Pass::getAttributes()` rendent par référence une
  carte non copiable.
- `IAttribute::getContext()` rend un `Context&` qui ne doit pas survivre à l'attribut ; tout
  accès après déplacement lève `FREED_CONTEXT` ; une référence prise avant un move désigne
  ensuite le contexte possédé par la destination.
- Les composants attribut prennent `OpenGLAttributeContext&` ; le « pas de contexte » est
  rejeté par le wrapper, plus par les composants.
- Les emprunts sont invalidés par la destruction du pass ou de l'attribut, ou par le
  remplacement du nom. `Attribute` ne possède aucun objet GL (VBO non libéré, préexistant).

## Commit et identité de fin (2026-10-17T16:26:54+02:00)

- `d30e69fc9059d9195f71ff4eb2436c14a9c99a1e` « fix(attribute): key set dispatch on Setter<T>,
  upload sizeof(T) and own contexts uniquely », parent `cb9f268`, branche
  `ensemble-artist-mcp-lifecycle-mission-5caa4a18f43401c7#eiyn0g-pool-6` ; 25 fichiers,
  +691/−138 ; ni push ni merge.
- `g2-identity-end.txt` : 1 commit depuis la base, 0 delta, 0 non suivi ; octets bruts = blobs
  `HEAD` pour les 42 fichiers suivis.
- Comparaison mécanique `g2-identity-tested.txt` / `g2-identity-end.txt` : 40 entrées communes,
  seule `docs/development/known-incidents.md` diffère (addendum) ; les deux `_meta` n'existent
  que dans l'identité de fin. Les 34 sources code/tests/build sont donc identiques aux octets
  testés par les campagnes finales ; les docs, hors build, ont été modifiées après.
- `main` (`67cbb02`) ne touche que `.opencode/evidence/mission-gpu-qa/f7-qa-plan.md`, hors des
  25 chemins : aucun conflit attendu au merge.
