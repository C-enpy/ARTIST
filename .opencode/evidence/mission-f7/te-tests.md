# F7 — génération de gaps TE (substitution explicite)

Auteur : te-cpp-subst (cpp-engineer substituant le contrat test-engineer,
trois instances inertes signalées au lead). Base : `17019e42ae4e3065651fc09c1dc544ac3f66e316`.
Périmètre : réserves T1/T2/T3 uniquement ; aucun changement de production/mock.
La sortie attend la relecture/intégration du lead et le test-reviewer fresh.

## Oracles et décisions avant exécution

Prédictions persistées avant code/run dans `out/mission-f7/te/te-slice-note.md`
et communiquées au lead. G1 : HEAD ci-dessus, delta vide, untracked 0 ; SHA256
agrégé 121 fichiers suivis include/tests/component-tests, ordre `git ls-files`,
chemin+NUL+octets+NUL : `d63d34af77ea1ce74d8f575af9f7e3f48b616725b68ae90e7851401a609f6c74`.

- **T1 KEEP** — `UniformOwnershipTests.MoveAssignmentTransfersLoadedContextWithoutCopyingBaseValue` :
  Loaded->Loaded ; identité du contexte source transférée ; source getContext
  lève `ERROR::UNIFORM::FREED_CONTEXT` ; destination garde l'identité et la valeur
  après destruction de la source. Un payload comptant ses copies, réellement
  stocké dans le `std::any` du sous-objet de base `UniformContext`, confirme zéro
  copie pendant le déplacement ; le compteur est vérifié opérant avant remise
  à zéro. Aucun ancien emprunt destination déréférencé après remplacement.
- **T2 MODIFY** — `AttributeTest.SetFloatDispatchesByElementType` :
  D2 attend `MockSetter<float>` `Times(1)` / `Ref(ownedContext)` ; spécialisation
  `shared_ptr<float>` `Times(0)` ; même objet partagé stocké, valeur 0.5f.
  Les deux singletons supplémentaires sont vérifiés/réinitialisés au TearDown,
  comme les singletons int existants. Ni wildcard sur le setter attendu, ni
  affaiblissement des attentes existantes.
- **T3 KEEP, caractérisation** —
  `UniformReaderTests.ReadUniforms_LateFailureKeepsPublishedPrefixAndPreviousEntries` :
  réflexion initiale stale/reloaded size=2 puis reloaded/reloaded size=3 ; rejet
  `DUPLICATE_NAME` après publication du premier reloaded. Map finale : deux
  entrées, reloaded size=3, stale size=2 et identité conservée. L'oracle distingue
  publication partielle et remplacement atomique/rollback. Aucune génération
  explicite ni atomicité n'est adoptée ; une future décision d'atomicité devra
  réviser volontairement cette caractérisation. Pas de DEFER nécessaire ici.

## Discipline de preuve

- Aucun RED moteur annoncé sur le code intégré pour ces caractérisations.
  Le contrôle négatif T2 est **oracle-only volontaire**, pas une régression du
  moteur : attentes initiales float=0 / shared_ptr<float>=1, puis seules ces
  attentes et leur commentaire sont corrigés après conservation de l'échec.
- Négatif : CTest exit 8, 1 cas / 1 échec ; binaire direct exit 1. Appel float
  sursaturé et attente shared_ptr<float> non satisfaite tous deux avant summary.
  XML `oracle-negative.junit.xml` SHA256
  `49d749bf5a593841bf91c613163ad445552b8036a31da790d6232d12e8850b24` ; source
  négative `32d273812cb95729793ec05e6139bf5f236d6e07fabbe9c58113b6e978f2f7c3` ;
  binaire négatif `804ac2b9d734759d34fe51b552c719f7fd673298b7b697655cafa39f16448924`.
  Source/binaire/LastTest négatifs préservés sous `out/mission-f7/te/oracle-negative/`.
- Premier build long : C1083 Invalid argument sur objet AttributeReaderTests.cpp,
  aucun test lancé, pas RED. Repli préautorisé : build court canonique
  `C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build`, source worktree,
  sans preset/modification CMake persistée, mêmes variables Debug/MOCK_GL/ASAN=OFF.
  Conan2, MSVC 14.44, SDK 10.0.26100.0 ; tooling/cache canonique réutilisé sous
  bail exclusif, pas copié. `vcvarsall` chargé dans chaque processus du runner.
- Aucun nouveau TU de test ; reconfiguration/re-glob explicite et découverte
  fraîche : 226 unitaires = 223 existants + 3 nouveaux. Les configurations
  invalides des concepts restent couvertes par les static_asserts existants.
- GREEN intermédiaire : sélection 3/3 puis unité 226/226, zéro échec/skip.
  Vérification finale du diff ci-dessous après ajout de l'assertion du compteur.

## Invariants et limites

Le wrapper Uniform possède un unique_ptr ; get/getContext empruntent. Le move
remplace/détruit l'ancien contexte destination ; l'emprunt source suit le contexte
tant que la destination vit. Le compteur CPU survit aux contextes. T3 n'offre que
la garantie de base (préfixe publié conservé après exception), pas de rollback.
Aucune garantie de concurrence : cas monothread. Pas d'objet ni de soumission
GPU réel dans ces nouveaux cas ; aucun lien supposé entre scope C++, frame et
fin d'utilisation GPU. Ces tests mockés ne prouvent ni rendu ni GPU lifetime.

Périmètre validé : Windows x64 Debug CPU uniquement. Pas de nouvelle campagne
GL native, ASan, Release/CI/autre plateforme, image ou performance. Auto-relecture
non indépendante ; aucun verdict global de mission. Aucun autre gap testé.

## Commandes et preuves finales

Runner/logs/JUnit/découvertes : `out/mission-f7/te/` du worktree ; baux rendus au
lead à l'envoi du résultat. Tous les builds `--parallel 1`, CTest sérialisé ; tous
les runs CTest utilisent `--test-output-size-passed 65536
--test-output-size-failed 65536`, `--no-tests=error`, XML témoin conservé.

Dernier candidat : `16-window-reglob-build.log` (exit 0),
`17-window-discovery.log` (226 unitaires), `18-window-selected.log` (3/3),
`19-window-unit.log` (226/226), zéro échec/skip. Les 226 noms JUnit sont exactement
les 226 noms découverts ; les trois nouveaux noms sont inclus, pas trois cas
supplémentaires à additionner à 226. Les campagnes précédentes sont des répétitions.

Obligations -> preuves -> tags :
- T1 move-assign -> nouveau cas dédié, `final-selected.junit.xml` et
  `final-unit.junit.xml` -> **executed, PASS CPU** ; zero-copy porte uniquement
  sur le payload observé, pas une revendication de coût machine nul.
- T2 dispatch float + vérification avant summary -> contrôle négatif XML 1/1
  échec puis mêmes assertions structurelles avec attentes D2 correctes, final
  1/1 vert dans sélection et suite -> **executed, PASS oracle CPU** ; mock ne
  prouve pas upload/rendu GL.
- T3 publication/générations -> cas dédié avec metadata 2->3 et identité de
  stale, 1/1 vert dans sélection et suite -> **executed, caractérisation CPU**,
  atomicité **non implémentée/non requise**, pas de correction moteur.
- Non-régression -> 226/226 unitaires frais, concepts positifs/négatifs existants
  recompilés -> **executed/compile, PASS dans ce périmètre**, pas PASS global.

SHA256 sélection finale :
`cc524e2a5171cb3f219a6d2204abeb425b960a7a78abf09bd3e409c6d6d69d94` ;
suite finale : `538121e06d9487e0e96d7d09c70730e58a6c2916cbc443a4d3a92dacf8fda6ab`.
G2 : mêmes 121 chemins que G1, source/test SHA256 final
`a55bd8fdc02f575151353116a4d10d3400ab410b529f73d35cdd115a44818a09` ; binaire
`1625c3795e62042fee3154197e72c98202c98b4ddd1f9b9483eba5464c264391`.
Delta production/mocks/CMake/fixtures vérifié vide contre `17019e4` ; uniquement
trois TU de tests et cette documentation de preuve. Détail par fichier/artefact
dans `out/mission-f7/te/manifest.json` ; binaire final et LastTest conservés.

Commandes effectives, sous `vcvarsall` dans le même processus :

```powershell
cmake -S . -B C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build
cmake --build C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build --target ArtistLib_tests --parallel 1
ctest --test-dir C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build -L unit --show-only=json-v1
# Même commande avec -R des trois noms pour la sélection ; chemins XML absolus.
ctest --test-dir C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build -L unit --output-on-failure --no-tests=error --test-output-size-passed 65536 --test-output-size-failed 65536 --output-junit <worktree>/out/mission-f7/te/final-unit.junit.xml
```

Discipline de bail : tous les runs sous bail exclusif accordé par le lead.
Le libellé explicite `build-window f7 / TE` n'a été annoncé que pour la dernière
vérification 16->19 (écart de forme initial signalé, pas d'annonce rétroactive) ;
aucun run concurrent ni GL natif. Baux build/tooling/cache rendus par le résultat
final au lead ; aucun nettoyage ou ressource Docker créée.
