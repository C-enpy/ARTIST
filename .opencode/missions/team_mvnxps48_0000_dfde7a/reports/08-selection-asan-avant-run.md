# 08 — Sélection ASan figée avant exécution

2026-10-25. Runner : lead, racine canonique ; aucun build/installation ni nouvelle suite normale. H3 documentaire intégré : `849e6d50c2a545bfdaf661d4d0f408e9a0066a7a`, sources identiques à318bf7a.

## Contrôles acquis

- Les hashes du manifeste H3 sont tous encore identiques ; pas de delta source/configuration ASan suivi.
- Préflight réutilisé après contrôle des logs : sain0 + ARTIST_ASAN_HEALTHY sans diagnostic ; faute86 + AddressSanitizer heap-use-after-free. Trois binaires identiques au handover42. Pas de nouveau canari ni reconstruction par principe.
- Build existant : /fsanitize=address, /RTC1, **-MDd** et TestUtilsTests.cpp présents. Un premier contrôle ad hoc cherchait `/MDd` et a échoué ; lecture de build.ninja a établi la graphie réelle `-MDd`. Aucun flag modifié ; pas un RED moteur.
- Versions réellement appelées via wrapper : CMake/CTest3.31.7, Ninja1.13.0.git.kitware.jobserver-pipe-1, Conan2.12.2. Les quatre commandes --version exit0 ; logs `out/logs/h3-tool-version-{0,1,2,3}-20261025-01.log`.
- Découverte CTest normalisée exit0 : `out/logs/h3-asan-discovery-20261025-01.log`,555 uniques =484unit+71OpenGL, ensembles identiques à l'archive normale. Les17 du protocole13 sont tous présents exactement une fois.

## Artefacts préalables au run

Archive créée exclusivement pour cette campagne : `out/qa-b2-h3/final-20261025-01/`.

- `discovery.json` : copie complète de la découverte actuelle.
- `selected-names.txt` : les555 noms exacts, aucune exclusion. SHA256 **9da0543500a4c597dccd03250bcd0a7db64493bc5c8e052ae150e165324b8a05**.
- `candidate-before-run.json` : état SELECTED_NOT_RUN, horodatage antérieur, HEAD documentaire/source, hashes sources/config/binaries,9empreintes PDB/DLL, canari réutilisé, environnement et commande complète.

Commande CTest prévue, via wrapper existant :

`ctest --preset windows-msvc-debug-asan --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-20261025-01/selected-names.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-20261025-01/asan-selected.xml`

Le processus dédié chargera scripts/asan-env.ps1 puis Set-AsanTestEnvironment. Bail fichier exclusif `out/artist-opengl.lock` et mutex local ARTIST_QA_OPENGL pendant la campagne ; absence d'autre processus CTest/ArtistLib_tests/component vérifiée avant départ. Ce bail coordonne les processus coopérant sur cette ressource, pas un verrou universel du GPU. Tous les workers ont confirmé l'absence de campagne et restent inactifs. RESOURCE_LOCK CTest seul ne suffirait pas entre processus.

Après run : archiver immédiatement LastTest.log et sorties complètes, comparer découverte/sélection/exécution/JUnit par nom et unicité, rechercher diagnostics tardifs puis recontrôler empreintes. Zéro failure/error/skip exigé pour PASS. Un résultat partiel ne vaut pas validation globale.

## Admission et statut

La complétion H3 a déclenché l'auto-affectation ASAN avant le claim du lead ; claim natif lead refusé car déjà in_progress. pool-1 a confirmé deux fois son inactivité et aucune commande ; il ne devient pas runner. Le lead exécute uniquement le mandat runner approuvé, avec bail canonique exclusif. L'audit n'est pas admis ; sa libération automatique doit rester empêchée tant que son gate n'est pas résolu. Aucun état natif fabriqué, worker réadmis ou modèle imposé.

Des erreurs de syntaxe ponctuelles des scripts de collecte ont été corrigées avant mutation/exécution de campagne. L'échec de contrôle -MDd a donné une hypothèse discriminante vérifiée, pas une répétition coûteuse du build. Ces incidents de dispositif ne sont pas des échecs des tests ARTIST.
