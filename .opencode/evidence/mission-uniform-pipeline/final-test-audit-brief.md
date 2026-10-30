# Audit indépendant des tests intégrés — contrat et preuves

Mandat lecture seule, pas exécution/édition/délégation. Candidat de code
`165aee5f1a1da7db8431555d196081f802558ed2`, précédé des intégrations Windowsa130773
et couture0d83731. HEAD de documentation fourni `fed283b8ec15db93e2911962dd68b68180511c22` ;
les sources testées sont inchangées selon comparaison SHA256 du coordinateur.
Ne pas vérifier HEAD par Git avec ce profil : identité fournie par le runner,
distincte de la lecture personnelle des fichiers locaux.

## Contrat à auditer

Livraison du pipeline Windows/Linux pour qualification ultérieure : scénarios
sélectionnables, phases list/plan sans effets puis prepare/build/run/report séparés,
identités candidat/shaders/exécutables/environnement et preuves brutes conservées.
CPU, OpenGL logiciel et matériel restent distingués, aucun fallback silencieux.
Qualification native, moteur même CPU, rendu, benchmark, installation, workflow
réel et cloud NON_EXECUTE dans la mission ; les tests ne doivent pas les déclencher.
Rien ici ne revendique une performance ou une campagne Windows/Linux démontrée.

Périmètre de cette revue : capacité des compléments de tests G1–G4 à détecter les
bugs plausibles de leur contrat, et conservation des garanties lors de la migration.
Pas un audit global du moteur, des workloads C++/shaders ou des outils installés.
Les contrats production nécessaires sont lisibles, mais aucune conception nouvelle.

- G1 : fragments PowerShell source admis, sous ports d'effets mémoire, produisent
  directement dans le bundle du vrai BundleSink les rôles/témoin. Collecte et SHA
  avant vrais reçus sous leases ; build réadmet le même témoin sans réécriture.
  Mutation/absence refusées avant description build/runner/factory ; Conan17 conserve
  partiels et première cause. Pas témoin préfabriqué, faux reçu, bundle recopié ou
  deuxième capture pour prétendre une continuité. Programmes installés avant snapshot.
- G2 : vrai WindowsAdapter et SubprocessRunner(popen_factory mémoire), environnement
  effectivement reçu. Six champs attendus indépendants d'env_delta, request IDs
  liés au plan/reçu/argv ; routes workload, découverte/exécution CTest distinctes.
  Matériel : forçages logiciels retirés ; CPU : contexte périmé retiré sans changer
  sa politique d'héritage des forçages ; parent synthétique complet préservé.
  Windows logiciel explicitement refusé, pas backend simulé ajoutant ce support.
- G3 : ancienne fermeture globale remplacée par admission fine prepare/build,
  run helper refusé avant effets. Structure n'est pas seule preuve comportementale :
  garder les tests fonctionnels de refus, zéro effet, sentinelle et erreurs.
- G4 : obligations historiques ordre/cibles/argv/codes17 et23/restauration/captures/
  refus des effets non admis préservées par tests comportementaux. Run direct via
  adaptateur/runner, pas helper-run. Pas de skip global, archive invisible,
  test de présence d'un nom ou réexécution tautologique d'une autre méthode de test.
  La fonction d'audit dispatch reste identique/extractible ; l'ancien programme
  autonome refuse avant lecture de payload. Wrapper quoting/code17 conservés.

## Candidat à lire, pas des justifications d'auteurs

Fichiers concernés :
- scripts/tests/verification_seams/g1_windows_fixture.py
- scripts/tests/verification_seams/g1_windows_memory_driver.ps1
- scripts/tests/verification_seams/test_g1_windows_helper_seam.py
- scripts/tests/verification_windows/test_effective_windows_environment.py
- scripts/tests/verification_windows/test_wrappers.py
- scripts/tests/verification_windows/test_dispatch.py
- scripts/tests/verification_windows/dispatch_fixture.ps1

Contrats et consommateurs utiles, lecture ciblée seulement :
scripts/verification/evidence.py, runner.py, adapters/windows.py ;
scripts/tests/verification_windows/binding_fixtures.py, wrapper_fixture.ps1,
helper_functional_fixture.ps1, helper_fragment_support.py, test_helper_functional.py,
test_native_binding.py, native_binding_fixture.ps1 et admissions JSON historiques ;
scripts/tests/verification_core/runtime_fixtures.py et
test_windows_helper_evidence_contract.py. Ce profil ne lance aucun de ces fichiers.
Ne pas lire les notes de justification d'auteurs, transcripts ou board. Les
docstrings/en-têtes de code font partie du candidat, pas une autorité d'acceptation.

## Preuves exécutées par le coordinateur, non par le reviewer

- Candidat intégréa130773 :5/5 tests environnement/admission,4/4 dispatch,
 10/10 wrappers/StartInfo, sans erreur, échec ou skip. Commandes Python3.13
 `-B -m unittest -v` sur les classes EffectiveWindowsEnvironmentTests,
 WindowsRealHelperBarrierTests (méthode admission seule), WindowsDispatchMigrationTests,
 WindowsWrapperTests et NativeBindingSourceTests (exact StartInfo seul).
 Cwd scripts/tests/verification_windows. Logs/manifestes :
 out/verification-synthetic/windows-migration-integrated-2026-10-30T12-31-46-914Z/.
- Candidat165aee5 :6/6 G1BaselineTests, puis4/4 G1CharacterizationTests, sans erreur,
 échec ou skip. Python3.13 `-B -m unittest` sur les deux classes du module
 scripts.tests.verification_seams.test_g1_windows_helper_seam, option -v, cwd racine.
 Logs/manifestes : out/verification-synthetic/windows-seam-baseline-v2-2026-10-30T12-50-42-192Z/
 et out/verification-synthetic/windows-seam-characterizations-2026-10-30T12-51-14-668Z/.
 Les stdout désignent les arbres originaux conservés sous out/verification-synthetic/g1-windows/.
- Empreintes avant/après stables et comparaison actuelle aux trois source_after
 sans divergence.53 fichiers production/core/tests core inchangés par rapport au
 manifeste windows-helper-integrated-2026-10-30T11-49-13-428Z : preuves historiques
 réutilisables, pas nouvelle exécution. Ce dernier candidat avait56helper,54adapter,
 core224=219succès+5skips,28contexte,31scénarios,11intégration. Ne pas additionner
 les sélections recouvrantes ni les réattribuer au reviewer.

Les seeds G2 ne prouvent pas la couture G1 ; les tests mémoire ne prouvent ni cmd/
vcvars, MSVC/Conan/CMake installés, API graphique ou machine GPU. Les cinq skips
symlink/reparse historiques, jonction06, encodage cmd non ASCII et Dispose/writers
hérités restent non qualifiés, pas à fermer ou à tester ici.

## Restitution attendue

Revue indépendante fresh : contrat → scénario/mutation discriminante proposée
AVANT finding haut, attendu vs comportement que le test distinguerait. Lecture
seule, zéro mutation exécutée. Findings fichier/ligne, obligation et oracle manquant ;
distinguer défaut bloquant, suggestion non bloquante et limite connue hors périmètre.
Registre bidirectionnel bref obligations/preuves et éventuelles preuves orphelines.
Avis borné sur G1–G4, pas PASS natif/global par extrapolation. Aucun code ou patch
de correctif ; retour au coordinateur pour propriétaire/runner puis ré-audit si besoin.
La lecture d'un manifeste est une preuve consultée, pas une exécution du reviewer.
