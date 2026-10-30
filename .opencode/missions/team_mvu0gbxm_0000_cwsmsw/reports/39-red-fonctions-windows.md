# 39 — Fonctions intermédiaires Windows : RED causal confirmé

## Résultat

Après revue des tests, du harnais PowerShell et de l'enfant Python factice, le coordinateur a exécuté les onze méthodes : neuf sont rouges, soit **28 échecs d'assertion avec sous-cas** ; deux passent. Aucun error ou skip. Les échecs proviennent des refus provisoires des fonctions à implémenter, pas d'une panne d'environnement ou du harnais.

L'autotest lance correctement le petit enfant Python fini et conserve son code 17. Les contre-exemples d'effets sont rejetés. Les fonctions d'environnement manipulent uniquement des maps ; les commandes Conan/CMake de composition sont enregistrées comme données, jamais lancées. Les empreintes restent stables.

## Suite autorisée

L'auteur implémente les six fonctions logiques : validation privée, encodage d'arguments Windows, copie/restauration d'environnement, contrôle des métadonnées simulées et conservation du code enfant. Les tests et audits restent inchangés. Les primitives OS, MSVC/vcvars et le helper complet ne sont pas autorisés à l'exécution ; leur raccordement reste à livrer.

Le transport devra préserver les limites publiques des cibles Unicode et contrôler la longueur après encodage à sa frontière Windows, sans confondre scalaires, octets et unités UTF-16. Les tests actuels ne démontrent pas ces cas limites natifs.

## Preuves

Candidat `b484f026b2a715c868b1b29b7a80fde23371a56e`, auteur windows-pipeline, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_wrappers.py -v`.

Journaux et empreintes : `out/verification-synthetic/windows-wrappers-red-2026-10-30T00-44-11-968Z/`. Données de fixture dans le worktree : `out/verification-synthetic/windows-wrapper-fixtures/20261030T004402Z-9f6660b3c8fd498b8a42c7115dd4826a/`.

Aucun moteur, outil de construction réel, installation, rendu, benchmark ou cloud lancé. La chaîne Windows complète n'est pas encore intégrée.
