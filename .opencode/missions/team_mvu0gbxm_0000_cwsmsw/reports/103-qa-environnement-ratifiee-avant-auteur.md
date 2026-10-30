# 103 — Plan QA de l'environnement ratifié avant auteur

Le complément QA est reçu intégralement et ratifié au §12 du plan. Il vise la
copie enfant au moment du lancement, pas seulement les variables décrites par
l'adaptateur. Les contrôles couvrent assainissement matériel, refus avant factory,
profils inchangés, parent intact et absence de contamination entre appels.

Une ambiguïté du scénario « chaque champ retiré » est résolue conformément au
contrat : si le sélecteur matériel lui-même est absent ou différent, aucun
traitement hardware ne se déclenche. Les cinq autres champs sont obligatoires
lorsque le sélecteur est explicitement matériel. Il ne s'agit pas d'affaiblir le
transport complet d'une commande matérielle.

Le plan sera commité avant admission d'un auteur unique runner/tests. L'auteur
commencera par les tests RED, sans modifier la production ni exécuter de test.
Le candidat Linux reste stable, non intégré ; son auteur n'écrit pas le runner.
Les travaux Windows sur l'adaptateur et les paramètres continuent sans collision.

## Références

- Plan proposé : `result_mvumebdm_0000_uv51g1`, qa-planner/pool-21, lectures
  réussies, aucun refus ni exécution ; réponse finale complète native reçue.
- Contrat `hardware-child-environment-v1.md`, plan `qa-plan.md` §12.
- Petite tranche indivisible, PM/LLM complexité brute62/retenue70, risque65,
  incertitude moyenne ; aucun changement de politique ou modèle explicite.

Aucun nouveau test, moteur, outil natif, GL, rendu, benchmark, installation ou
cloud exécuté. La reproduction dynamique du défaut reste à obtenir.

## Admission après commit du plan

Plan commité `b89110594731737f5f813c4017599a195e56033d` avant publication.
Tâche `task_mvumhg9g_005j_81env1` admise fresh avec worktree et completion lead,
tests RED seulement. Le statut natif confirme pool-26 sur build-engineer, worktree
isolé `ensemble-2dc31e0a-cwsmsw-pool-26`. Ownership unique futur du runner explicite,
sans auteur concurrent ; aucun fichier de production autorisé à l'écriture initiale.
