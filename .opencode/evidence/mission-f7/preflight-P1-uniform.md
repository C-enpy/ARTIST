# P1 — Préflight Uniform (mission F7 option A)

Preuve d'intégration, transmise par pool-2 (cpp-engineer), relecture lead 2026-10-17.
HEAD lu : `eb0dd14`, arbre lu inchangé (aucune mutation, aucun build — interdits P1 respectés).
Statut : lecture seule ; tous les constats restent **INCONCLUSIVE** jusqu'au RED exécuté par l'auteur T1.

## A. Constats vérifiés (statique + clause standard, non exécutés)

1. **`UniformContext.hpp:24,31,39` — constat exact.**
   - `std::any_cast<T>(&m_value)` appelle la surcharge pointeur « `const T* any_cast(const any*) noexcept` » (C++17) : pointeur sur le contenu si typeid matché, sinon `nullptr`. **Ne lève jamais** ; « Exceptions 1-3 » couvre uniquement les formes valeur.
   - Conséquence : les `catch bad_any_cast` aux lignes 26-29 et 41-44 sont du **code mort** ; les deux `TraceableException TYPE_MISMATCH` (l.28, l.43) sont inatteignables.
   - L.31 `return *std::any_cast<T>(&m_value);` : any vide OU type différent → `nullptr` → **déréférencement nullptr = UB**. Le garde `has_value()` (l.20) ne protège que le chemin du try inutile : sur any vide, aucun code ne jette avant le deref.
   - `setValue` (l.37-45) : tout type est accepté et écrase l'ancien (l.45) ; le contrat « TYPE_MISMATCH » implicite n'est jamais appliqué.
   - Message d'erreur trompeur : `typeid(m_value)` (l.28, l.43) = typeid statique de `std::any`, pas du contenu ; la forme correcte serait `m_value.type().name()` (dans le code mort).
2. **Contrat TYPE_MISMATCH non testé — confirmé** (détails B2/C).

## B. Consommateurs (grep exact ; à revérifier à l'implémentation, LSP incomplet sur templates)

- `setValue<T>` : `Uniform.hpp:72` ; `UniformTests.cpp:34,53,72,92,111,130,149,168,187,207` (préremplissage du type identique ensuite : jamais de mismatch testé ; aucun test TYPE_MISMATCH, aucun test get sur any vide ; `UniformContractTests.cpp:24-30` = existence/concept seulement).
- `getValue<T>` : `Uniform.hpp:93` ; `Setter.hpp:30,44,58,72,86,100,114,128,142,156,168,180,192,204,216,228,240,252,264,276,288,300,312,324,336,352,364,376,388,400,412,424,436,448` (34 sites, résultat transmis directement à `glUniform*`).
- Via `Uniform::set<T>/get<T>` : `Pass.hpp:138-146` (withUniform, guard null à l.140). Type-level : `Pass.hpp:151-154` et `:159-162` (cartes `unordered_map<string, shared_ptr<Uniform<IAttribute>>>`) ; `PassContext.hpp:138-141, :147-150` (getUniforms/getAttributes), `:93-100` (getUniform : copie du shared_ptr, nullptr si absent), `:50-53` (addUniform SANS null-check, contraste addShader `:35-42` throw NULL_SHADER).
- Ownership actuel : `Uniform.hpp:52-56,102` (chaque Uniform détient un shared_ptr unique ; ctor default = make_shared) ; `UniformReader.hpp:54-59` (make_shared par uniform du programme, wrap, addUniform).
- `getContext()` : `Uniform.hpp:96-99` retourne une **copie** du shared_ptr (pas un emprunt) ; consommateur réel : component-tests `PassTests.cpp:108-109` (usage chaîné, aucune copie retenue). `MockUniformContext.hpp:13` hérite le template parent : le RED est couvrable via mock sans GL.
- `UniformReaderTests.cpp:51-52` : assertions sur la carte getUniforms() — consommateurs indirects, aucun appel direct UniformContext, conformes au constat.
- Hors périmètre P1, même forme : `Attribute.hpp:86,88-91` (Setter hardcodé shared_ptr<int>) et `AttributeTests.cpp:89` (getValue<int> AttributeContext) → P2.

## C. Comportement observable aujourd'hui (à confirmer par le RED exécuté)

- `setValue(float)` puis `setValue(int)` : aucune exception, mismatch invisible (silent replace).
- `getValue<T>` sur any jamais set : UB (deref nullptr).
- `getValue<T>` sur any d'un autre type : UB (deref nullptr), TYPE_MISMATCH jamais levé.
- Chemin nominal (set du même type puis getValue par le Setter) : OK aujourd'hui ; aucune casse prévisible si le correctif fait lever proprement.

## D. Plan RED T1 (squelettes seulement — RED causal réel et code de test = auteur T1, fork P1)

Sites suggérés : suite unitaire, `MockUniformContext` (hérite le parent, pas de GL requis) ; message attendu via `expectSpecificError` (pattern `UniformReaderTests.cpp:20`).

- **RED-1 mismatch get** : `setValue<int>(1)` puis `getValue<float>()` → `EXPECT_THROW(TraceableException TYPE_MISMATCH)`. État actuel : crash access violation (UB) = RED causal authentique, PAS un échec d'assertion gtest ; documenter que l'échec observé est le SEGV/0xC0000005 du déréférencement null, pas le mock.
- **RED-2 any vide** : contexte default-constructé, même EXPECT_THROW ; état actuel : UB identique.
- **RED-3 mismatch set (décision à trancher par T1)** : `setValue<int>` puis `setValue<float>` → aujourd'hui NO_THROW. Contrat cible à définir : « freeze du type au premier set » ou « replace autorisé ». Option la plus petite cohérente : garder setValue replace-silencieux, resserrer getValue (les deux exceptions TYPE_MISMATCH actuelles réorientées sur le chemin réel). Le RED-3 est conditionnel à cette décision ; ne pas écrire le test avant.
- **Compile-time** : garder les static_asserts existants verts (`UniformContractTests.cpp:27-30`) ; ajouter un `static_assert(!std::is_nothrow_invocable<...>)` sur getValUe pour interdire un ajout ultérieur de noexcept faux ; conserver les rejets du concept ValidUniformApi comme preuve négative existante.
- **Attention MSVC Debug x64** : le déréférencement nullptr plante le processus/la suite ; T1 prévoit isoler les RED crash dans les tests où ce comportement est acceptable, ou des assertions try/catch propres post-correction uniquement.

## E. Observations annexes (INCONCLUSIVE, hors périmètre strict de P1, à reclasser)

- **A1** : `opengl UniformContext.hpp:55-57` — m_uniformId/m_size/m_type sans initialiseurs → valeurs indéterminées si lues avant setters (GL réel).
- **A2** : `UniformReader.hpp:49` — la variable size de glGetActiveUniform est récupérée puis jamais transmise ; `setSize(size)` absent (l.54-57). `setter.getGLSize()` (Setter.hpp:86+, 12 sites) transmet alors une valeur indéterminée à `glUniform*fv/*dv` — suspect réel, cœur de P3. À faire figurer dans le plan RED de P3.
- **A3** : `PassContext.hpp:50-53` addUniform accepte un shared_ptr null sans vérifier ; après un éventuel rename en ownership unique, le null-check est peut-être impossible à atteindre — trancher à T1.

## F. Limites

- Statique + clause standard, aucune exécution (interdit P1) : les UB restent INCONCLUSIVE jusqu'au RED exécuté.
- Greps effectués sur include/tests/component-tests avec keyword exact — pas de couche mock LSP ; un fork T1 devra re-grep les mêmes motifs pour revérifier les sites.
- Interdits respectés : aucune source modifiée, aucun build/GPU ; note rendue au lead.
