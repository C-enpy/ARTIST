# Remise Windows — premiere tranche GREEN_READY

Suite : GREEN statique 5/5 observe par le lead sur `01e970b` ; la deuxieme
tranche de tests confines est decrite dans `DISPATCH_RED.md`, puis correction
de dispatch seule dans `DISPATCH_GREEN.md` apres RED lead sur `b96df65`.
Aucun GREEN reel MSVC/process/env ni qualification de workload n'est revendique.
Troisieme sous-tranche RED proposee : `WRAPPERS_RED.md` ; nouvelle fixture a
relire avant execution. Les commandes precedentes restent filtrees par fichier.
Apres RED lead sur `b484f02`, correction logique proposee : `WRAPPERS_GREEN.md`,
avec reserve AST explicite pour une variable d'environnement nommee `Keys`.
Apres GREEN lead sur `9377eff`, nouvelle tranche `TARGETED_RED.md` : Keys/Count,
cibles unit+component et caracterisation Unicode, sans changement des audits.
Apres RED lead sur `d048c7d`, `TARGETED_GREEN.md` propose la correction ;
`AUDIT_PSBASE_REVIEW.md` isole l'exception d'audit explicitement approuvee.
Apres GREEN lead 37/37 sur `769a55a`, `NATIVE_RED.md` consolide OS/transport :
nouvelle admission de source et mode enfant a relire AVANT tout lancement.
Admission courante : `HARNESS_FIX_READY.md`, attentes finies et 14 methodes
selectionnees explicitement ; le cas reparse 06 reste NON_EXECUTE.

## Entrees et limites

- Base : `1fba1c98e3a9750f615deacf37a08eb99d4b4fa6` ; branche
  `ensemble-7e4f4a4a-c728-48b2-9578-2566a530b66c`.
- Brief, contrat v1 et plan QA presents et lus dans ce worktree :
  `.opencode/missions/team_mvu0gbxm_0000_cwsmsw/author-briefs.md`,
  `.opencode/evidence/mission-uniform-pipeline/{contract,qa-plan}.md`.
- Ownership : `scripts/verification/adapters/windows.py`,
  `scripts/build-windows.ps1` si necessaire, ce dossier de tests seulement.
- Premiere remise tests : `c32dc75f26fe8f6014d418b50eef3c3c5080a557`.
  Le lead a observe le RED causal (5 tests, 2 echecs attendus, 3 succes,
  code 1, aucune erreur, sources inchangees). Preuves annoncees par le lead :
  `out/verification-synthetic/windows-helper-static-red-2026-10-29T23-07-07-159Z/`.
- Suite autorisee : declarations de configuration compatibles seulement.
  Cette remise ajoute les deux parametres, sans modifier les assertions.
  Toute VerificationPhase explicitement demandee est refusee avant effets,
  pour ne jamais retomber sur le fullmode historique pendant cette tranche.
  Sans VerificationPhase, le chemin historique est inchange. Le nouveau switch
  de consentement ne donne a lui seul aucune garantie de garde reseau effective.
- GREEN_READY = candidate pour revue et run lead, **pas GREEN observe**.
  Aucun test ni helper execute par l'auteur. La logique operationnelle et
  l'adaptateur attendent leurs tests proposes et l'autorisation du lead.
- Aucun import core, copie de schema, shell, subprocess, reseau ou ecriture
  dans les tests. Ils ne lisent que le helper du worktree depuis `__file__`.

## Plan borne

1. Tranche presente : contrat textuel des nouveaux modes explicites du helper,
   plus preservation statique des modes historiques, de l'ordre Conan/CMake et
   du scope MSVC/restauration. Aucun appel PowerShell, meme sous simulation.
2. Tranche presente apres RED lead : declaration minimale compatible du helper.
   Interface privee : `-VerificationPhase` avec
   `prepare|build|run` (sans valeur implicite ; omission = ancien comportement)
   et `-AllowDependencyResolution` switch faux par defaut. Les trois phases
   restent refusees immediatement tant que leur logique n'est pas autorisee.
   Cible de la tranche operationnelle ulterieure : seul le chemin verification
   prepare exige ce consentement ; usages historiques inchanges. En phase
   build/run : aucun pip/Conan/configure, aucun test implicite ; enfants sous
   environnement MSVC puis restauration.
   Les nouveaux chemins build doivent rester distincts des arbres historiques
   selon toolchain/configuration/plateforme et options de qualification.
3. Apres disponibilite effective du core et avant GREEN de l'adaptateur :
   tests dedies avec doubles structurels, commandes non executees et runner
   sentinelle. Tester describe_* purs, chemins espaces et argv non shell,
   separation phases, refus logiciel Windows, CPU sans GL, receipts incompatibles,
   cibles exactes et absence de bootstrap. Ne pas copier schemas ni core.
4. Livrer windows.py par l'interface normative PlatformAdapter ; le runner core
   conserve codes natifs, timeouts, interruptions et logs bruts. Pour run,
   l'environnement doit etre initialise dans le meme processus PowerShell que
   l'enfant (jamais ConfigureOnly puis environnement presume dans Python parent).
   Les details prives de transport des argv enfant seront remis au lead avant
   leurs tests ; aucune expression PowerShell libre ni interpolation utilisateur.
5. Runner lead : GREEN puis non-regression synthetique apres relecture ; revue
   statique helper/MSVC et raccordement apres integration effective des producteurs.
   Aucune qualification reelle dans cette mission.

## Commande GREEN/non-regression proposee au runner lead

Repertoire de travail :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`.

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_helper_contract.py' -v
```

Executer seulement via Context Mode apres relecture des tests et verification
de l'interpreteur. Conserver commande, interpreteur/version, HEAD/delta, logs et
comptages sous `out/` par le runner lead. Aucun helper n'est appele par ce test.
Filtre desormais explicite pour ne pas inclure les nouveaux tests de dispatch
avant leur revue de confinement. La commande de tranche 2 est dans DISPATCH_RED.md.

Attendu GREEN par lecture, **NON_EXECUTE par l'auteur** : cinq methodes
decouvertes, cinq succes statiques, zero failure/error/skip.
Le RED lead a confirme les deux absences attendues ; premier echec :
`test_dependency_resolution_requires_an_opt_in_switch`, message
`Missing explicit $AllowDependencyResolution parameter ...`.
Une absence d'interpreteur, zero test ou import casse n'est pas le RED attendu.

Ces assertions sont volontairement une preuve de contrat de configuration
textuel. Un switch present n'etablit ni garde reseau effective, ni separation
runtime, ni preservation des codes/env : ne pas qualifier ces proprietes PASS
sur cette seule tranche. Les assertions de structure historique ne sont pas
une preuve comportementale PowerShell.

## Proposition de controles avant logique operationnelle (a trancher par lead)

Ne pas executer automatiquement : cette section n'autorise aucun nouveau test.
Les futures doubles ne devront jamais executer le helper original, meme avec
outils factices places simplement au debut du PATH (un oubli pourrait appeler
un outil reel).

- Premier controle additionnel statique propose : rejet de VerificationPhase
  explicitement fourni avant le dot-source asan-env et tout Push-Location,
  commande, ecriture ou initialisation environnement ; absence de valeur
  implicite et absence de chemin menant au legacy fullmode. Ce controle
  caracteriserait le verrou temporaire, sans pretendre prouver son runtime.
- Pour prepare/build/run, proposer ensuite un extrait de dispatch limite,
  executable seulement par un harnais de fixture qui remplace **toutes** les
  frontieres d'effets par fonctions sentinelles levant sur appel imprevu.
  Le harnais et le code exact extrait seront presentes au lead pour revue
  avant execution ; aucune invocation du fichier build-windows.ps1 lui-meme.
  Si cette extraction ne permet pas un confinement probant, rester aux tests
  Python de commandes sous doubles et a la revue statique du helper, sans
  revendiquer de preuve runtime MSVC/restauration.
- Cas discriminants a proposer : prepare sans opt-in refuse avant tout enfant ;
  prepare autorise produit Conan avant CMake et jamais build/test ; build refuse
  sans receipt et ne produit que les cibles prevues, sans pip/Conan/configure/test ;
  run ne produit que l'executable/selection declares, jamais prepare/build ;
  code enfant 17 conserve 17 et interdit l'enfant suivant ; restauration de
  PATH/INCLUDE/LIB, variables initialement absentes comprises, sur succes/echec ;
  chemin avec espaces transmis comme un argv, aucune expression shell libre.
- MSVC reel et restitution de code au niveau du processus PowerShell resteront
  NON_EXECUTE si le harnais autorise ne peut pas les demontrer. Invoke-Checked
  historique transforme les codes en throw : la correction future devra avoir
  un test discriminant sans changer silencieusement les callers historiques.
- L'adaptateur Python attend toujours le core effectivement integre : doubles
  structurels et TYPE_CHECKING possibles, aucun modele/schema copie.

## Premiere cause et dependances

Lecture du helper de base : pip est inconditionnel (ligne 64), Conan install
et configuration sont inconditionnels (107-115), ConfigureOnly retourne ensuite,
puis build complet et CTest implicites (124-127). Il n'existe ni mode build seul,
ni mode run seul, ni consentement explicite de resolution pour verification.
Les versions configurees restent MSVC 14.44/x64, Conan 2.12.2, Ninja 1.13.0,
profil `compiler=msvc/194`, C++23, runtime dynamic. Versions installees/ABI/STL
effectives **non sondees**, compilation std::format/stacktrace NON_EXECUTE.
Les presets, CMake racine, conanfile et CI ont ete inspectes sans execution.
Conan conserve CMakeDeps/CMakeToolchain et doit preceder CMake en preparation.

`scripts/verification/` est absent de la base : adapter/base.py, model.py et le
runner sont une dependance interbranche reelle. Les tests presents n'en ont pas
besoin. Les tests comportementaux windows.py attendent son integration ou un
transfert explicite par le lead ; aucune panne d'import n'est fabriquee en RED.
Serena est lie a la racine canonique ARTIST, pas a ce worktree, et annonce un
serveur C++ seulement : fichiers locaux et pas de preuve LSP Python/C++ ici.

## Matrice exercee

- Tests synthetiques Python Windows : RED lead observe sur le commit de tests
  (5 tests, 2 failures attendues, 3 succes, 0 error) ; GREEN NON_EXECUTE.
- Helper PowerShell, CMake/Conan/CTest et compilation : NON_EXECUTE.
- Suites moteur CPU, OpenGL materiel/logiciel, images et performance : NON_EXECUTE.
- Linux : hors ownership, NON_EXECUTE ; aucune conclusion interplateforme.
- Aucune ressource cloud/Docker creee, aucun registre Docker necessaire.

Livraison de cette tranche : candidate de tests/plan pour revue, pas pipeline
Windows termine ni fonctionnement demontre. Aucune preuve historique ne vaut
execution de ce candidat.
