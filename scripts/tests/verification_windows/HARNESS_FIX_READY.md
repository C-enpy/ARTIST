# HARNESS_FIX_READY — attentes bornees, admission hors reparse

Base : `e2f7516ed89859328c5c832eac69452ac08952de`.
Revue lead recue : admission source-pin acceptable comme gate de revue explicite,
pas sandbox ni admission automatique d'un futur GREEN. Correction autorisee
uniquement du harnais avant run ; les quatre stubs restent a refus provisoire.

## Diff de surete

Uniquement native_fixture.ps1, sans modifier les 15 tests/assertions :

- selfcheck : WaitForExit(10000), puis drains stdout/stderr chacun Wait(1000) ;
- si un delai expire ou une operation selfcheck echoue : diagnostic HARNESS,
  code 96, jamais erreur du produit interpretee comme RED ;
- selfcheck finally : au plus un Kill sur son objet Process possede, puis
  WaitForExit(2000), diagnostic HARNESS96 en echec et Dispose conserve ;
- cleanup capture : au plus un Kill sur l'objet fourni et possede, puis
  WaitForExit(2000), HARNESS96 si la fin n'est pas confirmee ;
- exceptions d'objet non associe au processus (stub/preflight avant Start)
  restent distinguees des echecs de nettoyage ; pas de boucle/retry/kill par PID.

Il ne reste aucun WaitForExit() ou Task.Wait() sans argument dans ce harnais.
Le watchdog parent Python 25 s n'est PAS le mecanisme de nettoyage : les delais
finis et le finally propres au harnais portent cette responsabilite. Une
expiration de nettoyage est une panne HARNESS et non un succes de confinement.

Les anciennes 37 assertions, les 15 nouvelles methodes, l'enfant, le pin et la
region NATIVE sont inchanges. Aucun corps GREEN ou cablage CLI/helper complet.

## Frontieres NON_EXECUTE et preconditions

test_06_payload_reparse_component_is_refused reste intact mais **NON_EXECUTE
pour cette admission**, sur instruction lead. Ne pas lancer de jonction,
symlink, elevation, autre API ou autre methode de privilege pour le remplacer.
Cette jonction proposee ne couvre PAS les deux tests symlink core SKIP : ce
sont frontieres et preuves distinctes. Le gap reparse reste ouvert.

Le selfcheck test_14 doit confirmer l'enfant fini et la conservation effective
de la variable vide dans cet environnement Windows/.NET. Si ce point echoue,
ce n'est PAS RED fonctionnel : suspendre l'acceptation de preuve, conserver
les logs et diagnostiquer sans modifier assertions ou privileges. Le test_07
ne prouve pas cette precondition a lui seul, puisqu'il vise encore un stub.

## Commande exacte proposee au runner lead, sans execution auteur

Cwd specifique pour import du module de test et de son helper local :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline/scripts/tests/verification_windows`

Via Context Mode, selection explicite des **14 methodes**, test_06 absent :

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

Le selfcheck est place en premier, sans failfast qui empecherait d'observer le
reste des refus attendus. L'analyse lead doit invalider le RED si cette
precondition ou un autre element HARNESS echoue, quels que soient les comptages.

## Prevision et matrice reelle

Prevision non executee : **14 methodes, 13 rouges / 38 failures avec sous-cas,
1 verte**, zero error/skip, code unittest 1. Cas 06 : non selectionne, NON_EXECUTE,
pas SKIP ajoute au code et pas test supprime. Premier echec produit attendu
apres selfcheck : refus provisoire de Resolve-VerificationPhaseFlags.

- Auteur : lectures, diff et analyse syntaxique statique seulement.
- Harnais/selfcheck/nouvelle suite : NON_EXECUTE par l'auteur.
- Reparse/jonction et symlinks core : NON_EXECUTE ici, aucune couverture inferee.
- Ancienne preuve 37/37 : lead sur sa base, pas relance pour ce rapport.
- Helper complet, quatre fonctions GREEN, OS leaves : toujours bloques.
- MSVC/vcvars/outils/build/install/moteur/GL/cloud : NON_EXECUTE ; aucune ressource reservee.

Le lead revoit ce diff avant le run selectionne. Cette remise corrige la surete
du harnais, pas le produit ni les findings core/alignement de racine Windows.
