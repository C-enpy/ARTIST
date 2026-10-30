# 44 — Premier GREEN runtime : 124 succès et un test non exécuté

## Résultat vérifié

Après relecture du runner, de la CLI, de la collecte, des écritures et des chemins d'exécution, le coordinateur a lancé les **125 tests du cœur** : **124 satisfaits, un SKIP, aucun échec ou error**. Le SKIP concerne la création d'un symlink, refusée par l'environnement Windows. Cette couverture reste INCONCLUSIVE ; aucune permission n'est élargie pour la forcer.

Les tests et fixtures sont inchangés depuis le RED ; empreintes stables. Les processus réellement lancés sont seulement les enfants Python factices possédés. Les tests de phases injectent adaptateur, parser et révision ; aucune composition avec un outil moteur réel n'est exécutée.

## Contrôles indépendants et réserves

Une revue indépendante du cœur Python est en cours, notamment sur lifetime des processus, chemins, preuves et erreurs. L'architecte examine en parallèle la qualification des anciens scénarios CTest matériels, pour lesquels les journaux actuels sont insuffisants. Le candidat garde un refus explicite plutôt que d'inventer cette preuve.

La lecture du coordinateur relève aussi deux points d'intégration : le répertoire de build Windows calculé par le cœur ne correspond pas encore au suffixe isolé imposé par le helper ; le refus d'une sélection CTest hardware connue comme non qualifiable doit survenir avant ses enfants, pas seulement lors de la collecte. Ils sont transmis au propriétaire, sans changement anticipé pendant la revue. La solution de compatibilité sera ratifiée avant correction.

Le candidat n'est pas accepté pour intégration globale sur ce seul résultat. Le parser réel et les adaptateurs restent à composer ; aucune qualification native Windows/Linux ni preuve d'image ou de performance.

## Preuves

Candidat `2631e5cab88dbd4e0af713a6f8458356c6ba55b2`, auteur pool-3, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v`.

Journaux et empreintes : `out/verification-synthetic/core-runtime-green-2026-10-30T02-13-19-712Z/`. Aucun CMake, Conan, CTest réel, helper de construction, moteur, GL, rendu, benchmark, installation ou cloud exécuté.
