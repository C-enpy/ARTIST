# 10 — Premiere preuve RED synthetique acquise

## Resultat observable

Le runner coordinateur a relu puis execute la premiere tranche de tests de l'orchestration : 28 tests de modeles Request et CommandSpec. Ils tournent sans dependance executable externe, sans moteur et sans reseau.

Le resultat est un RED causal : 28 tests executes, 63 echecs d'assertion en comptant les sous-cas, code retour 1. Les entrees invalides ne sont pas encore rejetees par le socle permissif et env_delta emprunte encore un mapping mutable. Ce sont les comportements attendus pour demonstrer la sensibilite des tests avant implementation, pas une panne d'import ou d'environnement.

Les empreintes du test et des modules compares avant/apres execution sont identiques. Le socle rouge n'est pas integre comme implementation valide. Le proprietaire a recu l'autorisation GREEN pour cette seule tranche ; les tranches planification/runner/preuves attendront leur propre RED.

## Attribution et preuve

- Auteur et proposition : pool-3/cpp-engineer, candidat e88d070c12b116a4c4972ee3936788e516ca84fe.
- Executant reel : coordinateur, Python 3.13.1, Windows, worktree isole de pool-3.
- Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v.
- Journaux complets, commande, code et empreintes : out/verification-synthetic/core-model-red-2026-10-29T23-01-29-624Z/ (manifest.json, stdout.log, stderr.log).

## Portee

Cette preuve porte sur les schemas et bornes des scripts uniquement. Aucun PASS du pipeline complet, compilation C++, comportement OpenGL ou gain n'est etabli. Les autres branches poursuivent leurs premieres tranches independantes. Integration et QA finale restent differées jusqu'aux livrables effectivement integres.
