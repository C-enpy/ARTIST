# Mission QA Linux — publication progressive corrigée par la voie native

## Résultat et portée

La correction opérationnelle autorisée du board est appliquée et relue : un seul préalable publié, sept consommateurs différés et huit clôtures réservées au superviseur. Cette correction réduit le risque de départ prématuré sans inventer de tâche, de résultat ou de dépendance.

La mission reste non approuvée et non démarrée. Aucun start, worker, classifieur, travail Linux, création WSL ou run CI n'a été lancé. La correction ne lève aucune pause opérateur.

L'absence de capacité décrite dans les rapports 03/04 est historique : le nouveau déploiement expose désormais les contrôles natifs nécessaires. Les rapports antérieurs sont conservés sans réécriture.

## Observations avant et après

Avant mutation, `team_status` confirme le rôle de lead, la politique « draft (not approved) », aucun teammate et huit tâches : une pending, sept blocked. `team_tasks_list` retrouve les huit vrais IDs importés, sans marqueurs de publication différée ou de clôture lead. Les valeurs par défaut published/assignee sont documentées dans la référence native.

La signature effectivement redécouverte de `team_tasks_add` expose `task_controls`. Un seul appel avec `tasks: []` et huit contrôles a retourné : « Added 0 tasks; recorded 8 task control(s) ». Le succès du lot signifie aussi que ses préconditions natives avant attribution/admission ont été acceptées ; aucune écriture SQL n'a été utilisée.

Après mutation, la relecture native et les assertions sur les huit lignes donnent :

- **1 publiée**, le contrat de parité ;
- **7 publication:deferred**, dont l'audit ;
- **8 completion:lead** ;
- **1 pending et 7 blocked**, statuts inchangés ;
- mêmes IDs, profils, contextes fresh, contrats worktree/report_mode, scores et décisions d'exécution affichés.

La relecture de `team_status` confirme encore « draft (not approved) », aucun teammate, classification manuelle sans appel, huit tâches. La mention générique « supervised/running » n'est pas une approbation et n'est pas interprétée comme telle.

Les dépendances n'ont fait l'objet d'aucune mutation : le lot ne contient que publication, completion_policy et motifs. Leur conservation relève du contrat natif `task_controls` ; la projection textuelle de `team_tasks_list` ne détaille pas les arêtes `depends_on`. Aucune inspection ni modification directe de SQLite n'a été réalisée.

## Contrôles appliqués — vrais identifiants

1. **Contrat de parité QA** (`task_mvsa43rc_0003_5x254n`) : published, completion lead. Seule racine publiée ; sa remise doit encore être reçue, vérifiée, persistée et commitée avant les auteurs. Le motif du contrôle ne présente pas cette remise comme déjà acquise.
2. **Distribution WSL dédiée sur I:** (`task_mvsa43rc_0004_dujtol`) : deferred, completion lead. Attendre le contrat QA intégré puis publication explicite.
3. **Build Linux et CPU** (`task_mvsa43rc_0005_r040cn`) : deferred, completion lead. Attendre QA/WSL intégrés, puis branche indépendante d'ASan.
4. **Portage ASan GCC** (`task_mvsa43rc_0006_6jw879`) : deferred, completion lead. Même barrière QA/WSL, propriétaire et sorties indépendants du build.
5. **OpenGL logiciel et CI** (`task_mvsa43rc_0007_cq2xsm`) : deferred, completion lead. Attendre l'intégration et le contrôle des deux producteurs build/ASan.
6. **Gaps du candidat intégré** (`task_mvsa43rc_0008_8508em`) : deferred, completion lead. Entrées réellement intégrées/exécutées ; compléments retenus à réaliser et intégrer avant audit.
7. **Audit fresh indépendant** (`task_mvsa43rc_0009_tr8pgb`) : deferred, completion lead. Maintenir hors publication/auto-admission tant que son contrat d'indépendance et de restitution n'est pas qualifié.
8. **Documentation de clôture** (`task_mvsa43rc_000a_0yh289`) : deferred, completion lead. Attendre le candidat audité et ses preuves intégrées.

## Progression future, seulement après approbation utilisateur

Contrat QA intégré → WSL qualifié → build et ASan en parallèle lorsque leurs entrées sont intégrées → intégration et contrôles des deux producteurs → OpenGL/CI → gaps sur candidat intégré/exécuté → audit indépendant qualifié → documentation.

La fin d'auteur ne vaut jamais intégration : l'auteur remet son livrable, le superviseur contrôle et intègre avant toute clôture. `completion_policy: lead` impose qui peut clôturer, mais ne prouve pas automatiquement que ces vérifications ont eu lieu. Chaque consommateur requiert une publication explicite ; la résolution des dépendances ne publie aucune tâche différée. Une publication ne contourne pas l'approbation native ou une pause.

## Réserve particulière : audit encore conditionnel

Le différé retire l'audit des projections partagées prévues par cette capacité. Il n'efface ni les messages ni les informations antérieurement divulguées et ne démontre pas une indépendance déjà qualifiée.

Le contexte fresh, worktree false et report_mode final_response enregistrés restent des préconditions statiques, pas des preuves runtime. Aucun reviewer n'a été lancé ou qualifié pendant cette correction.

Au futur jalon, préparer un dossier autonome : contrat, candidat intégré et preuves, sans conclusions privées des auteurs. Vérifier les permissions natives effectivement appliquées, le contrat hors board d'implémentation et la restitution réellement admissible. Si ces conditions ne sont pas démontrables, garder l'audit deferred et expliquer la limite plutôt que le publier automatiquement.

Une tâche deferred ne peut pas être achevée. Aucun mécanisme de clôture d'un audit externe n'est inventé ici. La mission n'est donc pas déclarée entièrement débloquée sur ce point.

## Références et exécution réelle

- Référence lue : `C:/Users/djden/source/repos/opencode-ensemble-session/docs/task-publication.md`.
- Skill rechargé depuis le paquet actif `20261028-task-publication-a9868de8ce5c`.
- Déploiement et validation du plugin annoncés par maintenance : commit `b7aff2a0546818f9193f8dba4064e758f768193a`, schéma 30, typecheck/build et 1828 tests réussis. Ces tests ne sont pas réexécutés ni attribués à ce superviseur.
- Appels de cette correction : découverte de catalogue, lectures natives avant/après, un appel atomique `team_tasks_add`, assertions de relecture. Les sorties longues avant/après ont été indexées dans Context Mode.
- Aucune modification des sources Linux, profils, permissions, modèles/politique, plugin ou base interne ; seul ce rapport est persisté dans le projet en plus de l'opération native sur le board.

Le lot de maintenance est terminé. Aucun start ni travail Linux ne suit cette restitution.
