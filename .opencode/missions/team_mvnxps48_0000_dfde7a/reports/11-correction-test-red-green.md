# 11 — Correction locale du test : RED → GREEN ciblé

2026-10-25. Contrat/QA10 commité avant édition (51a4356, amendement2a2e40e). Auteur et runner : lead, tranche locale indivisible, C62→70/R65 PM/LLM. Aucun nouveau worker d'implémentation, installation ou modification de production.

## Changement

Seul fichier modifié : `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp`, test `PassDestructorReleasesProgramThenShaders`. Ajout d'un EXPECT_CALL local `glGetActiveAttrib_mock` avec programme5/index0, taille du nom nominal du mock, quatre sorties NotNull et cardinalité1. L'action ON_CALL existante reste utilisée. Les assertions5/8 et attentes InSequence detach5/8 → deleteProgram5 → deleteShader8 restent exactement présentes.

Le défaut provenait du chemin de diagnostic d'un appel non intéressant imprimant le pointeur de sortie GLchar* avant son remplissage. Le correctif rend cet appel explicitement attendu sans désactiver globalement les warnings, sans NiceMock et sans toucher au remplissage0x7f/contrôle de terminaison du Reader. Aucune signature partagée ou API publique modifiée.

## Preuves ciblées

1. RED, avant édition, ancien binaire instrumenté : test isolé exit86, heap-buffer-overflow identique à la campagne initiale. Commande via wrapper : `ArtistLib_tests.exe --gtest_filter=ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders`. Log complet `out/logs/h4-focused-red-20261025-01.log`.
2. Contrôle CLI `--gmock_verbose=error` également86, conservé dans `h4-focused-quiet-control-20261025-01.log`. Contrôle non calibré : main.cpp appelle InitGoogleTest seul, option GMock non appliquée ; **pas de PASS ni preuve de suppression**. Il n'a pas servi au GREEN et n'a pas été répété.
3. Build incrémental ASan **unit seulement**, normalisé et parallèle1 : `cmake --build --preset windows-msvc-debug-asan --target ArtistLib_tests --parallel 1`, exit0, une compilation du TU changé puis link (5,7s). Log `out/logs/h4-asan-unit-build-20261025-01.log`. Pas de configure/clean ni rebuild component/probe.
4. GREEN **avec diagnostics par défaut**, aucun flag GMock : `ctest --preset windows-msvc-debug-asan -R ^unit[.]ShaderLifetimeTests[.]PassDestructorReleasesProgramThenShaders$ --parallel 1 --no-tests=error --output-on-failure --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/h4-focused-green-20261025-01.xml` : exit0,1/1. Log complet `out/logs/h4-focused-green-20261025-01.log`, JUnit et `out/qa-b2-h3/h4-focused-green-LastTest-20261025-01.log` conservés pour recoupement.

Le seul delta source entre RED et GREEN est cette attente locale. Les autres appels non intéressants et la même action par défaut restent actifs. La validation globale finale n'est pas déduite de ce1/1.

## Limites et suite

Les warnings de compilation C4005 dans le mock et C4834 sur une ligne inchangée sont conservés dans le log ; aucun correctif de warning hors périmètre. LSP Serena prêt mais macros GTest de cette TU sans symboles retournés ; impact borné par grep/lectures exactes, pas preuve d'absence de consommateurs.

À exécuter sur le delta intégré : non-régression CPU normale affectée (484tests), nouvelle découverte/sélection/manifeste, campagne ASan555 complète ; conserver archive rouge final-20261025-01. Preuve GL normale réutilisable seulement si ses sources/binaire restent identiques. Pas de performance revendiquée.

Revue indépendante adaptée du petit diff à obtenir dans un contexte fresh, après son intégration ; cela ne remplace pas l'audit final test-reviewer hors board ni son gate. Aucune tâche ASAN ou clôture déclarée réussie à ce stade. F8/C2/B1/GCC et statuts ADR inchangés.
