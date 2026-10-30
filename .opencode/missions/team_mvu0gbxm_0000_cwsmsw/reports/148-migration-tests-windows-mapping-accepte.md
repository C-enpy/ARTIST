# 148 — Migration des tests Windows : garanties réattribuées avant édition

Le pipeline doit transmettre le bon environnement aux enfants et conserver les
garanties des anciens tests sans imposer un chemin helper-run désormais interdit.
Le plan reçu couvre l'environnement réellement remis à la factory mémoire et
relie chaque ancienne obligation à un test actuel ou à un complément précis.

Une dépendance supplémentaire a été vérifiée : la fixture des wrappers extrait
la fonction d'audit de l'ancien dispatch. Cette fonction restera identique et
extractible. L'ancien programme autonome sera explicitement refusé sans lire
son payload ; sa composition obsolète sera migrée visiblement, avec ses garanties
d'ordre, d'arrêt, d'arguments et de restauration réattribuées. Quoting et code17
restent intacts. Pas de skip global, d'archive invisible ou de helper-run réintroduit.

Le mapping est accepté et persisté avant autorisation d'édition des seuls cinq
fichiers de tests. La production et les pins restent inchangés. Le coordinateur
relira les programmes puis exécutera les sélections synthétiques ; l'auteur ne
lance aucun test. La couture du même bundle avance en parallèle sur d'autres fichiers.

## Preuves et limites

- Plan `result_mvuxtter_008j_btze0z`, pool-40, H0 propre040824c, inspections statiques/Git
  seulement. PM/LLM73/66, brut/retenu identiques, politique inchangée.
- Contrat : `.opencode/evidence/mission-uniform-pipeline/windows-test-migration-plan.md`
  et QA§15. Lead a recoupé wrapper_fixture.ps1:258–281,
  test_wrappers.py:179–227 et Assert-FixtureDispatch:63–114.
- Refus Context Mode sur le worktree extérieur déclaré par l'auteur : pas de
  nouvelle tentative ni contournement autorisé, contrôles indisponibles à signaler.
- Aucun test nouveau ou outil natif exécuté. G2–G4 restent INCONCLUSIVE/NON_EXECUTE ;
  aucune qualification Windows/Linux, GPU ou performance acquise.
