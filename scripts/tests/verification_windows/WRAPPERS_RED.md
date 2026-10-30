# Troisieme tranche Windows — plan et RED_READY wrappers logiques

## Autorisation et preuve reutilisee

Base : `d47855057789cc62392e1ca7f74b2077ea6616a2`.
Le lead a observe GREEN dispatch 10/10 et statique 5/5, zero error/skip,
code 0, assertions/audit et empreintes stables. Logs annonces :
`out/verification-synthetic/windows-dispatch-green-2026-10-30T00-02-40-522Z/`.
Cette preuve ne qualifie ni MSVC reel ni le helper complet, toujours bloque.

Suite autorisee : preparer tests RED wrappers/entrees validees/raccordement
local ; relecture du confinement puis run lead, avant tout GREEN. Aucun test
execute par l'auteur, aucun helper complet ni vrai outil/build/MSVC/vcvars.

## Plan borne de cette sous-tranche

1. Introduire six points d'entree **refusant explicitement toute operation**
   dans une region de definitions du helper. C'est le squelette RED delibere,
   pas une panne d'import ni une copie du core. Le refus reel hors dispatch
   reste en place ; les usages historiques n'utilisent pas ces definitions.
2. Tester les entrees privees, l'encodeur Windows, snapshot/restauration et
   metadonnees MSVC au-dessus de quatre primitives de fixture. Tester le code
   enfant 17 et le quoting via un vrai **programme Python factice fini**, jamais
   Conan/CMake/CTest ou un workload. N'ajouter aucun wrapper OS reel en GREEN
   avant le RED causal lead et sa suite explicite.
3. Composer les wrappers extraits avec le dispatch existant et son **audit
   inchange**. Ici les processus Conan/CMake/workload restent des evenements,
   sans aucun lancement. Preparation/construction/execution separent leurs
   populations et l'environnement factice est restaure.
4. Apres cette tranche seulement : proposer le raccordement CLI, les primitives
   OS et l'import vcvars. La partie non exercable en mission recevra une revue
   statique explicite et des gaps NON_EXECUTE ; ne jamais inferer sa correction
   des maps. Le core non integre reste une dependance de windows.py, pas de ces
   tests. Aucun schema Request/receipt/baseProtocol n'est copie.

## Fichiers possedes et changement de production

- `scripts/build-windows.ps1` : region WRAPPERS contenant uniquement six
  fonctions a throw provisoire ; aucun corps operationnel nouveau.
- `test_wrappers.py`, `wrapper_fixture.ps1`, `wrapper_child.py`, cette note.
- Les 10 tests dispatch, 5 tests statiques et dispatch_fixture.ps1 sont intacts.
  Aucun audit existant n'est retire/elargi. Ne pas reutiliser une discovery
  globale avant relecture de la nouvelle fixture.

## Entrees privees, distinctes du modele core

Resolve-VerificationInputs(Values, Root, Phase, Configuration) traite une map
interne de **quatre champs exactement** : build_directory, targets, executable,
arguments. Il n'accepte pas une Request/receipt et ne recoit aucun module Python.

- build_directory : arbre normalise exact Root/out/build/windows-msvc-CONFIG-verification,
  pas l'arbre historique, une autre configuration ou un chemin externe ; pas
  besoin que cet arbre existe pour valider les entrees.
- targets : tableau non vide sans doublon de cibles fermees ; CPU et OpenGL workloads et
  ArtistLib_component_tests sont les noms du contrat. Aucun nom utilisateur libre.
- Pour run, executable appartient au meme arbre et correspond a un workload
  connu/selectionne ; pour prepare/build, executable vide et arguments vides.
  CTest/component selections n'ont pas encore leur protocole de transport ici.
- arguments : tableau de chaines uniquement, sans NUL ; au plus 128 elements,
  4096 caracteres par element et 16384 au total (reserve pour le prefixe Windows).
  Ce sont bornes de transport prive, pas une nouvelle grille de charge Request.
- Erreur de validation : throw prefixe `InvalidVerificationInput:`, avant effet.

L'encodeur prive entoure **toujours** chaque argument de doubles quotes et
double les antislashs devant quote/finale selon le protocole Windows CRT.
Ce format deterministe permet au harnais de verifier le prefixe Python exact,
pas seulement une chaine shell supposee equivalente.

Le format CLI externe (base64 ou fichier borne), les doublons JSON au decode,
les symlinks/reparse points et les receipts restent a raccorder/qualifier dans
la tranche suivante. Ne pas annoncer cette validation interne comme validation
complete d'un bundle ou d'une entree CLI publique. Aucun nouveau flag de payload
n'est declare tant que cette frontiere n'est pas prete.

## Confinement soumis au lead

- Python lance uniquement wrapper_fixture.ps1 par PowerShell connu, shell=False.
  Le helper complet n'est jamais importe/dot-source/invoque. Seules les six
  definitions marquees sont transmises, maximum 32 KiB/2048 noeuds par fonction.
- Audit nouveau et **specifique a chaque fonction**, pas extension de l'ancien :
  appels uniquement vers les primitives declarees, pas &/dot-source dynamique,
  provider env, variable scopee, type natif, module, redirection ou code imbrique.
  Seule methode autorisee : Path.GetFullPath, pure ; seules autres proprietes :
  Keys/Count/Length sur donnees. Types limites aux donnees et a System.IO.Path.
- Read/Write-VerificationEnvironmentLeaf manipulent une hashtable de fixture,
  vide et absence distinctes ; Read-Msvc retourne une map synthetique. Elles ne
  lisent/mutent **jamais** le vrai environnement ni n'appellent cmd/vcvars.
- Invoke-VerificationProcessLeaf est record-only pour composition. Pour child/
  quote/selfcheck, un seul lancement possible : sys.executable du runner Python,
  empreinte verifiee, avec prefixe fixe `-I -S -B wrapper_child.py result code`.
  Fichier enfant exact voisin versionne, empreinte verifiee, resultat dans le
  dossier de fixture exact. Tout autre programme/prefixe est refuse. Aucun shell.
- L'enfant n'importe que stdlib, ecrit exclusivement son resultat avec open(x),
  enregistre ses argv puis sort avec le code fixe du prefixe. Aucun argv n'est
  interprete comme code, chemin de commande, import ou autre executable.
- Fichiers d'entree/sortie et racine de preuve verifies ; journaux processus et
  enfant conserves sous out/verification-synthetic/windows-wrapper-fixtures.
  Le dossier unique est cree par le test, pas reutilise depuis une preuve ancienne.
  Le faux source root avec espaces est une donnee de validation uniquement :
  aucun arbre de build/repertoire source factice n'est cree ni touche.
- L'audit dispatch original est extrait par AST de sa fonction unique, sans
  evaluation du fichier complet, et reapplique inchange pour compose.
- Autotest positif : vrai enfant factice code 17, argument fixe. Contre-exemples
  d'audit independants de tout stub de production, rejet avant evaluation.
  Les appels interdits ciblent uniquement une sentinelle de test qui throw,
  jamais un vrai nom d'outil ; les autres mutants sont donnees/constructeurs
  purs et inoffensifs meme si l'audit etait accidentellement casse.
  Une panne de ces autotests invalide la preuve RED ; ce n'est pas le comportement
  a corriger pour rendre arbitrairement les tests verts.

Cette frontiere est pour sources/fichiers relus, **pas une sandbox generale**.
Le parent Python et les primitives de fixture sont aussi a relire par le lead.
Un programme absent, erreur AST, timeout, identite divergente ou fichier manquant
est une panne de harnais/precondition, jamais le RED causal attendu.

## Commandes exactes proposees, pas executees par l'auteur

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_wrappers.py' -v
```

Apres revue/autorisation lead seulement, via Context Mode. Le chemin du
sous-processus PS pointe exclusivement vers wrapper_fixture.ps1 ; son unique
enfant eventuel est `<sys.executable> -I -S -B wrapper_child.py <owned-result> 17|0 <data>`.
Les programmes nommes Conan/CMake du test compose ne sont jamais lances.

Non-regression separable, fichiers inchanges :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_dispatch.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_helper_contract.py' -v
```

## Attendu RED par lecture et matrice

11 methodes : 9 methodes rouges (28 failures avec sous-cas), 2 vertes
(barriere reelle statique et autotests de fixture), aucun error/skip sous Windows.
Premier echec fonctionnel attendu : test_child_wrapper_preserves_code_17_and_all_arguments,
code 1 / `Verification child wrapper is not implemented yet`, au lieu de 17.
Les autres refus provisoires, et non une dependance core/interpreteur, doivent
expliquer les failures. Une erreur de harnais invalide cet attendu.

- Syntaxes Python/PowerShell analysees sans erreur (ast.parse/Parser.ParseFile),
  sans importer/evaluer tests, fixture, enfant ou helper ; Git/lectures seulement.
- Nouvelle fixture et enfant factice : NON_EXECUTE par l'auteur ; RED a observer lead.
- Anciennes preuves : GREEN lead 10+5 sur la base seulement, pas ce nouveau candidat.
- Environnement OS reel, import MSVC/SDK, cmd/vcvars : NON_EXECUTE/non raccordes.
- Installer/telecharger/bootstrap, outils de build, suites moteur/GL : NON_EXECUTE.
- Linux, image/performance/cloud : NON_EXECUTE ; aucune ressource Docker/cloud creee.

## Preuves encore manquantes, non bloquantes pour cette remise RED

Les primitives OS ne sont pas implementees par ces tests. La distinction
absent/valeur vide est prouvee seulement pour leur contrat de map si GREEN futur ;
les limites de SetEnvironmentVariable sous Windows/.NET restent a verifier,
pas a masquer. Lecture vcvars, stdout/code partiels, isolation tooling/Conan,
transport CLI et raccordement final du helper complet restent GAP explicites.
Ce raccordement devra aussi refuser flags de verification sans phase et melanges
avec ConfigureOnly/ASan/preflight, sans changer l'appel historique sans ces flags.
La preparation component/CTest et la qualification reelle restent hors preuve.
Cette remise n'annonce ni pipeline Windows termine ni fonctionnement MSVC.
