# 121 — Corriger le harnais avant de chercher le RED du helper

Le coordinateur a relu le mapping de l'ancienne garde, l'extracteur, les pins,
le programme PowerShell test-owned et les tests. Seule la classe des préconditions
du harnais a ensuite été exécutée, conformément à la barrière annoncée.

Résultat : **7 tests, 3 échecs avec sous-cas**, aucune erreur ou skip ; empreintes
stables. Les baselines des codes0/17 et de l'exception factice sont bloquées par
une variable `$script:fixtureEvents` non définie lors de l'appel depuis le script
extrait. Les autres contrôles d'extraction/pins/audits réussissent. Les événements
restent vides et l'environnement synthétique intact dans les cas concernés.

Ce défaut de scope du programme de test **ne constitue pas un RED de production**.
Le test prepare nominal n'a pas été lancé. Le helper complet et son code restent
inchangés et fermés. Une correction de fixture seulement est autorisée, avec
nouvelle hypothèse explicite sur la liaison de son état entre scopes ; elle ne
doit ni injecter root/tooling dans l'entrée ni affaiblir les assertions.

Les baselines devront passer avant la seule expérience comportementale prévue.
Toute nouvelle source de fixture sera relue avant exécution par le coordinateur.
Aucune permission de tester ou d'implémenter le produit n'est donnée à l'auteur.

## Preuves

- Candidat `f43b38fe78bfbdb4aa7edbda07d31149a4f35059`, auteur pool-31 ; runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalHarnessTests`, cwd tests Windows du worktree auteur.
- Logs/manifeste lead : `out/verification-synthetic/windows-helper-functional-baseline-2026-10-30T08-13-41-983Z/`.
- Cas bruts dans le worktree auteur : `out/verification-synthetic/windows-helper-functional/20261030T081342Z-2adbc39b1a454516ac7981ff401966f1/`.

Le seul processus externe lancé est PowerShell avec le programme de test relu,
ses fragments pinés et ses feuilles record-only. Aucun helper complet, outil de
build, enfant moteur, GPU, rendu, benchmark, installation ou cloud exécuté.
