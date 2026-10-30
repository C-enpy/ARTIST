# Planifier pipeline Windows Linux

Session: ses_e76a0be90fferxA2bfE6DB40Bj
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T20:34:46.714Z — msg_1895f4d36001PQWu9i025i3bVD

Je reste strictement en lecture seule : inspection des raccordements existants et préparation du plan, sans fichier créé, commande de build/test ni délégation. Je distinguerai les commandes proposées des capacités encore non démontrées à l’exécution.

## assistant — 2026-10-29T20:36:26.359Z — msg_18960d273001QBUX1omY5v5neI

Les raccordements sont présents, avec deux points structurants : Linux ne propose pas encore Release, et le wrapper Windows installe les outils même avec `-ConfigureOnly`. Le contrôleur Linux impose bien llvmpipe, mais pas un total figé de 589 tests : il recoupe dynamiquement découverte et JUnit. Le plan doit conserver ces gardes sans dupliquer le harnais.

## assistant — 2026-10-29T20:37:36.535Z — msg_18961e493001bJ6hm4GaXqsu1f

## Périmètre et constat

**Lecture seule respectée : aucune modification, installation, compilation, exécution de tests ou délégation.**

- **Matrice réellement exercée : aucune.** Les résultats historiques ne constituent pas une qualification du futur pipeline.
- **Première cause d’échec exécutée : sans objet.**
- **Premier manque observé :** Release existe côté Windows, mais pas côté Linux ; `build-linux.py:29` et le profil Conan Linux imposent actuellement Debug.

### Raccordements vérifiés

- `CMakePresets.json` : Ninja, arbres séparés, Windows Debug/Release, Linux Debug ; ASan séparé en Debug.
- Conan 2 existant à conserver : `CMakeDeps`/`CMakeToolchain`, installation **avant** CMake.
- Profils : Linux x86_64/GCC14/libstdc++11 ; Windows x64/MSVC194, mise à jour 4, CRT dynamique.
- `cmake/CheckCxx23.cmake` compile **et lie** `std::format`/`std::stacktrace` : ne pas remplacer cette preuve par le numéro du compilateur.
- Cibles réelles : `ArtistLib_tests`, `ArtistLib_component_tests`, plus `artist_asan_probe` conditionnel. Labels `unit`/`opengl`, verrou OpenGL existant.
- **Correction du briefing :** `linux-evidence.py` impose llvmpipe et compare dynamiquement découverte/JUnit ; **589/513/76 ne sont pas codés en dur**. Il ne relève pas séparément un inventaire global « all ».
- `build-windows.ps1 -ConfigureOnly` crée potentiellement le venv et appelle pip. Son environnement MSVC est restauré à la sortie : appeler ensuite CMake depuis un Python non initialisé serait incorrect.
- Aucune cible benchmark déclarée dans les CMake inspectés ; les compteurs d’uploads existants ne sont pas un benchmark chronométré.

## Architecture recommandée : généraliser, ne pas doubler

**Un seul moteur Python**, proposé sous `scripts/verification/`, avec entrée `scripts/verify.py` :

- `plan.py` : résolution des scénarios et commandes, sans effets ;
- `runner.py` : sous-processus, codes retour, délais et ressources exclusives ;
- `evidence.py` : identité, inventaires, réconciliation JUnit et rapport ;
- `adapters/linux.py`, `adapters/windows.py` : particularités système uniquement ;
- `scenarios.json` : catalogue versionné, sans dépendance YAML.

Faire de `linux-evidence.py` un **adaptateur de compatibilité** du profil historique Linux logiciel. Extraire ses gardes, plutôt que les recopier. Conserver également les interfaces actuelles des deux scripts de build.

Sous Windows, le wrapper PowerShell initialise MSVC puis lance l’orchestrateur Python **dans cet environnement**, avant restauration. Aucun pip automatique dans les modes de qualification.

## Modes à contractualiser

- **`plan`** : lecture configuration/catalogue et sortie stdout uniquement ; aucun sous-processus, réseau, dossier, cache ou rapport sur disque. Invocation Python avec `-B`.
- **`prepare-build-explicit`** : bootstrap uniquement si explicitement demandé, Conan install → CMake → compilation ciblée. Documenter les téléchargements possibles et les sondes/lancements de découverte GoogleTest ; aucune suite comportementale automatiquement exécutée.
- **`qualification-cpu`** : binaires préalablement préparés et identifiés ; scénarios `unit`, sans construire ni installer implicitement.
- **`qualification-gl-software`** : OpenGL réel sur Mesa/llvmpipe ; Linux/Xvfb prévu. Windows logiciel reste une capacité optionnelle à établir, pas une promesse.
- **`qualification-gl-hardware`** : contexte réel, renderer/vendor/version/pilote observés et compatibles avec la cible attendue ; refuser une substitution logicielle.
- **`perf-controlled`** : Release, sans ASan/coverage, workload et protocole explicitement sélectionnés. Un benchmark du **temps CPU de soumission d’uploads OpenGL exige néanmoins un contexte OpenGL réel**.

Un scénario décrit plateforme, configuration, cibles, sélection CTest ou exécutable, fixtures/shaders, contexte, oracle, délai et ressources exclusives. Un scénario sans producteur disponible reste **planifiable mais non exécutable** ; ne pas recycler la durée CTest comme mesure de performance.

## Corrections bornées à prévoir

1. Ajouter `linux-release` aux trois familles de presets.
2. Étendre le helper Linux avec une configuration explicite ; transmettre `build_type=Release` aux profils Conan **host et build**, avant CMake.
3. Conserver ASan Debug seulement, ses canaris et sa politique d’environnement.
4. Garder les arbres actuels pour les toolchains actuellement nommées ; toute nouvelle toolchain nécessite un arbre distinct. Vérifier cache, architecture, ABI et configuration avant réutilisation ; un seul propriétaire actif par arbre.
5. Fournir à clangd/Serena le `compile_commands.json` de cette configuration valide, sans inventer d’includes.

**Dépendances minimales :** Python standard library, CMake ≥3.25, Conan 2.12.2, Ninja 1.13.0 ; compilateur/STL et dépendances graphiques préinstallés selon la machine. Préserver GLFW/GLEW/GLM/magic_enum actuels et GoogleTest v1.14.0. Ne pas introduire `ctest --tests-from-file` sans traiter explicitement son exigence de version ; labels/regex peuvent conserver le minimum existant.

## Preuves et critères d’acceptation futurs

Répertoire unique `out/verification/<run-id>/` contenant :

- commandes exactes, cwd, versions et environnement **filtré sans secrets** ;
- HEAD/delta, empreintes sources/configurations/scripts, fixtures et shaders ;
- empreintes binaires, DLL/runtime pertinents, graphe et révisions Conan résolus ;
- inventaire global, sélection figée, logs bruts, JUnit et rapport JSON.

Relever les identités avant préparation, après build et après exécution. Recouper les **noms**, pas seulement les totaux : population non vide, unicité, omissions, doublons, skips, retours enfants et fichiers périmés. Pour une baseline complète, vérifier aussi la couverture de l’inventaire global par les labels attendus.

Rapporter séparément précondition indisponible (**INCONCLUSIVE**), contrat violé (**FAIL**) et critère démontré (**PASS**). Pour la performance : oracle qualité, warm-up, répétitions, ordre A/B, dispersion, matériel et échantillons bruts ; aucune comparaison de performances cible depuis llvmpipe.

## Ownership et séquence proposés

1. **Contrat/catalogue/rapport** : schéma et critères QA avant implémentation.
2. **Cœur commun** : extraction des gardes et reprise de `scripts/tests/test_linux_evidence.py`.
3. **Adaptateurs OS en parallèle après stabilisation du contrat** :
   - Linux : helper, profil et presets Release ;
   - Windows : wrapper, environnement MSVC et bootstrap explicite.
4. **Intégration** : documentation et éventuel workflow séparé **`workflow_dispatch` uniquement**, sans rattachement automatique au push/PR ni installation déclenchant une campagne.

Les propriétaires conservent leurs tests de régression : absence totale d’effets en `plan`, identité shader/binaire modifiée, XML périmé/manquant, sélection vide, code enfant perdu, mauvais renderer, Release/ASan incompatible, chemins avec espaces et restauration d’environnement. Réutiliser aussi les tests ASan existants.

### Commandes de référence existantes — non exécutées

```bash
python3 -B scripts/build-linux.py --jobs 1 --test-label unit
```

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Release -Jobs 1 -TestLabel unit
```

Ce sont des entrées **actuellement présentes**, avec effets de préparation/build/test ; pas encore la future séparation des modes.

**Limites :** versions réellement disponibles, contexte graphique, support Windows logiciel et performances restent à vérifier lors d’une autorisation ultérieure. Cela ne bloque pas la préparation. Aucun nouveau backend, fournisseur cloud imposé, provisioning ou Docker requis.