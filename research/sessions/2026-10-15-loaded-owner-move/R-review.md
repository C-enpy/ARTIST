# R — Revue correctness indépendante du diff D+H intégré

Date : 2026-10-15. Sources : principal `C:/Users/djden/source/repos/ARTIST`, HEAD `e589aeece2c5abc46aedb089c6a88a6f7dbaa239`.
Entrée : `integrated-inputs.json`, fingerprint `a1e66dd728c8e6b0700cf7db694040c9592ce18acaf53145590ad0e85053c234`.
Producteurs : D `0bbca5042af3a52994c9540f6092a84d557f8db3`, H `a423fc250ff2040a47d52f2402019b5f35abee75`.

## Verdict et portée

- **PASS — revue du contrat borné**, sans nouveau bug démontré ni correction bloquante identifiée dans D+H. Ce verdict de lecture n'est ni une preuve exhaustive ni un retest C++/GPU.
- **PASS — vérification exécutée par R** : `out/tooling/build/Scripts/python.exe -B -m unittest discover -s scripts/tests -p test_verify_asan.py -v` : 8/8, zéro échec/skip. Logs et commande dans ce répertoire.
- **INCONCLUSIVE dans R** : exécution C++ intégrée, GPU intégré, campagne ASan moteur. Aucune compilation ni exécution moteur/GPU par R ; les baux V/S sont respectés.
- Une question non bloquante sur l'environnement ASan de la campagne est détaillée ci-dessous. Elle ne démontre pas un faux PASS de la campagne en cours.

Les 15 empreintes source concordent au début et en fin de revue. Les quatre nouveaux fichiers H non suivis ont bien été lus : `cmake/AddressSanitizer.cmake`, `cmake/probes/AddressSanitizer.cpp`, `scripts/verify_asan.py`, `scripts/tests/test_verify_asan.py`. Aucun fichier source/doc/script/instruction n'a été modifié. `git diff --check` ciblé : exit 0. Aucun Docker créé.

## Contrat C++ et consommateurs suivis

- `ARTIST/include/graphic/pipeline/Shader.hpp:133` — `IShader::operator=(IShader&&)` ; `Pass.hpp:207` — `IPass::operator=(IPass&&)` : garde d'identité avant tout effet ; `free(*m_context)` pendant que le type concret destination existe ; transfert du pointeur seulement après retour normal. Les pointeurs source/destination ne changent pas en cas d'exception de cleanup. Aucun `noexcept` indu vers le chemin throwing.
- `Shader.hpp:163,179-180` / `Pass.hpp:254,273-274` : classes concrètes `final`, move constructors `noexcept`, assignations concrètes defaulted potentiellement throwing, bases à assignation protected. Les copies restent supprimées. Les bases destructrices ne rappellent pas les méthodes pures. Pas de dérivation concrète trouvée ; les mocks dérivent des interfaces. Pipeline conserve ses `shared_ptr<IPass>` ; aucune mutation par assignation de base observée dans ses consommateurs.
- `graphic/opengl/pipeline/component/{shader,pass}/Freer.hpp` inchangés, consultés avec les Contexts et Validators : getters/setters d'ID OpenGL concrets n'allouent pas ; Freer shader supprime puis met à zéro ; Freer pass détache, supprime le programme puis met à zéro. Les vrais concepts OpenGL imposent le Freer empruntant `Context&` : le nouvel appel n'est pas silencieusement facultatif pour ce backend. La permissivité préexistante UniformReader n'est pas modifiée par cette tranche.
- L'ancien contexte de Pass reste vivant chez les aliases, mais son `passID` devient zéro. Ses shaders meurent au dernier alias. Les aliases source suivent le contexte transféré. Uniform/Attribute conservent leurs propres bindings/location ; leur usage après invalidation est interdit par le contrat approuvé, pas rendu sûr par ce correctif. Aucune nouvelle promesse de thread-safety/publication : mutations sérialisées et contexte GL compatible courant restent préconditions explicites.
- Pas de nouveau calcul de taille/offset, conversion risquée, emprunt sur pointee déplacé par réallocation, ni définition hors classe introduisant une ODR divergente. Les bornes `getShader(int)` et conversions size/int préexistantes restent hors diff et ne sont pas déclarées résolues.

## Échecs partiels et limites explicites

- `ShaderLifetimeTests.cpp:550` / `ShaderMoveCleanupFailureDoesNotConsumeSource` injecte un échec avant suppression native ; les deux contexts/IDs restent accessibles. Injection désarmée avant destruction.
- `ShaderLifetimeTests.cpp:575` / `PassMoveCleanupFailureAfterDetachDoesNotConsumeSource` couvre un detach effectué avant un échec de suppression ; aucune promesse de rollback.
- `ShaderLifetimeTests.cpp:606` / `PassMoveCleanupFailureFromMovedShaderPreservesOwners` utilise réellement `IShader::getContext()` sur un shader moved-from (pas un mock throwing). Le premier shader a déjà été détaché ; le programme et la source restent possédés. Le shader est réparé avant assertions/destruction. Cela prouve le chemin d'exception C++ ciblé dans les traces auteur, pas une récupération GL sans erreur après detach partiel.
- `~Shader` / `~Pass` restent nonthrowing : un cleanup qui lève pendant destruction termine encore. Contrat explicitement accepté comme limitation, non corrigé ni présenté comme corrigé. Les loaders compile/link, états partiellement chargés et incident des logs GL vides ont été suivis sans attribuer leurs limites préexistantes à ce diff.

## Valeur discriminante des tests

- CPU : ordre destination avant transfert/destruction, suppressions uniques, matrice Loaded/Empty/MovedFrom 3x3, self-move trois états, aliases source/destination et trois échecs de cleanup.
- GPU : `ShaderLifetimeComponentTests.cpp:137,163,195,240` contrôle objets vivants avant remplacement, libération immédiate hors attachement/current binding, conservation des shaders par alias, puis suppression différée du programme courant (`GL_DELETE_STATUS`, `GL_CURRENT_PROGRAM`, unbind sans attente globale).
- Les IDs comparés distincts sont vivants simultanément ; aucune nouvelle création ne s'intercale avant les interrogations de disparition. Pas d'hypothèse « GLuint jamais réutilisé ». Les locaux/aliases sont libérés avant le TearDown de la fixture qui détruit le contexte courant.
- Données auteur seulement, recoupées indépendamment : 58/58 hashes D et 92/92 H concordants. JUnit D reparsés : RED 5/5 échecs attendus ; RED lifetime 31 = 21 réussis/10 échoués ; RED cleanup 3/3 échecs ; GREEN ciblé 34/34 ; final auteur 137/137 = 106 unit +31 GL, zéro skip. Les deux RED GL montrent l'ancien shader/programme encore vivant, pas un contexte absent.
- Probes de compilation D : témoin positif exit 0, six négatifs avec motifs attendus C2248 (protected), C3246 (final), C2679 (cross-profile), 7/7. Source de sonde et logs examinés ; pas recompilés par R. Deux profils fixture effectivement assignables positivement sont employés, pas un profil GL inexistant.

## Harness ASan

- OFF ne change pas les flags ; ON est borné Windows x64 MSVC>=19.44/Ninja/Debug, hors COVERAGE/IPO/ZI, dans un arbre de preset séparé. Les flags instrumentent les TUs projet et GTest/GMock ; les dépendances Conan binaires et le driver ne le deviennent pas.
- `/RTC1` est conservé sur la base de l'expérience H, pas déclaré incompatible par présomption. `/INCREMENTAL:NO` explicite. Copie runtime POST_BUILD avant discovery GTest ; le probe fautif n'est pas enregistré en test CTest ordinaire.
- Le probe impose `__SANITIZE_ADDRESS__`, contrôle format/stacktrace et présence du runtime chargé. Le vérificateur exige témoin sain exit0 + marqueur sans diagnostic ASan, et faute exit86 + diagnostic exact heap-use-after-free. Un crash quelconque, un exit0 fautif ou un autre diagnostic ne passe pas. H contrôle 0/86 ; pas une preuve moteur. Les huit tests Python ci-dessus ont été réexécutés par R.

## Question — distincte d'un bug démontré

**Q1 — risque potentiel moyen/P2, non bloquant pour le diff au vu des preuves présentes**

- Références : `scripts/verify_asan.py:31-37` (`main`) et `scripts/build-windows.ps1:95-103` (préflight puis CTest).
- Scénario à qualifier : le parent a un `ASAN_OPTIONS` permissif tel que `exitcode=0` ou redirige les rapports vers `log_path`. Le canari remplace ses options dans un environnement enfant ; la suite CTest suivante conserve l'environnement parent.
- Attendu : l'option effective de la campagne moteur doit conserver une faute native discriminante et ses diagnostics. Observé par lecture : seul le canari fixe ses options ; aucune normalisation de l'environnement CTest dans ce script. Aucun faux PASS moteur n'a été exécuté/observé par R. `ASAN_OPTIONS` est absent du processus R, ce qui ne documente pas celui de S.
- Impact potentiel : une campagne CTest pourrait être interprétée PASS alors qu'une configuration ASan extérieure force un exit0 ou déporte le diagnostic ; un préflight réussi ne suffit pas à qualifier cet environnement différent.
- Vérification minimale : S consigne les options effectives au lancement des tests et confirme absence d'override dangereux (ou options explicitement non permissives) + absence de diagnostics. Si la robustesse à un environnement arbitraire fait partie du harness livré, tester le canari fautif sous le même environnement/chemin CTest dans un répertoire d'artefacts. Ce point reste une question de qualification, pas un défaut reproduit de la campagne courante.

## Incertitudes et routage

Serena est correctement rattaché à ARTIST ; sa recherche de références C++ a surtout renvoyé les définitions locales, donc complétée par lecture et matching textuel ciblé. GrepAI index ARTIST disponible/actualisé, mais trace `getContext` vide : aucune conclusion d'absence de consommateurs fondée sur cette trace. Aucun activate_project ni réindexation. Utilisation des fonctions MCP directes explicitement autorisée par le lead après constat que le catalogue Code Mode ne les exposait pas.

ADR-0003 Accepted/partial ; 0005 partial ; 0008 vérifié seulement dans sa portée datée. 0002/0004/0006 non adoptés. Pas de nouvelle frontière structurelle, équation/image ou hypothèse de performance nécessitant une autre délégation dans ce diff. Le bilan final demeure conditionné aux preuves intégrées V/S ; aucun résultat de cette revue ne clôt à lui seul le lifetime CPU/GPU général.
