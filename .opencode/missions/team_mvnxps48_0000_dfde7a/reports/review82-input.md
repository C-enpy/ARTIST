# Entrée de revue indépendante — candidat82bf52d

Source C++ intégrée82bf52d52e594fae02593866669a1af715127d1f. Entrée documentaire dans la racine canonique ARTIST autorisée ; ne pas retenter le chemin de worktree refusé. Pas de shell/test/édition. Aucun rapport de justification auteur nécessaire.

Contrat : mêmes assertions handles5/8, ordre/count detach5/8 → deleteProgram5 → deleteShader8 ; action nominale ON_CALL préservée ; buffer production non-NUL et contrôle de terminaison maintenus. Vérifier la compatibilité et les oracles, avec scénario discriminant pour tout finding bloquant. Cette entrée n'est pas une preuve d'exécution.

Provenance `git show82bf52d:<path>` recalculée par lead :
- ShaderLifetimeTests.cpp : a9290122f57b4c29c4b55a664fd5f0f9352e523748365d8ce807bfb9cdfe84ce
- glFunctionMock.hpp : b225c167a670c11637089fa93a00a0e6c2a88f821a73427936989274aa904e17
- AttributeReader.hpp :663f5c34e4b9f25c779c8d0b74d3cea2414fec3e488eab507955e8b5c9d37eab
- Diff git2a2e40e..82bf52d du seul TU :9ffe99f411687ec6c6f333a562258465cdc37b6eb2e04d50ba0fec95decde281

## Delta C++ exact, seul ajout

Dans `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp`, juste après les deux attentes CreateShader/CreateProgram :

```cpp
    // This is an output buffer, not a C string until the default action fills it.
    // An uninteresting-call diagnostic would print its non-NUL staging contents.
    EXPECT_CALL(gl, glGetActiveAttrib_mock(5, 0,
                                          static_cast<GLsizei>(sizeof(mock::opengl::glFunctionMock::ATTRIBUTE_NAME)),
                                          ::testing::NotNull(), ::testing::NotNull(),
                                          ::testing::NotNull(), ::testing::NotNull()))
        .Times(1);
```

## Test englobant et fixture

Le test complet peut être lu à la racine canonique explicitement autorisée : `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp:347–382`. Contexte condensé, sans changement sémantique :

```cpp
TEST_F(ShaderLifetimeTests, PassDestructorReleasesProgramThenShaders)
{
    std::vector<std::unique_ptr<pipeline::IShader<api::OpenGL>>> shaders;
    shaders.push_back(makeShader("test-datas/shaders/vertex/good/minimal.vert"));
    auto passContext = std::make_unique<context::OpenGLPassContext>();
    passContext->addShader(std::move(shaders.back()));
    auto &gl = *mock::opengl::glFunctionMock::instance();
    EXPECT_CALL(gl, glCreateShader_mock(::testing::_)).Times(1).WillOnce(::testing::Return(8u));
    EXPECT_CALL(gl, glCreateProgram_mock()).Times(1).WillOnce(::testing::Return(5u));
    // Le seul ajout est le delta exact ci-dessus, ici.
    {
        testing::InSequence sequence;
        EXPECT_CALL(gl, glDetachShader_mock(5, 8)).Times(1);
        EXPECT_CALL(gl, glDeleteProgram_mock(5)).Times(1);
        EXPECT_CALL(gl, glDeleteShader_mock(8)).Times(1);
    }
    {
        ClassicPass pass(std::move(passContext));
        pass.load();
        ASSERT_EQ(pass.getContext().getPassID(), 5u);
        ASSERT_EQ(pass.getShader(0).getContext().getShaderID(), 8u);
    }
}
```

TearDown appelle glFunctionMock::reset(), lequel appelle `testing::Mock::VerifyAndClearExpectations(instance().get())`. makeShader(path) renvoie un ClassicShader(path, ShaderType::VERTEX).

## Mock existant, extraits utiles

`ARTIST/tests/mock/opengl/glFunctionMock.hpp:385–429/512`. Espacement et commentaires condensés :

```cpp
static constexpr char ATTRIBUTE_NAME[] = "attribute_test";
// Action par défaut glGetProgramiv :
case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH: *params = sizeof(ATTRIBUTE_NAME); break;
case GL_ACTIVE_UNIFORM_MAX_LENGTH: *params = sizeof(UNIFORM_NAME); break;
case GL_ACTIVE_ATTRIBUTES:
case GL_ACTIVE_UNIFORMS:
case GL_LINK_STATUS:
case GL_INFO_LOG_LENGTH: *params = 1; break;
// Action par défaut glGetActiveAttrib_mock, existante :
ON_CALL(*this, glGetActiveAttrib_mock).WillByDefault(
    [this](GLuint program, GLuint index, GLsizei bufSize, GLsizei *length,
           GLint *size, GLenum *type, GLchar *name) {
        const char* mockAttribName = ATTRIBUTE_NAME;
        strncpy(name, mockAttribName, bufSize);
        if (length != NULL) { *length = strlen(mockAttribName); }
        (*size) = 1;
        (*type) = GL_FLOAT;
    });
MOCK_METHOD(void, glGetActiveAttrib_mock,
            (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *), ());
```

glEntry<Method>(function) enregistre le nom dans glCalls_ puis forward les arguments à la méthode mock. Aucun changement de ce contrat dans le diff.

## Production existante, extrait exact pertinent

`ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp:68–79` :

```cpp
                // Non-NUL fill makes a missing terminator observable, instead of
                // accepting it accidentally because the allocation was zeroed.
                std::vector<GLchar> attributeName(static_cast<std::size_t>(maxNameLength), '\x7f');
                GLsizei nameLength = 0;
                GLint size = 0;
                GLenum type = 0;
                glGetActiveAttrib(passID, i, maxNameLength, &nameLength, &size, &type, attributeName.data());
                if (nameLength <= 0 || nameLength >= maxNameLength || attributeName[static_cast<std::size_t>(nameLength)] != '\0')
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::PASS::ATTRIBUTE::INVALID_NAME");
                }
                const std::string name(attributeName.data(), static_cast<std::size_t>(nameLength));
```

Les fichiers canoniques ci-dessus sont explicitement désignés comme entrées si une lecture supplémentaire est utile. Les snippets condensés ne sont pas des fichiers compilables ni un remplacement de la preuve d'identité. Restituer la couverture et ses limites ; aucun PASS runtime présumé.
