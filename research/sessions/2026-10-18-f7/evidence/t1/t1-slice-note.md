# T1 — note de tranche (pool-5, mission F7 option A)

Base : `4f64a2b87f5aa8c3590ded4596aca906b5d320dc` (G1 confirmé 2026-10-17T13:50:27+02:00).

## Décisions consignées AVANT l'écriture des tests concernés (2026-10-17T14:19:05+02:00)

### RED-3 — mismatch au `setValue` : REPLACE (option P1 la plus petite cohérente)

- `setValue<T>` continue de remplacer la valeur et son type ; aucune rejet côté écriture.
- `getValue<T>` est resserré : type stocké différent → `ERROR::UNIFORM::GET::TYPE_MISMATCH`
  (message : type demandé + type réel `m_value.type().name()`) ; aucune valeur →
  `ERROR::UNIFORM::GET::UNINITIALIZED`. Plus de déréférencement de `nullptr`.
- Motifs : (1) changement minimal, aucun nouveau chemin de rejet en écriture ; (2) l'autorité
  du type d'un uniform est le type GL lu par le reader (`m_type`), un « freeze au premier set »
  côté C++ serait une règle sémantique partielle sans lien avec ce type — durcissement
  sémantique = T3 ou plus tard ; (3) la sûreté de lecture est assurée par le rejet typé GET.
- Conséquence D1 : le seul TYPE_MISMATCH atteignable est GET. Le `catch` mort de `setValue` et
  son message « SET » sont supprimés : aucun émetteur `SET::TYPE_MISMATCH` n'existe. Le message
  GET porte le libellé GET (corrige le « SET » trompeur relevé par P1) et le type réel.
- U5 (inférée) sous REPLACE : un `setValue` échoue seulement si la copie de la nouvelle valeur
  lève ; `std::any::operator=` donne la garantie forte → test de caractérisation (vert avant et
  après, tag `inference`), pas un RED.

### A3 — null-check de `addUniform` : AJOUTÉ (`ERROR::PASS::NULL_UNIFORM`, `std::invalid_argument`)

- Le `PassContext` devient propriétaire unique des wrappers :
  `unordered_map<string, unique_ptr<Uniform<API>>>` (même forme que `m_shaders`), `addUniform`
  prend un `unique_ptr`, `getUniform` rend un emprunt nullable `Uniform<API>*` (absent →
  `nullptr`, sémantique conservée pour `withUniform`), `getUniforms` rend la carte par référence.
- Avec un `unique_ptr`, le null reste représentable : il est rejeté à la frontière, symétrique
  de `addShader`/`NULL_SHADER`, pour garder l'invariant « aucune entrée nulle » sur lequel
  s'appuient les consommateurs de `getUniforms()`.
- Condition « si le RED causal l'établit » : RED attendu = `addUniform(name, nullptr)` accepté
  aujourd'hui (aucune exception, entrée nulle dans la carte) ; décision maintenue seulement si
  ce RED est observé.

### Autres choix locaux

- `Uniform` : contexte en `unique_ptr`, copie supprimée, déplacement par défaut (aucun
  `noexcept` écrit : la construction par déplacement l'est implicitement) ;
  `ERROR::UNIFORM::NULL_CONTEXT` (`invalid_argument`) à la construction ; `getContext()` emprunt
  `API::UniformContext&` ; tout accès moved-from (`set`/`get`/`getContext`) →
  `ERROR::UNIFORM::FREED_CONTEXT` (`runtime_error`), précédent Shader/Pass.
- Setters uniform OpenGL (34) et mock : `on(C&)` emprunté ; `OpenGLUniformFlow` passe à
  `HasBorrowedOnMethod` ; `HasOnMethod` conservé pour Attribute (T2).
- `MockUniform` devient un alias de `Uniform<API>` : stocké via `unique_ptr<Uniform<API>>`, une
  classe dérivée serait détruite par une base sans destructeur virtuel (UB).

## RED causal (avant toute modification de production, 2026-10-17T14:33:57+02:00)

Arbre `out/b`, HEAD `4f64a2b`, production intacte (seuls les fichiers de test diffèrent).

- Incident de harnais (pas un RED) : `red-a-build.log` exit 2, `UniformOwnershipTests.cpp`
  sans `MockShaderContext.hpp` (C2027 via `~unique_ptr<IShader<MockOpenGL>>`). Include ajouté.
- Build RED exécution : `red-b-build.log` exit 0 (10,3 s) ; C4834 `UniformContext.hpp(24)` et
  `(39)` = `any_cast` pointeur dont le résultat est jeté (catch morts).
- Découverte : `red-b-discovery.log` 16 nouveaux tests ; unit 192 (= 176 + 16).
- Exécution isolée CTest (un processus par test) : `red-b-runtime.log` exit 8,
  `red-b-runtime.junit.xml` 16 cas / 16 distincts / 9 échecs / 0 skip,
  SHA256 `81D97B60C03580BBA5EB4947C914C2FBAFF2174835C07DA49E9129A4DFE1FCF5`.
  - Crash = violation d'accès SEH `0xc0000005` interceptée par GoogleTest (PAS un échec
    d'assertion) : RED-1 `GetValueOfAnotherTypeThrowsTypeMismatch`, RED-2
    `GetValueWithoutValueThrowsUninitialized`, RED-3 `SetValueReplacesTheStoredType` (lecture de
    l'ancien type), `GetReadsOnlyAnOwnedValue/Empty`, `/MovedFrom`,
    `SetRunsTheSetterOnlyWithAnOwnedContext/MovedFrom`.
  - Échec d'assertion « Expected an exception » : `NullContextIsRejected`,
    `ContextBorrowIsRefusedOnlyWhenMovedFrom/MovedFrom`, `PassContextRejectsNullUniform`
    (+ `contains("u")` vrai : entrée nulle insérée). **A3 : RED causal observé, décision maintenue.**
  - Verts avant correction (oracles nominaux / caractérisation) : U1 `GetValueReturnsTheStoredValue`,
    U5 `FailedSetValueKeepsTheStoredValue` (tag inference), Loaded/Empty get, getContext, set.
- RED compilation (objet seul, n'empêche pas le build exécution) : `red-c-compile.log` exit 2,
  6 × C2607 exactement aux contrats attendus de `UniformContractTests.cpp` :
  42 copie constructible, 43 copie assignable, 47 constructible depuis `shared_ptr`,
  48 `getContext()` ne rend pas `Context&`, 88 `OpenGLUniformFlow` accepte un setter
  `on(shared_ptr<C>)`, 100 setters OpenGL réels non empruntés. Raison du rejet 88 fixée par paire
  minimale : 87 (setter emprunté accepté) et 89 (le setter partagé n'est pas appelable avec `C&`)
  passent. Verts dès le RED : move noexcept, move-assign, construction depuis `unique_ptr`,
  `getValue` non `noexcept`, rejets non-void et `on` absent.

## GREEN (2026-10-17T14:43:17+02:00)

Modifications (production) :
- `context/UniformContext.hpp` : `getValue` → `UNINITIALIZED` si vide, `TYPE_MISMATCH` (GET, type
  demandé + `m_value.type().name()`) si `any_cast` pointeur nul ; `setValue` = remplacement
  (`std::any::operator=`, garantie forte). Catch morts et message « SET » supprimés.
- `pipeline/Uniform.hpp` : `unique_ptr` contexte, `NULL_CONTEXT` (`invalid_argument`), copie
  `= delete`, move `= default` (noexcept implicite, vérifié par static_assert), `ctx()` privé →
  `FREED_CONTEXT` (`runtime_error`) pour `set`/`get`/`getContext` ; `getContext()` → `Context&`.
- `opengl/validator/UniformConcept.hpp` : `HasBorrowedOnMethod`. `validator/ComponentConcept.hpp` :
  commentaires seulement (`HasOnMethod` conservé pour Attribute).
- `opengl/pipeline/component/uniform/Setter.hpp` : 34 `on(OpenGLUniformContext&)`, 98 `->` → `.`.
- `context/PassContext.hpp` : carte `unique_ptr<Uniform>`, `addUniform(unique_ptr)` +
  `NULL_UNIFORM` avant mutation, `getUniform` → `Uniform*` nullable, `getUniforms` par référence.
- `pipeline/Pass.hpp` : type de retour `getUniforms()` + commentaire `getContext()` mis à jour.
- `opengl/pipeline/component/pass/UniformReader.hpp` : `make_unique` + transfert (mécanique, aucun
  durcissement).
Tests/mocks : `MockSetter` (uniform) `on(C&)`, `MockUniform` alias, `MockPass::getUniforms`,
`UniformTests` (10 arrangements `make_unique` + `std::move`, attentes intactes), `PassTests` unit
(3 `make_unique`, 2 `std::move`), `PassTests` composant (références ; `getContext()` emprunt).

Preuves :
- Build : `green-a-build.log` exit 0, 29 étapes, 113,1 s. Aucun nouvel avertissement
  (63 uniques vs 65 en P0) ; disparus : C4834 `UniformContext.hpp(24)` et `(39)`.
  `UniformContractTests.cpp.obj` recompilé : tous les static_asserts passent.
- 16 tests RED : `green-a-runtime.log` 16/16, `green-a-runtime.junit.xml` 16/16/0 échec/0 skip,
  SHA256 `B57360BBDC94CFC374882A3AE3FBBA7DAA75F7FFFC38D73EE40B68729897B299`.

## REFACTOR et non-régression

- Pas de restructuration au-delà du GREEN ; diff relu, `git diff --check` propre.
- Découverte fraîche `green-discovery-all.log` : 231 = 192 unit + 39 opengl (P0 : 215 = 176 + 39).
- Unit : `green-unit.log` 192/192 (2,99 s) ; `green-unit.junit.xml` 192 cas / 192 distincts /
  0 échec / 0 skip, SHA256 `379A7C6A7043041018D441A56E2F0BA0584864ACD35070C11C231A1F3990E108`.
- OpenGL réel : `green-opengl.log` 39/39 (15,61 s) ; `green-opengl.junit.xml` 39/39 distincts /
  0 échec / 0 skip, SHA256 `A6193122E41598865592E4BB77D6915FC977E4014300356DA7E98EF1C7199279`.
  Précondition : pool-1..4 idle (team_status), aucun processus ArtistLib/ctest ; CTest séquentiel
  par défaut (`CTEST_PARALLEL_LEVEL` vide ; P0 avait `-j1`).
- U6 : les 10 attentes `glUniform*_mock` de `UniformTests.cpp` sont identiques au témoin
  `b9c90fd` ; 0 ligne EXPECT/ASSERT/Times modifiée vs `4f64a2b`.

## Obligations → preuves

| Obligation | Nominal | Négatif | Tag |
|---|---|---|---|
| U1 get nominal / mismatch | `GetValueReturnsTheStoredValue`, OwnerState Loaded | RED-1 `GetValueOfAnotherTypeThrowsTypeMismatch` | executed |
| U2 any vide | OwnerState Loaded | RED-2 `GetValueWithoutValueThrowsUninitialized`, OwnerState Empty | executed |
| U3 ownership | static 44-46 (move, `unique_ptr`) ; Loaded/Empty `getContext`/`set` | static 42/43/47/48 ; MovedFrom ×3 (`FREED_CONTEXT`) ; `NullContextIsRejected` | static + executed |
| U4 concept | static 87 (emprunté), 100 (34 setters réels) | static 88/90/92 (paires minimales, motifs 89/91) | static |
| U5 setValue échoué | RED-3 `SetValueReplacesTheStoredType` (REPLACE) | `FailedSetValueKeepsTheStoredValue` | inference (caractérisation verte avant/après) |
| U6 non-rég GL mock | 10 `UniformTests` | — (attentes exactes inchangées) | executed |
| D1 messages | RED-3 (type stocké `float`) | RED-1 (libellé GET, type stocké `int`, pas `::SET::`) | executed |
| A3 null uniform | `PassTest.GetUniforms`/`WithUniforms`, composant `UniformManagementTest` | `PassContextRejectsNullUniform` (carte inchangée) | executed |

## Incidents et réserves

- Environnement (pas un RED) : C1083 `''` : argument non valide dans `out/build/windows-msvc-debug`
  (`p0-build2.log`, [39/48] `PipelineLifetimeComponentTests.cpp.obj`, chemin objet max 274 car.).
  Arbre court `out/b` dans le worktree (max 251 car.), `cmake --preset windows-msvc-debug -B out/b`,
  toolchain Conan de `out/build/windows-msvc-debug/conan_toolchain.cmake` ; googletest v1.14.0
  `f8d7d77c` cloné par FetchContent dans l'arbre. Ni `subst`, ni changement Git/CMake.
- **Réserve** : `docs/architecture/missions/2026-10-16-pipeline-ownership.md:481-484` demande des
  sorties courtes sous `out/` du dépôt principal ou le temp OpenCode approuvé, ressources
  attribuées par le lead ; `out/b` est dans le worktree (ignoré par Git) sans attribution → à
  régulariser ou accepter par le lead.
- Incident de harnais : include `MockShaderContext.hpp` manquant (`red-a-build.log`), corrigé.
- Écart : le plan QA cite « 9 UniformTests » ; `UniformTests.cpp` en contient 10 (les 10 vérifiés).
- SEH : sous MSVC Debug x64, GoogleTest intercepte la violation d'accès et CTest isole chaque test ;
  « la suite entière plante » (P1) n'a pas été observé dans ce mode. Pas d'exécution directe avec
  `--gtest_catch_exceptions=0` (inutile après GREEN).
- Limites : pas d'ASan (barrière T4, D5) ; move-assign prouvé statiquement + même garde `ctx()`,
  sans test d'exécution dédié ; `set<T>` sur un type sans spécialisation OpenGL reste une erreur
  de compilation (template primaire, préexistant) ; jumeau C4834 `AttributeContext.hpp(42)` laissé
  à T2 ; cohérence type GL / type C++ laissée à T3 ; docs/ADR non modifiés (décision lead).
- Contrats pour les consommateurs : `getUniform` rend un emprunt nullable (plus de copie
  `shared_ptr`) ; la carte `getUniforms()` n'est plus copiable ; `addUniform` prend un
  `unique_ptr` et remplace un nom existant (l'ancien uniform est détruit) ; les emprunts
  (`getUniform`, `getContext`) sont invalidés par destruction du pass/uniform ou remplacement du
  nom ; après un move, une référence de contexte désigne le contexte désormais possédé par la
  destination. Aucun objet GL n'est possédé par `Uniform` (une location n'est pas une ressource).
