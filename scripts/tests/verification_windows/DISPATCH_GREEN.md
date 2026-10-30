# Deuxieme tranche Windows — GREEN_READY dispatch seulement

## Base et preuve RED attribuee

Base : `b96df6530e486c6057e8ad7e7414a4a4946110a9`.
Confinement relu puis RED observe par le lead : 10 methodes, 6 methodes rouges,
11 failures avec sous-cas, 4 vertes, 0 error/skip, code 1. Autotests positifs et
enforcement etroit verts ; aucune panne AST/timeout ; empreintes stables.
Cause : refus provisoire des phases, pas manque de core/interpreteur.

Logs annonces par le lead :
`out/verification-synthetic/windows-dispatch-red-2026-10-29T23-56-06-752Z/`.
Fixtures brutes annoncees dans ce worktree :
`out/verification-synthetic/windows-dispatch-fixtures/20261029T235600Z-79d70755a9ed4d5787cd283698a8a41b/`.
L'auteur reutilise cette preuve attribuee, il n'a pas execute les tests.

## Correction bornee autorisee

Region entre les deux marqueurs dans `scripts/build-windows.ps1` uniquement :

- prepare sans opt-in refuse avant snapshot/setup/enfant ;
- prepare autorise : commandes **donnees aux doubles**, Conan install puis CMake
  configure, host/build de meme configuration, arbre/toolchain explicites ;
- build : cible(s) declaree(s) seulement, sans pip/Conan/configure/test ;
- run : executable et argv transmis directement a la frontiere, sans shell ;
- code du premier enfant non nul retourne tel quel, aucun enfant suivant ;
- snapshot puis try/finally dans le dispatch, restauration exacte par la frontiere ;
- phases terminales, aucun retour vers la campagne historique.

Aucun test, autotest, audit AST ni interface des doubles n'a ete modifie.
L'audit doit encore approuver ce nouveau texte a l'execution lead : un echec
de cet audit serait une panne de harnais/interface, pas un GREEN.

Le refus preexistant du helper reel est conserve **hors** region extraite,
avant toute frontiere/commande. Cette barriere est necessaire tant que les
wrappers reels et leurs entrees validees ne sont pas livres. Les nouveaux
parametres prives de dispatch sont injectes uniquement par la fixture ; aucun
raccordement implicite au helper ni recherche PATH d'une frontiere absente.
Omission de VerificationPhase : chemin historique inchange.

Cette tranche reste donc **INTERMEDIAIRE, non integrable comme fonctionnalite
Windows complete**. Elle ne pretend pas rendre prepare/build/run utilisables
sur une vraie machine et ne supprime pas la barriere avant la tranche suivante.

## Bornes de couverture

La preparation presente cible la premiere selection CPU, avec BUILD_TESTING=OFF.
La selection future de tests composants/lifetime, les receipts, les chemins
valides du vrai helper, le protocole d'argv externe PowerShell et les autres
combinaisons exigent leurs propres tests avant raccordement. Aucun schema core
n'est copie. Les frontieres actuelles ne sont que celles fournies par la fixture.

Get-VerificationMsvcEnvironment et Set-VerificationProcessEnvironment sont
des maps factices : MSVC reel, import vcvars, variables de processus et codes
d'un vrai enfant ne sont pas demontres. Invoke-Checked historique n'est pas
modifie ; la preservation du code a sa frontiere reelle reste a traiter dans
la tranche wrappers, avec revue/confinement et RED propres.

## Commandes proposees au runner lead via Context Mode

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

GREEN cible :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_dispatch.py' -v
```

Attendu par lecture : 10/10 methodes, zero failure/error/skip, code 0. Les
resultats/logs bruts des cas sont conserves par la fixture comme en RED.

Non-regression statique (helper non execute) :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_helper_contract.py' -v
```

Attendu par lecture : 5/5, zero failure/error/skip. L'auteur ne lance aucune de
ces commandes ; GREEN doit etre observe et attribue au lead apres revue.

## Matrice et suite

- RED synthetique Windows : observe lead sur la base, comptages ci-dessus.
- Syntaxe du helper : Parser.ParseFile sans evaluation, aucune erreur syntaxique.
- GREEN de ce candidat et non-regression : NON_EXECUTE par l'auteur.
- Verification reelle helper/MSVC/process/env : NON_EXECUTE et non raccordee.
- CMake/Conan/CTest, installation, moteur CPU/GL, images, performances : NON_EXECUTE.
- Linux : hors ownership, NON_EXECUTE. Aucun Docker/cloud ni ressource reservee.

Suite proposee au lead, pas encore codee ni executee : concevoir les tests des
wrappers reels sur extraits confines (processus enfants uniquement programmes
factices, env de fixture), puis le transport d'argv avec espaces et refus des
chemins/configurations incompatibles. Le helper reel entier demeure interdit
dans les tests de mission. La suppression de sa barriere doit attendre cette
qualification et le raccordement valide, pas le seul GREEN des maps.
L'adaptateur Python attend toujours un core effectivement integre.
