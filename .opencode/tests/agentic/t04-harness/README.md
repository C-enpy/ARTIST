# T04 — contrat local du validateur, version 1

Rédaction originale, sans dépendance installée. Oracle préalable :
[T03 §6–7](../../../evidence/mission-gpu-qa/t03-contract.md) et
[plan QA](../../../evidence/mission-gpu-qa/qa-plan.md), notamment G-03/06/07/08,
T-06/07. Les constantes des douze scénarios sont un **oracle de comparaison**,
pas un dispatcher ni une observation du choix d'un modèle.

## Invocation et sorties

Depuis la racine du candidat, avec Node v24.11.1 observé sous Windows x64 :

```powershell
node .opencode/scripts/validate-agentic.mjs --mode syntax --root . --manifest <manifest.json>
node .opencode/scripts/validate-agentic.mjs --mode schema --root . --manifest <manifest.json>
node .opencode/scripts/validate-agentic.mjs --mode offline --root . --manifest <manifest.json>
node .opencode/scripts/validate-agentic.mjs --mode native-evidence --root . --manifest <manifest.json> --evidence <native-observations.json>
node .opencode/tests/agentic/t04-harness/run-tests.mjs all
```

Les chevrons sont des paramètres à remplacer, pas des commandes exécutées.
Manifest/evidence relatifs à `--root`, ou absolus **dans** cette racine. Les
artefacts externes doivent être apportés par le collecteur autorisé dans cette
racine avant analyse. Pas de lecture arbitraire hors racine, ni téléchargement.

- `syntax` : JSON sans clés dupliquées, frontmatter des entités déclarées ; corps
  Markdown inerte. N'importe quel exemple de commande reste une donnée.
- `schema` : ajoute les champs/types utilisés et les conventions ARTIST ci-dessous.
- `offline` : ajoute découverte statique locale, collisions, liens inline,
  six IDs historiques, allowlist déclarative, obligations et analyse des permissions.
- `native-evidence` : offline puis analyse de traces **fournies**, jamais collecte.

Un seul JSON stdout, `schemaVersion:1`, aucun fichier écrit par le validateur.
Exit **0** = tous contrôles demandés PASS et non vides ; **1** = au moins un FAIL,
même si un autre contrôle est bloqué ; **2** = aucun FAIL mais précondition
manquante, INCONCLUSIVE ou zéro sujet/contrôle ; **64** = usage CLI invalide.
Pas de drapeau `--execute`, `--fix`, `--grant`, ni option d'authentification fictive.

L'enveloppe contient `mode`, `contract`, `verdict`, `missionVerdict`, `environment`
(Node/OS/architecture), `command` (exécutable et argv), `candidate`, `artifacts`,
`limits`, `counts` et `checks`. Chaque contrôle porte `mode`, `level`,
`obligation`, `criterion`, `code`, `expected`, `observed`, `verdict`,
`evidenceKind`, `preconditions`. Les `counts` dénombrent les **contrôles du
validateur**, pas des tests moteur ; `nativeCounts` conserve séparément les
comptages de scénarios déclarés par la source externe.

`missionVerdict` reste INCONCLUSIVE : ce programme ne peut conclure sur la mission.
`static-presence` distingue l'analyse de fichiers du runtime ; `inference`
qualifie les observations externes non authentifiées. Les tests Node prouvent
l'exécution du validateur, pas l'exécution native des scénarios de leurs fixtures.

## Sous-ensemble YAML explicite, pas YAML complet

Aucun parseur déclaré dans le projet (pas de package.json à la racine ou dans
`.opencode/`). Aucun import d'une dépendance globale du CLI. Le parseur accepte :

- UTF-8 valide, LF/CRLF, BOM initial facultatif ; délimiteurs `---` seuls sur leur
  ligne ; mapping racine ; indentation exactement par pas de deux espaces ;
- clés simples alphanumériques/underscore, `-`, `.`, `/`, ou clés quotées ;
- texte plain commençant par une lettre Unicode ou `_`, sans caractères de
  contrôle, `:` suivi d'espace/final ou amorce d'alias/anchor après un espace ;
  simples quotes avec `''`, doubles quotes/échappements JSON ;
- scalaires YAML core `true/True/TRUE`, `false/False/FALSE`, `null/Null/NULL`, `~`,
  nombres décimaux sans exposant ; les variantes reconnues ne deviennent pas du texte ;
- mapping imbriqué et séquence de scalaires ou mappings (forme permissions V2) ;
- conteneurs vides `[]` et `{}` ; commentaires hors quotes ;
- blocs `>`, `>-`, `|`, `|-` **un seul paragraphe**, indentation uniforme de deux
  espaces. Le caractère `#` dans le bloc est du texte, pas un commentaire.

Les clés dupliquées, tabulations, flow YAML non vide, tags, ancres/alias/merge,
directives, nombres hexadécimaux/dates non quotées et formes hors sous-ensemble
sont rejetés explicitement, jamais ignorés pour obtenir PASS. Un bloc avec lignes
vides est volontairement non supporté ; utiliser une description simple/quotée.
Les autres amorces plain sont refusées par une grammaire positive, pas une liste
noire partielle : `,item`, `-.5`, `+.5`, nombres avec exposant, dates, indicateurs
de séquence/mapping, etc. produisent `YAML_UNSUPPORTED`. Même un texte YAML valide
comme `-word`, `/path` ou `.text` exige des quotes dans ce sous-ensemble conservateur.
Les types core/décimaux explicitement supportés restent typés ; les contreparties
quotées restent des strings. Aucun fallback numérique ambigu vers du texte.
Ce n'est pas un parseur YAML universel ; le corpus réel intégré est vérifié.
Profondeur YAML limitée à 16, JSON à 64 ; chaque fichier à 4 Mio ; découverte
limitée à 20 niveaux et 1000 entrées de profils/skills.

Le schéma **local** supporte les agents `description`, `mode`, `permissions` et
les skills `name`, `description`, `license`, `compatibility`, `metadata` avec
`opencode/autoinvoke` booléen, `disable-model-invocation` booléen. Une permission
est `{action:string, resource:string, effect:allow|ask|deny}` ; l'objet legacy
n'est pas accepté. Les champs hors sous-ensemble produisent `SCHEMA_FIELD` : ce
n'est pas une déclaration qu'ils sont invalides dans tout OpenCode V2.

ARTIST exige description non vide, corps non vide, name du skill égal à son ID,
ID kebab-case minuscule ≤64 caractères, chemin `.opencode/agents/<id>.md` ou
`.opencode/skills/<id>/SKILL.md`. Agents feuilles `mode: subagent`, seul
`orchestrator` primary. Aucun modèle/variant dans ce contrat. Ce sont des
**conventions locales**, pas des exigences natives inventées : V2 distingue
notamment ID dérivé du chemin et name d'affichage et peut accepter du frontmatter
de skill absent. Le schéma local est embarqué, sans fetch pendant les tests.

Sources officielles consultées le 2026-10-16, liens seulement :
[agents](https://opencode.ai/v2/docs/agents),
[skills](https://opencode.ai/v2/docs/skills),
[permissions](https://opencode.ai/v2/docs/permissions).
Pas de contenu/code tiers adapté ; arbres SHA T02 non utilisés comme commits.

## Manifest et limites de l'offline

Objet `schemaVersion:1`, `lot:string`, `agents:[{id,path,skills:[id]}]`,
`skills:[{id,path}]`, `obligations:[G-/T-]`, `requiredLinks:[{from,to}]`,
`allowedPaths:[path]`. Champs supplémentaires rejetés. Obligations connues :
G-01 à G-08, T-01 à T-07. Elles doivent être uniques/non vides en offline.
Un ID dans cette liste **n'est pas** la preuve de satisfaction de l'obligation.
La pertinence du contenu et la couverture des 19 responsabilités restent à revoir.

Allowlist : chemins exacts ou suffixe `/**` pour un répertoire borné ; pas de glob
général. Contrôle de l'appartenance des agents/skills déclarés, pas de droit
d'écriture ni vérification de tous les hunks utilisateur. `..`, absolus, ADS,
caractères ambigus Windows et évasion symlink/junction sont rejetés. Les liens
Markdown peuvent remonter **dans** la racine ; une remontée sortante/encodée est
refusée. Liens HTTP/mailto non fetchés ; ancres non validées. Les requiredLinks
acceptent un lien inline résolu ou la référence littérale du chemin/ID en
backticks. Wikilinks/HTML/références Markdown indirectes non analysés.

Découverte : agents `.md`, skills `SKILL.md` récursifs ou `.md` à la racine des
skills ; collisions ID et casse, y compris deux `shader-programming`, rejetées.
Les six IDs historiques doivent exister ; cela ne certifie pas la préservation
octet à octet de leurs contenus. Une base Git exacte indisponible n'empêche pas
les contrôles statiques, mais laisse l'identité native INCONCLUSIVE.

Permissions : ordre conservé. Feuilles : deny universel `subagent` sans
réouverture après ce deny. Coordinateur `orchestrator/primary` : la dernière
règle applicable à `subagent/orchestrator` doit être deny ; les autres spécialistes
restent délégables. Le mode primary reste interdit aux vraies feuilles.

QA **forme A**, conservatrice : deny-all initial ; `read/glob/grep` seulement sur un **fichier régulier
existant exact**, relatif à root, sans évasion par junction/symlink. Chemins
composés de lettres ASCII, chiffres, `_`, `.`, `-` et `/` ; backslashes normalisés,
pas d'absolu/traversal/ADS. Tous les globs sont conservativement rejetés, y compris
`**`, `**/*` et **`docs/**`**, qui peuvent rouvrir des secrets dans un sous-répertoire.
`skill` accepte seulement un ID kebab-case exact ≤64 caractères. Les noms secrets
conventionnels `.env*`, `.git`, `.ssh`, `.aws`, `.azure`, `.gnupg`, `.npmrc`, `.pypirc`,
suffixes `.env`/`.env.*` (par exemple `production.ENV.local`), `secret(s)`,
`credential(s)`, clés `id_rsa/dsa/ecdsa/ed25519`, extensions
`key/pem/p12/pfx/kdbx` sont exclus sans sensibilité à la casse, même sous Windows.
Les effets allow **et ask** demandent ces bornes ; les autres actions non deny sont
refusées. Cette politique locale peut donc refuser des règles natives légitimes.
Elle ne certifie pas le contenu non secret d'un fichier à nom quelconque : revue
humaine des exceptions et protections héritées toujours requises.

QA **forme B**, `qa-readonly-declared-v1` : reconnaissance du template déclaré
adopté dans [le contrat F6](../../../evidence/mission-gpu-qa/qa-policy-boundary.md).
Seuls `qa-planner`, `test-engineer`, `test-reviewer`, avec chemin littéral
`.opencode/agents/<id>.md` et mode `subagent`, y sont éligibles. Les 27 triplets
ordonnés sont une constante du **code de confiance**, jamais une policy chargée
depuis un manifest, profil ou `expectations.json` candidat. L'oracle T05 reste
indépendant. Une copie exacte sous un rôle étranger n'obtient pas l'exemption.

Ordre exact : deny-all ; read:* ; glob:* ; skill:artist-build-validation ; 16 gardes
read `*.env`/`*.env.*` dans les huit casses du contrat ; deux gardes récursifs
`**/.env*`, `**/.ENV*` ; edit/shell/execute/subagent/external_directory deny sur `*`.
Comparaison structurelle des valeurs littérales **sensible à la casse**, sans
normalisation/glob simulation, ajout, suppression, permutation, doublon ni effet
substitué. L'ordre des champs YAML dans un triplet est indifférent, pas celui des
règles. Les FAIL schéma, identité, collisions et PERMISSION_SUBAGENT restent actifs.
Échouer B ne permet jamais à A de reconnaître un wildcard comme fichier exact.

`PERMISSION_QA.observed` donne agent/path, `form` A/B/null, `template`,
`templateVersion`, `firstMismatch` et message. Premier écart : `field` pour
id/path/mode, sinon `rule` **1-based**, avec expected/observed (null si absent).
`firstMismatch:null` et « template déclaré reconnu » signifient B exact, sous
`evidenceKind:static-presence`. `unsafe` désigne les exceptions hors forme A
lorsque B n'est pas reconnu ; aucun de ces champs n'atteste la sécurité native.

**D-PERM : mode décidé, enforcement non prouvé**. Aucun grant edit/shell/execute/
MCP/subagent, aucun runner autonome ni sandbox créé. Les 27 règles n'attestent pas
la confidentialité de tous les secrets, jonctions, règles héritées/session ou
variantes de casse effectives. missionVerdict reste INCONCLUSIVE ; native-evidence
reste exit2 faute d'authentification, ou exit1 en présence d'un FAIL prioritaire.

Le manifest de **ce lot T04** ne déclare aucun profil/skill, car il possède un
script et ses tests uniquement. L'utiliser seul rend donc exit 2 (`ZERO_SUBJECTS`),
pas un faux PASS global. Les fixtures indépendantes déclarent explicitement
leurs profils/skills ; l'union réelle appartient à T13 après intégration.

## Identité du candidat et non-mutation

Hashes SHA-256 des octets du manifest, profils/skills découverts et cibles locales
effectivement lues. HEAD via `git rev-parse` et blobs via `git ls-tree` en lecture
seule (`--no-optional-locks`, fsmonitor désactivé). Aucun `git diff` susceptible
d'invoquer un clean filter, aucun hook, import/évaluation de fixture ou commande
tirée des données. Les octets courants sont comparés aux blobs HEAD **sans filtre**.
Toutes les variables `GIT_*` héritées sont retirées (casse comprise), puis les
configs globale/système sont neutralisées sans écrire de fichier ; les gitfiles
des worktrees légitimes restent reconnus. Cela évite un HEAD étranger via
`GIT_DIR` + `GIT_WORK_TREE` même quand `--show-toplevel` ressemble à root.

`candidate.delta.sha256` = SHA-256 de la liste JSON ordonnée des différences
`[{path,headBlob,sha256}]`, ou SHA-256 de la chaîne vide si aucune différence.
`candidate.delta.paths` liste les chemins concernés. Portée : `validated-inputs`.
Les différences CRLF brutes comptent ; les fichiers étrangers à cette portée ne
sont pas certifiés inchangés. Le validateur ne remplace pas le contrôle des hunks
utilisateur et hashes d'exclus du lead. La capture externe et ses originaux sont
exclus du candidat pour éviter une auto-référence. Pas de garantie de snapshot
atomique face à un écrivain concurrent : bail unique requis.

## Format des observations EXTERNES, version 1

Ce format est celui du **consommateur offline**, pas un endpoint/schéma natif
OpenCode. Le lead conserve les réponses originales et leur provenance puis
prépare l'enveloppe ; aucune collecte ni session modèle n'est lancée ici.

- `schemaVersion:1` ; `runtime:{name,version,authorizedPath,location,session,call,at}`
  (chaînes non vides, heure parseable) ; `location` égale à la racine observée.
- `candidate:{head,hashes,delta:{sha256}}` : jeu exact d'entrées validées et bytes du
  même candidat. Ancien HEAD, jeu incomplet ou hash modifié : FAIL T-07.
- `counts:{discovered,executed,failed,skipped}` : entiers ≥0, somme cohérente avec
  les scénarios ; zéro et requis skipped : INCONCLUSIVE, contradiction : FAIL.
- `catalogue:{agents:[id],skills:[id],effectiveRules:[{agent,rules:[{action,resource,effect}]}]}` :
  catalogue/règles non vides, bien typés ; les IDs choisis doivent y figurer.
- `scenarios:[{id,prompt,principal,secondaries:[id],skills:[{id,call,reason?}],references:[artifact],status,evidenceKind,trace}]`.
  IDs S01–S12 dans l'ordre du T03 §5. Appels de skills distincts, minimum requis ;
  un ajout demande reason ; charger tout le catalogue n'est pas progressive
  disclosure. `status: PASS|FAIL|SKIPPED|NOT_AVAILABLE|INCONCLUSIVE|BLOCKED`,
  `evidenceKind: executed` pour une prétention native. Simulation explicite rejetée.
- `artifact` / `trace` : `{path,sha256}` vers un original dans la racine ; hash des
  **octets**, même non UTF-8. Missing : INCONCLUSIVE ; hash discordant : FAIL.
- `examples:[Sxx,Syy,Szz]` : au moins trois références distinctes présentes.
- `canaries:[...]` ci-dessous. Tout incomplet reste visible avec les FAIL éventuels.

Un booléen `authenticated:true`, une trace ayant le bon hash, les bonnes réponses
recopiées ou le texte d'un profil **ne donnent jamais** une authentification.
`NATIVE_AUTHENTICITY` reste INCONCLUSIVE, même avec une enveloppe complète et
cohérente : aucun collecteur/clé/attestation de confiance n'est configuré ici.
Ce mode ne peut donc rendre exit 0 dans ce contrat. Il détecte des défauts et
prépare la revue ; seul le lead peut établir l'origine par une voie indépendante.
Ne pas annoncer les comparaisons de constantes comme douze routages exécutés.

### Canaris préparés, jamais lancés

Chaque entrée : `{id,agent,action,resource,expected,gate,before,after,trace}`.
`expected: allow|deny`. `gate: allowed|denied` distingue le résultat du gate ;
`scanner-error`, `not-available`, abstention ou autre résultat non établi restent
INCONCLUSIVE. `before` / `after` sont des maps chemin → SHA-256 de sentinelles.
Un deny avec modification de sentinelle est FAIL. Pour un allow, l'oracle des
modifications permises reste à examiner par le runner, pas à déduire du gate.

Étiquettes de collecte proposées : C01 edit permis ; C02 edit interdit ; C03 patch
mixte ; C04 traversal ; C05 absolus ; C06 casse/slashes ; C07 junction si disponible ;
C08 shell composé/redirection ; C09 MCP mutateur/exécuteur ; C10 sous-délégation.
Le check de couverture porte sur les IDs ; la pertinence de chaque action et les
règles effectives du profil réellement chargé demandent une revue externe.

Bail exclusif **lead**, clone/fixtures jetables sans secrets/source canonique,
aucune nouvelle permission/installation implicite. Le CLI et deepObject prouvés
dans [native-preflight](../../../evidence/mission-gpu-qa/native-preflight.md)
sont une piste pour les futures collectes autorisées, pas un prérequis du GREEN
unitaire. Pas de catalogue/canari/GPU déclenché par ce lot.

## Tests et reprise

Le test Node invoque le CLI en subprocess, sans importer son implémentation.
Les oracles et les douze scénarios viennent explicitement du plan/T03. Fixtures
créées sous `.run-*` dans **ce répertoire seulement**, supprimées après chaque
test. `run-tests.mjs [all|isolation|contract] [test-name-pattern]` lance seulement
ces suites relues, jamais un chemin/une commande de manifest ; limite 180 s,
une suite à la fois. Runner et `git-fixture.mjs` neutralisent l'environnement Git,
HOME/USERPROFILE/XDG_CONFIG_HOME et les configs globale/système/paramétrées ;
hooks/templates vides possédés, timeout Git 10 s. Le helper vérifie racine et
git-dir **réels** avant add/commit et refuse un `.git` sortant. Le runner ne
nettoie que son mkdtemp, jamais un chemin hérité ; chaque test nettoie ses propres
mkdtemp. `isolation.test.mjs` utilise deux dépôts jetables possédés, sentinelles
refs/index/hooks/config/templates et un hook inerte relu (marqueur stderr + exit 73).
Aucune configuration Git globale de l'utilisateur n'est modifiée.
Le test symlink/junction signale un skip INCONCLUSIVE uniquement si sa
création est réellement indisponible. Les autres tests ne sont pas skippés.

La réexécution compare stdout octet à octet et snapshot des fichiers, y compris
`.git`. Aucun témoin moteur/GPU n'est remplacé par une fixture : cette suite
valide exclusivement le contrat du harness. Preuves RED/GREEN et limites dans
[t04-result](../../../evidence/mission-gpu-qa/t04-result.md).
Correctif de revue, RED/GREEN et nouvelles empreintes :
[t04-fix-result](../../../evidence/mission-gpu-qa/t04-fix-result.md).
