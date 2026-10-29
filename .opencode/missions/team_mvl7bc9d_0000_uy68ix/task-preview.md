# Aperçu des tâches — QA & harness

Préparation seulement, 2026-10-23. Aucun ID de board ni aucune admission.
Candidat observé : `989cb995ef6ae96f2eed8f945e2c6ff29bd90d92` ; à re-pinner avant exécution.
La décision QA du [rapport 001](reports/001-preparation-decision.md) a été arbitrée par l'opérateur : proportionnée ciblée, dérogation à la chaîne de profils pour cette mission seulement, périmètre confirmé. Voir le [rapport 002](reports/002-configuration-confirmee.md). La politique corrigée attend encore son preview natif et son approbation.

## Contrat borné proposé

Instruire les six branches de `docs/architecture/missions/2026-10-23-qa-harness-debt-batch.md`, sans ouvrir ADR-0004. Le rafraîchissement D est déjà documenté : le recouper, pas le refaire comme une branche de développement.

Pas de Release, autre plateforme/backend, image, performance, nouveau framework de test, réparation du plugin Ensemble ou installation d'une toolchain GCC. Aucun élargissement de contrat moteur sans défaut causal établi. Les modifications utilisateur présentes restent intactes.

Chaque auteur garde son cycle RED → GREEN → REFACTOR. Une caractérisation ou une campagne ne fabrique pas de RED. Les tests sont exécutés par le lead, avec attribution de l'auteur et du runner ; les workers proposent tests, patches et commandes. Le plan QA de risques/acceptation est tenu par le lead et persisté/commité avant les worktrees auteurs ; la revue indépendante ciblée reste conservée, sans chaîne de profils imposée.

## Tâches proposées — sans scores ni classifieur

1. **B1 — isolation Loader/ShaderLoader** — expertise `cpp-engineer`, contexte fresh.
   - Propriétaire : fixtures/tests des deux loaders sous `ARTIST/tests/src/graphic/opengl/pipeline/component/{pass,shader}/LoaderTests.cpp` et mocks directement concernés, après inventaire des usages.
   - Rejouer les deux ordres historiques, seed 7 et ordre associé au seed demandé 20261017 (seed effectif historique 61219). Enregistrer seed effectif et ordre réels ; ne pas supposer les mêmes permutations sur un binaire courant plus grand.
   - Isoler cause/lifetime des mocks ; corriger seulement après reproduction causale. Aucun défaut moteur n'est déduit de l'ancien FAIL.
   - Sortie : preuve RED/GREEN ciblée et rejouage des deux ordres, ou diagnostic motivé avec limites ; pas de garantie toutes permutations.

2. **B3 — F8 authentique** — expertise `cpp-engineer`, contexte fresh.
   - Propriétaire : `ARTIST/include/graphic/opengl/pipeline/component/shader/Reader.hpp`, son `ReaderTests.cpp` et fixtures strictement nécessaires.
   - Examiner les seams existantes ; distinguer injection sentinel et véritable `std::ifstream::failure`, ainsi que fermeture réellement échouée. Vérifier traduction exacte, absence de publication partielle et fermeture avant publication.
   - Corriger un défaut seulement après RED causal ; une preuve impossible dans l'environnement reste INCONCLUSIVE/DEFER, jamais un succès obtenu avec un mock de substitution.
   - Pas de changement aux loaders ni de nouvelle infrastructure générale. Si le scénario réel impose un élargissement, revenir au lead, puis arbitrage utilisateur si nécessaire.

3. **B6 — const-correctness stacktrace libstdc++** — expertise `cpp-engineer`, contexte fresh.
   - Propriétaire : `ARTIST/include/common/exception/TraceableException.hpp` et `ARTIST/tests/src/common/exception/TraceableExceptionTests.cpp`.
   - Borner la correction au chemin `getStackTraceString() const` → `format_stacktrace`, et aux dépendances directes nécessaires à cette branche.
   - Maintenir les contrats MSVC existants. La branche libstdc++ inactive sous MSVC n'est pas validée par un test du fallback ; conserver l'argument statique et la limite GCC explicite.
   - Pas de macros libstdc++ forcées sous MSVC pour prétendre à une preuve GCC. Contrat de compilation réel seulement si une toolchain compatible est déjà disponible ; aucune installation implicite.

4. **B4 — unbind et oracles exacts** — expertise C++/tests, contexte fresh.
   - Propriétaire : `ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp`, `ARTIST/tests-common/include/TestUtils.hpp`, `ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp`.
   - Réutiliser la garde RAII existante pour `UsePassTest`, `UseNextPassTest`, `ResetPipelineTest` ; aucune RAII moteur générale.
   - Caractériser l'oracle partagé : exception attendue acceptée, absence/mauvais type/message préfixé ou suffixé rejetés. Remplacer la sous-chaîne par l'égalité stricte, puis consolider `expectExactError` sans affaiblissement.
   - Inventorier tous les consommateurs. Si leur adaptation touche un fichier appartenant à B1/B3, attendre leur intégration ; pas d'édition concurrente ni d'extension silencieuse hors des consommateurs directs.
   - Aucun `ViewTests` n'a été localisé par le glob dédié : ne pas inventer ce chemin cité dans la fiche.

5. **B5 — anomalie C2** — lead, diagnostic en lecture seule initialement.
   - Entrées : `out/mission-c*`/artefacts historiques réellement retrouvés, `docs/development/known-incidents.md:403-414` et ReaderTests courant.
   - Recouper la baseline 20:35 et le RED exact 20:38 ; rechercher état de build/fixture et reproduire au périmètre causal si possible, sans altérer les travaux B3.
   - Sortie : reproduction expliquée ou proposition motivée de clôture. **C2 reste OPEN tant que l'opérateur n'a pas accepté une clôture sans reproduction.** Une absence de reproduction seule ne ferme rien.

6. **B2 — ASan du candidat intégré** — lead runner, expertise build si nécessaire.
   - Démarrer la campagne finale seulement après intégration effective de B1/B3/B4/B6 et gel des éventuels correctifs issus de B5/revue.
   - Déclarer avant exécution la sélection courante CPU + OpenGL ; recouper les 17 GL historiquement exclus avec leurs noms actuels. Une différence de liste doit être expliquée, pas masquée par un ancien comptage.
   - Préflight/canari ASan strict, puis campagne sur l'arbre dédié. Recouper découverte, exécution, JUnit, logs et empreintes ; pas de PASS global si cas requis absent, skip, erreur ou preuve périmée.
   - Aucun changement du harnais ASan n'est autorisé par cette seule campagne. Réutiliser ses tests existants uniquement si un changement réel du harnais est ensuite autorisé.

## Dépendances et ressources

- Après résolution des décisions et éventuelles barrières QA locales : B1 ∥ B3 ∥ B6, jusqu'à trois auteurs en parallèle ; enquête documentaire B5 indépendante.
- B4 après intégration de B1/B3 : la sémantique du helper partagé concerne leurs tests. B6 est indépendant de ces fichiers.
- B5 reste lecteur des entrées B3 tant que B3 est actif ; une reproduction utilisant le même arbre de build attend son bail.
- Le lead vérifie chaque shutdown/merge et le diff avant de publier les consommateurs ; aucune dépendance n'est libérée sur la seule fin d'un auteur.
- Un seul écrivain par fichier, un seul build par répertoire, un job par défaut ; GPU sérialisé via `artist_opengl`, ASan exclusif. Pas de builds/tests concurrents sur l'arbre canonique.
- Revue indépendante proposée après intégration : ciblée sur oracles, lifetime/erreurs et branche préprocesseur. Elle apporte une vérification distincte des tests exécutés ; pas d'audit moteur global.
- B2 est finale : toute modification ultérieure invalide uniquement les preuves impactées et impose leur renouvellement. Documentation/clôture et réindexation ARTIST après le dernier diff validé.
- Les consommateurs/reviewers ne sont pas publiés prématurément. L'opérateur a retenu la revue fresh ciblée et écarté la chaîne systématique de profils uniquement pour cette mission ; aucun spawn de préparation. Un repli hors Ensemble ne peut pas contourner la politique d'admission/modèles.

## Commandes proposées, non exécutées

Depuis la racine ARTIST ; les outils et préconditions seront revérifiés. Le wrapper existant gère Conan, MSVC, configuration et environnement ASan. Aucun script nouveau n'est demandé.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1
ctest --test-dir out/build/windows-msvc-debug -N --show-only=json-v1
ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error
ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer -AsanPreflightOnly
ctest --test-dir out/build/windows-msvc-debug-asan -N --show-only=json-v1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer
node .opencode/scripts/update-wiki.mjs
qmd embed -c artist-wiki
```

Les commandes CTest explicites servent aux sélections/discriminants utiles : ne pas rejouer la suite normale déjà exécutée par le wrapper uniquement pour remplir un rapport. La découverte ASan suit la préparation de cet arbre ; le wrapper final peut redécouvrir des tests et sa liste doit être recoupée avec la sélection déclarée avant leur exécution.

Les rejeux shuffle du binaire CPU utiliseront `--gtest_shuffle --gtest_random_seed=7` puis `--gtest_random_seed=61219` ; le chemin du binaire, ses empreintes, son CWD, l'ordre effectif et les sorties JUnit seront consignés après découverte. Les sondes F8/C2 précises dépendent du diagnostic, pas d'une autorisation de campagne indéfinie.

## Acceptation proposée

- Candidat Windows Debug intégré : tests découverts non nuls, unitaires et OpenGL réels sans FAIL ni skip requis ; oracles exacts, pas seulement code retour CTest.
- ASan : candidat final identifié, canari attendu 0/86, sélection et complément historique GL justifiés, zéro échec/skip requis ; dépendances/driver non instrumentés signalés.
- Chaque finding : corrigé avec preuve, clos avec motivation autorisée, ou DEFER/INCONCLUSIVE avec propriétaire et preuve manquante. **Instruire six branches ne promet pas six clôtures.** Un critère requis non vérifié empêche un PASS global.
- F8 reste partiel sans les branches authentiques ; B6 ne prouve pas GCC ; C2 sans reproduction nécessite son acceptation humaine séparée. Pas de tolérance, skip ou mock ajouté pour effacer un défaut.
- Incidents et preuves documentaires actualisés au résultat observé ; ADR-0002 Proposed/partial, ADR-0003 Accepted/partial, ADR-0004/0006 inchangés. Réindexation ARTIST vérifiée, sans toucher aux autres collections.
