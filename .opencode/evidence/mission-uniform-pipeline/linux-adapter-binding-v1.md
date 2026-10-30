# Raccordement Linux v1 — précision locale ratifiée avant GREEN

Entrée commune intégrée `94e4111257cd6ce72ea318d1d9119ddfe29fb71e`, réception95.
Proposition source `678b21502f6d993ed2bc5d9244caa63d9d4adc18`, plan
`scripts/tests/verification_linux/LINUX_BINDING_RED_READY.md`. Cette décision
n'implémente pas les modes ci-dessous et ne qualifie aucun outil Linux natif.

## Helper historique conservé, deux modes étroits

- `--build-only` exige une sélection non vide `--target <cible>` répétable,
  dédupliquée dans l'ordre, sans shell/split. Il décrit uniquement
  `cmake --build --preset linux-<configuration> --target <cibles...> --parallel <jobs>`.
  Aucun Conan, configure, CTest, canary ASan ou création de logs implicite.
  L'arbre doit déjà être préparé ; un échec ne déclenche pas de réparation.
- `--verification-workloads` exige `--configure-only`, incompatible avec build-only.
  Il ajoute `-DARTIST_BUILD_VERIFICATION=ON` au configure existant, sans construction
  ou exécution. Ce nom est déjà utilisé par `ARTIST/verification/CMakeLists.txt` ;
  les parents CMake restent au futur intégrateur, pas à l'auteur Linux.
- Les cibles ne sont acceptées qu'en build-only ; les modes sont exclusifs.
  Le défaut historique et configure-only sans nouveau flag gardent leurs argv,
  codes, profils et gardes. Conan2 host/build reste cohérent avant CMake.
  Release+ASan reste refusé ; build-only ASan ne prétend pas qualifier sa canary.

## Adaptateur et frontières

Constructor/descriptions calculent des CommandSpec depuis plan, bundle explicite
et reçus, sans écritures, processus, sondes, accès filesystem ou cache dernier bundle.
Prepare déclare son effet de résolution de dépendances pour l'opt-in du core ;
build réutilise le helper en mode construction seule, sans effet bootstrap.

Run direct depuis le mapping d'exécutables du BuildReceipt admis et le builder
de protocole existant. IDs par invocation stables et distincts issus du run_id,
sorties privées attribuées, aucun helper-run ni seconde capture. Une découverte
CTest puis sélection exacte par regex ancrée/échappée, même arbre/configuration,
sans nombres historiques fixes. Utiliser la syntaxe regex réellement acceptée
par CTest, pas des extensions propres à Python comme les groupes non capturants.

CPU sans GL/Xvfb ; software llvmpipe explicite ; hardware cible exacte et six
variables de contexte pour chaque invocation CTest. Le DISPLAY/X server doit
être préparé par l'opérateur lors de la future qualification ; aucun démarrage,
installation, réparation d'affichage ou fallback logiciel implicite.

`collect_environment` est la frontière distincte de métadonnées : observations
OS/architecture/Python et versions allowlistées g++-14/cmake/conan/ninja via argv
`--version`, sans shell, timeout fini au plus15s et sorties bornées/traitées.
Absence/erreur d'outil explicitement indisponible, pas de version inventée ni
de qualification GPU/STL déduite. Aucun dump de l'environnement ou secret.
Ces appels ne sont pas autorisés pendant les essais de cette mission : les tests
les doublent strictement. Les descriptions ne doivent jamais appeler cette frontière.

Request, schémas de reçus, core, presets, contrôleur historique et fichiers Windows
restent inchangés dans cette tranche. Les nouveaux modes et l'adaptateur suivent
RED observé par le lead, GREEN minimal, non-régression et revue indépendante avant
intégration. Aucun essai moteur, toolchain ou affichage réel n'est autorisé.
