# Attribute — correction compatible, remise auteur

## Candidat et admission

Base `f09d58a1d45dd6c3b726f0f2e3867198439e1f9e`, correctif
`ddab33a0f9a6263ac3c13143686020b00b179a64`. Contrat `38d03b7`, puis complément
QA `a41b8c0`, tous deux réellement lus avant édition et ancêtres vérifiés de
la base du worktree. Sources exécutables initiales : `a4c0f82`.

Branche auteur `ensemble-2836751d-f19d-4058-83a8-18d5d5f9e465`, worktree
`ensemble-2dc31e0a-ql1erh-pool-9`. Le cwd initial était canonique ; lecture
externe du worktree refusée une fois, signalée, non contournée. Après transition
native implementation/high, `session_move` a lié la session à son worktree.
Copie locale complète de la politique Context Mode lue. Aucun fichier canonique,
arbre Linux de référence/intégration, workflow ou helper pool-5 modifié.

Baux Windows et Linux/GPU explicitement transférés par le lead. Miroir privé
ext4 `/home/artist/src/ARTIST-attribute`, WSL `ARTIST-Ubuntu`, utilisateur artist,
sans sudo/installation système. Clone par bundle Windows : un clone direct d'un
worktree Windows depuis WSL ne résout pas son gitdir Windows. Le miroir pointe
maintenant sur `ddab33a`, diff source nul, après vérification des mêmes octets.

## Cause et patch borné

Le suffixe legacy `[0]` ne suffit pas sur Mesa : un array1 porte un nom nu,
comme le scalaire. La sonde antérieure du lead n'a pas été répétée.

Le Reader conserve énumération, clés legacy exactes, ownership et commit final
de staging. Une seule sélection locale par réflexion utilise
`GLEW_VERSION_4_3 || GLEW_ARB_program_interface_query` et les **trois** pointeurs
chargés. Sans cette capacité complète : zéro appel moderne, ancien chemin.
Association au même programme/PROGRAM_INPUT par nom via ResourceIndex, puis
validation NAME_LENGTH/TYPE/ARRAY_SIZE/LOCATION et ResourceName. Nom temporaire
exact ou unique ajout final `[0]`, longueur/NUL cohérents, allocation bornée par
les seules formes admissibles. Une réponse incohérente échoue sans publication
ni fallback. Diagnostics figés dans les tests avant implémentation :
`INVALID_MODERN_INDEX`, `INVALID_MODERN_PROPERTIES`, `INVALID_MODERN_NAME`, sous
le préfixe `ERROR::PASS::ATTRIBUTE::`.

Seuls quatre fichiers réservés sont modifiés : Reader, tests Reader, mock GL et
tests composants Attribute. Aucun Setter, wrapper CPU, contexte public, shader,
fixture GLSL, CMake ou dépendance modifié. Pas de requête moderne au set/draw.

## RED → GREEN → vérification finale

Preuves brutes non versionnées dans `out/logs/attribute/`, conservées dans le
worktree et copiées depuis le miroir. `manifest.json` recoupe XML/découverte et
SHA256 des sources, configurations, XML et exécutables Windows ; empreintes
binaires Linux dans `linux/linux-binary-hashes.txt`.

1. **RED causal natif, avant production** : `linux/red-build.log`, `red.log`,
   `red.xml`. Build composant réussi. Un test sélectionné/exécuté, un FAIL,
   zéro skip, CTest 8. Le témoin scalaire `QA_LONG_ATTRIBUTE` vec3/size1/location0
   passe ; `qaArray` array1 publie son unique clé native exacte et ses métadonnées,
   mais isGLArray=false au lieu de true. Array2 `qaArray2` échoue aussi uniquement
   sur le bit. Compile/link et contexte passent. L'assertion n'est plus une
   hypothèse d'orthographe fournisseur. Ce test a été exécuté avant toute édition
   du Reader. HEAD initial connu ; le hash complet du delta de cette première
   étape n'a pas été enregistré à chaud (ne pas lui attribuer le hash final).
2. **RED CPU avant production** : `linux/cpu-red.xml`, `cpu-red.log` et
   `cpu-red-build-fixed.log` : 18 tests, 16 PASS, deux FAIL attendus (classification
   moderne et diagnostics/réponses). Une première compilation a détecté deux
   SCOPED_TRACE sur la même ligne ; corrigée avant cette preuve, son échec n'est
   pas présenté comme RED comportemental.
3. **GREEN ciblé initial** : 95/95 Attribute (78 CPU,17 GL), `linux/green.xml`.
4. **Final reconstruit** : 588/588 dans **chacun** des quatre runs Windows normal,
   Windows ASan, Linux normal, Linux ASan ; 512 CPU +76 GL, zéro failure/error/skip.
   Inventaires non vides et noms JUnit identiques à la découverte propre à chaque
   plateforme. Entre plateformes, 144 noms typés diffèrent seulement par
   `(glm::qualifier)0` vs `0` ; remplacement explicite et bijectif, zéro manquant
   ou supplémentaire après ce seul rapprochement.

Dernier run ASan Linux :
`linux/asan-runs/20261028T190210Z-6d8394186dd34dc68889671c8685e9a8/ctest.xml`.
Linux normal : `linux/final-normal.xml`. Windows :
`out/build/windows-msvc-debug{,-asan}/ctest.xml`, journaux finaux dans
`out/logs/attribute/windows-final-{normal-retry,asan}.log`.
Un premier build normal Windows final a expiré pendant la découverte GoogleTest
(pas un test échoué ni XML accepté) ; une relance identique sans changement de
timeout/CMake a compilé/découvert puis exécuté les 588 tests avec succès.
Copie d'archives Linux `cp -a` : refus de préservation des timestamps NTFS ;
copie simple ensuite réussie, aucune permission système modifiée.

Canaris ASan vérifiés par les outils existants : sain 0, heap-use-after-free 86,
diagnostic attendu, summaries PASS. Options
`halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0` ; ni instrumentation
des dépendances/driver ni absence exhaustive de fuites revendiquées.

### Commandes effectivement utilisées

Dans le miroir Linux, PATH commence par
`/home/artist/src/ARTIST/out/tooling/build/bin:/usr/bin:/bin` :

```sh
python3 scripts/build-linux.py --configure-only
cmake --build --preset linux-debug --target ArtistLib_component_tests --parallel 1
LIBGL_ALWAYS_SOFTWARE=1 GALLIUM_DRIVER=llvmpipe xvfb-run -a ctest --test-dir out/build/linux-debug -R NativeReaderArrayOneRegression --no-tests=error --output-on-failure --output-junit /home/artist/src/ARTIST-attribute/out/logs/attribute/red.xml
cmake --build --preset linux-debug --target ArtistLib_tests --parallel 1
ctest --test-dir out/build/linux-debug -R AttributeReaderTests --no-tests=error --output-on-failure --output-junit /home/artist/src/ARTIST-attribute/out/logs/attribute/cpu-red.xml
cmake --build --preset linux-debug --parallel 1
ctest --test-dir out/build/linux-debug --show-only=json-v1
LIBGL_ALWAYS_SOFTWARE=1 GALLIUM_DRIVER=llvmpipe xvfb-run -a ctest --preset linux-debug --output-junit /home/artist/src/ARTIST-attribute/out/logs/attribute/final-normal.xml
LIBGL_ALWAYS_SOFTWARE=1 GALLIUM_DRIVER=llvmpipe xvfb-run -a python3 scripts/build-linux.py --address-sanitizer
ctest --test-dir out/build/linux-debug-asan --show-only=json-v1
```

Windows, dans le seul worktree auteur, un job par défaut :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -AddressSanitizer
ctest --test-dir out/build/windows-msvc-debug --show-only=json-v1
ctest --test-dir out/build/windows-msvc-debug-asan --show-only=json-v1
git diff --check
```

## Couverture A1–A8 et disposition des trois anciens FAIL

- A1/A5 : NativeReaderArrayOneRegression conserve son nom mais énumère la clé
  indépendamment, contrôle scalaire et arrays1/2, map unique et métadonnées exactes.
  NativeCapabilitiesAndArraySpelling ne bloque plus sur `[0]` : il consomme la
  réflexion moteur sous la clé effectivement mesurée et exige le bit array.
- A2 : ModernUnavailableNeverCallsModernFunctions refuse tout appel moderne sans
  capacité malgré pointeurs présents, puis pour chacune des trois entrées absentes.
  ModernCoreAndExtensionKeepOnlyLegacyKeysAndDistinguishArrayOne couvre core et
  extension, scalaire/array1 et capacité minimale de nom. Fallback des suffixes
  historiques explicitement configuré, jamais GLEW réel dans les mocks.
- A3/A4 : indices arbitraires non confondus avec locations/ordre ; formes exactes
  et ajout unique, arrays1/2, double ajout refusé ; index invalide, propriétés
  incomplètes/type/taille/location contradictoires, NAME_LENGTH nul/négatif/minimal
  invalide/oversize, longueurs négative/nulle/capacité/au-delà, NUL manquant/interne
  et désaccord NAME_LENGTH. Aucun fallback après sélection moderne.
- A6 : NativeArraysDescribeThenRefuseWithoutTouchingDecoy remplace sa clé codée
  en dur par la clé legacy indépendante. Après préparation, snapshots cible,
  leurre, binding, état VAO/attribut, pointeur et programme ; ARRAY_NOT_SUPPORTED,
  octets/taille/usage conservés. NativeReflectedScalarUploadPreservesDecoy vérifie
  upload vec3 exact et leurre intact. Pas de garantie ajoutée sur le miroir CPU
  du wrapper (qui stage avant le Setter).
- A7 : LateModernErrorKeepsPublishedGenerationThenValidReflectionCanCommit contrôle
  ancienne map non vide, premier candidat valide, faute moderne tardive, ancien
  pointee/métadonnées/valeur inchangés et absence de préfixe publié ; une réflexion
  ultérieure valide remplace la génération. Tests legacy de doublons, count0 et
  remplacement conservés.
- A8 : aucun consommateur/update changé. Douze formes interleaved, quatre buffers
  separate, transform feedback et pixels préparés/legacy existants restent verts,
  sans tolérance ni oracle modifiés. Sept nouveaux noms : six CPU modernes et
  un GL scalaire ; pas retrait de test historique ni compensation par quota.

Les trois FAIL initiaux de la mission restent des archives du lead ; leur
orthographe codée en dur est disposée comme hypothèse fournisseur injustifiée,
pas comme succès historique. Le nouveau RED établit la vraie classification.

## Environnement, verdicts et limites

Linux : Ubuntu24.04, noyau WSL2 6.6.87.2, GCC/G++14.2.0, libstdc++14 ABI1 selon
profil intégré ; CMake3.28.3, Conan2.12.2, Ninja1.13.0, Python3.12.3.
Contexte demandé3.3core, **obtenu4.5core**, Mesa25.2.8, llvmpipe LLVM20.1.2,
GLSL4.50, contexte courant/thread et share-group non partagé consignés dans XML.
Windows : MSVC14.44, CMake3.31.7, contexte **obtenu3.3core**, NVIDIA RTX3090,
driver621.01, GLSL3.30 ; identité extraite des sorties des tests, pas déduite du GPU.

PASS auteur : classification et clés A1/A5, gates/réponses A2–A4, refus et upload
natifs A6, staging A7, non-régression A8, normal/ASan aux configurations exécutées.
Refactor : aucun refactor transverse nécessaire ; validation complète du diff final.
Localisation/impact : Reader seulement en production, réflexion par génération ;
coût CPU estimé de trois requêtes GL par attribut sur la route moderne, aucune
nouvelle requête au set/draw. Coût GPU/performance non mesuré, aucun gain annoncé.

Serena canonique confirmé, références du primaire Reader vers alias PassContext
observées ; spécialisation C++/templates et références replaceAttributes incomplètes,
complétées par grep exact et lectures ciblées. Delta non indexé lu dans le worktree.
QMD absent lors de l'analyse initiale ; contrats/extraits locaux fournis utilisés.

INCONCLUSIVE / hors preuve auteur : intégration réelle lead, revue indépendante
sensible et consumers QA (à organiser après intégration), CI distante de ce candidat,
Release, UBSan, GPU Linux matériel, benchmark, fuites exhaustives. Le chemin3.3 sans
route moderne sur un autre pilote à nom ambigu reste un **gap global explicite**,
ni skip/PASS universel ni minimum4.3 ajouté. Statuts ADR inchangés.
