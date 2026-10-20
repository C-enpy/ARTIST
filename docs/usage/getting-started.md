---
title: Premiers pas avec ArtistLib
category: usage
tags: [artist, guide, rendering, cpp]
aliases: [getting-started]
summary: "Build Windows Debug et chaîne Classic à deux passes des tests OpenGL."
status: source-checked
source_revision: 1005dff0804e18c412129843e944d7f1f55a0f48
created: 2026-10-20
updated: 2026-10-20
sources: [ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp, CMakePresets.json, scripts/build-windows.ps1]
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Premiers pas avec ArtistLib

[[index|Accueil]] · [[usage/pipeline-and-passes|Pipeline et passes]] ·
[[usage/uniform-attribute|Uniform et Attribute]] · [[usage/closed-execution|Exécution fermée]].

## Construire

Prévoir Windows x64 Debug, MSVC **19.44 / toolset 14.44** et SDK Windows,
Python, CMake **≥ 3.25**, Ninja et **Conan 2**. GLFW et GLEW sont résolus
par Conan ; les tests composant exigent un contexte réel OpenGL 3.3 core.

Depuis la racine : `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1`.
Cette commande d'outillage (pas un snippet C++) est recoupée avec
`scripts/build-windows.ps1` et `CMakePresets.json` : Debug, un job et tous
les tests par défaut, preset `windows-msvc-debug`, Conan avant CMake.
Voir [[development/build-and-test]] pour les versions et limites.

**Testé historiquement** : 546/546 (477 unitaires + 69 OpenGL) au SHA
`fdbce15dcc05663cc58618a844d931b07b84750f`, Windows Debug.
**Contrôlé ici** : extraction des sources au `source_revision`, sans relance.
`ProgramTests.cpp` est inchangé entre le SHA testé et cette révision.
Ni Release, ni autres OS, ni performance ne sont promis.

## Deux passes Classic chargées

Tous les blocs sont des extraits contigus **1:1** de
`ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp`.
Ils s'utilisent dans le harnais GoogleTest existant, pas comme un `main`
autonome : la fixture `OpenGLComponentTest` initialise GLFW/GLEW et le
contexte ; les chemins `test-datas/` sont ceux des tests. Les includes et
alias complets restent dans le fichier source. Types et fabrique utilisés
par les tests `PipelineTest` :

```cpp
    using ClassicPass = pipeline::Pass<api::OpenGL, profile::Pass::Classic>;
    using ClassicPipeline = pipeline::Pipeline<api::OpenGL, profile::Pipeline::Classic>;

    /// Builds a classic pass owning its freshly created shaders.
    ClassicPass makeClassicPass()
    {
        std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/vertex/good/minimal.vert", context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<api::OpenGL, profile::Shader::Classic>>("test-datas/shaders/fragment/good/minimal.frag", context::ShaderType::FRAGMENT));
        return ClassicPass(std::move(shaders));
    }
```

`PipelineTest::makeLoadedPipeline` charge avant transfert ; `LoadedPipeline`
contient le pipeline et deux handles copiés pour observation, jamais owners :

```cpp
    LoadedPipeline makeLoadedPipeline()
    {
        ClassicPass first = makeClassicPass();
        ClassicPass second = makeClassicPass();
        EXPECT_NO_THROW(first.load());
        EXPECT_NO_THROW(second.load());
        const GLuint program0 = first.getContext().getPassID();
        const GLuint program1 = second.getContext().getPassID();
        return LoadedPipeline{ClassicPipeline(makePassVector(std::move(first), std::move(second))), program0, program1};
    }
```

`PipelineTest::makePassVector` effectue le transfert :

```cpp
    static std::vector<std::unique_ptr<pipeline::IPass<api::OpenGL>>> makePassVector(ClassicPass first, ClassicPass second)
    {
        std::vector<std::unique_ptr<pipeline::IPass<api::OpenGL>>> passes;
        passes.push_back(std::make_unique<ClassicPass>(std::move(first)));
        passes.push_back(std::make_unique<ClassicPass>(std::move(second)));
        return passes;
    }
```

`PipelineTest.UsePassTest` observe le programme réellement lié :

```cpp
    ASSERT_NO_THROW(loaded.pipeline.use(0));
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 0);

    ASSERT_NO_THROW(loaded.pipeline.use(1));
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program1);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 1);

    glUseProgram(0); // the test unbinds before its owners are destroyed
```

`PipelineTest.UseNextPassTest` montre que `false` peut signifier « dernière
passe exécutée », non « rien exécuté » :

```cpp
    ASSERT_TRUE(loaded.pipeline.hasNext());
    ASSERT_TRUE(loaded.pipeline.useNext());
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);

    // Last useful call: the last pass stays selected and bound.
    ASSERT_FALSE(loaded.pipeline.useNext());
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program1);
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), 1);

    glUseProgram(0);
```

`PipelineTest.ResetPipelineTest`, après `use(0)`, distingue reset et unbind :

```cpp
    ASSERT_NO_THROW(loaded.pipeline.reset());
    // F10: reset rewinds the cursor only — it neither unbinds the current
    // program nor destroys it.
    EXPECT_EQ(loaded.pipeline.getContext().getCurrentPass(), -1);
    EXPECT_EQ(glIsProgram(loaded.program0), GL_TRUE);
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), loaded.program0);

    glUseProgram(0); // the test unbinds explicitly before destruction
```

Les tests créent `loaded` par `makeLoadedPipeline()` dans leur corps et
utilisent `GlLeaseWitness` : owners créés après `SetUp`, détruits avant
`TearDown`, contexte vivant et courant. Ces extraits omettent ce préambule ;
consulter les tests complets pour l'exécution. La fin du scope C++ ou de la
frame ne prouve pas la fin de travaux GPU asynchrones.
