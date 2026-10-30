# TESTS_RED_READY — candidat auteur, pas RED observé

Entrée exacte : `3ba6628a480c40461a93be42531fe87d3cff3633`, branche
`ensemble-cdf99a0b-f065-4c64-b2a7-77ebd25316d9`. Worktree propre à l'admission ;
QA §13 B5/B6/T4 présent dans ce commit avant création du worktree. Aucun fichier
producteur modifié. Helper Windows task_mvuo974b_0069_duptzs indépendant et fermé.

## Plan local et transport proposé AVANT implémentation

1. Lead relit ces tests, puis exécute seul RED causal. Les tests synthétiques
   nominaux calibrent seulement le parseur : ils ne remplacent pas les fichiers réels.
2. Après autorisation GREEN, ajouter option root OFF, inclusion `verification`
   sous garde directe ARTIST_BUILD_VERIFICATION après ArtistLib, hors BUILD_TESTING.
   Aucun add_test, découverte, custom command ou lancement ajouté. Leaf inchangé.
3. Nouveau workflow dispatch seulement ; deux jobs conditionnels Windows/Linux,
   labels self-hosted/OS/X64 fixes, machines préparées par opérateur. Python déjà
   disponible ; Linux DISPLAY et Windows environnement MSVC constituent des
   préconditions documentées dans le workflow, pas des réparations automatiques.
   Aucune preuve renderer depuis un label. Pas d'install/provisioning/fallback.
4. **Extension d'ownership à ratifier par le lead :** petit lanceur privé
   `scripts/verification_workflow.py`. Pas encore créé. API privée proposée :
   `build_argv(env) -> tuple[str, ...]`, pure, ValueError en entrée invalide ;
   `dispatch(env, *, invoke) -> int`, appelle une seule fois `invoke(argv)` et
   conserve son code. Fonctions autonomes sans imports internes/globaux auxiliaires,
   afin de tester uniquement leurs AST sans importer le module. Point d'entrée
   statique transmet os.environ et `verify.main` existant : aucun sous-processus
   supplémentaire, nouveau schéma public ou second mécanisme de capture.
5. Entrées workflow via env VERIFY_PLATFORM/PROFILE/PHASE/CONFIGURATION/SCENARIO/
   BUNDLE/EXPECTED_GL_VENDOR/EXPECTED_GL_RENDERER/ALLOW_DEPENDENCY_RESOLUTION.
   VERIFY_OUTPUT_ROOT fixe `out/verification`. Domaines fermés identiques aux tests.
   Une sélection de scénario à la fois, catalogue complet sélectionnable ; CLI
   existante conserve les sélections multiples hors workflow. Opt-in booléen
   strict true/false, accepté true uniquement en prepare. List/plan refusent cet
   opt-in et ne sondent rien. Vendor/renderer matériels exacts non vides : contrôler
   sans modifier les octets, refuser contrôles/whitespace-only ; quotes et
   métacaractères restent données argv littérales, jamais shell. Windows logiciel
   refusé. --bundle opaque transmis au core : admission/identités/prédécesseur
   restent propriété du core, aucune réparation ou phase implicite du lanceur.
6. Shell workflow : seulement `python -B scripts/verification_workflow.py` sous
   Windows, `python3 -B scripts/verification_workflow.py` sous Linux. Aucun input
   interpolé dans run. Checkout@v4 `clean: false` préserve les bundles opérateur ;
   upload-artifact@v4 distinct `if: always()` du seul `out/verification/`, absence
   warn, aucun continue-on-error/masquage verdict. Un code CLI nonzero échoue le
   job même si l'upload réussit. Un code0 list/plan n'est pas un PASS des scénarios.
7. Après GREEN : revue indépendante configuration/transport, intégration native,
   non-régression lead ; docs finales dans tranche ultérieure.

## Oracles, confinement et commandes proposées au lead

cwd : racine du candidat intégré ou ce worktree. Interpréteur historique du lead
`C:/Python313/python.exe` (à vérifier par lui, aucun sondage outil par l'auteur).

```
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_integration -p "test_*.py" -v
```

- B-T1/T2 : assertion initiale attendue `B1: root option missing or not OFF` ;
  matrice logique OFF/ON×BUILD_TESTING, ordre ArtistLib, vrai chemin/leaf/cibles.
- B-T2/T3 : commentaires/strings dummy ne font pas fonction ; contre-exemples
  synthétiques ON/inconditionnel/tests-only/mauvais chemin/lancement/dummy cible.
- T4a/e : assertion initiale workflow absent. Contrôle textuel structuré borné
  (indentation/mapping événements/choix/actions/run), mutations obligatoirement
  effectives. Aucun parser YAML installé ni supposé ; revue GitHub/YAML nécessaire.
- T4b/c/d : assertion initiale lanceur absent, jamais ImportError. Extraction AST
  uniquement des deux fonctions ; aucune initialisation module, import cible,
  shell, outil, action/helper ou enfant. Invoke injecté obligatoire enregistre
  tuple exact et renvoie code synthétique ; prédécesseur absent modélisé code3,
  manque de preuve code6, codes 0/2/3/4/5/6/130 inchangés et un seul appel.
  Les tests du core déjà intégrés restent autorité sur preuves réelles : ce delta
  vérifie uniquement que transport/upload ne les remplace pas par succès.

Aucun unittest/fake enfant exécuté par l'auteur ; pas de configure/build/CTest,
suite moteur même CPU, GL, benchmark, installation, workflow ou cloud. Pas de base
compile_commands fabriquée ni de qualification toolchain/STL/ABI prétendue.
Tests Python auteur : NON_EXECUTE ; RED attendu mais NON_OBSERVE. Natif/C++/GPU,
Windows/Linux réels : NON_EXECUTE, livraison future pour qualification uniquement.

Contrôle auteur réellement exercé via Context Mode : `ast.parse` des trois fichiers
sans import cible, 11 méthodes trouvées (4 CMake, 2 workflow, 5 transport), syntaxe
acceptée. Aucune de ces méthodes appelée. Presets observés : Ninja, arbres par
preset, configurations Debug/Release distinctes ; conanfile : CMakeDeps et
CMakeToolchain. Versions compilateur/STL/architecture/ABI non sondées (aucune
commande native de qualification autorisée). PRE_TEST et Conan restent inchangés.

## Limites explicites

Le parseur CMake est volontairement borné, pas un évaluateur CMake complet ; il
refuse les bracket arguments. Il ignore commentaires et chaînes pour les noms
de commandes ; vérifie structure et garde plutôt qu'un grep de mots. Les scopes
alternatifs touchant l'intégration imposent revue. La matrice est logique, pas
quatre configurations réelles. Contrôles workflow ne démontrent pas GitHub Actions.
La conservation effective du core et upload distant ne sont pas réexercés ici.
Revue doit confirmer shell/labels/MSVC/DISPLAY et gestion des annulations/upload.
Seuls nouveaux tests et cette note locale sont persistés à la première remise.
