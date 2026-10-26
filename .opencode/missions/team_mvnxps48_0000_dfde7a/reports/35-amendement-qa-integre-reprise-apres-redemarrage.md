# 35 — Amendement QA intégré ; préparation de la calibration

2026-10-26. Reprise après redémarrage, sans répéter les essais acquis.

## Intégration vérifiée

Commit **fb3c9b9ff126f55a4e41955c23b4b09aa0bae84c** : amendement ciblé au plan QA17 existant, paquet d'entrée QA et rapports32/33/34. Proposition pool-6 reçue en cinq fragments, intégrée par le lead avec les deux décisions explicites opérateur. Q complétée seulement après ce commit.

Commandes réellement exécutées : git rev-parse HEAD ; git diff --name-only -- ARTIST (vide) ; git diff --check sur les cinq chemins retenus (exit0) ; git add ciblé et git commit --only sur ces chemins ; git diff --cached --name-status. Le rename GrepAI concurrent reste seul staged, non annexé. Aucun nouveau test/build/ASan/GPU, aucun push.

Context Mode a de nouveau exposé ctx_execute après une absence transitoire du catalogue ; appel réel réussi. Aucun redémarrage/réinstallation effectué. Notification pool-5 consultée une fois : aucun message non lu retourné, pas de relance ni travail B1 répété.

## État et suite

- B5/C2 : fermeture administrative motivée explicitement acceptée au34, sans reconstruction complète ni verdict moteur ; limites et déclencheurs de réouverture conservés.
- B1 : témoins causaux29/30 conservés, revue finale attendue ; aucun sweep.
- B3 : calibration stdio et recherche close bornées autorisées ; contrat Reader non adopté, catch/close authentiques non prouvés. Aucun auteur de sonde/Reader admis.
- P-CAL task_mvpd9ish_0000_2jk9hn auto-affectée à pool-7/project-manager : actualisation RO du graphe, scores et ownership, avant publication des deux branches indépendantes. Source82bf52d, HEADfb3c9b9, QA17 commité fourni.

Barrières : intégrer le graphe/contrat de sonde, puis auteur de sonde et recherche OS close en parallèle sans fichiers partagés. Le lead valide les commandes exactes et exécute les essais seulement après intégration du livrable. L'adoption d'un futur correctif Reader et la disposition d'obligations non démontrées restent des décisions distinctes. Aucun PASS global, aucune nouvelle portée Linux/GCC/Release/image/performance/backend.

## Complément admission P-CAL

Une demande effective de pool-7 attendait confirmation du snapshot. Contrôle natif lead : session ses_e87558e7affeOtee8XOFUkxYPZ, agent project-manager, parent lead attendu, location canonique ARTIST (pas worktree isolé). Contexte commence par l'affectation propre à cette tâche, sans histoire auteur. Git dans cette location : HEADfb3c9b9, diff HEAD -- ARTIST vide. Chemin réel de politique team_mvnxps48_0000_dfde7a/model-policy.json fourni ; la différence avec teamID n'est pas une politique absente. Lecture et transport déjà observés ; GO finalisation RO envoyé dans la même session, aucun bootstrap ni droit élargi. Les notifications des producteurs déjà intégrés ne sont pas des livraisons nouvelles.
