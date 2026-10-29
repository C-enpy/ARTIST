# 42 — HANDOVER : finir la mission ARTIST QA & harness

Date : 2026-10-25. Document autonome de reprise, préparé par le coordinateur. **Mission non terminée.** Aucun build, test, worker ou commit supplémentaire exécuté pour cette passation ; seuls identité, présence des artefacts, état natif et disponibilité de Context Mode ont été contrôlés.

## 1. Résultat attendu et première action

Finir le lot B1–B6 déjà engagé, **sans recommencer son développement ni ses validations encore valides** : accepter/intégrer la remise generator, figer H3, exécuter la campagne ASan complète prévue, faire l'audit fresh des tests, actualiser la documentation et donner un bilan borné. Ne pas transformer les dettes restantes en PASS.

Première action dans la nouvelle session : lire AGENTS et ce document, vérifier le rattachement à l'équipe existante, puis contrôler/persister la remise QA du rapport39. **Ne pas relancer pool-16 pour obtenir ce texte : il est déjà intégralement sur disque et ne propose aucun patch.**

Le prompt de reprise en fin de document exprime le GO opérateur pour ce flux, si l'utilisateur le copie dans la nouvelle session. La seule création de ce document n'a pas lancé ce flux.

## 2. Identité vérifiée au moment du handover

- Racine : `C:/Users/djden/source/repos/ARTIST`, branche `main`.
- HEAD : **`318bf7a0e5da2a65cc437ccc29baa92418822bc4`**.
- `git diff --name-only -- ARTIST` : **aucun delta suivi non commité** au contrôle. Ne pas en déduire que tout le dépôt est propre.
- Équipe native : **`mission-aaae4c9ca30bb8be`**.
- Répertoire de politique/rapports : **`.opencode/missions/team_mvm73qjo_0000_6wzibc/`**. Ce nom diffère de l'ID d'équipe : conserver les deux identités, ne pas les renommer.
- Politique native : **révision3 approuvée**, hash d'approbation `16384ce225b6a451fa55a81dc520ff236091dfb47d8dc370a8fef35200c77a5a`.
- `model-policy.json` reste au **format version2**. « approved v3 » désigne ici la révision d'approbation, pas une migration implicite au format3.
- Providers autorisés : `openai`, `zai-coding-plan`, `opencode`. **`opencode-go` retiré** après quota épuisé ; Cursor déjà exclu auparavant. Ne pas confondre `opencode` et `opencode-go`.
- QA : `proportionate` ; `startingIntelligence=50`, `classificationBeta=0.35`, classification native `manual`.
- SHA256 actuel du fichier de politique : `762ad6837788b24cc3b34af3721ba310e5852b4ba378798bc9d6bf7f1fc841c7`. Ce hash de fichier n'est pas le hash d'approbation natif ci-dessus.

**Équipe dans une nouvelle session :** vérifier que les outils natifs donnent réellement le rôle de coordinateur de cette équipe. Ne pas appeler `team_create` pour recréer/converter silencieusement la mission, ne pas modifier sa base interne ni forcer un modèle par spawn/message. Si la nouvelle session n'a pas la propriété native nécessaire, signaler précisément le besoin de rattachement/transfert autorisé ; ne pas inventer une API de transfert. L'ancien board peut rester attaché à la session précédente.

## 3. Cap, contraintes et sources à lire

Lire `AGENTS.md`, `docs/engineering-principles.md`, `docs/adr/README.md`, `docs/architecture/correction-plan.md`, puis les seuls rapports ciblés ci-dessous. Corpus documentaire QMD : `artist-wiki`, intent explicite.

Statuts ADR à maintenir : 0001/0002/0004/0006/0007 Proposed ; 0003/0005 Accepted/partial ; 0008 vérifié Windows Debug seulement. Pas de backend nouveau, RAII exhaustive, gain performance, validation GCC/Release/image présumés.

Contraintes persistantes : un écrivain par fichier, un build par sortie, GL sérialisé et bail entre processus ; runner lead séparé des profils QA. Pas de nouvelle installation/service/dépendance ni nouvelle seam/API F8. Pas d'usage des mocks pour conclure une preuve OpenGL réelle. Toute correction nouvelle : contrat et RED causal avant implémentation, plancher développement complexité70 ; risque distinct. Analyses seules peuvent rester sous70.

## 4. Livrables déjà intégrés et preuves acquises

### Préconditions

- H_A F8 : commit `354b53f`, rapports08/09. Sonde Windows lecture autorisée, close réel différé.
- Protocole ASan et17 GL : commit `4f8385e`, **rapport13-protocole-b2-et-recoupement.md**.
- H_Q préalable : commit `64d97dc3004d3aad2f69492e832da052b9d4d70a`, **rapport17-plan-qa.md**. Livré avant auteurs, ne pas le redemander.

### B6 — const-correctness

Commit **`ecb548b64da8dd932f9e201edf21d6f2005313e8`** : seul ajout `const` au helper privé `format_stacktrace`, sans changement d'API publique. Rapport23.

RED causal **transposé sous MSVC**, positif d'abord exit0, négatif const-this C2662/exit2, puis GREEN extrait du header fixé exit0. Ce n'est pas une validation native GCC/libstdc++. Non-régression MSVC ciblée2/2, zéro skip. Le défaut d'assertion fallback inconditionnelle en cas de future vraie branche GCC reste une limite future, pas une correction à entreprendre ici.

Le merge natif pool-11 n'avait rien capturé ; le lead a sauvegardé puis intégré le seul delta relu. Archives sous `out/qa-b6/`, dont `recovered-TraceableException.hpp`. Ne pas supprimer son worktree non réconcilié.

### B4-U — débind des trois tests

Commit **`9395b16`** : deux caractérisations et ProgramUnbinder existant dans UsePassTest/UseNextPassTest/ResetPipelineTest. Déclaration après owners, avant bind ; assertions F10 inchangées. Rapports21/22/29.

Caractérisation puis refactor réel **10/10 GL PASS**, zéro skip. Premier échec de littéral GoogleTest (newline finale manquante) conservé et corrigé après lecture de GTest effectivement lié : pas un RED moteur. Artefacts sous `out/qa-b4/characterization-20261024-{01,02}` et `out/qa-b4/refactor-20261024-01`.

### Helpers stricts et candidat H2

Plan/allowlist commit **`4c6cda2`**, rapport30. Implémentation/test commit **`318bf7a`**, rapport31.

`ARTIST/tests-common/include/TestUtils.hpp` compare le message entier par EXPECT_EQ et garde le catch par catégorie. Sept tests sous `ARTIST/tests/src/common/TestUtilsTests.cpp` : égalité, préfixe, suffixe, portion, absence, autre catégorie standard, exception non standard. RED observé2/7 (préfixe/suffixe acceptés à tort), puis GREEN7/7.

Consolidation de deux helpers locaux dans ClosedExecutionTests/PipelineCursorTests. Première non-régression555 :16 échecs d'attendus historiques partiels, conservés ; les attendus ont été complétés dans sept fichiers explicitement allowlistés, sans modifier la production ni relâcher les assertions.

**Résultat normal final de cette tranche :555/555 PASS =484 CPU+71 OpenGL, zéro failure/error/skip**, listes découverte/JUnit identiques. Artefacts complets :
- `out/qa-helpers/red-20261024-01/`
- `out/qa-helpers/green-20261024-01/`
- `out/qa-helpers/nonreg-20261024-01/` (16 FAIL historiques à préserver)
- **`out/qa-helpers/nonreg-20261024-02/`** : ctest.xml, LastTest.log, runner.log, discovery.json.
- Logs originaux `out/logs/helpers-nonreg2-{build,discovery,run}.log`.

Ne pas refaire les555 normaux sans changement pertinent ou preuve d'identité périmée. Ce résultat ne couvre pas le harness agentic concurrent non commité.

### Revue et generator

Correctness-reviewer pool-14, contexte neuf : revue statique de318bf7a reçue, **aucun défaut bloquant démontré** dans le scope. Rapport32 ; tâche `task_mvml0las_001d_p1gdy4` complétée après réception. Pas une exécution GPU/GCC ni l'audit test-reviewer final.

Generator test-engineer pool-16 : **rapport39-h2-generator.md complet sur disque, sept parties et résultat natif terminal reçus**. KEEP E5/E6, aucun patch supplémentaire au périmètre Windows actuel ; DEFER GCC/ASan final/F8, C2 et histoire B1 conservés. Aucune exécution/édition par ce worker. Le marqueur littéral FIN_H2_GAPS n'a pas été reçu séparément, mais les sept parties et le résultat terminal confirment la remise intégrale : ne pas redemander une analyse à cause de cette convention.

Rapport39 encore à relire/acter/commiter par le lead dans le flux de reprise. **La tâche de reprise generator n'a pas été marquée complète** pendant la pause : `task_mvnswi1s_0000_eufeda`. Sans patch accepté, H3 peut garder les mêmes sources318bf7a après intégration documentaire de la remise.

## 5. ASan : ce qui est fait et ce qui manque

### Fait — préflight sur H2, PAS campagne finale

Arbre séparé : `out/build/windows-msvc-debug-asan`. Cache Debug, ASanON, toolchain Conan existante, CheckCxx23 actif. Configuration et reconstruction **clean-first** des deux cibles et du probe effectuées. Environment canonique des enfants :
```text
ASAN_OPTIONS=halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0
ASAN_ACTIVATION_OPTIONS supprimée
```
**Conserver /RTC1, MDd, assertions et options Debug** : cmake/AddressSanitizer.cmake34–43/83 exige leur maintien, MSVC14.44 a compilé et le canari a prouvé l'instrumentation. Ne pas répéter l'ancienne hypothèse erronée « RTC1 doit être absent ».

Canari validé via `scripts/verify_asan.py` : sain exit0/marqueur attendu, faute exit86 **et** heap-use-after-free. Artefacts existants :
- `out/qa-b2-h3/asan-canary/{summary.json,healthy.log,use-after-free.log}`
- `out/logs/b2-h2-asan-{configure,clean,canary,discovery}.log`
- Découverte ASan **555 noms uniques**,17 historiques présents chacun exactement une fois.

### Empreintes observées pendant la passation (pas des empreintes datées du canari)

- ASan unit.exe `1472f35dd312dd369a8b80b637648a418706e8ce405ce21cd9a294eec361cd7a`
- ASan component.exe `b2bd0125d7ae8cc872c4a9a8a5d208377f19a3afd4a4ee6f330e6fd20dc63d0c`
- ASan artist_asan_probe.exe `14064d5b22846746d23794b4bb181c3e1b28cf0275eefcc1a50b73c2a941bfee`

Les trois exécutables sont dans l'arbre ASan ci-dessus, respectivement ARTIST/tests, ARTIST/component-tests et racine. Les archives sont ignorées par Git : conserver `out/` pour la nouvelle session, ne pas faire de clean/clone sans sauvegarde.

### À faire sur H3

1. Vérifier identité sources/configurations/outillage et validité du préflight conservé ; reconstruire seulement si nécessaire, pas pour remplir un rapport.
2. Découvrir/fixer la sélection avant exécution ; tous les CPU+GL prévus, aucune exclusion silencieuse,17 noms du rapport13 inclus exactement une fois. Archiver sélection et manifest candidat **avant** run. Le fichier de sélection finale n'a pas encore été créé ici.
3. En environnement normalisé depuis un processus dédié, exécuter le CTest ASan sélectionné séquentiellement, avec JUnit et sorties complètes. Comparer noms découverte/sélection/exécution, zéro failure/error/skip requis ; lire les diagnostics tardifs et conserver LastTest.log avant écrasement.
4. En cas d'échec : cause et frontière avant correction, pas d'assouplissement d'oracle ni de nouveaux reruns sans hypothèse.

Exemple du protocole13, **pas une commande exécutée par ce document** :
```powershell
$py = Join-Path $PWD 'out/tooling/build/Scripts/python.exe'
. ./scripts/asan-env.ps1
Set-AsanTestEnvironment
# Après avoir créé/contrôlé la sélection absolue et fixé H3 :
& $py out/tooling/run-build.py b2-h3-asan-selected ctest --preset windows-msvc-debug-asan --tests-from-file C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/asan-selected-names.txt --parallel 1 --no-tests=error --output-on-failure --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit C:/Users/djden/source/repos/ARTIST/out/qa-b2-h3/asan-selected.xml
```

Le wrapper `out/tooling/run-build.py` et `msvc-env.cmd` existent mais ne sont pas versionnés ; vérifier leur présence avant usage. Le premier impose2 jobs par défaut : passer explicitement parallel1. Ne pas lancer aveuglément build-windows.ps1, qui peut appeler pip/Conan --build=missing ; aucune installation nouvelle autorisée implicitement.

Toolchain observée lors des preuves : Windows x64 Debug, MSVC14.44.35207/19.44, CMake3.31.7, Conan2.12.2, Ninja1.13.0.git.kitware.jobserver-pipe-1. OpenGL réel observé RTX3090 /3.3.0 NVIDIA617.42. Pas de preuve image/performance.

## 6. Dettes à conserver ouvertes, pas de nouvelle tranche corrective implicite

### F8 / B3

Sonde LockFileEx autorisée exécutée par lead : baseline/recovery exact4608 ; sous verrou ReadFile échoue33/n0 mais ifstream open1,ios0,bad0,eof1,fail1,gcount0 ; Reader retourne normalement et publie vide (événements0,0). Cleanup vérifié, fixture absente, exit10. **Défaut causal candidat établi, catch ifstream réel non démontré.**

Rapports25/26 ; analyse architecte pool-13 reçue : les flags iostream seuls ne distinguent pas cette erreur d'un EOF normal ; size-check ne résout pas identité/TOCTOU/ABA. Ne pas rejeter fail() ou vide systématiquement. Remplacer ifstream par stdio/Win32 nécessite décision distincte sur erreurs, modes, ownership, fermeture et calibration. Aucun correctif Reader autorisé ou appliqué.

**Catch réel INCONCLUSIVE ; close réellement échoué DEFER/non couvert ; aucune clôture globale F8.** Reprise coordinateur+spécialiste build/OS sous dispositif/décision séparés. Artefacts `out/qa-f8/calibration-20261024-01/{f8-lock-probe.cpp,f8-lock-probe.exe,build.log,run.log}`.

### B1

477/477 CPU à l'époque du rapport24, en ordre normal, seed demandée20261017/effective61219, seed7 ; aucune sortie après PASSED final. Cela ne reproduit ni ne ferme l'incident historique176. GoogleTest mélange la population enregistrée entière : filtre176 sur binaire484/477 ne rétablit pas l'ordre historique avec la même seed. Aucun correctif sans RED causal ; ne pas multiplier les seeds au hasard. Disposition finale = limite explicite, pas cause résolue.

### C2 / B5

Historique20:35/20:38 : archives primaires absentes dans deux périmètres déjà recherchés, aucune preuve autorisant imputation moteur ou clôture. **OPEN/INCONCLUSIVE** ; pas de nouvelle chasse sans hypothèse/mandat. Pas de fermeture motivée acceptée par silence.

### B6

GCC/libstdc++ natif reste non validé. Oracle fallback actuel du test n'est pas adapté à une future vraie trace ; hors campagne Windows de ce lot, ne pas forcer les macros ni installer GCC ici.

## 7. Audit final et sortie

Après H3/campagne : test-reviewer **fresh et indépendant**, hors board d'implémentation visible au reviewer, reçoit contrat/candidat/preuves, pas les justifications auteurs. Diagnostic discriminant avant finding haut ; corrections au propriétaire puis ré-audit. Lecture seule, pas de shell/tests/édition autonome, runner lead distinct.

**La voie native d'admission de cet audit hors board n'a pas encore été validée.** Ne pas supposer que les champs du texte de tâche configurent le dispatcher, ni contourner la politique catalogue. Si le runtime impose un claim incompatible, signaler précisément le blocage au lieu d'accorder des droits ou de créer une fausse tâche achevée.

Sortie documentaire : actualiser au moins les incidents/build-validation/photographie de correction concernés dans docs selon faits vérifiés, préserver frontmatter/taxonomie/wikilinks, statuts ADR inchangés. Décrire explicitement les FAIL historiques résolus, INCONCLUSIVE/DEFER ouverts, les commandes/comptes/skips, identité binaire/source et limites de la campagne. Reindexer le seul artist-wiki si la documentation est modifiée, suivant AGENTS. Ne pas produire un PASS global moteur/GCC/F8.

## 8. Board natif : dette de statut, pas travaux à relancer

Snapshot du handover :26 tâches,6 completed,8 in_progress,12 blocked. Nombreux anciens producteurs/gates importés n'ont pas été réconciliés ; ne pas traiter les statuts comme preuve de travail actif ou d'absence d'intégration.

IDs utiles :
- generator livré pool-16 : **task_mvnswi1s_0000_eufeda**, encore in_progress ; accepter/commiter39 avant complétion.
- generator échoué pool-15 : task_mvml0lat_001e_u5r71a, garder inactif, pas réveiller/forcer modèle ni fausse complétion.
- correctness pool-14 : task_mvml0las_001d_p1gdy4, completed.
- B4-U : task_mvmhqczv_000b_5jc7pv, completed.
- B6 : task_mvmhqczw_000c_fm4fmx, completed.
- B1 : task_mvmhqczu_0009_sb99l2, in_progress historique, diagnostic sans correction.
- B3 : task_mvmhqczv_000a_iszkxc, in_progress historique, sonde/diagnostic remis.
- architecte F8 : task_mvmk6jba_0014_bp09tc, livrable reçu au26 mais statut non réconcilié.
- gate H3 importé : task_mvmbkkq8_000e_y5itru.
- campagne finale importée : task_mvmbkkq8_000f_86yru2.
- clôture importée : task_mvmbkkq8_000g_1vitry.

Les anciens alias B4/B6/helpers/H2 restent parfois blocked malgré vrais livrables intégrés. Réconcilier uniquement avec références de commit, sans déverrouiller une nouvelle exécution doublon. Ne pas marquer completed un échec ou une dette pour vider le board.

Pool-14 idle, pool-15 error, pool-16 idle au dernier contrôle. Ne pas réactiver les membres terminés pour des accusés inutiles. Un shutdown a déjà déclenché un doublon historique dans cette mission : pas de cycles répétés shutdown/réadmission. Tout cleanup de worktree modifié reste interdit sans réconciliation/sauvegarde/autorisation adaptée.

## 9. État non commité et prudence d'intégration

Le dépôt n'est pas propre. Changements suivis observés hors ARTIST/ :
- `.gitignore`, `.grepai/.gitignore`, renommage `.grepai/config.yaml`→`config.example.yaml` ;
- AGENTS.md, profils orchestrator/qa-planner/test-engineer/test-reviewer ;
- `.opencode/evidence/mission-gpu-qa/qa-policy-boundary.md` ;
- `.opencode/scripts/validate-agentic.mjs` ;
- contrats/manifests/fixtures des tests agentic t04-harness, t05-qa, t13-integration.

Des corrections concurrentes de l'opérateur/autres interventions ont été conservées. **Pas de git add -A, reset, clean, commit global ou annulation de ces fichiers.** Beaucoup de rapports et la politique sont également non suivis ; persister/commiter des chemins nommés seulement après lecture. Le présent handover est enregistré, mais pas commité par cette passation.

Profils QA : corrections autorisées vers31 règles identiques (communicationv2), execute/team_message/read/glob/grep/skill, edit/shell/subagent deny, autres MCP fermés. Lecture et transport réellement prouvés pour pool-16. Copies worktrees ont été synchronisées ; cela ne prouve pas le hot reload d'autres sessions.

**Limite harness non réglée :** validate-agentic.mjs distinguait encore planner31 des autres QA27 lors du rapport34. Ces fichiers ont des modifications concurrentes : relire le diff avant de conclure à leur état actuel. Ne pas annoncer PASS harness depuis les555 tests C++ ; si intégration des corrections profils/validateurs est requise, prendre une tranche bornée et testée (RED/GREEN adaptés, complexité>=70), sans effacer les obligations de sécurité ni copier aveuglément les assertions.

## 10. Incident de transport : résolu pour cette remise

Le rapport QA >10Ko était refusé par team_message ; un lead trop strict avait interdit les retries sans distinguer cette cause des refus de permissions. **La fragmentation a résolu la remise** : sept messages courts ont été reçus et écrits par le lead au39. Pour un futur long rapport : fragments numérotés, idéalement <4000 octets UTF8, appels séquentiels, puis résultat de fin. Ne pas accorder edit aux profils QA pour contourner une limite de taille.

`team_results({from:...})` a souvent renvoyé vide malgré notification ; un appel global a parfois récupéré le vrai contenu. Les messages du lead réaffichés ne sont pas des résultats worker. Ne pas réagir à chaque notification vide par une nouvelle relance ni déclarer une progression sur ce seul signal.

## 11. Context Mode : situation actualisée — ne pas reproduire le blocage

Après redémarrage, le serveur était activé/connecté (confirmé par capture et CLI) mais ses outils n'étaient pas exposés au catalogue Code Mode. L'absence de résultat de recherche n'était **pas** la preuve d'un serveur désactivé. Pas d'installation/réinscription/upgrade ou redémarrage service nécessaire établi.

**Mise à jour vérifiée pendant ce handover :** le catalogue expose de nouveau `tools["context-mode"].ctx_execute` et un appel réel a réussi, marqueur `CONTEXT_MODE_EXECUTION_OK`, avec lecture du HEAD/status et présence des artefacts. Ancien blocage d'exposition levé dans cette session. Vérifier une fois dans la nouvelle session, pas de boucle de diagnostic/reload si l'appel marche.

Avant scripts d'analyse non triviaux, lire `C:/Users/djden/.agents/context-mode-script-policy.md`. Utiliser chemins absolus et args en tableaux, sorties bornées et archives ; ne pas confondre script fautif et panne MCP. Aucun besoin de lire tout l'historique chat pour reprendre.

Si le CLI est nécessaire pour diagnostic : il n'est pas dans le PATH observé ; chemin découvert `C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.1.4-x64/opencode-cli.exe`. `mcp list` y indiquait context-mode connected. `/reload` est un rechargement natif de la même session, pas un nouveau chat obligatoire.

## 12. Ordre de reprise recommandé

1. Vérifier racine/HEAD/équipe/politique et disponibilité Context Mode, sans reconstruire.
2. Lire rapport39 et31/32 ; acter la remise, intégrer la documentation correspondante par chemins ciblés, fermer seulement la vraie tâche generator livrée. Figer H3, même source318bf7a si aucun patch accepté.
3. Établir sélection et identité ASan depuis les artefacts existants ; exécuter la campagne finale prévue sous bail, sans réinstaller ni répéter le TDD/les555 normaux pour la forme.
4. Organiser l'audit test-reviewer fresh indépendant dans une admission réellement compatible et permissionnée ; traiter findings puis ré-audit si nécessaire.
5. Traiter explicitement la cohérence profils/validateurs/docs et les statuts historiques, sans annexer les diffs concurrentes. Si un arbitrage de portée est nécessaire, présenter un rapport courant puis une question unique regroupée.
6. Bilan et clôture du lot à sa portée vérifiée, dettes F8/C2/B1/GCC encore explicites ; pas de nettoyage destructif ou push non demandé.

## Prompt à copier dans la nouvelle session

> Reprends et termine la mission ARTIST QA & harness existante. Lis `AGENTS.md` puis `.opencode/missions/team_mvm73qjo_0000_6wzibc/reports/42-handover-nouvelle-session.md` et applique son plan borné. Ce message est mon GO de reprise du flux déjà approuvé, pas une autorisation d'élargir F8, d'installer des dépendances ou de changer la politique modèles. Réutilise les preuves valides ; le generator est intégralement au rapport39, ne le relance pas. Vérifie l'accès natif à l'équipe existante sans la recréer. Context Mode a de nouveau fonctionné lors du handover : contrôle-le une fois, puis poursuis. Reste à intégrer la remise/H3, exécuter l'ASan final, faire l'audit fresh des tests et actualiser/clore le registre avec ses limites. Préserve tous les diffs concurrents et les worktrees ; aucun PASS global ou clôture F8/C2/GCC implicite.
