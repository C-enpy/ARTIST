# Q3 — retrait des sondes de conformité driver de l'acceptation

## Décision et attribution

Demande utilisateur : « Sort cette sonde et regarde s'il y en a d'autres qui
répondent aux mêmes critères ». Cette décision remplace l'obligation Q2/DH-REFUS
de maintenir la sonde struct dans l'acceptation. Les résultats historiques FAIL
NVIDIA restent archivés ; aucune réécriture de leurs assertions, logs ou JUnit.

Plan proposé par qa-planner/pool-11, tâche task_mve2ljw1_000d_t1sg1k,
candidat result_mve2xqm2_000f_9lxe5p, message msg_14ff7d42f00182FB1Bu18wBdjV.
Synthèse persistée par le lead avant écriture auteur. QA en lecture seule,
aucune édition/exécution. Le résultat natif complet conserve le registre détaillé.
Scores PM/LLM validés : Q3 brut/retenu44/44 risque42 ; intervention indivisible
du lead sur les tests brute38/retenue70 risque38. Plancher développement appliqué.
Politique native approuvée inchangée, aucun override de modèle.

Base déclarée4619395 à recouper par runner. Aucun diff moteur/tests à l'entrée.
Modifications externes harness/profils/AGENTS/docs/.grepai et index à préserver.
ADR0003/5 Accepted/partial,0008 Windows Debug,0001/2/4/6/7 Proposed inchangés.

## Critère et registre

Retirer le verdict qui juge seulement le driver via GL brut/helper de fixture,
sans obligation ARTIST ni précondition nécessaire d'une preuve moteur.
Écriture source autorisée : seul
ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp.
Pas de production, shader, CMake, mock ou test unitaire modifié.

REMOVE :
- classe AttributeQaDriverConformanceTest et StructVertexInputIsRejected ;
- boucle de refus dvec3/bool dans NativeInterfacesNamesAndRefusals ;
- second glGetActiveAttrib avec bufSize4 et assertions de troncature dans ce test.

KEEP :
- noms des préflights conservés pour limiter le delta de découverte ;
- profil/capacités, alias, arrays et spelling réellement requis par les oracles ;
- programme12interfaces/TF separate/matrice/nom naturel long et sa réflexion ;
- tous Reader/Setter/upload/TF/freeze/ownership et négatifs CPU ARTIST ;
- ShaderTests.CreateShaderBad*, FailedCompileLeavesUsableContext, refus des
  Loaders : ils vérifient propagation d'erreur et état ARTIST, pas GL seul ;
- tests Uniform et précondition fp64 existante, pas de masque de la sonde.

Inventaire indépendant QA :8sources component-tests et CMake, loaders mockés
échantillonnés. Inventaire lead :10fichiers .cpp/.hpp de component-tests,
recherche exacte des compilations/liens GL et des verdicts bruts. Aucun autre
contrôle de refus GLSL brut lu hors AttributeComponentTests.cpp. Pas d'affirmation
d'audit exhaustif de toute l'infrastructure externe ou du vault.

## Oracles et risques

Avant édition, découverte JSON CTest unit/opengl sur les binaires frais de G.
Après rebuild : ensemble opengl identique sauf le seul nom
opengl.AttributeQaDriverConformanceTest.StructVertexInputIsRejected ; delta−1.
Ensemble unit inchangé. Pas DISABLED_, GTEST_SKIP ou filtre pour simuler le retrait.
Relecture source nécessaire pour les trois tranches internes non comptées par CTest.
Pas de RED comportemental artificiel pour la suppression de contrôles hors contrat :
FAIL historique, inventaire/contrat puis contrôles syntaxe/diff/découverte et runs.

Top3 risques QA I×P : sur-suppression16, masquage12, découverte périmée12.
Déclencheur : corps mixte préflight, ancien filtre struct, liste POST_BUILD ancienne.
Rayon : perte d'oracles ou faux PASS. Détection : diff chirurgical, comparaison
ensembliste des noms, suite GL complète sans -E/-R et JUnit comptés.
Mitigation : un écrivain, build série, aucune modification des tests KEEP ;
gap initial : absence des contrôles sur candidat retiré, à fermer par lead runner.

## Exécution et sortie

Runner unique lead, un build/GPU à la fois. Script réel :
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
-Configuration Debug -TestLabel unit -Jobs 1.
Puis CTest --test-dir out/build/windows-msvc-debug, labels unit/opengl séparés,
--no-tests=error --parallel 1 --output-on-failure --output-junit chemins distincts.
Suite OpenGL complète sans exclusion, sans sélection de noms. Logs sous
out/attribute-gl/q3-driver-probes/. Versions/candidat/delta/hashes et compte
découverts/exécutés/failures/skips conservés. Un skip fp64 éventuel reste
INCONCLUSIVE fp64, pas retrait de sonde. Aucun résultat nouveau à ce stade.

Budget10 unités : setup2.5,planifié5.6,buffer1.9 ; après setup ratio74.7/25.3.
Planifié : édition1.5,diff1,buildunit2,découverte/GL1.1. Buffer non rogné.
Après intégration : audit indépendant fresh hors board sur diff/candidat/oracles,
sans justifications auteurs ; correction et ré-audit si finding établi.
Documentation de résultat après preuve, pas de clôture générale de mission ni
image/perf/ASan/Release/autreOS/API revendiqués.
