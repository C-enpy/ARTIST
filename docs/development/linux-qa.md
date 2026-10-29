---
title: Qualification QA Linux et Windows
category: development
tags: [artist, build, testing, guide]
summary: "589 tests Windows, Ubuntu WSL et CI, normal/ASan ; procédure commune, réflexion Attribute compatible et limites de preuve."
status: observed
source_revision: f58a49f4261e018749c59b0d8aab9dd7e85ba53e
created: 2026-10-29
updated: 2026-10-29
sources: [scripts/build-linux.py, scripts/linux-evidence.py, scripts/build-windows.ps1, conan/profiles/linux-gcc, cmake/AddressSanitizer.cmake, .github/workflows/cmake-single-platform.yml, .opencode/evidence/mission-linux-qa/qa-plan.md, .opencode/evidence/mission-linux-qa/final-test-audit.md, .opencode/evidence/mission-linux-qa/final-test-reaudit.md]
---

# QA Linux — procédure et portée

## Résultat exécuté et identité

Le runtime de référence `78e7b74` a exécuté **589/589 tests dans chacune des six
configurations** Windows, Ubuntu WSL et Ubuntu CI, normales et ASan : 513 CPU et
76 OpenGL, zéro failure/error/skip. Découvertes et JUnit concordent ; les ensembles
Windows/Linux sont bijectifs après le seul cast GLM `(glm::qualifier)0` → `0`.
Par rapport aux 581 initiaux, sept tests CPU et un test GL sont ajoutés, aucun retiré.

L'adaptateur de preuves est ensuite renforcé à `f58a49f` pour inclure les shaders
de `test-datas` dans ses identités : runtime C++ et shaders inchangés. **10/10 tests
Python du contrôleur passent sur chaque OS** ; nouveau RED causal sur deux
sous-cas shader avant correction, ré-audit indépendant favorable après GREEN.
WSL normal/ASan et CI renforcés **40841460327** passent également chacun 589/589,
avec 169 empreintes vérifiées dont 20 fichiers shader. Ces quatre nouveaux runs
restent distincts des six campagnes initiales. Voir la preuve portable
`_meta/session-2026-10-29-linux-qa.json` pour le dernier état vérifié.

La première CI `40741589259` avait réellement échoué sur trois cas Attribute,
comme WSL. La CI `40760506988` valide le correctif de capacité à `78e7b74`.
Aucun résultat précédent n'est rebaptisé exécution d'un candidat ultérieur.

## Environnement qualifié

- Distribution dédiée **ARTIST-Ubuntu**, Ubuntu 24.04.5 LTS x86_64, WSL2 ; disque
  `I:\WSL\ARTIST-Ubuntu\ext4.vhdx`, utilisateur non privilégié `artist`.
- Sources, caches Conan/pip, venv et sorties sur ext4 sous `/home/artist`, pas un
  build dans `/mnt/c`. Le miroir final est `/home/artist/src/ARTIST-attribute`.
- GCC/G++ 14.2, libstdc++14, ABI C++11=1, glibc 2.39 ; CMake 3.28.3,
  Conan 2.12.2, Ninja 1.13.0. La sonde C++23 compile **et lie** format/stacktrace,
  avec `stdc++exp` quand requis : le numéro de GCC seul ne constitue pas la preuve.
- Linux graphique : Xvfb et Mesa 25.2.8/llvmpipe, contexte observé OpenGL 4.5 core
  pour une demande 3.3. Windows : MSVC 19.44, NVIDIA RTX 3090, 3.3 core, pilote 621.01.
- `docker-desktop` et les distributions préexistantes sont préservées : aucun
  arrêt global WSL, unregister, migration ou nettoyage forcé.

La création initiale autorisée fut `wsl --install Ubuntu-24.04 --name ARTIST-Ubuntu
--location I:\WSL\ARTIST-Ubuntu --version 2 --no-launch`. **Ne pas la répéter** sur
une distribution existante. Le bootstrap système et ses paquets sont consignés
dans `research/sessions/2026-10-28-linux-qa/wsl-bootstrap.md` ; la CI liste ses
paquets Ubuntu dans le workflow. Les scripts de build ne disposent pas de sudo.

## Reproduire sans second harnais

Dans un checkout Linux au bon candidat, avec GCC14 et dépendances système du
workflow déjà installés :

```bash
python3 -m venv out/tooling/build
out/tooling/build/bin/pip install -r scripts/build-requirements.txt
export PATH="$PWD/out/tooling/build/bin:$PATH"
python3 -B scripts/tests/test_linux_evidence.py -v
xvfb-run -a python3 -B scripts/linux-evidence.py
xvfb-run -a python3 -B scripts/linux-evidence.py --address-sanitizer
```

La campagne finale réutilise le venv ext4 existant via
`PATH=/home/artist/src/ARTIST/out/tooling/build/bin:/usr/bin:/bin` ; elle n'a pas
réinstallé les outils. Le helper appelle `build-linux.py --configure-only`, donc
**Conan install précède CMake**. Profil versionné `conan/profiles/linux-gcc`,
presets `linux-debug` et `linux-debug-asan`, caches et sorties séparés, un job.
Pour CPU seul : `python3 -B scripts/build-linux.py --test-label unit`.

Sous Windows, les commandes réellement utilisées sont :

```powershell
cmake --build --preset windows-msvc-debug --target clean --parallel 1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Jobs 1
cmake --build --preset windows-msvc-debug-asan --target clean --parallel 1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Jobs 1 -AddressSanitizer
```

Le clean concerne un arbre déjà configuré ; ne pas le traiter comme bootstrap
d'un répertoire inexistant. Même précaution Linux pour revalider un header :
`cmake --build --preset linux-debug --target clean --parallel 1`, puis helper ;
équivalent pour `linux-debug-asan`. Pas de compilations concurrentes d'un même
arbre, ni de contextes graphiques partageant les fixtures.

## Gardes et limites

Le helper commun WSL/CI produit un répertoire unique `out/linux-evidence/<UTC-UUID>` :
HEAD/delta, commandes et versions, découverte et XML par label, identités de sources,
configurations, **données shader** et exécutables. Il refuse zéro population,
omissions/doublons/skips/échecs, résultat périmé, code enfant perdu, contexte
llvmpipe absent ou identité modifiée. Les gardes ne prouvent pas une stabilité
transactionnelle contre toute écriture transitoire qui serait annulée avant le
second relevé ; pas d'écriture concurrente supportée promise.

ASan GCC14 utilise `-fsanitize=address` à la compilation et au linkage. Preuve
séparée : 43 TU unit et 13 TU composants instrumentées, symboles/libasan dans les
binaires WSL dont les hashes correspondent aux exécutables testés ; commandes
CI conservées. Canari sain **0**, use-after-free **86** avec diagnostic attendu,
options `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`. Le canari
seul ne démontre pas l'instrumentation des tests moteur. Dépendances Conan et
bibliothèques système/pilote ne sont pas recompilées avec ASan.

Attribute conserve les clés natives legacy exactes. Si version core4.3 ou
extension **annoncée effectivement** et fonctions nécessaires disponibles, la
réflexion moderne distingue array1 du scalaire ; son nom reste temporaire. Le
flag expérimental de GLEW ne suffit pas. Réponse incohérente : staging rejeté,
anciens pointees conservés. Arrays1/2 refusés avant mutation native des buffers
cible/leurre et états vérifiés ; aucune nouvelle garantie de rollback CPU du wrapper.

Limites conservées : premier delta RED classification non hashé intégralement à
chaud ; échec incrémental Windows puis réussite clean à sources inchangées, cause
exacte du suivi de dépendances non entièrement établie. Un pilote 3.3 ambigu sans
route moderne reste non universellement qualifié. Pas de Release, UBSan, coverage,
performance, recherche exhaustive de fuites ou GPU matériel Linux validé.

Les ADR gardent leurs statuts : 0001/2/4/6/7 Proposed, 0003/5 Accepted/partial,
0008 vérifié dans sa baseline Windows Debug seulement. Cette nouvelle preuve
Linux n'adopte ni backend supplémentaire ni architecture globale.

Voir [[development/build-and-test|historique et validation]],
[[architecture/current-state|photographie]], [[adr/README|statuts ADR]].
