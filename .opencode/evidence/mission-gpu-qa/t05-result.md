# T05 — Trois profils QA fail-closed, runner séparé

Date : 2026-10-16. Auteur/runner des contrôles offline : `pool-12`, profil actuel
`build-engineer` autorisé à exécuter. Ce n’est **pas** une exécution par les trois
nouveaux profils restrictifs ni une revue indépendante. Aucun sous-agent lancé.

## Résultat et périmètre

- **PASS, executed** : suite locale Node, **58/58**, 0 échec, 0 skip, répétée avec
  mêmes verdicts ; contrats du checker et présence statique des trois profils.
- **PASS, static-presence** : frontmatter V2 `description` / `mode: subagent` /
  `permissions`, aucun modèle fixé ; méthode portable puis pin dans chaque profil.
- **INCONCLUSIVE** : découverte native des nouveaux profils, règles effectives,
  canaris, transport final, composition des grants par lot et autonomie d’écriture.
  Aucun de ces points n’est qualifié PASS par les tests offline.
- Aucun CMake/build/CTest/GPU, sanitizer ou benchmark exécuté dans T05. Aucun
  résultat moteur historique (dont les 165 cas documentés ailleurs) attribué à ce lot.
- Aucun fichier moteur, PM, orchestrator, AGENTS, commande, skill existant,
  configuration globale/Ensemble/politique ou index modifié.

Les profils ne possèdent **aucun edit autorisé**, pas même pour un artefact nommé.
DENY-ALL puis seules exceptions `read`, `glob`, skill `artist-build-validation`.
Les denies env suivent read ; huit variantes ASCII de casse de `.env` et `.env.*`
sont explicites (pas d’hypothèse de case-folding Windows). `external_directory`
reste denied. `grep` n’est pas ouvert : la ressource native est un motif, pas le
périmètre des fichiers. Shell, execute, sous-agents et toutes les autres actions
restent fermés. Pas d’allow MCP/équipe ni d’action 1MCP inventée.

Le ruling « Runner séparé » est repris sans le convertir en sandbox : réponse
texte/patch, persistance lead, tests relus exécutés par le lead distinct, attribution
explicite. Un skill ne confère pas de permission. L’inaccessibilité des MCP par
Code Mode exige une digestion lead, pas une ouverture d’execute. Le schéma observé
`Permission.Ruleset` ne prouve pas la composition native et aucun grant/API n’a
été appliqué. Écriture autonome tests/artefact : blocked, dégradation consentie.

## Livrables

- `.opencode/agents/qa-planner.md` : matrice I/P séparés, cinq champs des risques
  ≥10, registre bidirectionnel, nominal/négatif, intent, heuristiques, budget et
  remise du plan au lead ; aucun test écrit par ce rôle.
- `.opencode/agents/test-engineer.md` : generator de gaps sans remplacer le TDD
  auteur, oracle avant code, KEEP/MODIFY/REJECT/DEFER, bug production laissé rouge,
  healer des seuls nouveaux tests avec deux réparations maximum.
- `.opencode/agents/test-reviewer.md` : fresh/hors board, mutant plausible avant
  finding haut, preuves qualifiées, PASS légitime, aucune écriture/APPLY implicite,
  DEFER_TO vers les rôles existants et briefing texte à persister par le lead.
- `.opencode/tests/agentic/t05-qa/manifest.json` : interface §6, `schemaVersion: 1`,
  IDs, obligations, liens et neuf chemins exacts. `skills: []` signifie aucun skill
  créé/modifié dans T05 ; les trois agents référencent le skill existant.
- `contract.test.mjs`, `fixtures/expectations.json`, `fixtures/offline-checks.mjs` :
  oracle explicite issu de QA/ruling, indépendant du texte candidat ; tests
  nominaux et mutants négatifs par obligation matérielle. Aucune dépendance T04.
- `fixtures/native-canaries.json` : **18 propositions par profil**, NOT_RUN,
  observed/nativeGrant null. Données non exécutables, aucune capture fabriquée.

Le checker n’est pas un parser YAML général ni le moteur de permissions OpenCode.
Sous-ensemble strict documenté : description/mode/permissions ordonnés, chaînes,
triplets de règles ; clés inconnues/dupliquées, blocs, tags, aliases et forme non
supportée rejetés. Les contrôles de prose sont de **présence statique** ; ils ne
prouvent pas qu’un agent respecte la méthode ni que la prose est sans contradiction.
L’exactitude du pin a été relue séparément sur les sources citées.

## Candidat, environnement et matrice réellement exercée

Base : `e4b0aa735de7ad21632527f640160973412fa47f`, worktree initial propre, reçu
detached puis attaché à la branche demandée avant toute écriture :
`ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-12`.
Delta : les neuf nouveaux fichiers listés par le manifest ; empreintes ci-dessous.

- Windows `win32` x64, **Node v24.11.1** à `C:\Program Files\nodejs\node.exe`.
  Runner : pool-12 via Context Mode, subprocess Node réel (pas Bun du sandbox).
- RED checker : 50 sélectionnés/exécutés, 3 PASS / 47 FAIL, exit 1, 0 skip.
- GREEN checker : mêmes 50, 50 PASS / 0 FAIL, exit 0, 0 skip.
- RED retour à la ligne : 3 sélectionnés/exécutés, 0 PASS / 3 FAIL, exit 1, 0 skip.
- GREEN final + non-régression : 58 découverts/exécutés, 58 PASS / 0 FAIL,
  exit 0, 0 skip ; répétition séparée 58/58 également, séquentielle.
- C++/STL/ABI, CTest, GPU, Linux/Release/CI et native V2 : non exercés ici.
  Les commandes C++ du pin sont documentaires, pas une matrice exécutée.

Commandes exactes, depuis le worktree pool-12 :

```powershell
node --version
node --test '--test-name-pattern=offline validator' .opencode/tests/agentic/t05-qa/contract.test.mjs
node --test '--test-name-pattern=ordinary Markdown line wrapping' .opencode/tests/agentic/t05-qa/contract.test.mjs
node --test .opencode/tests/agentic/t05-qa/contract.test.mjs
node --test .opencode/tests/agentic/t05-qa/contract.test.mjs
git diff --check
git diff --cached --check
```

Les deux commandes filtrées ci-dessus sont maintenant GREEN sur le code final ;
les états RED historiques sont décrits ci-dessous, pas reproductibles en tant que
RED sur l’implémentation corrigée. La commande sans filtre est l’entrée stable.

### Première cause et corrections bornées (TDD du code de test)

1. Checker initial accept-all (`inspectProfile`/`inspectManifest` retournent `[]`).
   Test négatif retire une clause attendue, par exemple `## Méthode portable` :
   `AssertionError [ERR_ASSERTION]: missing ## Méthode portable`, actual false,
   expected true. Ce n’est ni un import manquant, ni l’absence des futurs profils,
   ni une panne d’environnement : ces derniers étaient exclus par le filtre.
   Correction : parser le sous-ensemble strict, comparer les règles/manifest à
   l’oracle explicite, vérifier ordre et clauses. GREEN 50/50, oracles inchangés.
2. Ajout du contrat de wrapping Markdown : `grant exact natif` + retour à la ligne
   + `vérifié` était refusé comme `T-06.runner` (3/3 FAIL). Correction bornée :
   normaliser les espaces de la prose avant comparaison, sans changer ni mots ni
   règles. Les suppressions de chaque clause restent rejetées ; final 58/58.
3. Les profils/manifest/JSON de canaris sont documents/config : contrôles de
   syntaxe/cohérence adaptés, pas de faux RED comportemental. Aucune assertion,
   permission, tolérance ou preuve requise n’a été affaiblie pour obtenir GREEN.

Logs générés, ignorés, dans **ce worktree** :

- `out/logs/t05-red.log` — SHA256 `fef648c97c1aa234b489720edf361f9826d8a4b9ba490a3de8472858af6e2991`
- `out/logs/t05-green-checker.log` — `07e5d699c2a52953bd99c8e9de97194185f0b95a115f6f681f3f921b0b83ca8b`
- `out/logs/t05-red-wrapping.log` — `8ba40511af7b848a8794faf87a3f11d5eb66f8ead2998136a285c42cab196f03`
- `out/logs/t05-green-final.log` — `363d865f3462a2f552b4d7eff361fa00af483468dc1c96d158cc5eb551f501e7`
- `out/logs/t05-repeat.log` — `bb453bb9ca1339421512352ca4bcf19fcde3aa91a8069e5eb3da3f1b10bb7a24`

## Pin observé — dix points

1. Racine canonique ARTIST, diff utilisateur/non suivis non hérités : AGENTS et QA/GPU lus explicitement sur le canon ; source8/8 inchangées au contrôle.
2. Principes, registre ADR et correction-plan relus ; Proposed/not-started distinct d’Accepted/partial/verified et du code.
3. `PipelineContext` : unique_ptr possédé, `getPass` emprunte `IPass<API>&`, source recoupée ; pas de revendication shared_ptr.
4. CMake/presets/Conan/CI inspectés : intégration déjà Conan 2, CMakeDeps/CMakeToolchain ; aucune migration ni toolchain changée.
5. C++23 exige compile/link format/stacktrace ; versions/architecture/ABI/CRT/SDK doivent être revérifiées avant build, pas déduites d’un drapeau.
6. Suites réelles `ArtistLib_tests`/`ArtistLib_component_tests`, labels unit/opengl, mocks CPU séparés du contexte OpenGL réel.
7. Fixtures réelles TestUtils/OpenGLComponentTests/mocks ; commandes build-windows et CTest reprises de build-and-test, non exécutées ici.
8. Un build actif par répertoire, contexte/fixtures GPU sous verrou `artist_opengl` ; aucune compile_commands.json dans ce worktree, aucun include path inventé.
9. Commits locaux et patches contrôlés : 433bdbc lifetime, e736bba bornes signées, b00f484 publication/cleanup, 760f4f8 ASAN_OPTIONS hostile.
10. Artefacts existants dans `.opencode/evidence/mission-gpu-qa/` ; GrepAI/Serena/QMD/Context prescrits mais routes Code Mode non accessibles aux nouveaux profils.

## Sources et limites de fraîcheur

- Canon `C:/Users/djden/source/repos/ARTIST/AGENTS.md`, `docs/agentic/QA.md`,
  `docs/agentic/GPU.md` ; empreintes conformes à execution-brief (8/8 entrées protégées).
- `execution-brief.md`, `qa-plan.md`, `t03-contract.md` §6–8, `input-drift.md` intégrés.
  `.grepai/config.yaml` dérivé (`last_index_time`) préservé, aucune écriture/indexation.
- `docs/engineering-principles.md`, `docs/adr/README.md`, ADR-0005/0008,
  `docs/architecture/correction-plan.md`, `docs/development/build-and-test.md`,
  CMake/presets/Conan/CI, cibles/fixtures et patches des quatre commits locaux.
- QMD collection `artist-wiki`, intent explicite puis get du registre ADR ; les
  fichiers locaux font foi pour la fraîcheur (écart des nombres historiques déjà
  signalé dans T03). Serena initial_instructions/config : racine canonique ARTIST,
  pas d’activate_project, pas de diagnostic C++ revendiqué sans base fiable ici.
- Documentation officielle V2 fetchée/indexée via Context Mode le 2026-10-16 :
  https://opencode.ai/v2/docs/config, /agents, /permissions. Actions/resources,
  ordre last-match, env/external_directory et effet execute-denied recoupés ; la
  documentation n’est pas une observation des règles effectives de cette session.

## Handoff natif et décisions non tranchées

Demande de probes envoyée au lead avant les écritures, sans bloquer l’auteur.
Seul le lead admet/exécute les canaris et collecte le transport final, dans un
clone/fixture jetable sans secrets ni source canonique cible, sous bail exclusif.

- Charger réellement les trois profils ; conserver catalogue, runtime/version,
  location/session/appel/heure, règles effectives, candidat HEAD+hashes+delta.
- Vérifier lecture nominale et refus env/casse/externe ; edits même nommés refusés
  par défaut, cross-lot/multi-ressources/traversal/absolu/casse/slashes/junction,
  shell composé/redirection, execute, MCP mutateur exact observé et sous-délégation.
- Distinguer gate deny, outil indisponible, erreur scanner et abstention. Conserver
  sentinelles avant/après et traces originales. Le JSON fourni n’authentifie rien.
- Le positif d’édition avec grant est **bloqué** tant que le lead n’a pas vérifié
  une autorisation native exacte par lot et sa composition. Aucun grant proposé
  par namespace, pas d’API devinée ; l’autonomie d’écriture ne devient pas PASS.
- T13 : validation croisée avec T04 après intégration ; raccordement de la boucle
  au coordinateur hors ownership T05. T14/reviewer fresh : relecture indépendante
  des scénarios et gaps, sans prendre mes checks de présence pour des canaris.
- Décisions humaines restantes : niveaux/mocks non précisés par le lot, grants
  exacts et transport réellement possibles, critères de sortie autonomes encore
  blocked. Pas de nouvelle politique, score, modèle ou second coordinateur ici.

## Empreintes du candidat testé (SHA256)

Les huit fichiers exécutables/config/profils sont identifiés indépendamment de
ce rapport autoréférent ; son identité finale est donnée par le commit de livraison.

```text
15f384b6b002c117f0212a0fb3ac3d65f4edc3fd74ab9489f4cce7e74efff1b2  .opencode/agents/qa-planner.md
43e121db18fb5df4504c1133b6156a1e662683b4e2cfa33f82802369e910706a  .opencode/agents/test-engineer.md
729455d724bedc3cd2f8bfccb38e0efbb8e3719af5d882c67d79dc9dd5d8e739  .opencode/agents/test-reviewer.md
67121e498c2dae958f126b70e78c72a9d615e3654405844a6656e74dea3dc360  .opencode/tests/agentic/t05-qa/contract.test.mjs
c64cee7a6e6286a01b55d4e06884926bf7fc40dbeef02fd16108c877d8c6e2ae  .opencode/tests/agentic/t05-qa/manifest.json
e8585d6f68767e33ed7691943b59117ce71dcbb0b0c646892011e18e301f082c  .opencode/tests/agentic/t05-qa/fixtures/expectations.json
3d80cdea319e3a310c7820d80d6916ea5dbfd601e09fd51c4baa4ad22878699e  .opencode/tests/agentic/t05-qa/fixtures/offline-checks.mjs
25cf9a2de2b04031b884524e7481d06fd25fd8255126e6a0e47377125a2235bc  .opencode/tests/agentic/t05-qa/fixtures/native-canaries.json
```
