# T3 — UniformReader (auteur t3-worker-3)

## Candidat et protocole déclaré avant le driver réel

Base 05fef285b7cfac98886b4358a93906ea78d950fd ; G1 delta=0, untracked=0.
SHA256 Reader 046D77C5D0D4ECA47B590074CA78F8A5182E1984C70E99F42DF86620764EC24F,
concept 564D8C68D4FA27D308ADA6F0710B05BF226A5DBD9A5336A7F67596FAF5B85CFA.
Worktree `ensemble-artist-mcp-lifecycle-mission-5caa4a18f43401c7#eiyn0g-t3-worker-3`.
Baux lead : cache/tooling canonique réutilisé sans copie ; GL et ASan exclusifs.

Build initial sous worktree : C1083 chemins longs, **pas RED**. GO opérateur/lead :
arbre court `C:/Users/djden/source/repos/ARTIST/out/mission-f7/t3-build` (pas subst).
VsDevCmd/vcvarsall 14.44 et commandes dans le même processus ; un job.
Logs, JUnit, commandes exactes : `out/mission-f7/t3/` du worktree.

RED concept : 3 C2338 au motif absence/borrowed/void (`04-red-concept-short.log`).
Pour isoler les prochains RED sans toucher la production, TU temporairement `.cpp.red`
hors GLOB ; assertions restaurées intégralement au GREEN, re-glob obligatoire.
RED mocks : `05-red-mocks-reglob.log`, 7 découverts, 4 échecs causaux (nom long,
collision préfixe, location -1, doublon exact), 3 verts (nominal, ID0, zéro uniforme).
Les 2 derniers cas P3 sont des caractérisations, pas des RED artificiels.

**Attendu GL écrit avant exécution** : sélection native
`opengl.UniformReaderComponentTest.*` ; nominal : `testUniform` présent,
location différente de GLuint(-1), GL_INT ; nom long actif de 324 caractères : nom
complet présent, location native valide et GL_INT. Code non corrigé attendu : nominal
PASS, nom long FAIL sur présence du nom complet (buffer fixe 256).
`GL_ACTIVE_UNIFORM_MAX_LENGTH` mesuré et imprimé, pas déduit du GPU.
Tous les owners meurent avant TearDown ; witness avant pass, handles par valeur,
observation GL_CURRENT_PROGRAM, glUseProgram(0) avant destruction ; aucun glFinish/draw.

**Sélection ASan déclarée avant toute exécution ASan** : tous les tests `unit.`
du candidat final + uniquement les suites GL lifetime historiques
`ShaderLifetimeComponentTests` (9), `PipelineLifetimeTest` (8), `PipelineTest` (5),
noms confirmés par découverte native (ces derniers sont les tests du TU ProgramTests).
Sélection attendue : 223 unit + 22 GL = 245 ; exclus 23 GL : AttributeComponentTest(3),
PassTest(7), ShaderTest(10), UniformReaderComponentTest(3).
Nouveaux tests GL UniformReader, non-lifetime,
exclus ; les autres GL non-lifetime exclus explicitement. Aucun renommage de niveau
de preuve : ASan ne remplace pas GL natif. Build ASan distinct sous `t3-build/asan`,
clean-first, canari reconstruit 0 puis 86 ; options par processus
`halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0` ; sélection absolue
`--tests-from-file`, premier FAIL conservé. Pas de campagne parallèle.

## Contrats et décisions locales (à compléter au rendu)

Reader monomorphe Classic, on(PassContext&) emprunté, aucune refonte du dispatch.
Pass possède les wrappers Uniform et leurs contextes uniques ; noms copiés dans la
map, buffer temporaire non retenu. Les emprunts invalidés avec owner/reload ne sont
pas prolongés. Pas de transfert GPU nouveau ; réflexion sous contexte courant sur
thread de test ; pas de partage concurrent promis. Durée d'utilisation GPU distincte
du scope : les tests ne soumettent aucun draw et débindent le programme explicitement.
Garantie visée : basique (uniforms déjà publiés conservés lors d'une erreur suivante),
pas de remplacement silencieux sur doublon ; pas de rollback global ajouté.

Précision doublon : garde locale à UNE réflexion, pas aux entrées préexistantes.
`Pass::load` documente free shaders + pass.free + reload ; Freer laisse les maps en
place. Une garde globale initiale a régressé ce contrat : 2 FAIL (mock + GL réel),
`09b-red-reload.log`. Ensemble local corrigé, anciens wrappers remplaçables à la
réflexion suivante ; leurs emprunts invalidés. L'itération reste monomorphe, noms
runtime ; allocations buffer/ensemble sur chargement, aucun gain de performance revendiqué.

Durcissement metadata borné : taille de nom donnée par GL, longueur et NUL vérifiés
avant accès/publication ; location reste GLint jusqu'au rejet de -1 ; doublon exact
rejeté avant remplacement. Taille positive de l'uniform transmise à `setSize`.
P3 §1/addendum A2 signale cette omission : RED taille=2 observée à 0, pas UB démontrée
sur ce chemin (make_unique<T>() value-initialise) ; 0/-1 rejetés. `10-red-size.log`,
3/3 FAIL avant ce correctif. Noms malformés : 5/5 FAIL avant correction dans
`07-red-malformed.log` (vide/négatif/hors capacité/NUL interne/terminateur incohérent).

Premier non-rég : `11-green-native-all.log`, 268 découverts, 267 PASS, 1 FAIL.
`ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders` utilise la vraie
chaîne mockée ; son mock générique répond 1 au nouveau MAX_LENGTH et écrit ensuite
un nom de longueur 12. Adaptation des DONNÉES mockées : ON_CALL exact ID/pname avec
sizeof(UNIFORM_NAME), trois attentes destruction intactes ; glFunctionMock inchangé.
Les 45 GL natifs étaient déjà PASS, zéro skip. Premier FAIL/JUnit conservés.

Assertions finales mockées RENFORCÉES après RED (jamais affaiblies) : MAX_LENGTH
exactement une query si uniforms présents (zéro si vide), nombres de lookup exacts
avant rejet ; malformed names zéro lookup. AtMost temporaire du RED permettait de
montrer les sorties erronées du vieux Reader sans faire de la nouvelle query l'oracle.

Erreurs d'invocation conservées et hors RED : script d'environnement/redirect initial,
regex CTest non quotée (`09-red-reload.log`, commande pipe PowerShell), puis corrigée
dans `09b`. Aucun échec d'outil n'est compté comme preuve métier.

## Résultats finaux et registre obligations → preuves → qualification

- **R1 PASS / executed + compile** : 3 C2338 RED au motif exact dans
  `04-red-concept-short.log`, puis TU restauré sans affaiblissement ; positif + 3
  négatifs compilent, prédicat runtime vert sur le candidat final.
- **R2 PASS / executed CPU** : `15-green-unit-reader-final.log` et
  `green-unit-reader-final.xml` : 17/17, zéro skip. Garde ID0/vide caractérisée ;
  nom long/collision/location/doublon, 5 noms malformés, taille et reload couverts.
  Ownership unique T1 conservé, buffer prêté uniquement aux appels driver synchrones.
- **R3 PASS / executed GL natif** : 45/45 GL dans la suite complète ; nouveaux
  UniformReaderComponentTest nominal/long/reload (3), tous verts. Population attribute
  couverte par `opengl.PassTest.AttributeReadingTest` exécuté dans cette même suite.
  `GL_ACTIVE_UNIFORM_MAX_LENGTH=325` mesuré ; noms complets 324 octets présents,
  IDs comparés à glGetUniformLocation réel, GL_INT et count=1. Locks CTest
  `artist_opengl` vérifiés dans la découverte pour les 3 nouveaux GL.
- **C1/C2 PASS / executed** : `12-green-native-final.log`,
  `green-native-final.xml` : **268/268 = 223 unit + 45 GL**, zéro skip.
  Découverte JSON et noms JUnit égaux exactement. Sources G2 avant/après identiques
  sur les 7 fichiers C++/fixture ; binaires hachés, voir `g2-{before,after}.json`.
  Limites stdout JUnit passé/échoué 65536. Re-glob à chaque ajout/restauration de TU.
- **C3 PASS / executed ASan auteur** : clean-first du probe dans arbre distinct,
  `13-asan-configure-clean-canary.log` + `asan-canary/` : sain 0, use-after-free 86,
  status PASS. `14-asan-build-selected.log`, `green-asan.xml` : **245/245 = 223 unit +
  22 GL**, zéro skip ; noms JUnit = sélection pré-déclarée exactement. Découverte
  ASan = native (268) ; les 23 autres GL sont exclus, pas skips. Sélecteur inchangé,
  SHA256 `2b1fd7b14574813d4cd4963cdd231f6c85a4df09510e09260b856196a4860b39`.
- **C4 PASS / registre local** : REDs et premier échec non-rég conservés ; aucune
  couverture mockée renommée GPU ; aucune preuve performance/image fabriquée.

Commandes principales (environnement MSVC 14.44.35207 / SDK 10.0.26100.0 chargé dans
le même processus ; CMAKE_BUILD_PARALLEL_LEVEL=1 ; cache Conan sous bail) :

```powershell
$b = 'C:/Users/djden/source/repos/ARTIST/out/mission-f7/t3-build'
conan install . -pr:h conan/profiles/windows-msvc -pr:b conan/profiles/windows-msvc -s:h build_type=Debug -s:b build_type=Debug -c:h tools.cmake.cmaketoolchain:user_presets= -of $b --build=missing
cmake -S . -B $b -G Ninja -DCMAKE_TOOLCHAIN_FILE="$b/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DMOCK_GL=ON -DARTIST_ENABLE_ASAN=OFF
cmake --build $b --parallel 1
ctest --test-dir $b -L unit -R UniformReader --output-on-failure --no-tests=error
ctest --test-dir $b --output-on-failure --no-tests=error --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <absolute>/green-native-final.xml
# ASan : Conan installé de même dans "$b/asan", configure identique avec ASAN=ON.
cmake --build "$b/asan" --target artist_asan_probe --clean-first --parallel 1
python scripts/verify_asan.py "$b/asan/artist_asan_probe.exe" --log-dir <absolute>/asan-canary
cmake --build "$b/asan" --parallel 1
ctest --test-dir "$b/asan" --tests-from-file <absolute>/asan-selection.txt --output-on-failure --no-tests=error --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <absolute>/green-asan.xml
```

`<absolute>` = chemin absolu `out/mission-f7/t3/` du worktree donné en tête.
Commandes exactes développées dans chaque log. Politique ASAN_OPTIONS par processus
et suppression ASAN_ACTIVATION_OPTIONS appliquées par `out/mission-f7/t3/run.ps1`.
Auto-relecture : diff production + callers Pass::load/readUniforms/Freer/Setters,
tests nouveaux et adaptation ShaderLifetime relus ; `git diff --check` sans erreur.

## Limites et handoff

Ce résultat n'est **pas une intégration ni une revue indépendante** : diff à relire
par le lead avant barrière T4, puis vérification intégrée. Aucun Shader/Loader/Freer,
Pass.hpp dispatch, context, backend, ADR ou glFunctionMock modifié. Pas de glGetError
ajouté à la production, pas d'extension D8 nécessaire. Pas de garantie forte globale,
de concurrence, de draw/image, de performance, de Release ou autre plateforme.
Contexte courant/thread et publication entière du chargement restent préconditions.
Comportement antérieur conservé : reload remplace les noms retrouvés, mais ne purge
pas les anciens noms absents de la nouvelle réflexion ; politique de génération et
publication atomique à arbitrer hors cette tranche, pas corrigée silencieusement.
ASan n'instrumente pas Conan/driver et n'établit pas synchronisation GPU ni absence
d'UB générale. Baux GL/ASan/Conan rendus au lead dans le message de résultat final ;
aucune ressource Docker créée. Artifacts ignorés à préserver avant retrait du worktree.
