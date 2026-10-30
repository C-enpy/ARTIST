# 91 — Préparer le plan QA du transport privé Windows

Le collecteur étant effectivement intégré, le prochain changement peut être planifié : transmettre explicitement le bundle à la préparation et conserver un petit payload privé Windows attribué avant chaque commande. Les descriptions et modes consultatifs doivent rester sans effets ; aucun auteur ne reçoit encore l'autorisation d'implémenter ce delta.

Le coordinateur a vérifié le profil qa-planner effectivement servi à la location ARTIST via l'API native : mode subagent, lecture/recherche autorisées, édition/shell/sous-agent refusés, restitution permise. Aucun droit n'a été modifié ; ce contrôle ne prouve pas une sandbox. Le brief local fournit le candidat, les preuves et les extraits nécessaires, sans imposer une lecture externe au profil restrictif.

Admission publiée après commit des entrées `493bf4f73a8ba5f8f1b7350deb7aaab09fcd1b3b` : tâche `task_mvujeqpu_004m_3sfeit`, contexte fresh, worktree false et final_response explicites. Le contrôle natif suivant confirme pool-21 effectivement routé qa-planner, sans worktree. Sa première lecture et sa restitution de ce nouveau livrable restent à recevoir.

Premier message reçu de pool-21 : lectures AGENTS/skill/brief/binding/plan/code réussies, aucune écriture ou test. Il signale un `File not found` unique pour `qa-inputs.md`, sans retry. Le lead constate que ce lien annoncé dans son brief est erroné et corrige `qa-bundle-inputs.md` : aucune restitution intégrale du corpus pool-5 n'est prétendue ; les exigences/scénarios déjà persistés dans `qa-plan.md` et leur digestion bornée sont fournis pour ce delta. Le lecteur n'a pas à rechercher un chemin externe ni recommencer son analyse. Le plan final complet reste attendu ; la réception n'est pas déduite du seul message annonçant sa préparation.

La tentative de différer nativement l'ancienne tâche core a été refusée : ce contrôle exige une tâche jamais admise. Le statut n'a pas été changé et aucune complétion fictive n'est fabriquée. Ce refus n'est pas réessayé ; la barrière d'écriture et le besoin d'un nouveau brief explicite ont été transmis par la voie d'équipe. Toute reprise historique doit rester inactive sur le delta tant que le plan QA n'est pas commité.

La reprise historique est apparue sur pool-20. Sa restitution `result_mvujds60_004k_0zc5nk` confirme la barrière, aucune écriture ni exécution, et un worktree propre à `8ddb85e`. Il reste inactif jusqu'au plan QA commité et au nouveau point Git auteur explicite ; la création automatique de ce worktree n'est pas présentée comme une autorisation d'y développer avant cette barrière.

## Références techniques

- Entrées locales : `.opencode/evidence/mission-uniform-pipeline/qa-bundle-inputs.md`, décision `windows-adapter-binding-v1.md` ; HEAD de réception `8ddb85e508a501c12c67d400319e687e735c7c08`.
- Diagnostic : le binaire desktop ne rendait aucun JSON sur la commande API ; aucun refus de permission. Le CLI réellement découvert sous `resources/opencode-cli.exe` rend ensuite OpenAPI puis `agent.list` avec `location[directory]` ARTIST. Seuls location, mode et permissions utiles ont été remontés via Context Mode ; aucune session ou base interne modifiée.
- Planification QA : PM/LLM locale, complexité brute/retenue60, risque65, incertitude moyenne ; lecture seule, pas développement ou modification de grille.

Aucun nouveau test, build, moteur, OpenGL, rendu, benchmark, installation ou cloud exécuté par ces contrôles d'admission. Le plan reste à produire puis à commiter avant auteurs.
