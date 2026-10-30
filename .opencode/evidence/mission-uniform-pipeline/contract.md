# Contrat ratifie — pipeline de verification portable, version 1

Ratification coordinateur, mission team_mvu0gbxm_0000_cwsmsw, revision native 1. Source avant travaux : fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6. Candidat architecte complet result_mvu180xv_000g_9w08hw. Les decisions locales ci-dessous remplacent les chemins et variantes proposes dans ce candidat lorsqu'ils different.

## 1. Mandat et limites

Livrer l'outil et des scenarios reels pour qualification ulterieure sous Windows et Linux. Aucun gain requis. Pendant cette mission, seuls les tests du pipeline sur donnees synthetiques et processus factices sont executables. Pas de compilation moteur, suite moteur, rendu, benchmark, installation de dependance, execution distante ni cloud. Les controles C++/GL sont statiques ; leur compilation et execution restent NON_EXECUTE. Les tests Python doivent empecher tout appel accidentel aux outils reels.

OpenGL existant seulement : GLFW 3.3, GLEW, contexte 3.3 core. Ni EGL/ANGLE ni nouveau backend. ADR-0004 reste Proposed/partial, ADR-0006 Proposed/not-started. Ce contrat adopte localement une procedure de comparaison, pas ces ADR globalement.

Diff preexistant profils .opencode/agents/*.md, rapports autre mission et .tmp exclu. Aucun changement dans ARTIST/include/. Pas de timestamps GPU, UBO/SSBO, autres types prepares ou framework de benchmark general.

## 2. Decisions de structure et ownership

Core, proprietaire coeur :

- scripts/verify.py (CLI uniquement), scripts/verification/__init__.py
- scripts/verification/model.py, catalogue.py, planning.py, runner.py, evidence.py, reporting.py
- scripts/verification/adapters/__init__.py et base.py (interfaces seulement)
- scripts/tests/verification_core/test_*.py

Windows, proprietaire Windows : scripts/verification/adapters/windows.py, scripts/build-windows.ps1 si necessaire et scripts/tests/verification_windows/test_*.py.

Linux, proprietaire Linux : scripts/verification/adapters/linux.py, scripts/build-linux.py, scripts/linux-evidence.py si necessaire, CMakePresets.json, conan/profiles/linux-gcc, scripts/tests/verification_linux/test_*.py. Seul Linux modifie les presets ; préserver les interfaces historiques.

Scenarios, proprietaire OpenGL : ARTIST/verification/ (CMakeLists.txt, CpuUniformWorkload.cpp, OpenGLFloatWorkload.cpp et supports locaux), scripts/verification/scenarios/ (descriptions additionnelles pures si utiles), scripts/tests/verification_scenarios/test_*.py. Pas d'edition des tests existants, CMake parents, presets ou modules core. Les declarations centrales du catalogue restent au core ; transmettre a l'integrateur tout raccordement interbranche.

Integration, apres fusion des producteurs : CMake parents, nouveau .github/workflows/verification-pipeline.yml et docs/development/verification-pipeline.md ; transfert explicite des ownerships si raccordements dans fichiers producteurs. Option CMake ARTIST_BUILD_VERIFICATION=OFF par defaut ; cibles ArtistUniformCpuWorkload et ArtistUniformOpenGLWorkload. Aucune inscription automatique aux suites CTest ou campagnes CI existantes.

Dependances nouvelles : aucune. Python standard library ; C++ standard library pour chronometrage par lots. Les dependances moteur existantes restent celles de Conan. Google Benchmark reste une extension hors tranche, non installee. std::chrono::steady_clock mesure des intervalles muraux cote CPU, pas une consommation CPU thread ni un temps GPU.

## 3. Profils, routes et catalogue ferme

Profils : cpu-only (sans contexte GL), opengl-software (Linux llvmpipe explicitement demande), opengl-hardware (GL reel, renderer compatible avec cible materielle declaree). Windows logiciel indisponible explicitement dans v1. Le nom d'un GPU ou nvidia-smi ne prouve pas le contexte utilise. Ambiguite => UNVERIFIED, jamais hardware par exclusion des seuls noms logiciels connus. Une mesure CPU de soumission GL exige GL ; pas de repli silencieux vers logiciel.

Routes de la comparaison principale : pass-name (Pass::withUniform<float>) et prepared-float (prepare une fois, puis set(float)). Pas de quatrieme route lookup-name redondante. borrowed-uniform peut etre ajoutee seulement comme diagnostic secondaire borne, sur owners vivants, si l'integrateur le juge utile ; elle ne conditionne pas la livraison.

Scenarios statiques :

1. cpu.float-mirror : workload C++ sans GL, miroir CPU via API existante, valeurs deterministes et oracle. Il ne prouve pas d'upload.
2. gl.float-routes-correctness : deux routes, 0 -> .5 -> 0, miroir CPU, readback natif et pixels fixes independants ; egalite A/B supplementaire, jamais oracle unique. Reutiliser les shaders existants test-datas/shaders/vertex/good/f2-single-values.vert et fragment/good/minimal.frag, leurs parametres stables initialises hors mesure. Conserver leurs empreintes. Triangle et FBO RGBA8 lineaire 32x32 ; reference aux points fixes existants loin des bords. Pour lots 1/16/256, cellules 32x32 distinctes (grilles 1x1/4x4/16x16), viewport par cellule ; update utile puis draw dans chaque cellule, changement de phase a chaque repetition. Chaque cellule est verifiee, pas seulement la derniere. Egalite interdriver bit-a-bit non promise.
3. gl.float-preparation : preparation apres link/reflexion, nominal et refus des noms absents, types incompatibles, arrays y compris taille1, contexte incorrect et metadonnees invalidees. Raccorder les tests existants pertinents via selections CTest dans le catalogue pour la future campagne plutot que recopier tous leurs internes.
4. gl.float-lifetime : selection bornee des tests existants de mauvaise cible, contexte absent, retrait/destruction/revocation et ancienne incarnation numerique. Aucune dereference apres retrait. CTest et tous ces executables restent interdits dans cette mission.
5. gl.float-submission-cpu : deux routes et meme travail ; separer preparation, updates seuls (microdiagnostic), et updates+draws utiles. Chauffe, repetitions, ordre reproductible et controle A/A. Un controle de travail distinct observe uploads/draws et absence de nouvelles resolutions natives pour A comme B ; instrumentation, logs, readbacks et assertions hors fenetre chronometree. Les gardes de production restent actives. Aucune promesse d'allocation nulle. Valeurs, noms et donnees prepares hors chrono.

Le catalogue decrit pour chaque scenario : id/version, profils/plateformes, routes, configurations, cibles requises, producteur workload ou selection CTest, fixtures/shaders, oracle, metriques, timeout et ressources exclusives. Les chemins de fixtures sont toujours resolus depuis la racine source, pas le cwd utilisateur. Aucun import ou listing ne charge un executable/GL.

Les selections de tests existants doivent etre explicites et non vides a la decouverte future, avec noms et retours reconcilies. Pas de nombre historique 589 code en dur. Un scenario sans producteur implementé/raccorde ne remplit pas l'acceptation, meme si son plan est imprimable.

## 4. API Python stable entre branches

Les noms ci-dessous sont normatifs. model.py fournit dataclasses immuables, enums et serialisation JSON to_dict/from_dict avec validation. Les listes serialisees deviennent tuples dans le modele. SCHEMA_VERSION=1. Les noms de champs de cette section sont ceux echanges par les adaptateurs ; un changement d'interface est arbitre par le lead et transmis a toutes les branches.

Request : platform (windows|linux), profile, scenarios (tuple IDs), configuration (debug|release), routes (tuple, vide=defaut scenario), iterations (>0), repetitions (>0), batch_size (1|16|256), measurement_requested (bool), allow_dependency_resolution (bool=False). Toute option inconnue ou valeur hors bornes est rejetee. Les bornes de charge et timeout sont positives et plafonnees ; leur valeur est incluse au plan.

ScenarioSpec : id, version, profiles, platforms, routes, configurations, targets, fixtures, producer, oracle, metrics, timeout_seconds. producer distingue workload et selection_ctest. Catalogue ferme sans plugins ni noms de modules issus d'une entree utilisateur.

CommandSpec : argv (tuple de str), cwd (str), env_delta (mapping str->str), timeout_seconds (positif), phase (prepare|build|run), declared_effects (tuple de str). Aucun shell=True ni interpolation shell de donnees utilisateur ; PowerShell via fichier/arguments valides, pas expression libre.

VerificationPlan : schema_version, request (Request), scenarios (tuple ScenarioSpec), commands (tuple CommandSpec), unresolved (tuple str). plan_sha256 se calcule sur representation JSON canonique du plan sans champ hash autoreferent. list/plan n'exigent aucune observation locale de Git/toolchain/device.

ProcessOutcome : returncode (int ou null), stdout (str), stderr (str), timed_out (bool), interrupted (bool). Le runner reel ecrit les journaux complets et garde les sorties partielles ; les valeurs stockees dans les preuves ne doivent pas etre tronquees silencieusement.

PreparationReceipt, BuildReceipt, ExecutionReceipt : schema_version, run_id, plan_sha256, phase, state, artifacts (mapping de chemins relatifs et empreintes), details (mapping JSON). En l'absence d'une preuve requise, pas de receipt valide reconstruit depuis un ancien dossier.

Interfaces :

    catalogue.list_scenarios() -> tuple[ScenarioSpec, ...]
    planning.make_plan(request: Request, catalogue: tuple[ScenarioSpec, ...]) -> VerificationPlan
    PlatformAdapter.describe_preparation(plan: VerificationPlan) -> tuple[CommandSpec, ...]
    PlatformAdapter.describe_build(plan: VerificationPlan, preparation: PreparationReceipt) -> tuple[CommandSpec, ...]
    PlatformAdapter.describe_execution(plan: VerificationPlan, build: BuildReceipt) -> tuple[CommandSpec, ...]
    PlatformAdapter.collect_environment() -> dict
    ProcessRunner.run(command: CommandSpec) -> ProcessOutcome
    evidence.prepare(plan, adapter, sink) -> PreparationReceipt
    evidence.build(plan, preparation, adapter, sink) -> BuildReceipt
    evidence.execute(plan, build, adapter, sink) -> ExecutionReceipt
    reporting.report(bundle) -> dict

PlatformAdapter et ProcessRunner sont des Protocols exportes par adapters/base.py et runner.py respectivement. Les doubles sont injectes explicitement ; les tests ne doivent jamais basculer vers un runner reel si une doublure manque. Les describe_* sont purs. collect_environment est reserve aux phases a effets, jamais list/plan. Le core choisit l'adaptateur par table explicite et imports locaux purs.

## 5. CLI et effets

    python -B scripts/verify.py list [--json]
    python -B scripts/verify.py plan --platform windows|linux --profile PROFILE --scenario ID [--scenario ID ...] --configuration debug|release [--route ROUTE ...] [--iterations N] [--repetitions N] [--batch-size 1|16|256]
    python -B scripts/verify.py prepare <selection identique> --output-root PATH [--allow-dependency-resolution]
    python -B scripts/verify.py build --bundle PATH
    python -B scripts/verify.py run --bundle PATH
    python -B scripts/verify.py report --bundle PATH [--json]

Sans sous-commande : aide seulement. verify.py fixe sys.dont_write_bytecode avant imports locaux. list/plan : aucun subprocess, reseau, ecriture/dossier/bytecode, sonde Git/toolchain/GL ni chargement workload ; stdout uniquement. Les identites non connues sont unresolved, pas inventees. report lit uniquement et imprime ; il ne relance rien.

prepare : cree bundle unique, capture identites/environnement et configure explicitement ; aucun workload ni build. Les helpers existants peuvent resoudre des dependances : sans allow_dependency_resolution, refuser leur mode potentiellement reseau, meme si un cache semble present. Aucun pip implicite en qualification. build : cibles requises seulement et receipt compatible, aucun test ; run : executables deja construits et identites compatibles, aucun prepare/build implicite. Ces commandes reelles sont livrees mais NON_EXECUTE dans la mission.

## 6. Adaptateurs et compatibilite

Windows : reemploi build-windows.ps1 pour prepare explicite ; pip/Conan possibles seulement en bootstrap autorise. Conserver l'environnement MSVC pendant toutes les commandes enfants puis le restaurer. Ne pas appeler ConfigureOnly puis supposer l'environnement PowerShell persistant dans un Python parent. Fichiers avec espaces et code retour enfant preservés. Profils Debug/Release/ASan distincts ; pas d'ASan/coverage dans une mesure optimisee.

Linux : profil Conan versionne, installation avant CMake, linux-release configure/build/test explicite, type de build host/build coherent, arbres distincts. Profil logiciel Linux historique et gardes llvmpipe conserves. Pour hardware : aucune variable forçant logiciel, cible attendue et renderer observe concordants. Xvfb n'est ni installe ni choisi silencieusement pour remplacer le materiel. Gardes de linux-evidence.py reutilisees sans appeler son main de campagne. Toute extraction dependante du core attend integration/transfert explicite.

Un seul ecrivain par arbre de build et bundle ; contexte GL exclusif lors de la future qualification. Pas de mutation concurrente de source. Les identites avant/apres detectent les changements persistants, sans promettre une transactionnalite contre modifications transitoires annulees.

## 7. Protocole C++ borne

Pas de lecteur JSON generaliste en C++ ni dependance JSON ajoutee. Python cree le plan et passe des arguments explicites au workload :

    ArtistUniformCpuWorkload --scenario ID --route ROUTE --iterations N --repetitions N --batch-size N --request-id ID --run-id ID --plan-sha256 HASH --result PATH
    ArtistUniformOpenGLWorkload <memes options> --profile PROFILE --measurement-requested true|false

Valeurs enum connues et entiers via parseur strict borne (from_chars), sans exception silencieuse, arg inconnu/duplique refuse. Selection de route une fois par lot, pas par update. Les noms/valeurs ne deviennent pas types C++.

Le workload emet un resultat JSON versionne par fichier cree exclusivement, sans ecraser un ancien resultat ; ecriture incomplete jamais acceptee. Escape correct des chaines natives/chemins ; stdout/stderr sont logs, pas protocole a parser. Toute dependance sur JSON request et code generator est exclue de v1.

Resultat requis : schema_version=1, run_id, request_id, plan_sha256, scenario, scenario_version, route, profile, parameters (iterations/repetitions/batch_size/measurement_requested), operations_expected, operations_observed, test_verdict, measurement_validity, observations, samples, environment. Les echantillons possedent metric, value, unit, repetition et population. Les fichiers references restent dans le bundle ; paths absolus/../symlink sortant sont rejetes par le collecteur. Pas de NaN/inf presente comme mesure valide.

L'executable GL possede contexte, ressources et owners ; emprunts et generations ne prouvent pas la fin d'utilisation GPU. Pas de relink/delete natif externe. Destruction dans l'ordre, attentes/readbacks hors chrono et declares. Horloge monotone par lots, chauffe et controle A/A, repetitions bornees, resolution/unites/politique d'ordre conservees. Pas de glFinish par update, temps CPU jamais etiquete GPU. Une mesure absente, insuffisante ou incoherente n'est pas zero.

Codes workloads : 0 oracle satisfait et resultat conforme ; 10 oracle faux ; 20 precondition indisponible ; 30 erreur interne/protocole. Codes natifs bruts conserves ; code0 sans resultat conforme n'est pas PASS.

## 8. Etats, agregation et artefacts

pipeline_state : NOT_STARTED|RUNNING|COMPLETED|FAILED|INTERRUPTED|BLOCKED.
test_verdict : NOT_EXECUTED|PASS|FAIL|INCONCLUSIVE.
measurement_validity : NOT_REQUESTED|ABSENT|VALID|INVALID.
freshness : CURRENT|STALE|UNVERIFIED.

Oracle faux peut donner pipeline COMPLETED et test FAIL, mais le CLI run retourne un code non nul. Mesures de scenarios a oracle faux => INVALID, jamais publiees comme comparaison valide. Manifeste incomplet => aucune conclusion favorable. list/plan code0 ne signifie pas tests PASS.

CLI : 0 operation conforme ; 2 demande invalide ; 3 precondition ; 4 panne/protocole/identite ; 5 test echoue ; 6 inconclusif/mesure demandee invalide ; 130 interruption. Priorite agregee : interruption, panne pipeline, echec domaine, precondition, inconclusif, succes. Toutes les causes individuelles restent visibles. Selection vide, doublons, omissions et skips requis ne sont jamais succes.

Bundle : out/verification/<UTC-UUID>/ avec plan.json, manifest.json, identity-before.json, identity-after.json, environment.json, commands.jsonl, logs/, raw/, report.json optionnel explicite. Tous les documents propres au pipeline : schema_version, run_id, plan_sha256 ; formats externes JUnit restent bruts et references par manifeste.

Conserver HEAD/delta, empreintes sources/config/scripts/catalogue/contrat/shaders/executables, dependances resolues, options build, plateforme, device/driver observes, argv/cwd/env filtre/horaire/code/logs et donnees brutes. Jamais dump complet d'environnement ni secrets. Decouverte/selection/execution reconciliees par noms et populations ; stale, absences et formats invalides refusés. Echecs/interruptions gardent les sorties disponibles sans masquer le code enfant. Rapport sans preuves n'execute rien et declare les manques.

Precision locale ratifiee apres re-audit : CURRENT concerne la population actuelle fermee de fichiers pertinents deja utilisee pour admettre run, pas seulement les anciens chemins empreintes. Pour le build, cela comprend les cibles requises avec correspondance unique et les configurations/bibliotheques deja enumerees par le controle d'identite. Un ajout dans ce domaine ou une ambiguite de cible donne STALE/code4 lors du rapport, sans processus, sonde de revision ou ecriture. Cela ne rend pas faux le resultat historique conserve. Aucun rejet de tout nouveau fichier genere hors de ce domaine n'est exige. Admission et rapport doivent reutiliser la meme regle privee d'enumeration, sans second schema public.

## 9. Validation dans la mission et barrieres

Tests synthetiques en RED/GREEN puis non-regression, auteur de tranche et runner lead distingues : modes purs, schemas, plans, commandes, erreurs/timeout/interruption, resultats manquants/malformes/stale, shaders/binaires modifies, mauvais renderer, comptages, faux succes code0, valeurs nulles/negatives et mesures invalides. Les processus reels autorises sont exclusivement programmes factices appartenant aux tests, jamais outils ou executables moteur. Les commandes exactes dependent des fichiers integres : python -B -m unittest discover -s scripts/tests/verification_<branche> -p 'test_*.py' -v.

Les auteurs doivent proposer tests et commandes avant code ; le lead observe le RED attendu avant implementation minimale, puis observe GREEN. Une panne d'import accidentelle ou d'environnement n'est pas RED causal. Les parties C++ non executables sous mandat restent explicitement sans RED/GREEN ; leur revue statique ne devient pas compilation/image/performance prouvee.

Contrat et plan QA persistes/commites avant worktrees auteurs. Chaque branche commence du HEAD contenant ces entrees. Producteurs integres avant integration, puis generator des seuls gaps demontres et audit fresh. Aucun transfert implicite de diff non commite. Les profils QA ont restitution textuelle seulement, lecture et permissions effectives a qualifier. Pas de sandbox presumee.

Reception : fichiers reels et raccordes, tests synthetiques attribues, revue des risques modifiés et limites honnetes. Livraison pour qualification Windows/Linux, jamais fonctionnement graphique ou performances demontres. Aucun gain exigé ; qualification cloud hors mission.
