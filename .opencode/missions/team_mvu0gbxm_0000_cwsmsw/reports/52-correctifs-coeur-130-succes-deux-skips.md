# 52 — Correctifs du cœur : 130 succès, deux contrôles non exécutés

## Résultat vérifié

Le coordinateur a relu le runner et le diff des preuves, du rapport et de la CLI. La suite complète découvre **132 tests : 130 satisfaits, deux SKIP, aucun failure ou error**, code de sortie 0. Les tests et fixtures sont inchangés depuis le RED ciblé ; empreintes stables pendant l'exécution.

Les régressions exécutables passent : drainage borné sous doubles, contradiction des empreintes rejetée, nouvelle configuration détectée, répertoire Windows isolé, refus matériel précoce et variables de contexte héritées retirées avant application de la déclaration explicite.

Les deux SKIP concernent les liens symboliques, dont Windows refuse la création. Leur comportement runtime reste INCONCLUSIVE. Aucune permission ou méthode de contournement n'est ajoutée. Les tests de drainage ne constituent pas une expérience réelle de descendant Windows conservant les pipes ouverts.

## Barrière maintenue

Le ré-audit indépendant du correctif est demandé avant réception et intégration. Le candidat reste stable en attendant les findings. La correction d'ouverture du journal via handle vérifié doit notamment être examinée statiquement malgré l'absence de test de symlink exécuté.

Le parseur et le producteur de contexte historiques ne sont pas encore intégrés à ce cœur. Les adaptateurs de production restent également hors de cette preuve ; le résultat ne vaut pas validation du pipeline global. Aucun moteur, compilateur, outil de construction réel, OpenGL, rendu, benchmark, installation ou cloud exécuté.

## Preuves

- Candidat : `8ecb5675825b551ee6beede171ecf48e8351f48b` ; auteur pool-3 ; runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v`, depuis le worktree auteur.
- Tests/fixtures inchangés depuis `797cc568978e936de4d81a8c94ee4396c0534358`.
- Journaux, commande et empreintes : `out/verification-synthetic/core-review-green-2026-10-30T02-52-08-696Z/`.
