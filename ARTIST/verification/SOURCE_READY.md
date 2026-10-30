# Scenarios uniforms — remise consolidee SOURCE_READY, NON_EXECUTE

## Attribution de la preuve Python deja acquise

Le runner lead rapporte sur `82e6cc849de2fee9555c745863d9e0bf9e226e9b` :
**29/29 tests**, 0 failure/error/skip, tests inchanges et hashs stables.
Journaux annonces :
`out/verification-synthetic/scenario-hardware-green-2026-10-30T00-10-40-796Z/`.
Ce GREEN suit les RED de disponibilite/semantique puis GREEN du protocole initial
et de la cible hardware ; attribution dans les remises historiques voisines.
L'auteur n'a execute aucun test. Les 29 tests et le protocole Python ne changent
pas dans cette remise native. Deux caracterisations nominales espace/NBSP ont
ensuite ete ajoutees (SOURCE_FIX_READY.md), sans RED fabrique ni changement
fonctionnel Python : non-regression courante attendue **31/31**, a executer par
le lead. Ce GREEN historique ne prouve ni C++, ni descriptions,
manifest/link, Unicode natif ou raccordement. Non-regression finale au lead.

## Fichiers et frontieres

- `WorkloadProtocol.hpp` : CLI ferme, from_chars, bornes avant allocations,
  produit verifie par division, JSON v1, creation exclusive C++23 noreplace,
  echappement complet des controles/quotes/backslashes et validation UTF-8.
  Pas de lecteur JSON C++, std::quoted ou dependance supplementaire.
- `CpuUniformWorkload.cpp` : UniformContext::setValue/getValue<float> existant,
  controle independant 0/.5/0 puis chaque update de la population demandee.
  Aucune initialisation GL. Le selecteur CLI pass-name n'affirme pas un Pass
  ni un upload natif : ce scenario ne prouve que le miroir CPU.
  Le parametre template est instancie par CpuMirrorDomain, tag local sans backend,
  correctif du finding independant pool-4. Re-audit independant transmis par le
  lead : `result_mvu7w2bo_000y_6ww1e3`, ancre sur les objets Git de
  `72a382e1959fab80e6d83952719bf029352c6f7e` : finding CPU **leve statiquement**,
  sans compilation/execution ni reception globale. Tag inchange depuis cet avis.
  Cela n'accepte pas la nouvelle entree Unicode ou qualification hardware ;
  reviewdiff de la source consolidee `2d212c403e10cead02a959368c054ed34a5e43dc`
  et non-regression lead restent requises.
- `OpenGLFloatWorkload.cpp` : contexte GLFW non partage, GLEW, 3.3 core obtenu
  verifie ; deux routes de production, ressources et owners locaux ; aucun
  relink natif externe, compute, SSBO, debug output ou appel non garanti 3.3.
- `CMakeLists.txt` : deux cibles optionnelles, rien execute a la construction,
  aucune inscription CTest ni modification des parents/presets/tests existants.
  Windows/MSVC uniquement : /utf-8 et incorporation locale du manifeste
  `UniformWorkload.manifest`, sans /ENTRY force ni launcher ou changement OS.
- `scripts/verification/scenarios/descriptions.py` : descriptions et noms CTest
  statiques, zero import/sonde/initialisation ; candidat de raccordement, pas un
  second catalogue ni un schema core recopie.

## Hardware — precision ratifiee, implementation SOURCE_ONLY

Le lead a ratifie hardware-expectation-v1.md (commit canonique c7c3d143), puis
autorise le natif apres GREEN 29/29. Le parseur natif accepte la paire optionnelle
`--expected-gl-vendor` / `--expected-gl-renderer`, complete, hardware uniquement,
non vide, sans C0/DEL/C1/NUL, <=4096 **scalaires Unicode** par chaine. Ni octets
UTF-8 ni unites UTF-16 ne servent de compteur. Doubles/flags inconnus refuses.
Le correctif borne du re-audit `result_mvu8p34c_0000_6de7kv` exclut les deux
seules options cibles du filtre generique nonblanc : espace/NBSP non vides
restent exacts, sans strip ; empty/controle restent refuses code30. Les autres
options sont inchangees. Cause et chemins source dans SOURCE_FIX_READY.md ;
re-audit du correctif encore requis, pas de preuve native executee.

Avant programme/upload/draw, le natif collecte vendor/renderer/version/GLSL,
version/profil/courant du contexte, thread/share group ; il exige >=3.3 core,
le match exact declare/observe et refuse les logiciels connus (GDI Generic inclus).
Succes du controle : qualification MATCHED_EXPECTATION. Absence, discordance,
logiciel ou precondition indisponible : **code20/INCONCLUSIVE/UNVERIFIED**.
Windows software est refuse explicitement avant creation de contexte ; Linux
software accepte uniquement un renderer llvmpipe observe, explicitement demande.
Aucun fallback, installation, selection Xvfb ou variable forçant logiciel ajoutee.

Environment conserve cible declaree et observations ; parameters garde exactement
ses quatre champs, identite/plan_sha256 inchanges. La cible doit etre liee au
hash/receipts par le core ; pas de schema core copie ici. Ce match attribue la
cible dans un environnement maitrise, pas une preuve d'acceleration physique
universelle ou de features avancees deduites du nom.

## Unicode Windows — contrat local ratifie, source non executee

Les seules deux cibles Windows/MSVC utilisent wmain et convertissent UTF-16 vers
UTF-8 strict avec proprietaires locaux vivants jusqu'au retour. Surrogates
isoles refuses ; paire valide compte un scalaire. Linux main conserve les bytes
UTF-8 valides, sans changement de locale. ResultPath passe par std::u8string puis
filesystem::path natif ; racine source est un literal u8, paths shader emis en
bytes UTF-8. Ni remplacement silencieux ni normalisation du match exact.

Le Reader Classique moteur reste narrow et inchangé. Le manifeste local demande
activeCodePage=UTF-8, comportement documente Windows1903+. Le controle effectif
est **GetACP==CP_UTF8 et AreFileApisANSI**, apres creation/contexte et avant Reader
GL ; precondition manquante => code20/UNVERIFIED, pas version devinee, fallback
ASCII ou modification des reglages OS. Aucun point d'entree /ENTRY force.
Chemins/manifeste/oracles de lecture dans NATIVE_UNICODE_AND_TARGET_READY.md.
Support CRT/link/Unicode **NON_EXECUTE**, a qualifier apres integration.

## Correctness et ownership — examen statique uniquement

Les shaders existants sont resolus depuis ARTIST_VERIFICATION_SOURCE_ROOT,
jamais depuis cwd :

- `test-datas/shaders/vertex/good/f2-single-values.vert`
- `test-datas/shaders/fragment/good/minimal.frag`

Initialisation hors mesure : matrice identite, vecteur/array nuls, facteur
fragment entier 1. FBO RGBA8 lineaire non multisample, grille 1x1/4x4/16x16,
cellules 32x32, VAO/VBO triangle (-.75,-.75),(.75,-.75),(0,.75). Etat GL fixe,
pack buffer nul et pack alignment 1. Un clear du FBO par phase, jamais par cellule.

Le controle 0/.5/0 est toujours execute, meme repetitions=1 : pour chaque cellule,
miroir CPU et glGetUniformfv ; image lue apres tous les draws de la phase et
quatre points fixes verifies dans **chaque** cellule. Points locaux, origine bas
gauche : (8,8), (24,24), (18,16), (2,2). Valeur 0 attend blanc/noir/blanc/noir ;
.5 attend noir/blanc/blanc/noir. Canaux 255/0, alpha 255. Images precedentes non
effacees pendant la construction de la grille. Pixels du controle conserves
dans le JSON, premiere divergence dans stderr ; aucun faux oracle A/B seul.

Un controle natif calibre puis observe glUniform1f et glGetUniformLocation via
pointeurs GLEW pass-through, sur le **meme kernel de phase** : 3*batch uploads,
3*batch appels draw et zero nouvelles resolutions pour la route selectionnee.
Le draw est compte au site d'appel (glDrawArrays GL1.1 n'a pas de faux hook
GLEW Windows), sa consommation est verifiee par l'oracle pixel independant.
Les pointeurs sont restaures avant toute mesure. Les deux routes sont des
invocations distinctes ; l'integrateur peut comparer leurs pixels de controle
en complement des deux oracles independants. Aucune egalite interdriver globale
ou nouvelle instrumentation moteur n'est promise.

Avis shader independant rapporte par le lead : pool-5,
`result_mvu7m96w_000s_r34ufc`, candidat `cbf39ce183c61cd62c2b3a8a48ab220e5b4214af`.
OpenGLFloatWorkload.cpp SHA-256
`2db9c10b7393ae100bb47ae5f01ffcb2d939c382d583ebd4446782d2ec133f80` pour cette
revision historique, recoupe avant le present patch. Aucun defaut bloquant demontre sur
geometrie/viewports/pixels/shaders par cette lecture. Sommets locaux 0 :
(4,4),(28,4),(16,28) ; .5 : (12,12),(36,12),(24,36), marge minimale annoncee
environ 2.01 pixels. Cela ne constitue ni image obtenue ni PASS graphique.

**Deux limites non bloquantes de l'oracle doivent accompagner les observations** :

- Toutes les cellules d'une phase sont identiques. Une permutation bijective
  de ces cellules reste invisible ; l'oracle detecte les omissions/duplications
  qui changent les pixels attendus, pas toute attribution a une mauvaise cellule.
- La fenetre chronometree peut contenir plusieurs phases, mais son readback
  ne verifie que la derniere. Le controle distinct verifie 0/.5/0 ; il ne prouve
  pas chaque image intermediaire de la population mesuree.

Aucun marqueur de cellule ou compteur supplementaire ajoute : cela depasserait
le besoin de livraison. Les formulations conservees dans observations.workload
du JSON natif sont maintenant respectivement
`cell_oracle_limit: identical-cell bijective permutations are invisible` et
`timed_image_oracle_limit: only final phase read back; separate 0/.5/0 control
does not prove every intermediate measured image`. Geometrie/viewport/oracles
et boucles chrono relus restent inchanges ; seules qualification/Unicode/entrees
et ces deux metadonnees evoluent dans le present diff, a revoir independamment.

Le contexte est le premier owner et le dernier detruit. Le FBO/VAO/VBO precede
dans la destruction le binding, puis Pass/shaders, puis contexte. Destruction et
dernier glGetError dans le contexte courant ; glUseProgram(0) avant retraite du
Pass. Les acces prepares restent dans la duree de vie du Pass, sans mutation
de metadonnees, retrait ou pointee revocable dereference apres retraite.

## Mesures candidates, jamais executees ici

Horloge steady_clock, temps **mural cote CPU**, pas temps CPU thread ni GPU.
Selection de route une fois avant le kernel ; nom/valeurs/vecteurs prepares hors
intervalles. Gardes de production actives. Preparations programme/acces separees
dans observations ; acces prepare absent pour pass-name est null, pas zero.
Chauffe 0/.5/0, puis ordre fixe et conserve pour chaque repetition : main utile,
microdiagnostic updates-only, controle meme-route A/A. Chaque kernel utile fait
un update puis draw dans chaque cellule, change de phase selon
`[0,.5,0][(iteration+repetition)%3]`. Viewport et clear par phase font partie du
temps mural du travail utile et sont identiques pour les deux routes.

Seuls les samples main sont `cpu_wall_submission`, `ns`, population
iterations*batch, repetition indexee depuis 0. operations_expected/observed
updates/draws concernent exclusivement main et valent iterations*repetitions*
batch (draws=0 CPU). Le controle, calibration, chauffe, updates-only et A/A sont
declares exclus et leurs populations/durees brutes restent dans observations.
En mesure, observed compte les kernels completes sans instrumentation dans la
fenetre, pas des compteurs driver materiels ; controle natif separe et pixels
verifient le travail, sans pretendre chronometrer sa completion GPU.

Readbacks bloquants, logs, compteurs de controle et assertions sont hors fenetre.
Pas de glFinish, timestamps GPU, synchronisation globale par update ou retrait
anticipe. Les appels de gardes normaux dans set() restent dans le cout mesure.
Une exception de garde invalide la tranche et ne devient pas un gain.

Politique predeclaree **candidate**, non qualifiee sur plateforme : >=3
repetitions, duree >=100*max(tick declare, minimum positif observe sur 1024
lectures), derive absolue A/A <=25%. Le faux A/A et cette limite de bruit restent
a qualifier ; aucun seuil de gain positif, aucune conclusion de performance.
Zero/temps negatif ne devient pas un sample valide. Samples manquants => INVALID,
mesure non demandee => NOT_REQUESTED, precondition avant mesure => ABSENT.
Un oracle faux => code10/FAIL, mesure demandee INVALID, jamais un succes favorable.

## Raccordement reserve a l'integrateur

1. Dans le parent approprie, ajouter ARTIST_BUILD_VERIFICATION OFF par defaut et
   add_subdirectory(verification) conditionnel apres ArtistLib ; ne pas inscrire
   les workloads aux suites CTest usuelles. Cibles exactes :
   ArtistUniformCpuWorkload et ArtistUniformOpenGLWorkload.
2. Convertir SCENARIO_DESCRIPTIONS en ScenarioSpec central. Les restrictions
   paire profil/plateforme et le contrat cible hardware sont explicites dans
   INTEGRATION_CONSTRAINTS, pas des profils inventes par exclusion.
3. Convertir les noms CTEST_SELECTIONS en regex ancree avec re.escape par nom ;
   decouverte future obligatoire, selection non vide et reconciliation exacte
   noms/execution/JUnit. Construire ArtistLib_tests et component_tests seulement
   pour ces selections, avec MOCK_GL=ON pour les unit.* concernes. Un unit.* est
   une preuve de garde mockee CPU, **pas** une preuve OpenGL reelle.
4. Les plateformes sont chargees de l'attribution renderer avant les selections
   CTest existantes qui ne consomment pas la nouvelle CLI. Reutiliser la garde
   llvmpipe historique, jamais qualifier hardware par absence de nom logiciel.
   Le transport attendu est maintenant ratifie ; aucun run hardware sans cible.
   Ces suites historiques exigent encore un controle de contexte equivalent.
5. Construire argv via protocol.build_workload_argv ; result_path unique dans
   raw/, jamais stdout comme protocole. Le fichier est cree exclusivement : pas
   d'ecrasement, pas de dossier cree implicitement. Une interruption/ecriture
   incomplete reste brute mais ne devient pas une preuve acceptee.
6. Conserver HEAD/delta, hashes de tous les supports natifs/Python/CMake,
   executables, contrat, manifeste et des deux shaders ; deps/options/environnement via core.
   Le natif n'implemente pas son propre SHA-256 ni un second schema de receipts.
   Collecter resultats bruts et logs complets, verifier confinement/identites,
   parser JSON par protocol.parse_workload_result puis agreger les codes bruts.
   Toujours conserver le JSON brut avant validation. Un code20 avec resultat
   INCONCLUSIVE/UNVERIFIED et mesure ABSENT reste une precondition non satisfaite,
   meme si le parseur hardware strict refuse ses observations : ne pas le
   reclasser automatiquement en panne pipeline ni reconstruire une mesure.
   Debug/ASan/coverage ne doivent pas devenir une comparaison optimisee.

## Validation, limites et prochaine preuve

Aucun build, configure, CMake, Conan, CTest, executable moteur meme CPU, GL,
rendu, benchmark, installation ou cloud execute par l'auteur. Compilation,
API, image et performance **NON_EXECUTE / INCONCLUSIVE** Windows et Linux.
Le C++ a ete relu statiquement seulement ; pas de RED/GREEN C++ fabrique.
Windows : bridge wmain/manifest/paths implements en SOURCE_ONLY ; encodage CRT,
paths non-ASCII et embedding du manifeste restent a qualifier, pas un
fonctionnement deja demontre. Le support noreplace de la STL C++23 doit aussi
etre confirme par la future compilation autorisee.

Serena voit la racine canonique ARTIST (pas ce worktree), uniquement C++ ; les
headers/tests pertinents y ont ete inspectes sans modifier de contrat moteur.
La zone unit sous __mock_gl__ est absente de son apercu de symboles ; noms,
gardes de compilation et selections viennent alors des lectures exactes du
worktree. Aucun resultat vide n'est presente comme absence de consommateur.
Revue independante sensible et audit des raccordements restent au lead avant
reception. Ce candidat est du **source pour qualification**, pas un fonctionnement
demontre ni une livraison globale acceptee.
