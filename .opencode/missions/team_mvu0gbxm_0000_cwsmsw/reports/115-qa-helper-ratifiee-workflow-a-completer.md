# 115 — Plan QA du helper ratifié, workflow conservé au mandat

Le complément QA complet est reçu et relu. Le coordinateur ratifie au §13 les
contrôles du helper prepare/build et des parents CMake : admission avant effets,
chaîne source réelle sans root caché dans fixture, phases séparées, restauration,
codes/bruts, option OFF et inclusion indépendante des tests.

La proposition de supprimer le nouveau workflow n'est pas retenue : il figure
explicitement au contrat ratifié. Un complément limité à ses contrôles manuels
est demandé, sans campagne automatique ni machine cloud. Le workflow attendra ce
plan commité ; le helper indépendant peut commencer ses tests après le présent
commit. Aucune nouvelle demande utilisateur n'est nécessaire pour respecter le
contrat déjà approuvé.

La reprise Windows pool-30 confirme un worktree propre à l'entrée intégrée et
aucune écriture ou exécution. L'ancien brief de HEAD est périmé ; il ne sera pas
restauré. Après ce commit, le lead pourra aligner proprement le worktree existant
et donner un mandat explicite limité aux nouveaux tests helper. Pas de seconde
réimplémentation d'adaptateur ou de garde supprimée sans RED.

## Références

- Plan proposé `result_mvuo139f_0002_rccy4h`, qa-planner/pool-21, lecture seule.
- Reprise inactive `result_mvuny606_0064_xts2q7`, pool-30, HEAD `a7aacb9` fourni
  et vérifié par l'auteur ; aucun candidat de code nouveau.
- Plan ratifié `qa-plan.md` §13 ; tâches restantes au rapport114.

Aucun test, helper complet, outil natif, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté lors de cette réception.

## Admission du seul helper après commit

Plan commité `2b836e82e5827ffe3c8aa79f88fb4ad9080ba0cf` avant publication de
`task_mvuo974b_0069_duptzs`, build-engineer fresh avec worktree, tests RED seulement.
Les scores de cette nouvelle tranche sont ceux validés par le PM :78/68.
L'ancien worktree propre pool-30 reste inactif ; il n'est finalement pas aligné
ou réutilisé pour contourner l'évaluation de la nouvelle tranche. Le transfert
d'ownership du helper au nouvel auteur unique est explicite. Les anciennes tâches
restent des suivis, sans permission de reprendre une source concurrente.

La branche CMake/workflow attend son complément QA manuel propre ; ce délai n'est
pas une dépendance technique au helper. Les deux branches seront parallèles dès
que cette entrée sera reçue et commise.
