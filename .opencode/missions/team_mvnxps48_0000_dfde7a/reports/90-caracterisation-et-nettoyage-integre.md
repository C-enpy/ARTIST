# 90 — Caractérisation préalable acquise et nettoyage source

2026-10-26. Entrée QA89 committée `f27f690` avant écriture auteur locale. Runner/auteur : lead. Aucun nouveau worker, outillage ou probe exécuté.

## Preuve avant refactoring

Reader.hpp ancien inchangé ; trois caractérisations ajoutées et préremplissage stale dans tests absent/vide. Build `python out/tooling/run-build.py stable-pre-build cmake --build --preset windows-msvc-debug --target ArtistLib_tests --parallel 1` exit0. Découverte `stable-pre-discovery` :558 noms dont15 `unit.ReaderTests.*` ; liste figée `out/qa-f8/stable-20261026-01/pre-reader-selection.txt`. Exécution `stable-pre-reader ctest --preset windows-msvc-debug -R ^unit[.]ReaderTests[.] --parallel 1 --no-tests=error --output-on-failure --output-junit .../pre-reader.xml` : **15/15,0échec**, y compris les trois anciens tests destinés au retrait. Logs complets `out/logs/stable-pre-*.log` et JUnit préservés. Aucune preuve RED inventée : refactor pur fondé sur caractérisations, aucune correction comportementale revendiquée.

## Diff auteur local intégré avant consommateurs

- Reader : retirer alias/paramètre callback et remplacer l'appel par extractor direct ; supprimer commentaires d'injection et préciser fichier stable. Boucle/mask/catch/diagnostics/ordre close-vérification-publication inchangés.
- Tests : retirer ScriptedStreambuf/global/adaptateur, PublicationProbeContext et leurs trois tests. Ajouter court1/4095,8192 exact, texte/LF/tabulations/NUL sur8193 ; conserver toutes fixtures/régressions utiles et assertions existantes ; absent/vide vérifient désormais source préremplie. `<functional>` conservé pour hash du helper de fixture, autres includes dédiés à instrumentation retirés.
- Retirer exactement les13 fichiers85 sous les deux répertoires probes. Contrôle consommateurs immédiatement avant retrait :9 correspondances, toutes dans les probes/protocoles eux-mêmes ; aucun consommateur production/CMake/CI/scripts actifs trouvé dans le périmètre contrôlé.
- Après retrait : recherche `git grep` source/scripts/CMake/presets/.github des identifiants seam et noms probes retourne1 (aucune correspondance), pas erreur. `git diff --check` exit0. Diff limité :15 fichiers,57 insertions/1711 suppressions. Archives, wrappers partagés, diffs étrangers et stagedrename GrepAI intacts.

Les gaps catch/extraction/close authentiques et ordre portable restent ceux85/89, pas clôturés par retrait. Le nouveau candidat doit encore être compilé/testé/revu. Les commandes d'observation ont rencontré deux erreurs locales de quoting et un patch atomique rejeté pour une virgule de contexte erronée ; corrigés avant effet, ni panne MCP ni RED.

Prochaine étape : revue indépendante C++ du candidat en parallèle de la validation runner, pas de mutation concurrente. Windows Debug complet et ASan nouveau candidat justifiés par header commun consommé largement ; aucun555 ou17 présumé depuis la baseline.
