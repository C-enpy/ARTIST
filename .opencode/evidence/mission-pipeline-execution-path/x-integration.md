# Intégration X — preuves (2026-10-20)

- **Auteur** : `cpp-x` (profil `cpp-engineer`), branche `ensemble-35541b43-7e32-41a6-a12c-962d7d846ba5` :
  - `36c6a13` **x-red** — tests du contrat seuls (+105 `ProgramTests.cpp`, +253 `ClosedExecutionTests.cpp`, aucun CMake, aucun type) ;
  - `ac692db` **x-green** — le type seul (+52 `ClosedExecution.hpp`).
  - Rapport final perdu (capture `final_response`, 6ᵉ occurrence de la panne transport) ; travail préservé dans le worktree, mergé puis committé ici par le lead.
- **Fichiers intégrés** : `ARTIST/include/graphic/pipeline/ClosedExecution.hpp` (nouveau) ; `ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp` (nouveau) ; `ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp` (+105). **Gelés intacts** (vérification diff vide : `Pipeline.hpp`, `Pass.hpp`, `PipelineContext.hpp`, `component/pipeline/User.hpp`, `Resetter.hpp`).
- **Baseline (lead, sur `e47c920` avant X)** : **530/530 PASS** (466 unit + 64 opengl), 0 FAIL — `out/baseline-x-2026-10-20.log`.
- **RED reproduit par le lead sur les tests de l'auteur** : header masqué → build unit échoue (exit 1) : `fatal error C1083: Impossible d'ouvrir le fichier include: 'graphic/pipeline/ClosedExecution.hpp': No such file or directory` à `ClosedExecutionTests.cpp(20)` — `out/mission-x/red-repro-build.log`. Ordre causal prouvé par l'historique (`x-red` sans le type précède `x-green`).
- **GREEN intégré (lead, runner)** : **543/543 PASS** (474 unit = 466+8 ; 69 opengl = 64+5), 0 FAIL, 0 skip — `out/mission-x/green-integration.log` (comptages console recoupés). JUnit par label : runs de **l'auteur** dans son worktree, copiés en évidence attribuée (`junit-unit-cpp-x-run.xml`, `junit-opengl-cpp-x-run.xml`) — citation antérieure vers `out/mission-x/junit-*.xml` du principal **corrigée** (écart E1, audit G).
- **Registre (mise à jour)** : O1, O2, O3 (pré-états 0 et 1, unit+GL), O4, O5 (k=1 et k=2), O6 (passes réelles moved-from, variantes p0/p1), O8, O14 → **COVERED (executed)** ; O7 re-confirmé par run ; O9 baseline+candidat exécutés ; O10 placement ✓ (`include/graphic/pipeline/`) ; **O11–O13 → revue R (pending)**.
- **Tests nouveaux** : `unit.ClosedExecutionTests.*` (6), `unit.ClosedExecutionPassTests.*` (2), `opengl.PipelineTest.ClosedExecution*` (5, dont O14 = `ClosedExecutionUnloadedSecondPassBindsZeroWithoutThrowing`).

---

## Corrections d'audit (E1/E2) et mise à jour registre — 2026-10-20

- **E2 — identité des runs** : baseline sur `e47c920` (log `out/baseline-x-2026-10-20.log`, SHA256 `ABC2E29537BCEBEC…`) ; RED reproduit sur `ef009fa` (log `out/mission-x/red-repro-build.log`, SHA256 `C552758E9885FC27…`, exit 1, `C1083` à `ClosedExecutionTests.cpp(20)`) ; GREEN sur `ef009fa` (log `out/mission-x/green-integration.log`, SHA256 `9C6F5403384F3FE0…`, exit 0, 543/543).
- **E1 — JUnit** : les JUnit par label (`junit-unit.xml` 474/0, `junit-opengl.xml` 69/0) proviennent des runs de l'auteur dans son worktree ; copiés ici en `junit-unit-cpp-x-run.xml` / `junit-opengl-cpp-x-run.xml`.
- **Registre après R ∥ G** : O11–O13 → **COVERED par revue statique indépendante** (r-report.md, verdicts A–I PASS, aucun bloquant). Réserves R-1 (RAII d'unbind sur échec ASSERT) et R-2 (`glIsProgram` ré-assert) : optionnelles, tranchées au registre. Propositions G : P1 DEFER recommandé, P2 proposée (coût S), P3–P7 REJECT. I-A : GAP-accept confirmé dans sa portée exacte.
