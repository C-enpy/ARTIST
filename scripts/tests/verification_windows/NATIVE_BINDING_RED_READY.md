# P0–P5 — nouvelles coutures natives, TESTS_RED_READY seulement

## Entrée et périmètre

Worktree pool-31 conservé sur `44d3a0795414c2fb0d4fb24776014f4d1fd45ccb`.
Contrat final `windows-helper-evidence-v1.md`, QA §14 et rapport125 lus à la racine
canonique ARTIST, après commit lead `6989261563d28502524d3d01c4ef2b2662739c78`.
Pas de merge/copie du core. Propriétaire core distinct, aucun fichier evidence.py,
BindingRunner Python ou fake core modifié. Scores approuvés 78/68 inchangés.

Les preuves lead restent attribuées à leur candidat : baselines V2 7/7 puis
RED1 prepare code1/NOTIMPLEMENTED, journal vide, parent intact, harness_error null.
Elles ne prouvent pas les nouveaux oracles P0–P5. Aucun GREEN production autorisé.

## Mapping complémentaire AVANT toute modification historique

Toutes les lignes suivantes sont des propositions pour ratification lead ;
**aucun de ces anciens fichiers n'est modifié par cette remise**.

1. `helper_functional_fixture.ps1::Read-VerificationMsvcEnvironmentLeaf` et
   `test_helper_functional.py::MSVC` : l'oracle actuel valide x64/14.44/SDK et la
   restauration ; sa donnée ne porte pas VSINSTALLDIR. Futur renfort : ajouter
   uniquement cette métadonnée standard issue du VS explicitement demandé, ou
   d'un résultat vswhere factice déclaré en entrée. Pas de root/tooling injecté.
   Conserver le canari secret, les assertions de root/venv, et les deux options
   installation_path Conan égales à la sélection réellement observée. Le cas
   VS par défaut devra vérifier une sélection non vide, pas une variable magique.
2. `helper_fragment_support.py::extract` et `helper_fragments_red.json` : ancien
   pin = entrée fermée et huit définitions historiques. Les futures commandes de
   contexte et le déplacement SOURCE du root exigent mapping de chaque nouveau
   appel/définition puis pin d'entrée relu ; jamais auto-repin. Garder provenance
   PSScriptRoot, ordre source et tests de refus/antilegacy. Cette remise utilise
   un extracteur nouveau indépendant, sans remplacer ni modifier l'ancien.
3. `helper_functional_fixture.ps1::Assert-FixtureAst` : toute future extension
   d'allowlist sera nominative (contexte privé/ports bornés), pas wildcard. Les
   sept baselines et onze contrats, leurs assertions de timing/paths/configuration,
   et l'ancien oracle test_wrappers restent intacts ici.
4. WRAPPERS/NATIVE et `native_source.sha256` : aucun changement, aucun repin.
   Le nouveau programme réutilise le constructeur natif en mode données et le
   wrapper Child réel. Seule la primitive de capture est remplacée par un double
   mémoire ; elle n'est pas exécutée. Son comportement réel reste preuve antérieure
   à portée bornée, avec le gap Dispose/writers hérités toujours ouvert.

## Source absente : limite explicite, aucun scaffold

Le code de contexte/wiring/exporter n'existe pas encore dans le helper. Le nouveau
pin privé admet l'état courant pour les baselines, avec binding/exporter NON_ADMIS.
Les tests de source réelle produisent HARNESS/INCONCLUSIVE tant que cette source
n'est pas présente ET revue/empreintée. Aucun corps de fonction de production
n'est inventé dans la fixture pour les faire passer. Une fonction absente ne vaut
pas RED. Le RED causal reste celui déjà observé sur la fermeture globale.

Les noms privés proposés comme couture de tests sont : Enter/Get/Exit-
VerificationContext, Set-VerificationCommandRole, Get-VerificationExporterSource,
les sept feuilles du plan natif et les ports ci-dessous. Nouvelle région proposée
`ARTIST_VERIFICATION_BINDING`, séparée des deux régions pinées. Ce choix de noms
doit être relu avant source ; il ne crée aucune API publique. Après code autorisé,
le lead devra admettre explicitement le nouveau pin privé, pas exécuter une source
inattendue. Pas de scaffold source dans cette remise.

ABI privée de probe proposée : Enter-VerificationContext reçoit une hashtable de
valeurs admises root/phase/configuration/bundle_root/build_directory/log_directory/
jobs/timeout_milliseconds/python/visual_studio_installation, créée par les fonctions
réelles flags/payload/paths depuis les paramètres source du fichier. Get/Exit sans
argument ; absence/réentrance : préfixe causal InvalidVerificationContext.
Set-VerificationCommandRole reçoit le rôle fermé. Le probe direct n'impose pas
l'ordre d'une campagne : les cas entrée source prepare/build le vérifient.
Budget épuisé : préfixe privé InvalidVerificationBudget. Horloge en millisecondes,
une lecture d'origine à Enter puis une lecture par tentative de lancement ; les
tests observent ticks et budgets, pour ne pas confondre refus de rôle et expiration.

## Programme test-owned et ports

Un seul spawn : PowerShell fixe + `native_binding_fixture.ps1`, jamais le helper
complet. Les exécutions ultérieures sont des scripts FRAGMENTS test-owned dans
le même interpréteur, sans processus natif descendant. La factory crée uniquement
un objet mémoire ; aucune méthode Start/Kill/WaitForExit n'est disponible.

Liaison des seuls effets/observations, pas remplacement du wiring :
- New-VerificationProcessObjectLeaf : factory mémoire avec StartInfo réel,
  identifiant/compteur unique et Dispose observable/éventuellement fautif.
- Read-VerificationClockLeaf : suite de ticks déterministes, jamais Sleep.
- Read-VerificationHostObservationLeaf() : hashtable environment et process_executable
  synthétiques,
  pas un validateur de contexte ou de toolchain.
- Read-VerificationToolMetadataLeaf(Program) : hashtable version brute pour chemins
  possédés uniquement ; pas de booléen « valid », pas de recherche d'outil réelle.
- Invoke-VerificationNativeProcess : double terminal mémoire, observe exactement
  StartInfo.FileName/Arguments/cwd/env/timeout/logdir et écrit uniquement les deux
  bruts synthétiques sous le rôle possédé. Aucun command.json/témoin fabriqué par
  ce double : leur production doit venir du wiring réel.

Le double garde exactement les quatre champs de résultat de la primitive actuelle ;
une interruption est simulée par exception, pas par ajout d'un champ d'API. Les
effets d'outils factices ne font qu'écrire fichiers placeholder/cache texte fournis
en entrée ; aucun cache/témoin n'est déclaré valide par le double. Les observations
de versions restent des chaînes brutes. Avant sérialisation d'un env enfant, toute
clé/valeur non synthétique est refusée avec message sans valeur : pas de fuite
d'environnement réel si une future source contourne accidentellement le port.

Read/Write-EnvironmentLeaf, Invoke-VerificationProcessLeaf, validation contexte,
préconditions build, sélection/validation MSVC et logique exporter restent SOURCE.
Les fichiers CMake/metadata/exe factices sont de simples données dans le cas isolé.
Les faux .exe ne sont jamais lancés. Le constructeur piné vérifie leurs chemins,
puis la factory/capture mémoire empêche tout lancement.

Pour l'exporter, couture privée proposée : Get-VerificationExporterSource retourne
un programme littéral fixe contenant Write-VerificationMsvcExport(Snapshot,
OutputFile) et un seul appel d'entrée snapshot .NET Process. Les tests extraient
le vrai corps de filtrage/écriture et lui donnent une map synthétique ; ils
n'exécutent PAS l'entrée lisant l'environnement OS. Structure de l'entrée/literal
contrôlée séparément. Pas de filtre « équivalent » recopié dans le test.
Le getter est contrôlé AST comme retour d'un unique literal ; le fichier réellement
émis par le wiring doit porter le même hash. Le faux cmd n'interprète jamais son
command_line : il vérifie transports fixes avant d'appliquer le vrai corps exporteur
aux données synthétiques. Collision/handle de transport possédé empêchant Delete
testent les chemins d'échec, sans supprimer un fichier étranger. Le finally de
l'entrée réelle est observé avant tout cleanup de protection test-owned ; un leak
reste FAIL, jamais masqué par le cleanup de la fixture.
L'allowlist fermée proposée est explicitée dans TOOL_ENVIRONMENT_NAMES du nouveau
test (système/temp/user paths, VS/VC/SDK, Conan/jobs). Les oracles vérifient le
sous-ensemble de clés, pas seulement l'exclusion de deux canaris connus. Cette
liste de tests est indépendante du filtre production, jamais importée par celui-ci.

État des doubles seulement : table Global nommée ARTIST_NATIVE_BINDING_TEST_STATE_V1
dans l'interpréteur isolé, collision refusée, retrait finally et assertion d'absence.
Aucun Global dans le binding production. Les scripts SOURCE partagent leur propre
scope de fichier ; root provient des lignes source et du vrai PSScriptRoot, jamais
de la table des doubles. Le probe bas niveau n'est pas une preuve du dispatch réel :
les cas prepare/build utilisent exclusivement l'entrée source complète extraite,
en conservant son ordre. Les 18 tests précédents restent une autre preuve.
La région de définitions binding doit précéder tout exécutable d'entrée et le
dispatch dans la SOURCE ; l'extracteur refuse de la remonter depuis après les
appels. Le corps exporteur existe seulement dans son script littéral, sans fonction
helper homonyme masquée par la fixture. Ces contrôles empêchent deux formes de
wiring artificiellement réparé par l'extraction.

## Oracles et exécution lead prévue

- P0 : extraction/population/pins et constructeur données ; factory 0/17/throw,
  Dispose, horloge, no-clobber et cleanup indépendants avant source réelle.
- P1 : contexte absent/réentrant refusé, copies détachées, null/vide et deux
  invocations indépendantes ; environnement OS comparé seulement en mémoire,
  seul booléen d'égalité publié, aucun dump de valeurs réelles.
- P2/P3 : wrapper réel vers feuille réelle, command_line exactement transmise,
  un Process par appel, codes null/nonzero/timeout/exception/Dispose défavorables,
  première cause et bruts ; horloge globale décroissante, épuisement zéro factory.
- P4 : exporter réel, canari secret exclu, vide présent/absent omis, transport exact,
  collision sans clobber et cleanup anormal visible. Traces exactement neuf champs.
- P5 : entrée prepare réelle, témoin douze chaînes après succès seulement ; VS
  explicite invalide sans fallback, VSINSTALLDIR identique host/build ; entrée build
  refuse témoin/cache/toolchain/build.ninja divergents avant observations/factory,
  aucun bootstrap/configure/rewrite. Témoin déclaratif, aucune preuve de hash binaire.

Le nouveau module ne doit PAS être ajouté à une découverte Windows globale.
Depuis `scripts/tests/verification_windows/`, après revue des programmes :

```powershell
C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingHarnessTests
# Seulement après source autorisée, intégrée à ce worktree et nouveau pin relu :
C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingSourceTests
```

Le deuxième appel n'est PAS un RED recevable sur les fonctions absentes actuelles.
La couture avec les vrais receipts core attend les deux producteurs intégrés.
Toutes preuves nouvelles seront sous out/verification-synthetic/windows-native-binding/
avec input/result/process/stdout/stderr, fragments et identités. Auteur non runner :
aucun PS/unittest/import de test/fakechild/processus outil exécuté ; AST Python seul.

## État de remise auteur

**6 baselines et 20 tests source proposés**, pas 26 succès. Le binding et l'exporter
sont absents/non admis sur ce candidat : les 20 tests sont explicitement bloqués
HARNESS/INCONCLUSIVE, sans fallback ni fonction validante de test remplaçant le
produit. Une nouvelle admission privée/source et la ratification des coutures
nommées précèdent leur exécution. Les anciens 7+11 restent intacts.

Contrôle réellement effectué par l'auteur : AST du nouveau Python, sans import
local ni exécution ; identité source/pins par lecture de données et contrôle Git.
PowerShell, constructeur/factory/clock/exporter et les tests proposés restent
NON_EXECUTE. Les résultats 7/7+RED1 sont exclusivement les preuves lead antérieures,
pas une mesure de ce nouveau candidat. Aucun compilateur, configure/build, moteur,
GPU, qualification native, identité binaire ou performance démontrés.
