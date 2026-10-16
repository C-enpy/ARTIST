# T13 — raccordement GPU + QA au coordinateur existant

Date : 2026-10-16. Auteur/exécutant : **integrate-gpu-qa**, build-engineer, feuille.
Tâche `task_mvbgob62_000f_d1v9zg`, PM/LLM brut = retenu **C83/R80**, inchangés.
**PASS borné des contrôles locaux ; mission/native/GPU INCONCLUSIVE.**

## Candidat et périmètre

- Input imposé : `1821af67ce75f838d2ae35349a1085efa8d3168b`.
- Admission observée propre, détachée à `83d7bdd1896701c3bd051e36fdff0048128293d2`.
  Session déplacée vers **son seul worktree assigné**, puis `git switch --detach
  1821af67ce75f838d2ae35349a1085efa8d3168b`, branche neuve créée sans écraser de ref.
- Branche : `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-integrate-gpu-qa`.
- Candidat source testé propre : **`89bf66f19bd6ef648b5a7d43f68ce668d7f9d722`**,
  14 fichiers, 899 insertions / 11 suppressions contre input. Le commit suivant
  ajoute seulement ce rapport et `validation-report.md` ; il ne modifie pas les
  entrées testées. Les hashes ci-dessous identifient le candidat sans auto-référence.
- Base opérateur six fichiers déjà importée **82edafe**, provenance
  `operator-base/manifest.json`. Diff de ces six fichiers entre 82edafe et input :
  vide. Les **162 lignes ajoutées non vides** de l'import sont retrouvées dans le
  candidat après normalisation des retours de ligne ; une ligne du paragraphe
  toolchain/routage a été repliée, pas retirée. Ce n'est pas une prétention
  d'identité octet à octet des six fichiers désormais enrichis.
- Aucun accès d'écriture à main/worktree lead ; aucune copie de base canonique
  dirty. Les prescriptions GPU.md/QA.md canoniques ont seulement été lues.
- Écrivain unique T13. Aucune sous-délégation, session modèle, canari, installation,
  indexeur GrepAI/QMD, mutation globale/politique/ensemble.json, C++/GPU/build ou migration.
  Aucune ressource Docker créée. Les travaux A1c/Readers de la base ne sont pas
  attribués à cette mission.

Changements : catalogue manuel v1 (19 domaines, 4 API, 12 choix/minima, oracles
nominal/négatif), manifest union explicite, raccordement instructions/commandes,
seul paragraphe PM séparant les risques, correction **textuelle** Conan 2,
synthèse Obsidian/liens/journal, tests locaux et plan natif non exécuté. Aucun
routeur/helper de production ajouté, aucun T04–T12 modifié. Le helper Git T04 est
réutilisé intact. Le manifest union borne les lectures, pas l'ownership d'écriture.

## Matrice réellement exercée

- **Windows win32 x64**, Node **v24.11.1**, `C:\Program Files\nodejs\node.exe` ;
  Git **2.52.0.windows.1**, `C:\Program Files\Git\bin\git.exe`. C'est Node qui
  exécute les tests, pas le Bun du sandbox Context Mode. PATH/Node/Git sont des
  entrées de confiance ; environnements Git des fixtures isolés par T04.
- T04 final : **355/355 PASS**, fail/skip/cancel = 0.
- T05–T12 final : **170/170 PASS**, fail/skip/cancel = 0. Détail recoupé aussi par
  lot : 58 + 9 + 15 + 16 + 12 + 22 + 22 + 16.
- T13 final : **63/63 PASS**, fail/skip/cancel = 0. Donc **588 tests distincts**
  de harness/contrats, pas 588 tests moteur. Répétitions non additionnées.
- Union T13 : syntax **41**, schema **81**, offline **215** contrôles PASS,
  zéro fail/skip/inconclusive au niveau demandé ; **missionVerdict INCONCLUSIVE**.
  Trois QA reconnus forme B / `static-presence`, pas enforcement.
- Huit manifests producteurs : offline PASS, comptes respectifs
  **100, 95, 94, 98, 104, 107, 109, 102**, tous missionVerdict INCONCLUSIVE.
- Répétition union offline sur le même candidat : JSON **identique en bytes**,
  exit 0 deux fois. Delta des entrées validées vide, SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `node --check`, `git diff --check` : exit 0 ; arbre source propre au run final.
  Contrôle documentaire borné : 4 notes avec champs frontmatter présents,
  37 wikilinks résolus, 0 cassé (hors `[[nodiscard]]` historique cité en code).
  Pas un parseur YAML/Obsidian universel ; tags de la nouvelle note tous existants.
- **Non exercés** : CMake/compilateur/STL C++23/ABI, configuration Conan, CTest,
  GPU/image/performance/ASan, Linux/Apple/CI distante. CMake, presets, Conan, CI,
  ADR-0005/0008 et guide build ont été inspectés : Conan 2 CMakeDeps/CMakeToolchain
  avant CMake, Ninja dans les presets, cibles ArtistLib_tests et
  ArtistLib_component_tests. Aucune version de compilateur/STL ni découverte
  CTest n'est validée par ce lot ; aucune fausse compile_commands.json fournie.

## Commandes exactes reproductibles

Depuis la racine du worktree ci-dessus, en PowerShell ; aucune installation :

```powershell
node --version
git --version
node --test --test-concurrency=1 --test-reporter=tap .opencode/tests/agentic/t13-integration/contract.test.mjs
node .opencode/tests/agentic/t04-harness/run-tests.mjs all
$lots = @('t05-qa','t06-vulkan','t07-d3d12','t08-metal','t09-opengl-debug','t10-pipelines-shaders-simulation','t11-resources-graph-portability','t12-assets-performance-validation')
$tests = $lots | ForEach-Object { ".opencode/tests/agentic/$_/contract.test.mjs" }
node --test --test-concurrency=1 --test-reporter=tap $tests
foreach ($mode in @('syntax','schema','offline')) {
  node .opencode/scripts/validate-agentic.mjs --mode $mode --root . --manifest .opencode/tests/agentic/t13-integration/manifest.json
  if ($LASTEXITCODE -ne 0) { throw "T13 $mode exit $LASTEXITCODE" }
}
foreach ($lot in $lots) {
  node .opencode/scripts/validate-agentic.mjs --mode offline --root . --manifest ".opencode/tests/agentic/$lot/manifest.json"
  if ($LASTEXITCODE -ne 0) { throw "$lot exit $LASTEXITCODE" }
}
node --check .opencode/tests/agentic/t13-integration/contract.test.mjs
git diff --check
```

Les mêmes exécutables/arguments ont été lancés via Context Mode, stdout/stderr
capturés sans troncature dans `out/t13-integration/`, jamais deux runners simultanés
sur cet output. Les huit producteurs ont aussi été exécutés individuellement avec
les mêmes options Node. Le test négatif `native-evidence` utilise seulement un
JSON vide de test `{schemaVersion:1}` dans une fixture jetable : **pas** une
observation ni un dossier natif candidat. Il vérifie exit 2 ; avec un lien absent,
exit 1 reste prioritaire. Les autres mutations vérifient missing skill, collision,
lien absent, traversal, frontmatter malformé, permission QA élargie.

## Première cause, corrections bornées, avant/après

1. Tests écrits avant les documents/configs : **61 contrôles / 61 FAIL** sur
   l'input non raccordé. Première cause : `catalog-contract.json` absent ; clauses
   du coordinateur/entrées QA et Conan 2 manquantes également détectées. C'est un
   contrôle de configuration avant modification, **pas un RED comportemental**.
2. Après raccordement : **54 PASS / 7 FAIL**. Première cause causale analysée via
   `ctx_execute_file` : `git add` de la fixture échoue `Filename too long` sur
   `artist-build-validation/references/validation-levels.md` ; exit 128. Ce n'est
   ni RED produit ni défaut de T04. Correction bornée : `mkdtemp` sous le répertoire
   court approuvé `%TEMP%/opencode/t13-*`, au lieu du worktree profondément imbriqué.
   Helper Git, garde d'identité, isolement et assertions **inchangés** ; cleanup
   seulement du répertoire retourné par mkdtemp. Aucun réglage Git/global/subst.
3. Après cette correction : **61/61 PASS**. Ajout de deux négatifs explicites
   (frontmatter du coordinateur et FAIL prioritaire avec natif indisponible) :
   **63/63**, puis non-régression finale 588/588 sur le commit source propre.
4. Aucun nouveau helper/routeur de production à rendre GREEN. Les modifications
   sont documentaires/config ; les tests et mutations ciblent des contrats
   statiques et le CLI T04 **inchangé**, pas une exécution de la boucle native.
   Aucune assertion/tolérance/skip des producteurs affaiblie. Erreurs d'échappement
   dans des appels Context Mode d'analyse corrigées dans la session, pas panne MCP.

## Conservation et preuves qualifiées

Comparaison bytes contre input : **10/10** fichiers sentinelles inchangés :
validate-agentic.mjs, git-fixture.mjs, les trois profils QA, Readme.md,
CMakeLists.txt, CMakePresets.json, conanfile.txt, ensemble.json. Diff contre input
limité aux 14 chemins autorisés ci-dessus ; sources/skills/T04–T12 et politique
exclus. Le paragraphe PM est un ajout seul. Les 53 crédits restent en fin du README.

Serena initialisé et racine active ARTIST canonique confirmée, jamais réactivé vers
le worktree. QMD recherché dans artist-wiki puis note lue ; le corpus canonique ne
contient pas ce delta non intégré. Aucun watcher/indexeur/embedding lancé. Docs V2
agents/permissions consultées via Context Mode le 2026-10-16 ; déclarations ≠ gates.

Le ruling **Runner séparé** est conservé : QA texte/patch ; lead mandaté distinct
persiste, relit et exécute. Autonomie dégradée explicitement consentie, pas prouvée
autonome ; aucun grant par lot ni canari testé. Gaps détaillés dans
`validation-report.md`, propositions seulement dans `native-probe-plan.md`.
T14 et revue fresh finale restent au lead après intégration ; cet auteur ne
s'auto-attribue pas leur tâche et ne qualifie pas son auto-contrôle d'indépendant.

## Empreintes SHA-256

Sources du candidat testé (chemins relatifs à la racine) :

```text
7c9bedd8d1ecfd13475d50854d9cc11e78a11abe78255c20b1cdd473df34aa50 .opencode/agents/build-engineer.md
a9e62667d1b9dbce8a52c46129df9046beeb8944612e492d7b73b918ad586487 .opencode/agents/orchestrator.md
7647e6b3a32225e0aec5bd098686142df2c786870ac2fede9b78fc8de00958e7 .opencode/agents/project-manager.md
7d47215ba7c0fbafa9f854dc5e32abe336ea897b4489881770a6d0b5360bb2bf .opencode/commands/engine-mission.md
642259cec68a61d5e53334eef7de8588f59c26cdb7c95c341e21a3b6b1296749 .opencode/commands/engine-tdd.md
9564ae5bfc928ba9ba2641f7500861147e386c0666be5d2928ee8f567ec78686 .opencode/evidence/mission-gpu-qa/catalog-contract.json
ce838707fb6740cca63bdafe93930f5b445c9d6922e81d6b7899b720b1253fde .opencode/evidence/mission-gpu-qa/native-probe-plan.md
60ec5ac203d6a8264a9fac3bf8e5842b2a3117dea0332eec1b5590746d7a8499 .opencode/tests/agentic/t13-integration/contract.test.mjs
7333b61351952f831878d5a96f38a9310174e11268b082710f5f094a83c08680 .opencode/tests/agentic/t13-integration/manifest.json
cd5e23f76baffa1d285d41ae9aecaabe823ff61b9a848a7dc35e90fcd2e57c03 AGENTS.md
3514bcfa59e911cad560e1664254c274893f1514630b953e24d7563945d7a552 docs/agentic/gpu-qa-harness.md
b5132c5bc95c5af95ce33b28e25ae4dc2336ff4b8cc7ca61d334d09ff4d0340d docs/index.md
f8b1ad65fbc4229157cf1af06ba5d4863ee525e81071d9a00cd633cec99aefd6 docs/log.md
da50422594ed77749bb62f059575210ccc45149bc231b9a68853afbb1c10c730 docs/opencode-harness.md
```

Logs privés préservés sous `out/t13-integration/` (à recueillir par le lead avant
retrait du worktree ; non versionnés, les commandes restent reproductibles) :

```text
b21f861399c5d80551cd32223f08283734a6462ba9a9e9fdbd43fe13e06a4400 before-config.tap
a787cda35e5f1c56447cb3a5955870313a2b4cf3d7ea0921bdc7e6cd97a4ec9c after-config-01.tap
225f43d5cdb054156de6e21ede5e8ee0099d5134c1302421f230b944f8d6038c after-config-02.tap
b97ca87bd9850d80e24b7d6dd5477ec4790cd5a1a8764337ac9b23fae0f20563 after-config-03.tap
139762e6c26bec147bd8810d8cd86fb1baaa8dcf088caac7ab63e3beac3b7c58 final-t04-all.tap
3673ac21d9b4f7c5aace7fac4a27263bebc1ce59b1c929e75bbe350c9174b92e final-producers.tap
806d3d1836e5f715670426ed60d040feb7c213ccb7a08c53fb96eca1c427f7b6 final-t13.tap
48374e81a725dae2ca59455c37fd0f2c1cc12bd599b5100de4bff8a4b6c15093 final-t13-integration-syntax.json
3985a49edb5773f85014c5b7ec644d84dadeba01542e6112babbac548f280fb4 final-t13-integration-schema.json
f57e78205560bba1a3a90f9ce569390384d987815823bf5c8a093d4ba85122a8 final-t13-integration-offline.json
11f0deba973276fdbcae3d501e219863ef1f1acd0417662249422d903d1fbed7 final-basic-checks.json
c06385d87a43205d6afd36e8686ab3736ddc6939ae74f06445de33428238c502 document-check.json
```

Inputs versionnés sous `.opencode/evidence/mission-gpu-qa/` :

```text
4d1a0a97016591feab9348c9b72e628c54e8453468d3c9403167d49d5c3071ec t03-contract.md
4827762c8ac452b4e7980f374d828403d8e0fb134469c9230a4e01b76cf97c12 qa-plan.md
29c53a0ff22fd46001741a21bec3e9cdf70871694629d9d53b9a2e500ca0d1d0 t02-external-sources.md
6debdd200a28c5d2fadc95cd41ad807ac99e2abb7c3586e862855d1ed4df535f t02-source-inventory.md
b906b1265fadd35cac736197f9cccce9ad88c65227363405c6c2b36ab987c510 qa-policy-boundary.md
```

Inputs canoniques lus seulement : GPU.md
`be03e5600ecdbfbf6ae58e04f0c2df6433bfadae59d549ca9d2c2f3b134a8873`,
QA.md `4b62a9c68b9a0d32c962260f115e866591043bd7681330377d006734c6a8d870`,
execution-brief.md `9c14954e66e535e7b34a906367f3b1b27d8a1a212f4b91fa076ab641d0846818`.
