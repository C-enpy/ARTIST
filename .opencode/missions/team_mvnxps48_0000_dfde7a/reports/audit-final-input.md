# Audit final — contrat et entrées neutres

2026-10-25. Entrée du lead pour test-reviewer indépendant. Ne lire ni rapports auteurs, ni rapport generator, ni revue correctness, ni board d'implémentation. Ce document et les chemins ci-dessous sont explicitement fournis dans le contexte autorisé ARTIST.

## Admission avant analyse

Confirmer ton profil natif test-reviewer, contexte fresh, permissions effectivement exposées, puis lire ce document et restituer au lead par execute→team_message. Aucun shell/test/édition/délégation ni API/fetch. Claim natif sur cette tâche dédiée permis par l'adaptation opérateur approuvée ; ce n'est pas une permission de consulter le board d'implémentation. Le profil statique sur disque ne prouve pas l'enforcement. Décrire les observations sans lancer des sondes interdites. Attendre le contrôle lead avant analyse. Un refus connu ne se répète pas. Si lecture/transport/profil incompatible, BLOCKED, sans demande d'élargissement de droits.

## Candidat et ressources

Racine canonique explicitement autorisée : C:/Users/djden/source/repos/ARTIST. HEAD582b156cbde1390060a749ba215ce4d21835768e, source C++82bf52d52e594fae02593866669a1af715127d1f. Le lead a vérifié diff source/config vide entre ces pins et absence de diff suivi ARTIST. Pas d'accès nécessaire aux anciens worktrees. Références docs fournies : docs/engineering-principles.md, docs/adr/README.md. ADR0001/2/4/6/7 Proposed,0003/5 Accepted/partial,0008 vérifié Windows Debug uniquement.

## Obligations à auditer

1. Helper strict : message entier exact et catégorie d'exception ; égalité nominale acceptée, préfixe/suffixe/portion/absence/autre catégorie/non-standard détectés. Pas d'affaiblissement pour obtenir GREEN.
2. Consolidation des helpers locaux sans régression d'oracle ; attendus historiques complets au lieu de sous-chaînes. Évaluer les bugs plausibles que les assertions laisseraient survivre.
3. ProgramUnbinder : caractérisations et débind dans les trois tests pipeline ; lifetime/déclaration cohérents avec owners et contrat F10 existant. Pas de correction production F10.
4. format_stacktrace const : résultat Windows et preuve transposée ne valent pas validation GCC/libstdc++ native ; limiter les conclusions du test fallback.
5. ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders : réflexion nominale attendue une fois, handles5/8 et ordre detach→deleteProgram→deleteShader maintenus ; action nominale ON_CALL non remplacée. Aucun relâchement global des mocks/diagnostics.
6. Preuves d'intégration : identité/découverte/sélection/exécution, erreurs/skips, diagnostics tardifs ; ne pas transformer les résultats Windows en preuve Linux/image/performance.

## Sources de tests et consommateurs autorisés

- ARTIST/tests-common/include/TestUtils.hpp
- ARTIST/tests/src/common/TestUtilsTests.cpp
- ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp
- ARTIST/tests/src/graphic/pipeline/PipelineCursorTests.cpp
- ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp
- ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp
- ARTIST/tests/src/graphic/context/PassContextTests.cpp
- ARTIST/tests/src/graphic/context/PipelineContextTests.cpp
- ARTIST/tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp
- ARTIST/tests/src/graphic/opengl/pipeline/component/shader/ReaderTests.cpp
- ARTIST/tests/src/graphic/pipeline/PassTests.cpp
- ARTIST/tests/src/graphic/pipeline/UniformOwnershipTests.cpp
- ARTIST/component-tests/src/graphic/shader/ShaderTests.cpp

Pour localiser le helper const et son test : glob/grep bornés à ARTIST, motifs format_stacktrace et TraceableException, puis lectures nécessaires uniquement. Les sources production/mock directement consommées par ces tests sont lisibles pour comprendre l'oracle, pas pour étendre l'audit moteur. Aucun accès aux justifications auteurs.

## Preuves disponibles (runner lead, pas reviewer)

Archive principale : out/qa-b2-h3/final-h4-20261025-01/ : candidate-before-run.json, discovery.json, selected-names.txt, asan-selected.xml, ctest-full.log, LastTest.log, run-start.json, run-end.json, verification-summary.json. Empreintes des neuf artefacts du summary recoupées par le lead à la reprise. Résultat archivé ASan555/555=484CPU+71GL,0failure/error/skip,17historiques chacun une fois ; sélection pré-run, parallel1, no-tests=error, JUnit et logs complets. Diagnostic tardif à examiner sans réexécuter.

Normal CPU actuel : out/qa-b2-h3/h4-normal-unit-20261025-01.xml,484/484. Les71GL normaux sont historiques réutilisés par identité, pas rejoués. Archive normale historique : out/qa-helpers/nonreg-20261024-02/ ; RED helpers : out/qa-helpers/red-20261024-01/ ; GREEN helpers : out/qa-helpers/green-20261024-01/. Archive rouge ASan initiale : out/qa-b2-h3/final-20261025-01/,554/555, conservée ; ne pas qualifier l'ancien candidat PASS. Pas de shell/parse API : utiliser lectures bornées, demander au lead une digestion si un artefact est volumineux.

## Remise

Verdict borné, registre obligation→preuve et test→obligation, findings bloquants séparés des suggestions/gaps hors scope. Pour finding haut : entrée→mutant plausible→assertion attendue/manquante et localisation précise. Mutation mentale seulement. Distinguer présence statique, inférence et executed par le lead. Rapport en fragments séquentiels <4000octets si nécessaire, restitution native au lead, persistance par le lead. Aucun rapport écrit par toi. Aucun nouveau test demandé pour remplir un quota.

Limites à conserver : F8 catch INCONCLUSIVE/close DEFER, C2 OPEN/INCONCLUSIVE, B1 historique non résolu, GCC natif non validé ; Linux/Release/image/performance non couverts. Tous DEFER_TO reviennent au lead. Ne pas inventer de défaut si aucun bloquant n'est démontré.
