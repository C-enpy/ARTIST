# GREEN_READY natif — quatre entrees, revue avant execution

**Statut courant : GETTERS_FIX_READY**, voir `GETTERS_FIX_READY.md`. Apres
correction du quota, la tentative lead sur 90c26f8 a echoue. Le getter est corrige
sur preuve diagnostique, mais le vide est perdu sur retour pur SANS produit ;
la proposition courante est RETURN_BOUNDARY_PROPOSAL.md, pas une nouvelle suite.
Le titre historique ne vaut pas resultat GREEN, ni accord d'execution.

## RED observe et perimetre autorise

Base RED lead : `77900399fb9f18bc9f75d17904f8f8195b138321`.
Le lead a observe test14 PASS (pin refuse, enfant code17, vide present), puis
13 methodes rouges / 38 failures conformes, zero error/skip, hashes stables.
Logs annonces :
`out/verification-synthetic/windows-native-selected-red-2026-10-30T03-03-59-434Z/`.
Premieres causes : les quatre refus provisoires attendus, pas un import ou une
precondition manquante. test06 reparse est NON_EXECUTE, aucun SKIP ajoute.

Autorisation suivante recue : correction consolidee des quatre entrees natives
seulement, sans execution auteur, sans CLI/helper complet ni core recopie.
Le candidat est remis en deux commits : source GREEN, puis pin/admission
distincte (`NATIVE_PIN_ADMISSION.md`). Le pin de refus reste volontairement
incompatible avec la source GREEN jusqu'au deuxieme commit. Un hash recalcule
n'est pas une permission d'executer : le lead revoit les corps et le diff de pin.

## Correction et effets limites

`scripts/build-windows.ps1`, region NATIVE uniquement :

- Read-VerificationPayloadFile : lecture FileStream en partage lecture seulement,
  longueur 2..1048576 octets avant allocation, UTF-8 strict sans remplacement.
  Petit lecteur JSON non recursif pour les quatre champs PRIVES existants ;
  chaines/arrays de chaines seulement, ponctuation/escapes/paires surrogates,
  doublons/noms inconnus/manquants et fin de texte verifies. Pas de conversion
  JSON qui ecrase les doublons, ni nouvelle dependance JSON/.NET Core, ni Request
  ou receipt. Les limites logiques des valeurs restent dans le resolver existant.
- Containment par chemins absolus normalises et prefixe bundle avec separateur ;
  inspection ReparsePoint du fichier et de chaque ancetre jusqu'a la racine,
  refus des alternate data streams. Cette branche reparse reste SOURCE_ONLY.
- Resolve-VerificationPhaseFlags : allowlist des noms, types/bornes des flags,
  legacy sans phase implicite, exclusivite ASan/configure-only et verification,
  transport obligatoire, timeout 1..3600 s, jobs 1..64, bootstrap opt-in uniquement
  pour prepare. Aucune declaration de nouveau flag dans le param du helper.
- New-VerificationNativeStartInfo : executable natif et cwd existants, ligne
  deja encodee conservee, taille UTF-16 complete argv0 quote + espace + args +
  NUL <=32767. Surrogates/NUL verifies. ProcessStartInfo sans shell, redirects
  et CreateNoWindow ; environnement enfant Clear puis Ajout des seules valeurs
  explicites, vides conserves. Aucun PInvoke/mutation de l'environnement parent.
  Les six noms publics de contexte n'ont aucune source ambiante dans cet objet.
- Invoke-VerificationNativeProcess : seul Process/StartInfo fourni par l'appelant,
  exige non demarre, puis un unique Start de cet objet. Pas de nouveau Process,
  shell, lookup PID ou commande externe ; aucun Kill tiers/descendant.
- Logs stdout.bin/stderr.bin ouverts CreateNew AVANT Start, jamais ecrases.
  Capture BaseStream en octets, pas de decodage ligne/Unicode modifiant les logs.
  Deux buffers 64 Kio, au plus une ReadAsync par flux, progression des deux flux
  pendant l'execution. Chaque bloc lu est ecrit en entier, sans quota cumule,
  troncature volontaire ou plafond substitut. Une erreur IO reste une panne
  explicite avec les traces effectivement ecrites, jamais un succes presume.
- Timeout d'execution : 1..3600000 ms. En finally, arret de ce seul objet si vivant,
  confirmation WaitForExit(2000), drain des flux <=2000 ms, fermeture des lecteurs,
  observation des eventuelles taches pendantes Wait(1000) chacune, fermeture logs.
  Aucune attente de Task.Result sur tache non terminee, thread/script/job orphelin
  cree volontairement, retry de kill ou boucle sans delai d'execution/drain.
- Code brut 0/17 preserve ; timeout = timed_out vrai, returncode null. Retour
  des chemins bruts uniquement, pas de modele de resultat de campagne.
  Premiere panne preservee, diagnostics de cleanup ajoutes apres elle, jamais
  remplaces par une fausse reussite. Les fichiers deja crees sont conserves en
  cas de panne, vides/partiels si l'enfant n'a pas pu produire de donnees.

Le Process appartient toujours a l'appelant : il n'est pas Dispose par la
capture, ce qui permet au harnais de verifier HasExited avant son propre Dispose.
Les fichiers, lecteurs et lectures asynchrones appartiennent a la capture.
Le format newline-before-finally de la region reste coherent avec le dispatch
existant ; aucun test textuel historique n'est modifie ou reinterprete comme
preuve runtime. Le helper complet reste a son refus initial.

## Sources/tests figes

Les 15 methodes natives/assertions, les 37 tests precedents, les deux fixtures
PS, leur garde d'audit/source et wrapper_child.py sont inchanges. Seul le pin
du texte natif change au second commit apres remise de la source pour revue.
Pas de nouvelle fixture ni methode de jonction/symlink, pas de toolchain modifiee.

## Commandes exactes au runner lead, APRES revue source + admission

Cwd pour les commandes ci-dessous :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline/scripts/tests/verification_windows`

Selection native explicite, test14 en premier, test06 toujours exclu :

```powershell
$selected = @(
  'test_native_contracts.WindowsNativeContractTests.test_14_review_pin_and_independent_native_fixture_selfcheck',
  'test_native_contracts.WindowsNativeContractTests.test_01_flags_nominal_and_legacy_have_no_implicit_phase',
  'test_native_contracts.WindowsNativeContractTests.test_02_flags_reject_mixed_missing_unknown_or_bootstrap',
  'test_native_contracts.WindowsNativeContractTests.test_03_payload_utf8_and_large_astral_values_use_file_transport',
  'test_native_contracts.WindowsNativeContractTests.test_04_payload_rejects_duplicates_unknown_invalid_utf8_and_size',
  'test_native_contracts.WindowsNativeContractTests.test_05_payload_cannot_escape_bundle',
  'test_native_contracts.WindowsNativeContractTests.test_07_explicit_child_environment_preserves_empty_and_omits_absent',
  'test_native_contracts.WindowsNativeContractTests.test_08_native_length_is_complete_utf16_after_quoting',
  'test_native_contracts.WindowsNativeContractTests.test_09_capture_raw_codes_zero_and_17_and_logs',
  'test_native_contracts.WindowsNativeContractTests.test_10_both_large_streams_are_drained_without_deadlock',
  'test_native_contracts.WindowsNativeContractTests.test_11_timeout_keeps_partial_streams_and_stops_owned_child',
  'test_native_contracts.WindowsNativeContractTests.test_12_existing_logs_are_not_overwritten_or_child_started',
  'test_native_contracts.WindowsNativeContractTests.test_13_composed_flags_payload_environment_capture',
  'test_native_contracts.WindowsNativeContractTests.test_15_reserved_context_environment_requires_explicit_declaration'
)
C:/Python313/python.exe -B -m unittest -v @selected
```

Non-regression ciblee des 37 tests, meme cwd, sans discovery native globale :

```powershell
C:/Python313/python.exe -B -m unittest -v test_psbase_audit test_targeted_contracts test_wrappers test_dispatch test_helper_contract
```

Ces commandes doivent etre lancees via Context Mode avec journaux conserves
sous out/ et identites source/pin/fixtures/enfant avant/apres recoupees.
Attendu NON OBSERVE par l'auteur : 14/14 natifs + 37/37 historiques,
zero failure/error/skip. Ne pas lancer test06 ou un remplacement.
Le test14 de vide/pin/enfant reste une precondition de l'interpretation du reste,
pas un motif pour changer l'environnement ou masquer ses assertions.

## Matrice reellement exercee par l'auteur

- Windows/PowerShell : parsing AST statique du fichier et de la region, PASS.
  Aucun corps de fonction, fixture, selfcheck, enfant ou test evalue.
- Comparaison Git : region WRAPPERS, DISPATCH, helper hors NATIVE, tests,
  fixtures et enfant figes ; verification des limites/selection par lecture.
- GREEN natif et non-regression : NON_EXECUTE par l'auteur.
- Payload reparse et deux symlinks core : NON_EXECUTE ici, frontieres distinctes.
- Helper/CLI reels, compiler/STL/ABI, Conan/CMake/CTest/build/install, MSVC/vcvars,
  moteur CPU/OpenGL, Linux et cloud : NON_EXECUTE. Aucun Docker/reservation.

## Risques et limites explicites

- Source-pin = gate de revue, PAS sandbox ni approbation tacite des nouveaux corps.
  Le harnais verifie le seul Python/enfant fixe ; le produit n'est pas autorise
  a executer un workload par reconstruction de chemin. Les BuildReceipt verifies
  resteront l'autorite lors de l'integration core, pas les donnees de cette fixture.
- Reparse : SOURCE_ONLY ; aucun test NTFS nouvellement execute/autorise. Les
  inspections de chemin avant ouverture ne sont pas un jail resistant a un acteur
  hostile qui remplace des ancetres concurremment (TOCTOU). Aucun tel acteur n'est
  dans les fixtures possedees. Une qualification de cette menace exige un autre
  contrat/une autre preuve, pas un PASS deduit du prefixe de chemin.
- Les delais bornent la boucle, l'arret et l'observation des lectures ; ils ne
  rendent pas temps-reel les appels OS synchrones Start/Write/Dispose. Un echec
  de stop/drain/reclamation est une panne de pipeline, pas timeout metier valide.
- La fin de l'enfant direct doit liberer les writers pour reclaim des ReadAsync.
  Le fake ne cree aucun descendant. Des descendants reels gardant des handles de
  pipe pourraient empecher la reclamation ; aucune preuve d'arbre/process tree
  ni qualification de ce cas n'est revendiquee par ces tests. Ne pas lever le
  refus du helper complet sur cette seule tranche ; ce risque reste a traiter
  avant une admission avec des outils reels/arbre de processus.
  Plus precisement, Dispose du lecteur PRECEDE pending.Wait et peut lui-meme
  etre une operation synchrone fragile face a un writer herite : les Wait bornes
  ne prouvent donc pas un delai absolu de reclamation. La capture de production
  exige une revue independante avant le helper complet, pas une simple admission
  du pin. Aucun nouveau test avec parent/descendant reel n'est autorise ici.
- Aucun transfert de contexte GL, parser JSON d'observation ou schema core copie.
  windows.py attend le core integre, les receipts et le producteur parser.
- Ces tests factices ne verifient ni std::format/stacktrace, ni une toolchain,
  ni une execution CTest/reelle GPU ou une performance. Livre pour qualification
  de cette tranche seulement, pas fonctionnement global demontre.
