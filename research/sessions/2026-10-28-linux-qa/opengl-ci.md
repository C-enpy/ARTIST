# OpenGL / CI Linux — campagne initiale, FAIL conservé

## Identité et admission

Auteur pool-5. Plan QA lu intégralement avant édition ou campagne. Worktree
`ensemble-2dc31e0a-ql1erh-pool-5`, HEAD de départ `a4c0f82e1feecf5fc8dda5ab3fa3d32355c19530`, propre.
Build/CPU et ASan intégrés ; copie Linux de référence au même HEAD vérifiée avant
clone privé `/home/artist/src/ARTIST-integration`, ext4, utilisateur artist,
distribution ARTIST-Ubuntu. Aucune édition du checkout Linux de référence.

Conan 2.12.2 / Ninja 1.13.0 depuis le venv de référence en lecture ; CMake3.28.3,
GCC14.2/libstdc++14, profil linux-gcc explicite x86_64/libstdc++11/C++23.
La configuration existante vérifie format/stacktrace au compile/link ; Conan
CMakeDeps/CMakeToolchain est exécuté avant CMake. Sorties et caches Linux privés.
`compile_commands.json` vient des configurations réelles, pas d'include paths inventés.
Serena reste attaché au canonique ; aucune analyse symbolique du miroir prétendue.

## Commandes et matrice réellement exercée

Dans le miroir privé, PATH préfixé par
`/home/artist/src/ARTIST/out/tooling/build/bin`, un job, campagnes sérielles :

```sh
xvfb-run -a python3 -B scripts/linux-evidence.py
xvfb-run -a python3 -B scripts/linux-evidence.py --address-sanitizer
```

Le helper compagnon réutilise `build-linux.py --configure-only`, presets,
canari `verify_asan.py`, CTest JSON/JUnit et bibliothèque standard. Aucun
parseur console GoogleTest ni modification du lanceur existant. Chaque commande
réelle et son résultat sont archivés dans le run ; CTest utilise `--no-tests=error`,
JUnit distinct par label. `LIBGL_ALWAYS_SOFTWARE=true`, `GALLIUM_DRIVER=llvmpipe`
sont appliqués aux enfants ; xvfb-run possède/nettoie son serveur, pas de display étranger tué.

- WSL normal : build réussi ; CPU506/506 ; OpenGL72/75, trois échecs, zéro skip.
- WSL ASan : canari sain0/UAF86 et diagnostic attendu ; build réussi ; CPU506/506 ;
  OpenGL72/75, mêmes trois échecs, zéro skip. Options canoniques, detect_leaks=0.
- Deux populations JSON/JUnit recoupées exactement par nom,506 et75 uniques.
- Windows : six tests auxiliaires du helper passent (enfant exit7,
  output préexistant refusé, population normale, manquant/skip/duplicate/failure,
  CTestexit8 avec75cas/3fail préservés et main→manifestFAIL/exit8).
  Linux : six mêmes tests passés sur la version finale, code0.
  Aucun RED comportemental initial fabriqué :
  tests et implémentation auxiliaire écrits ensemble, limite TDD explicitement conservée.
- Windows moteur normal/ASan : bail et campagnes conservés par le lead,
  aucune exécution moteur Windows attribuée à cet auteur.

Contexte réellement journalisé dans les75JUnit de chaque campagne :
`Mesa | llvmpipe (LLVM 20.1.2, 256 bits) | 4.5 (Core Profile) Mesa 25.2.8-0ubuntu0.24.04.4`.
Les suites PreparedFloatUniformComponentTests, PreparedFloatUniformRenderTests et
ReflectedProgramGuardCrossTests passent ; aucun oracle pixel/tolérance modifié.
Les mutants Windows ne sont pas revendiqués comme exécutés sous Linux.

## Première cause et frontière

`AttributeQaPreflightTest.NativeCapabilitiesAndArraySpelling` : réflexion native
Mesa renvoie `qaArray`, taille1, type35665, emplacement0, au lieu du `qaArray[0]`
exigé par la fixture. Message explicite Q2 stop/arbitration required. Deux suites
en aval échouent sur la même hypothèse : NativeReaderArrayOneRegression et
NativeArraysDescribeThenRefuseWithoutTouchingDecoy. Lead informé ; aucune
normalisation, alias, exclusion ou correction moteur/fixture improvisée.
**Parité fonctionnelle globale FAIL**, pas un succès du seul fait du canari ASan.

## Artefacts et fraîcheur

Sous `out/linux-evidence/` du miroir, copie Windows worktree sous
`out/linux-parity-evidence/linux-evidence/` :
- normal initial `20261028T172225Z-4e352cedae774fe395f9ebf7ea6ccfef` ;
- ASan initial `20261028T174257Z-c22318dc066f48559194a828508879d7` ;
- normal après correction helper `20261028T180529Z-749ee7459ea74ad39cb91a11a932127e`.

Chaque run contient inventaires JSON, XML, logs, commandes et empreintes sources/binaires.
Première tentative interrompue par timeout avant tests exclue, pas RED moteur.
Les deux manifests initiaux conservent leur valeur INCONCLUSIVE erronée de statut :
les commandes ont renvoyé8 et JUnit3fail ; ces archives ne sont pas réécrites.
Correction bornée du helper : conserver comptes/échecs puis propager exit8 et
écrire FAIL. Troisième run réel vérifie manifestFAIL, populations506/75 et3fail.
Pas de cache de résultat réutilisé, chemin de run UUID et création exclusive des logs.

ASan initial SHA256 : manifest `59f552f35d32c7d39aa27717b828e4927b02e3c66ce9ecbe21600fe408687ee1`,
GLXML `d75447aec3182bd869493564a15e911aed30f9939dea47e0b49575b3dcfcf56d`,
CPUXML `35e5014768cac33af191cafe85853f82ae5b9576af88de85be21e17050784913`,
canari summary `610177a1274205232909384224ed1cd60fcc3f7fe026152d0bb1c451b46058b8`.

## CI distante et limites

Commit initial workflow/helpers `84bacef5fcf3f13fad5b9b29e18eeaa70b72f911`, branche
distante `mission/linux-qa-parity` créée sans pousser main. Run natif réellement
observé : https://github.com/C-enpy/ARTIST/actions/runs/40741589259 .
Résultat final : deux jobs exécutés sur runner Ubuntu24.04 propre, conclusion failure,
exit8 ; installation, builds, tests auxiliaires et uploads artefacts réussis.
Artefacts téléchargés sous `out/linux-parity-evidence/ci-40741589259/`.
Pour CHAQUE mode : CPU506/506,75GL exécutés/72pass/3mêmesfail/0skip,
inventaire JSON et JUnit concordants exactement par nom ; contexte identique WSL.
Canari ASan distant sain0/UAF86, diagnostic validé par vérificateur existant.
Les XML contiennent les résultats PreparedFloatUniform/Render/ReflectedProgramGuard.
Manifests distants initiaux INCONCLUSIVE conservés, verdict consolidé FAIL.
La correction finale de reporting n'a pas été réexécutée à distance : test orchestrateur
mocké et runWSL réel vérifient précisément ce delta ; aucun second run aveugle pour
les trois mêmes échecs moteur. Le lead reprend leur correction dans une tranche distincte.
Sonar/Codecov/coverage/Valgrind retirés de la baseline obligatoire ; aucun secret
facultatif requis, aucun changement de protection ou runner auto-hébergé.
Le workflow conserve les preuves via upload-artifact même en échec.

## Sonde native séparée — pas correction moteur

Brief temporaire lead reçu/lue avant écriture de `.tmp/qa-attribute-name-probe.cpp`.
Source privée, aucun changement fixture/moteur. Compilateur et dépendances Conan
existants, build séparé et run sous xvfb-run, tous deux exit0. Artefacts sous
`out/linux-evidence/20261028-attribute-name-probe/` : source.cpp, build.log,
run.log, manifest.json et probe ; manifeste contient commande exacte et hashes.
Les sources exactes des shaders sont journalisées avant compilation.
SHA256 source `76adb5b30d315108449ecbafebfccc791b03a2aec8dc8e4d2424c77cd82b677c`,
binaire `9ccd33dfe46868bf482a3414b394203d089ddcd8329609f80a177336be4950f9`,
log `df1435dc59239d7e0cb2b8a2136276ee68254ee0177aebb4cce84dbe548a6eaf`.
Build avec -Wall/-Wextra : un warning misleading-indentation dans le nettoyage
de cette sonde temporaire (trois if indépendants), conservé au log, aucun warning
désactivé ni prétention de compilation warning-free.

Même contexte Mesa/llvmpipe4.5core ; extension ARB_program_interface_query annoncée
et fonctions chargées, route moderne effectivement exécutée. Les trois programmes
scalar/array1/array2 compilent/linkent ; entrées actives, toutes requêtes error0 :
- legacy : nom `qaProbe` pour les trois ; size1/1/2, type35665, NUL correct ;
- moderne : `qaProbe` scalaire, `qaProbe[0]` pour array1 et array2 ; tailles1/1/2 ;
- locations scalaire ET array1 : base0, [0]0, [1]-1, [2]-1, missing-1 ;
- array2 : base0, [0]0, [1]1, [2]-1, missing-1.

Observation discriminante : noms legacy/moderne divergent pour les arrays sur
ce pilote. La requête [0] ne distingue pas scalaire/array1 ici. Aucun alias,
draw, upload, hypothèse de corruption du buffer leurre ou verdict normatif3.3
universel n'en est déduit. Programmes/shaders possédés détruits sous leur contexte.

GPU matériel, performances, Release, UBSan, fuites exhaustives non validés.
Aucune ressource Docker créée.

## Remise et limites

Bail Linux build/GPU rendu explicitement au lead après terminaison normale de tous
les processus possédés ; aucune nouvelle campagneGPU/CI lancée après cette remise.
Delta limité workflow/helper/test/ce rapport ; aucun C++ de production, CMake,
profil Conan, preset, wrapper Windows ou assertion modifié.
Localisation : commandes dans scripts/linux-evidence.py et workflow ; impact :
conservation et recoupement de preuves normal/ASan, baseline indépendante des services
facultatifs. Limites : trois échecs attributs reproduits empêchent la parité globale ;
pas correction C++ dans cette tranche, pas revue indépendante prétendue.
