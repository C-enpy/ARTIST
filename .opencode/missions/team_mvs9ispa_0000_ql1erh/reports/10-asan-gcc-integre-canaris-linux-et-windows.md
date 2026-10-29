# QA Linux — ASan GCC intégré, canaris Linux et Windows vérifiés

## Résultat et portée

La tranche module/canari ASan GCC est intégrée au commit `f461f43706477c6e5bd55ba3049730b05fabc9f9`. Le lead a relu le diff, recoupé le RED causal et reconstruit/testé le canari Linux sur les sources finales ; le canari Windows réel et les contrôles de restauration existants passent aussi.

Ce jalon ne valide pas encore le moteur Linux normal/ASan, les pixels ou la CI. La branche build/CPU reste active ; OpenGL/CI attend l'intégration des deux producteurs.

## Livrable auteur et contrôle d'intégration

Auteur pool-4, base `4375080`, commit `f6f0c6d7745b711e5a093feeae8925ca7fde3619` : cinq fichiers, worktree propre, diff --check propre. Plan QA déclaré lu avant édition. Pas de modification des presets, profil ou lanceurs détenus par pool-3.

Fichiers intégrés :
- `cmake/AddressSanitizer.cmake` : garde explicite Linux x86_64 GCC14 C/C++, Ninja Debug, flags compile/link, garde instrumentation ; branche MSVC conservée.
- `cmake/probes/AddressSanitizer.cpp` : diagnostic sain Linux, API Windows conditionnelle ; faute UAF isolée et intentionnelle inchangée.
- `scripts/tests/asan-fixture/CMakeLists.txt` : fixture autonome sans dépendances moteur.
- `scripts/tests/test_verify_asan.py` : trois contrôles supplémentaires d'environnement ; vérificateur de production inchangé.
- `research/sessions/2026-10-28-linux-qa/asan-author.md` : remise et commandes auteur.

Shutdown puis merge natif : livrable fusionné ; nettoyage de worktree impossible, branche/worktree conservés sans suppression forcée ni répétition du cycle. Commit lead limité aux cinq fichiers, aucun travail préexistant incorporé. Tâche `task_mvsa43rc_0006_6jw879` clôturée par le lead après intégration et contrôles réels, pas par l'auteur.

## RED et écart d'identité détecté

Le log auteur `red-causal.log` contient le refus Windows-only du module ancien après détection fonctionnelle GCC14. Les erreurs initiales de quoting/include sont explicitement exclues : ce ne sont pas des RED causaux.

Au contrôle lead, le miroir Linux conservait un include CMake supplémentaire et l'ancienne version des tests Python, alors que le worktree final avait été nettoyé/enrichi. Cette différence n'est pas cachée derrière le GREEN auteur. Le lead a synchronisé les fichiers finaux dans le seul miroir privé ASan (auteur arrêté), puis configuré/construit une sortie neuve `out/asan-lead-final/build` et rejoué les contrôles. Le GREEN final ci-dessous est donc attribué au lead sur les sources réellement intégrées, sans reconstruction de l'historique auteur.

## Contrôles réellement exécutés par le lead

### Linux, utilisateur artist, miroir privé ext4 ARTIST-asan

Configuration de `scripts/tests/asan-fixture`, Ninja, gcc-14/g++-14, Debug, sortie neuve, puis build du seul `artist_asan_probe` avec un job. Commandes de configuration/build consignées dans les sorties sous `out/asan-lead-final/`.

`python3 -B scripts/verify_asan.py out/asan-lead-final/build/artist_asan_probe --log-dir out/asan-lead-final/canary` : **PASS borné**, sain 0, UAF 86 et diagnostic attendu. Politique `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`.

`python3 -B scripts/tests/test_verify_asan.py -v` : **11/11**, code 0.

Les refus Release et Debug+COVERAGE ont été exercés par l'auteur ; logs conservés distinctement, pas réattribués au lead.

### Windows sur l'arbre intégré, exécutions sérialisées

- `C:/Python313/python.exe -B scripts/tests/test_verify_asan.py -v` : **11/11**, code 0.
- `C:/Python313/python.exe -B -m unittest discover -s scripts -p test_asan_env_fixture.py -v` : **9/9**, code 0 ; fixtures Python privées, pas bras PowerShell réels.
- `powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -AddressSanitizer -AsanPreflightOnly -Jobs 1` : **code 0**, canari MSVC réel sain 0/UAF86, diagnostic attendu. Le Bypass est limité à ce processus, pas une modification de politique système.
- `C:/Python313/python.exe -B scripts/test_asan_env.py --label linux-qa-asan-integrated-20261028` : **code 0**, normalisation hostile et **4/4 bras réels de restauration** (retour/exception, variables présentes/absentes) réussis, zéro failure. Pas de revendication de suite moteur exécutée.

Le contrôle de restauration peut mettre temporairement à l'écart puis restaurer son arbre de build selon son contrat ; il a été exécuté après le canari, sans autre build Windows concurrent.

## Preuves, identités et limites

Logs Windows/runner : `out/logs/mission-linux-qa/asan-integrated-*.txt`. JUnit/restauration : `out/asan-caller-restoration/linux-qa-asan-integrated-20261028/{junit.xml,results.json}`.

Copies des artefacts auteur et lead Linux : `out/logs/mission-linux-qa/asan/{asan-author,asan-lead-final}/`. Summary canari final SHA256 `45f1278fe187d671c55310b99404868b1c31763b463c937eee926a5174ee1b8f` ; log UAF `3bbc2d88c6d3dbbffc146cc744090f233d4c97d09442f26f5a29aaf9c350ce75`.

Sources finales recoupées Windows/Linux : module `77e9b199905a303e528a71f5acdef09857f1c64a314da618a83fe66aef6baa66`, tests Python `9ef8eb7aa2abc0425065640cf27e406865c2c17aa8a592b03f31f8e1616eb018`. Binaire Linux canari final `e4daebd0fd91e49a2c2e8cef7350ad7b0b0e7660e57440c048b697f7316c4c8c`.

L'instrumentation couvre les TU construites dans cet arbre, pas les binaires Conan/système/driver. `detect_leaks=0` reste explicite. Pas d'UBSan, de preuve d'absence de fuites, de performance, de GPU ou de RAII générale.

Le refus de lecture de la politique externe Context Mode par l'auteur est conservé comme limite d'accès : pas de répétition ni élargissement ; l'auteur rapporte n'avoir produit que des commandes directes, pas de script d'analyse non trivial. Le lead possède déjà cette politique dans son contexte. Aucun profil/MCP partagé reconfiguré.

## Suite autorisée

Pool-3 a reçu explicitement chemins distro/source/venv/outils/cache et bail CPU normal, un job ; seul bail apt/dpkg de la distro dédiée. Son miroir privé et ses sources restent indépendants de la présente intégration. Il connaît le contrat d'environnement canonique ASan que le lanceur devra appliquer avant canari ET CTest.

Le lead intègrera son livrable avant les campagnes conjointes. L'audit final reste deferred et conditionnel à son contrat hors board/profil/restitution ; aucune auto-publication ni PASS global.
