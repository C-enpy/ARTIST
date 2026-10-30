# Linux intégré — PLAN borné et TESTS_RED_READY

Auteur pool-24 / `task_mvul0bd4_0053_ptc60a`, 30 octobre 2026.
**Arrêt avant GREEN. Aucun test exécuté par l'auteur.** Les assertions ci-dessous
sont candidates à la revue puis au RED causal par le coordinateur, pas une preuve
de RED déjà obtenue. Aucun résultat du brouillon pool-19 n'est repris ou copié.

## Admission et état vérifié

- Worktree : `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-24`.
- Branche : `ensemble-e15edd1f-f0b4-477a-9077-9a6445817f53`.
- HEAD initial exact : `8c7e51b28ae085ada2eb98c97b292fa7147e2988`, propre après
  déplacement explicite de la session hors de la racine canonique. Aucun fichier
  de cette racine canonique modifié par l'auteur.
- Core intégré `94e4111`, protocole `describe_preparation(plan, *, bundle_root)`
  constaté ; rapports 82, 85, 95 et contrat/QA §11/binding Windows relus.
- Fondations Release `d2d7be7` déjà présentes : ni refaites ni modifiées.
  Réemploi des preuves lead 6 Release + 19 historiques du rapport82, sans les
  attribuer au présent candidat. Les fichiers historiques restent inchangés.
- Mandat PM/LLM reçu : complexité brute/retenue78, risque71, incertitude moyenne,
  plancher70 ; pas de réévaluation/routage de modèle ici.
- QA §11 était persistée avant l'auteur ; sa description de l'ancienne signature
  est une photographie antérieure, pas la signature désormais intégrée.

## Périmètre et plan après revue du lead

1. Relire ces tests et leurs imports/doubles ; exécuter le RED via Context Mode,
   avec HEAD/delta/SHA256, commande/cwd/interpréteur, logs et comptages distincts.
   `linux.py` est **un scaffold nominal non validant** : méthodes retournant `()`
   et collecteur `{}`. Il évite un faux RED `ImportError`, sans implémenter une
   commande, une sonde, une écriture ni un cache « dernier bundle ».
2. Après accord lead et RED causal : implémenter uniquement l'assemblage pur dans
   `scripts/verification/adapters/linux.py` et les deux modes bornés du helper
   `scripts/build-linux.py`, puis GREEN/non-régression par le lead. Aucune phase
   GREEN n'est autorisée par la présente remise.
3. Revue indépendante ciblée : pureté, provenance des reçus, confinement et
   séparation des phases ; corrections/ré-audit avant intégration lead.
4. L'intégrateur futur enregistre les cibles CMake optionnelles ; la qualification
   native reste séparée, ultérieure et **NON_EXECUTE** dans cette mission.

Fichiers exclus : core/base/model/runner/reporting/verify, C++, workflows et toute
plateforme Windows. `scripts/linux-evidence.py` et `CMakePresets.json` ne nécessitent
aucune édition proposée ici ; si un besoin apparaît, le proposer au lead **avant**
édition. Pas de copie d'un brouillon ou de changement des oracles historiques.

## Proposition de surface helper (à accepter avant GREEN)

- `--build-only`, avec `--target <cible>` répétable, obligatoire et dédupliqué en
  conservant l'ordre. Un seul appel `cmake --build --preset linux-<configuration>
  --target <cibles...> --parallel <jobs>`. Aucun Conan, configure, CTest, canary
  ASan, création de répertoire de logs ou workload implicite. Le mode ne configure
  rien et exige un arbre déjà préparé ; un échec natif reste un échec.
- `--verification-workloads` exige `--configure-only`, ajoute seulement
  `-DARTIST_BUILD_VERIFICATION=ON` après le preset CMake. Incompatible avec
  `--build-only`. L'option CMake est déjà celle du fichier source-only
  `ARTIST/verification/CMakeLists.txt` ; **pas de nouveau nom concurrent**.
- `--configure-only` sans ce nouveau flag garde son argv historique exact.
  Le mode par défaut garde install/configure/build/CTest, les noms de presets,
  les gardes ASan, les codes et le comportement existants. Les cibles ne sont
  acceptées qu'en build-only pour éviter une modification implicite du défaut.
- Debug/Release installent Conan2 host **et** build avec le même `build_type`,
  depuis `conan/profiles/linux-gcc`, avant le premier CMake. ASan garde
  `linux-debug-asan` et ses options historiques ; Release+ASan reste refusé.
  Build-only ASan ne prétend pas avoir exécuté la canary ni qualifié ASan.

Ces choix sont testés comme **proposition explicite**, sans édition du helper
dans cette remise. Aucun ancien test/signature/oracle changé. Si la revue
modifie cette surface, persister le mapping avant adaptation des tests.

## Adaptateur et preuves futures

- Constructeur/descriptions : calcul lexical depuis source_root absolu et
  plan/reçus, sans `resolve/stat/open`, processus, réseau, sonde d'environnement,
  création de fichier ou cache mutable de bundle. `bundle_root` est explicite ;
  Linux n'a pas besoin du transport JSON Windows. Le core reste propriétaire des
  bundles/leases/receipts/logs/captures. Pas de nouveau schéma public.
- Préparation décrit le helper configure-only opt-in et déclare l'effet
  `dependency-resolution` ou `conan-install`, pour le refus du core avant le
  runner si `allow_dependency_resolution` est faux. Build décrit le même helper
  build-only sur l'union ordonnée des cibles sélectionnées, sans effet bootstrap.
- Arbre unique conforme à `evidence._build_root` :
  `source_root/out/build/linux-debug` ou `linux-release`. Ni arbre Windows, ni
  configuration changée dans un reçu, ni second cache de build/toolchain.
- Exécution : chemins du mapping `BuildReceipt.details.executables` et identité
  admise, jamais un nom d'exécutable deviné comme autorité. Appels directs via
  `build_workload_argv`, options visibles et sorties distinctes sous `bundle/raw`.
  ID de requête déterministe par invocation à partir du run_id, unique entre
  routes/scénarios et CTest. Pas de helper-run, seconde capture ou wrapper.
- CTest : une découverte puis une sélection par regex échappée/ancrée de l'union
  exacte des noms du plan, mêmes `--test-dir` et `-C`, `--parallel 1`,
  `--no-tests=error`, JUnit privé. Le core garde ses gardes historiques et son
  recoupement découverte/JUnit ; pas d'appel au `main` de linux-evidence.
- CPU : aucun GL/Xvfb dans argv/env_delta. Software : demande explicite
  `LIBGL_ALWAYS_SOFTWARE=true`, `GALLIUM_DRIVER=llvmpipe`, observation llvmpipe
  future obligatoire. Hardware : cible vendor/renderer exacte et six variables
  `ARTIST_VERIFICATION_{CONTEXT_V1,RUN_ID,PLAN_SHA256,REQUEST_ID,
  EXPECTED_GL_VENDOR,EXPECTED_GL_RENDERER}` explicites sur chaque CTest exécuté.
  Les observations réelles futures doivent encore qualifier cette cible ; aucun
  PASS déduit de « non-llvmpipe », RTX ou de l'absence d'une variable logicielle.
- Précondition opérateur pour **toute** qualification GL future : DISPLAY/X
  server déjà accessible, droits et pilotes préparés. Hardware ne doit pas
  hériter d'un forçage logiciel. Aucune sélection/réparation d'affichage, aucun
  lancement/install de serveur X et aucun fallback logiciel dans ce raccordement.
  Le runner existant supervise le primaire, pas une garantie de cleanup de tous
  descendants ; aucun superviseur supplémentaire proposé.
- `collect_environment` est la seule frontière de sondes de cet adaptateur :
  versions `g++-14/cmake/conan/ninja --version` via argv, timeout <=15s, métadonnées
  OS/architecture/Python et variables allowlistées, jamais dump d'environnement
  ou secrets. Outil absent = indisponibilité explicite, pas une version inventée.
  Ces versions seules ne prouvent ni STL complète, ni GPU, ni compilation.

## Tests candidats et confinement

- `test_binding_adapter.py` : assemblage prépare/build, cibles ordonnées,
  pureté/répétabilité, bundles permutés, mapping avec espaces et refus de mauvais
  arbres, vrai builder d'argv, routes/identités/sorties distinctes, profils,
  sélection/discovery CTest, six variables hardware, collecte allowlistée/outil
  absent. B1/B3/B4/B6, risques R5 et attribution/effets de QA §11.
- `test_binding_helper.py` : flag configure explicite, Conan host/build avant
  CMake, build-only sans install/configure/test/log implicite, déduplication,
  debug/release/ASan, erreurs et combinaisons invalides. B4/B6, R5.
- `test_binding_composition.py` : vrais plan/receipts/sink/core et parser CPU ;
  refus opt-in avant helper, phases séparées/capture, reçu étranger refusé,
  brut partiel conservé, JSON CPU synthétique attribué. B1/B3/B4/B5/B6.
- `binding_fixtures.py` : uniquement fixtures possédées à espaces, faux runner
  **en mémoire**, aucune délégation à un exécuteur réel. Les bytes d'exécutables
  sont du texte non exécutable. Tous Popen/run/call/check_*/os.system/spawn et
  socket sont bloqués avant chaque fixture. Seuls les deux tests de collecte
  et les tests helper remplacent `subprocess.run` par un double strict en mémoire.
  Le helper est importé en mémoire sous garde, jamais lancé comme processus.
- Les imports projet utilisés sont actuellement purs (catalogue/modèle/core/
  protocol/scaffold) ; aucun module de workload natif n'est importé.
- Les assertions de rejet CLI peuvent déjà passer car les flags n'existent pas ;
  ce ne sont **pas** les oracles RED causaux. RED attendu sur les cas positifs :
  « adapter must describe ... not an empty scaffold », état prepare non COMPLETED
  avec cause « adapter described no command », ou flag helper positif non reconnu
  capturé comme assertion explicite, jamais ImportError ou toolchain manquante.

List/plan consultatifs : réemploi des tests et garanties core intégrés du rapport95,
sans modification du dispatcher ni duplication de leurs oracles. Les tests Linux
ci-dessus n'utilisent aucun fallback process ; les commandes CTest et workloads
ne sont ici que des données examinées.

## Remise au coordinateur : commandes non exécutées par l'auteur

Dans ce worktree, après revue des imports/doubles et acquisition du runner lead,
via Context Mode avec sorties persistées sous un nouveau `out/verification-synthetic/` :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p 'test_binding_*.py' -v
```

Cette commande **est pour le lead uniquement**, pas une commande de campagne
Linux. Attendu : population non vide, plusieurs assertions RED causales décrites
ci-dessus. Le nombre réel et la première cause appartiennent au journal du lead.
Conserver aussi les échecs, erreurs et SKIP ; ne pas les transformer en GREEN.

Après GREEN autorisé seulement : même filtre puis `test_release_contract.py`
inchangé. Historiques 9+10 sur copies exactes confinées comme rapport82, jamais
directement sur une arborescence partagée. Les suites core/contexte/scénarios
restent au runner lead selon le diff effectivement intégré et ses dépendances.

## Matrice effectivement exercée et limites

- Windows/worktree pool-24 : Git et lecture source ; contrôle AST sans import de
  modules candidats/tests seulement, `C:/Python313/python.exe` **3.13.1**, code0 :
  cinq fichiers Python syntaxiquement valides, **34 tests déclarés** (17 adaptateur,
  11 helper, 6 composition), aucun test exécuté. Pas d'unittest, helper, enfant
  factice ou programme moteur lancé. Contrôle syntaxique ne prouve ni import ni
  comportement. Le script `-B -c` exécuté était uniquement :

  ```python
  import ast, pathlib, sys
  root = pathlib.Path("C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-24")
  paths = [root / "scripts/verification/adapters/linux.py", *sorted((root / "scripts/tests/verification_linux").glob("*binding*.py")), root / "scripts/tests/verification_linux/binding_fixtures.py"]
  paths = list(dict.fromkeys(paths))
  print("AST_ONLY Python", sys.version.split()[0])
  total = 0
  for path in paths:
      tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
      tests = sum(isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)) and node.name.startswith("test_") for node in ast.walk(tree))
      total += tests
      print(path.relative_to(root).as_posix(), "AST_OK", "tests_declared=" + str(tests))
  print("TOTAL_DECLARED_NEW_TESTS", total)
  print("No candidate imports or tests executed")
  ```
- Tests Python synthétiques du nouveau raccordement : **NON_EXECUTE par auteur**.
  RED/GREEN causal et non-régression : **INCONCLUSIVE**, attente runner lead.
- Linux Debug/Release/ASan, compilateur/STL/générateur/ABI réels : **NON_EXECUTE**.
  Déclaré seulement : Linux x86_64, GCC14/libstdc++11 ABI, cppstd23, Ninja,
  Conan2 CMakeDeps/CMakeToolchain ; versions directes conanfile inchangées.
  La sonde `cmake/CheckCxx23.cmake` doit encore compiler/lier format/stacktrace
  sur le futur hôte ; aucune base compile_commands valide produite ici.
- CPU moteur, OpenGL logiciel/matériel, images, mesures, install/cloud :
  **NON_EXECUTE** et interdits à cette tranche. Aucun gain revendiqué.
- Serena partagée confirmée sur racine canonique ARTIST, LSP C++ seulement ;
  absence de couverture Python et delta isolé analysé par fichiers/AST, pas de
  diagnostics symboliques inventés ni de réactivation du projet partagé.

Première cause exécutée du nouveau raccordement : **non observée**, aucun test
lancé. Cause source anticipée : adaptateur non implémenté et modes helper absents.
Remise **TESTS_RED_READY**, pas pipeline Linux livré/validé ; attente revue et RED
du lead, puis autorisation GREEN. Aucun outil d'équipe ou sous-agent utilisé.
