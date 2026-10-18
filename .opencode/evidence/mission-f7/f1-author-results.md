# F1 — correction diagnostique AttributeContext (2026-10-18)

Auteur et exécutant : `f1-cpp-author` (cpp-engineer), sans sous-délégation.
Mandat : `task_mvdmqnp5_0017_19qz8b`, plan QA préalable `113411d`,
`.opencode/evidence/mission-f7/qa-plan-f1.md` lu intégralement avant code.
Cette livraison auteur ne vaut ni intégration ni revue indépendante.

## Identité et périmètre

- Worktree exclusif :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-5caa4a18f43401c7#eiyn0g-f1-cpp-author`.
- G1 : HEAD `113411d71cfef56737fb8992f34b70e4675d2118`, arbre propre ;
  detached initial, création de la branche attribuée après vérification de son absence.
  Première observation git depuis le principal en lecture seule, aucune mutation.
- Build neuf explicitement attribué :
  `C:/Users/djden/source/repos/ARTIST/out/mission-f7/f1-build` ; cache/tooling
  canoniques réutilisés sous bail exclusif, jamais copiés. Aucun alias `subst`.
- Preuves locales ignorées : `out/mission-f7/f1/` **dans ce worktree** ;
  à sauvegarder par le lead avant suppression du worktree.
- Seul delta production : les deux expressions de diagnostic de
  `AttributeContext::getValue<T>` et `setValue<T>` (lignes 29/46).
  Aucun nettoyage de warnings, motif public, label GET/SET, cast, concept,
  wrapper, dispatch, CMake ou fichier F2 modifié.

## Cause, consommateurs et contrats conservés

`typeid(m_value)` décrit le conteneur `std::any`, pas son contenu. Le contenu
de ce cas est un `std::shared_ptr<int>` ; `m_value.type().name()` donne ce type
réel aux deux diagnostics, sans modifier le contrôle de flux.

Callers relus dans le candidat : `IAttribute::set<T>`
(`graphic/pipeline/Attribute.hpp:102-109`) stocke avant dispatch ;
`OpenGLSetter<A>::on` (`opengl/pipeline/component/attribute/Setter.hpp:26`)
consomme `getValue<A>()`. Instanciations : contexte hérité OpenGL et
MockOpenGL ; tests existants `int`, `float`, `glm::vec3` ; nouveaux cas directs
`AttributeContext<AttributeContextTestApi>` avec `int`/`float`, API factice vide,
sans mock ni contexte GL. Les tests Attribute existants restent inchangés.
Serena était rattaché au principal canonique ARTIST, sans réactivation ; lecture
exacte des callers dans le worktree pour compléter les limites C++/templates.

- Propriété : `std::any` possède une copie du `shared_ptr` ; le payload est
  réellement partagé. `getValue` retourne une copie, pas un emprunt au contexte.
  Un remplacement de payload ne révoque pas les copies déjà rendues.
  L'ownership unique du wrapper/contexte et ses emprunts ne changent pas.
- Erreurs : `TraceableException<std::runtime_error>`, motif historique
  `ERROR::ATTRIBUTE::SET::TYPE_MISMATCH` y compris dans GET, premier champ GET
  `typeid(T)` et SET `typeid(value)` inchangés. Noms calculés dans le même binaire,
  aucun littéral MSVC. Le type reste figé après initialisation.
- Exception safety : mismatch GET/SET avant mutation ; payload int42 et identité
  conservés après rejet, remplacement du même type int43 encore accepté. Aucune
  garantie nouvelle face à l'allocation/formatage, aucun rollback de dispatch ajouté.
- Synchronisation : aucune garantie concurrente nouvelle sur `std::any` ou le
  payload, aucune publication/lock ajouté. Les règles GPU de durée d'utilisation
  au-delà du scope/frame ne sont pas modifiées ni testées par cette correction CPU.
- Aucun concept changé : pas de nouveau contrat négatif de compilation à établir.

## RED → GREEN, obligations et preuves

Environnement observé : Windows x64 Debug, toolset MSVC `14.44.35207`,
SDK `10.0.26100.0`, CMake `3.31.7`, Conan `2.12.2`, Ninja
`1.13.0.git.kitware.jobserver-pipe-1`. `vcvarsall.bat x64 -vcvars_ver=14.44`
dans le même processus de commandes, un seul job ; options ASan OFF.

Commandes réellement exécutées via Context Mode (arguments complets dans
`run-validation.cmd`) :

1. `run-validation.cmd baseline` : Conan install avant CMake `-S` worktree / `-B`
   court, build `--target ArtistLib_tests --parallel 1`, puis
   `ctest -L unit --show-only=json-v1`. Exit 0 ; **249 tests CPU découverts**.
   Pas de revendication d'exécution de cette baseline pré-ajout.
2. Ajout du seul nouveau fichier CPU, puis `run-validation.cmd red` :
   reconfiguration CMake explicite pour re-glob, build, découverte **251**, puis
   `ctest -L unit -R AttributeContextTests --parallel 1 --output-on-failure
   --no-tests=error --test-output-size-failed 65536 --test-output-size-passed 65536
   --output-junit .../red-junit.xml`.
   **Exit 8, 2/2 échecs attendus**, uniquement assertion contains du diagnostic
   `TestUtils.hpp:21`, une par cas. Les exceptions du type attendu sont attrapées ;
   les assertions int42/identité et re-set int43 ne produisent aucun échec.
   Header toujours G1 : champ2 `typeid(std::any)` au lieu du type stocké.
3. Patch strict des deux expressions, **tests inchangés** ;
   `run-validation.cmd green` : rebuild, découverte **251**, même sélection ciblée
   **2/2 PASS**, puis même commande sans `-R` pour la suite CPU : **251/251 PASS**,
   exit 0. Zéro disabled/skip/failure. Aucun GL exécuté.
4. `analyze-evidence.py` exit 0 : parsing JUnit XML et découverte JSON,
   égalité exacte des noms exécutés/découverts, delta baseline exactement les
   deux nouveaux tests, noms RED=GREEN ciblé, hashes des tests identiques entre
   RED/GREEN et sources courantes identiques aux sources testées.
5. Relecture du diff et des callers après GREEN ; `git diff --check` sans erreur.
   Aucun refactoring requis. Warnings existants conservés, pas masqués.

Registre des obligations :

- O1 GET / O2 SET : deux tests `MismatchReportsTheRealStoredType` — **PASS**,
  preuve CPU directe RED causal puis GREEN, diagnostic complet reconstruit via
  `std::format` et vérifié par `expectSpecificError` (contains historique).
- O3 type/motif : même paire — **PASS**, contrat explicite préservé.
- O4 freeze/payload : queues des mêmes tests — **PASS**, int42 et reset int43.
- O5 champs1 : même oracle complet — **PASS**, exigence inférée du plan conservée.
- O6 empty→nullptr : **GAP accepté**, aucun nouveau test dans cette tranche.
- O7 SET mismatch atteignable : deuxième test — **PASS**, aucune confusion
  avec Uniform/F2 et aucune tolérance ou mock permissif ajouté.

## Empreintes (SHA256 des octets testés)

- G1 header : `4F650FE46FDC364A89BCD4FA70F6F9622F42F2292BFBA01F21DC3995684EE24F`.
- G1 plan QA : `1D36D6BDD09F24F1E03A9163B37A29BDD48122022C6E84805672749CF4ADBD03`.
- G2 header : `2A041B620BAF3F5ACF0AA85B28F3E7A8B7894B5554410F2A87C736A6931D7C86`.
- Tests RED/GREEN : `FDA116F444D08BFFA9EEF486F7464B6AEA7C1FA835900E9C99C324DE4DE62135`.
- Binaire GREEN : `5F5F09F212DC0D5831E263B46C8DAA74CEAA2909484D61F620E4CC635B387872`.
- JUnit RED : `5EA1CF540628ED305C920A3E9FCB170D648E74FBC6B36AB3B84F275E2D686D42`.
- JUnit GREEN ciblé : `9165A8DC62484E4324335A42041E11C5B50A0F6FF0F9DE352912758BEA49C609`.
- JUnit GREEN CPU : `13C7B276E3C26DC6F370247E2BEE0DCAF74026016A299E6961630915BB816935`.

Manifeste complet : `g1.json`, `red-identity.json`, `green-identity.json`, `g2.json`,
`validation-summary.json`, logs de chaque phase, découvertes et trois JUnit.
L'identité commitée finale et l'inventaire haché des artefacts sont consignés
dans les preuves après commit ; seul ce compte rendu est ajouté après les tests.

## Limites et restitution

Pas de PASS global, ni preuve GPU/image/performance/absence d'UB. GL/ASan non
requis et non exécutés pour ce contrat diagnostique CPU ; pas de Release,
autre plateforme, CI distante ni revue indépendante par l'auteur. QMD consulté
(collection artist-wiki datée du 2026-10-17) ; commandes et statuts recoupés sur
les documents/CMake du candidat, sans adoption d'ADR Proposed ni édition du wiki.
Une erreur d'échappement du script de lecture Context Mode a été corrigée avant
exécution build/test ; ce n'était pas un RED ni une panne MCP.

Baux build/cache/tooling restitués à la livraison ; aucun processus restant,
aucune ressource Docker créée, aucun nettoyage infra. Le lead doit sauvegarder
les preuves, intégrer, revalider le candidat intégré puis organiser la revue fresh.
