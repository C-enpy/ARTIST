# ASan GCC — remise auteur bornée

Base Windows et miroir Linux : `4375080d7dff52819578382d3aca3832b11d065b`.
Plan QA lu avant édition. Runner : pool-4. Miroir privé ext4 :
`/home/artist/src/ARTIST-asan`, distribution `ARTIST-Ubuntu`, user `artist`.

## Matrice réellement exercée

- Linux x86_64, GCC/G++14.2, CMake3.28.3, Ninja1.13 : fixture autonome,
  Debug ASan configure/build PASS ; contrôle sain0, UAF86 avec diagnostic attendu.
- Linux même toolchain Release : refus explicite attendu ; Debug+COVERAGE : refus attendu.
- Windows Python : 11 tests verify_asan, 9 tests fixture restauration PASS.
- Moteur Linux normal/ASan et OpenGL, canari MSVC et bras restauration réels :
  NOT_RUN par auteur, à exécuter après intégration. Pas de preuve de fuites/GPU/performance.

## Commandes exactes (dans le miroir Linux)

```sh
cmake -S scripts/tests/asan-fixture -B out/asan-author/green -G Ninja -DCMAKE_MAKE_PROGRAM=/home/artist/src/ARTIST/out/tooling/build/bin/ninja -DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14 -DCMAKE_BUILD_TYPE=Debug
cmake --build out/asan-author/green --target artist_asan_probe --parallel 1
python3 scripts/verify_asan.py out/asan-author/green/artist_asan_probe --log-dir out/asan-author/canary
```

La fixture sans dépendances moteur ne nécessite pas Conan ; les configurations moteur
conservent impérativement l'installation Conan2 avant CMake. `compile_commands.json`
produit par la fixture ne couvre que le canari, pas le moteur/Serena.

RED causal : même commande configure avec sortie `out/asan-author/red`, module ancien :
refus Windows-only après détection fonctionnelle GCC14.2. Deux erreurs de préparation
(quoting PATH hôte puis chemin include fixture) ont précédé ce RED et n'en font pas partie.
GREEN : flags GCC compile/link et macro instrumentation, canari existant porté sans
changer l'oracle Python. Logs distincts : `out/asan-author/{red-causal,green,release,coverage}.log`
et `out/asan-author/canary/{healthy,use-after-free}.log`, `summary.json` dans le miroir.

## Portée et limites

Seuls Linux x86_64 GCC14 Ninja Debug et branche Windows MSVC existante sont admis.
Options instrumentent TU projet et GTest/GMock construits dans cet arbre, pas les
binaires Conan, système ou drivers. Aucun UBSan ; detect_leaks=0 conservé.
Le vérificateur refuse une politique héritée hostile avant tout enfant ; le lanceur
Linux doit normaliser son environnement enfant avant canari ET CTest. Les nouveaux
tests couvrent refus politique permissive/redirection et activation différée, en plus
des oracles diagnostic absent, mauvais code et contrôle sain déjà présents.
La branche MSVC et ses contrôles PowerShell ne sont pas remplacés par des contrôles Linux.

Localisation/impact : module, probe, tests vérificateur et fixture seulement ; aucun
preset/lanceur ni contrat moteur changé. Les chemins symboliques connus ne nécessitent
pas d'exploration moteur. Catalogue MCP fluctuant après session_move ; aucune racine
partagée ni include path inventé/reconfiguré. Politique scripts externe refusée,
copie locale demandée ; seules commandes directes Context Mode utilisées.
