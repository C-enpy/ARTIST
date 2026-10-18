# A2 — oracles renforcés, entrée de Q2 et PM2

Restitution engine-architect / pool-3, tâche task_mvdvuv25_0009_lutmr8,
fresh, lecture seule, C65/R66 PM/LLM. Résultat natif reçu, relu et persisté par
le lead ; aucun code/build/GPU exécuté par A2. Base 4a6276a ; pas de delta moteur
depuis 4a9b722. Utilisateur : « Renforcer avant l'auteur », limites DH2–DH4 de
Q refusées. Le présent contrat d'oracles est soumis à Q2, pas un GREEN.

ADR inchangés :0003/5 Accepted/partial,0008 Windows Debug seulement,
0001/2/4/6/7 Proposed. Types/ownership/freeze/TYPE_MISMATCH du contrat A conservés.

## Rectification documentaire vérifiée par le lead

Source primaire téléchargée sans modification sous out/attribute-gl/ :
https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.3.30.pdf
GLSL3.30, révision6 du 11-Mar-2010, §4.3.4, page imprimée29 (PDF35).
Courte citation : « Vertex shader inputs can also form arrays of these types,
but not structures. » Exemple natif de la spécification : in vec2 texCoord[4].
§4.3.8.1, page35 : un tableau d'inputs reçoit des locations consécutives.

La proposition Q initiale qui annonçait un échec de compilation des inputs
tableaux en GLSL330 est FAUSSE et ne doit pas être exécutée comme oracle.
La lecture du PDF vérifie le texte, pas le comportement du driver installé.
Le Reader doit toujours refuser l'utilisation des arrays, pas leur description.

## Classes de preuves strictement séparées

COMPILE_TIME : traits/layouts/is_same et famille fermée.
NATIVE_STATE : appels driver et requêtes d'état sur vrai programme lié.
NATIVE_TF_INTERLEAVED / SEPARATE : valeurs réellement consommées/capturées typées.
NATIVE_READER : réflexion réelle et chemin moteur Reader.
NATIVE_API_PROBE : limites/profil et contrôle volontaire bufSize API réduit.
COMPILE_REFUSAL / LINK_REFUSAL : erreurs shader/programme, jamais RED Setter.
CPU_FAULT_INJECTION : métadonnées impossibles/anormales injectées, nom explicite
FaultInjection, jamais présentées comme sorties natives du driver.

Les gardes de production restent exigées même lorsqu'une erreur n'est pas
naturellement produite par un driver conforme. Les injections ne rendent pas
une case native COVERED. Un critère natif impossible reste une décision explicite.

## D-TF : 12 sorties typées exactes

Un sommet, 12 in aux locations0..11, 12 out identité du même type, sans cast.
gl_Position = vec4(0), non capturé. Fragment minimal sans in entier.
Varyings de transform feedback configurés avant link du programme de fixture :
Pass::load n'expose pas ce crochet, aucune refonte de l'API production pour cela.
Sélection du programme avant draw ; EnableVertexAttribArray appartient à la fixture.
12 VBO vivants ; leurre lié entre bind et set pour chacun.

Interleaved : 30 composantes de4 octets =120 octets. Offset et payload :
- float @0 :0.5 ; vec2 @4 :(16777216,16777218) ;
- vec3 @12 :(-1,0.25,0.75) ; vec4 @24 :(1,-2,0.5,16777216) ;
- int @40 :16777217 ; ivec2 @44 :(INT_MIN,INT_MAX) ;
- ivec3 @52 :(-1,16777217,16777216) ;
- ivec4 @64 :(INT_MIN,-16777217,16777217,INT_MAX) ;
- uint @80 :UINT_MAX ; uvec2 @84 :(0,16777217) ;
- uvec3 @92 :(1,16777216,16777217) ;
- uvec4 @104 :(UINT_MAX,16777217,2,2147483648u).

vec3 contigu12, pas de padding std140. Vecteurs jamais réinterprétés par une
struct C++ agrégée au padding implicite. Comparaison bitwise des entiers,
exacte des flottants représentables. Sentinelle0xA5 à partir de l'octet120.
GL_RASTERIZER_DISCARD, DrawArrays POINTS un sommet, query PRIMITIVES_WRITTEN=1.
glGetBufferSubData après EndTransformFeedback, pas de glFinish nécessaire.
Mode separate complémentaire : float,int,uint,vec4 ci-dessus, un buffer chacun,
4 attributs maximum et <=4 composantes chacun (minimums GL3.3).
Pas de gl_SkipComponents, gl_NextBuffer ni de qualifier xfb plus récent.

Avant BeginTransformFeedback, vérifier INTEGER et TYPE corrects ; si faux,
échouer sans exécuter un draw avec interface entière incompatible/indéfinie.
INT_MIN seul ne discrimine pas une conversion float32 ; 2^24+1 oui. INT_MAX et
UINT_MAX après TF identité apportent des cas extrêmes, pas une couverture de
tous les entiers. Distinguer GL_INT/GL_UNSIGNED_INT : mêmes bits possibles.

## D-ARR : tableaux natifs légaux mais naming à observer

Sondes GLSL330 core utilisant in vec3 qaArray[1] et qaArray2[2], tous éléments
actifs. Attendu : compilation/link réussis. Enregistrer nom exact,length,size,
type,location. Le flag size1 n'est COVERED natif que si le driver expose exactement
qaArray[0]/size1/GL_FLOAT_VEC3 ; ne pas inventer un suffixe si nom nu.
La référence API glGetActiveAttrib3.3 ne garantit pas ici le spelling attendu.
Échec compile ou autre spelling : arrêter et porter le trou à décision humaine,
pas remplacer l'oracle natif par un mock. Pas de support arrays dans Setter.
foo[1],foo[00],foo[0]bar sont des injections CPU du parseur de suffixe.

## D-LOC/D-DUP/D-TRUNC/D-SIZE : atteignabilité et contrôle négatif

- Attribut utilisateur actif d'un programme lié valide : location non négative.
  Énumérer les built-ins si driver les expose (candidat gl_VertexID) et relever
  locations ; sans négatif naturel, INVALID_LOCATION reste injection CPU.
- Noms actifs uniques sur link valide. Deux in homonymes => refus compilation ;
  deux locations0 actives => refus link. DUPLICATE_NAME => injection CPU.
- Reader conforme avec bufSize MAX_LENGTH inclut NUL et ne tronque pas. Contrôle
  API séparé bufSize4 démontre une troncature API, pas un throw production Reader.
  Length trop grande, NUL absent/interne ou incohérent => injection CPU.
- Size active>=1 ; size<=0 et MAX_LENGTH<=0 avec count positif => injections CPU.

D-NAME : identifier naturel de300 lettres, puis256 si nécessaire ; le nom256+NUL
dépasse déjà char[256] actuel. Si300 lie, oracle exact300 obligatoire, MAX_LENGTH>=301.
Si seul256 lie, il devient l'oracle ; si aucun ne lie, INCONCLUSIVE, pas un mock
prétendu couvrant. GLSL330 §3.7 ne spécifie pas un minimum1024 de caractères.

Refus GLSL natifs distincts : dvec3 sans extension, struct,bool ; garder logs,
ne pas appeler le Reader sur programme qui n'a pas lié. Mat4 légal publié
GL_FLOAT_MAT4/size1 puis Setter refuse. aPos/location0/GL_FLOAT_VEC3 inchangé.

## Setup et RED définis

Relever vendor,renderer,version,GLSL,profil core. Une seule purge de l'erreur
GLEW connue puis GL_NO_ERROR ; ne pas purger les erreurs du test.
Minimums : VERTEX_ATTRIBS16, VERTEX_OUTPUT_COMPONENTS64,
TF_INTERLEAVED_COMPONENTS64, TF_SEPARATE_ATTRIBS4, TF_SEPARATE_COMPONENTS4.
Relever MAX_VARYING_COMPONENTS sans retirer une sortie pour contourner une limite.
Interleaved30, gl_Position4 ->34 sorties : sous minimum64. Link12 échoué => arrêt,
pas un sous-ensemble annoncé comme les12. TearDown dérivé avant glfwTerminate :
VAO, leurre, TF buffers, programme, query ; aucun delete ajouté en production.
Sources légales sous test-datas/shaders/qa-attribute ; refus en littéraux séparés.

Mocks setup : IPointer présent ; glGetProgramiv distingue chaque pname.
Sans mock compilable/contexte, aucune preuve RED comportementale.
RED attendus sur ancien code :
- leurre écrasé car pas de bind explicite Setter ;
- vecN -> enum GLSL passé à Pointer provoque erreur/configuration fausse ;
- int/uint -> INTEGER false, aucune TF lancée ;
- mock : Pointer observé au lieu d'IPointer, une fois symbole de mock disponible ;
- nom naturel >255 tronqué si préflight shader lie ;
- drapeau false alors que nom [0] réel a été observé ;
- pas d'exception pour injections d'erreurs contrat.
Float scalaire coïncide déjà avec Pointer : RED causal via leurre, pas faux échec.

## Budget proposé et réévaluation requise

A2 propose140 : setup36, planifié80, buffer24 ; après setup104, ratio76.9/23.1.
Setup limites6/sondes10/noms6/mocks+RAII14.
Planifié layouts4,exclus3,Reader8,nom6,array8,mat4 4,Setter10,erreurs8,leurre6,
TF interleaved14,separate8,injections6,non-régression5.
Le lead relève un écart arithmétique : ces lignes planifiées totalisent90, pas80.
Q2/PM2 doivent réconcilier le total sans couper TF obligatoire ni double compter
setup/planifié ; pas d'adoption silencieuse d'un budget incohérent.

Estimation A2 D brute78 risque64 ; avis, pas score publié. Le précédent score
D validé était78/76. Le renforcement ne justifie pas automatiquement un risque
plus faible. PM2 réévalue complexité/risque et explique toute variation,
plancher développement70. Grille de modèles approuvée inchangée.

## Barrière

Q2 doit amender le plan, retirer l'erreur GLSL et la capture vec4 à casts,
garder DH2–DH4 refusés, classer chaque preuve, fixer les décisions encore ouvertes,
et rendre un registre bidirectionnel/cas discriminants/budget cohérent.
Le lead persiste/commit avant worktree auteur. Aucune tâche D publiée.
Hors lot : lifetime VBO, RAII globale, ASan, perf, Release, autres backends/mesh.
