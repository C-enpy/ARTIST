# Round F — patchs de gaps acceptés (2026-10-20)

- **Generator** : `test-engineer` (sous-agent direct, LECTURE SEULE respectée — patchs texte seulement), session `ses_ea6a778b1ffeERgKT7GybYo2N5`, HEAD de génération `f5623d6`. Application : lead (persistance/exécution — runner séparé, attribution lead).
- **Items appliqués** (tous acceptés en chat par l'opérateur) :
  - **P1/T-1** — `RunOnCrossedChainsOverTheSamePassesReplaysFreshly` (CPU) : deux chaînes croisées sur les mêmes passes, InSequence par invocation p0,p1 | p1,p0 ; anti-mémoïsation (identité de passe/état statique ⇒ FAIL) ; surcharge 5-args de `makeSilentChain` (sonde Times(0) + VerifyAndClear).
  - **P2/T-2** — `RunAfterFirstPassFailureRestartsCleanly` (CPU) : deux phases, `VerifyAndClearExpectations` entre phases ; pas d'état d'échec persistant/latch ⇒ phase 2 rejoue p0,p1 sans exception.
  - **R-1/T-3** — garde RAII `ProgramUnbinder` (namespace anonyme) dans les 5 tests GL, déclaré dernier local ⇒ détruit en premier sur tout chemin de retour (y compris ASSERT_*) ; remplace les 5 `glUseProgram(0);` finaux ; commentaire de fixture mis à jour.
  - **R-2/T-4** — `ASSERT_NE(program0, 0)` + `ASSERT_EQ(glIsProgram(program0/1), GL_TRUE)` ajoutés avant `run()` dans les tests GL 2/3/4 (forme `ASSERT_EQ` assumée par le generator, convention du fichier).
- **Aucun RED attendu** (comportement déjà tenu) ; **tout échec = DEFECT**, aucun constaté.
- **Run intégré (lead, runner)** : **545/545 PASS, 0 FAIL, 0 skip** — unit **476** (474+2), opengl **69** — `out/mission-x/green-f-round.log`, exit 0. Discovery ClosedExecution : #150–159 unit (10), #513–517 opengl (5).
- **DEFER enregistré (registre, hors lot)** : le même pattern d'unbind skippable existe dans les 3 tests préexistants (`UsePassTest:170`, `UseNextPassTest:192`, `ResetPipelineTest:214`) — même classe de défaut, périmètre préexistant, extension non décidée par ce lot ; owner : mission harnais ultérieure ; impact : scénario de test échoué seulement, comportement GL défini.
- **Comptages** : baseline 530 (`e47c920`) → X 543 (`ef009fa`) → F **545** — delta final = 15 tests ClosedExecution (10 unit + 5 GL).
