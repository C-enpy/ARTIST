# TESTS_RED_READY3 — trois findings indépendants, tests proposés seulement

## Identité et frontière d'admission

Base propre vérifiée `1d0fba7b2914c9d610f9c1857a7f1b0f3637abc0`.
Revue indépendante lead : `result_mvuvfbfr_007l_n44e8z`, sans run reviewer.
SHA256 source normalisé admis :
`2424062ecf17bdd5f679506aeb11b816365ed10a2df3bf60274952cc9ae86e2a`.
L'auteur ne modifie NI production NI les deux JSON d'admission, aucun pin ni
ancien oracle. Autorisation actuelle : nouveaux tests RED uniquement, pas GREEN.

Ce candidat n'est pas intégrable comme correction produit. **Aucun RED observé
par cette remise** : tous les programmes/test cases ci-dessous sont NON_EXECUTE.
Le lead doit relire la nouvelle fixture et l'extraction F3 avant toute exécution.
Une erreur de parsing, identité, extraction, un baseline invalide ou une fonction
absente est HARNESS/INCONCLUSIVE, jamais un RED causal.

## Fichiers et maintien des 44 tests actuels

- `test_review_regressions.py` : cinq contrôles de harness puis trois méthodes
  source, dans deux classes distinctes sélectionnables. Pas d'import du helper.
- `test_native_binding.py` : seul support `run_case` étendu par trois paramètres
  optionnels à défaut neutre : msvc_trailing_separator, targets,
  discovery_cache_lines. Aucun des 26 corps de méthodes existants ni leurs
  assertions n'est changé. Les 18 hauts niveaux restent octet-normalisés identiques.
- `host_observation_copy_fixture.ps1` : nouvelle action source-copy, proposition
  confinée à deux statements SOURCE et deux définitions pures relues, détaillée
  ci-dessous. Aucun changement de la fixture native/capture existante.
- Présente note : mapping, prévisions, commandes et limites. Pas de core,
  adaptateur, schéma, CMake/CI, backend, cloud ou ressource réservée.

## F1 — séparateur final VSINSTALLDIR

Nouveau paramètre de donnée `msvc_trailing_separator=True` : le chemin choisi
reste celui déclaré sans séparateur final, le faux export ajoute un seul `\` à
VSINSTALLDIR. Les fichiers factices cl/vcvars et le reste des métadonnées sont
inchangés ; aucun vrai vcvars/cmd n'est appelé.

`test_msvc_trailing_separator_is_same_installation_for_host_and_build` exige
succès, témoin normalisé, et les deux arguments Conan EXACTS -c:h/-c:b sur la même
installation choisie. Le contrôle nominal utilise les métadonnées sans suffixe.
Cause prédite : la comparaison textuelle GetFullPath(export) vs installation
précède la normalisation. Attendu RED : refus InvalidMsvcEnvironment, pas erreur
de fixture ou réparation des données. Aucun défaut natif reproduit n'est acquis.

## F2 — contrat producteur argv / consommateur cache

Le payload sélectionne ArtistLib_tests (compilation/exécution réelles interdites).
Prepare tourne seulement par les fragments déjà admis et la capture mémoire.
L'oracle exige le token exact `-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING=PRE_TEST`
et BUILD_TESTING=ON, sans modifier le consommateur ni admettre n'importe quel type.

Le cache synthétique est une représentation EXPLICITE à deux cas du token réellement
capturé, pas une configuration CMake cachée :
- `-D…=PRE_TEST` → `…:UNINITIALIZED=PRE_TEST` ;
- `-D…:STRING=PRE_TEST` → `…:STRING=PRE_TEST`.

Toute autre forme est HARNESS, pas interprétée ou réparée. Aucune déclaration de
cette variable n'a été trouvée dans les CMakeLists/*.cmake du projet ; cette lecture
statique ne prétend pas exécuter ni qualifier CMake. Le second sous-cas utilise
EXACTEMENT la représentation dérivée de l'argument OBSERVÉ comme donnée d'un
nouveau prepared tree. Il exige que la vraie admission build l'accepte avant
msvc-env/cmake-build mémoire. L'arbre/witness/cache ont les identités et contrôles
existants ; pas de bootstrap/configure correcteur à l'étape build.

Contrôles séparés : STRING=PRE_TEST accepté pour cette cible ; POST_BUILD, doublon
STRING, UNINITIALIZED et BOOL refusés avant observations/horloge/factory/capture.
Le contrôle STRING vérifie aussi le décodeur de token et les deux représentations
finies sur chaînes explicites ; token absent/type non modélisé restent HARNESS.
Ainsi le futur fix est attendu côté producteur avec type STRING explicite, pas
par relâchement du type/valeur/doublon côté consommateur.

Prévision : prepare réussit mais son token non typé échoue à l'oracle exact ; le
cache UNINITIALIZED dérivé est ensuite refusé par le consommateur. Deux sous-cas
RED d'une méthode, sans vrai CMake. Les négatifs restent des contrôles verts.

## F3 — proposition d'extraction avant tout run

La nouvelle fixture n'appelle JAMAIS Read-VerificationHostObservationLeaf en entier,
GetEnvironmentVariables ou GetCurrentProcess et ne substitue PAS un faux corps à
cette feuille. Aucun snapshot OS ni processus réel n'est observé dans cette action.

Champs input JSON exacts : format=1, action=source-copy, case, binding, snapshot.
Seul binding est du SOURCE transmis : région BINDING courante de l'extracteur déjà
admis, ≤65536 caractères, hash recoupé avec fragments.binding du JSON LEAD inchangé.
Tout le bloc est parsé en DATA, composé de ≤64 définitions ; aucune évaluation du
bloc complet. Le nom Read-VerificationHostObservationLeaf doit être unique ; ses
six statements top-level doivent garder les types AST attendus.

Les seuls statements copiés sont **1 et 2 (indices zéro-based)** de son EndBlock :
1. `$values = @{}` ;
2. la vraie boucle foreach de copie, dans son ordre d'origine.

Le statement0 de snapshot réel est vérifié par son texte AST normalisé mais JAMAIS
évalué. Les statements3–5 d'observation processus/return ne sont jamais sélectionnés.
La boucle ≤256 caractères est limitée à ces deux formes nominatives, relues pour
le RED actuel et le GREEN futur éventuel :

```powershell
foreach ($name in $block.Keys) { $values[$name] = $block[$name] }
foreach ($name in $block.PSBase.Keys) { $values[$name] = $block[$name] }
```

Une seule est extraite depuis la source ; ce ne sont PAS deux implémentations
alternatives injectées pour faire passer le test. La fixture ajoute seulement
`param([Collections.IDictionary]$block)` et `return $values` autour des deux vrais
statements. Snapshot synthétique exclusivement, dictionnaire détaché de toute map
OS. Pas de commande/type/membre arbitraire ou interpolation de données en code.

Le résultat copié est ensuite passé aux DEUX vraies fonctions SOURCE
Get-VerificationEnvironmentNames et Select-VerificationEnvironment, extraites de
la même AST/hash BINDING, noms uniques, chacune≤4096 caractères. Contrôle AST des
commandes/variables/types/membres, pas de copie fake du filtre. Seul appel de
fonction autorisé : Get-VerificationEnvironmentNames ; membres ContainsKey/IndexOf,
types string/char/hashtable ; refus des variables qualifiées, fonctions imbriquées,
scriptblocks, redirections et using. Programmes composés parsés avant évaluation.

Snapshot input borné aux seuls PATH/SystemRoot/TEMP/Keys/Count/canari privé, toutes
valeurs synthétiques chaînes≤1024, sans NUL. Le baseline sans Keys/Count doit donner
la copie/filtration exactes ; un hash non admis doit refuser AVANT sélection/évaluation.
Le RED ajoute Keys=COPY_KEYS_CANARY, Count=COPY_COUNT_CANARY et le canari privé :
copie COMPLÈTE exigée avant filtre, puis exclusion des trois noms sans perdre
PATH/SystemRoot/TEMP vide. Le dictionnaire fourni doit rester inchangé. Aucune
variable d'environnement réelle n'est injectée/supprimée, aucun dump réel n'est produit.

Résultats bruts : input/result JSON, stdout/stderr, argv/code/timeout et identités
sous out/verification-synthetic/windows-native-binding/. La fixture publie le
résultat en CreateNew et ne crée aucun sous-processus. Le Python runner ne lance
que l'interpréteur PowerShell fixe déjà vérifié ; pas cmd, .bat, helper ou outil.
Les compteurs ou déclarations d'un fake ne remplacent pas les assertions exactes
sur les maps réellement produites par la tranche SOURCE.

## Commandes proposées au LEAD après revue — auteur NON RUNNER

Cwd : `scripts/tests/verification_windows/`. Pas de découverte globale.
Les six baselines existantes restent requises si leur preuve ne correspond pas
au candidat final. Avant les RED, exécuter les CINQ contrôles de la nouvelle tranche :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ReviewRegressionHarnessTests
```

Seulement si ces contrôles et les pins/extractions sont valides, trois noms précis :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ReviewRegressionSourceTests.test_msvc_trailing_separator_is_same_installation_for_host_and_build test_review_regressions.ReviewRegressionSourceTests.test_prepare_discovery_argument_and_derived_cache_satisfy_build_consumer test_review_regressions.ReviewRegressionSourceTests.test_real_host_copy_preserves_shadowing_keys_before_real_filter_excludes_them
```

Prévision NON EXÉCUTÉE : cinq contrôles verts ; trois méthodes source rouges,
possiblement cinq failures avec sous-cas (F1:1, F2:2, F3:2), zéro error/skip.
Les nombres ne sont pas un oracle de causalité : le lead doit recouper chaque
échec avec les valeurs/arguments, phase_error et rapports bruts. Pas de GREEN
production avant RED acquis et autorisation distincte de correction.

## Matrice réellement exercée et limite encodage

- Auteur Windows : lecture, matching exact, AST Python sans import et comparaison
  des 44 tests existants, hashes/diff ; nouveaux tests/fixture PS NON_EXECUTE.
- Production, paramètres/WRAPPERS/NATIVE/BINDING, admissions JSON et capture : intacts.
- PS AST/runtime, 5 contrôles, 3 RED, Linux, outils, CMake, moteur, GPU : NON_EXECUTE.
- Encodage Default.CodePage vs cmd pour chemin non ASCII : limite NON ÉTABLIE,
  pas défaut natif reproduit. Une expérience future pourrait comparer des octets
  de batch synthétiques pour une page EXPLICITE et des chemins non ASCII, mais
  elle ne prouverait pas la page réelle de cmd/vcvars. Aucune nouvelle expérience,
  toolchain, API de capture ou modification contractuelle n'est introduite ici.
- Revue indépendante à réauditer après corrections/preuves. Ce candidat remet
  des tests pour qualification RED par le lead, jamais une qualification native.
