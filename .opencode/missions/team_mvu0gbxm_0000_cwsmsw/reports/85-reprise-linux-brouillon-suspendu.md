# 85 — Reprise Linux inactive, brouillon non intégré

Le nouveau propriétaire de la tâche Linux confirme lecture/plan uniquement et inactivité jusqu'aux entrées communes réellement intégrées. Son HEAD contient les fondations Release reçues, mais pas encore la future signature `describe_preparation(plan, *, bundle_root=...)`.

Le coordinateur contrôle son worktree : un brouillon antérieur non commité subsiste sur le helper Linux, le nouvel adaptateur et quatre fichiers de plan/tests. Il est préservé sans suppression, merge, commit ou exécution. Il ne constitue ni une entrée autorisée des consommateurs ni une livraison du raccordement ratifié.

La mention de « résultats synthétiques » dans la remise ne permet pas d'attribuer une commande ou un exécutant. Une restitution factuelle des éventuels essais antérieurs est demandée, sans relance ni nouvelle écriture. Aucun résultat de ce brouillon n'est retenu comme GREEN par le coordinateur. Les preuves déjà acquises des seules fondations Release restent celles du rapport 82.

Le plan proposé pour l'adaptateur attend collecteur, complément QA et interface intégrés. La construction seule via le helper historique est une proposition à traiter dans cette tranche future, pas une autorisation présente de reconfiguration, CTest ou nouveau câblage.

## Traçabilité

- Remise `result_mvui4q2f_0049_p7rcmd`, tâche `task_mvu0yczu_0007_2rv7yr`, propriétaire pool-19.
- HEAD `d2d7be70b5d1b82816c03faab7135a9f1405eb4b`, worktree `ensemble-2dc31e0a-cwsmsw-pool-19`.
- Chemins contrôlés : `scripts/build-linux.py`, `scripts/verification/adapters/linux.py`, et `scripts/tests/verification_linux/{LINUX_ADAPTER_PLAN.md,README.md,test_adapter.py,test_core_composition.py}`.
- Contrôle effectué : `git status --short`, `git diff --stat`, `git rev-parse HEAD` seulement.

Aucun nouveau test ou outil moteur exécuté par le coordinateur ; aucune qualification native ou GPU attribuée à ce brouillon.
