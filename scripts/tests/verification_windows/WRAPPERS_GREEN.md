# Troisieme tranche Windows — GREEN_READY logique, reserve AST explicite

## RED attribue et autorisation

Base : `b484f026b2a715c868b1b29b7a80fde23371a56e`.
Le lead a observe 11 methodes, 9 rouges / 28 failures avec sous-cas, 2 vertes,
zero error/skip, code 1. L'enfant Python fini code 17 et les rejets d'effets
etaient verts ; seules les six fonctions a refus provisoire causaient le RED.
Empreintes annoncees stables.

Logs lead :
`out/verification-synthetic/windows-wrappers-red-2026-10-30T00-44-11-968Z/`.
Fixtures annoncees dans ce worktree :
`out/verification-synthetic/windows-wrapper-fixtures/20261030T004402Z-9f6660b3c8fd498b8a42c7115dd4826a/`.

GREEN autorise : six wrappers logiques et composition, sans primitives OS
reelles ni helper complet ; ne changer ni les 11 tests ni leurs audits. Cette
remise conserve ces fichiers intacts et ne lance aucun test.

## Correction bornee

- Resolve : quatre champs internes fermes, tableaux et cibles connus non vides
  sans doublon, repertoire exact de configuration, executable d'un workload
  selectionne dans cet arbre, pas de commande run cachee en prepare/build.
- Arguments : chaines sans NUL ni surrogate isole, 128 elements, 4096 **scalaires
  Unicode** par element et 16384 scalaires au total. Comptage explicite des paires
  UTF-16 ; aucune limite de 4096 unites UTF-16 ni limite en octets UTF-8.
- Codec : doubles quotes deterministes, antislashs doubles devant quote/finale,
  espaces/quotes/antislashs transmis comme donnees Windows CRT, jamais shell.
- Snapshot : copie de Hashtable via addition, detachee du map partage du leaf.
- Restauration : suppression des absences puis restitution des valeurs, vide
  conserve par la primitive factice ; enumeration sur liste de cles capturee.
- MSVC : map synthetique validee toolset 14.44 / cible x64 / format SDK Windows,
  copie retournee ; ce n'est PAS une sonde MSVC/STL ou compile/link C++23.
- Enfant : encodage de chaque argument puis primitive injectee, retour entier
  brut obligatoire ; aucune conversion d'un resultat absent/malforme en code 0.

Le refus du helper complet avant dispatch est inchange. Les quatre primitives
ne sont pas raccordees a l'OS dans ce candidat. Le dispatch deja qualifie et
le mode historique du helper n'ont pas ete modifies.

## Reserve AST a apprecier par le lead avant primitives OS

Constat de lecture / hypothese a caracteriser par un test lead : les cles d'une
Hashtable PowerShell peuvent masquer ses membres adaptes, notamment une
variable d'environnement valide nommee `Keys` face a l'accesseur `.Keys`.
L'audit ferme interdit les accesseurs bruts alternatifs (`PSBase` / enumerateur).
Il n'a pas ete elargi pour contourner ce point.

Mesure reversible : Set-VerificationProcessEnvironment refuse un `Keys` non
null dans le map courant ou desire **avant toute mutation**, au lieu de perdre
silencieusement une entree. Les snapshots et maps metadata utilisent une copie
par addition, sans cet accesseur. Les champs internes Values sont fermes et
refusent aussi les noms inconnus qui masqueraient Keys/Count.

Cette limite ne doit PAS etre presentee comme une restauration exhaustive de
tous les environnements Windows : c'est une **reserve de completion**, pas un
succes implicite ni une raison de modifier les assertions en GREEN.
Proposition au lead : cas synthetique `Keys=''` et `Count='valeur'`, puis choix
d'un acces pur borne aux cles et revue ciblee du nouvel audit si necessaire.
Ne pas activer les primitives OS ou supprimer le refus du helper complet tant
que cette reserve n'est pas resolue. Aucun test de cette hypothese par l'auteur.

## Unicode et vraie frontiere native, non raccordee

Deux arguments de 4096 caracteres astraux comptent chacun 4096 scalaires
(8192 unites UTF-16) et ne sont pas rejetes pour cette seule longueur par Resolve.
Ils representent 8192 scalaires au total ; ce comportement est voulu par le code,
pas encore une preuve executee supplementaire aux 11 tests figes.

La future primitive native doit borner la **ligne complete apres quoting**,
argv0 et NUL compris, selon la limite CreateProcessW de 32767 unites UTF-16.
Ne pas appliquer cette limite aux octets UTF-8 ni avant l'expansion des quotes
et antislashs. Les tests figes ne prouvent pas cette frontiere OS.
Un futur transport CLI en base64 d'UTF-8 peut depasser la ligne native alors
que les argv finaux resteraient admissibles : ne pas le choisir aveuglement
pour ces grandes valeurs. Le transport par fichier borne/identifie et son
raccordement au core restent a proposer ; aucun schema core n'est copie ici.

## Commandes proposees au runner lead via Context Mode

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_wrappers.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_dispatch.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_helper_contract.py' -v
```

Attendus par lecture : 11/11, puis 10/10 et 5/5 ; zero failure/error/skip.
GREEN/non-regression doivent etre observes par le lead, pas presumes sur ce
document. Une incompatibilite AST reste une panne de harnais/interface a
apprecier, pas une permission d'elargissement automatique.

## Matrice et limites

- RED wrappers : observe lead sur la base ci-dessus.
- Nouveau GREEN / non-regression : NON_EXECUTE par l'auteur.
- Analyses de syntaxe seulement par l'auteur, sans evaluation des fonctions.
- Env OS reel / MSVC / cmd / vcvars / transport CLI / receipts : GAP, NON_EXECUTE.
- CMake/Conan/CTest, installation, moteur CPU/GL, image/performance : NON_EXECUTE.
- Linux : hors ownership, NON_EXECUTE ; aucun Docker/cloud ni ressource reservee.

L'adaptateur Python attend toujours le core integre. Les scenarios cc26104
SOURCE_ONLY et leurs 31 tests Python annonces par le lead ne qualifient pas
les primitives Windows de ce candidat. Cette remise reste intermediaire et
n'est pas une livraison complete de verification Windows utilisable.
