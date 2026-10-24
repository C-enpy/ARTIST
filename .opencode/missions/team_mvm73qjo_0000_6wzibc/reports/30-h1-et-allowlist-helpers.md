# 30 — H1 et allowlist de la tranche helpers stricts

2026-10-24. Contrat local de développement, avant écriture des tests et du helper. Base intégrée9395b16 : B6 et B4-U intégrés ; B1 sans RED courant, F8 diagnostic sans correction autorisée, C2 ouvert. Ces limites sont maintenues, pas converties en branches corrigées. H_Q17 couvre les oracles de cette tranche et précède ses écritures.

## Résultat et invariants

Le helper partagé doit accepter le message entier égal, rejeter préfixe/suffixe/sous-chaîne/mauvais type d'exception/absence d'exception. Garder le contrat existant de capture par catégorie `catch(const E&)`, sans imposer une nouvelle égalité de type dynamique ; les exceptions dérivées compatibles restent acceptées. Deux implémentations locales déjà strictes peuvent appeler le même helper, sans affaiblir leurs oracles.

Méthode PM/LLM locale : estimation brute55, complexité retenue70 (plancher développement), risque55. Tranche indivisible owner lead, tests→RED exécuté→correction minimale→consolidation→non-régression. Ressources exclusives : TestUtils.hpp et les trois fichiers de test autorisés ci-dessous ; un seul runner par arbre, GL séquentiel. Aucun changement moteur, aucune installation/API nouvelle, aucun élargissement F8. Findings établis : TestUtils utilise find ; les deux helpers locaux utilisent STREQ. Serena refs `{}` ne prouve pas absence de consommateurs : inventaire exact rg ci-dessous, compilations/tests nécessaires.

## Allowlist d'édition

- `ARTIST/tests-common/include/TestUtils.hpp` : égalité complète seulement, diagnostic de catégorie générique si nécessaire.
- `ARTIST/tests/src/common/TestUtilsTests.cpp` : nouveau contrat du helper, avec reporter GoogleTest existant ; pas de framework nouveau.
- `ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp` : consolidation du helper local expectExactError.
- `ARTIST/tests/src/graphic/pipeline/PipelineCursorTests.cpp` : consolidation du helper local expectExactPipelineError.

`ViewTests` non localisé : aucune permission implicite sur un sous-arbre. Tous les autres consommateurs restent immuables ; si un ancien appel dépend d'une sous-chaîne, retour au contrat au lieu d'affaiblir l'égalité.

## Inventaire exact du helper partagé (lecture, pas permission d'édition)

Commande `rg -l expectSpecificError ARTIST`, sortie complète traitée avant décision. Hors définition commune, fichiers :
- tests/src/graphic/context/{AttributeContextTests,PassContextTests,PipelineContextTests}.cpp
- tests/src/graphic/pipeline/{AttributeLifetimeTests,AttributeOwnershipTests,PassTests,PipelineOwnershipTests,PipelineTests,ShaderLifetimeTests,ShaderTests,UniformOwnershipTests}.cpp
- tests/src/graphic/opengl/pipeline/component/attribute/{SetterTests,UnbinderTests}.cpp
- tests/src/graphic/opengl/pipeline/component/pass/{AttributeReaderTests,UniformReaderTests,LoaderTests,ShaderAttacherTests,ReflectionPublicationTests}.cpp
- tests/src/graphic/opengl/pipeline/component/pipeline/{ResetterTests,UserTests}.cpp
- tests/src/graphic/opengl/pipeline/component/shader/{ReaderTests,LoaderTests}.cpp
- component-tests/src/graphic/pipeline/AttributeComponentTests.cpp
- component-tests/src/graphic/shader/ShaderTests.cpp

Chemins relatifs à ARTIST/. Certaines lignes sont de simples imports inutilisés : ne pas les compter comme appels exécutés. Les deux helpers locaux supplémentaires ont été lus ; références exactes identifiées par rg dans leurs seuls fichiers.

## Oracles et preuve attendue

Contrats CPU : message identique accepté sans failure ; message réel avec préfixe, suffixe ou seulement une portion de l'attendu génère exactement une failure non fatale ; absence/wrong-category/nonstd génèrent exactement une failure fatale. Avant correction, préfixe/suffixe doivent échouer par manque de failure du helper (pas par compilation/configuration). Ne pas caler les tests sur le texte instable du formatter GoogleTest.

Après correction : contrats tous verts, puis suites consommateurs CPU et GL réels sur candidat intégré, comptes/non-skips recoupés. Les scripts existants et CMake glob nécessitent une configuration explicite pour découvrir le nouveau fichier ; aucune installation par script wrapper. Revue indépendante à candidat H2 et campagne finale H3, pas de revendication globale anticipée.

## Révision bornée après première non-régression

RED causal exécuté : préfixe/suffixe acceptés à tort, cinq autres contrôles PASS. Correction minimale du helper : sept contrats PASS. La campagne complète découvre555 cas et produit16 FAIL (zéro skip/error), dus à des attendus partiels historiques ; aucune correction production n'est justifiée. Avant nouvelle écriture, le lead étend l'allowlist de tests aux **sept fichiers consommateurs déjà inventoriés** suivants : context/PassContextTests.cpp, context/PipelineContextTests.cpp, opengl/pipeline/component/shader/LoaderTests.cpp, même dossier ReaderTests.cpp, pipeline/PassTests.cpp, pipeline/UniformOwnershipTests.cpp (tous sous tests/src/graphic), et component-tests/src/graphic/shader/ShaderTests.cpp.

Révision dans le périmètre strict-errors approuvé : compléter uniquement les messages attendus des16 cas révélés, à partir des formats d'erreur de production et des entrées de fixture (index/taille/nom/type), pas recopier un diagnostic driver ni tolérer des variantes. La compilation échouée a pour contrat le code et la newline, pas le log driver envoyé sur un autre canal. Toute modification de comportement moteur, nouvelle primitive I/O ou matcher partiel reste interdite. Les assertions d'état/lifetime ne changent pas. Les autres fichiers restent immuables. Archives non-régression rouge conservées avant relance.
