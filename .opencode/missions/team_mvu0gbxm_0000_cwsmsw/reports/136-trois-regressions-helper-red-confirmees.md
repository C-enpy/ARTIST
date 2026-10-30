# 136 — Trois régressions du helper reproduites avant correction

Après relecture des programmes et du diff, les **cinq contrôles de harnais passent**.
Le coordinateur exécute ensuite les trois régressions : **trois tests, cinq échecs
avec sous-cas**, aucune erreur ou skip, empreintes stables.

Les causes sont recoupées :
- Le séparateur final du VSINSTALLDIR exporté entraîne le refus MSVC, sans erreur
  de harnais, après les seuls rôles python-pip et msvc-env simulés.
- Prepare émet PRE_TEST sans type ; son cache synthétique explicitement dérivé
  UNINITIALIZED est refusé par build avant observation ou processus. Le cache
  STRING valide et les négatifs sont vérifiés séparément dans les préconditions.
- La vraie boucle source avec Keys masque les entrées et produit une copie
  incorrecte, puis un filtre vide. Le snapshot synthétique reste inchangé ; aucun
  snapshot réel du parent ni observation de processus n'est exécuté par cette sonde.

Le GREEN minimal est autorisé sur ces trois sites : comparaison des répertoires
MSVC équivalents, production explicite STRING=PRE_TEST, et accès non masquable
à la collection des clés. Les nouveaux oracles restent inchangés. Les hashes
modifiés devront être relus et admis explicitement par le coordinateur.

## Mapping ponctuel d'un ancien oracle

`test_helper_functional.py:359` exige actuellement l'argument PRE_TEST sans type.
Sa garantie est le mode PRE_TEST uniquement pour une sélection de tests.
Remplacer ce seul littéral attendu par `-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING=PRE_TEST`
conserve cette garantie et exige la représentation correcte du cache. Cette
adaptation précise est ratifiée avant édition ; aucun autre oracle n'est affaibli.

## Preuves et limites

- Candidat tests `de752f8fc9044d759a79209114f4bb21fe4656ac`, propre, source et
  pins inchangés. Auteur pool-31 ; exécutant coordinateur.
- Commandes `C:/Python313/python.exe -B -m unittest -v test_review_regressions.ReviewRegressionHarnessTests`, puis `test_review_regressions.ReviewRegressionSourceTests`, cwd tests Windows du worktree auteur.
- Journaux/manifeste : `out/verification-synthetic/windows-helper-review-red-2026-10-30T11-21-13-861Z/`. Les stdout pointent les cas bruts ; le rapport MSVC du cas à séparateur final a été recoupé séparément.
- Modèle de cache fini déclaré, pas une exécution CMake. Les cinq échecs ne sont
  pas cinq défauts indépendants : deux régressions observent deux frontières.

Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté. Encodage non ASCII et qualification native restent
non établis. Ré-audit indépendant requis avant intégration.
