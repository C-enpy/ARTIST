# Windows adapter — plan lecture seule sur fondations integrees

## Points Git et portee

- Core lu depuis l'objet Git `107a2d3bbd88ba95a2eb70e904085911b71662bd`, pas depuis
  une copie locale supposee a jour. Commit disponible et interfaces verifiees
  par lecture/AST seulement, sans import ni execution.
- Candidat observation/getters de ce worktree : `6a19ad7e58ae78830d08de0bdce7c748ba269210`.
  Aucun nouveau resultat de run communique pour ce candidat ; fixture GREEN
  toujours NON_EXECUTE par l'auteur. Produit/pin restent stables.
- Le lead annonce rapport73 `3bd72fc`, 142 PASS + 2 symlink SKIP/144 integres,
  contexte28/28 et scenarios31/31 repasses. Ce sont preuves LEAD, pas reruns de
  cette lecture ; aucun de ces comptages ne qualifie notre fixture Windows.
- Aucun checkout/cherry-pick, copie de branche/schema, ajout windows.py, import
  du core ou cablage source. Attendre le point Git coordonne fourni par le lead.
  Ce plan n'est pas une autorisation de commencer ces modifications.

## Interfaces effectivement observees

`scripts/verification/adapters/base.py` declare PlatformAdapter :
describe_preparation(plan), describe_build(plan, preparation),
describe_execution(plan, build), collect_environment().
`scripts/verify.py::_adapter` attend WindowsAdapter(source_root) depuis le module
Windows selectionne de facon fermee, pas un nom/plugin fourni par l'utilisateur.

`planning.make_plan` renvoie des commandes vides et des identites non resolues ;
il ne charge/sonde pas l'adaptateur. Preserver cette absence d'effets pour
list/plan et ne pas materialiser de fichier pour seulement consulter un plan.

`model.Request` accepte les configurations debug/release en MINUSCULES. Le helper
prend Debug/Release : conversion explicite au seul pont CLI, pas nouveau build
tree. `evidence._build_root` Windows utilise deja
out/build/windows-msvc-{configuration}-verification, aligne sur notre resolver.
Pas de cache Windows sous Linux ou de changement implicite de toolchain.

CommandSpec porte argv/cwd/env_delta/timeout/phase/effets declares. L'adaptateur
doit consommer les types integres, pas reproduire dataclasses/from_dict/schemas.
Ne pas inventer une garantie de qualification a partir de metadonnees MSVC.

## Autorite des receipts et executables

Lectures precises dans `evidence.py` :

- `_predecessor` verifie identites run/plan/phase, COMPLETED/exit_code0, egalite
  au receipt persiste, manifeste/plan non modifies et hashes des artifacts.
- `build` verifie le snapshot preparation AVANT l'appel a describe_build, puis
  conserve un snapshot built et la population d'executables dans le BuildReceipt.
- `execute` appelle `_predecessor` puis `_assert_snapshot(..., built=True)` AVANT
  describe_execution ; ne pas court-circuiter cette admission en appelant un
  adaptateur directement pour lancer un workload.
- BuildReceipt expose notamment les references d'executables dans
  details.executables et leurs identites dans details.identity.files, plus les
  racines/bundle/run_id/plan_sha256. Ces champs ne sont pas un nouveau schema
  a recopier : utiliser l'objet du core et ses admissions.

Le plan d'adaptateur devra lier chaque cible choisie a l'entree de ce receipt,
au chemin sous la racine source verifiee et a son identite construite. Aucun
argv0 fabrique par simple concat builddir/target.exe ne devient une autorisation.
Une egalite d'argv dans un fake ne prouve pas l'autorite d'un executable reel.

## Deux coutures a regler au point Git, sans contournement silencieux

1. **Commandes d'execution visibles du core.** `_execution_requests` inspecte
   CommandSpec.argv pour --scenario/--route/--result et les identites/parametres
   attendus, population et sorties non dupliquees. Une invocation PowerShell
   cachant toutes ces options dans notre payload prive n'est pas substituable
   telle quelle : le core ne verrait plus les demandes attendues.

   La description directe de l'executable verifie par le receipt est compatible
   avec cette inspection ; le core garde alors la capture R1/R2. Si le pont
   helper-run est conserve, il faudra au contraire un contrat d'assemblage
   explicitement compatible, pas dupliquer des arguments seulement pour tromper
   l'inspection ou lancer un second processus hors des commandes declarees.
   Choix a fixer par le lead dans l'assemblage deja autorise ; aucun de ces
   chemins n'est implemente/selectionne silencieusement dans cette lecture.

2. **Propriete du fichier prive de preparation.** Le helper natif lit un fichier
   borne ; describe_preparation(plan) ne recoit ni sink, ni bundle, ni receipt.
   `prepare` cree/lease le bundle avant l'appel mais ne lui passe pas ce contexte.
   Ne pas reconstruire un run_id ou chemin bundle non fourni, ni ecrire dans un
   bundle devine ou au moment de list/plan. Fixer a l'assemblage le proprietaire
   de la materialisation/capture de ce transport et ses effets declares, en
   reutilisant les entrees communes. Pas de nouveau modele Request ou receipt.

Ces coutures sont des points d'integration decrits, pas un echec observe ou un
blocage global de la mission. Elles n'autorisent pas a ouvrir le helper complet.

## Reutilisation et frontieres de responsabilite

- Prepare/build : reutiliser garanties Conan2 AVANT CMake, CMakeDeps/Toolchain,
  root/config/toolchain uniques et un ecrivain/build par sortie. Cibles reelles
  existantes ; tests => BUILD_TESTING ON + PRE_TEST, CPU-only conserve OFF.
  Ne pas contourner CheckCxx23 ni son format/stacktrace ; pas de migration outil.
- Les feuilles Windows et capture factice ne sont pas une preuve de vrais outils,
  vcvars/toolset/SDK/ABI, compilation ou lien. Leur qualification reelle reste
  hors execution de cette mission. Aucune compile_commands.json factice/include
  path invente pour cacher des diagnostics.
- collect_environment : observations filtrees selon le core, pas un dump du
  vrai env/secret ni installation/probe de machine pendant list/plan/report.
- Run : sorties/resultats/JUnit uniques sous la preuve possedee, identites depuis
  plan et receipt admis, aucune measurement fabriquee quand absente/invalide.
- Les six noms publics de contexte viennent UNIQUEMENT de CommandSpec.env_delta
  declare et des identites plan/receipt ; les valeurs ambiantes ne declarent rien.
  Reutiliser le filtrage du runner commun, pas une copie parallele de son contrat.
- Ownership du collecteur JUnit/contextes : distinct. Ne modifier ni `_junit`,
  ni parser contexte, ni fixture GL pour faciliter l'adaptateur. Consommer leur
  entree effectivement integree APRES la barriere ; pas de branche recopiee.
- Au point 107a2d3, execute refuse encore explicitement hardware CTest via
  `_unsupported_hardware_ctest` en attente de preuve structuree integree. Le
  garde GL de production integre n'efface pas cette dependance du collecteur.
  Aucun repli logiciel ou route workload de substitution pour obtenir du vert.

## Tranche TDD proposee seulement APRES point Git coordonne

1. Reutiliser les factories et fakes communs effectivement integres, pas copier
   runtime_fixtures/schema d'une ancienne branche. Garder les tests Windows
   observation/getters distincts de l'admission de l'adaptateur commun.
2. RED synthetique : preservation list/plan sans factory/process/ecriture ;
   configuration/root Windows ; selection des cibles/effets ; ordre Conan/CMake ;
   opt-in prepare seul et aucun bootstrap dans build/run.
3. RED receipt : receipt incompatible/stale, executable absent/ambigu/horsidentite,
   population de commandes/sorties/IDs invalide, contexte ambient non declare.
   Utiliser les receipts produits/verifies par le core avec fichiers factices,
   pas une simple reconstruction d'argv ou un faux schema local.
4. GREEN minimum au pont choisi ; ne reimplementer ni ownership/capture R1/R2,
   ni parser GL/JUnit, ni les politiques de preuve du core. Les tests de transport
   contextes dependront du collecteur integre, sans empiéter sur son auteur.
5. Lead runner uniquement pour RED/GREEN et non-regression sur donnees synthetiques
   et programmes factices identifies. Pas moteur/GL/benchmark/MSVC/vcvars/install,
   suite moteur ou ressource cloud. Les commandes de tests exactes attendent les
   fichiers/targets reels de ce prochain point ; aucune commande fictive livree.

## Matrice actuelle et limites

- Auteur : objets Git/interfaces/AST lus, aucune source du core importee/executee.
- Fixture observation actuelle : candidate 6a19ad7, aucun nouveau run auteur.
- Adaptateur commun, collecteur, CLI complete, Windows/Linux reels : NON_EXECUTE.
- Aucun fichier source adapte, aucune preuve de rendu/performance, aucun cloud.
- Analyse C++/clangd/Serena et compile database : non applicables a cette lecture
  d'objets Git Python ; aucun diagnostic LSP pris pour preuve d'integration.

Localisation : interfaces/base, model, planning, evidence, verify au commit pinne.
Impact ecrit : cette note seulement. Prochain acte source reserve au point Git
coordonne et a l'ownership explicite du lead, pas declenche par ce plan.
