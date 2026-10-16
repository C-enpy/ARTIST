# T04-R2 — template QA déclaré et résiduels F1/F2

Date : 2026-10-16. Auteur/exécutant : **fix-qa-template**, build-engineer autorisé,
pas un des profils QA restrictifs. Tâche `task_mvbfmxku_000c_0l2lks`, PM/LLM
C74/R80. Aucun sous-agent, auto-claim, modèle imposé, grant ou probe natif.

## Candidat et périmètre

- Base explicitement vérifiée : `40243373aed800f7f510fd2431808ebd11baf673`.
- Worktree isolé :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-fix-qa-template`.
- Branche : `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-fix-qa-template`.
  Worktree initial propre/détaché à `83d7bdd`, seed `git switch --detach
  40243373aed800f7f510fd2431808ebd11baf673`, puis création de la branche absente
  sans écrasement de ref. Session déplacée vers ce seul worktree.
- Sources de contrat lues : `qa-policy-boundary.md`, `review-harness-fix.md`,
  `t04r-integration.md`, T03 §6–7, instructions AGENTS et skill build-validation.
  Le JSON F6 du lead a été relu, puis **réexécuté ici**, pas réutilisé comme preuve
  d'une exécution de cet auteur.
- Écritures : validateur, tests/README/manifest T04 et ce rapport neuf seulement.
  Profils QA, tout T05 (dont son oracle), anciens rapports, runner Git et tests
  d'isolation **inchangés** : contrôle `git diff --name-only` contre la base vide
  sur ces chemins. Aucun changement du main/worktree lead, moteur ou configuration.

## Matrice réellement exercée

- **Windows x64 / Node v24.11.1 / Git 2.52.0.windows.1** : tests Node et CLI
  syntax/schema/offline/native-evidence sur données inertes. Node utilisé :
  `C:/Program Files/nodejs/node.exe` (pas le Bun du sandbox Context Mode).
- **RED, avant modification du validateur** : 252 nouveaux tests, 34 PASS,
  218 FAIL attendus, zéro skip/cancel. F6 : 183 FAIL ; F1-R : 24 FAIL/1 PASS ;
  F2-R : 11 FAIL/33 PASS. Les nombreux FAIL F6 incluent les diagnostics de premier
  écart absents, **pas 183 réouvertures de permissions**.
- **GREEN ciblé** : 252/252, zéro échec/skip/cancel.
- **Non-régression finale** : **355/355 = 103 existants + 252 nouveaux**, zéro
  échec/skip/cancel. Runner isolé `run-tests.mjs all`, une suite à la fois.
- **Consommateur réel T05** : CLI offline exit0, 100 contrôles/0 FAIL/0 skip ;
  exactement trois `PERMISSION_QA` PASS, tous `static-presence`, forme B.
  `missionVerdict: INCONCLUSIVE` conservé. Les 100 contrôles ne sont pas 100 tests.
- **Suite indépendante T05 inchangée** : 58/58, zéro échec/skip/cancel.
- `node --check` validateur/tests et `git diff --check` : exit0.
- **Non exercés** : C++/STL/ABI, CMake/CTest, GPU/image/performance, Linux/macOS,
  CI distante, chargement/enforcement natif. CMake, presets, Conan et CI inspectés :
  ce commit est déjà en Conan 2/CMakeDeps/CMakeToolchain, sans migration effectuée.
  Pas de commande C++ inventée, cache réutilisé ni compile_commands artificielle.
  Serena reste lié à la racine canonique ; aucune analyse C++ symbolique revendiquée.

## Premières causes et corrections bornées

### F6 — faux négatif sur les trois consommateurs réels

RED CLI : **exit1**, `PERMISSION_QA` FAIL pour qa-planner, test-engineer et
test-reviewer ; seuls `read:*` et `glob:*` étaient incriminés. Le test nominal
échoue `1 !== 0`, sans erreur de parseur ou précondition manquante.

Correction : constante littérale de confiance **qa-readonly-declared-v1**, exactement
27 triplets ordonnés (16 gardes env + **2**, pas 8, récursifs). Comparaison par
valeurs action/resource/effect, sensible à la casse, indépendante de l'ordre des
champs YAML. Identités/path littéraux et mode subagent contrôlés ; copie exacte
sous un rôle étranger refusée. Aucun template chargé depuis les profils,
manifest ou oracle T05. Forme A conservatrice maintenue, sans fallback large.

Diagnostic : agent/path, nom/version, premier écart de champ ou de règle (1-based,
expected/observed, null si absente), preuve `static-presence` ; « template déclaré
reconnu », jamais enforcement vérifié. D-PERM devient correctement **mode décidé,
enforcement non prouvé**. Schéma/collisions/PERMISSION_SUBAGENT/FAIL prioritaire
restent actifs ; native-evidence non authentifié reste exit2, ou exit1 avec FAIL.

Oracles : trois vrais fichiers inchangés et manifest T05 ; trois identités en
fixture ; format CRLF/BOM/quotes/commentaire/ordre des champs équivalent ; retrait,
duplication et changement de chaque règle, déplacement de chaque garde située
après read:* ; réouvertures tardives read*/read**, secrets exacts/casse, external,
edit/shell/execute/subagent/MCP/team_*, skill*/nouveau skill/grep*, permutation,
usurpations ID/path/mode, faux oracle candidat, schéma/collision et preuve native.

### F1-R — suffixe env non reconnu par la borne de la forme A

RED : les 24 sentinelles inertes `docs/production.<env-case>[.local|.staging.backup]`
étaient admises exit0 au lieu d'exit1 malgré deny-all et garde env antérieure à
l'allow exact. Cas `production.ENV.local` reproduit. La regex ne reconnaissait
`.env` qu'en début de composant. Aucun secret réel lu.

Correction : exclure `.env` en suffixe, suivi de fin ou `.`, dans **chaque composant**,
sans sensibilité à la casse ; pas une liste de trois noms. Les huit casses et trois
suffixes sont vérifiés, avec `docs/guide.md`, `environment.md` et
`production.environment.md` nominaux conservés. GREEN : 25/25.

### F2-R — fallback plain non borné

RED : `,item`, `-.5`, `+.5` étaient des chaînes admises (exit0 au lieu d'exit1),
ainsi que d'autres amorces de la même classe. Pas un échec d'environnement.

Correction : grammaire positive du texte plain, initiale lettre Unicode ou `_`,
sans contrôle/mapping/alias ambigu ; booléens/null/décimaux supportés restent
typés séparément. Autres amorces explicitement hors sous-ensemble, quotes requises.
Cela refuse aussi certains scalaires YAML légitimes (`-word`, `/path`, `.text`) :
restriction **documentée**, pas prétention YAML universel. Les contreparties
simple/double-quotées sont acceptées ; corpus réel T05/intégré préservé.
GREEN : 44/44 (30 classes rejetées + 9 types core/décimaux + 5 textes nominaux).

## REFACTOR et sûreté

Comparaison de triplets isolée dans `qaRuleMismatch`, diagnostics structurés et
sous-ensemble documentés. Après le GREEN minimal, aucune restructuration
supplémentaire justifiée ; non-régression complète sur le diff final. Aucun oracle
ni assertion antérieure affaibli, aucune tolérance/skip ajouté. Seule adaptation
du helper de lancement : manifest de fixture par défaut inchangé, chemin explicite
possible pour tester les vrais consommateurs sans dupliquer l'option CLI.

F4/F5 inchangés et relancés : environnement Git neutralisé, contrôles locaux
hooks/templates/configs, racine/git-dir vérifiés avant écritures, timeouts et
nettoyage des seuls mkdtemp possédés. **Zéro `.run-*` restant** après les suites.
Hypothèses inchangées : Node/Git/PATH de confiance, absence d'écrivain hostile dans
les fixtures possédées ; ce n'est pas une neutralisation exhaustive de
NODE_OPTIONS/PATH ni une garantie de cleanup après SIGKILL. Aucune ressource Docker.

## Commandes exactes

CWD = worktree ci-dessus. Appels exécutés via Context Mode (`spawnSync`, argv
séparés, timeout externe 240 s ; runner interne 180 s ; CLI de fixture 20 s).
Équivalents PowerShell avec le même exécutable et les mêmes arguments :

```powershell
& 'C:/Program Files/nodejs/node.exe' .opencode/scripts/validate-agentic.mjs --mode offline --root . --manifest .opencode/tests/agentic/t05-qa/manifest.json
& 'C:/Program Files/nodejs/node.exe' .opencode/tests/agentic/t04-harness/run-tests.mjs contract '^F6:|^F1-R:|^F2-R:'
& 'C:/Program Files/nodejs/node.exe' .opencode/tests/agentic/t04-harness/run-tests.mjs all
& 'C:/Program Files/nodejs/node.exe' --test --test-reporter=tap .opencode/tests/agentic/t05-qa/contract.test.mjs
& 'C:/Program Files/nodejs/node.exe' --check .opencode/scripts/validate-agentic.mjs
& 'C:/Program Files/nodejs/node.exe' --check .opencode/tests/agentic/t04-harness/contract.test.mjs
git diff --check
```

Les deux premières commandes ont été exécutées RED puis GREEN. Aucun contenu
de profil/manifest n'est exécuté ; tests relus et attribués à cet auteur autorisé.

## Logs et empreintes SHA256

Logs locaux ignorés, sous `out/agentic/mission-gpu-qa/t04-r2/` ; disponibles dans
ce worktree, non transférés par le seul commit. Analyse causale via Context Mode.

- `red-real-t05.json` : `b0e9cd03f6f183663cded9ea63d7362b6436b856e3a8c00cf23b136086bc30c6`
- `red-regressions.tap` : `eece40c3690e5daf268b58d93e03dec1bf0d8ab7e5a224307c5eeb4b95eeddbb`
- `green-regressions.tap` : `2b2eb36eb357b5203d6640ad25c54776483696039ace9133a8440f27e1b1c3f7`
- `green-all.tap` : `6eab6795bdb5f45a92ecc60b57e8b14f41644c1a4e322c819b2999af84e7e6a5`
- `green-t05.tap` : `17f65605337e8302a9bf6f07b57816cacc9307979235c5563d7baf79bab21f44`
- `green-real-t05.json` : `992b144523677020fd67e9fea28e63339e71977d9eac24cffcf52d01a5504483`
- `check-validator.log`, `check-tests.log`, `diff-check.log` : vides, exit0,
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` chacun.

Sources du GREEN (octets du worktree) :

- Validateur : `cfe410a914f6f73c8c39fc929a6fd70bc402c3f82cf9002daa411c79662c20eb`
- Tests T04 contract : `bbce7054841256a6fa3fbf9320d44edb857765ad1ea4bddcc8134296ad405366`
- README T04 : `76a19555100d028339e1c32b62fa331c32c361aa800916491b2719a1833da3be`
- Manifest T04 : `83ba415e23bd16e7020a79e06284c1a99fa05dbf3e6acd1f98961119464cf4bc`
- Runner inchangé : `f9333104cb911d3fd336cdb70f283292bab17134e907932e9c795ca8a95d560a`
- Helper Git inchangé : `c8b84fdec3f8502769a59b5620525acffdbbdce25aa08ba84f8c75a54873063d`
- Tests isolation inchangés : `892902ce85d2a43e0e04b5e7762ea6eef657a2c3ec9cd2abb66818b697668777`
- Oracle T05 inchangé : `e8585d6f68767e33ed7691943b59117ce71dcbb0b0c646892011e18e301f082c`
- Manifest T05 inchangé : `c64cee7a6e6286a01b55d4e06884926bf7fc40dbeef02fd16108c877d8c6e2ae`
- qa-planner inchangé : `15f384b6b002c117f0212a0fb3ac3d65f4edc3fd74ab9489f4cce7e74efff1b2`
- test-engineer inchangé : `43e121db18fb5df4504c1133b6156a1e662683b4e2cfa33f82802369e910706a`
- test-reviewer inchangé : `729455d724bedc3cd2f8bfccb38e0efbb8e3719af5d882c67d79dc9dd5d8e739`

## Résultat et transfert

**PASS borné** : corrections F6/F1-R/F2-R et non-régression offline exécutées.
**INCONCLUSIVE** : enforcement/transport natif, canaris, autonomie d'artefact QA,
mission globale ; aucune preuve de sandbox, isolation OS ou performance GPU.
Auto-vérification de l'auteur, **pas revue indépendante**. Intégration, conservation
des logs et ré-audit fresh appartiennent au lead avant T13 ; aucun consommateur
supplémentaire ni changement de politique engagé par cet auteur.
