# 137 — Deux corrections vertes, garde du harnais MSVC à adapter

Les trois corrections ont été relues et leurs hashes réadmis par le coordinateur.
Les cinq contrôles préalables passent. Sur les trois régressions, celles du cache
PRE_TEST typé et de la copie avec Keys passent ; le cas MSVC échoue encore dans
le **harnais**, sans nouveau défaut produit établi.

Le cas atteint désormais la troisième commande simulée, Conan. Le producteur
transmet le chemin VS normalisé comme attendu, mais la garde de confidentialité
du faux enfant compare encore toutes les valeurs au snapshot MSVC brut, où cette
valeur conserve son séparateur final. Elle refuse donc la donnée synthétique
normalisée avant d'enregistrer la capture Conan.

Le propriétaire reçoit une adaptation nominative de ce seul contrôle VSINSTALLDIR :
admettre le chemin sélectionné synthétique attendu, sans tolérance générale sur
les autres clés/valeurs ni fuite des valeurs réelles. La production, les pins et
toutes les assertions des tests restent inchangés. F1 demeure INCONCLUSIVE à ce
stade de harnais ; il ne devient ni GREEN ni nouveau défaut natif reproduit.

La séquence s'est arrêtée avant les suites 6/20/7/11, qui restent NON_EXECUTE
sur ce candidat. Le ré-audit indépendant et l'intégration restent en attente.

## Preuves

- Source `ead7f8374206062892dfb752626ffc1e76ac3dfc` ; admissions explicites lead
  `7fcee245ec14b95fea97cd60fa483ea04f8e308e`, worktree pool-31.
- Auteur pool-31 ; exécutant coordinateur. Commandes nominatives des classes
  ReviewRegressionHarnessTests puis ReviewRegressionSourceTests du module
  test_review_regressions, Python3.13 `-B -m unittest -v`, cwd tests Windows.
- Résultat :5/5 contrôles,3 tests avec1 échec de harnais,0 erreur/skip,hashes stables.
- Journaux : `out/verification-synthetic/windows-helper-review-green-2026-10-30T11-28-41-518Z/` ; cas MSVC brut pointé dans stderr.

Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté. Les effets du faux enfant restent en mémoire.
