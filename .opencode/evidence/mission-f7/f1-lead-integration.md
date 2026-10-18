# F1 — intégration lead, 2026-10-18

Auteur bfd94a0, base113411d (planQA commité avant auteur). Worktree propre,
preuves copiées au canon `out/mission-f7/f1/` avant shutdown puis team_merge.
Manifeste `artifacts-sha256.json` :
`1ea759528a79785cdd50db7473854df769cfd4f58bdf9f59e9a91eb62c9ada04`.
25/25artefacts copiés re-hashés, zéro divergence.

Diff production strict : deux expressions typeid(m_value).name() remplacées
par m_value.type().name() dans AttributeContext::getValue et setValue.
Préfixes publics y compris SET-in-GET, types d'exception, champs1 des diagnostics,
freeze/payload préservés. Nouveau testCPU dédié2cas sans mock/GL. Note auteur
f1-author-results.md conservée. Aucune dérive .grepai ni mission locale intégrée.

Identité post-merge des deux sources = hash G2 auteur :
- header : `2a041b620baf3f5acf0aa85b28f3e7a8b7894b5554410f2a87c736a6931d7c86`.
- test : `fda116f444d08bffa9eef486f7464b6aea7c1fa835900e9c99c324de4de62135`.

Revalidation lead exécutée après merge (cmd cwdprincipal) : load-msvc.cmd,
cmake --preset windows-msvc-debug (re-glob), build --parallel1,
ctest --preset windows-msvc-debug -L unit --no-tests=error --output-on-failure
--output-junit cheminABSOLU/lead-integrated-unit.xml. Chaîne exit0.
JUnit251cases/failures0/disabled0/skipped0, log lead-integrated-unit.log,
sous out/mission-f7/f1 du principal. Pas de nouveau runGL/ASan pour F1.

RED2/2 causal avant production et GREEN2/2 ciblé+251unit auteur consignés
dans les logs/manifeste copiés. Revuefresh indépendante après intégration
nécessaire avant clôture F1. Aucun PASSglobal ou UBexhaustive revendiqué.
