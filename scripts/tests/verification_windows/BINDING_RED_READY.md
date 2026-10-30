# Windows intégré — PLAN borné et TESTS_RED_READY

## Identité et périmètre auteur

- Tâche `task_mvukzhsc_0052_f1bbfq`, auteur unique pool-23 ; PM/LLM brut69,
  retenu70, risque58, incertitude moyenne, sans changement de politique/modèle.
- HEAD observé avant écriture : `8c7e51b28ae085ada2eb98c97b292fa7147e2988`,
  branche `ensemble-4b686588-c97c-46ab-99a1-e176a14b9184`, worktree
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-23`.
  Worktree propre à l'entrée. Le cwd initial pointait sur main ; corrigé par
  `session_move` vers le worktree existant AVANT toute écriture. Aucun fichier
  de main ni ses profils préexistants n'a été modifié par cette tranche.
- Sources d'autorité lues : AGENTS, contract.md, qa-plan.md dont §11, binding v1,
  rapports81/90/95, base/model/evidence/runner/protocol/planning/CLI intégrés,
  fondations Windows et helper. Serena reste attaché au canonique ARTIST, LSP
  C++ seulement ; il ne décrit pas ces nouveaux Python du worktree. Navigation
  de fichiers ciblés, aucun changement de contrat commun, aucune preuve LSP C++.
- Fichiers possédés : nouveau `adapters/windows.py`, helper `build-windows.ps1`
  pour une tranche ultérieure seulement, nouveaux tests/docs de ce répertoire.
  Core/model/runner/reporting/verify/base/C++/CMake/presets/workflows exclus.
  Anciennes assertions51, fixtures et pins INCHANGÉS au RED.

## Remise actuelle (aucun GREEN)

`windows.py` est un scaffold nominal : constructeur stockant seulement la racine,
quatre méthodes levant explicitement NotImplementedError. Il évite ImportError,
ne valide ni environnement ni reçu et ne prétend pas fournir une plateforme.
Les tests convertissent ces placeholders en assertions causales ; le core peut
aussi les conserver comme cause d'un reçu FAILED/code4. Le helper réel conserve
son refus avant dispatch. Aucun câblage de production n'est ouvert.

Deux modules sélectionnables :

1. `test_adapter_binding.py` : 27 méthodes, vrais Request/VerificationPlan et
   PreparationReceipt/BuildReceipt produits par `evidence.prepare/build`,
   fixtures source/build inertes sous un TemporaryDirectory possédé. Le mapping
   exécutable admis contient volontairement un sous-répertoire avec espaces :
   concaténer build_root/target.exe est incorrect. Les doubles sont des objets
   en processus, pas des programmes. Aucun subprocess ne peut être leur fallback.
2. `test_binding_helper_interface.py` : une méthode source-only, chemins privés
   et timeout du validateur natif déjà intégré mais absents des paramètres du
   helper. Pas d'exécution PowerShell et pas d'assertion de fermeture affaiblie.

`binding_fixtures.py` réutilise RuntimeFixture et son écrivain JSON synthétique,
alignés sur le vrai plan Windows. Il ne copie pas de modèle/schema/runner de
production. La préparation de contrôle est explicitement un seed factice pour
isoler describe_build/run, pas un succès de WindowsAdapter. Pour les tests de
préparation réelle, collect_environment est doublé explicitement et testé à part.
Le faux runner reconnaît les commandes de préparation/build puis écrit cache et
binaires INERTES ; pour run il n'interprète que le workload visible admis, jamais
CTest. L'absence d'implémentation ne peut pas conduire à lancer un outil réel.

## Oracles et bornage du plan

- **B1/B4/B6, T1/T5/T7** → 01–04,25–27 : signatures intégrées, construction et
  descriptions pures (sentinelles d'écriture/process/réseau), deux bundles
  intercalés sans dernier-bundle mutable, configuration debug/release traduite
  seulement au pont helper, list/plan sans factory, pas de dump secret/device
  probe. ASan n'existe pas dans Request : rejet de debug-asan, aucun drapeau
  historique ASan injecté ; pas d'extension du schéma pour l'ajouter. Windows
  logiciel refusé sans fallback matériel.
- **Exécution directe ratifiée** → 05–12 : exact build_workload_argv partagé,
  options et valeurs visibles, executable issu du mapping ET identity.files du
  BuildReceipt, cwd source root, résultats uniques sous raw, IDs uniques entre
  invocations et différents entre deux runs du même plan, descriptions répétables.
  CTest direct, découverte avant sélection exacte, même arbre -verification et
  Debug/Release, -j1, --no-tests=error, six env de contexte explicitement issues
  du plan/run ; aucun nom RTX utilisé comme capacité. CPU sans variable GL.
  Absence de mapping, inventaire absent ou mapping hors arbre refusés sans effet.
- **B2–B5, T2–T6** → 13–24 : opt-in avant bootstrap, payload exact matérialisé
  par le sink et empreinté/publié avant faux runner ; build sans résolution,
  même payload, logs communs ; mutation et collision sans réparation ni enfant,
  code enfant17 conservé comme cause avec code pipeline4 et bruts ; exécution
  CPU synthétique recueillie uniquement par le core ; no-clobber du résultat,
  reçu étranger/binaire modifié/ambigu refusés et cible hardware absente code3.
  Fermeture/native reparse ne sont pas reproclamées démontrées : réemploi des
  tests B2 intégrés et fondations51, pas une nouvelle preuve de l'OS.
- **S3/S7** → test helper source-only : paramètres privés de transport et timeout.

Séquence après réception de cette remise : lead relit fixture/test/scaffold,
observe RED causal et garde ses logs ; ratifie le pont ci-dessous et le mapping
de l'ancien oracle ; seulement ensuite auteur GREEN minimal (Python et helper),
puis contrôles factices ciblés, non-régression et revue indépendante du delta
ownership/env/exécution. Pas de nouvelle infrastructure ni campagne moteur.

## Couture outil manquante : proposition AU LEAD AVANT production

Constat : le helper legacy installe Conan/Ninja sous out/tooling/build/Scripts,
mais `scripts/build-requirements.txt` ne contient PAS CMake. Il serait faux de
présumer que ctest.exe s'y trouve. L'environnement vcvars établi dans un enfant
PowerShell ne persiste pas dans le parent Python. Un simple argv0 `ctest` dans
describe_execution masquerait cette lacune ; le test07 exige un chemin absolu.
Lecture historique ciblée confirmant la couture disponible : le cache canonique
`C:/Users/djden/source/repos/ARTIST/out/build/windows-msvc-debug/CMakeCache.txt`
contient CMAKE_COMMAND et CMAKE_CTEST_COMMAND vers `C:/Program Files/CMake/bin/`
(lignes430/436), Ninja et Debug (lignes482/40). Ce cache n'est NI réutilisé dans
le worktree NI pris comme qualification du candidat, seulement comme preuve des
champs existants. Aucune commande CMake/CTest n'a été exécutée pour cette lecture.

Proposition privée minimale, à ratifier avant GREEN :

1. Prepare/build gardent le helper et les seuls enfants reçoivent MSVC/SDK x64,
   tooling PATH et CONAN_HOME. Préparation seule peut bootstrap/résoudre après
   opt-in. Build ne réinstalle rien. Conan2 précède CMake avec profil existant,
   outil CMake préinstallé, et PRE_TEST pour les sélections CTest.
2. Réutiliser le pont DÉJÀ fourni par CMake : `CMAKE_COMMAND:INTERNAL` et
   `CMAKE_CTEST_COMMAND:INTERNAL` dans le CMakeCache.txt du même arbre. Le cache
   est déjà couvert par `BuildReceipt.details.identity.files` via _build_files.
   Aucun dump d'environnement, stdout vcvars, secret, marqueur JSON, fichier de
   pont ou champ Request/receipt/CommandSpec supplémentaire n'est nécessaire.
3. describe_execution vérifie le digest du cache contre l'inventaire du reçu
   avant lecture de ses deux entrées uniques, puis décrit l'outil absolu
   CMAKE_CTEST_COMMAND lié à l'installation CMAKE_COMMAND. Absence/doublon/forme
   invalide/installation incohérente refuse, jamais shutil.which/fallback PATH
   ou dernier bundle. Le reçu reste l'autorité ; cette lecture pure ne certifie
   pas la machine ni un contexte GPU. Le helper garde dans ses stdout déjà
   capturés les versions/chemins/empreintes des outils effectivement employés
   pour l'identité de qualification, sans imposer un nouveau parser public.
4. Les workloads utilisent les exécutables admis et leurs répertoires runtime
   issus du build, pas le PATH complet vcvars. Le besoin de DLL supplémentaire
   doit être explicite/refusé, pas résolu par un dump global. CTest hérite des
   six valeurs déclarées dans CommandSpec.env_delta ; CPU n'en déclare aucune.

Cette proposition nécessite des négatifs supplémentaires seulement APRÈS
ratification : cache absent/modifié, entrée absente/doublée/malformée, chemin
incompatible avec l'installation CMake liée et préservation des logs. Test07
modélise les deux entrées CMake existantes dans le cache synthétique et exige
ce chemin précis ; la remise ne prétend pas couvrir les négatifs supplémentaires.
Si le lead choisit une autre couture privée, son mapping de ce test sera explicite.
Aucun besoin d'édition core accepté implicitement.

## Mapping EXPLICITE avant toute modification PowerShell

- `test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch`
  est l'unique attente ancienne identifiée comme intentionnellement obsolète
  lors de l'ouverture. Elle reste INTACTE ici. Proposition de remplacement
  ultérieur : garde d'entrée qui valide phase/flags/payload/opt-in avant tout
  effet et quitte la route vérification sans legacy fallback ; tests négatifs
  effets0, plus nominal prepare/build via fragments relus. Ne pas supprimer
  simplement ce test pour verdir.
- Param block : ajout de VerificationInputFile, VerificationBundleRoot,
  VerificationLogDirectory, VerificationTimeoutSeconds alignés au validateur
  Resolve-VerificationPhaseFlags déjà intégré. Ancien Configuration/Jobs et
  les drapeaux legacy restent identiques. Le nouveau test source-only vérifie
  ce delta ; test_helper_contract existant reste intact.
- Bloc de refus lignes513–518 : remplacement UNIQUEMENT après RED et admission
  lead du mapping ci-dessus. Dispatch existant lignes520–572 conserve son ordre
  Conan→CMake et son finally. Toute modification de ce fragment exige relecture
  de `dispatch_fixture.ps1` et de ses oracles AVANT édition, pas un pin seul.
- Nouvelles fonctions de liaison : région distincte, hors WRAPPERS/NATIVE.
  Les feuilles Read/Write-VerificationEnvironmentLeaf,
  Read-VerificationMsvcEnvironmentLeaf et Invoke-VerificationProcessLeaf
  seront raccordées après validation ; aucune duplication/altération native
  admise par ce document. Extracteur de fragments et doubles d'outils/Process
  seulement : événements ordonnés, nonzero, exception et restore, opt-in absent,
  build sans bootstrap, legacy non atteint. Aucun appel au helper complet,
  cmd/vcvars/MSVC/CMake/Conan/CTest réels dans ces futurs tests.
- Régions WRAPPERS et NATIVE + `native_source.sha256` : inchangées ; aucun repin
  nécessaire/autorisé par défaut. Si une fonction pinée doit changer, remettre
  exactement son diff, les attentes touchées et l'admission indépendante avant
  réexécution. Le pin N'EST PAS une sandbox.
- Code legacy sous le dispatch : préserver le chemin historique. Une éventuelle
  extraction de fonctions communes nécessite son propre mapping détaillé AVANT
  modification ; pas de refactor implicite de la campagne historique ici.
- Capture privée helper-run reste source-only HORS preuve du run commun ; limite
  connue Dispose/writers hérités inchangée. Test06 jonction NON_EXECUTE ; jamais
  discovery native globale. L'ouverture du helper ne qualifie pas ces gaps.

## Commandes RED exactes pour le runner LEAD (non lancées par l'auteur)

Cwd du worktree ci-dessus, puis :

```powershell
Set-Location scripts/tests/verification_windows
C:/Python313/python.exe -B -m unittest -v test_adapter_binding test_binding_helper_interface
```

28 méthodes proposées selon AST (27+1), pas un comptage découvert/exécuté. Attendu :
nonzero, assertions `CAUSAL RED: Windows ... binding is not implemented`, reçus
FAILED/code4 au lieu des nominales et paramètres privés non liés ; AUCUN ImportError,
outil réel absent ou contexte GPU manquant ne constitue le RED accepté. Quelques
contrats core/consultation déjà intégrés peuvent être verts : ils sont contrôles
de composition, pas preuve du nouveau raccordement.

Les tests temporaires détruisent leurs seules fixtures synthétiques à cleanup ;
le lead conserve commande/cwd/interpréteur/hash, stdout/stderr et résultat unittest
sous une nouvelle sortie exclusive `out/verification-synthetic/`. Pas de commande
build/test moteur rendue exécutable dans ce mandat.

Non-régression future : modules nouveaux puis cœur/contexte/scénarios synthétiques
intégrés ; fondations51 via la sélection NOMINATIVE du rapport81 et son manifeste,
hors06 et sans discovery native. Ne pas lancer `unittest discover` sur tout ce
répertoire Windows ; cela sélectionnerait la jonction non autorisée. L'ancienne
assertion de fermeture ne peut être adaptée qu'après réception du mapping.

## Matrice réellement exercée et limites

- Auteur Windows/Python : lecture de fichiers/objets Git et AST sans import projet
  seulement. AST réellement accepté pour les quatre nouveaux .py sous
  `C:/Python313/python.exe` 3.13.1 via Context Mode : 27+1 méthodes comptées, aucun
  import projet. Aucun test/unittest/enfant factice exécuté. RED/GREEN : NON_EXECUTE.
- Fichiers CMake/presets/Conan/CI inspectés : Ninja, Windows MSVC x64, profils
  Conan2 existants, sondes C++23 format/stacktrace et ordre conservés par contrat.
  Pas de version compilateur/STL/SDK/ABI qualifiée sur ce candidat ; ni build ni
  compile_commands.json artificiel. Baseline ADR0008 Windows Debug seulement.
- Windows Debug/Release natifs, Linux, ASan, découverte CTest réelle, GPU logiciel
  ou matériel : NON_EXECUTE, livrable destiné à qualification ultérieure.
- Aucun moteur CPU/GL, rendu, benchmark, installation, cloud, changement système,
  team tool ou sous-agent. Aucune preuve de performance ni résultat de campagne.
- Les 51 contrôles Windows et les 191+3SKIP/194 core,28 contexte,31 scénarios sont
  preuves LEAD historiques des rapports81/95, NON rejouées par l'auteur.
- Première cause du candidat : attendue, non observée en test. L'adaptateur est
  absent au HEAD de départ ; le scaffold rend explicite ce manque. Le pont outil
  et l'ancien oracle de fermeture attendent décision/revue lead avant GREEN.

Retour au lead pour revue/commit intégré/RED observé. Ne pas clore la tâche et
ne pas poursuivre GREEN avant restitution de cette preuve par le runner.
