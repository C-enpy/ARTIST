# T04 — harness de validation, résultat borné

Date : 2026-10-16. Auteur : pool-4 / build-engineer, tâche
`task_mvbbntgi_002z_x8sox5`. PM/LLM : complexité **80**, risque **78**, bruts =
retenus, conformément au brief ; aucun override modèle/variant, aucun sous-agent.

## Périmètre et pin

- Entrée Git vérifiée **avant écriture** :
  `40a7cc3eac80a4a2d344116365384d48bf92ae48`, worktree initialement propre.
- Branche : `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-4`.
- Racine d'écriture :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-4`.
- Contrat T03 lu en premier, puis qa-plan, T02 inventory/external-sources,
  native-preflight et execution-brief intégrés ; prescriptions AGENTS/GPU.md/QA.md
  canoniques consultées en lecture seule. Aucun diff utilisateur recopié.
- Seuls ajouts : `.opencode/scripts/validate-agentic.mjs`,
  `.opencode/tests/agentic/t04-harness/{contract.test.mjs,manifest.json,README.md,.gitignore}`,
  et le présent rapport. Fixtures temporaires sous ce même répertoire, nettoyées.
- Compilateur/STL/ABI, génération CMake et CTest : **non exercés**, hors tranche.
  CMake/presets/Conan/CI inspectés : état actuel Conan 2 CMakeDeps/CMakeToolchain,
  C++23, Ninja. Aucune migration, toolchain, dépendance moteur ou cache touché.
  Aucun compile_commands artificiel ; Serena canonique ARTIST non reconfiguré.

## Livrable

Quatre interfaces exactes T03 §6, sortie stdout JSON `schemaVersion:1`, exits
0/1/2/64. Sous-ensemble YAML strict documenté et rejet explicite, schéma local
distingué de V2, collisions/casse, chemins/junctions, six IDs historiques,
liens, allowlists et analyse conservatrice de permissions. Aucun package ajouté.

`native-evidence` consomme uniquement des données externes : identité
HEAD/hashes/delta, complétude, comptages, contradictions, originaux byte-hashés,
comparaison aux douze oracles et format de canaris. Pas de routeur local annoncé,
aucune API modèle/session/worker, collecte native, permission accordée ni sandbox
qualifié. `NATIVE_AUTHENTICITY` conserve INCONCLUSIVE même si le JSON est complet :
aucun mécanisme de confiance indépendant n'est configuré.

Contrat d'utilisation et limites détaillés :
[README T04](../../tests/agentic/t04-harness/README.md).

## Matrice réellement exercée

- **Windows x64, Node v24.11.1**, exécutable
  `C:/Program Files/nodejs/node.exe`, Git `2.52.0.windows.1` : suite locale
  **58 tests, 58 PASS, 0 FAIL, 0 skipped, 0 cancelled** ; exit 0, 25 248 ms pour
  la dernière exécution complète observée. Git sert seulement aux fixtures et à
  l'identité en lecture seule ; aucun filtre/hook/commande du corpus exécuté.
- Syntaxe des deux `.mjs` : `node --check`, exits 0.
- `git diff --check` et `git diff --cached --check` : exits 0 ; les six fichiers
  indexés appartiennent tous à l'allowlist. Le dossier evidence étant ignoré,
  ajout forcé du seul `t04-result.md` explicitement possédé, pas de règle modifiée.
- Fixtures `syntax/schema/offline` nominales : exit 0, JSON versionné.
  Negatifs parse/type/champ/doublon/YAML non supporté/ID/casse/shader-programming,
  skill/lien/baseline/allowlist, traversal/ADS/encodage, ordre des permissions,
  grant QA large shell/execute/MCP/subagent/edit/read : rejets causaux vérifiés.
- **Junction Windows effectivement créée**, évasion rejetée `PATH_ESCAPE` ;
  aucune précondition symlink skippée sur cette machine.
- G-08 : deux exécutions stdout byte-identiques ; snapshot des fichiers, y
  compris `.git`, inchangé. Markdown contenant un `throw` JavaScript resté inerte.
- Native sur **fixtures non authentifiées seulement** : zéro/skips/catalogue
  vide/runtime absent/scénario manquant, ancien HEAD/hash modifié/simulation/
  doublon/routage/comptages contradictoires/trace altérée et scanner ≠ gate exercés.
  FAIL et blocage conservés simultanément, exit 1 prioritaire. Fixture cohérente
  non authentifiée : exit 2, aucun PASS natif revendiqué.
- Manifest de lot T04 réel, sans agent/skill possédé : syntax/schema exit 2
  (2 contrôles chacun), offline exit 2 (43 contrôles), tous sans FAIL mais
  `ZERO_SUBJECTS: INCONCLUSIVE`. Ce n'est ni un faux PASS ni une découverte native.
- Linux/macOS, CI distante, runtime OpenCode, chargement/routage natif, canaris
  effectifs, C++/CTest/GPU/image/performance : **non exercés / INCONCLUSIVE**.

## RED → GREEN, causes et corrections bornées

1. **RED initial** : tests/oracles écrits avant le validateur ; une interface
   minimale exécutable renvoyait JSON INCONCLUSIVE/2 sans validation. Node et les
   fixtures Git étaient fonctionnels, aucun MODULE_NOT_FOUND/toolchain absente.
   `49 tests / 0 pass / 49 fail / 0 skipped`, exit 1. Première cause :
   `syntax doit accepter les données valides`, attendu 0, observé 2. Aucun rejet
   causal demandé n'était encore implémenté. GREEN minimal : 49/49, 0 skip.
2. **RED de durcissement** : 57 tests, 49 PASS / 8 FAIL / 0 skip. Première cause :
   profil feuille `mode: primary` indûment accepté, attendu 1, observé 0. Autres
   contre-exemples : hex YAML converti en texte, bloc vide silencieusement replié,
   comptages/catalogue/routage contradictoires, référence mal typée perdant
   l'enveloppe, hash calculé après décodage UTF-8. Corrections limitées aux contrôles
   concernés ; assertions non affaiblies. GREEN : 57/57, 0 skip.
3. **RED ciblé catalogue** : `effectiveRules:[{}]` accepté comme déclaration de
   règles, exit 2 au lieu du FAIL/1 requis. Test ciblé : 1 exécuté/1 FAIL/0 skip.
   Correction : contrôle des types agent/rules/action/resource/effect et maintien
   de la complétude INCONCLUSIVE. GREEN/non-régression finale : **58/58**.

Les commandes et sorties ont été exécutées/analysées via **Context Mode**, runtime
Node enfant explicitement distingué de Bun du sandbox MCP. Une erreur initiale
d'échappement dans un script d'inspection Context Mode a été corrigée dans ce chat ;
ce n'est ni une panne MCP ni un RED comportemental.

## Commandes exactes de reproduction

PowerShell, depuis la racine du worktree ci-dessus (séquentiel) :

```powershell
node --check .opencode/scripts/validate-agentic.mjs
node --check .opencode/tests/agentic/t04-harness/contract.test.mjs
node --test .opencode/tests/agentic/t04-harness/contract.test.mjs
node --test --test-reporter=tap .opencode/tests/agentic/t04-harness/contract.test.mjs
git diff --check
git diff --cached --check
```

La commande TAP a servi aux extraits RED et GREEN ; la commande sans reporter
est celle de la dernière exécution complète à 58 tests. Commande RED ciblée :

```powershell
node --test '--test-name-pattern=catalogue externe' --test-reporter=tap .opencode/tests/agentic/t04-harness/contract.test.mjs
```

Exercice volontaire du manifest T04 sans sujet (exit **2 attendu**) :

```powershell
node .opencode/scripts/validate-agentic.mjs --mode syntax --root . --manifest .opencode/tests/agentic/t04-harness/manifest.json
node .opencode/scripts/validate-agentic.mjs --mode schema --root . --manifest .opencode/tests/agentic/t04-harness/manifest.json
node .opencode/scripts/validate-agentic.mjs --mode offline --root . --manifest .opencode/tests/agentic/t04-harness/manifest.json
```

Les quatre modes, dont `--mode native-evidence --evidence evidence.json`, sont
exercés par la suite sur des racines de fixtures isolées dans l'allowlist T04.
Les données de test ne prétendent pas être des captures natives authentifiées.

## Empreintes du diff testé (SHA-256, octets)

- `validate-agentic.mjs` : `46138cab414d6c48662f11d685680cd5e3abbcb5bd576f808cf57cd99790e61e`.
- `contract.test.mjs` : `a85de38fc9cf56093918ca275a1587e810c450ce67429883e6b8e8a3cd9eb84f`.
- `manifest.json` : `b828537363c6bfe8efcb54d8e9c64e60e6717d80edc0ad79e17b97c3910e0ef3`.
- `README.md` : `0d8451bd2c33ba4ea18c757bd6fd47a657037ca88c5fdb80421f14fe4bd76a25`.

## Sources, dispositions et transfert

- Contrat/plan/T02/préflight/brief : sources intégrées au HEAD d'entrée. G-08 et
  T-07 adoptées pour **le harness**, aucune décision moteur ajoutée.
- Documentation officielle V2 agents/skills/permissions consultée le 2026-10-16,
  URLs dans README ; rédaction originale, liens seulement. Aucune adaptation
  tierce sans notice/commit, aucun SHA d'arbre présenté comme commit.
- Skills chargés : artist-build-validation, opencode, serena, opencode-ensemble.
  Pas de symboles C++ nécessaires ; connexion Serena canonique conservée.
- Aucune installation/driver/SDK/capture/benchmark, ressource Docker ou nouvelle
  infrastructure. Aucune modification des ADR, de PipelineContext ou de la CI.
- **DEFER_TO lead/T13** : union des manifests après merge, pertinence sémantique
  des contenus, hash/diff des exclus/utilisateur, capture et authentification
  natives sous bail unique, revue des douze sélections et trois exemples,
  décision D-PERM et canaris réellement effectués. Le préflight contient le CLI
  prouvé ; il n'a pas été invoqué par ce lot. Revue indépendante/T14 non réalisée
  par cet auteur. PASS local du harness ≠ PASS de la mission GPU/QA.
