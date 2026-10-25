# 09 — ASan final FAIL ; décision corrective bornée

2026-10-25. Étape vérification effectuée par le lead, pas clôture de mission.

## 1. État et changements depuis08

H3 documentaire intégré au commit `849e6d50c2a545bfdaf661d4d0f408e9a0066a7a`, source moteur/tests toujours identique à318bf7a. Remise39 acceptée sans patch ni nouvelle intervention generator. La campagne ASan prévue est maintenant réellement exécutée, une fois, sans reconstruction, installation ou replay normal.

**Verdict de campagne : FAIL**, pas INCONCLUSIVE. CTest exit8 : **554 PASS /555,1failure,0error,0skip**. CPU483/484 ; OpenGL71/71. Les555 noms découverts, sélectionnés et exécutés sont identiques et uniques. Les17 historiques sont chacun exécutés exactement une fois. Leur présence seule ne rend pas toute la campagne verte.

## 2. Commande et preuves

Commande effective via `out/tooling/run-build.py h3-asan-selected-20261025-01`, processus PowerShell dédié après `Set-AsanTestEnvironment` :

`ctest --preset windows-msvc-debug-asan --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-20261025-01/selected-names.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/final-20261025-01/asan-selected.xml`

Archive **out/qa-b2-h3/final-20261025-01/** : sélection et candidat figés avant run, discovery.json, asan-selected.xml, runner.log, ctest-full.log et LastTest.log copiés immédiatement, run-start.json/run-end.json, verification-summary.json. Sortie CTest originale également conservée dans `out/logs/h3-asan-selected-20261025-01.log`.

- Début16:21:27Z, fin16:23:13Z ; durée CTest105,49s, pas mesure de performance moteur.
- Environnement enfant exact `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`, activation absente. /RTC1, -MDd et assertions Debug conservés.
- Bail fichier exclusif `out/artist-opengl.lock` tenu pendant la campagne et libération vérifiée ensuite ; aucun processus CTest/ArtistLib tests restant. Le nom du mutex réellement journalisé est **LocalARTIST_QA_OPENGL**, sans antislash (échappement du dispositif), et non le nom prévu Local\ARTIST_QA_OPENGL. Le bail fichier et le contrôle des processus sont bien effectifs ; ne pas présenter ce mutex comme un verrou GPU universel.
- Empreintes sources/config/exécutables et9PDB/DLL toutes inchangées après run.
- LastTest.log :554 marqueurs Test Passed.,1Test Failed. Aucun marqueur de troncature recherché ; sortie JUnit maximale23320octets, inférieure au plafond65536.
- Après chaque dernier résumé `[  PASSED  ]`, aucun texte tardif non vide observé. Aucun diagnostic ASan dans les tests comptés PASS. Ces contrôles sont exécutés sur les sorties, pas inférés du seul code retour.
- `verification-summary.json` SHA256 **b6d5b6e00daf2a8d506fb80bc0c811fd43a1d9fb39488c4c9239bccaca050a01**, avec empreintes des autres artefacts.

## 3. Défaut observé et localisation bornée

Test échoué : **unit.ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders**.

Diagnostic : **AddressSanitizer heap-buffer-overflow**, lecture16octets à la fin d'une allocation15octets. La pile traverse `std::_Narrow_char_traits::length`, le diagnostic `UntypedDescribeUninterestingCall` de GoogleMock, puis :

- `ARTIST/tests/mock/opengl/glFunctionMock.hpp:512` : `glGetActiveAttrib_mock(..., GLchar*)` ; wrapper à201.
- `ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp:70–74` : buffer de sortie rempli de0x7f avant appel GL ; son contrôle de terminaison est à75–79.
- `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp:347–373`, appel `pass.load()` à370 ; assertions d'ordre detach/program/shader présentes à363–365.

**Hypothèse étayée, pas encore correction validée :** le pretty-printer GoogleMock interprète le pointeur de sortie GLchar* comme une chaîne C avant l'action du mock et lit le buffer volontairement non terminé. La pile place le débordement dans ce diagnostic, pas dans une lecture utilisateur démontrée après retour de glGetActiveAttrib. Aucune attribution globale au moteur, au driver, au compilateur ou à B1 historique ne découle de ce seul run.

Ne pas masquer le défaut en remettant le buffer de production à zéro, en désactivant ASan/les diagnostics globalement ou en retirant les assertions lifetime. Une éventuelle correction doit conserver l'oracle de terminaison manquante et l'ordre de destruction attendu.

## 4. Gates et bilan non clos

La tâche ASAN n'est pas marquée completed : sa condition zérofailure n'est pas satisfaite. Elle reste inactive côté pool-1, qui a attesté n'avoir exécuté aucune commande ; le runner réel est le lead. Pas de nouvelle admission AUDIT/CLOTURE ni cycle shutdown/réadmission.

QA-PORTABILITE : pool-2 a remis un blocage d'admission/entrées, pas une analyse de fond. Audit test-reviewer : NOT_RUN, admission indépendante encore non démontrée. Aucun résultat indépendant inventé. Le bilan documentaire terminal dépendant de ces entrées n'est pas déclaré livré. Les corrections des profils/runtime concurrents ne sont pas annexées.

F8 catch INCONCLUSIVE/close DEFER ; C2 OPEN/INCONCLUSIVE ; B1 historique non résolu ; GCC natif non validé. Aucun PASS global, Linux, Release, image, performance ou RAII exhaustive. ADR inchangés.

## 5. Options et recommandation

**Option recommandée : autoriser une tranche corrective limitée aux tests/mocks C++ concernés**, pas au Reader ni à son contrat de production. Principal cpp-engineer ; un propriétaire pour le RED→GREEN→non-régression. Diagnostic initial en lecture seule, puis reproduction ciblée causale avec les binaires existants avant toute édition ; scénario discriminant entre logging des pointeurs de sortie et action du mock. Contrat/plan QA17 à réutiliser et amender si nécessaire avant auteur ; fichiers autorisés explicitement après localisation, pas tout le sous-arbre. Revue indépendante sensible requise si le lifetime ou les oracles sont modifiés ; le gate natif n'est pas levé par cette autorisation.

Estimation PM/LLM locale pour cette nouvelle tranche, pas classifier : complexité brute62, **retenue70**, risque65 ; incertitude sur étendue des autres sorties GLchar* et sur admission spécialiste. Une modification de production, une nouvelle seam/dépendance ou une réparation du runtime resterait une autre décision.

Après correction pertinente : GREEN ciblé et non-régression concernée, puis identité/sélection figées à nouveau avant campagne ASan finale. Aucun rerun identique sans nouvelle hypothèse. Conserver l'archive rouge actuelle intégralement.

**Alternative : conserver le FAIL et suspendre ce lot**, avec ces artefacts et un bilan d'inachèvement ; aucun nettoyage, aucune transformation du FAIL en dette close.

**Décision demandée : autoriser cette tranche limitée tests/mocks, ou conserver le FAIL et suspendre le lot ?** La correction du routage/admission de l'audit reste distincte et n'est pas implicitement autorisée.
