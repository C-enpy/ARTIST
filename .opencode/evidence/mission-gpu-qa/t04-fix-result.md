# T04-R — correctifs de revue et isolation Git

Date : 2026-10-16. Auteur : fix-harness-review / build-engineer. Tâche
`task_mvbejo41_0003_bd8y9u`, C76/R80, sans sous-agent ni override de modèle.
Revue indépendante source : `review-harness.md`, F1–F4 ; hypothèse additionnelle
bornée d'identité Git, nommée F5 ci-dessous. Pas de qualification indépendante de
ce correctif : réaudit fresh et intégration restent au lead, T13 toujours bloqué.

## Entrées, ownership et environnement réellement observé

- Session déplacée nativement dans le worktree **assigné**, propre, initialement
  détaché à `b4b7e873cea63c15d36581cf5aff0c082271b34c`. Puis `git switch --detach
  3e3445f795ae11bff9212ab7b0ff5ea2b0d87dad` et création de la branche assignée
  absente, sans écraser de ref ni merge manuel. HEAD d'entrée réel vérifié :
  `3e3445f795ae11bff9212ab7b0ff5ea2b0d87dad`.
- Branche : `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-fix-harness-review`.
  Worktree sous `C:/Users/djden/.local/share/opencode/worktree/3a086c/` avec ce même nom.
  Aucune écriture dans main/utilisateur ou le worktree d'intégration du lead.
- Scope : validateur, `t04-harness/**`, ce nouveau rapport. Rapport historique
  `t04-result.md` intact. Manifest complété par ce rapport ; aucun profil/skill,
  AGENTS, configuration globale, politique ou fichier moteur changé.
- Windows x64 ; **Node v24.11.1** (`C:/Program Files/nodejs/node.exe`) ;
  **Git 2.52.0.windows.1** (`C:/Program Files/Git/bin/git.exe`). Les en-têtes du
  runner attestent le Node des tests, pas celui du processus sandbox Context Mode.
- AGENTS, skill build-validation, CMake/presets/Conan/CI et contrats T03/QA/préflight/
  isolation consultés. Le dépôt d'entrée utilise déjà Conan 2, non migré ici.
  Aucun build C++, CTest, compilateur/STL/ABI, GPU, installation ou performance
  exercé ni qualifié ; aucun `compile_commands.json` fabriqué. Serena reste lié
  au canonique ARTIST, non reconfiguré pour ce worktree ; pas de réindexation.
- Avant toute reprise des 58 tests historiques : runner relu, environnement Git
  neutralisé, hooks/templates vides possédés. L'ancien helper est extrait sans
  changer son comportement Git, uniquement pour l'injection contrôlée du RED.
  Toutes les expériences hostiles ciblent **deux dépôts jetables possédés**,
  jamais un `.git` canonique. Aucun hook externe non relu n'a été exécuté.

## Matrice réellement exercée (liste)

- **F4 RED : 6 tests, 0 PASS, 6 FAIL, 0 skip**, exit 1. `GIT_DIR`, `GIT_WORK_TREE`
  et `GIT_INDEX_FILE` redirigent bien add/commit vers la victime jetable ; le HEAD
  du sujet reste inchangé. Les variantes config global/system/count/parameters
  exécutent le seul hook relu, inerte (`T04_INERT_HOOK` sur stderr puis exit 73) ;
  Git retourne 1. La variante template installe le fichier sentinelle indésirable.
  Ceci établit une vulnérabilité **conditionnelle reproduite**, pas un incident
  historique sur une ressource utilisateur.
- **F4 GREEN : 6/6**, exit 0 ; puis **58/58 historiques**, exit 0, sans skip,
  seulement après isolation. Le helper retire tous les `GIT_*` hérités, isole
  HOME/USERPROFILE/XDG et configs globale/système/compteurs/parameters, impose
  hooks/templates vides, timeout 10 s et racine/git-dir réels avant add/commit.
- **F1/F2/F3/F5 RED : 33 tests, 6 PASS, 27 FAIL, 0 skip**, exit 1.
  Les 6 contrôles déjà verts sont les quatre exceptions exactes F1, `: item`
  déjà refusé et le worktree légitime. Aucun de ces PASS n'est prétendu RED.
- **F1/F2/F3/F5 GREEN : 33/33**, exit 0, sans skip. Puis **97/97** avec isolation
  et anciens tests, avant ajout des gardes complémentaires.
- **Final : 103/103**, exit 0, **0 échec / 0 skip** : 58 historiques + 37 tests
  de contrat supplémentaires + 8 d'isolation. Six gardes complémentaires ajoutés
  après GREEN (4 actions/effets globaux, gitfile sortant, casse env Windows) sont
  des contrôles de non-régression, pas de nouveaux RED revendiqués.
- **Syntaxe : 5 `node --check`**, exit 0 ; **`git diff --check`**, exit 0.
  JSON unique, quatre modes, exit 0/1/2/64, non-mutation/répétition, zéro sujet,
  skips externes, priorité FAIL et authenticité INCONCLUSIVE restent vérifiés.

## Premières causes et corrections bornées

### F1 — exceptions QA

RED : `read: **` (et variantes) rendait `PERMISSION_QA PASS` au lieu de FAIL.
Le filtre comparait seulement au littéral `*` et `.env` sans ignorer la casse.
13 négatifs reproduisent les globs globaux/ambigus, `docs/**`, secrets `.ENV`,
chemin Windows, credentials et `.git/config` ; quatre nominaux exacts passent.

GREEN : tous les globs sont rejetés conservativement, y compris les préfixés,
qui peuvent lire un `.env` imbriqué. Exceptions read/glob/grep limitées aux fichiers
réguliers exacts sous root, chemins non ambigus ; skill à un ID exact. Secrets
conventionnels exclus sans sensibilité à la casse. allow et ask sont contrôlés.
Grammaire et limites documentées dans README. Un nom arbitraire ne prouve pas
l'absence de secret dans le contenu ; aucun enforcement natif ni grant appliqué.

### F2 — lexer YAML local

RED : `- item`, `? key`, `-`, `?`, `:`, `texte:` étaient acceptés en string ;
True/TRUE/False/FALSE/Null/NULL échappaient au contrôle de type description.
GREEN : indicateurs plain ambigus → `YAML_UNSUPPORTED` ; variantes YAML core
reconnues comme bool/null → `SCHEMA_TYPE FAIL` pour une description. Les
contreparties quotées restent valides. Aucun parseur YAML universel ajouté.

### F3 — coordinateur distinct des feuilles

RED : orchestrator/primary/self-deny valide échouait `PERMISSION_SUBAGENT` parce
que le validateur exigeait le deny universel des feuilles. GREEN : dernière
règle correspondant à `subagent/orchestrator` doit être deny. Les spécialistes
peuvent être autorisés ; une réouverture globale ultérieure invalide le contrat.
Le négatif historique primary sur une vraie feuille reste inchangé et vert.

### F4 — preuves discriminantes d'isolation

Premier run à marqueur, paire `.run-isolation-XY29Dz/{subject,victim}` :

- HEAD sujet avant/après : `8b465dda0398e75b676314ab91d8088a2f5b2148` ;
  HEAD victime initial : `b78c1ca94038cedf94eae825d8b135581d27ed9f`.
- SHA256 index victime avant : `bfb1d4b6360968225149adc9ec08d269bbe5f0aa09541bd4ef352275ef90f129` ;
  après : `9fef498f272fcda605acda00e099fa832cbb7b1719106b9cf315dd9c35c26fcd`.
- SHA256 ref victime avant : `668639423c027c1b53e29db22fc2d73745273315a35c5fcbd996045e25b07b3c` ;
  après : `2bb88a5f36e351087459d1a8ea060a8f9272a7f5812c5e7dce871071dfd27afd`.
- Les logs conservent tous les chemins et snapshots (refs/index/hooks/config/
  templates). GREEN exige les snapshots victimes inchangés. Nettoyage limité
  aux mkdtemp créés ; vérification finale : **aucun `.run-*` restant**.

### F5 — hypothèse Git promue seulement après RED

Deux fixtures possédées `.run-5H09t8` et `.run-700wU7`. HEAD root attendu :
`bf19e08f8a73cff07c40cbc073469dae2abcb693`. HEAD victime :
`64ff3e4f25ef380ec6b9f2d4bec58d2a43efe7d3`.
Avec GIT_DIR victime + GIT_WORK_TREE root, `candidate.head` rendait exactement
le HEAD **victime**, malgré show-toplevel égal à root. Snapshots inchangés :
défaut d'identité en lecture seule, pas mutation du validateur.

GREEN : suppression case-insensitive des GIT_* hérités puis configs globale/
système neutralisées sans écriture ; gitfiles des worktrees légitimes conservés.
Pas de git diff/clean filter ajouté ; `scope: validated-inputs` inchangé.

## Commandes exactes et ordre

PowerShell, depuis la racine du worktree assigné ci-dessus ; Node/Git sur PATH
aux chemins observés. Exécutions sérialisées via Context Mode, stdout/stderr
capturés sous `T04 = .opencode/tests/agentic/t04-harness` dans `T04/out/`.

```powershell
# RED F4 avant durcissement, uniquement sous ce runner contrôlé
node .opencode/tests/agentic/t04-harness/run-tests.mjs isolation
# GREEN F4, puis seulement maintenant les 58 historiques
node .opencode/tests/agentic/t04-harness/run-tests.mjs isolation
node .opencode/tests/agentic/t04-harness/run-tests.mjs contract
# Même commande avant et après corrections du validateur
node .opencode/tests/agentic/t04-harness/run-tests.mjs contract 'F[1235]:'
# Non-régression finale, reproductible sur le diff livré
node .opencode/tests/agentic/t04-harness/run-tests.mjs all
node --check .opencode/scripts/validate-agentic.mjs
node --check .opencode/tests/agentic/t04-harness/git-fixture.mjs
node --check .opencode/tests/agentic/t04-harness/run-tests.mjs
node --check .opencode/tests/agentic/t04-harness/contract.test.mjs
node --check .opencode/tests/agentic/t04-harness/isolation.test.mjs
git diff --check
```

Les états RED sont des états intermédiaires documentés, pas le comportement du
diff final. Ne pas relancer l'ancien helper hors runner/deux fixtures possédées.

## Empreintes SHA256 du diff testé et des preuves

Fichiers livrés (octets du worktree, pas des fichiers régénérés) :

- validateur : `eee31c180bfd1ac82cf22abef6ca150738a6b3820d2d2a25c3c439f154a61068`
- contract.test.mjs : `803e8b80bfd5ead39bdeebfdf0545904c627ed75a7f42367ce1231b7398ff19d`
- isolation.test.mjs : `892902ce85d2a43e0e04b5e7762ea6eef657a2c3ec9cd2abb66818b697668777`
- git-fixture.mjs : `c8b84fdec3f8502769a59b5620525acffdbbdce25aa08ba84f8c75a54873063d`
- run-tests.mjs : `f9333104cb911d3fd336cdb70f283292bab17134e907932e9c795ca8a95d560a`
- README.md : `0800663fbb4a3c7ca3cdd2a70dc813b13a43937a2c61e0c2e89d471c57e8fcb3`
- manifest.json : `822b9cc46511d8d0bf17a6abf0b09ef4b316d18240bbc86e968bb34092b594ea`

Logs locaux sous T04/out (ignorés par Git, disponibles dans ce worktree ; ce
rapport et les tests sont les preuves persistées à intégrer) :

- f4-red-marker.tap : `ebb8eacd37221d650bcacdfe8fe261521942cf43274834c918964e4d6851cd59`
- f4-green-isolation.tap : `558b03ce0b122588d5c4e7a16a8978b7bef8f922ecc57b72ba605fd9ccd76a8f`
- f4-green-contract.tap : `0de223b59c0081e6d5bcd4dea503cf4e71438a47c479c7adde382a7531e65f1a`
- f1235-red.tap : `de91972998ad3f8e191d7502102cd2344427362dccb58beb8f3812ab60bc38a4`
- f1235-green.tap : `c6e62c5df34ef357e630fa386b8877ce8f43e44be40fb8e0bff004e4ab8adb1c`
- final-all.tap : `fb754e8cfb8bbb5832f1604c1238db47418ed6c62dff7c8c104486026e4f6e31`
- final-checks.jsonl : `5b9b158343a93fe83ae7349677f11174375ca33936fc4dc1db1167db03b8e278`

## Verdict et limites

**PASS local** pour les correctifs et la non-régression ci-dessus. Aucun test
affaibli, aucune migration de toolchain ou dépendance. Aucun Docker créé.
**INCONCLUSIVE** pour authenticité native, permissions réellement enforceables,
routage/modèle/skills natifs, autres plateformes, C++/GPU, images et performance.
Ce résultat ne débloque pas T13 à lui seul : le lead doit intégrer et faire le
réaudit indépendant prévu, puis refaire l'union sur le candidat intégré.
