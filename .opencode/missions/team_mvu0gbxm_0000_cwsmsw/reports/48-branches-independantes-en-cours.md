# 48 — Corrections du cœur et contexte historique en parallèle

Le contrat du contexte historique et son addendum QA sont committés au HEAD `50af7923ff9094ac8e7a175d3256126204b4da41` avant la nouvelle branche auteur. L'architecte et le planificateur QA ont remis leurs livrables en lecture seule ; leurs tâches documentaires sont intégrées et terminées.

Le producteur/parser de contexte est admis sur le profil GPU/OpenGL prévu, en worktree isolé et contexte fresh. Son ownership est limité à la fixture commune, au seul arrêt dérivé nécessaire, au parser pur et à ses tests dédiés. Les corps de tests, assertions, moteur et CMake restent exclus. Le premier livrable est le RED du parser, exécuté uniquement par le coordinateur après revue.

En parallèle, le propriétaire du cœur prépare les régressions des quatre défauts indépendamment établis avant correction. La branche Windows continue ses fonctions et transports factices ; Linux attend les interfaces communes réellement intégrées. Le nouveau producteur n'autorise pas ces consommateurs à copier une branche non intégrée.

Les trois tâches d'intégration globale, gaps et audit final restent différées. Aucune fin d'auteur ni test partiel n'est transformé en livraison du pipeline complet. Les scénarios intégrés conservent leurs 31 tests verts et leurs limites SOURCE_ONLY ; le cœur conserve 124 succès et un symlink non exécuté, avec intégration bloquée par ses défauts ouverts.

Admission observée : `pool-10`, profil `gpu-engineer`, tâche `task_mvucvx2k_0020_a3xma2`, worktree natif isolé. Scores PM/LLM bruts/retenus 78/72, plancher développement respecté ; modèle non forcé et politique native inchangée. Aucun moteur, build, rendu, benchmark, installation ou cloud lancé.
