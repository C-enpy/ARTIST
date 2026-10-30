# Helper Windows A — mapping et candidat TESTS_RED_READY (pool-31)

## Admission, périmètre et état

Entrée vérifiée : `2b836e82e5827ffe3c8aa79f88fb4ad9080ba0cf`, worktree
`ensemble-2dc31e0a-cwsmsw-pool-31`, branche
`ensemble-b4f17505-8f37-40e0-9076-488e1c1a7fb6`, index/diff propres avant édition.
QA §13 présente dans ce HEAD ; `86b51ad` intégré. Contrat, binding v1 et rapports
113/114/115 lus. PM/LLM brut/retenu 78, risque 68, incertitude moyenne : repris
du mandat, politique inchangée. Ownership helper transféré à pool-31 ; aucune
copie des branches historiques. Livrable initial : nouveaux fichiers de tests
seulement. Aucun changement de production, d'adaptateur ou d'ancien oracle.

**NON_EXECUTE** : auteur non runner. Aucun unittest, PowerShell, faux enfant,
helper, outil, configure/build/CTest/moteur/GL, installation ou cloud exécuté.
RED attendu, pas observé. La revue lead de ce programme et de ce mapping précède
son exécution. GREEN reste interdit avant RED causal observé et mapping ratifié.

## Mapping obligatoire de l'ancien oracle (avant sa modification)

`test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch`
reste **strictement intact** au RED, comme les 54 tests adaptateurs et les 51
fondations/pins historiques. Après ratification puis RED seulement, proposition :

1. `assertIn` de la condition de phase : conserver la décision explicite et
   remplacer l'obligation de refus global par `test_prepare_reaches_real_binding`
   et `test_build_is_one_selected_command` (A1/A3/A4). Une garde manquante n'est
   pas une ouverture admise : les négatifs doivent refuser avant toute feuille.
2. `assertIn` du throw NOTIMPLEMENTED : ne pas simplement supprimer. Le remplacer
   par les refus causaux nominativement testés : run (avec/sans opt-in), opt-in
   absent ou sur build, flags legacy/unknown, payload/transport/arbre/config/cibles
   invalides. `events == []`, environnement inchangé et payload inchangé restent
   obligatoires (A1/A2/A6). NOTIMPLEMENTED ne satisfera pas ces oracles GREEN.
3. `assertNotIn Invoke-VerificationChild` avant dispatch : remplacer par absence
   de **toutes** feuilles d'effets avant admission, plus sentinelle de chute
   legacy, audit des commandes et extraction bornée A-T0/A-T1/A-T6 ; pas seulement
   absence d'une chaîne. Aucune phase verification ne rejoint le legacy.
4. Helper complet jamais exécuté : invariant conservé par un unique spawn vers
   le programme PowerShell TEST-OWNED ; celui-ci n'exécute que les fragments
   admis. Les régions natives de process/capture ne sont jamais chargées.

Ces correspondances remplacent les assertions, pas leur intention de sécurité.
La future modification de l'ancien oracle et le nouveau pin d'entrée devront
figurer explicitement dans le diff GREEN relu. Aucun repin WRAPPERS/NATIVE.

## Extraction et confinement proposés (A-T0, RA1/RA2)

`helper_fragment_support.py` extrait les paramètres réels, WRAPPERS, les deux
fonctions natives **données seulement** `Read-VerificationPayloadFile` et
`Resolve-VerificationPhaseFlags`, puis la garde réelle jusqu'à END_DISPATCH.
Chaque marqueur est unique, les tailles sont bornées ; identité entière et
empreintes de chaque fragment sont admises par `helper_fragments_red.json`.
La déclaration historique root/tooling/venvPython est extraite à son emplacement
réel : actuellement APRÈS dispatch. Elle n'est jamais remontée avant celui-ci.
Un nouveau pin d'entrée après GREEN exige revue distincte, pas d'auto-acceptation.

Le programme extrait est créé dans `case/source tree with spaces/scripts/` ;
**le vrai `$PSScriptRoot` du fichier test-owned** alimente exactement les trois
affectations source root/tooling/venv. Aucun `$root`/tooling/MSVC caché injecté
dans l'appelant. Une baseline séparée exécute ces mêmes trois lignes dans un
autre script, puis vérifie les chemins dérivés ; elle ne partage pas leur scope
avec l'entrée. Une sentinelle après les fragments détecte la chute legacy.

Avant toute évaluation : pin, syntaxe AST, fonctions extraites attendues,
allowlist commandes/constructs/variables d'entrée et confinement des chemins de
la fixture. Audit mutant = rejet HARNESS/96, jamais RED. Ce dispositif est un
contrôle d'admission de source relue, **pas un sandbox PowerShell universel**.

Seules feuilles factices proposées, à ratifier avant production :

- `Initialize-VerificationTooling(Root, Python)` : prépare tooling uniquement
  pour prepare opt-in ; record-only ici. Futur réemploi borné du legacy
  venv/ensurepip/pip, dépendances épinglées existantes ; pas encore autorisé/code.
- `Get-VerificationTooling(Root)` : Conan/Ninja existants sous
  out/tooling/build/Scripts et CMake absolu depuis son installation existante
  résolue explicitement. CMake n'est PAS une dépendance pip du fichier de deux
  requirements existant ; aucune installation CMake inventée. Aucun
  bootstrap/réparation sur build.
- `Assert-VerificationPreparedTree(BuildDirectory, Configuration)` : admission
  lecture seule du cache/toolchain et configuration de cet arbre avant effets.
  Pas de nouveau receipt/schéma. La fixture simule cette observation, pas CMake.
- `Read-VerificationEnvironmentLeaf` / `Write-VerificationEnvironmentLeaf` :
  map synthétique, jamais environnement du processus. Les Get/Set wrappers réels
  opèrent sur cette map ; absence et chaîne vide restent distinctes.
- `Read-VerificationMsvcEnvironmentLeaf` : métadonnées x64/14.44/SDK factices ;
  Get-VerificationMsvcEnvironment réel les valide. Le pont de binding doit
  filtrer les clés avant enfants (secret synthétique exclu).
- `Invoke-VerificationChild(Program, Arguments)` : journal et code/exception
  prédéfinis, AUCUN processus. Observe root/tooling/venv réels du script extrait,
  argv, map enfant et bruts synthétiques ; aucun stdout interprété en protocole.

Le wrapper Child supérieur est remplacé dans cette fixture : quoting et capture
native restent couverts uniquement par les preuves antérieures, pas redémontrés
par A. Le câblage natif futur devra être proposé/relu séparément avant GREEN :
réemploi des primitives pinées, cwd/env explicites, outils possédés, logs privés
prepare/build et capture core primaire. Pas de nouvelle capture backend, ni
modification Dispose/writers hérités source-only, ni deuxième preuve helper-run.
Si ces signatures/feuilles nécessitent un changement de contrat public, de pin ou
de frontière env/process, retour au lead AVANT code, pas de changement implicite.

## Plan borné et matrice de tests proposée

- A-T0 : extraction/pin/population/syntaxe + baseline racine et feuilles factices ;
  mutations commandes/process/env/root/markers rejetées sans exécution.
- A-T1 : prepare opt-in/build préparé admis ; run/unsupported/legacy/flags invalides
  refusés avant tooling/env/enfants et sans chute legacy.
- A-T2 : fichiers JSON synthétiques réels quatre champs ; Debug/Release/espaces,
  cibles workloads et tests ; mauvais bundle/arbre/config/type/cible/run transporté,
  champs supplémentaires/dupliqués/JSON invalide : payload inchangé, zéro effet.
- A-T3 : tooling prepare explicite, Conan2 host/build cohérents puis CMake fresh
  dans le seul arbre verification ; ON workload, BUILD_TESTING/PRE_TEST conditionnels.
  Conan nonzero : pas de CMake. Aucun build/test/run dans prepare.
- A-T4 : tooling existant/MSVC puis UNE commande build avec cibles/jobs exacts ;
  arbre non préparé/tooling absent/nonzero : pas de bootstrap/Conan/configure-repair.
- A-T5 : enfant voit map outil filtrée ; appelant synthétique restauré exactement
  succès/nonzero/exception, absent/vide. Première cause et bruts conservés.
- A-T6 : sentinelle legacy, confinement spawn, identité source stable ; anciennes
  sélections nominatives par le lead seulement. `test06` jonction NON_EXECUTE.

Commande proposée au lead (depuis ce worktree, après revue ; PAS exécutée ici) :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_helper_functional.py -v
```

D'abord sélectionner la classe `HelperFunctionalHarnessTests` seule ; toutes ses
baselines doivent réussir avant d'interpréter le RED de
`HelperFunctionalAdmissionTests.test_prepare_reaches_real_binding`. Commandes
nominatives depuis `scripts/tests/verification_windows/` (PAS exécutées) :

```powershell
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalHarnessTests
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalAdmissionTests.test_prepare_reaches_real_binding
# Après RED ratifié, code minimal et nouveau pin d'entrée relu seulement :
C:/Python313/python.exe -B -m unittest -v test_helper_functional
```

La commande de découverte **du fichier seul** ci-dessus ne remplace pas cette
barrière de baseline. Les autres classes/fichiers restent hors de cette première
exécution. Attendu sur
ce HEAD : refus explicite « Verification phases are not implemented yet », journal
vide et environnement inchangé, alors que l'oracle exige Conan→CMake factices.
Fonction absente, parse error, interpréteur absent, timeout, pin ou fixture cassés
= HARNESS/INCONCLUSIVE, jamais RED. Aucun discovery Windows global.

Les sorties futures restent sous
`out/verification-synthetic/windows-helper-functional/<UTC-UUID>/` : inputs,
fragments, source/pins/fixture identities, résultats et stdout/stderr/process.json.
Pas de dump d'environnement réel : les maps conservées sont exclusivement factices.
Les programmes générés sont des copies bornées de fragments, pas le helper entier.
Le gap WindowsAdapter→factory commune reste réservé au generator QA ultérieur :
aucun import/adaptateur/core ou doublon de cette preuve dans la tranche A.

## Contraintes build observées, non exécutées

CMake racine/presets, Conan et CI lus : Ninja, profil Windows x86_64 msvc194
runtime dynamic, cppstd23, update4 ; Conan2 CMakeDeps/CMakeToolchain AVANT CMake.
Cinq dépendances inchangées. CMake >=3.25 et sonde compile/link format/stacktrace
conservées. Métadonnées MSVC simulées ne prouvent ni version installée ni STL.
Prepare prévoit --fresh --preset windows-msvc-{debug,release}, -B arbre distinct
*-verification, --toolchain de ce même arbre ; build ne reconfigure rien.
La future cible CMake parent est une autre branche, pas réparée ici.
Versions installées, configure/build, compile_commands, découverte CTest et
Windows/Linux natifs : NON_EXECUTE. Ne pas fournir à clangd une base inventée.

## Résultat auteur

Contrôles effectivement exercés : analyse AST Python des deux nouveaux fichiers,
sans import local ni exécution, syntaxe OK ; comptage AST **7 baselines et 11 tests
de contrat proposés**, pas 18 tests passés. SHA256 source normalisée recoupée avec
le pin : `1884d29431ecf01129c4318d0603f14fe8fc0cf9e360bf7a8baa9e42b1d7f66e`.
Syntaxe et runtime PowerShell NON_EXECUTE, donc harness non encore démontré.
Serena reste lié à la racine canonique ARTIST avec seul serveur C++ : ces nouveaux
fichiers Python/PowerShell du worktree sont examinés avec les outils fichiers,
pas présentés comme vérifiés par LSP. Aucune compile DB nouvelle/configuration.

Livré pour revue RED_READY ; aucun résultat de campagne ni gain. Localisation :
nouveaux tests/fixtures de ce dossier ; impact production nul ; limites : tests
et PowerShell NON_EXECUTE, mapping/feuilles proposés à ratifier par le lead.

## Correction fixture après la première baseline lead (pas de RED production)

Le lead a exécuté **seulement les 7 baselines** du candidat `f43b38fe` :
3 failures (sous-cas fakechild 0/17 et baseline exception), 0 error/skip.
Les résultats JSON ont été relus par l'auteur via Context Mode : code96,
événements vides, parent synthétique inchangé ; première cause réelle :
`$script:fixtureEvents` non définie depuis l'appel par `root-probe.ps1`.
Les refus mutants attendus ont aussi des harness_error et ne sont pas ces
3 échecs. Logs lead :
`out/verification-synthetic/windows-helper-functional-baseline-2026-10-30T08-13-41-983Z/` ;
cas détaillés dans ce worktree :
`out/verification-synthetic/windows-helper-functional/20261030T081342Z-2adbc39b1a454516ac7981ff401966f1/`.
Le nominal prepare RED n'a **pas été exécuté**. Le mapping de l'ancien oracle
est reçu, mais sa ratification effective attend baselines valides puis RED causal.

Hypothèse nouvelle bornée : `$script:` dans les fonctions appelées depuis un
autre fichier se résout vers le scope de ce fichier extrait, pas celui du runner
TEST-OWNED. Remplacer les seules références de stockage des feuilles par une
liaison explicite indépendante de ces scopes de fichier.

Correction proposée, NON_EXECUTE par l'auteur :

- Une table nommée `ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1`, exclusivement
  TEST-OWNED, est enregistrée dans le scope Global du **seul interpréteur isolé**.
  Elle contient input/case, journal, map d'environnement synthétique et index de
  fakechild. Aucun root/tooling/venv dérivé n'est partagé par cette table.
- Nouvelle fonction fixture `Get-FixtureState` : lit explicitement
  `$global:ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1`. Les feuilles récupèrent
  cette même table, jamais `$script:fixtureEvents`/Input/Environment/Case/Index.
  Le global PowerShell de ce faux processus n'est ni une variable d'environnement
  OS, ni un état/API de production, ni un état Python persistant.
- Nouvelles commandes de scope à relire : `Get-Variable -Scope Global`
  (contrôle de collision et absence finale), `Set-Variable -Scope Global`
  (création unique) et `Remove-Variable -Scope Global` (cleanup). Nom constant,
  pas de nom reçu du payload. Collision : refus HARNESS sans écraser ni supprimer
  la liaison préexistante ; aucune suppression avec Force.
- Snapshot des résultats synthétiques puis suppression en `finally`, même sur
  nonzero/exception/exit source ou panne de snapshot. Une panne de suppression
  n'est pas ignorée. Aucun appel de feuille après cette suppression. Le résultat
  porte `fixture_state_removed`, vérifié par une assertion supplémentaire commune.
- 7 baselines et 11 contrats conservés, aucune assertion précédente retirée.
  Extraction, source ordre/root, whitelist source, spawn, ancien oracle, helper,
  adaptateurs, WRAPPERS/NATIVE et tous leurs pins restent inchangés. Pas de nouvelle
  commande autorisée au code source extrait. Le probe et l'entrée restent deux
  scripts séparés dont root provient du vrai PSScriptRoot, sans hoisting/injection.

Expérience discriminante prévue après revue lead du delta : reprendre exactement
la classe HarnessTests (commande nominative plus haut). Les baselines 0/17
doivent observer chacun leur fakechild et code brut ; la baseline throw doit
observer la cause FixtureChildException ; parent absent/vide restauré ;
`fixture_state_removed=true` sur ces cas ET tous les audits de rejet.
Les **7 baselines doivent réussir** avant la seule commande RED prepare prévue.
Un nouvel échec de fixture reste HARNESS/INCONCLUSIVE, jamais RED production.
Après deux essais sans information nouvelle, changer d'expérience, pas élargir
le harness ni injecter root. Aucune qualification/commande native autorisée.

Auteur : seuls analyse des données existantes, AST Python sans import local,
inspection du diff et contrôles Git. Aucun unittest/PowerShell/faux enfant lancé
pour cette correction ; GREEN production toujours interdit. Les feuilles natives
futures restent une proposition à détailler/revoir avant GREEN, pas un livrable
fonctionnel déjà démontré.
