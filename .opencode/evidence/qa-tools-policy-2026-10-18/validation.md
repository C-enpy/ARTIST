# Permissions QA révisées — preuve bornée du 2026-10-18

## Candidat, ownership et attribution

Demande utilisateur : autoriser execute/shell/subagent dans les trois profils QA,
maintenir edit interdit, vérifier les restrictions via Code Mode.
Plan préalable attribué au qa-planner `ses_eb0fa9b3fffeEq67zPJgqxD2g5`, persisté
par le lead et commité **230c551dd05a4fdbeff0a8cce3b78bc39e734688** avant patch.
Auteur des profils, runner et intégrateur : **lead**, directement dans ARTIST,
sans équipe ni worktree auteur. PM/LLM C35→70/R45 ; aucun routage/modèle forcé.
Le travail utilisateur déjà dirty n'a pas été modifié par ce lot.

Configuration uniquement : aucun moteur, shader, build C++/CTest, GPU/image ou
benchmark exécuté. ADR inchangés. Pas de faux RED comportemental pour ce lot.

SHA256 des profils vérifiés (déclaration/découverte, profils non commités) :
- qa-planner : `7fdd45dd1059efb724dc6c6644c3ef977b7d86b17460359682ed535c44571c41`
- test-engineer : `f669918b2b21495470d60858910f46088a1e4fff4da1a9f6121733e6698059d2`
- test-reviewer : `19964cccff238fb72e6fac7616cb2c055a4e86bce4032849d852de20c0f62bad`

## Déclaration et découverte — exécutées par le lead

Environnement : Windows, Node v24.11.1, OpenCode v2.0.32. CLI actif découvert
par `Get-Process -Name '*opencode*'`, non supposé à partir du PATH.
Racine : `C:/Users/djden/source/repos/ARTIST`.

```powershell
$cli = 'C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.0.32-x64/opencode-cli.exe'
& $cli --version
& $cli api agent.list --param 'location[directory]=C:/Users/djden/source/repos/ARTIST'
& $cli api get /openapi.json
node .opencode/evidence/qa-tools-policy-2026-10-18/verify.mjs $cli
git diff --check
node --test --test-concurrency=1 --test-reporter=tap .opencode/tests/agentic/t05-qa/contract.test.mjs
```

Ces commandes ont été exécutées via Context Mode (hors découverte courte des
processus par shell lead). GET seulement : aucune session/grant/config modifiée.

- `verify.mjs` final : **exit 0**, trois profils vérifiés, 27 règles locales / 32
  règles résolues chacun. Syntaxe contrôlée par le parser T05 existant ; les règles
  sont comparées intégralement à l'oracle indépendant historique dont seules les
  trois actions explicitement révisées deviennent allow, en mémoire.
- Le frontmatter avant patch est aussi vérifié contre l'oracle historique :
  seules shell/execute/subagent changent de décision. Secrets, edit, externe,
  read/glob/skill et deny-all conservés. Aucun allow MCP/équipe ajouté.
- GET natif : les consignes révisées sont présentes dans les trois systèmes résolus.
  Les règles sont contiguës avec un refus `browser:*` supplémentaire en fin de liste.
- `git diff --check` : exit 0 à ce candidat. Vérifier à nouveau après la documentation.
- T05 historique : **exit 1, 58 tests, 55 pass, 3 fail, 0 skip**. Les trois tests
  `candidate <role>: static contract (not native discovery)` signalent
  `T-06.permissions`. Ils pinnaient la politique fermée, désormais révisée par
  l'utilisateur. Aucun oracle, assertion ou validateur historique modifié.
  Mise à jour/versionnement du harness historique : **DEFER**, hors des trois profils.

Résultats générés : `out/qa-tools-policy-2026-10-18/declarations.json` et
`legacy-t05.tap`. Ces fichiers ignorés ne remplacent pas le présent rapport.
Le premier essai de verify supposait à tort que les 27 règles terminaient le
tableau natif : assertion échouée sur le refus browser ajouté par OpenCode.
Correction du contrôle à partir de cette observation, aucun changement des profils.
Deux erreurs d'échappement Code Mode ont précédé le script persistant ; elles ne
sont ni une panne MCP ni un RED produit. L'ancien CLI 2.0.30/PATH absent n'a pas
été pris pour une absence du runtime.

## Probes Code Mode — trois feuilles, final transmis au lead

Invocation native via `subagent`, profils existants sélectionnés sans modèle forcé,
contexte fresh, aucune équipe. Les probes sont de l'instrumentation en mémoire,
pas une exécution de tests moteur par un QA.

- qa-planner : **ses_eb0f3e195ffe5z7Jfk7f1pnNZ1**, deux appels execute rapportés.
- test-engineer : **ses_eb0f3e1c3ffeihVAZxCuyE45Ry**, trois appels execute rapportés.
  Décisions : KEEP calcul/découverte, DEFER appel Context Mode non exposé.
- test-reviewer : **ses_eb0f3e193ffeIpbdkaY5Cc7MaP**, revue fresh hors board et probe.
  Aucun finding matériel démontré ; vérification du delta/enforcement limitée.

Dans chaque restitution finale, le calcul demandé a retourné exactement
`{"probe":"qa-tools-policy-2026-10-18","value":4}`. Les recherches ciblées n'ont
exposé aucun outil Context Mode/ctx_execute, team_message, team_tasks_complete
ou Serena replace_symbol_body ; résultats browser hors cible écartés. Aucun chemin
deviné, aucun appel ctx_execute tenté sans découverte, aucun mutateur appelé.
Les comptes et résultats des probes sont **rapportés par les feuilles** ; leurs
réponses finales ont été relues par le lead. Pas de lecture des raisonnements,
pas de trace de refus natif inventée ni de reproduction sous le profil du lead.

Ces observations concordent avec le deny-all non assorti d'exceptions MCP/team
dans les règles natives résolues. Elles ne prouvent pas à elles seules le lien
causal entre chaque absence du catalogue et une règle précise, ni le refus d'un
appel imbriqué qui n'a pas été possible. Découverte et invocation restent séparées.

## Registre de sortie O1–O5 / limites

- O1/T1 : **PASS déclaré** : parser et règles exactes, trois effets changés.
- O2/T2 : **PASS déclaré/résolu seulement** : protections maintenues ; refus natifs
  edit/secrets/externe non exercés. Aucun vrai secret ni fichier cible muté.
- O3/T3 : execute utilisable selon trois restitutions ; outils imbriqués recherchés
  absents. **INCONCLUSIVE** pour refus natif à invocation ; Context Mode/transport
  réel **NOT_RUN**, pas de capacité fonctionnelle démontrée pour ces outils.
- O4/T4 : textes et descriptions réconciliés ; rôle feuille/runner séparé conservé.
  Un shell ou exécuteur autorisé n'est pas confiné par `edit deny` : aucune sandbox
  revendiquée. Shell/subagent sont allow natifs mais non exécutés par les QA.
- O5/T5 : candidat, empreintes, runner, commandes et sessions ci-dessus ; pas de
  PASS global. L'échec legacy T05 est conservé et prioritaire sur sa suite.

Pas d'ouverture supplémentaire pour masquer l'absence des outils. Si Context Mode
ou Ensemble doivent être réellement accessibles, une décision distincte doit
nommer les actions imbriquées exactes, puis refaire leurs probes. Le fallback texte
reste utilisable sans promesse de clôture autonome du plugin.

Références consultées : documentation officielle V2
`https://opencode.ai/v2/docs/agents`, `/permissions`, `/api` le 2026-10-18 ;
`docs/engineering-principles.md`, registre ADR et correction-plan, contrat/ruling
historiques `.opencode/evidence/mission-gpu-qa/qa-policy-boundary.md`.
Photographie : `docs/agentic/gpu-qa-harness.md`. QMD n'a pas été réindexé pour les
autres brouillons utilisateur présents dans le worktree.

## Ré-audit indépendant et disposition

Ré-audit par test-reviewer dans la même session indépendante
`ses_eb0f3e193ffeIpbdkaY5Cc7MaP`, lectures seulement : les preuves actuelles ne
sur-affirment ni refus natif ni confinement. Gap moyen proposé : un futur texte
affirmant faussement « MCP refusé nativement » ne serait pas rejeté par verify.mjs.
Scénario discriminant mental, pas de mutation exécutée.

Disposition lead : **vérification documentaire manuelle explicite sur ce candidat**,
effectuée par le lead et le reviewer ; verify.mjs ne prétend pas la remplacer.
L'automatisation de cette vérification reste DEFER, sans acceptation humaine
implicite d'un risque futur. Le helper `inspectProfile` a été relu par le lead :
il compare les règles intégrales avec `isDeepStrictEqual` (offline-checks.mjs:45).
Le reviewer n'avait pas lu cette dépendance et conserve cette limite attribuée.
Les profils n'ont pas changé depuis les probes/revues et les empreintes restent
celles listées ci-dessus. Les modifications d'autres fichiers utilisateur et les
contradictions résiduelles de leurs brouillons de consignes ne sont pas réécrites.
