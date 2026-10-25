# 12 — H4 vérifié : ASan555 PASS, CPU484 PASS et revue statique indépendante

2026-10-25. Candidat C++ intégré **82bf52d52e594fae02593866669a1af715127d1f** ; seule modification C++ :8lignes ajoutées au test ShaderLifetimeTests.cpp. Production identique à318bf7a. Backend de présentation arrêté à la demande ; aucun port8765 restant au contrôle.

## 1. Résultats réellement exécutés

Après RED causal86 puis GREEN ciblé1/1 au rapport11 :

- Build incrémental normal **unit seulement**, `cmake --build --preset windows-msvc-debug --target ArtistLib_tests --parallel 1`, exit0.
- Non-régression normale CPU : `ctest --preset windows-msvc-debug -L unit --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <racine>/out/qa-b2-h3/h4-normal-unit-20261025-01.xml` : **484/484**,0failure/error/skip, noms identiques au sous-ensemble unit de la découverte ASan actuelle. Logs `out/logs/h4-normal-unit-{build,run}-20261025-01.log` et LastTest archivé dans out/qa-b2-h3.
- Particularité de dispositif conservée : l'appel parent PowerShell normal contenait `ExecutionPolicy` sans tiret et a émis une erreur de préambule Get-ExecutionPolicy/Bypass. Les commandes build/CTest suivantes ont réellement tourné et leurs codes/logs/JUnit ont été recoupés ; cette erreur ne devient ni un RED produit ni une invocation sans diagnostic. Aucun rerun pour effacer cette trace, aucun changement de politique système.
- **71OpenGL normaux historiques réutilisés**, pas rejoués : sources et binaire component normaux identiques à la preuve31. Ne pas annoncer555normaux nouvellement exécutés.
- Nouvelle découverte ASan normalisée exit0 ; sélection555 figée avant run, mêmes noms que l'archive rouge initiale ; les17historiques chacun une fois. Manifeste pré-run au HEAD82bf52d, hashes source/config/binaries/PDB/DLL enregistrés.
- Campagne finale corrigée **ASan555/555 =484CPU+71OpenGL**, exit0, **zéro failure/error/skip**. Derniers diagnostics contrôlés ; résultats identiques par noms/uniqueness entre découverte, sélection, exécution et JUnit. Les17historiques chacun exactement une fois.

Commande effective via wrapper existant `out/tooling/run-build.py h4-asan-selected-20261025-01` :

`ctest --preset windows-msvc-debug-asan --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-h4-20261025-01/selected-names.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-h4-20261025-01/asan-selected.xml`

Runner lead, processus dédié après Set-AsanTestEnvironment ; bail fichier exclusif out/artist-opengl.lock et mutex LocalARTIST_QA_OPENGL. Début16:59:27Z, fin17:01:12Z ; durée CTest103,06s, pas mesure performance. ASAN_OPTIONS exact canonique, activation absente ; /RTC1, -MDd, assertions conservés. Component et probe non reconstruits, canari0/86 antérieur réutilisé après contrôle d'identité/logs. Aucun nouveau configure, clean, Conan/pip ou installation.

Archive **out/qa-b2-h3/final-h4-20261025-01/** : candidate-before-run.json, discovery.json, selected-names.txt, asan-selected.xml, ctest-full.log, LastTest.log, runner.log, run-start.json/run-end.json, verification-summary.json. L'archive rouge final-20261025-01 est conservée intégralement et reste FAIL à son ancien candidat.

Recoupement post-run :555marqueurs Test Passed.,0Test Failed. ;0diagnostic ASan dans les sorties,0texte tardif après les résumés PASSED ; aucune marque de troncature recherchée ; sortie JUnit maximale17112octets (<65536). Toutes les empreintes du manifeste et9PDB/DLL identiques après run. Sélection SHA2569da0543500a4c597dccd03250bcd0a7db64493bc5c8e052ae150e165324b8a05 ; binaire ASan unit2f945fb722a1ca83e8bb40c98cb2fc4c885c9f01d1ac7593162f73e3b051b764 ; verification-summary.json **aeb82398ce9a549ab560686f942ac55862988c2f77a380771fe184308f1bba7f**.

## 2. Revue indépendante adaptée, réellement reçue

Tâche supplémentaire `task_mvo2i3hl_0000_8mpvxt`, affinité correctness-reviewer explicite, C45/R55 PM/LLM (analyse). Auto-dispatch sur **pool-3/correctness-reviewer**, session native ses_e8c04042effegjjPjglcZ3r6Tm, contexte neuf déclaré sans rapports auteur10/11 ; transport team_message effectif. Le spawn explicite lead a été refusé car tâche déjà affectée, pas de doublon.

Premier accès worktree refusé external_directory : aucune nouvelle tentative de ce chemin ni ouverture de droits. Résolution par **nouvelle entrée autorisée**, pas contournement : [review82-input.md](review82-input.md), contexte/contrat/delta et extraits fournis par le lead dans le dépôt canonique explicitement désigné. Profil/transport déjà confirmés, reviewer repris dans sa même session indépendante pour traiter ces données. Lectures ciblées canoniques réellement faites, aucun test/shell/édition/délégation. Hashes fournis/recalculés par le lead, pas par le reviewer.

### Remise pool-3 acceptée par le lead

**Aucun défaut bloquant démontré dans le delta ciblé.** Points indépendamment vérifiés par lecture :

1. ShaderLifetimeTests.cpp363–367 : attente program5/index0/capacité sizeof(ATTRIBUTE_NAME), quatre sorties non nulles, Times(1), aucune action substituée ; ON_CALL nominal conservé.
2. Lignes371–373 : InSequence detach(5,8), deleteProgram(5), deleteShader(8), chacun une fois ; assertions handles5/8 intactes379–380. Scénarios discriminants statiques : suppression/inversion/double destruction restent des violations GMock ; absence/double réflexion ou mauvais programme/index/capacité ne sont pas acceptés silencieusement.
3. Mock : ATTRIBUTE_NAME14caractères+NUL, capacité15, copie15 et length14 ; conversion GLsizei constante représentable. AttributeReader70–79 : remplissage non-NUL, garde signée avant indexation, rejet terminaison manquante préservés. Aucun nouvel emprunt/owner ou capture de pointeur.
4. Pas de finding bloquant ni suggestion de style. Limite explicite : ce correctif local ne garantit pas la sûreté de tous les diagnostics d'appels inattendus ; aucune exécution indépendante CPU/GPU/GCC/performance ni RAII exhaustive prouvée.

La revue et la campagne ont avancé en parallèle : source intégrée et stable pour la lecture, un seul runner/build canonique. Cette revue correctness-reviewer ne remplace **pas** l'audit final test-reviewer hors board.

## 3. État exact des gates

La correction et la campagne H4 sont vérifiées à leur portée. Le résultat FAIL initial n'est pas effacé ; il est résolu pour ce test par un delta causal testé/revu. QA-PORTABILITE et audit final restent séparés ; aucune admission test-reviewer hors board native compatible n'est encore établie. Ne pas libérer automatiquement l'audit sur le seul PASS ASan ; aucune tâche fantoche/completed pour arrêter le dispatch.

Pas de clôture globale moteur/F8/C2/GCC, Linux/Release/image/performance ; statuts ADR maintenus. F8 catch INCONCLUSIVE/close DEFER, C2 OPEN/INCONCLUSIVE, B1 historique non résolu. Les diffs concurrents et l'index GrepAI sont préservés.
