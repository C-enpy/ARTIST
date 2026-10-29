# 22 — GREEN B6 transposé et caractérisation B4 10/10

Date : 2026-10-24. Runner : lead ; propositions/correctif : pool-11 B6, pool-10 B4. Pas de clôture globale.

## B6 — vrai delta auteur et contrôle GREEN

Après RED causal rapport21, pool-11 a appliqué dans son worktree le seul ajout `const` au helper privé format_stacktrace. Le lead a vérifié : un seul fichier modifié, `git diff --check` propre, contenu complet égal au pin64d97dc avec ce seul qualificatif ajouté. Aucun include/API/fallback/test changé.

Générateur GREEN de l'auteur persisté/relû à `out/qa-b6/generate-green-probe.ps1`. Sortie exclusivement possédée `out/qa-b6/green-20261024-01`. Il exige le helper **déjà const**, extrait le header réellement fixé et remplace uniquement les quatre tokens de type ; il n'ajoute aucun const.

Source fixée SHA256 `8b716aa41b3c946d5d0440c05a90389ab45adbeea654c2063f21be89cda0b803`. TU GREEN SHA256 `a2b89ee39572c9457325449ec3bd4bc35c587b9b5dc3a193ad5d53f9d72c71a7`, vérifié identique au positif baseline modulo EOL. Manifest : `b6-green-manifest.json` sous la sortie. Compilation réelle, mêmes options que RED :
```text
cl.exe /nologo /std:c++latest /permissive- /EHsc /W4 /c /Fo<green>/b6-green.obj <green>/b6-green.cpp
```
**Exit0**, log `out/logs/b6-const-green.log`. GREEN obtenu dans la frontière C++ transposée, **pas GCC/libstdc++**. Non-régression ArtistLib MSVC sur candidat intégré reste à exécuter. Aucune production canonique changée par ce contrôle ; source fixée encore dans le worktree auteur.

Auteur informé du résultat, gel du delta demandé et shutdown non forcé demandé pour préparer l'intégration native ; arrêt effectif/merge restent à vérifier à la notification, pas de loop de polling ni fausse complétion. Ce stop ne vaut pas non-régression réussie.

## B4 — correction du test, pas du moteur

L'auteur a proposé exactement le delta des deux littéraux attendus pour inclure la newline terminale. Le lead avait recoupé le formatter de la dépendance effectivement liée (rapport21). Les deux assertions restent égalité intégrale, nombre1/type fatal inchangés ; pas de trim, recherche partielle ou alternative tolérée.

Le lead a appliqué les seuls deux littéraux et vérifié que tout ProgramTests.cpp était identique au snapshot du run01 hors ces deux substitutions. Les trois anciens tests ne sont toujours pas refactorés à cette étape.

## B4 — campagne de caractérisation02 exécutée

Même sélection exacte des10 noms du rapport21, même runner/environnement/options, nouveau dossier possédé `out/qa-b4/characterization-20261024-02`.
- Build component target `--parallel 1` : exit0, log `out/logs/b4-characterization2-build.log`.
- CTest opengl/PipelineTest sélection ciblée, `--parallel 1 --no-tests=error --output-on-failure`, limites passed/failed65536 et JUnit nouveau chemin : **exit0**.
- JUnit recoupé avec liste découverte conservée : **10 noms uniques/10 exécutés,0 failures,0 errors,0 skipped**. Huit anciens + deux caractérisations ; aucun double comptage.
- Source stable pendant run, SHA256 `0ef3a6673b4c939e2d43c9fc681bb1720c787555ac80f4bf8e10c100770a4091` ; binaire component SHA256 `20a922179460ed1c68083b28703e95a5314171b5acc3fc788a31d579b66b2f88`.
- Log `out/logs/b4-characterization2-run.log` ; XML/LastTest.log complet/runner.log/snapshot source archivés séparément dans sortie02. Le premier FAIL et son snapshot restent dans sortie01.

**PASS ciblé de caractérisation GL** : débind terminal sauté observé dans contrôle, garde unbind avant cleanup observée dans variante, owners/destruction/contextes contrôlés. Pas d'image/performance/rendu moteur complet. Le FAIL initial de littéral est résolu pour ce dispositif local, pas effacé de l'historique.

Résultat transmis au même auteur : proposition textuelle du seul refactor des trois anciens tests désormais autorisée, garde après owners et avant binding, débinds terminaux supprimés, F10/assertions inchangés. Pas de modification de production/TestUtils, pas d'édition du worktree détaché ; le lead persistera puis rejouera la sélection sur ce delta relu.

## État

Pas encore de suite globale, d'intégration B6 ou de non-régression du refactor B4. B1/B3 non couverts par ces preuves ; C2 OPEN/INCONCLUSIVE, close réel DEFER, ASan H3 NOT_RUN. Revue indépendante et consommateurs d'intégration demeurent derrière le vrai candidat intégré.
