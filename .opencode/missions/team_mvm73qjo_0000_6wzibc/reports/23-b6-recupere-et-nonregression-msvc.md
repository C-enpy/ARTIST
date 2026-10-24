# 23 — B6 récupéré dans le canonique, non-régression MSVC 2/2

Date : 2026-10-24. Auteur du correctif : pool-11 ; récupération/intégration et runner : lead. Portée : un qualificatif privé, contrôles transposés et deux tests MSVC ; GCC reste absent.

## Intégration : limite native constatée et récupération

Après shutdown non forcé, `team_merge(pool-11)` a renvoyé : branche sans commit, changements non committés restés dans le worktree, risque de perte au nettoyage. **Aucun merge natif réussi n'est revendiqué.** Aucun nouveau shutdown/re-spawn et aucune suppression de worktree pour contourner ce constat.

Le lead a sauvegardé le header auteur dans `out/qa-b6/recovered-TraceableException.hpp`, SHA256 **`8b716aa41b3c946d5d0440c05a90389ab45adbeea654c2063f21be89cda0b803`**, identique au source ayant produit GREEN rapport22. Puis application par patch du seul delta relu dans le canonique :
```diff
-        std::string format_stacktrace(const std::stacktrace &st)
+        std::string format_stacktrace(const std::stacktrace &st) const
```
Le header canonique résultant a été rehashé, égal à cette sauvegarde et au source auteur validé. Aucun include/fallback/API publique/test modifié par B6. Les deltas ProgramTests et profils préexistants ne sont pas embarqués dans le commit ciblé de B6. Le worktree auteur n'est pas nettoyé ; sa copie non committée doit rester préservée tant que son traitement natif n'est pas réconcilié.

## Preuves RED/GREEN déjà obtenues

Rapports21/22 : contrôle positif MSVC transposé exit0 avant négatif ; négatif exit2, C2662 const-this ; contrôle GREEN extrait du vrai header fixé exit0, TU identique au positif baseline. Générateurs, TU, manifestes et logs sont dans out/qa-b6 canonique, **pas uniquement dans le worktree à durée limitée**.

Preuve de causalité C++ const uniquement, pas de GCC/libstdc++, pas de promesse de portabilité ou performance. Retour std::string possédé, accesseur public const, lifetime emprunté de stackTrace, exceptions et absence de noexcept restent inchangés.

## Build MSVC du candidat récupéré

Commande exécutée via le wrapper existant et environnement vérifié :
```text
out/tooling/build/Scripts/python.exe out/tooling/run-build.py b6-msvc-unit-build cmake --build --preset windows-msvc-debug --target ArtistLib_tests --parallel 1
```
**Exit0**, 39 étapes compile/link, log `out/logs/b6-msvc-unit-build.log`. Header SHA stable avant/après, cache Debug/Ninja/ASanOFF et `ARTIST_CXX23_NATIVE:INTERNAL=1` inchangés ; aucune installation, nouvelle plateforme, macro GCC ou sonde C++23 désactivée. Warnings C4834/C4005 à des emplacements hors diff (AttributeContext/mocks GLEW) conservés dans le log, sans correction incidente ni attribution causale à B6.

## Découverte et exécution ciblées réellement faites

Découverte fraîche via `ctest --test-dir out/build/windows-msvc-debug -L unit -R ^unit[.]TraceableExceptionTests[.] --show-only=json-v1` : exactement deux noms uniques :
- `unit.TraceableExceptionTests.What_IsForwardedToTheBaseException` ;
- `unit.TraceableExceptionTests.GetStackTraceString_ReadsTheReturnedValueSafely`.

Exécution via wrapper label `b6-msvc-nonreg-run`, mêmes filtres, `--parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <sortie>/ctest.xml`.

**Exit0, JUnit2/2 exécutés,0 failure,0 error,0 skipped**, noms recoupés à la découverte. Sortie exclusivement possédée `<sortie>` = `out/qa-b6/msvc-nonreg-20261024-01`. XML, LastTest.log complet, runner.log, discovery.json et snapshot du header archivés aussitôt, avant tout CTest ultérieur. Logs originaux `out/logs/b6-msvc-discovery.log` et `out/logs/b6-msvc-nonreg-run.log` conservés.

Header SHA256 identique au source GREEN ; binaire unit SHA256 **`0d215657f2d1bc69a8c4a56f94404e81a8bb2559185c99c1f6b168fb0c9c1adc`**. Les autres tests du binaire reconstruit n'ont pas été exécutés par cette sélection.

## Disposition et barrières

**PASS ciblé MSVC 2/2** et RED/GREEN transposé obtenus ; source B6 récupéré et destiné au commit ciblé avec ce rapport, dont hash sera communiqué par le lead. La tâche de livraison source peut être terminée après contrôle de ce commit, pas sur le seul statut auteur. Revue indépendante du candidat intégré et non-régression globale restent à faire ; aucun PASS GCC, Release, ASan, image/performance ou global.

B4 : caractérisation10/10 obtenue avant refactor, patch des trois anciens encore attendu. B1/B3 ne sont pas couverts. C2 OPEN/INCONCLUSIVE et close réel DEFER inchangés. Les artefacts canoniques ne doivent pas être retirés avant clôture/archivage des preuves.
