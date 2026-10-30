# 84 — Les quatre régressions du collecteur reproduisent les défauts

Le coordinateur a relu les quatre tests et leur confinement, puis exécuté ce seul fichier : **quatre tests, quatre échecs d'assertion attendus**, aucun error ou skip. Les préconditions nominales passent et les empreintes restent stables ; production et 167 tests acquis sont inchangés.

Le rapport contradictoire donne effectivement code0 au lieu de protocole4. Les trois combinaisons de résultats perdent effectivement FAIL au profit d'INCONCLUSIVE : workload échoué puis précondition, première assertion CTest puis interruption, première assertion puis XML tronqué.

Le GREEN minimal est autorisé. Il doit conserver le verdict d'échec déjà démontré, agréger séparément les priorités de codes, préserver les causes et populations disponibles par invocation, et refuser les contradictions de reçu même hors COMPLETED. Aucun nouveau schéma public, adaptateur ou changement d'interface bundle n'est inclus. Les 171 tests et assertions restent conservés.

Les assertions de population situées après la première assertion échouée ne sont pas encore atteintes ; le RED ne prouve pas individuellement leur sensibilité. Le candidat GREEN sera relu avant exécution ciblée, non-régression et ré-audit indépendant. L'intégration du collecteur reste bloquée jusque-là.

## Preuves

- Candidat `55201be44c07545db972c6380b506b467a7227fb`, auteur pool-14, runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_context_review_regressions.py -v`, worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/context-review-red-2026-10-30T04-56-57-319Z/`.
- Avis à l'origine des tests : `result_mvuhogem_0043_6tlusy` ; le défaut du rapport reste attribué comme gap hérité, non comme nouvelle régression.

Aucun processus réel, moteur, compilation, CTest réel, OpenGL, rendu, benchmark, installation ou cloud exécuté.
