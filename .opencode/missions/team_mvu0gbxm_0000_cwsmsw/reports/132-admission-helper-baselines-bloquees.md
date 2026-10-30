# 132 — Pins admis, baselines bloquées par le harnais

Le coordinateur a relu les adaptations du harnais et recoupé les hashes privés,
puis persisté explicitement les deux admissions JSON dans le worktree auteur.
La production et les assertions antérieures sont inchangées. Les primitives
WRAPPERS/NATIVE ne sont pas repinées.

Seules les six baselines natives ont été exécutées : **six méthodes, six échecs
avec sous-cas**, aucune erreur ou skip ; empreintes stables. Le harnais refuse
une commande de l'entrée avant toute observation, factory ou horloge. Le parent
OS est intact et l'état de fixture nettoyé. Aucun test source ou haut niveau n'a
été lancé après cet échec de précondition.

Le contrôle nominatif du harnais natif n'admet pas encore quatre helpers privés
de l'entrée pourtant relus et autorisés dans le mapping. Cette hypothèse statique
est transmise au propriétaire pour confirmation et correction de cette seule liste,
avec diagnostic de nom sans arguments sensibles. Pas de wildcard ou retrait
d'assertion ; source de production et pins restent stables.

Cet échec de harnais **n'est pas un RED du produit**. Les vingt tests source
restent NON_EXECUTE. Les baselines devront passer avant de reprendre leur exécution.

## Preuves

- Candidat harnais `8324d3ebe03c2c9d934fdbdd612001aba48ed7e6` ; admission lead `4ef3d3e13160d0bf502a88c97f93bcc3edba23b8`, source `27dd1ed` inchangée.
- Exécutant coordinateur ; commande `C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingHarnessTests`, cwd tests Windows du worktree pool-31.
- Journaux/manifeste : `out/verification-synthetic/windows-helper-binding-green-2026-10-30T10-48-15-858Z/`.
- Hypothèse ciblée : allowlist `Assert-BindingSource` ne contient pas encore
  Get-VerificationAbsolutePath, Assert-VerificationRegularPath,
  Select-VerificationEnvironment et Write-VerificationToolchainWitness.

Seul PowerShell test-owned a été lancé. Aucun helper complet, cmd/vcvars, outil
de build, moteur, GPU, rendu, benchmark, installation ou cloud exécuté.
