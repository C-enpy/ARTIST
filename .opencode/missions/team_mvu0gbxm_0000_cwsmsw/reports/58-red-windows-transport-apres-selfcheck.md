# 58 — RED Windows confirmé après auto-test du harnais

## Résultat vérifié

Après revue du nettoyage borné, le coordinateur a exécuté l'auto-test seul : il passe. Il confirme le refus d'un fragment non revu, le lancement de l'enfant Python factice, son code 17 et la conservation d'une variable d'environnement vide. Aucun échec de précondition du harnais observé.

Les treize autres méthodes autorisées donnent ensuite **38 échecs d'assertion avec sous-cas**, aucun error ou skip. Les quatre refus provisoires expliquent les échecs attendus. Les empreintes sont stables et les tests, le programme factice, le pin et le produit étaient inchangés avant ce RED.

Le cas de jonction n'a pas été sélectionné : il reste NON_EXECUTE, distinct des symlinks non exécutés du cœur. Le premier RED n'atteint pas encore toutes les assertions de capture et timeout après les refus provisoires ; il ne prouve pas leur sensibilité individuelle.

## Suite bornée

Le GREEN minimal des quatre fonctions est autorisé. L'auteur doit conserver les tests et remettre le nouveau code ainsi que son pin pour revue explicite avant exécution. Aucun pin recalculé ne remplace cette revue. Le helper complet reste bloqué ; aucune consommation d'interfaces centrales non intégrées.

Les fonctions doivent lancer seulement l'objet Process possédé et validé, garder les délais bornés et conserver les sorties disponibles. Les essais sur l'enfant fini ne prouvent pas la gestion d'un arbre réel de processus. Aucun MSVC, vcvars, outil moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.

## Preuves

- Candidat `77900399fb9f18bc9f75d17904f8f8195b138321`, auteur windows-pipeline, runner coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest -v` suivi du nom complet de l'auto-test, puis des treize noms complets autorisés ; listes exactes conservées dans le manifeste. Cwd : dossier `scripts/tests/verification_windows` du worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/windows-native-selected-red-2026-10-30T03-03-59-434Z/`.
- Données détaillées des cas dans le worktree : dossiers `windows-native-fixtures/20261030T030359Z-05c03aa343944edf95dda8ce3b3d246f` et `20261030T030400Z-48c3ce0866ed4b33948a097509de520f`.
