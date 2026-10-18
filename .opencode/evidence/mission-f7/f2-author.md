# F2 — rendu auteur OpenGL, 2026-10-18

Auteur : `f2-opengl-author` (`gpu-engineer`), tâche
`task_mvdku07l_000r_kmgt7l`. Contrat et QA lus au commit `d24340f` avant code.
Cette sortie ne vaut **ni intégration ni clôture F2**. Revue indépendante fresh
et revalidation du candidat intégré : lead. Documentation `docs/` : ownership lead.
Code/tests/fixtures commités : `f51e16d709197a7820dc767060c83673f33a27ed` ;
tests exécutés avant commit sur les sept SHA256 ci-dessous, identiques au commit.
La note est commitée séparément (répertoire evidence ignoré, ajout forcé explicite).
Delta code `git diff --binary d24340f f51e16d`, SHA256
`451f51074c1f149396f19ee960d1a56033f9b8608487ccd363e8a9f8997b585e`.
Les sept blobs commités et fichiers disque ont été recoupés aux sept hashes testés.

## Identité et ressources

- Worktree exclusivement modifié :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-5caa4a18f43401c7#eiyn0g-f2-opengl-author`.
- G1 communiqué avant mutation : HEAD
  `d24340f7e129a03da2a7c74a25025526c9c3cd00`, arbre propre, delta vide,
  SHA256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
  Les deux cibles production sont identiques entre le pin QA `483360e` et `d24340f`.
- SHA256 initiaux : contexte
  `135bfa52c4ab85a16f12cb5fdf35770b548a085deab1e38dd88617f5d986c170` ; setter
  `dbd26d8c4b1b63cd411c173bae7ea1405c9f2b5476363ea196c53bada89c609c`.
- Baux build/cache/GL exclusifs reçus ; un job, tests GL sérialisés, campagne
  annoncée au lead **après GREEN CPU et avant le premier test natif**.
  `RESOURCE_LOCK artist_opengl` hérité du CMake composants, inchangé.
- Sortie courte autorisée :
  `C:/Users/djden/source/repos/ARTIST/out/mission-f7/f2-build` ; `-S` explicitement
  worktree auteur. Checkout principal non modifié, hors sortie/cache autorisés.
  Tooling/cache canoniques réutilisés, jamais copiés : `out/tooling`, `out/conan2`.
  Aucun alias/subst, aucune installation SDK/driver, aucun Docker créé.

## Cause et patch borné

`Uniform::set<T>` stocke un seul T. Les 30 fonctions vectorielles/matricielles
passaient pourtant la taille réfléchie : zéro par défaut dans `make_unique`, ou
N pour un tableau dont le stockage ne contient pas N valeurs. Les quatre fonctions
scalaires acceptaient également silencieusement la métadonnée tableau.

- `OpenGLUniformContext` : init déterministe id0/size1/type0. Aucun contexte ni
  programme/location valides ne sont garantis par ces valeurs.
- `Setter.hpp` : helper inline partagé, appelé dans **chacun des 34 on() avant GL**,
  rejet size!=1 par `TraceableException<runtime_error>`, motif stable
  `ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED`, diagnostic `size=N`.
- **30 count littéraux 1** ; aucune cardinalité réfléchie passée à glUniform.
- Stockage std::any avant dispatch conservé, **pas de rollback nouveau**.
  Reader/Pass/Attribute/ownership T1-1, concepts et UNSUPPORTED_TYPE inchangés.
  Aucun mock étendu, aucun changement d'ABI shader de production.

## RED → GREEN, attendu/observé

Tous les logs et JUnit complets sont dans `out/mission-f7/f2/` **du worktree**.
Ils sont ignorés Git : à sauvegarder par le lead avant destruction du worktree.
`run.ps1` conserve les commandes exactes ; `audit.ps1` produit `manifest.json`
(hashes sources/fixtures/logs/JUnit, tailles, dates UTC, inventaire par type/site,
recoupement découverte/exécution). Sorties CTest passées/échouées : 65536 octets.
Manifeste remis : SHA256
`40d30bb9165b0c69385aced5a6b23c3d1a22ad9815a770c8889a18c8c592079f`,
sept sources et 34 artefacts, hashes des 34 recoupés sans écart (~3Mo au total).

1. Nouveau fichier CPU sous mocks, avant production, et oracles historiques
   count0 encore intacts. `configure-before.log`, `build-red.log`,
   `discovery-red.log`, `identity-before.log` : découverte **293=248 unit+45 GL**,
   compilation fonctionnelle. `red-cpu.{log,xml}` : **248 exécutés, 22 FAIL, 0 skip** ;
   les 226 préexistants, dont les dix UniformTest historiques, restent verts.
   - A1 : vec3 attend count1, observé mock count0 + attente non satisfaite.
   - A2/A3/A4 : dix types mockés, tailles2 et0, attend throw typé + zéro GL ;
     observé absence de throw et appels interdits Times(0) (20 FAIL).
   - A5 : make_unique attend size1, observé size0 (1 FAIL). id/type y étaient déjà0.
   - Aucune lecture de champ indéterminé ni aucun appel driver OOB volontaire.
2. Après conservation de ces FAIL, les **six** anciens oracles count0 sont
   renforcés en count1, Times(1) inchangé. `build-red-oracle.log`,
   `red-oracle.{log,xml}` : avant production, **6 FAIL/10**, count0 observé contre1
   attendu ; les quatre scalaires restent verts. Pas de tolérance élargie.
3. Patch production minimal ; ajout du seul test direct-init **post-GREEN** pour
   ne jamais lire volontairement ses anciens champs indéterminés en RED.
   `build-green.log`, `green-cpu.{log,xml}` : **249/249 PASS, 0 skip**.
4. Nouveaux composant et fixtures ; **re-glob explicite** par configure, puis build :
   `configure-final.log`, `build-final.log`. `discovery-final.log` :
   **297=249 unit+48 GL**, non nul, trois nouveaux composants effectivement découverts.
   `green-f2-native.{log,xml}` : **3/3 PASS, 0 skip**, aucun RED dangereux sur driver.
5. Non-régression : `final-cpu.{log,xml}` **249/249 PASS**, puis
   `final-native.{log,xml}` **48/48 PASS**, zéro skip/échec.
6. G2 de tous les sept fichiers exécutables/fixtures avant et après relance :
   `g2-before-final.log`, `g2-after-final.log`. Ratification sur sources inchangées :
   `ratified-cpu.{log,xml}` **249/249 PASS** (4.24s),
   `ratified-native.{log,xml}` **48/48 PASS** (19.95s), zéro skip/échec.
   Durées de campagne seulement, **pas un benchmark**. Aucun refactoring additionnel
   nécessaire ; le diff source est resté identique entre les campagnes natives.

## Commandes et environnement

Runner local reproductible (fichier brut fourni dans les preuves) :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File out/mission-f7/f2/run.ps1 -Action configure
powershell -NoProfile -ExecutionPolicy Bypass -File out/mission-f7/f2/run.ps1 -Action build
powershell -NoProfile -ExecutionPolicy Bypass -File out/mission-f7/f2/run.ps1 -Action discover
powershell -NoProfile -ExecutionPolicy Bypass -File out/mission-f7/f2/run.ps1 -Action test -Name ratified-cpu -Label unit
powershell -NoProfile -ExecutionPolicy Bypass -File out/mission-f7/f2/run.ps1 -Action test -Name ratified-native -Label opengl
powershell -NoProfile -ExecutionPolicy Bypass -File out/mission-f7/f2/audit.ps1
```

Le runner charge `out/tooling/load-msvc.cmd` (vcvars x64,14.44) **dans le même
processus** avant Conan/CMake/CTest. Conan install précède configure, profils
versionnés host/build `conan/profiles/windows-msvc`, Debug ; sortie `-of f2-build`.
CMake `-S worktree -B f2-build -G Ninja`, toolchain Conan de cette sortie,
BUILD_TESTING/MOCK_GL ON, ASAN/COVERAGE OFF, `--parallel 1` ; CTest `-j1 -L label
--no-tests=error --output-on-failure --output-junit ... --test-output-size-passed
65536 --test-output-size-failed 65536`. Aucun preset au binaryDir impropre.

Observés : Windows11 Professionnel x64 `10.0.26300`, MSVC `19.44.35221.0`,
toolset `14.44.35207`, SDK `10.0.26100.0`, CMake `3.31.7`, Conan `2.12.2`,
Ninja `1.13.0.git.kitware.jobserver-pipe-1`.

## Preuves natives, limites distinctes

Fixture GLFW/GLEW : demandé3.3 core, **obtenu3.3.0 NVIDIA617.42**, core mask1,
GLSL3.30, NVIDIA RTX3090 ; nom GPU non utilisé comme preuve de capacité.
`GL_MAX_VERTEX_UNIFORM_COMPONENTS=4096`, share=null (contexte non partagé),
contexte courant et thread consignés dans chaque JUnit. Owners détruits avant
le contexte ; programme courant vérifié puis glUseProgram(0) avant destruction,
y compris sur assertion via garde locale. Pas de glFinish/barrière ajoutée.

- S4 nominal **PASS/executed-native** : GLfloat/vec3/mat3, metadata size1,
  upload via API publique et readback exact des 1/3/9 composantes.
- S4 tableau **PASS/executed-native** : `f2Array[0]`, type GL_FLOAT_VEC3,
  size2, locations[0] et[1] actives. Référence fixe `(2,3,5)/(7,11,13)` avant rejet,
  seed valide de six floats ; readback **séparé à chaque location** avant et après
  exception typée size2. Les deux éléments sont inchangés ; ce n'est pas seulement
  une vérification de glGetError ni du second élément. Pas de promesse std::any.
- fp64 représentatif **PASS/executed-native** : extension
  `GL_ARB_gpu_shader_fp64=1`, entrées glUniform1d/glUniform3dv/glUniformMatrix2dv/
  glGetUniformdv non nulles observées ; fixture GLSL330+extension requise compilée/
  liée. GLdouble/dvec3/dmat2 uploadés et relus exactement (1/3/4 composantes).
  Le test saute explicitement/INCONCLUSIVE si gate absent sur un autre système.
- KHR_debug annoncé par GLEW=1 ; aucun callback/capture utilisé, donc **pas** de
  preuve par debug output. glGetError=GL_NO_ERROR aux points vérifiés ; aucun log
  d'échec compile/link ni message driver pertinent observé dans ces trois cas.
- **INCONCLUSIVE driver pour les 28 autres spécialisations F2** : ne pas étendre
  la preuve de ces six types à toute la famille. Matrices non carrées, ivec/uvec,
  autres tailles float/double : inventaire/revue statique, pas preuve native F2.
- Image/draw/performance/Release/autres OS/CI/ASan : hors critères, non exécutés.
  Aucun gain mesuré. Impact CPU **estimé non mesuré** : une comparaison de taille
  par set nominal ; impact GPU nominal attendu inchangé (toujours un élément),
  rejet sans soumission GL pour tailles invalides.

## Inventaire et obligations

`inventory-before.log`, `inventory-green.log`, `inventory-final.log`, puis `manifest.json` :
**34 spécialisations / 34 appels / 34 gardes avant appel / 30 count1 / 0 oubli**.
Les deux occurrences finales `getGLSize()` sont exclusivement les lectures du
helper (garde/diagnostic), pas des counts GL.

- Scalaires4 : GLfloat, GLint, GLuint, GLdouble.
- Vecteurs12 : vec2/3/4, dvec2/3/4, ivec2/3/4, uvec2/3/4.
- Matrices18 : mat2/3/4, mat2x3/3x2/2x4/4x2/3x4/4x3 et les neuf dmat homologues.
- S1 **PASS/executed-mock** : quatre scalaires + vec2/3/4 + mat2/3/4,
  size0/2 rejetées, motif/size typés + zéro appel des dix entrées mockées.
  Les 24 autres gardes : **PASS/static-presence seulement**.
- S2 **PASS/executed-mock + static-presence** : six counts historiques renforcés,
  A1 contenu vec3 vérifié ; revue exhaustive30 sites distincte des exécutions.
- S3 **PASS/executed-CPU** : make_unique puis direct-init post-GREEN, id0/size1/type0.
- S4 : preuves natives ci-dessus ; Reader inchangé, ses tests existants verts.
- S5/I2 **PASS/static-presence** : wrapper et ordre stockage/dispatch inchangés ;
  garde dans on avant GL. Rollback std::any explicitement non promis.
- I1 **PASS/non-régression + static-presence** : concepts et type non supporté
  inchangés, suite CPU fraîche. I3 **PASS/executed-mock/native** : motif et taille,
  pas d'égalité rigide sur tout le diagnostic/stacktrace.
- Vérification indépendante et candidat intégré : **INCONCLUSIVE**, à organiser
  par le lead ; cette auto-vérification auteur n'est jamais indépendante.

## SHA256 du candidat testé (sept fichiers inchangés entre runs)

- `opengl/context/UniformContext.hpp` : `380cdaab465d6c11495fc3d5ada5e5c0578afd2f0f4d76b76bf7eb310703a6cb`
- `opengl/pipeline/component/uniform/Setter.hpp` : `26ad921f403689f5f8ffff1cab61a2de74baf5c9a58842000ec64e4e7d8526dd`
- `tests/src/graphic/pipeline/UniformTests.cpp` : `b528cd7f8087a85c4970a5c4da86b76d3fc1a93936d048bae8d4eaf5a58c2246`
- `tests/src/graphic/pipeline/UniformCardinalityTests.cpp` : `6b82d168eef95786d5fbe4c6dc3e36608354b834da6cfa072e0ebbacf5bebc48`
- `component-tests/src/graphic/shader/UniformCardinalityComponentTests.cpp` : `e80b826c0a33479c05fa11b844178804c341646428fe25ebd13aef9f77f6924a`
- `test-datas/shaders/vertex/good/f2-single-values.vert` : `9edc5dbaad84917ccc2fcc0fd61113b30c3e8f8413dc94c9f51fad97b647c86e`
- `test-datas/shaders/vertex/good/f2-fp64-single-values.vert` : `4dc08c977835e2cfcf57febaf1902eb9f03f37b5648ffdbab60b6132998e91a7`

Les cinq premiers chemins sont sous `ARTIST/` (`include/graphic/` pour les deux
premiers). Baux build/cache/GL **rendus en fin de tranche auteur** ; aucune
commande supplémentaire après remise. Ressources Docker : aucune, donc aucun
registre de création requis. Pas de modification/adoption ADR : 0003/0005 partial,
0004/0006 Proposed, 0008 Windows Debug seulement.
