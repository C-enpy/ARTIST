# Couture Windows du même bundle — plan A0 accepté

Proposition `result_mvuxoywe_008g_tq7mqk`, auteur pool-39/build-engineer,
lecture seule, aucune exécution. Entrée H0 `040824c` ; code helper `52e7330`,
collecteur `9aedc0a`, guide `9a45bf6`. Planification PM/LLM A0 63/59,
développement A1 78/73, brut/retenu identiques, plancher70 respecté pour A1.
Le lead a recoupé RuntimeFixture et evidence.prepare/build/_finish sur le code
intégré. Ce document ratifie le plan, pas les futurs programmes ni leur exécution.

## Ownership et frontières

Trois nouveaux fichiers exclusivement sous `scripts/tests/verification_seams/` :
- `g1_windows_fixture.py` : installation avant snapshot et runner fermé ;
- `g1_windows_memory_driver.ps1` : effets mémoire et programme test-owned ;
- `test_g1_windows_helper_seam.py` : baselines et quatre caractérisations.

Aucun changement de production, test/fixture historique, admission JSON, pin,
catalogue, autre branche Windows, CMake ou CI. L'auteur ne lance aucun test,
import cible, interpréteur PowerShell ou faux programme. Le lead relit les
programmes puis exécute les sélections synthétiques sous mandat runner distinct.

## Installation et identité

RuntimeFixture fournit seulement la racine temporaire possédée et l'installation,
pas son plan Linux ou son runner. Construire un vrai plan Windows cpu-only,
cpu.float-mirror, Debug, résolution explicitement autorisée, et WindowsAdapter.
Installer avant evidence.prepare tous programmes, fragments extraits et admis,
profil/requirements/fixtures requis et outils inertes, dont vcvarsall.bat.

Le programme composé au chemin scripts/build-windows.ps1 de cette racine contient
les paramètres, wrappers, données, binding et entrée réels admis. Ni corps legacy,
ni helper complet, ni Invoke-VerificationNativeProcess réel ne sont exécutés.
Le root vient du vrai PSScriptRoot. Préserver les identités de la source originale,
des pins inchangés, des fragments et du programme composé ; ne pas remplacer le
reference_head historique des admissions par H0. Aucune mutation de scripts après
snapshot et aucune modification de la politique _source_files.

Les entrées/sorties variables sont sous out/seam-control/<phase> de la même racine.
Le test atteste la propriété/confinement exacts de sa racine temporaire, sans
élargir la garde à un parent arbitraire. Un programme nécessaire découvert après
snapshot impose un nouveau cas installé avant identité, pas une exclusion.

## Flux et substitutions

Vrai BundleSink → evidence.prepare → snapshot et lease → transport privé sink
fermé/hashé → unique CommandSpec helper du vrai adaptateur → runner synthétique
fermé → driver mémoire → fragments réels produisant rôles et témoin directement
dans CE bundle → collecteur réel → observateur déléguant au vrai _finish → vrai
PreparationReceipt. evidence.build reçoit ce même receipt/plan/sink et réadmet
les mêmes octets avant adapter.describe_build puis collecte le build.

Le runner refuse toute commande autre que les descriptions prepare/build attendues
(argv, cwd, phase, flags, bundle, transports et logs). Substitution explicite vers
un seul appel du driver par commande, tracée comme simulation. Un ProcessOutcome
par invocation traverse BundleSink.command une seule fois ; ni seconde capture,
ni rappel à sink.command, ni seconde exécution pour fabriquer une preuve.

Seules feuilles remplacées : New-VerificationProcessObjectLeaf,
Read-VerificationClockLeaf, Read-VerificationHostObservationLeaf,
Read-VerificationToolMetadataLeaf et Invoke-VerificationNativeProcess. Factory
mémoire sans Start/Kill/WaitForExit, horloge finie et observations synthétiques.
Conserver les vraies fonctions contexte/admission/selection/tooling/wrappers,
Invoke-VerificationChild, rôles et Write-VerificationToolchainWitness.
Exécuter le vrai corps Write-VerificationMsvcExport extrait du littéral admis,
sur dictionnaire synthétique, pas son entrée de lecture OS. Aucun expected_witness
complet fourni au producteur, witness_data ou faux receipt favorable.

Le driver peut composer ces définitions et substitutions avant d'invoquer l'entrée
relue ; l'ordre doit exclure tout intervalle où une primitive de processus réelle
serait appelable. Pins, population AST nominative, garde des commandes et sentinelles
seront relus avant admission du programme. Ces gardes ne sont pas une sandbox.

## Filesystem et observations

Un bundle créé par le vrai sink, un arbre Windows Debug et un cas isolé. Vérifier
.phase.lock dans le bundle, .verification-writer.lock dans l'arbre, payload fermé
déjà inventorié et répertoire de phase possédé par le core avant chaque invocation.
Le driver ne crée/adopte ni bundle ni répertoire privé de phase ; les vraies
fonctions source créent les sous-répertoires de rôles conformément au contrat.

Avec tooling préinstallé et découverte VS synthétique : prepare produit python-pip,
vswhere, msvc-env, conan-install, cmake-configure ; build msvc-env et cmake-build. Traces
command.json source et stdout.bin/stderr.bin binaires directement dans le bundle.
Faux Conan écrit seulement le toolchain synthétique, configure le cache/Ninja,
build l'exécutable cible inerte requis par le vrai inventaire, jamais exécuté.
Transports exacts export-msvc.ps1/load-msvc.cmd/msvc-env.json ; pas d'adoption,
écrasement, parcours supplémentaire ou lancement .exe/.cmd/.bat.

## Oracles avant tests

Baselines : extraction/pins/population/ordre/exporter/syntaxe du composé ; programmes
préinstallés et inventoriés ; contrôle source-mutation ; observateur de lease absente ;
factory mémoire/compteurs/fermeture des writers/octets binaires/absence d'effet interdit.
Une erreur d'extraction/pin/syntaxe est HARNESS/INCONCLUSIVE, jamais RED production.

Quatre caractérisations sur production intégrée correcte :
1. Nominal prepare→build : observer octets fermés au retour runner puis avant le
   vrai _finish, calcul SHA256 indépendant, comparaison manifeste puis receipt.
   Même payload/témoin, octets/hash/mtime conservés en build, pas bootstrap/configure.
2. Mutation après prepare favorable, nouveau bundle : refus avant describe_build,
   runner/factory ; aucune réempreinte réparatrice, altération préservée.
3. Suppression après prepare favorable, nouveau bundle : même refus précoce,
   aucune recréation du témoin.
4. Conan17 après partiels : aucun configure/témoin favorable ; bruts hashés avant
   reçu défavorable, première cause enfant17 conservée, anomalie collecte secondaire.

Observateur _finish délègue toujours à la vraie fonction. Compter séparément le
driver PowerShell réel test-owned et les objets Process simulés des outils ;
aucune ambiguïté d'attribution ou revendication de campagne native.
Contre-exemples test-owned : lease absente, hash divergent et second appel doivent
faire échouer les oracles concernés, sans modifier la production ou déclarer un
faux RED historique. Défaut production révélé : reste rouge et revient au lead.

## Remise et suite

Auteur remet les trois fichiers committés, programmes/AST/allowlists/substitutions,
identités proposées, noms des baselines et caractérisations, commandes futures et
limites puis STOP. Aucun run auteur. Commande future, après relecture lead :
`C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_seams -p test_g1_windows_helper_seam.py -v`.
Disponibilité Python/PowerShell à vérifier avant run ; logs sous out/verification-synthetic,
commandes/cwd/candidat/hash et comptages attribués au runner lead. Non-régression
ciblée seulement après baselines et tests ; exécutions sérialisées.

Après shutdown/merge, contrôle du diff intégré et mise à disposition des preuves,
audit test-reviewer fresh hors board d'implémentation. Branche B indépendante.
G1 reste INCONCLUSIVE/NON_EXECUTE jusqu'aux résultats. Aucun changement ADR,
aucune preuve native Windows/Linux, outils/STL/ABI/GPU ou performance attendue.
