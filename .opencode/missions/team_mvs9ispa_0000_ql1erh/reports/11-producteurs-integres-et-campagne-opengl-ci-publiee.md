# QA Linux — producteurs intégrés, campagne OpenGL/CI publiée

## Résultat vérifié

Le build/CPU est intégré à `a4c0f82e1feecf5fc8dda5ab3fa3d32355c19530`, après l'intégration ASan `f461f437`. Le lead a recoupé les preuves auteur, les sept identités source exécutables et le comportement CPU Windows sur l'arbre intégré. Le checkout Linux de référence a été explicitement transféré/avancé au même HEAD et vérifié propre.

La tâche build/CPU a été clôturée par le lead ; seule la tranche OpenGL/CI a été publiée. Elle est affectée à pool-5, build-engineer fresh, sur une base disposant des deux producteurs réellement intégrés. Gaps, audit et documentation restent deferred.

## Preuves CPU et oracle stacktrace

Rapport auteur intégré : `research/sessions/2026-10-28-linux-qa/build-cpu.md`. Commits auteur `c944672` puis `ee5b30ce95600976e571795ae0a9c0222b388e7c`, six fichiers, diff --check propre. La copie temporaire de politique fournie par le lead a été lue puis retirée par le lead avant shutdown, sans toucher les fichiers auteur.

Conan a d'abord signalé des paquets X11/XCB manquants : précondition système corrigée dans la seule distro dédiée, pas RED moteur. Le RED CPU archivé porte sur un unique test : l'oracle fallback littéral appliqué à tort à la branche stacktrace native libstdc++. Le correctif ne change que le test, conserve static_assert by-value et fallback Windows exact, et compare la sérialisation native exacte des entries capturées. Aucune assertion remplacée par substring/tolérance.

Relecture lead des XML archivés :
- RED : 506 cas uniques, un échec GetStackTraceString_ReadsTheReturnedValueSafely, aucun skip.
- GREEN CPU : 506 uniques, zéro échec/skip, mêmes noms que la découverte Linux.
- GREEN lanceur complet : idem 506/506, sortie JUnit neuve.
- Les sept hashes source/configuration exécutables correspondent exactement au contenu intégré (presets, profil, dépendances, sonde STL, lanceur, tests du lanceur, test C++).

Windows exécuté par le lead après merge : `powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -TestLabel unit -Jobs 1`, code 0, recompilation du test concerné, **506/506 CPU**, test fallback inclus. Neuf tests Python du lanceur exécutés par le lead sur Windows : **9/9**, code 0. Le rapport auteur attribue aussi 9/9 à Linux. Le wrapper Windows de production est inchangé.

Historique TDD du nouveau lanceur : l'auteur conserve les premiers essais avec fichier absent comme preuve de bootstrap manquant, pas comme RED moteur ; le RED CPU et les contrôles négatifs ultérieurs ne reconstituent pas une preuve comportementale antérieure manquante du lanceur. Cette limite reste explicite.

## Correspondance Windows/Linux : noms bruts différents, population réconciliée

Le manifeste Windows historique a été relu et son SHA256 recalculé concorde avec `29a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b`.

La première comparaison exigeant l'égalité brute des noms échoue : **144 noms de tests typés** affichent le dernier paramètre GLM comme `0` sous MSVC et `(glm::qualifier)0` sous GCC. Ce n'est ni un échec moteur ni une égalité de noms à prétendre obtenue.

Rapprochement borné : seule la représentation explicite du cast enum pour cette valeur 0 est normalisée ; aucun autre remplacement. Vérification de l'injectivité de chaque côté (506 entrées, 506 clés uniques), égalité des ensembles d'identités, zéro manquant/excédentaire après cette correspondance. Les noms bruts et les 144 associations sont conservés dans `out/logs/mission-linux-qa/build-cpu-author/cross-platform-test-name-map.json`. Aucun test n'a été renommé ou supprimé pour satisfaire le rapprochement.

## Intégration et moyens remis au consommateur

Shutdown/merge natifs réussis pour le contenu ; nettoyage de worktree encore impossible, branches/worktrees conservés sans nouveau cycle ou suppression forcée. Le commit lead n'inclut que les six fichiers auteur ; les travaux étrangers restent hors des commits de mission.

Le lead a transféré `out/logs/mission-linux-qa/integrated-producers.bundle` dans `/home/artist/src/ARTIST`, checkout détaché HEAD a4c0f82 et status vide. Le consommateur doit vérifier son propre HEAD/plan puis travailler dans un miroir privé, pas dans cette référence.

Pool-5 a reçu ownership workflow/helpers seulement, vraie matrice WSL/outils, baux Linux build/GPU logiciel/apt, un job, références des preuves, correspondance de noms, défaut Sonar historique et limites. Les builds Windows sont réservés au lead pour éviter les collisions.

La politique complète Context Mode a été transmise par le lead dans le canal du worker avant ses analyses structurées, sans élargissement de droits ni nouvelle lecture externe refusée. Une erreur de syntaxe de l'envoi initial a été corrigée avant livraison ; le second appel confirme la réception de l'envoi, pas encore sa lecture par le worker.

## Suite et limites

Le consommateur doit exécuter OpenGL réel logiciel/pixels et suites normales/ASan, puis une CI propre sur les mêmes commandes. Branche distante de mission/PR ou événement natif privilégié ; aucune poussée/merge main, modification de protections/secrets ou runner auto-hébergé n'est autorisée implicitement.

Les preuves Windows complètes normal/ASan restent à finaliser au candidat intégré ; le lead les réalise en parallèle de la campagne Linux sur des sorties séparées. Ni CPU506 ni canari0/86 ne prouvent les 75 OpenGL ou l'intégration ASan moteur.

Audit final toujours deferred, hors board et restitution à qualifier au bon jalon. Aucun GPU Linux matériel, benchmark, Release, UBSan ou absence exhaustive de fuites revendiqué.

## Emplacements des preuves

- `out/logs/mission-linux-qa/build-cpu-author/` : 18 fichiers auteur copiés avant nettoyage, dont RED/GREEN/JUnit/discovery/candidate.patch/candidate.sha256.
- `out/logs/mission-linux-qa/build-integrated-windows-unit.{txt,xml}` : exécution lead.
- `out/logs/mission-linux-qa/build-integrated-launcher-windows.txt` : neuf tests Python lead.
- Tâche consommateur : `task_mvsa43rc_0007_cq2xsm`, publication explicite après clôture réelle des deux producteurs, completion lead conservée.
