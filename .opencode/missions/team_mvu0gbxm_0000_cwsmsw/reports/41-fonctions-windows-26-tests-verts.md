# 41 — Fonctions Windows : 26 tests verts, deux raccordements à compléter

## Résultat vérifié

Le coordinateur a relu les six fonctions logiques et confirmé que tests et audits sont inchangés. **11 tests des fonctions, 10 du dispatch et 5 de l'interface passent**, sans error ou skip. Les empreintes restent stables.

Le codage d'arguments traverse le petit enfant Python factice ; les fonctions d'environnement travaillent sur des maps. Cela ne démontre ni les primitives OS, ni l'import MSVC, ni le helper complet, toujours bloqué.

## Gaps concrets avant réception complète

Une variable d'environnement nommée `Keys` est refusée pour éviter un accès PowerShell ambigu. Ce refus explicite est sûr mais ne satisfait pas la restauration exhaustive. Un test discriminant est demandé avant correction ; toute évolution de l'audit devra être limitée à l'accès exact requis, pas à des méthodes arbitraires.

La liste privée des cibles omet `ArtistLib_tests`, pourtant requis avec `ArtistLib_component_tests` par les scénarios préparation/lifetime intégrés. Un test nominal de ces deux cibles et de configuration avec tests activés est demandé. Le scénario CPU seul doit conserver sa configuration sans tests ; les 26 assertions existantes ne sont pas affaiblies.

Ces corrections restent dans le périmètre du pipeline prévu. Aucun nouveau backend, charge GPU, outil ou infrastructure n'est ajouté.

## Preuves

Candidat `9377eff5ea413f749656d0ce22127e15710a417c`, auteur windows-pipeline, runner coordinateur. Découverte unittest avec `C:/Python313/python.exe -B`, dossier `scripts/tests/verification_windows`, filtres séparés `test_wrappers.py`, `test_dispatch.py`, `test_helper_contract.py`, option `-v`.

Journaux et commandes : `out/verification-synthetic/windows-wrappers-green-2026-10-30T01-18-54-446Z/`. Aucun moteur, vrai compilateur/Conan/CMake/CTest, installation, rendu, benchmark ou cloud exécuté.
