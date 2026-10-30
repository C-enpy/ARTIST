# 98 — RED Windows confirmé, localisation de CTest ratifiée

Le pipeline doit décrire des commandes Windows attribuées sans deviner les outils
installés. Le RED déjà exécuté sur données synthétiques donne **28 tests et
25 échecs d'assertion avec sous-cas**, aucune erreur ou skip ; empreintes stables.
Les causes sont le scaffold non implémenté et les paramètres privés non déclarés.
Helper, 51 anciens contrôles et pins restent inchangés. Aucun test n'est relancé.

L'avis d'architecture est reçu et retenu : retrouver CTest depuis les deux entrées
du cache CMake du même build, déjà empreinté dans le reçu. Lire une seule fois,
vérifier puis parser les mêmes octets ; refuser ambiguïtés et chemins incohérents.
Cela évite un chemin supposé dans le venv ou un choix implicite via PATH, sans
nouveau schéma. Le hash du cache ne qualifie pas le binaire outil lui-même.

La prochaine étape Windows reste **des tests supplémentaires avant correction** :
négatifs du cache et preuve de lecture unique. Le helper complet reste fermé.
Son ouverture attend son propre mapping et des tests fonctionnels de fragments
avant implémentation. La tranche Linux indépendante reste autorisée en parallèle.

## Preuves et limites

- Candidat RED `f14ea7115394c2a5e072ee402823b9063cfb3007`, auteur pool-23,
  exécutant coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest -v test_adapter_binding test_binding_helper_interface`, cwd `scripts/tests/verification_windows` du worktree auteur.
- Logs : `out/verification-synthetic/windows-binding-red-2026-10-30T06-41-49-105Z/` ; manifeste relu, statut1, empreintes avant/après identiques.
- Consultation `result_mvulud8x_0000_07pkef`, lecture seule sans test ; décision persistée dans `windows-adapter-binding-v1.md`.

Aucun PowerShell, helper complet, MSVC, Conan/CMake/CTest réel, moteur, rendu,
benchmark, installation ou cloud exécuté. Windows/Linux natifs et GPU NON_EXECUTE.
