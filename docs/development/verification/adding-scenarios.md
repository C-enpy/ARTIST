---
title: Ajouter un scénario de vérification
category: development
tags: [artist, testing, build, guide]
summary: "Contrat, fichiers à modifier, exemple de sélection CTest et protocole TDD pour ajouter un scénario sans contourner le catalogue fermé ni les preuves."
created: 2026-10-30
updated: 2026-10-30
sources:
  - scripts/verification/catalogue.py
  - scripts/verification/model.py
  - scripts/verification/planning.py
  - scripts/verification/scenarios/protocol.py
  - scripts/verification/adapters/windows.py
  - scripts/verification/adapters/linux.py
  - scripts/verification/evidence.py
  - scripts/verification/reporting.py
  - ARTIST/verification/WorkloadProtocol.hpp
  - ARTIST/verification/OpenGLFloatWorkload.cpp
  - ARTIST/verification/CMakeLists.txt
  - scripts/build-windows.ps1
  - scripts/tests/verification_scenarios/test_contract.py
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Ajouter un scénario de vérification

Voir [[development/verification/README|utiliser le pipeline]] pour les commandes
et [[development/verification-pipeline|référence opérateur]] pour les préconditions,
reçus et codes de sortie. Ce guide décrit le code observé au 30 octobre 2026 et une
méthode d'extension ; aucun scénario supplémentaire n'est installé par sa lecture.

## 1. Choisir la bonne forme

Le pipeline n'est pas un système de plugins : pas de découverte de modules
arbitraires, de commande shell dans le catalogue ou d'import à effets de bord.
Deux producteurs seulement sont actuellement admis :

- **`selection_ctest`** : sélectionner des tests GoogleTest/CTest existants par
  noms exacts ; adapté à un contrat de préparation, lifetime ou refus.
- **`workload`** : lancer un exécutable identifié produisant un résultat structuré ;
  adapté à une séquence de travail contrôlée et, si ratifiée, à une mesure.

Préférer une sélection existante si elle prouve exactement l'obligation. Ne pas
créer un binaire, un protocole ou un framework générique sans nécessité. ^[inferred]

## 2. Écrire le contrat avant d'implémenter

Définir dans le brief de mission :

1. Question, `id` stable et version ; observation attendue et ce qu'elle **ne** prouve pas.
2. CPU/GL logiciel/GL matériel, OS, Debug/Release, routes, cibles et fixtures.
3. Travail et population : updates/draws attendus, itérations, répétitions et lots.
4. Oracle indépendant : résultat attendu, cas nominal, négatifs et mutant/scénario
   discriminant. Ne pas calculer attendu et observé avec la même implémentation.
5. Pour une comparaison : référence réelle, même qualité et workload, coût de
   préparation séparé, unité mesurée, échauffement, dispersion/bruit et limites.
6. Identités à préserver, première cause d'erreur et traitement des absences/skips.
7. Fichiers possédés, dépendances, arbre de build/contexte exclusifs et exécutions
   autorisées : synthétiques, compilation, tests moteur, GL, image ou performance.

Un comportement sous contrat déclenche la boucle QA d'AGENTS : plan avant auteurs,
TDD, intégration avant consommateurs et revue indépendante adaptée. Les scores
PM/LLM sont distincts ; toute future tâche de code/test a le plancher de complexité 70.
Une frontière de protocole ouverte revient à l'architecte, pas à une extension
silencieuse de ce guide. Respecter [[engineering-principles|les binding times]] :
métadonnées fermées au plan, effets seulement dans les phases explicites.

## 3. Carte des points d'extension

- **Catalogue actif** : `scripts/verification/catalogue.py::list_scenarios`.
  Ajouter un `ScenarioSpec`, données seulement. `scenarios/descriptions.py` est
  la source historique mentionnée par ce catalogue, pas l'entrée de `verify.py`.
- **Modèle et plan** : `model.py::ScenarioSpec`, `Request`, `VerificationPlan` et
  `planning.py::make_plan`. Ils valident les sélections fermées. Profils, OS,
  configurations, routes et producteurs ont des domaines explicites.
- **Workload Python** : `scenarios/protocol.py::_selection`,
  `build_workload_argv`, `parse_workload_result`, validation des opérations/samples.
  IDs, routes, paramètres et métrique sont fermés : un nouvel ID workload ajouté
  au seul catalogue serait refusé ici. Les versions actuellement acceptées sont1.
- **Workload C++** : `ARTIST/verification/WorkloadProtocol.hpp::parse`, `Result`
  et son sérialiseur ; implémentations CPU/GL sous `ARTIST/verification/`.
  Le parseur admet lui aussi des IDs précis ; ne pas affaiblir ce refus.
- **Build/cibles** : `ARTIST/verification/CMakeLists.txt`, option racine
  `ARTIST_BUILD_VERIFICATION` OFF par défaut, helpers et inventaire d'exécutables.
  Une nouvelle cible impose de vérifier aussi la liste admise dans
  `scripts/build-windows.ps1` et les tests d'intégration CMake. Build ne lance
  pas automatiquement un workload et ne l'ajoute pas à CTest.
- **Adaptateurs** : `adapters/windows.py` et `adapters/linux.py::describe_execution` :
  descriptions communes argv/environnement, target du reçu et chemins privés.
  Conserver run direct, pas helper-run ; ne pas dupliquer leur orchestration
  dans le nouveau scénario. Modifier un adaptateur seulement si le contrat l'exige.
- **Collecte et relecture** : `evidence.py` et `reporting.py`, rapprochement
  scénario/route/request ID, compteurs, contexte, JUnit, artefacts et fraîcheur.
  Une nouvelle forme de résultat/métrique peut demander une migration explicite
  de ces consommateurs, pas seulement un nouveau champ produit par le binaire.

Avant tout changement de contrat, rechercher références **et tests** ; un nom
retrouvé ne prouve pas une couverture exhaustive. Ne pas réécrire le cœur pour
rendre le scénario permissif. ^[inferred]

## 4. Exemple minimal : sélection CTest supplémentaire

Exemple documentaire **non enregistré** : isoler le test de réflexion float déjà
existant, sans nouveau workload ni protocole. Dans le tuple retourné par
`catalogue.list_scenarios`, le futur auteur pourrait ajouter :

```python
ScenarioSpec(
    id='gl.float-reflection-smoke', version=1,
    profiles=_GL_PROFILES, platforms=_PLATFORMS, routes=(),
    configurations=_CONFIGURATIONS,
    targets=('ArtistLib_component_tests',),
    fixtures=(_COMPONENT_SOURCE, *_SHADERS),
    producer={'kind': 'selection_ctest', 'names': (
        'opengl.PreparedFloatUniformComponentTest.StrictReflectionAndTypedReadback',
    )},
    oracle='Strict scalar reflection and native typed readback on real OpenGL',
    metrics=(), timeout_seconds=60,
),
```

Ces constantes existent dans le catalogue. Le nom vient du test réel, pas d'une
expression régulière inventée ; le découvrir sur la cible reste obligatoire.
Cet exemple ne démontre ni tous les cas de préparation ni une performance.

Écrire d'abord les tests de catalogue/plan/description attendus et constater leur
RED causal avant l'ajout. Vérifier ensuite que les adaptateurs décrivent la
découverte CTest puis une sélection exacte non vide, avec JUnit, sérialisation et
`--no-tests=error`. Un nom absent de la découverte doit être refusé, sans élargir
la sélection à toute la suite. La phase GL réelle n'est lancée que si autorisée.
Ne pas transmettre cet ID à `build_workload_argv` : ce n'est pas un workload.

## 5. Cas plus large : nouveau workload ou nouvelle mesure

Étendre ensemble l'admission Python, l'admission C++ et le résultat attendu,
avec tests négatifs de divergence. Réutiliser le protocole commun si compatible ;
ne pas fabriquer un JSON favorable ou désactiver le contrôle d'identité.

Le document v1 transporte notamment `schema_version`, `scenario_version`,
`run_id`, `request_id`, `plan_sha256`, `scenario`, `route`, `profile`, paramètres,
opérations attendues/observées, verdict, validité, observations, environnement et
samples. Les parsers font autorité sur leurs types/champs exacts ; ce résumé
n'est pas un schéma alternatif. Une hausse de version exige de traiter les
consommateurs existants, pas de changer seulement `ScenarioSpec.version`.

Les samples actuels sont `cpu_wall_submission` en `ns`, avec répétition et
population. Une durée GPU nécessite une autre instrumentation et son contrat :
ne pas renommer une durée CPU. Dans les intervalles chronométrés, éviter logs,
assertions, readbacks et compteurs de diagnostic ; conserver les gardes de
production. Produire les entrées hors intervalle et vérifier le travail hors
mesure. Le contrôle A/A et la politique de bruit actuels sont candidats, pas
un seuil de qualité universel déjà qualifié sur toutes les machines.

Préserver les sorties brutes et codes workload 0/10/20/30 (succès/oracle/précondition/
protocole), traduits par le cœur vers ses codes CLI distincts. Une erreur de
compilation ou un contexte absent n'est pas le RED attendu d'un nouvel oracle.

## 6. TDD et réception par niveau de preuve

1. **Hors ligne** : admission/refus du scénario, zéro effet list/plan, profils/
   routes/configurations incompatibles, versions et résultats malformés, identité
   croisée, zéro test découvert, compteurs faux, mesure absente ou invalide.
2. **Composition synthétique** : vrais catalogue/plan/adaptateur/runner avec
   doubles aux seules frontières d'effets. Vérifier argv, environnement effectif,
   première cause, reçus et relecture du même bundle. Un mock ne prouve pas GL.
3. **Natif autorisé** : compilation de la cible, découverte réelle puis population
   exacte et oracle CPU/GL/image. Préconditions manquantes → INCONCLUSIVE.
4. **Performance autorisée** : Release, machine et renderer observés, deux voies
   sur le même candidat/protocole, échantillons et coût de préparation conservés,
   comparaison reproductible à qualité égale, pas de gain présumé.

Points de départ pour les tests existants :
`scripts/tests/verification_core/test_{model,plan,runtime}_contract.py`,
`scripts/tests/verification_scenarios/`, `verification_windows/`,
`verification_linux/`, `verification_integration/` et `verification_seams/` sous
`scripts/tests/`. Ces répertoires n'ont pas tous les mêmes effets : relire les
fixtures avant de proposer une commande. Pas de découverte globale aveugle.
Les noms de fichiers sont des repères, pas une consigne de relancer toute la suite.

Une tranche garde un propriétaire pour RED → GREEN minimal → refactoring et
non-régression. Intégrer les producteurs avant leurs consommateurs et QA finale.
Le runner autorisé conserve commandes/cwd/HEAD/diff, discovery/exécution, comptages,
skips, logs et hashes. Les reviewers apportent un avis indépendant borné ; ne pas
leur attribuer les commandes du runner. Séparer les verdicts des niveaux ci-dessus.

## Brief à transmettre à un agent

Copier ce bloc dans une nouvelle session, remplacer les champs, puis suivre AGENTS.
Un brief n'est ni un `start` natif ni une autorisation d'exécution implicite.

```text
Projet ARTIST, racine du dépôt et HEAD/diff à vérifier avant toute édition.
Lire AGENTS.md, docs/engineering-principles.md, docs/adr/README.md,
docs/architecture/correction-plan.md, docs/development/verification/README.md
et docs/development/verification/adding-scenarios.md.

Objectif : ajouter [ID et question], producteur [selection_ctest ou workload].
Profils/OS/configurations/routes : [...]. Version : [...].
Oracle indépendant et preuve attendue : [...]. Négatifs/mutant : [...].
Comparaison éventuelle : référence réelle [...], travail identique [...],
métrique/unité/population [...], préparation et politique de bruit [...].
Fichiers/symboles autorisés et interdits : [...].
Contrats/consommateurs à examiner : catalogue, modèle/plan, protocoles Python/C++,
cibles/helpers, adaptateurs, collecte/relecture et tests selon le producteur.
Dépendances, ownership et ressources exclusives : [...].
Exécution autorisée : [lectures / synthétique / build / moteur / GL / mesure].
Pas d'installation, workflow ou cloud sans mandat explicite.
Évaluation PM/LLM : scores bruts/retenus et risque, complexité code/test >=70.
QA avant auteur ; RED causal observé, GREEN minimal, non-régression et revue.
Préserver verdict/validité/fraîcheur, artefacts et première cause ; zéro test != PASS.
Remise : diff borné, fichiers, commandes réellement exécutées, candidat/hashes,
comptages/skips, preuves RED/GREEN et limites NON_EXECUTE/INCONCLUSIVE restantes.
Si l'équipe est utile : préparer la mission native et sa configuration,
attendre le start utilisateur ; ne pas réutiliser l'ancienne équipe clôturée.
```

## Checklist de fin

- [ ] ID/version et périmètre explicitement ratifiés, catalogue immuable et sans effets.
- [ ] Producteur choisi et tous ses consommateurs cohérents ; aucune ouverture wildcard.
- [ ] Oracle indépendant, cas nominal et négatifs discriminants ; RED causal conservé.
- [ ] Fixtures/shaders/exécutables attribués au candidat, reçus et preuves non réparés.
- [ ] Windows/Linux prouvés séparément ou limites annoncées, pas de fallback caché.
- [ ] Verdict test, validité mesure et fraîcheur indépendants ; absence ≠ réussite.
- [ ] Documentation opérateur et index actualisés, statuts [[adr/README|ADR]] conservés.
- [ ] Compte rendu : livré/qualifié/mesuré distingués, aucune performance inventée.
