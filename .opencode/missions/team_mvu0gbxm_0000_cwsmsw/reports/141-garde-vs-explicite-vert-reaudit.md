# 141 — Garde du chemin explicite vert, ré-audit demandé

Le coordinateur a relu la normalisation symétrique des chemins requested/prepared
avant comparaison et recoupé les hashes source/binding avant admission privée.
Les racines restent préservées, le chemin du témoin reste l'autorité du build,
et le validateur global ainsi que les tests sont inchangés.

Résultats : **3/3 contrôles et 1/1 régression du résidu**, puis **5/5 contrôles et
3/3 régressions précédentes**, **6/6 baselines et 20/20 tests source natifs sous
doubles**, **7/7 baselines et 11/11 admissions haut niveau**. Aucun échec, erreur
ou skip ; empreintes stables. Le refus d'un répertoire réellement différent reste
causal et sans capture ni objet Process.

Le reviewer initial reçoit le candidat corrigé et les preuves RED/GREEN pour
ré-audit résiduel. Aucune intégration n'est prononcée avant son avis. La couture
sur un même témoin et le mapping des anciens tests restent des étapes distinctes ;
ces cas indépendants ne sont pas présentés comme une campagne prepare→build.

## Preuves

- Source `cc47fb440e4f72008d5a4ecb99555851e9c0ba91` ; admissions lead/candidat
  `dc240fcfe97a7f201903122bd9c738b644f918b8`, worktree pool-31.
- Auteur pool-31 ; exécutant coordinateur. Commandes Python3.13 `-B -m unittest -v`
  sur les huit classes nominatives des modules test_review_regressions,
  test_native_binding et test_helper_functional, cwd tests Windows.
- Commandes exactes, hashes, journaux et cas bruts : `out/verification-synthetic/windows-helper-explicit-vs-green-2026-10-30T11-45-27-377Z/`.
- Ré-audit pool-36, tâche existante `task_mvuv4i6m_007j_ehar0y`, lecture seule.

Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté. Encodage non ASCII et gap Dispose/writers hérités
restent des limites non qualifiées, sans revendication native.
