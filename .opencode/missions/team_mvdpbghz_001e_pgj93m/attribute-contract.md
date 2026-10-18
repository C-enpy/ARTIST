# Attribute-GL — contrat A transmis à Q

Source : restitution native `pool-1`, tâche `task_mvdpvwee_001f_pihzux`,
engine-architect, contexte fresh, lecture seule. C58/R62, PM/LLM.
HEAD inspecté : 4a9b722a8f1b64470172686aee961221fa6384c0, diff documentaire
préparatoire partagé ; aucun code/build/test/GPU exécuté par A.
Le lead persiste cette restitution ; elle n'est pas un plan QA ni une preuve GREEN.

## Statuts et portée

AGENTS.md et engineering-principles applicables. ADR-0003/0005 Accepted/partial,
0008 vérifié Windows Debug seulement ; 0001/0002/0004/0006/0007 Proposed.
Pas d'ADR-0009 ou adoption globale d'ADR-0004. Chaîne verticale Attribute
réfléchi seulement, lifetime VBO/RAII exhaustive/ASan/perf/Release/autres OS et
backends hors périmètre. `.grepai/config.yaml` hors périmètre.

## 12 spécialisations canoniques

GLM cci.20230113 ; defaultp = highp = packed_highp dans la configuration inspectée.
Liste fermée :
- float ; glm::vec2 ; glm::vec3 ; glm::vec4 ;
- std::int32_t ; glm::ivec2 ; glm::ivec3 ; glm::ivec4 ;
- std::uint32_t ; glm::uvec2 ; glm::uvec3 ; glm::uvec4.

Pas de spécialisation supplémentaire pour alias identiques. Vérifier is_same :
float/GLfloat ; int32_t/glm::int32/int/GLint ; uint32_t/glm::uint32/unsigned/GLuint ;
vecN/highp_vecN, ivecN/highp_ivecN, uvecN/highp_uvecN ; i32vecN/u32vecN seulement
s'ils sont identiques à ivecN/uvecN. Divergence => arrêt et décision, pas une 13e forme.

Hors famille même si sizeof identique : qualifiers mediump/lowp, vec1/ivec1/uvec1,
i8/i16/u8/u16, doubles, matrices, aligned, tableaux C, std::array/vector,
pointeurs et structs. Primaire sans on(). glm.hpp n'importe pas vec1.

Layout vérifié compile-time : scalaire 4/4 (sizeof/alignof), vec2 8/4,
vec3 12/4, vec4 16/4 ; offset composante i = i*4, aucun padding, stride sizeof(T).
Un flag alignant vec3 à 16 doit faire échouer la preuve, pas être accepté.

Correspondances exactes, avec cardinalité 1 et isGLArray false :
float/GL_FLOAT, vec2/GL_FLOAT_VEC2, vec3/GL_FLOAT_VEC3, vec4/GL_FLOAT_VEC4 ;
int32/GL_INT, ivec2/GL_INT_VEC2, ivec3/GL_INT_VEC3, ivec4/GL_INT_VEC4 ;
uint32/GL_UNSIGNED_INT, uvec2/GL_UNSIGNED_INT_VEC2,
uvec3/GL_UNSIGNED_INT_VEC3, uvec4/GL_UNSIGNED_INT_VEC4.

float => glVertexAttribPointer(location, composantes, GL_FLOAT, GL_FALSE,
sizeof(T), nullptr). Int32/uint32 => glVertexAttribIPointer avec GL_INT ou
GL_UNSIGNED_INT. L'enum GLSL brut n'est jamais le scalaire mémoire ; cardinalité
n'est pas composantes. Un seul T, pas un mesh ni dispatch par sommet.

## Réflexion et état initial

OpenGLPassAttributeReader utilise GL_ACTIVE_ATTRIBUTE_MAX_LENGTH pour le buffer.
count>0 et maxLength<=0, longueur invalide, NUL manquant/interne =>
ERROR::PASS::ATTRIBUTE::INVALID_NAME ; aucun nom tronqué publié.
Doublon => ERROR::PASS::ATTRIBUTE::DUPLICATE_NAME.
size<=0 => ERROR::PASS::ATTRIBUTE::INVALID_SIZE.
Location conservée GLint et <0 => ERROR::PASS::ATTRIBUTE::INVALID_LOCATION
avant conversion ; location 0 valide. INVALID_PROGRAM_ID existant inchangé.

getGLType() reste l'enum GLSL brut ; getGLSize() reste la cardinalité.
setGLArray(bool), isGLArray() const, défaut false ; détection par suffixe exact
"[0]", pas uniquement size>1. Un enum non supporté reste décrit sans rejeter le
Pass. L'architecte propose le cas réfléchi a[0]/size1 comme oracle négatif.
Le lead exige que Q vérifie la légalité native GLSL 3.3 de toute fixture négative
(arrays, doubles, bools, etc.) : mock valide ≠ shader d'entrée vertex légal.
Un shader invalide n'est pas un RED Setter. Si une preuve native est impossible,
nommer le gap et demander arbitrage plutôt qu'affaiblir l'oracle silencieusement.

Initial : buffer0, attribute id0, type0, size0, usage0, array false.
Location0 n'est pas une sentinelle invalide.

## Setter et erreurs ordonnées

Attribute::set reste setValue puis Setter, sinon ERROR::ATTRIBUTE::UNSUPPORTED_TYPE.
set<T> reste bien formé : fermeture du trait/absence de on au compile-time ;
rejet runtime par exception via API existante. Payload CPU déjà stocké conservé
au refus ; changement de T ultérieur soumis au freeze TYPE_MISMATCH.

Avant tout appel GL, ordre des validations Setter::on :
1. buffer0 => ERROR::ATTRIBUTE::SET::BUFFER_ID_NOT_SET ;
2. valeur absente/nulle => ERROR::ATTRIBUTE::SET::NULL_VALUE ;
3. array true ou size!=1 =>
   ERROR::ATTRIBUTE::SET::ARRAY_NOT_SUPPORTED size={size} ;
4. enum différent => ERROR::ATTRIBUTE::SET::SHADER_TYPE_MISMATCH expected={} actual={}
   avec enums unsigned ;
5. usage hors STREAM/STATIC/DYNAMIC => DRAWING_MODE_NOT_SUPPORTED existant.

Ensuite seulement glBindBuffer(GL_ARRAY_BUFFER, buffer de l'attribut),
glBufferData, puis Pointer/IPointer. Après succès GL_ARRAY_BUFFER cible ce VBO,
sans restauration. VBO leurre doit conserver taille et octets.
Préconditions : contexte courant approprié, VAO valide lié, VBO vivant.
Binder et Unbinder inchangés ; pas de glDeleteBuffers en production.

Freeze et messages TYPE_MISMATCH inchangés : GET typeid(T) puis type stocké ;
SET typeid(shared_ptr demandé) puis type stocké. NULL_CONTEXT, FREED_CONTEXT,
NULL_ATTRIBUTE, UNBIND::BUFFER_ID_NOT_SET restent inchangés.
Ownership unique et emprunts F7 conservés.

## Binding times et oracles à Q

Compile-time : traits, layout, sélection Pointer/IPointer, HasComponent.
Pas de nouveau binding link-time C++, pas de cook. Réflexion runtime après link
GLSL ; état VAO/VBO/driver au set, pas par sommet.

- AttributeContractTests.cpp : 12 layouts/is_same ; négatifs mediump_vec3,
  dvec3, mat4, vec1 (extension limitée au TU). OpenGLAttributeFlow garde Setter<int>.
- SetterTests.cpp : mock glVertexAttribIPointer à ajouter ; vec3 réfléchi
  GL_FLOAT_VEC3/size1/arrayfalse -> bind cible puis Pointer(3, GL_FLOAT, false,12).
  Entiers : IPointer, jamais Pointer. Refus => zéro GL y compris BindBuffer.
  Buffer et null restent prioritaires. Ancien size3/GL_FLOAT n'est plus oracle.
- AttributeReaderTests.cpp : MAX_LENGTH réel (défaut mock 1 inadéquat), noms
  exacts, troncation rejetée sans clé, location -1 avant cast, array flag même
  size1, mat4 publié sans rejeter Pass.
- PassTest aPos/GL_FLOAT_VEC3/location0/size1/arrayfalse reste valide.
- Natif : 12 formes réellement exécutées, requêtes SIZE/TYPE/STRIDE et
  BUFFER_SIZE, float NORMALIZED false, entiers INTEGER true. Valeur consommée
  par shader sur vec3, un signé et un non signé ; autres formes au moins requêtes
  natives discriminantes, pas couverture par analogie. GL_NO_ERROR insuffisant.
  Observer buffer leurre. Fixtures nettoient VAO/VBO même après ASSERT.
- AttributeContextTests et AttributeOwnershipTests : non-régression inchangée.

Rapport A : aucune décision ouverte. Qualification lead : contrat transmissible
à Q sous vérification des préconditions/oracles, pas promesse d'exécution native
des formes GLSL interdites. Aucun résultat 299/299 historique réutilisé comme PASS.
