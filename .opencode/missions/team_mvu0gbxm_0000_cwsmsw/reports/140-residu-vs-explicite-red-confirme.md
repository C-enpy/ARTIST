# 140 — Résidu du chemin explicite : RED confirmé

Le coordinateur a relu les nouvelles données et assertions. Les **trois contrôles
passent** : prepare avec séparateur final, build sans séparateur et refus causal
d'un répertoire réellement différent. Le seul test du garde avec séparateur final
donne ensuite **un échec attendu**, sans erreur ou skip, empreintes stables.

Le rapport brut confirme la cause exacte « explicit Visual Studio differs from
preparation », sans erreur de harnais, capture ou objet Process. Le témoin normalisé
et l'argument conservant le suffixe sont vérifiés avant l'assertion de succès.
L'environnement OS reste intact.

Le GREEN minimal est autorisé au garde build de Read-VerificationMsvcEnvironmentLeaf,
avec la même identité de répertoire que la comparaison sélection/export et sans
modifier globalement la validation des chemins. Le refus d'une installation
réellement étrangère doit rester intact. Tous les tests, fixtures et assertions
restent inchangés ; nouveaux hashes à proposer puis admettre par le lead.

Il s'agit d'un garde testé sur un arbre synthétique valide, pas encore d'une
couture prepare→build sur le même témoin. Ré-audit indépendant requis avant
intégration ; les deux autres findings restent fermables selon leurs preuves.

## Preuves

- Candidat `71333efc4d0ddebcae22b647047ef266a321f3e7`, auteur pool-31 ; exécutant coordinateur.
- Commandes `C:/Python313/python.exe -B -m unittest -v` avec les classes
  test_review_regressions.ExplicitVsDirectoryHarnessTests puis
  ExplicitVsDirectorySourceTests, cwd tests Windows du worktree auteur.
- Journaux, manifestes, hashes et liens vers les cas bruts : `out/verification-synthetic/windows-helper-explicit-vs-red-2026-10-30T11-41-01-816Z/`.

Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté ; seulement les fragments admis sous effets mémoire.
