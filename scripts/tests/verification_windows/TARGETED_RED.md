# Tranche ciblee Windows — RED_READY Keys et cibles catalogue

## Base et preuve reutilisee

Base : `9377eff5ea413f749656d0ce22127e15710a417c`.
Le lead a observe GREEN 11/11 + 10/10 + 5/5, zero error/skip, code 0,
assertions/audits et empreintes stables. Logs annonces :
`out/verification-synthetic/windows-wrappers-green-2026-10-30T01-18-54-446Z/`.
La reserve Keys a ete confirmee par lecture, pas qualifiee runtime.

Mandat suivant : tests RED ciblant Keys/Count et les cibles requises par le
catalogue integre gl.float-preparation/lifetime, avant correction GREEN.
L'auteur ne lance aucun test ; le helper complet et les primitives OS restent
interdits/non raccordes. Le core runtime n'est pas encore integre ici.

## Premiere tranche de tests et fichiers

- Nouveau `test_targeted_contracts.py`, sept methodes seulement.
- Reutilisation par composition de la factory du module test_wrappers, sans
  sous-classer sa TestCase et sans exposer cette classe a la discovery.
- Meme wrapper_fixture.ps1, meme enfant Python fini, meme audit dispatch ;
  **aucune modification des 26 tests existants, des audits, du helper ou des
  schemas/payloads** dans cette remise.
- Les sorties brutes restent sous windows-wrapper-fixtures, sans nouvelle
  famille de harnais ni outil. Les scopes/processus permis sont inchanges.

## Oracles nominaux et negatifs

1. Snapshot map contenant Keys='' et Count='texte' : copie detachee, aucune
   mutation du map courant ; attendu deja vert avec la copie par addition.
2. Restauration : ces noms sont valides, non interdits. Valeurs vides, suppression
   des absences et remplacement exact, avec Keys dans le courant et/ou desire.
   Attendu rouge sur le refus UnsupportedVerificationEnvironment actuel.
3. Resolve doit accepter les **quatre vrais noms** : ArtistUniformCpuWorkload,
   ArtistUniformOpenGLWorkload, ArtistLib_tests, ArtistLib_component_tests.
   Pas de nouvelle route/backend ou nom libre. Attendu rouge sur ArtistLib_tests.
4. Prepare puis build de la paire ArtistLib_tests + ArtistLib_component_tests :
   commandes enregistrees uniquement, Conan avant CMake, construction exacte
   des deux cibles ; ni campagne implicite ni outils reellement lances.
5. Prepare de chaque cible de tests doit activer BUILD_TESTING=ON. Le cas
   component seul isole ce defaut de configuration : cible deja admise, mais
   commande actuelle OFF. Les anciens tests CPU gardent leur assertion OFF.
6. Lorsque les tests sont construits, demander explicitement
   CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST pour ne pas lancer la
   decouverte executable GoogleTest pendant build. Ceci est un oracle de
   commande, pas une preuve de decouverte CTest reelle.
7. Caracterisation nominale : deux arguments distincts de 4096 scalaires
   astraux, soit chacun 8192 unites UTF-16. Resolve les accepte puis le seul
   enfant Python factice les restitue exactement. Aucun RED n'est attendu ou
   fabrique pour cette dimension deja implementee. Verifier le modele de ligne
   native complete, argv0 et NUL compris, <=32767 unites UTF-16, pas octets.

Lecture statique des vrais CMake de ce worktree : ARTIST/CMakeLists.txt declare
project(ArtistLib) et ajoute tests/component-tests sous BUILD_TESTING.
ARTIST/tests/CMakeLists.txt cree ${PROJECT_NAME}_tests ; component-tests cree
${PROJECT_NAME}_component_tests. Les deux gtest_discover_tests omettent un
DISCOVERY_MODE explicite, d'ou la variable PRE_TEST proposee. Le besoin catalogue
integre des deux cibles est fourni par le lead ; aucun catalogue/core copie.

## Proposition AST a relire apres RED, pas encore appliquee

Le RED Keys doit d'abord etre observe sur l'implementation et l'audit actuels.
Aucune extension AST n'est necessaire pour executer ces nouveaux tests RED.

Pour GREEN ulterieur, proposition etroite a approuver par le lead :

- uniquement dans Set-VerificationProcessEnvironment ;
- uniquement acces non invoque, en lecture, exactement `$current.PSBase.Keys`
  et `$Values.PSBase.Keys` ;
- Values reste parametre [hashtable], current garde sa validation Hashtable
  issue de Read-VerificationEnvironmentLeaf ;
- autoriser le noeud interne PSBase **seulement** s'il est le recepteur de l'un
  de ces deux acces complets approuves, jamais comme propriete generique ;
- aucune methode, nouveau type, nouvelle commande, variable quelconque,
  outil/leaf ou reflexion generalisee ; tout le reste de l'audit reste identique.

Cas d'enforcement a proposer avec cette extension apres son approbation :
refuser metadata.PSBase.Keys, Values.PSBase.Values, Values.PSBase.GetType() et
PSBase isole. Il s'agit d'une revue de la frontiere de donnees relues, pas d'une
extension silencieuse d'une pseudo-sandbox. L'auteur ne l'applique pas ici.

## Commande exacte proposee au runner lead, via Context Mode

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_targeted_contracts.py' -v
```

Ne pas lancer de discovery globale avant revue. L'import de test_wrappers
n'execute aucun test et n'expose pas sa TestCase au loader du nouveau module.
La nouvelle classe ne reprend que setUpClass et run_fixture, pas ses methodes
test_*. Sous-processus : interpreteur PS + fixture deja revue ; unique enfant
reel eventuel = Python -I -S -B wrapper_child.py, jamais un outil/moteur.

## Attendu de lecture (NON_EXECUTE)

Sept methodes, cinq methodes rouges / huit failures avec sous-cas, deux vertes,
zero error/skip sur Windows equipe. Premier echec attendu : test_02_restore_...
(3 sous-cas), refus Keys avant mutation. Autres causes : allowlist sans
ArtistLib_tests puis BUILD_TESTING=OFF dans le cas component seul.
Le quatrieme sous-cas de restauration, Count seul sans Keys, est nominal.

Snapshot et grande Unicode sont nominaux : s'ils echouent, examiner leur cause
separement ; ne pas les rebaptiser RED attendu ou reduire les scalaires pour
obtenir un succes. Une panne d'import/fixture/audit/identite/timeout n'est pas
la preuve RED. Les 26 tests anterieurs ne sont pas reexecutes par l'auteur.

## Matrice et suite bornee

- Auteur : lectures CMake, Git et syntaxe Python statiques seulement.
- Nouvelle tranche / enfant factice / non-regression : NON_EXECUTE par l'auteur.
- Helper complet, OS leaves, MSVC/cmd/vcvars, build/install/CTest : NON_EXECUTE.
- Moteur CPU/GL, image/performance, Linux : NON_EXECUTE ; aucun Docker/cloud.

Apres observation du RED et suite explicite lead seulement : correction de
l'acces de cles avec audit etroit relu, ajout ArtistLib_tests a l'allowlist,
BUILD_TESTING ON si la selection contient une cible de tests (sinon OFF) et
PRE_TEST pour leur decouverte. Pas de CMake parent, nouveau schema ou protocole
core ; source helper bloque jusqu'au futur contrat de confinement OS/CLI.
