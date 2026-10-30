# 40 — RED de l'orchestration complète observé

## Résultats

Le coordinateur a relu les tests, les modules intermédiaires, l'adaptateur/runner factices et le petit script enfant possédé. Deux suites ont été exécutées sur le candidat :

- Cible contenant des espaces : **15 tests, 12 échecs d'assertion** avec sous-cas. Le rejet trop strict du modèle est reproduit sans erreur d'import.
- Orchestration complète : **49 tests, 93 échecs d'assertion** avec sous-cas. Le runner vide, les phases `NOT_STARTED`, la CLI vide et la réconciliation permissive expliquent ces résultats.

Aucun error ou skip ; empreintes contrôlées stables. Beaucoup de contrôles situés après la préparation s'arrêtent encore à l'assertion de préparation absente : leur sensibilité individuelle n'est pas démontrée par ce premier RED. La création des symlinks n'est pas atteinte ; zéro skip ne prouve donc pas cette couverture.

## Suite autorisée

L'auteur implémente le chemin runtime minimal cohérent et aligne le modèle sur les cibles non vides. Les 125 tests et leurs assertions sont conservés. Le candidat GREEN sera relu puis exécuté par le coordinateur ; les gaps et la revue indépendante porteront sur les livrables réellement intégrés.

Les tests utilisent uniquement des fichiers temporaires possédés, des callbacks et des processus Python factices. Les adaptateurs réels et le parseur scénarios ne sont pas copiés dans une branche isolée : leur composition sera vérifiée dans l'arbre intégré. Un refus de précondition code20 devra rester distinct d'une panne de protocole, en conservant les résultats bruts.

Aucun outil moteur, CMake, Conan, CTest, GL, rendu, benchmark, installation ou cloud exécuté. Ce RED n'est pas un fonctionnement global du pipeline.

## Preuves

Candidat `7bd17a06f855de43e95de94b3d2d442c7462058f`, auteur pool-3, runner coordinateur. Commande `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_hardware_contract.py -v`, puis même commande avec `test_runtime_contract.py`.

Logs complets et empreintes : `out/verification-synthetic/core-runtime-red-2026-10-30T00-58-40-124Z/`.
