# Isolation de l'intégration — décision et provenance

Date : 2026-10-16. Choix utilisateur : « Isoler l'intégration (Recommended) ».

## Motif et périmètre

Une activité externe a ajouté sur main les commits 3c670c9/407c578/f8a18a8 et
79596ca (A1c/Reader/subst). Des modifications Reader.hpp/ReaderTests.cpp sont
également apparues dans le répertoire canonique. Elles ne sont ni les livrables
ni les preuves de cette mission ; elles sont préservées, jamais attribuées ici.
Aucun nettoyage des subst/fixtures de cette autre activité n'est autorisé par
la lecture d'un message de commit.

Nouveau worktree créé par opencode.worktree_create, puis session_move natif :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/gpu-qa-integration`.
Base propre observée : `79596ca1fcf7a605eab956a2b9b940739add1cf9`.
Branche propre à la mission : `mission/gpu-qa-integration-e52f73a` (créée sans
écraser une référence). La mission/lead, politique approuvée v1 et ses restrictions
restent rattachées ; aucune nouvelle équipe ou politique legacy créée.

## Transfert contrôlé

T06/T07/T08/T09/T12 avaient déjà été shutdown/merge via team_merge dans le
répertoire canonique. Leurs 44 fichiers ont été comparés aux blobs producteurs,
44 identités vérifiées et aucune collision d'allowlist. Le nouveau worktree
propre n'avait aucune différence commise sur ces chemins depuis la base auteurs.
Le transfert a restauré UNIQUEMENT ces blobs livrés dans le nouveau worktree,
sans copie des sept diffs utilisateur et sans écrire l'index/main canonique.

Producteurs :

- T06 : f4d6a66d6ebe0aadae1b2453f24821c0da0b77f3 (7 chemins).
- T07 : 0c5687d9699bb0accd12e80349fb36ffab70a960 (7).
- T08 : 8101ad3fdc3a849e8f10a069e1a36826d7f6e519 (7).
- T09 : 58c6c73ce7d88bcbce3d5ee5e214e12c17e9141d (9).
- T12 : 3f857ce6c59a25a6dcdb65e6929b709bfa9e48ff (14).

Les 68 tests locaux de ces cinq lots ont été exécutés par le lead sur le candidat
canonique avant déplacement, exit0, 68 pass/0 fail/0 skip. Node 24 a produit le
reporter spec : le premier filtre de résumé TAP a rendu une liste vide, puis
lecture du log a confirmé les 68 comptages. Pas zéro test ni faux PASS. Logs
initiaux : `out/agentic/mission-gpu-qa/batch-01-local.*` du répertoire canonique.
Relance sur le worktree déplacé encore requise pour sa preuve finale.

Après déplacement, team_merge(T04) a créé le validateur UNIQUEMENT dans le
worktree d'intégration ; absence confirmée dans le répertoire canonique et
indexes des deux worktrees vides. Destination native ainsi vérifiée, pas présumée.
T10/T11 ont ensuite été intégrés par team_merge dans ce worktree.

- T04 : 8ca29d1898b2ceba4c1a64c0bcc67a1a05d3296c (6 ajouts).
- T10 : 792e14a41ee8262269e351e5d6aabf9befb473b7 (13).
- T11 : 460e5db puis 314751e6509d6d0567f6842238774cca7f108db9,
  deux commits dont le DELTA COMPLET depuis 40a7cc3 porte sur 14 fichiers.

## Incident de références détachées

Les commits T07/T12/T11 existaient avec worktree propre et base vérifiée, mais
la référence de branche enregistrée par Ensemble était absente. team_merge a
signalé « not something we can merge », pas un conflit de contenu. Le lead a
créé SEULEMENT chaque référence manquante possédée par la mission vers le commit
exact vérifié, sans écraser de ref, puis réutilisé team_merge. Aucun git merge
manuel, reset global ou perte des livrables. Les autres branches préservées par
shutdown ont fonctionné directement.

## Suite et limites

Les copies des 44 fichiers sur main ne seront retirées qu'après sauvegarde
commise dans l'intégration et contrôle d'identité avant chaque restauration/
suppression sélective ; les entrées utilisateur/externe ne seront pas touchées.
Les fichiers partagés T13 nécessiteront toujours leurs snapshots S et delta S→A,
pas une reprise aveugle des sept diffs utilisateur ni leur attribution au lot.

Les tests contractuels locaux ne prouvent ni sélection native, permissions,
GPU, image ou performance. Les critères manquants demeurent INCONCLUSIVE jusqu'à
leurs observations et revues indépendantes. Aucun commit moteur externe n'est
une preuve de cette mission GPU/QA.
