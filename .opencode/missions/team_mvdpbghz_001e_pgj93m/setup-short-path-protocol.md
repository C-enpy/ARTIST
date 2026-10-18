# B intégré — protocole discriminant de setup D

Proposition : build-engineer/pool-7, tâche task_mvdybblj_000z_nj1cs8,
C54/R43 PM/LLM, fresh, lecture seule. Restitution native relue par le lead.
Aucune compilation/test ni écriture par B. Pool-7 arrêté. Auteur et exécutant
de la future expérience : D/pool-6, même tâche TDD ; pas un nouveau runner autonome.

Source R :
C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e469fd1358b48e9d#pgj93m-pool-6
HEAD source attendu :1f1ad63b3a4d9d5819845eeb135275beaee4234c.
Sortie exclusive S proposée (Temp/opencode approuvé) :
C:/Users/djden/AppData/Local/Temp/opencode/b-nj1cs8
Build long B0=R/out/build/windows-msvc-debug.
Objet O=ARTIST/component-tests/CMakeFiles/ArtistLib_component_tests.dir/src/graphic/pipeline/AttributeComponentTests.cpp.obj.

## Faits et hypothèse

Digest D :C1083 fichier généré vide/Invalid argument,source193caractères,objet267,
logSHA2562ddc1de7c75b2d32681fe1fa346c0e5e26c272bf24a30df4c27a34a0bb87fe12.
Calcul B sur git ls-tree de la base canonique, sans accès externe au log :
R121,cwd150,/Fo relatif116,absolu267 ; /Fd résolu215. Source/objet longueurs
concordantes,mais cl n'imprime pas le nom fautif. Hypothèse chemin objet long,
pas encore démonstration générale MAX_PATH pour toutes les APIs Windows.
45cpp dans tests+component-tests,12objets calculés>259,33entre211et259.
Frontière259 :unit LoaderTests.cpp,ReaderTests.cpp,UserTests.cpp selon B.
Le caractère# existe aussi dans les autres commandes ; pas cause isolée établie.
Étape46/58 du graphe Ninja n'est pas la46eTU. -TestLabel unit ne restreint pas
la compilation de toutes les cibles avant CTest,label ne sélectionne que les runs.

## Préconditions et protections

Un seul processus de compilation,aucun test/GPU,source et HEAD inchangés avant/après.
Pas pip/réinstallation arbitraire. Venv R/out/tooling/build/Scripts déjà créé ;
PATH le préfixe,CONAN_HOME resteR/out/conan2,CMAKE_BUILD_PARALLEL_LEVEL=1.
Importer le même vcvars et même SDK que setup,MSVC14.44.352/cl19.44,
Conan2.12.2,Ninja1.13.0 attendus ; version différente=>arrêtINCONCLUSIVE.
Reprendre découverte vswhere/import environnement de scripts/build-windows.ps1
lignes66–92,sans relancer tout le script. Helper de chargement sousS/tooling.
Pas SUBST/junction/symlink/registrylongpaths/déplacement worktree natif.
Ne pas copier CMakeCache,CMakeFiles,_deps d'une autre racine.
Si S préexiste avec contenu non possédé=>arrêt,ne rien écraser/supprimer.
Conserver les artefacts ; aucune suppression récursive de S incluse dans ce GO.
Si une configuration produit un fichier source inattendu (CMakeUserPresets,etc.),
signaler l'écart et arrêter,pas effacer un fichier d'ownership incertain.

## Phase0 — vérification sans compilation

Depuis la racine D autorisée,analyser son log complet et résoudre les /Fo des
compilations effectivement réussies contre le cwd. Ne pas assimiler une ligne
command affichée à une compilation réussie sans son résultat.
Attendu hypothèse :aucun objet absolu>259 compilé avec succès ; relever les
témoins à259si présents. Si succès>259 :hypothèse invalidée par ce témoin,
arrêt avant nouvelle compilation,commander une nouvelle expérience au lead.
Conserver commandes/longueurs/état de chaque témoin,sans dump complet du log.

## Phase1 — A/B isolant /Fo

Seulement après Phase0 compatible. Même source,cwdcl,natifcl,flags et /Fd.
Extraire la commande réelle depuis Ninja -t commands O ou compile_commands.json ;
ne pas la reconstruire depuis la liste partielle transmise par chat.
Créer S/fo,S/logs,S/tooling seulement si libres et possédés.
Capturer texte initial,hash et commande effective. Une occurrence /Fo pour O.
Commande dans un @rsp :copier ce rsp sousS/logs,ne pas modifier original.

Arm0 :commande inchangée,cd /d B0,logS/logs/arm0.log.
Attendu :exit1,C1083 nomvide,Invalid argument reproduit.
Arm1 :seulement /Fo remplacé par S/fo/AttributeComponentTests.cpp.obj (86caractères),
mêmecwd/même/Fd,logarm1. Toute autre différence d'arguments=>INCONCLUSIVE/arrêt.
Utiliser un objet court neuf possédé,ne pas supprimer un préexistant inconnu.
Si cmd généré,doubler % sans changer les arguments vus par cl ;quoter les chemins
contenant# ou espaces. Journaliser les commandes réellement exécutées.

Succès discriminant :Arm0 reproduit,Arm1exit0,objet>0octet,diff limité /Fo,
HEAD/source inchangés. Aucun RED/GREEN moteur établi par ce résultat.
Arm0réussit=>garder objet/logs,INCONCLUSIVE,longueur non reproduite,arrêt.
Arm1échoue autrement=>nouvelle cause,arrêt ;pas répétition à coût accru.
Arm1mêmeC1083=>rapport avant Phase1b. Phase1b suggérée B changerait aussi /Fd
versS/fd (54caractères) mais n'est PAS incluse dans le premier GO du lead.

## Phase2 proposée par B — non autorisée par le GO initial

Après revue humaine/lead de Phase1,possibilité de configurer build courtS/build,
longueur57,mêmeobjet174 et pluslongenviron181. Source resteR.
Le script/preset fixe source/out/build et n'expose pas-B ; ne pas combiner le
preset existant avec une sortie manuelle ni modifier scripts/CMake pour l'enquête.

Équivalent proposé :Conan install sourceR versS/build,profilconan/profiles/windows-msvc
hôte/build,build_typeDebug,installation_pathMSVCidentique,tools.build:jobs=1,
tools.cmake.cmaketoolchain:user_presets= vide pour éviter écriture source.
Puis cmake --fresh -S R -B S/build -G Ninja,
-DCMAKE_TOOLCHAIN_FILE=S/build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
-DBUILD_TESTING=ON -DMOCK_GL=ON -DCOVERAGE=OFF -DARTIST_ENABLE_ASAN=OFF.
Comparer commandes de la TU après seules substitutions des préfixes build ;
/Od/Ob0/RTC1/std:c++latest/MDd/Zi/showIncludes/FS restent. Pas __mock_gl__ sur
component ; pas Qt ajouté,Conan directglm/glfw/glew/opengl/magic_enum,GTest1.14.
Dry-run ninja -C short -j1 -n -- O :si plus queTU/régénération=>arrêt,ne pas
exécuter graphe large. Sinon compilation Oseulement. Pas cibleexe niCTest à ce
stade :gtest_discover_tests POST_BUILD pourrait exécuter un binaire.
Erreurs réseau/FetchContent,find_packageinattendu ou toolchain différente
restentINCONCLUSIVE,ne pas changer de générateur ou STL pour passer.

## Mandat initial effectif

Le lead autorise D seulement Phase0 puisPhase1 si témoins compatibles.
Buildsource/flags inchangés,aucune source production/tests neuve à écrire,
aucun RED fonctionnel ni GPU. Retour résultats/hashes/longueurs/commands/exits
avant élargir àPhase1b/2. B support livré n'est pas une autorisation de cloreD.
Lead réévaluera ensuite la bonne racine de sortie pour le cycleTDD complet.
