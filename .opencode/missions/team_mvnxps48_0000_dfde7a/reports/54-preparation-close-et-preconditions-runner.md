# 54 — Préparation close parallèle et préconditions runner stdio

2026-10-26. Décision52/entrée53 intégrées e1508b9 avant préparations ; source82bf52d inchangée. Aucune calibration/build/test exécutés à ce stade, contrôle RED écrit par pool-8 attendu.

## Branches prêtes publiées, consommateurs retenus

- P_CLOSE task_mvpfek26_001g_uwx145, PM/LLM analyse65/70, pool-7 attendu : graphe/fiches nouveau probeclose, sourcefuture séparée scripts/probes/qa-f8-close/** ; aucun code/publication/autreagent par PM. Contexte de préparation conservé explicitement non indépendant.
- Q_CLOSE task_mvpfek26_001h_4gr085, analyse61/73, admission seuleqa-planner : amendement17E4/identité/lifetime/supervision, pas secondplan. Aucun auteurclose publié avant intégration des deux entrées. SourceS pool8 indépendante, même lead runner sérialisera les builds/sorties.

## Admission QA observée et bornée

Avant publication, GET sessionpool6 ses_e8ba57542ffea1tdYPD3SqBw3x : agentqa-planner, canoniqueARTIST, permissions et ensemble.permissions exactementprofilcanonique. Après auto-dispatch, overlaygénérique reappliqué : nouvelle restriction par PATCHpublic de **cette seule session**, après vérification permissions==overridesEnsembleOwned ; aucune règle opérateur distincte retirée, aucunepermission ajoutée horsprofil. Metadata transmise conservée horsjeu géré.

PATCHexit0/corpsvide a fait échouer le parseurJSONclient ; pas de seconde mutation. GETseul après cette erreur confirme profilqa-planner et permissions ET ensemble.permissions exactementcanoniques ; seuleclemetadataensemble.permissions rendue. Erreurclient non panneMCP/nonFAILtest. Lecture53+restitution postrestriction demandées avant GOanalyse ; aucune prétention sandbox/hotreloadglobal. Aucun accèsAPI/test/shell/edition ou élargissement demandé au QA.

Restitution postrestriction effectivement reçue : lecture53 **relative**,25lignes, après notification du contrôle, transportteam_message fonctionnel. La première lecture antérieure n'a pas été requalifiée rétroactivement. Lead a validé cette admission et donné GO de fond Q_CLOSE dans la même session restrictive/contexte conservé ; aucun rebootstrap. Les anciens blocked d'attente ne sont pas des FAIL comportementaux.

## Préparation runner indépendante de l'auteur

Lectures lead seules pendant écritureS : entréecompile_commands unique TraceableExceptionTests.cpp, protectionsDebug /RTC1/-MDd/-Zi/-std:c++latest et includes actuels présentes. Envwrapper actuel VS18Community/toolset14.44, pas ancienVS2022 à copier ; wrapper fusionne stdout/stderr et archiveout/logs/tag.log, cwdcanonique, commandeintégrale/exit visibles. CMAKE_BUILD_PARALLEL_LEVEL=2hérité duwrapper ne créera pas de parallélisme pour un seul cl direct ; aucun CMakebuild simultané prévu. Ligne adaptée finale/source et réservation sortie seront figées après intégration, pas déjà approuvées/exécutées.

Empreintes observées avant intégration, à contrôler avant réemploi :
- out/tooling/build/Scripts/python.exe : b2c836c52cdf063180b9ee76f67ac42946101b79ac457f3494035a67c090d961.
- out/tooling/run-build.py : a6008206b40a3c9089651e7785c60347d89a75c9fb07a12538e40964ee3de93a.
- out/tooling/msvc-env.cmd : 0414592709229aecfe88844f70f1cf7a10784c1663d205aedd3ecc0d1df96726.
- out/build/windows-msvc-debug/compile_commands.json : d060be875f29a332a38747af795a292143b98e277a0115b22fe1cd6066da7c51.

SourceRED doit arriver/intégrer avant runner ; buildcassé ne constituera pas RED, et12325→8192 ne peut être déclaré exécuté depuis la seule lecture. Aucune réutilisation automatique ASan555/GPU/GCC/Linux/Release/image/perf ; aucun Reader modifié.
