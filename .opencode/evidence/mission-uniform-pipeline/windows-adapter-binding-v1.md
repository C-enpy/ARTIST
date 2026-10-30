# Raccordement Windows v1 — décision locale ratifiée, intégration différée

Décision lead après proposition lecture seule `result_mvugyqsg_003r_m4rxgf`.
Cette note fixe la prochaine tranche ; elle ne décrit pas des interfaces déjà
modifiées. Le collecteur de contexte en cours doit d'abord être intégré.
Complément QA persisté/commité avant le worktree auteur de cette tranche.

## Commandes d'exécution

`WindowsAdapter.describe_execution(plan, build)` décrit directement les workloads
admis par le BuildReceipt et CTest : les options visibles dans CommandSpec.argv
sont exactement celles exécutées. Pas de helper-run opaque, payload miroir,
second lancement ou deuxième capture. Le runner commun possède capture, codes,
logs et admission ; l'adaptateur assemble purement commandes et env_delta.

Les chemins workloads viennent du mapping cible/exécutable admis et de son
inventaire d'identité, pas d'une concaténation de noms considérée comme autorité.
CTest est un outil lanceur, pas une cible moteur du receipt : arbre, configuration,
cibles sélectionnées et découverte doivent rester liés aux mêmes entrées admises.
Les six variables de contexte sont déclarées explicitement depuis plan/receipts.
Aucun environnement établi dans un PowerShell enfant n'est supposé persister dans
le parent Python ; le pont d'environnement outil doit rester explicite et filtré.

Le helper garde prepare/build après raccordement et tests. Ses fonctions privées
run/capture peuvent rester source-only hors du critère du run commun ; elles ne
doivent pas envelopper les commandes directes ou constituer une seconde preuve.

## Préparation et propriété du transport

Au prochain point d'intégration, la signature devient :
`describe_preparation(plan, *, bundle_root: str) -> tuple[CommandSpec, ...]`.
Le chemin absolu est fourni par `evidence.prepare`, après création/admission du
bundle sous lease. Pas de sink vivant, setter mutable, global, nouveau modèle
sérialisé ou run_id supplémentaire. Linux et doubles acceptent aussi ce keyword,
sans fallback attrapant TypeError pour revenir silencieusement à l'ancienne API.

Le sink matérialise un transport Windows privé, petit et borné, après opt-in et
avant appel d'une commande. Convention unique retenue :
`raw/windows-helper-input.json`, exactement quatre champs existants :
`build_directory` selon `_build_root`, `targets` union ordonnée dédupliquée du
plan, `executable: ""`, `arguments: []`. Pas d'enveloppe publique ajoutée à ce
payload ; sa liaison run/plan est portée par manifeste, receipt et empreinte.

Création exclusive, fermeture, empreinte et inscription artifact précèdent la
commande. Logs privés : `logs/windows-helper-prepare/`, puis
`logs/windows-helper-build/`, créés exclusivement sous le même lease. La capture
commune reste la preuve primaire des commandes. Aucun chemin de dernier bundle
deviné ni écriture par une description, list ou plan.

Build réutilise le même payload après admission du PreparationReceipt et de son
empreinte ; sa signature reste inchangée. Collision ou mutation refuse avant
lancement, sans réécriture réparatrice. Une tentative nouvelle utilise un nouveau
bundle. Artefacts partiels conservés en échec, pas de suppression de preuves.

## Séquence et responsabilités

1. Intégrer le collecteur de contexte, vérifier le diff et les tests disponibles.
2. Persister/committer le complément QA de ce delta et fixer le HEAD auteur.
3. Un auteur core unique traite interface, matérialisation et adaptations bornées
   des doubles : RED causal, GREEN minimal, non-régression puis revue appropriée.
4. Intégrer cette entrée avant les consommateurs Windows/Linux. Leurs branches
   anciennes ne reçoivent pas les fichiers implicitement et ne copient pas de
   schéma ou de branche en cours.
5. Raccorder les adaptateurs et le helper prepare/build avec tests factices avant
   implémentation ; aucune exécution d'outil moteur réel dans cette mission.

Oracles : deux bundles du même plan distincts, descriptions pures, payload fermé
et empreinté avant fake runner, no-clobber et mutation avant build, opt-in réservé
à prepare, argv directs conformes, exécutable admis, configuration cohérente et
Conan2 avant CMake. Les détails de tests sont précisés par le complément QA,
sans nouveaux scénarios moteur ni infrastructure.

Request, CommandSpec et schémas de receipts restent inchangés. Aucun backend,
variante C++ ou promesse GPU ajoutée. Toutes commandes réelles MSVC/build/workload
restent NON_EXECUTE ; seuls données synthétiques et programmes factices relus
peuvent être exécutés par le coordinateur.

## Précision ratifiée — localisation privée de CTest

Consultation indépendante d'architecture `result_mvulud8x_0000_07pkef`, après
proposition Windows `f14ea7115394c2a5e072ee402823b9063cfb3007`. Le lead retient
le cache du même arbre admis, sans nouveau champ ou registre d'outils.

Pour les seuls scénarios CTest, `describe_execution` exige le `CMakeCache.txt`
de l'arbre du BuildReceipt et son entrée dans `details.identity.files`. Une seule
lecture bytes, SHA256 comparé au reçu, puis décodage/parsing de ces mêmes bytes :
aucune vérification et lecture indépendantes. Cache absent, non inventorié ou
modifié : refus avant runner, sans recherche de remplacement.

Exiger exactement une définition non vide de `CMAKE_COMMAND:INTERNAL` et de
`CMAKE_CTEST_COMMAND:INTERNAL`. Refuser doublon même identique, mauvais type,
définition malformée ; une définition d'autre type ne peut être ignorée. Les
valeurs sont des chemins Windows absolus vers `cmake.exe` et `ctest.exe`, dans
le même répertoire d'installation normalisé selon la sémantique Windows,
indépendamment de l'hôte des tests. Aucune expansion d'environnement, shell,
résolution via cwd/PATH ou hypothèse de venv. Le chemin CTest déclaré est employé
inchangé pour découverte et exécution sur le même arbre/configuration.

Cette lecture de données admises est sans écriture, processus ou sonde, mais
n'est pas indépendante du filesystem. Un plan workloads-only ne lit pas ce cache.
Le hash admet les octets déclarant le chemin, pas le binaire CTest lui-même.
Même répertoire signifie cohérence, pas attestation d'installation. Remplacement
du binaire à chemin constant non détecté par ce contrôle ; aucun test ne doit
prétendre le contraire. Outil absent/inexécutable : panne explicite, pas fallback.
Qualification native et identité du binaire outil restent non démontrées.

Négatifs synthétiques supplémentaires avant GREEN : cache absent/non inventorié/
modifié ; chacune des deux clés absente/doublée/mal typée/malformée/vide ; chemin
relatif, mauvais exécutable, installations divergentes ; chemins à espaces,
séparateurs/casse Windows ; une lecture unique, workloads-only sans cache et
absence d'effets/fallback. Aucune exécution native n'est autorisée par ce contrat.

Le RED des 28 premiers tests ne couvre que l'adaptateur et la déclaration des
paramètres. Il n'autorise pas l'ouverture fonctionnelle du helper. L'ancien oracle
de fermeture reste intact tant qu'un mapping détaillé et les tests de fragments
entrée/dispatch/env/restore n'ont pas été relus et exécutés en RED par le lead.
Régions WRAPPERS/NATIVE, pins et legacy restent inchangés dans cette sous-tranche.
