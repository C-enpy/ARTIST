# 138 — Trois corrections vertes, ré-audit demandé

Le coordinateur a relu la branche nominative du harnais : seule la valeur
VSINSTALLDIR sélectionnée, synthétique et confinée est acceptée. Production,
pins et assertions sont inchangés depuis la correction des trois défauts.

Le cas MSVC passe désormais. Puis **5/5 contrôles et 3/3 régressions passent**,
ainsi que **6/6 baselines natives, 20/20 tests source, 7/7 baselines haut niveau
et 11/11 admissions**. Aucun échec, erreur ou skip ; empreintes stables. Le cas
MSVC nominatif est inclus dans les trois régressions, pas un scénario distinct
à additionner.

Les preuves RED et GREEN, la source corrigée et le petit ajustement de harnais
sont transmis au reviewer initial pour ré-audit ciblé des trois findings. Le
candidat reste stable et non intégré. La couture avec le collecteur intégré et
le mapping des anciens tests restent à terminer avant la QA finale.

Ces résultats portent uniquement sur les fragments source et doubles synthétiques.
L'encodage cmd des chemins non ASCII n'est pas établi ; le gap Dispose/writers
hérités reste inchangé. Aucun fonctionnement natif ou moteur n'est démontré.

## Preuves

- Candidat `9680f2d9c47e69ddd538e9c01044ac8423d4a304`, source `ead7f8374206062892dfb752626ffc1e76ac3dfc`, admissions lead7fcee245.
- Auteur pool-31 ; exécutant coordinateur. Commandes Python3.13 `-B -m unittest -v` : cas MSVC nominatif, classes ReviewRegressionHarnessTests/SourceTests, NativeBindingHarnessTests/SourceTests, HelperFunctionalHarnessTests/AdmissionTests, depuis tests Windows du worktree auteur.
- Commandes exactes, cwd, hashes et cas bruts : `out/verification-synthetic/windows-helper-review-green-v2-2026-10-30T11-31-45-890Z/`.
- Ré-audit pool-36, tâche existante `task_mvuv4i6m_007j_ehar0y`, lecture seule.

Seul PowerShell test-owned avec effets mémoire et tranche de copie sur dictionnaire
synthétique est lancé. Aucun helper complet, cmd/vcvars, outil de build, moteur,
GPU, rendu, benchmark, installation ou cloud exécuté.
