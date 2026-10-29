# 24 — B1 : trois ordres CPU verts, cause historique non fermée

Date : 2026-10-24. Runner lead ; source canonique ecb548b. Aucune correction B1 appliquée.

## Préconditions et exécutions réelles

Le binaire unit reconstruit pour B6 est réutilisé, sans build supplémentaire : SHA256 `0d215657f2d1bc69a8c4a56f94404e81a8bb2559185c99c1f6b168fb0c9c1adc`. Avant run, aucun delta non commité dans ARTIST/include, ARTIST/tests et ARTIST/tests-common ; le delta ProgramTests GL n'est pas dans ce binaire.

Découverte CTest fraîche `-L unit --show-only=json-v1`, log `out/logs/b1-unit-discovery.log` : **477 filtres GoogleTest bruts uniques**, aucune propriété DISABLED à disposer. Les noms bruts de commandes CTest sont utilisés pour confronter XML, sans confondre les noms pretty-printés des types.

Commandes exécutées séquentiellement depuis ARTIST via wrapper existant :
```text
ArtistLib_tests.exe --gtest_color=no --gtest_output=xml:<sortie>/default.xml
ArtistLib_tests.exe --gtest_color=no --gtest_output=xml:<sortie>/seed20261017.xml --gtest_shuffle --gtest_random_seed=20261017
ArtistLib_tests.exe --gtest_color=no --gtest_output=xml:<sortie>/seed7.xml --gtest_shuffle --gtest_random_seed=7
```
Binaire absolu `out/build/windows-msvc-debug/ARTIST/tests/ArtistLib_tests.exe`. Sortie exclusivement allouée `<sortie>` = `out/qa-b1/baseline-20261024-01`.

- Défaut : **exit0,477/477,0 failure/error/skip**.
- Seed demandée20261017, effective **61219** observée dans le log : **exit0,477/477,0 failure/error/skip**.
- Seed7, effective7 : **exit0,477/477,0 failure/error/skip**.

Pour chaque XML :477 noms uniques, correspondance exacte aux filtres découverts. Logs complets `out/logs/b1-baseline-default.log`, `b1-baseline-seed20261017.log`, `b1-baseline-seed7.log`. SHA binaire stable après les trois runs. Lecture du brut au-delà du dernier `[ PASSED ]` : **zéro ligne non vide** dans chacun ; aucun diagnostic tardif masqué observé. Pas seulement un verdict JUnit.

Il s'agit de **477 cas distincts dans trois ordres**,1431 exécutions, pas1431 contrats/tests distincts. Les deux non-régressions B6 du rapport23 font partie de ces477, pas deux cas supplémentaires.

## Portée B1 : aucune clôture causale

PASS borné sur ces trois ordres actuels ; **pas de RED B1** et aucun correctif autorisé par ces résultats. L'incident historique concernait176 cas : population et ordre ne sont pas identiques. Cause des anciennes fragilités encore non établie, pas de preuve de toutes les permutations.

Recoupement framework du lead, sans nouveau test : GoogleTest local `gtest.cc:6263–6274` mélange toutes les suites enregistrées puis chaque suite ; `TestSuite::ShuffleTests:3078–3079` mélange test_indices_ sans filtre. Donc filtrer176 noms dans un binaire477 ne rétablit pas nécessairement l'ordre historique pour une même seed. Toute reprise historique nécessite ordre/pin/binaire identifiés, ou expérience ciblée causale ; pas une multiplication aléatoire des seeds.

Ces preuves et cette limite ont été transmises à pool-8 pour sa proposition discriminante et l'analyse des fixtures/ON_CALL/reset/destruction. La différence contexte membre/local n'est pas une cause démontrée seule. Pas d'édition du mock partagé sans inventaire et accord lead.

## Préconditions des auteurs B1/B3

- Même session pool-8 résolue puis déplacée nativement vers son worktree existant `.../ensemble-2dc31e0a-6wzibc-pool-8` ; lecture locale .git pointeur/H_Q demandée avant édition. Aucun modèle/permission changé. Son accusé effectif reste attendu.
- Pool-9 était state off sans worktree dans le dernier contrôle. Création native `qa-harness-b3-reader`, puis déplacement de sa **même session** `ses_e91b21fabffe5nG3MbTpXJzKP2`. Contrôle lead : racine exacte, HEAD **ecb548b**, statut propre, H_Q présent, Reader.hpp identique octet pour octet au canonique. Les contrats H_Q/H_A précèdent bien cette création.
- B3 reçoit un mandat de **proposition textuelle** de sonde Windows OS→ifstream→Reader, pas d'édition/compilation/exécution autonome ni nouvelle seam/API. Si erreur OS devient EOF/publication vide, remonter le candidat défaut avant correctif ; close réel DEFER. Accusé de lecture effectif/proposition encore attendus ; création et move ne prouvent pas tous les outils correctement rattachés.

## Doublon B6 et état global

Pool-12 a signalé une réaffectation B6 incohérente après la livraison pool-11, sans mutation. Le lead lui a donné le commit ecb548b, les preuves réelles et l'instruction d'inactivité, sans nouvelle enquête ni shutdown répétitif. La vraie reprise B6 a été complétée **après** commit et non-régression2/2 (rapport23), pas pour stopper artificiellement le dispatcher.

B4 refactor des trois tests encore attendu ; caractérisation10/10 seulement. C2 reste OPEN/INCONCLUSIVE, close réel DEFER et ASan H3 NOT_RUN. Aucune revue indépendante finale ni PASS global ARTIST à ce stade.
