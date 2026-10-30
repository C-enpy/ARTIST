# 26 — Dispatch Windows vert sous outils factices

## Résultat

Le coordinateur a relu la nouvelle région de dispatch et confirmé que les tests et l'audit du harnais sont inchangés. Les **10 tests comportementaux passent**, puis les **5 contrôles d'interface passent**, sans error ni skip. Les empreintes des sources contrôlées restent stables.

Les tests exercent les phases isolées, le refus de préparation sans consentement, la transmission des arguments, les codes enfants et la restauration d'une map d'environnement. Ils exécutent uniquement le fragment extrait dans le programme de test PowerShell déjà relu.

## Prochaine tranche et limites

Le helper réel conserve son refus préalable : ses fonctions de lancement et d'accès à l'environnement MSVC ne sont pas encore raccordées. Le candidat ne livre donc pas encore une chaîne Windows complète. L'auteur doit proposer les tests et le confinement de ces frontières avant leur implémentation, sans lancer le helper complet ni un outil réel.

L'adaptateur Python attend les interfaces communes intégrées. Aucun moteur, compilateur, installation, rendu, benchmark ou cloud lancé. Ce GREEN ne prouve pas le fonctionnement réel de MSVC.

## Preuves

Candidat `d47855057789cc62392e1ca7f74b2077ea6616a2`, auteur windows-pipeline, runner coordinateur. `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_dispatch.py -v`, puis même commande avec `test_helper_contract.py`.

Logs et manifeste : `out/verification-synthetic/windows-dispatch-green-2026-10-30T00-02-40-522Z/`. Données des cas dans le worktree : `out/verification-synthetic/windows-dispatch-fixtures/20261030T000240Z-e28d9dcd507946a1ab72d86f60debf35/`.
