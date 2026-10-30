# TESTS_RED_READYF1B — identité du VS explicite au guard build

## Base, autorité et statut

Base propre vérifiée `9680f2d9c47e69ddd538e9c01044ac8423d4a304`.
Réaudit transmis par le lead : `result_mvuwbbib` (pas de nouveau run reviewer).
Source helper identique à ead7f837, SHA256 normalisé :
`88f1751bed1b897a7d3df965de7533ec20e12a95746ab2c75eb8d34606880545`.
Admissions LEAD `7fcee245` conservées sur disque. Autorisation actuelle : TESTS
RED SEULEMENT pour F1B ; aucune correction de production avant RED lead.

Finding établi statiquement : dans Read-VerificationMsvcEnvironmentLeaf, la
garde build compare les deux Get-VerificationAbsolutePath(requested/prepared)
AVANT la normalisation locale des séparateurs. Prepare peut donc recevoir un
argument explicite avec suffixe, publier le témoin normalisé sans suffixe, puis
build refuser la même identité explicite au seul motif du séparateur final.

Les PASS F2/F3 et la correction de l'export F1 restent attribués à leurs preuves
antérieures. Aucun RED F1B n'est acquis par cette remise, aucun GREEN proposé.
Les anciennes barrières, couture collecteur/main et revue indépendante restent
requises avant intégration.

## Diff autorisé et confinement

1. `test_native_binding.py::NativeBindingCase.run_case` ajoute la seule option
   `explicit_vs_trailing_separator=False`, neutre pour les cas existants. Si
   explicitement demandée avec un VS explicite, ajouter un seul `\` à la valeur
   de flags.VisualStudioPath. Cette option n'ajoute aucun paramètre SOURCE.
2. Elle ne modifie NI input.msvc.VSINSTALLDIR NI expected_witness ni le témoin
   synthétique écrit avant build. Aucun TrimEnd/normalisation dans la fixture,
   pas de guard fake changé, aucune nouvelle tolérance pour rendre le test vert.
3. `test_review_regressions.py` reçoit deux classes nouvelles, ajoutées APRÈS le
   module existant : trois contrôles séparés et un seul test RED. Les huit méthodes
   antérieures, leurs classes et leurs assertions restent intactes.
4. Les 26 autres méthodes natives et 18 hauts niveaux/assertions, la capture
   mémoire, sonde F3, production/WRAPPERS/NATIVE, JSON/pins, preuves existantes,
   core/adapters/CMake/CI : inchangés. Pas de nouveau framework ou vraie CLI helper.

Même runner fixe et confinement out/verification-synthetic/windows-native-binding
que le support existant ; stdout/stderr/argv/code, input/result et empreintes sont
conservés par les fonctions existantes. Aucun nouveau processus factice natif,
cmd/vcvars/build/installation, lecture ou mutation d'environnement OS ajoutés.

## Trois contrôles avant RED

`ExplicitVsDirectoryHarnessTests` :

- **test_prepare_explicit_suffix_publishes_normalized_witness** : argument
  VisualStudioPath = sélection + `\`, export synthétique sans suffixe ; succès
  attendu, témoin complet normalisé et choix Conan -c:h/-c:b exacts.
- **test_build_explicit_without_suffix_accepts_normalized_prepared_witness** :
  prepared tree valide du support, argument sans suffixe égal au témoin normalisé ;
  succès et seuls rôles mémoire msvc-env/cmake-build. Le témoin reste inchangé.
- **test_build_rejects_genuinely_different_explicit_directory_without_repair** :
  chemin explicite réellement DIFFÉRENT, fourni par le cas existant visual_studio=invalid.
  Exiger exactement « InvalidMsvcEnvironment: explicit Visual Studio differs from
  preparation. », pas une erreur de présence/toolset/parse/fixture ; zéro objet
  Process mémoire et zéro capture, donc ni vswhere, réparation ou build.

Le troisième contrôle isole l'identité de répertoire : l'autre chemin synthétique
est nommé missing VS et aucune seconde installation n'est créée ou qualifiée.
Sa présence n'est pas une précondition de cette garde, qui doit refuser AVANT de
l'inspecter. L'oracle de cause exacte empêche de substituer un faux RED/contrôle
vert causé par un outil manquant. Il ne prétend pas valider un second VS réel.

## Nouveau RED F1B, guard isolé

`ExplicitVsDirectorySourceTests.test_build_explicit_suffix_accepts_same_normalized_prepared_directory`
utilise le même prepared tree SYNTHÉTIQUE valide que le contrôle build, mais
transmet VisualStudioPath = sélection + `\`. Avant l'assertion de succès, il vérifie
le flag EXACT (suffixe conservé), l'export synthétique sans suffixe, le témoin
normalisé et tous ses champs. L'assertion héritée vérifie aussi les octets du
témoin avant/après ; aucune réparation silencieuse n'est autorisée.

Attendu métier : succès puis seulement msvc-env/cmake-build mémoire.
Prévision RED actuelle : code non nul, InvalidMsvcEnvironment à la comparaison
requested/prepared avant normalisation ; harness_error doit rester null. Cette
cause sera à recouper dans le rapport brut par le lead, pas déduite du seul code.

**Ce sont des cas indépendants.** Prepare du contrôle A et build du contrôle B/RED
ont des répertoires/témoins distincts construits par le support. Ils contrôlent
la frontière d'identité, pas une vraie campagne prepare→build sur un même témoin.
La couture réelle du même bundle/témoin reste après intégration ; aucune preuve
end-to-end ni qualification native n'est inventée ici.

## Commandes nominatives réservées au lead

Après revue de ce diff et contrôle des identités/extractions/baselines existantes,
depuis `scripts/tests/verification_windows/` :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ExplicitVsDirectoryHarnessTests
```

Seulement si les TROIS contrôles sont valides, le seul RED prévu :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ExplicitVsDirectorySourceTests.test_build_explicit_suffix_accepts_same_normalized_prepared_directory
```

Prévisions NON EXÉCUTÉES : contrôles3/3 PASS puis une méthode/une failure causale,
zéro error/skip. Une erreur de syntaxe, extraction, pin ou contrôle préalable
invalide donne HARNESS/INCONCLUSIVE, jamais le RED attendu. Pas de découverte
globale ou lancement des suites moteur pour cette tranche.

## Matrice réellement exercée

- Auteur Windows : lecture, diff et AST Python sans import ; comparaison des
  corps/classes et assertions antérieurs, hashes/source/pins ; aucun runtime PS.
- Nouveaux contrôles3 + RED1 : NON_EXECUTE. Aucune correction GREEN source.
- Linux, outil natif/CMake/CTest, moteur/GL/benchmark, installation/cloud : NON_EXECUTE.
- Encodage non ASCII, Dispose/writers hérités, couture main et ancien périmètre
  de preuve : non étendus. Réaudit requis après une vraie preuve du résidu.
