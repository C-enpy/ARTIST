---
title: "Audit V — lifetime OpenGL 2026-10-19"
category: architecture
tags: [artist, architecture, rendering, testing, ownership]
summary: "Audit test-reviewer fresh du candidat 71dacbb : K1, K1-migration, K2, K3, K5, bornes H et diff des tests COVERED. PASS borné Windows Debug, pas une preuve exhaustive."
status: completed
implementation: verified
source_revision: 71dacbb40ad8746fce5e57cd3cbecc2e1fa8a493
created: 2026-10-19
updated: 2026-10-19
sources:
  - "opencode:ses_eac13149bffeGZlfxc6nQ0sv3f"
  - "references/session-2026-10-19-opengl-lifetime.md"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Audit V — candidat `71dacbb`

Reviewer : test-reviewer, session `ses_eac13149bffeGZlfxc6nQ0sv3f`, hors board
d'implémentation. Candidat natif accepté par le lead :
`result_mvf8gjec_008t_9t7qc0`. Le premier candidat `result_mvf81glz_008o_dxg58x`
était INCONCLUSIVE (bornes H et diff non fournis). Aucune écriture ni
exécution par le reviewer.

Le lead a recoupé le diff tests/mocks avant acceptation. Moteur
`71dacbb40ad8746fce5e57cd3cbecc2e1fa8a493`. HEAD documentaire au moment de
l'attestation : `a7b63ef` (docs et transcripts seulement). Binaire unitaire
`0a952ff3005336893ca8bb34841903838f96a98ac72bcfaa63aa9b521ecc2109`, binaire
OpenGL `68714279b3a0234ea62d72a43bf5e991e6b2b2ed2f2b30e3d926e7f34f66759c`.

## Campagne fraîche

Runner = lead, Windows x64 Debug, `scripts/build-windows.ps1 -Configuration Debug -Jobs 1`.

- Unit : 466/466, 0 échec, 13,01 s (`out/attest/utf8/build-unit.log`).
- OpenGL : 64/64, 0 échec, 27,98 s (`out/attest/utf8/build-opengl.log`).
- `ninja: no work to do` sur ce binaire.
- Un `ctest` lancé depuis la racine du dépôt sans `--test-dir` a écrit
  « No tests were found!!! » (exit 8). Exclu des preuves.

## Verdicts

Tous les critères du registre audité sont **COVERED** : K1, K1-migration, K2,
K3, K5 (baseline historique 326/326 sur `cd1b598` et non-régression 530/530),
bornes H, audit différentiel des tests et mocks.

**PASS borné de l'audit des tests**, Windows Debug seulement. Ce n'est pas une
preuve exhaustive de correction, de lifetime GPU physique, d'ASan, de Release,
d'image ou de performance. ADR-0003 reste Accepted/partial. Aucun ADR n'est
promu.

### Bornes H, 2026-10-19T14:24–14:25

Même SHA256 unitaire avant et après. Manifeste
`out/attest/utf8/h-bounds-manifest.txt`.

- Graines 1 et 3 : échec puis création, 2/2.
- Graine 4 : création puis échec, 2/2.
- `LoadShaderFailureTest` isolé : 1/1.
- Shuffles 7 et 61219 : 466/466 chacun.

### Différentiel recoupé

`out/attest/utf8/diff-tests-mocks-cd1b598-71dacbb.diff`, SHA256
`b4d1394cc2ec23b11950a31c6253d91a90cb908a3ac14a80334bd4efd35b68b7`.

- `PassTests` : `get<float>()` devient `get<GLint>()` plus un readback `glGetUniformiv`.
- DuplicateName : la publication de préfixe (size 1, ID 7, attribute ID 0) est
  remplacée par une map vide ou par la génération précédente conservée.
- Échec tardif Uniform : size 3 remplacé par l'ancien size 2, identité et valeur
  conservées.
- `GTEST_SKIP` ajouté seulement dans le `#else` de `GTEST_HAS_DEATH_TEST`.
- Le garde fp64 ajoute `GLEW_VERSION_4_0` comme alternative à l'extension ; les
  points d'entrée restent exigés. Le test natif de la campagne fraîche est Passed.

Aucune mission suivante n'est sélectionnée.
