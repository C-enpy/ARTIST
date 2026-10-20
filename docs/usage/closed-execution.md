---
title: "Choisir une exécution fermée à deux passes"
category: usage
tags: [artist, documentation, rendering, ownership]
summary: "Utiliser ClosedExecution pour deux emprunts ordonnés, sans remplacer le curseur IPipeline."
created: 2026-10-20
updated: 2026-10-20
source_revision: ec9a9a641eef92c690d8ff97b94f81211da92ded
sources:
  - "ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp"
  - "ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp"
  - "docs/architecture/closed-execution-pattern.md"
  - "docs/references/session-2026-10-20-pipeline-execution-path.md"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Choisir une exécution fermée à deux passes

Utiliser `IPipeline` pour piloter le curseur (`use`, `useNext`, `reset`) et
consommer les passes possédées : [[usage/pipeline-and-passes]]. Utiliser
`ClosedExecution<API>` lorsque l'opération voulue est précisément **activer
deux passes dans un ordre fixé**, indépendamment de ce curseur.

Ce sont deux chemins coexistants, pas deux implémentations interchangeables
de la même opération. La chaîne n'est pas un hook de `use(index)` : y brancher
une activation double changerait le contrat d'un appel. Le
[[architecture/closed-execution-pattern|motif d'architecture]] explique cette
frontière et ses exclusions ; ADR-0002 reste Proposed.

## Reprendre la chaîne Classic des tests réels

Extrait contigu 1:1 de
`ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp`, test
`PipelineTest.ClosedExecutionBindsProgram1AndLeavesInitialCursor`, lignes
247–264, au SHA `ec9a9a641eef92c690d8ff97b94f81211da92ded`.

```cpp
    GlLeaseWitness lease(glContextAlive);
    auto loaded = makeLoadedPipeline();
    ASSERT_NE(loaded.program0, 0);
    ASSERT_NE(loaded.program1, 0);
    ASSERT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    ASSERT_EQ(glIsProgram(loaded.program1), GL_TRUE);
    ASSERT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);

    pipeline::ClosedExecution<api::OpenGL> chain(loaded.pipeline.forPass(0), loaded.pipeline.forPass(1));
    ProgramUnbinder unbind; // destroyed first, before the owners, on every path
    ASSERT_NO_THROW(chain.run());

    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program1);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    EXPECT_EQ(glIsProgram(loaded.program1), GL_TRUE);
```

Il s'agit d'un **extrait de corps de test**, non d'un exécutable autonome.
Reprendre la fixture `PipelineTest` et ses helpers dans le fichier cité :
`makeLoadedPipeline()` construit un pipeline Classic possédant deux passes
Classic chargées ; `GlLeaseWitness` contrôle le bail du contexte ;
`ProgramUnbinder` appelle `glUseProgram(0)` en sortie, y compris après un
retour anticipé d'assertion. Les namespaces `pipeline` et `api`, les includes,
le contexte GLFW/GLEW et les chemins des shaders viennent du même fichier.
Le premier parcours se trouve dans [[usage/getting-started]].

## Responsabilités de l'appelant

- Le pipeline reste propriétaire des passes en `unique_ptr`. La chaîne ne
  contient que deux références `IPass<API>&` : ni programme, ni contexte,
  ni ownership transféré. Les deux passes doivent survivre à la chaîne et à
  chacun de ses appels ; ne pas conserver la chaîne après leur destruction.
- Construire la chaîne n'active rien. `run()` appelle la première passe puis
  la seconde. Le curseur reste inchangé, qu'il soit à -1, 0 ou 1 ; les tests
  composants correspondants vérifient ces trois états.
- Ne pas supposer que `reset()` délie le programme : il rembobine le curseur.
  La déliaison du test est explicite et distincte.
- Si la première activation échoue, la seconde n'est pas lancée. L'exception
  est propagée sans enveloppe ; il n'existe **pas de rollback GPU** des
  activations déjà réussies. La chaîne n'ajoute ni cache de handle ni état de
  reprise : chaque activation repasse par `pass.use()`.
- Le contexte OpenGL doit rester courant sur le thread d'appel pendant les
  activations et le nettoyage. La chaîne n'ajoute aucune synchronisation.
  L'ordre de destruction C++ du test garantit son bail de contexte, pas la
  fin arbitraire de travaux GPU au-delà du scope ou de la frame.
- L'appelant choisit les références ; aucun contrôle de leur identité n'est
  promis. N≠2, ordre variable par frame ou topologie pilotée par la scène ne
  sont pas des usages généralisés par ce type.

## Ce que les preuves disent réellement

`ClosedExecutionTests.RunActivatesFirstThenSecondOnce`, dans
`ARTIST/tests/src/graphic/pipeline/ClosedExecutionTests.cpp`, vérifie l'ordre
CPU avec `InSequence`. Le readback GL ci-dessus vérifie le programme final,
**pas à lui seul l'ordre intra-chaîne**. Les deux familles se complètent.

La [[references/session-2026-10-20-pipeline-execution-path|session de référence]]
consigne 546/546 tests Windows x64 Debug au candidat `fdbce15` (477 unitaires,
69 OpenGL). Les blocs sont recoupés au SHA documentaire indiqué ; aucun
nouveau run moteur n'est revendiqué ici. Pas de preuve Release, ASan, autre
OS ou équivalence d'image. Le virtuel `IPass::use` demeure : **aucun gain de
performance ni dévirtualisation n'est revendiqué**.

Voir aussi [[usage/uniform-attribute]] ; retour [[index|accueil du vault]].
