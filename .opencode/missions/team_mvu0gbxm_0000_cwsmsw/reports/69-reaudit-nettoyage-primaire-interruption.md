# 69 — Ré-audit : un dernier chemin d'interruption à couvrir

## Levées bornées

La revue indépendante confirme statiquement la suppression de l'ancien mécanisme pipes/lecteurs Windows, la capture par fichiers et la persistance des sorties/statuts avant erreur. Elle lève également le défaut de population du build conformément au domaine CURRENT ratifié. Aucun contrôle global des fichiers générés n'a été ajouté.

## Résidu bloquant

Une interruption peut survenir après affectation du primaire à `child`, avant l'entrée dans le bloc d'attente protégé. Le handler extérieur enregistre alors l'interruption, puis le `finally` capture et ferme les fichiers sans tenter l'arrêt ou la récolte du primaire. Celui-ci peut continuer après le retour. La même obligation s'applique à la fenêtre entre un handler d'attente et la boucle d'arrêt.

Le coordinateur a recoupé ce flux dans le code. Il s'agit du nettoyage du primaire déjà possédé, pas d'un arrêt de descendants ni d'une extension d'objectif. Les tests actuels déclenchent les interruptions dans les appels protégés et ne détectent pas ce chemin.

## Suite

L'auteur doit remettre un test déterministe avec faux primaire et interruption ponctuelle après acquisition, sans processus réel ni signal OS. L'injection doit être bornée et restaurer toute instrumentation préexistante. Le RED sera exécuté avant correction ; les 143 assertions et mappings acquis restent conservés.

L'intégration du cœur reste bloquée sur ce résidu. Les autres branches continuent. Aucun nouveau test, moteur, compilation, rendu, benchmark, installation ou cloud exécuté lors de cette revue.

## Preuves

Avis `result_mvufdejj_0000_enwt6h`, candidat `258a9cbc5523846b42e99a75d6facfc80e19ed2c`. Localisation : `runner.py:83–91,126–165`. Dernières preuves exécutées inchangées : rapport 67, neuf tests ciblés satisfaits puis 141 succès et deux symlink non exécutés sur 143 tests. Aucune exécution ou édition par le reviewer.
