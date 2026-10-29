# QA Linux — validation finale et documentation intégrée

## Résultat livré

La parité demandée est vérifiée dans son périmètre borné : **589/589 sous Windows, Ubuntu WSL et Ubuntu CI, normal et ASan**, 513 CPU + 76 OpenGL, zéro failure/error/skip. L'adaptateur renforcé inclut les shaders consommés ; ses quatre nouvelles campagnes Linux passent également589/589, avec169empreintes vérifiées dont20fichiers shader.

Les deux défauts trouvés en revue sont corrigés après RED causal : capacité GLEW effective et identité des shaders. Les ré-audits indépendants ont fermé leurs findings. Le dernier contrôleur dispose de **10/10 tests sous Windows et Linux**. Les preuves ne sont pas rebaptisées : Windows/runtime78e7b74 et Linux renforcéf58a49f restent des exécutions distinctes, C++/shaders/config identiques.

## Documentation et connaissance

Procédure commune, environnement WSL sur I:, versions, commandes, résultats et limites sont intégrés dans `docs/development/linux-qa.md`, liés depuis build-and-test, accueil et photographie courante. Les notes ADR-0005/0008 pointent ces preuves séparées sans changer leurs statuts. Preuve portable : `docs/_meta/session-2026-10-29-linux-qa.json`.

Les changements préexistants de trois notes ont été préservés et exclus du commit par staging partiel contrôlé ; aucun écrasement ou inclusion des travaux étrangers. Neuf fichiers documentaires intégrés à `e8a6dbe`, frontmatter/tags et153liens contrôlés ; le littéral C++ `[[nodiscard]]` du journal historique n'est pas un lien. QMD ciblé :1nouveau,7modifiés,52inchangés ;79chunks de8documents ré-embarqués. Recherche et lecture de la nouvelle note dans artist-wiki confirmées.

## Publication et dette administrative

La PR sera empilée sur le candidat initial exact de cette mission pour exclure78commits locaux comprenant d'anciennes missions non intégrées à main. Aucun push/merge de main, changement de protection/secrets ou runner auto-hébergé. Le dernier push documentaire portera une marque skip-ci : il ne change aucun exécutable/configuration depuis les campagnes validées et ne doit pas déclencher une nouvelle campagne moteur uniquement pour les rapports. Cette marque n'est jamais utilisée pour un changement de source non validé.

Deux anciennes lignes du plan restent **deferred/inactives** : audit et documentation, pourtant réellement livrés. La clôture native de l'audit a refusé une tâche non publiée ; la publier rétrospectivement pourrait lancer un doublon. Elles sont maintenues inactives par contrôle natif, sans complétion fabriquée ni mutation de base interne. Leurs livrables et acceptations sont consignés ici ; ce reliquat administratif ne vaut ni test manquant ni invitation à relancer. Équipe et worktrees préservés, pas de cleanup forcé.

## Commandes et preuves principales

- Windows lead : clean des deux presets existants, puis `scripts/build-windows.ps1 -Jobs 1`, option `-AddressSanitizer` pour le second ; XML `gate-green-windows-*`.
- Linux lead : `xvfb-run -a python3 -B scripts/linux-evidence.py`, puis option `--address-sanitizer`, miroir ext4, un job ; campagnes renforcées `shader-identity-final-linux/20261029T082047Z-9067b2324c294e30bf25e50e24a6762e` et `20261029T082059Z-cb91935384bd43ceb3203b24998132fb`.
- CI réelle [40841460327](https://github.com/C-enpy/ARTIST/actions/runs/40841460327), deux jobs success, artefacts téléchargés sous `out/logs/mission-linux-qa/ci-40841460327/`, découverts/XML et169hashes recoupés. Résumé `shader-identity-final-reconciliation.json`.
- Tests contrôleur : `python -B scripts/tests/test_linux_evidence.py -v`,10/10chaqueOS ; logs `shader-identity-green-{windows,linux}.log`, RED et hashes capturés avant production. Les trois mutants antérieurs en mémoire restent archivés, aucune mutation production sur disque.
- Index : `node .opencode/scripts/update-wiki.mjs`, `qmd embed -c artist-wiki`, puis recherche QMD/lecture de la note. Aucun autre corpus indexé.

## Limites restantes

GPU Linux logiciel llvmpipe seulement, pas de Release/UBSan/coverage/performance/fuites exhaustives ; detect_leaks=0 et dépendances système/Conan non instrumentées. Premier delta RED classification historiquement incomplet ; cause des objets Windows incrémentaux incohérents non entièrement expliquée, échecs conservés. Fallback3.3 ambigu sans réflexion moderne non universellement qualifié. Double relevé d'empreintes, pas isolation transactionnelle contre toute écriture transitoire. Les avertissements GitHub sur les actions Node20 ne sont pas une panne et n'ont pas déclenché de migration hors mandat.
