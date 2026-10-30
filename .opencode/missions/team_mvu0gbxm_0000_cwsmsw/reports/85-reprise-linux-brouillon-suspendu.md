# 85 — Reprise Linux inactive, brouillon non intégré

Le nouveau propriétaire de la tâche Linux confirme lecture/plan uniquement et inactivité jusqu'aux entrées communes réellement intégrées. Son HEAD contient les fondations Release reçues, mais pas encore la future signature `describe_preparation(plan, *, bundle_root=...)`.

Le coordinateur contrôle son worktree : un brouillon antérieur non commité subsiste sur le helper Linux, le nouvel adaptateur et quatre fichiers de plan/tests. Il est préservé sans suppression, merge, commit ou exécution. Il ne constitue ni une entrée autorisée des consommateurs ni une livraison du raccordement ratifié.

La mention de « résultats synthétiques » dans la remise ne permet pas d'attribuer une commande ou un exécutant. Une restitution factuelle des éventuels essais antérieurs est demandée, sans relance ni nouvelle écriture. Aucun résultat de ce brouillon n'est retenu comme GREEN par le coordinateur. Les preuves déjà acquises des seules fondations Release restent celles du rapport 82.

## Provenance reçue et recoupée ensuite

La restitution `result_mvui6goz_0000_rxkov5` précise que **pool-19, auteur du brouillon, a exécuté lui-même les tests avant le message restrictif**, sous Windows x64/Python 3.13.1. La séparation auteur/runner n'a donc pas été respectée dans cette reprise. Aucun résultat n'est réattribué au coordinateur et aucun de ces passages ne vaut validation du raccordement futur.

Commandes déclarées : `python -B -m unittest discover -s scripts/tests/verification_linux -p test_*.py -v`, `python -B scripts/tests/test_build_linux.py -v` et `python -B scripts/tests/test_linux_evidence.py -v`, cwd worktree pool-19. L'auteur indique cinq passages de découverte (20/13 erreurs, 20/1 échec, 25/0, 26/0, 27/0), et deux passages de chacun des historiques 9/0 et 10/0. Les journaux intermédiaires ont été écrasés ; aucune empreinte séparée de l'état RED n'est disponible. Les erreurs d'import/adaptateur absent ne sont pas reconnues ici comme RED causal admissible.

Le coordinateur a uniquement lu les cinq artefacts existants dans `out/linux-adapter-synthetic/` du worktree. Ils confirment les résumés conservés : `red.log` 20 tests/13 erreurs ; `green.log` 27 tests OK ; `legacy-build.log` 9 tests OK ; `legacy-evidence.log` 10 tests OK. `summary.json` est présent. Les passages intermédiaires et leur identité restent du témoignage, non recoupables depuis ces seuls logs.

Selon la restitution, chaque passage historique du contrôleur lance un enfant Python factice `-c "raise SystemExit(7)"`, soit deux au total ; fichiers/répertoires synthétiques également créés. Aucun moteur, Conan, CMake, CTest, GL, Xvfb, installateur, benchmark ou cloud réellement lancé selon l'auteur. Aucune nouvelle exécution depuis l'arrêt ; le lead n'a pas relancé ces tests.

Les artefacts restent conservés et le brouillon reste NON intégrable. Le futur auteur recevra le contrat au bon HEAD et remettra ses tests avant implémentation, avec exécution réservée au coordinateur. Pas de suppression, nettoyage, complétion fictive ou réadmission pour masquer cet écart.

Empreintes SHA-256 relevées en lecture seule :

- `red.log` : `54d6070bc62ccb0f817dc4d3fc5c9b155d2c8524bc3d560bf15d57f9e22e3724`.
- `green.log` : `410479c2e91661b409509fb569808f6ed049cb282cd20adae9ab197e8f58c42a`.
- `legacy-build.log` : `eb7a8fbab773dcf57eb25c8ff8d9360f5afc4738937f8aded4f46b38c4973d79`.
- `legacy-evidence.log` : `a544f224b307cb94369f2211b0fbc9d509c5de287996ed2913425b8c7faf72ee`.
- `summary.json` : `73da20b920b7a66efa91326054631ddbf3c5a578bed632fc50db68f6b94a9c11`.

Le plan proposé pour l'adaptateur attend collecteur, complément QA et interface intégrés. La construction seule via le helper historique est une proposition à traiter dans cette tranche future, pas une autorisation présente de reconfiguration, CTest ou nouveau câblage.

## Traçabilité

- Remise `result_mvui4q2f_0049_p7rcmd`, tâche `task_mvu0yczu_0007_2rv7yr`, propriétaire pool-19.
- HEAD `d2d7be70b5d1b82816c03faab7135a9f1405eb4b`, worktree `ensemble-2dc31e0a-cwsmsw-pool-19`.
- Chemins contrôlés : `scripts/build-linux.py`, `scripts/verification/adapters/linux.py`, et `scripts/tests/verification_linux/{LINUX_ADAPTER_PLAN.md,README.md,test_adapter.py,test_core_composition.py}`.
- Contrôle effectué : `git status --short`, `git diff --stat`, `git rev-parse HEAD` seulement.

Aucun nouveau test ou outil moteur exécuté par le coordinateur ; aucune qualification native ou GPU attribuée à ce brouillon.
