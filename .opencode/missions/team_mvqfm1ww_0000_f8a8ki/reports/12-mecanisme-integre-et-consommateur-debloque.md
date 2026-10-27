# 12 — Accès float intégré, preuve du consommateur graphique à produire

## Objectif et étape

Préparer un uniform float une fois puis le modifier sans recherche répétée est
maintenant disponible dans la branche principale. La fusion native du mécanisme
a réussi et les douze fichiers intégrés correspondent exactement aux empreintes
de la livraison contrôlée. Le consommateur graphique peut donc être publié.
Il doit encore démontrer l'usage réel, les rejets et les pixels attendus ; la
mission n'est pas terminée et aucun gain de performance n'est annoncé.

## Résultats vérifiés et limites

- Auteur arrêté effectivement, candidat committé et propre avant fusion.
- Archives finales et manifeste copiés dans le dépôt du lead ; quatorze artefacts
  recoupés par empreinte, sans suppression du worktree auteur.
- Preuves auteur déjà recoupées : 575 tests normaux (502 CPU, 73 OpenGL) et 575
  ASan, mêmes ensembles de noms, aucun failure/error/skip. Les cinq commandes
  convenues sont enregistrées avec sortie zéro. Pas de nouvelle exécution par
  le lead : ce sont des résultats auteur, pas une campagne finale intégrée.
- Douze empreintes source identiques après fusion, contrôle de whitespace et
  commit sélectif réussis ; sources possédées propres après commit.
- RED initial causal conservé, mais plusieurs tests d'invalidation ajoutés après
  les gardes : limite TDD explicite, pas une preuve RED rétroactive.
- Consommateur, pixels, contrôles discriminants C/L/A, revue indépendante,
  complément de tests, audit et photographie documentaire restent à faire.

## Incident d'intégration traité sans perdre les changements étrangers

Le premier appel natif a refusé l'index contenant un renommage étranger .grepai.
Aucune source moteur n'a changé lors de ce refus. Le lead a sauvegardé le patch
d'index et ses entrées, puis désindexé seulement les deux chemins concernés,
sans modifier leurs fichiers. Le second appel natif a intégré les sources.
Le patch d'index a ensuite été restauré et comparé exactement, ainsi que les
octets de travail. Le commit moteur n'inclut aucun changement étranger.

L'outil signale aussi que le retrait du worktree auteur a échoué après la fusion.
Les branches sont conservées ; aucune seconde fusion ni nettoyage forcé n'est
nécessaire à la suite. Ce reste administratif n'est pas un défaut du moteur.

## Suite autorisée

Publier uniquement la migration de target (decoy conservé par nom), le test pixel
et l'exemple borné. Le même auteur garde son cycle TDD et le bail graphique
exclusif. Les revues et propositions de gaps attendent l'intégration de cette
deuxième tranche, puis seront parallèles en lecture seule. Cette séquence dépend
réellement de l'API et du candidat intégré, pas d'un nouvel accord utilisateur.

## Annexe technique

- H1 intégré : `8b60b57898b28a7ceb7b5d4434aadcd4684581b9`.
- Auteur : `1a287564a48bb3e81873c6a0114fb19d51782636`, base H0 `49005ed2`.
- Producteur réel : `task_mvqn5vg6_0003_gblyx2`.
- Archive : `out/qa-prepared-float/1a287564-delivery/` ; manifeste SHA256
  `d2667e78a44b96cafe2aecc957d6ae37c8935546089acf1a200a93535f28c34e`.
- Sauvegarde index : `out/qa-prepared-float/integration-index/`.
- [Contrat adopté](../contract.md), [plan QA actif](../qa-plan.md),
  [graphe prévu](../task-preview.md). Ces deux derniers historiques de préparation
  ne remplacent pas le présent état d'intégration.
