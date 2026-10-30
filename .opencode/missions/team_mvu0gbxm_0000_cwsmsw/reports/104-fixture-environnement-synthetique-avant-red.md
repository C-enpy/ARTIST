# 104 — Préserver la confidentialité des journaux RED

Les dix tests candidats du runner sont reçus et relus, mais pas exécutés.
Le faux processus et les captures restent en mémoire/fichiers possédés. Une
construction de fixture copie toutefois l'environnement réel dans le parent
testé, avant des comparaisons de dictionnaires complets. Un échec unittest peut
imprimer les différences et conserver des valeurs sensibles dans les logs.

Le coordinateur demande donc de remplacer cette entrée par un dictionnaire
entièrement synthétique avant exécution. Aucun affaiblissement des assertions
d'environnement effectif ou de parent intact : comparer le parent synthétique
complet suffit, puisque les processus sont tous factices. Aucun besoin des
variables réelles de la machine n'est établi dans cette fixture.

Seule la préparation de fixture et sa note peuvent changer ; les dix méthodes,
le runner et les anciens tests restent inchangés. Le RED reste NON_EXECUTE,
pas une panne d'environnement ni un échec de production. Aucun secret n'a été
imprimé par une exécution de ces tests : celle-ci n'a pas été lancée.

## Références

- Candidat relu `0a60636482b68e99132bd2819c7d8df27864d4ef`, auteur pool-26.
- `test_hardware_child_environment_contract.py::parent`, lignes68–69 ;
  `assert_child`, lignes135–145 ; note RED_READY adjacente dans evidence.
- Convention de refus proposée retenue pour le futur correctif : ValueError
  attribuable à la clé fautive, avant factory ; aucun nouveau type/code public.

Aucun test, enfant réel, moteur, outil natif, GPU, rendu, benchmark, installation
ou cloud exécuté pendant cette réception.
