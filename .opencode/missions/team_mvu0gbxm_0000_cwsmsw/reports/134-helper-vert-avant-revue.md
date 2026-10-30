# 134 — Helper vert sous doubles, revue indépendante avant intégration

Le coordinateur a relu les deux corrections et leurs hashes, puis admis les pins
privés sans modifier les primitives intangibles. Les deux tests discriminants
passent : argument inconnu refusé avant effets et absence d'écriture de restauration
si le tooling échoue avant application. La restauration après application reste
présente, avec un drapeau posé avant l'appel pour couvrir une application partielle.

Sur ce candidat : **6/6 baselines natives, 20/20 tests source, 7/7 baselines haut
niveau et 11/11 tests d'admission**, aucun échec, erreur ou skip ; empreintes stables.
Les deux discriminants appartiennent aux onze tests et ne constituent pas deux
scénarios distincts supplémentaires dans ce comptage.

Une revue indépendante fresh du helper est publiée avant intégration. Elle vise
aussi les ports réels remplacés dans les tests, le transport fixe MSVC et les
chemins d'erreur. Le candidat reste stable, non intégré. L'ancien oracle de
fermeture et le prototype de test_dispatch restent à raccorder selon leur mapping ;
aucune découverte Windows globale n'est lancée.

Après réception de la revue, le collecteur intégré et le helper devront encore
être réunis puis testés en couture synthétique. Ce GREEN sous doubles ne qualifie
pas les outils installés, leurs binaires ou le fonctionnement natif Windows.

## Preuves

- Source `f534bd2ac5145193b2bd34182fed5e68291a8f68` ; admissions explicites lead
  `1d0fba7b2914c9d610f9c1857a7f1b0f3637abc0` dans le worktree pool-31.
- Auteur pool-31, exécutant coordinateur. Commande `C:/Python313/python.exe -B -m unittest -v` : deux méthodes discriminantes puis les quatre classes nominatives native/high-level ; cwd tests Windows du worktree auteur.
- Commandes exactes, cwd, hashes, journaux et chemins des cas bruts : `out/verification-synthetic/windows-helper-binding-green-v3-2026-10-30T11-00-25-284Z/`.
- Revue `task_mvuv4i6m_007j_ehar0y`, fresh, lecture seule ; PM/LLM72/76.

Seul PowerShell test-owned avec fragments relus et effets mémoire a été lancé.
Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté. Gap Dispose/writers hérités conservé.
