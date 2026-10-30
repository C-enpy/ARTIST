# 147 — Plan de couture du même bundle accepté avant tests

Le pipeline doit prouver que les preuves produites par le helper sont celles
collectées puis réadmises en construction, pas deux jeux de données compatibles
fabriqués séparément. Le plan reçu conserve un seul bundle et de vrais reçus,
avec les mêmes octets de témoin entre préparation et construction.

Le coordinateur a recoupé l'ordre préparation/collecte/reçu et l'admission avant
description build dans le code intégré, ainsi que le rôle limité de RuntimeFixture.
Le plan est accepté et persisté avant le prochain worktree auteur. Trois nouveaux
fichiers de tests seulement sont réservés, sans changement de production, de pin
ou de fixture historique. Les effets outils resteront simulés en mémoire ; aucun
helper complet, outil natif ou moteur ne sera lancé.

La prochaine tranche peut écrire les programmes et les tests. Le coordinateur
les relira avant toute exécution, vérifiera les préconditions puis les quatre cas
de couture. Les tests Windows de l'autre branche restent indépendants. L'audit
final attend les deux livrables réellement intégrés, pas seulement la fin des auteurs.

## Preuves et portée

- Proposition A0 `result_mvuxoywe_008g_tq7mqk`, pool-39, lecture seule ; PM/LLM63/59.
- Contrat persisté : `.opencode/evidence/mission-uniform-pipeline/g1-seam-fixture-plan.md`,
  raccordé au plan QA§15. A1 développement78/73, brut/retenu identiques.
- Code recoupé : evidence.py718–807 et runtime_fixtures.py214–253 ; HEAD lead
  avant ce plan29d5758, code inchangé depuis52e7330/9aedc0a.
- Ni programme ni test nouveau exécuté. G1 demeure INCONCLUSIVE/NON_EXECUTE ;
  qualification native Windows/Linux et GPU toujours NON_EXECUTE.

## Publication après persistance

Le plan/QA ont été committés en `d19c720638bc56eb46f2862874110d0e1b6f2852`
avant publication de la tranche tests `task_mvuxrzkh_008i_pze6wy` : build-engineer,
contexte fresh et worktree isolé, PM/LLM78/73. La dépendance est le vrai ID du
plan A0 reçu et persisté. La réception finale reste réservée au lead après merge
et vérification ; aucun worker runner ni changement de politique ajouté.
