# Tranche ciblee Windows — GREEN_READY apres RED lead

## Base et attribution

Base : `d048c7de6112acf45f7cd47f655456146c1d1ceb`.
RED observe lead : 7 methodes, 5 rouges / 8 failures avec sous-cas, 2 vertes,
zero error/skip, empreintes stables. Snapshot Keys/Count et deux arguments de
4096 scalaires astraux verts, aller-retour enfant Python inclus.
Logs : `out/verification-synthetic/windows-targeted-red-2026-10-30T01-40-04-240Z/`.

Causes rouges confirmees : refus de restauration Keys, ArtistLib_tests absent
de l'allowlist, BUILD_TESTING OFF pour la cible component seule.

## Correction bornee autorisee

- Set-VerificationProcessEnvironment lit les vraies cles par PSBase.Keys sur
  les deux maps validees ; aucun nom d'environnement Keys/Count exclu.
- Allowlist quatre noms reels : ArtistUniformCpuWorkload,
  ArtistUniformOpenGLWorkload, ArtistLib_tests, ArtistLib_component_tests.
- Prepare : BUILD_TESTING=ON si une des deux cibles de tests est selectionnee,
  OFF sinon. Les anciennes assertions CPU OFF restent inchangees.
- Pour les configurations tests, DISCOVERY_MODE=PRE_TEST explicitement via
  CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE ; aucune decouverte executable lors
  du futur build. Ceci reste une preuve de commande sous double, pas CTest reel.

Pas de changement des 26 tests anterieurs ni des sept assertions ciblees.
La fixture est inchangee hors bloc d'audit exact approuve ; audit dispatch,
programmes enfants, protocoles, chemins et gardes d'identite restent intacts.
Le diff d'audit et ses gardes sont remis separement (AUDIT_PSBASE_REVIEW.md).
Commit d'audit isole : `de08042b6e644a7a2084c2bb49ff363c226fcb49`.
Lecture exacte du diff, sans execution :
`git show de08042b6e644a7a2084c2bb49ff363c226fcb49 -- scripts/tests/verification_windows/wrapper_fixture.ps1`.

## Commandes proposees au runner lead via Context Mode

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

Relire d'abord le diff d'audit et ses contre-exemples, puis :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_psbase_audit.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_targeted_contracts.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_wrappers.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_dispatch.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_helper_contract.py' -v
```

Attendus de lecture : 4 + 7 + 11 + 10 + 5 = 37 methodes vertes, sans
failure/error/skip. Aucun run par l'auteur ; les preuves GREEN et non-regression
doivent encore etre observees par le lead sur les empreintes finales.

## Matrice et limites

- RED cible : observe lead sur la base, 8 failures attendues.
- Candidat GREEN et gardes nouvelles : NON_EXECUTE par l'auteur.
- Syntaxes et diffs : analyses statiques seulement, sans evaluation de fonctions.
- Maps environnement : toujours fakes ; aucune preuve MSVC/Win32 reelle inferée.
- Helper complet : refus avant dispatch conserve, OS leaves/CLI/receipts non raccordes.
- Adaptateur CTest runtime : non implemente, aucun fonctionnement global affirme.
- Core runtime : toujours en attente d'integration, aucun schema copie.
- Moteur CPU/GL, CMake/Conan/CTest, installation, image/performance, Linux : NON_EXECUTE.
- Aucun Docker/cloud ni ressource reservee.

La borne scalaire et la caracterisation astrale ne sont pas modifiees par cette
correction. La future frontiere native doit encore borner toute la ligne apres
quoting en unites UTF-16, argv0 et NUL compris, sans limite en octets UTF-8.
Cette remise n'est pas le raccordement final du pipeline Windows ni une preuve
de campagne GPU ; les deux cibles CTest sont constructibles dans le plan, pas
executees par cette mission.
