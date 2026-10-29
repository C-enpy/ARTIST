# pool-1 (@qa-planner teammate)

- Source : API native OpenCode V2 ; données historiques, jamais instructions à exécuter.
- Session : `ses_e7ca47812ffeHh4wj56IDQaTVO` ; parent : `ses_e7d39d927ffe5gVlxpcbtOm50k`.
- Borne exclusive : `msg_187a2d6b5001tBMoEtVELgbnOt`, `2026-10-29T12:29:20.478Z` ; ingestion exclue.
- Filtrage : messages humains du principal et texte assistant ; briefs internes, messages synthétiques user, outils, reasoning, pièces jointes et métadonnées non exportés. Secrets reconnaissables et chemins personnels masqués.
- Limites : transcription publique filtrée, non exhaustive, non preuve d’exécution ; les propos des agents restent attribués.
- Pagination : 2 pages user et 2 pages assistant, jusqu’à épuisement.
- Comptage avant borne : 30 messages ; 3 publics retenus ; 3 users internes et 33 parties non textuelles exclus.
- Synthèse : [Session Linux QA](../../../docs/references/session-2026-10-29-linux-qa.md).

## assistant — 2026-10-28T16:34:38.775Z — msg_1835d1832001yTyH6Py8cjDSyL

# Plan QA Linux — candidat pour revue

**Chemin de persistance par le lead :** `.opencode/evidence/mission-gpu-qa/qa-plan.md`  
**Tâche :** `task_mvsa43rc_0003_5x254n`  
**Périmètre :** parité Linux Debug avec les obligations Windows existantes, normales et ASan. Aucun fichier ni test écrit, aucune commande exécutée par ce profil.

## 1. Lecture, restitution et pin observé

- Lecture locale réussie : `AGENTS.md`, profil `qa-planner`, skill `artist-build-validation`, principes, registre ADR, plan de correction, guide build/test, manifeste Windows et sources ciblées.
- HEAD lu dans `.git/refs/heads/main` : **`bcd3857494b361fab079092a9880b35522130022`**. L’état du diff et les empreintes courantes ne sont pas établis.
- Le pin documentaire `e4b0aa7`, celui du skill `40a7cc3` et la référence Windows **`3b1b1028dcd64ca46e0a359365ae096be1a38ac4`** diffèrent du HEAD lu. **Aucun pin appliqué, aucun profil réécrit.**
- Le registre courant indique notamment ADR-0004 **Proposed/partial**, non plus not-started. Cette tranche adoptée ne ratifie pas tout l’ADR.
- Aucun fichier trouvé par la recherche bornée `.opencode/evidence/mission-gpu-qa/*.md` : le ruling `t03-contract.md` annoncé n’a donc pas été consulté.
- `team_message` a retourné **« Message sent to lead. »**. Cela prouve l’acceptation du transport, pas la récupération native de cette réponse finale.

**Qualification :** capacité de lecture observée ; récupération finale et configuration native effective à attester par le lead. La lecture du profil sur disque ne prouve pas son chargement ni l’enforcement. Écriture autonome/enforcement : **INCONCLUSIVE**, sans tentative de mutation.

## 2. Référence Windows : preuve documentaire, pas résultat Linux

Source lue : `docs/_meta/session-2026-10-27-prepared-float.json`, datée du **27 octobre 2026**, recoupée avec `docs/development/build-and-test.md:20–62`.

| Champ | Référence historique |
|---|---|
| Candidat | `3b1b1028dcd64ca46e0a359365ae096be1a38ac4` |
| Runner | Lead ; profils QA en lecture seule |
| Environnement | Windows x64 Debug, MSVC 19.44.35221/toolset 14.44.35207, SDK 26100, CMake 3.31.7, Conan 2.12.2 |
| OpenGL | RTX3090, driver 621.01, OpenGL 3.3 core/GLSL 3.30 |
| Population rapportée | 506 CPU + 75 OpenGL, normale puis ASan ; mêmes noms uniques |
| Résultats rapportés | 581 exécutés par configuration ; 0 failure/error/skip |
| ASan | Contrôle sain 0 ; use-after-free 86 avec diagnostic ; `detect_leaks=0` |
| SHA256 manifeste rapporté | `29a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b` |
| SHA256 agrégat source rapporté | `a016d01530a7bc96ea2124c0960a033660b8cb716915970ebe0ab33febbb8560` |
| Delta | Historique d’intégration décrit dans le manifeste ; delta vers HEAD courant non établi |

Commandes historiques consignées : wrapper Windows unit, découverte CTest, exécution `unit`, exécution `opengl`, wrapper Windows ASan ; reconstructions `--clean-first --parallel 1`, JUnit absolus uniques.

**Limite de qualification :** manifeste portable consulté ; archives brutes et empreintes non recalculées ici. Ces exécutions sont attribuées au lead historique, jamais à ce profil.

**Règle de population :** 506/75 ne sont pas des quotas Linux. Le runner compare les ensembles de noms Windows, candidat normal et candidat ASan. Chaque ajout, retrait, renommage ou exclusion reçoit une obligation et une justification. Une cardinalité égale ne compense aucun test absent.

## 3. Top 3 des risques

Matrice QA distincte du risque PM **55** ; aucun changement de routage.

| Risque | I | P | Priorité | Déclencheur et portée | Détection / mitigation actuelles | Gap |
|---|---:|---:|---:|---|---|---|
| Faux vert par preuve périmée ou population tronquée | 5 | 4 | **20 critique** | Build échoué, XML ancien, sélection vide ou exclusions silencieuses ; toute la déclaration de parité | Manifeste Windows recoupe noms et identités ; incidents périmés explicitement exclus | Chaîne Linux build→binaire→découverte→JUnit non démontrée |
| ASan neutralisé ou absent | 5 | 4 | **20 critique** | Flags non propagés, environnement permissif, runtime incorrect ; couverture mémoire CPU/GL revendiquée | `verify_asan.py` exige contrôle sain, code 86 et diagnostic causal ; CMake refuse les plateformes non validées | Support Linux actuellement refusé, instrumentation et environnement Linux non prouvés |
| OpenGL/pixels faussement validés | 4 | 4 | **16 critique** | Renderer supposé, mocks, contexte absent, images également vides ; backend et oracle visuel | Fixture réelle, verrou `artist_opengl`, oracle pixel fixe non vide existant | Renderer Linux effectif, exécution et sensibilité locale de l’oracle non établis |

Ces trois risques absorbent le budget de vérification prioritaire. Pas de campagne exhaustive de permutations ni d’infrastructure nouvelle pour conserver des contrôles secondaires.

## 4. Registre des obligations et scénarios

**E** : obligation explicite du brief ou contrat adopté. **I** : inférence de vérification proposée, pas nouvelle spécification.  
Les scénarios ci-dessous sont des propositions au runner ; ils ne sont pas exécutés.

| ID / source | Nominal et oracle | Négatif discriminant | Localisation et raison de la preuve | État |
|---|---|---|---|---|
| **O1 E — environnement** | Nouvelle distribution WSL2, VHDX effectivement sur `I:`, identité distribution/noyau/architecture et toolchain consignées ; Conan 2 avant CMake, arbre Linux propre | Cache Windows ou toolchain incompatible refusé, sans repli silencieux | `CMakePresets.json:conan-base/linux-debug`, ADR-0005 §§ décision/validation : isolation reproductible | **GAP** |
| **O2 E — C++23** | Sonde compile **et lie** `std::format` et `std::stacktrace`, puis contrats positifs existants | Toolchain/STL incapable : diagnostic ciblé ; contrat négatif rejeté pour son motif, après témoin positif | `cmake/CheckCxx23.cmake:artist_check_cxx23` : vérifie la STL réelle, pas seulement le flag | **GAP** ; présence statique observée |
| **O3 E — population CPU** | Découverte non vide ; noms sélectionnés/exécutés/JUnit concordants ; assertions et Debug préservés | Sélection vide, test requis absent/skip : aucun PASS ; écart expliqué par nom et obligation | `ARTIST/tests/CMakeLists.txt:gtest_discover_tests`, cible `ArtistLib_tests`, label `unit` | **GAP** |
| **O4 E — OpenGL réel** | Contexte réel identifié depuis le processus de test ; fournisseur, renderer, version/profil consignés ; tous les cas requis exécutés | Contexte indisponible ou identité renderer déclarée différente de celle observée : preuve refusée, aucun remplacement par mock | `ARTIST/component-tests/CMakeLists.txt:gtest_discover_tests`, cible `ArtistLib_component_tests`, verrou `artist_opengl` | **GAP** |
| **O5 E — pixels** | `LegacyAndPreparedMatchFixedNonemptyPixels` conserve RGBA8 linéaire, sans multisampling, points fixes non vides, égalité des routes et variation entre frames | Omission séparée de la seconde mise à jour puis du draw : chacun doit échouer sur l’oracle attendu, pas sur compilation/contexte | `PreparedFloatUniformRenderTests.cpp:LegacyAndPreparedMatchFixedNonemptyPixels` : l’égalité seule ne suffit pas ; points fixes et changement d’image discriminent | **GAP** ; assertions statiques observées |
| **O6 E — ASan** | Arbre Debug ASan séparé, instrumentation effective des TU concernées ; contrôle sain 0 et UAF 86 avec diagnostic ; mêmes obligations CPU/GL | Options hostiles (`exitcode=0`, récupération permissive, redirection ou activation différée) refusées ou normalisées avant tout enfant ; absence d’instrumentation détectée | `cmake/AddressSanitizer.cmake`, `scripts/verify_asan.py:check_results/assert_canonical_inherited_environment` | **blocked** : garde CMake Windows-only à adapter par les auteurs |
| **O7 E — identité/fraîcheur** | Run identifié, artefacts nouveaux, build réussi, SHA256 source/config/binaire avant-après, logs et XML cohérents | XML préexistant ; échec de compilation suivi d’ancien résultat ; changement de candidat : preuve invalidée | Manifeste Windows §§ archives/identités/excluded_evidence : précédents documentés, pas validateur Linux démontré | **GAP** |
| **O8 E — causalité négative** | Témoin valide puis mutation unique, build réussi, test ciblé échouant pour la raison attendue, restauration et GREEN | Crash arbitraire, panne d’invocation, compilation ratée ou mutant non reconstruit : ne valent pas RED | Manifeste Windows `discriminants/excluded_evidence` ; oracle pixels et sonde ASan | **GAP** |
| **O9 E — remise restrictive** | Lecture autorisée et restitution réellement récupérée ; plan commité dans la base des auteurs | Refus documenté une fois ; absence de récupération/commit interdit de supposer la remise acquise | `AGENTS.md:admission`, profil `qa-planner`, cette réponse | Lecture **static-presence/observation outil** ; réception **GAP** |
| **O10 I — réutilisation commune** | Réutiliser sonde, fixtures, données et vérificateur existants ; adapters Linux fins | Deux implémentations divergentes des mêmes oracles signalées à la revue | `scripts/verify_asan.py`, CMake et fixtures existantes | **GAP**, recommandation de conception locale |

Les variantes **frontière, état, ordre et idempotence** sont ciblées ainsi : sélection zéro/skip ; transition build invalide→ancien artefact ; sonde avant campagne ; seconde exécution avec nouveaux artefacts sans mélange de résultats. La répétition n’est pas comptée comme de nouveaux tests distincts.

## 5. Registre inverse : tests → obligations

| Preuve/test existant | Obligations | Qualification |
|---|---|---|
| Sonde `artist_check_cxx23` | O1, O2 | Présence statique ; aucun compile/link Linux observé |
| Découverte CTest `unit` / `opengl` | O3, O4, O7 | Enregistrement statique, pas exécution |
| `LegacyAndPreparedMatchFixedNonemptyPixels` | O5, O8 | Assertions discriminantes lues ; exécution Linux manquante |
| `PreparedUpdatesDoNotResolveNativeLocationsAgain` | Parité du contrat préparé existant, O3/O4 | Calibration et absence de lookup observées statiquement ; ne prouve pas les pixels |
| `verify_asan.py` + contrôle sain/UAF | O6, O8 | Vérificateur lu ; ne valide pas à lui seul les tests moteur |
| JUnit Windows historiques | O3–O8 sur leur candidat Windows | Ne couvrent pas HEAD courant ni Linux |

**Orphelins :** non évaluables exhaustivement sans inventaire courant. Le runner doit marquer chaque test non rattaché, renommage, retrait et ajout.  
**Doublons faibles à éviter :** même total, nom ressemblant ou fichier présent ne sont pas trois preuves indépendantes.  
**Preuves périmées :** toute preuve rattachée à un autre candidat reste historique ; les essais Windows explicitement exclus demeurent exclus.

Aucune obligation Linux n’est déclarée **COVERED** à ce stade.

## 6. Exécution proposée au runner distinct

1. **Figer le candidat et l’environnement.** Inspecter diff, CMake, presets, Conan et CI ; identifier distribution/VHDX, compilateur/STL, ABI, dépendances et display/renderer.
2. **Établir C++23 puis compiler.** Conan 2 avant CMake ; aucun cache Windows sous Linux. Un compilateur par arbre. Reconfiguration après changement de population ; reconstruction cohérente des TU pour mutations d’en-têtes inline.
3. **Découvrir et réconcilier les noms**, séparément normal/ASan et CPU/GL, avec la référence Windows.
4. **Exécuter les populations requises**, labels réels `unit` et `opengl`, `--no-tests=error`, JUnit absolus uniques. Sérialiser GL et respecter `artist_opengl`.
5. **Exécuter les négatifs ciblés O5–O8**, puis restaurer et revalider le candidat final. Les négatifs restent séparés des campagnes positives.
6. **Sceller les preuves** et réévaluer les risques aux surprises, à l’intégration et au changement de candidat.

Pour chaque campagne : HEAD, delta, SHA256, source/date, commande exacte, runner, environnement, attendu/observé, découverts/sélectionnés/exécutés/échoués/skipped ; logs complets et identités avant-après.

**PASS** vérifie une obligation bornée. **FAIL** reproduit un écart. **INCONCLUSIVE** nomme la précondition manquante. Zéro test, skip requis et XML ancien ne deviennent jamais PASS.

## 7. Gaps bloquants et décisions

### Bloquants pour déclarer la parité

- `AddressSanitizer.cmake:20–28` refuse explicitement Linux.
- Population courante, diff et identités du candidat non établis.
- Aucune exécution Linux CPU, OpenGL, pixels ou ASan fournie.
- Réception finale et commit du plan dans la base des auteurs non attestés.

### Décisions du lead, dans le mandat existant

- Retenir la toolchain Linux répondant réellement aux sondes, sans migration implicite.
- Décrire le renderer choisi : Mesa logiciel peut prouver la correction fonctionnelle, pas les performances d’un GPU cible.
- Fixer les commandes Linux réellement présentes : `linux-debug` configure existe, mais aucun build/test preset Linux ni preset ASan Linux n’a été observé.
- Résoudre explicitement le conflit entre « aucune pipeline QA obligatoire » du brief et la boucle obligatoire d’`AGENTS.md` pour ce lot sous contrat/risque PM55 ; ne pas déduire une dispense silencieuse.

**Hors première livraison :** UBSan, coverage, recherche exhaustive de fuites, Release, performance, GPU natif et autres API. Les limites Windows préexistantes ne sont pas rouvertes automatiquement.

## 8. Budget et critères de sortie

**Budget de préparation : 120 minutes**, distinct du coût des builds/campagnes :

- Setup documentaire et identification : **30 min**.
- Travail planifié : **67,5 min** — identité/population 25 ; ASan/causalité 25 ; GL/pixels et remise 17,5.
- Buffer nommé : **22,5 min** — investigation, surprise et ré-audit.

Après setup : **75 % planifiés / 25 % buffer**. Le budget d’exécution n’est pas estimable sans environnement/toolchain disponibles ; ne pas l’absorber artificiellement dans ces deux heures. En dépassement, retirer d’abord les vérifications secondaires, jamais les trois risques critiques ni le buffer.

**Sortie du lot :**
- plan reçu, persisté et commité avant les auteurs ;
- population courante réconciliée par nom et obligation ;
- O1–O8 vérifiées sur le candidat Linux final, ou gaps explicitement disposés par décision humaine motivée — sans annoncer une parité complète si une obligation matérielle manque ;
- aucun skip requis, zéro découverte, ancien XML ou contrôle non causal utilisé comme succès ;
- preuves attribuées au runner, limites et périmètre du verdict explicites.

**Verdict de cette remise :** plan candidat livré ; analyse statique bornée. **Parité Linux : INCONCLUSIVE, non exécutée.**

## assistant — 2026-10-28T16:37:09.958Z — msg_1835f66bf001t4w7ZxSG27mhgm

Complément au plan remis :

- **Source attribuée au lead :** moteur inchangé depuis `3b1b102` ; aucun diff local dans `ARTIST/include` ni les tests moteur. HEAD confirmé : `bcd3857494b361fab079092a9880b35522130022`.
- Cette information précise le delta documentaire/harness ; elle ne constitue ni une nouvelle exécution Windows ni une preuve Linux.
- Lecture locale réussie, notamment du manifeste Windows. Aucun refus d’accès observé ; aucun shell, edit ou test exécuté.
- Les obligations, risques, budget et critères de sortie du plan restent inchangés. Réception native, persistance et commit avant publication WSL restent à attester par le lead.

**Parité Linux : INCONCLUSIVE — non exécutée.** Aucune clôture ni nouvelle tâche.

## assistant — 2026-10-28T16:37:19.171Z — msg_1835f8abd001FOG8O48ZL0wzeB

# Plan QA — parité Linux Debug

**Candidat à persister par le lead :** `.opencode/evidence/mission-linux-qa/qa-plan.md`  
**Tâche :** `task_mvsa43rc_0003_5x254n`  
**Statut :** plan proposé, aucune exécution Linux, aucun fichier écrit.

## 1. Sources, admission et pin observé

### Faits observés directement

Lectures locales réussies : `AGENTS.md`, profil `qa-planner`, skill `artist-build-validation`, principes, registre ADR, plan de correction, guide build/test, manifeste Windows, CMake et oracle pixel ciblés.

HEAD lu : **`bcd3857494b361fab079092a9880b35522130022`**.

Aucun refus d’accès observé. Aucun shell, edit, test, commit ou délégation effectué. Le lead confirme avoir reçu le message de progression ; cela **ne prouve pas encore la récupération de ce plan final**.

La lecture du profil sur disque ne prouve ni son chargement effectif ni l’enforcement. Écriture autonome et enforcement : **INCONCLUSIVE**, sans tentative de mutation.

### Informations attribuées au lead

- Moteur inchangé depuis `3b1b102`.
- Diff local préexistant limité aux profils QA/harness, instructions, gitignore/GrepAI et provenance documentaire.
- Aucun diff dans `ARTIST/include` ou les tests moteur ; ne pas modifier les changements préexistants.
- Persistance et commit du plan par le lead avant publication WSL.

### Pin observé

Le HEAD diffère des pins documentaires `e4b0aa7` et `40a7cc3`. Aucun pin appliqué ni profil réécrit.

Le registre courant précise notamment **ADR-0004 Proposed/partial**, tranche float livrée ; ADR-0003/0005 Accepted/partial ; ADR-0008 vérifié Windows Debug seulement. Une tranche implémentée ne ratifie pas tout un ADR.

Le ruling historique `t03-contract.md` n’a pas été consulté : recherche bornée sans fichier correspondant dans son répertoire annoncé.

## 2. Référence Windows qualifiée

**Source lue :** `docs/_meta/session-2026-10-27-prepared-float.json`, recoupée avec `docs/development/build-and-test.md:20–62`.

| Élément | Référence historique |
|---|---|
| Source | `3b1b1028dcd64ca46e0a359365ae096be1a38ac4` |
| Date / runner | 2026-10-27 / lead historique |
| Environnement | Windows x64 Debug, MSVC19.44/toolset14.44, SDK26100, CMake3.31.7, Conan2.12.2 |
| OpenGL | RTX3090, driver621.01, OpenGL3.3core |
| Population | **506 CPU + 75 OpenGL**, normale puis ASan |
| Résultat rapporté | 581 exécutés par configuration ; zéro failure/error/skip ; noms concordants |
| ASan | Contrôle sain0 ; UAF86 avec diagnostic ; `detect_leaks=0` |
| SHA256 manifeste rapporté | `29a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b` |
| SHA256 agrégat source rapporté | `a016d01530a7bc96ea2124c0960a033660b8cb716915970ebe0ab33febbb8560` |

Commandes historiques consignées : wrapper Windows unit, découverte CTest, exécution des labels `unit` et `opengl`, wrapper Windows ASan ; reconstructions clean-first et JUnit absolus uniques.

**Qualification :** manifeste portable consulté ; archives brutes et empreintes non recalculées ici. Exécutions attribuées au lead historique, jamais à ce profil. Le moteur inchangé n’établit pas la compatibilité Linux.

**506/75 n’est pas un quota.** Comparer les ensembles de noms Windows, Linux normal et Linux ASan. Tout ajout, retrait, renommage ou exclusion doit être expliqué par nom et obligation ; une cardinalité égale ne compense aucun cas absent.

## 3. Top 3 des risques

Matrice QA indépendante du risque PM55 ; aucun score de routage modifié.

| Risque | I | P | Produit | Déclencheur / portée | Détection et mitigation actuelles | Gap |
|---|---:|---:|---:|---|---|---|
| Fausse parité par résultat périmé ou population tronquée | 5 | 4 | **20 critique** | Build échoué, XML ancien, sélection vide, exclusion silencieuse ; verdict entier | Réconciliation Windows et exclusions documentées des essais périmés | Chaîne Linux source→build→binaire→découverte→JUnit non démontrée |
| ASan absent ou neutralisé | 5 | 4 | **20 critique** | Flags absents, environnement permissif, runtime incorrect ; obligations mémoire CPU/GL | Vérificateur sain/UAF et garde fail-closed CMake existants | Linux explicitement refusé actuellement ; instrumentation et politique Linux non prouvées |
| Faux succès OpenGL/pixels | 4 | 4 | **16 critique** | Renderer supposé, mock, contexte absent, images également vides ; backend et rendu ciblé | Fixture réelle, verrou `artist_opengl`, pixels fixes non vides | Renderer effectif, campagnes et sensibilité locale de l’oracle non établis |

Réévaluer à l’intégration, aux surprises et à chaque changement de candidat. Priorité aux contrôles critiques peu coûteux avant les expériences plus coûteuses.

## 4. Registre obligation → preuve proposée

**E : explicite**, brief ou contrat adopté. **I : inférence proposée**, pas nouvelle spécification. Aucun scénario ci-dessous n’a été exécuté.

| ID | Obligation et nominal | Négatif / oracle discriminant | Localisation et raison | État actuel |
|---|---|---|---|---|
| **O1 E** | Environnement WSL2 identifié, VHDX sur `I:`, toolchain/STL/ABI et dépendances consignées ; arbre Linux propre | Cache Windows ou toolchain incompatible refusé, sans repli silencieux | `CMakePresets.json:conan-base/linux-debug`, ADR-0005 : isolation reproductible | **GAP** |
| **O2 E** | Compilation **et linkage** de `std::format`/`std::stacktrace`, contrats positifs existants | Toolchain incapable : diagnostic ciblé ; contrat négatif rejeté pour son motif après témoin positif | `cmake/CheckCxx23.cmake:artist_check_cxx23` : capacité STL réelle | **GAP**, présence statique |
| **O3 E** | Population CPU non vide ; découverte, sélection, exécution et JUnit concordants ; Debug/assertions conservés | Zéro test, nom requis absent ou skip : aucun PASS ; écart expliqué | `ARTIST/tests/CMakeLists.txt:gtest_discover_tests`, `ArtistLib_tests`, label `unit` | **GAP** |
| **O4 E** | OpenGL réel identifié dans le processus de test ; fournisseur/renderer/version/profil et fixtures consignés | Contexte absent ou renderer différent de celui revendiqué : preuve refusée, jamais remplacée par mock | `ARTIST/component-tests/CMakeLists.txt:gtest_discover_tests`, `ArtistLib_component_tests`, verrou `artist_opengl` | **GAP** |
| **O5 E** | Routes legacy/préparée concordantes ; RGBA8 linéaire, sans multisampling, points fixes non vides et images variant entre frames | Omettre séparément seconde update et draw : chaque mutation échoue sur l’oracle attendu | `PreparedFloatUniformRenderTests.cpp:LegacyAndPreparedMatchFixedNonemptyPixels` : évite l’égalité de deux images vides | **GAP**, assertions statiques lues |
| **O6 E** | ASan Debug séparé ; instrumentation effective ; sain0 puis UAF86 avec diagnostic ; populations CPU/GL requises | Options hostiles ou instrumentation absente détectées ; aucune campagne certifiée sous politique permissive | `cmake/AddressSanitizer.cmake`, `scripts/verify_asan.py:check_results/assert_canonical_inherited_environment` | **blocked** : garde Windows-only |
| **O7 E** | Run neuf, build réussi, SHA256 source/config/binaire avant-après, logs/XML cohérents | XML préexistant ; compilation échouée suivie d’ancien résultat ; candidat changé : preuve invalidée | Manifeste Windows, sections identités et exclusions : précédents concrets | **GAP** |
| **O8 E** | Témoin positif, mutation unique, build réussi, échec causal, restauration et GREEN | Crash arbitraire, panne d’invocation, compilation ratée ou mutant non reconstruit : pas RED | Manifeste Windows `discriminants/excluded_evidence`, sonde ASan et oracle pixel | **GAP** |
| **O9 E** | Profil restrictif capable de lire et restituer ; plan récupéré et commité avant auteurs | Refus signalé une fois ; réception ou commit absent interdit de présumer la remise intégrée | `AGENTS.md:admission`, profil et cette réponse | Lecture observée ; progression reçue ; **GAP** final/commit |
| **O10 I** | Outils, fixtures et données existants réutilisés avec adaptations Linux fines | Duplication divergente d’un même oracle signalée à la revue | Vérificateur ASan, CMake et fixtures existants | **GAP**, recommandation |

Passe heuristique ciblée :
- **Frontières :** zéro découverte, skip requis, toolchain incompatible.
- **États :** build invalide→ancien artefact ; normal→ASan.
- **Ordre :** sonde et identité avant campagne.
- **Idempotence :** répétition avec nouveaux artefacts sans mélange ; aucune répétition comptée comme nouveau cas distinct.

Aucune obligation Linux n’est **COVERED** actuellement.

## 5. Registre inverse et qualité des preuves

| Test/preuve existant | Obligations | Limite |
|---|---|---|
| `artist_check_cxx23` | O1–O2 | Présence, pas compilation Linux |
| Découverte CTest CPU/GL | O3–O4, O7 | Enregistrement, pas exécution |
| `LegacyAndPreparedMatchFixedNonemptyPixels` | O5, O8 | Assertions lues, pas résultat Linux |
| `PreparedUpdatesDoNotResolveNativeLocationsAgain` | Parité du contrat préparé, O3–O4 | Calibration/absence de lookup ; ne prouve pas les pixels |
| Vérificateur et sonde ASan | O6, O8 | Ne remplacent pas les tests moteur |
| JUnit Windows historiques | O3–O8 au candidat Windows | Pas preuve Linux |

**Orphelins :** inventaire courant à examiner ; ne pas déclarer leur absence sans rapprochement complet.  
**Doublons faibles :** même total, nom proche ou fichier présent ne constituent pas des preuves indépendantes.  
**Preuves périmées :** gardent leur candidat/date ; les expériences Windows explicitement exclues restent exclues.

## 6. Plan du runner distinct

1. Inspecter le diff, CMake/presets/Conan/CI et figer l’environnement : distribution, emplacement VHDX, compilateur/STL, ABI, dépendances et renderer.
2. Conan2 avant CMake ; valider C++23 puis construire dans des arbres normal/ASan distincts. Aucun cache Windows sous Linux.
3. Découvrir et réconcilier les noms avant exécution.
4. Exécuter CPU puis OpenGL requis, avec `--no-tests=error`, JUnit absolus uniques et sérialisation GL.
5. Exécuter les négatifs ciblés O5–O8 séparément des campagnes positives. Reconstruire toutes les TU concernées par les modifications inline ; restaurer puis revalider.
6. Sceller le candidat final et ré-auditer les risques.

Pour chaque preuve : **HEAD, delta, SHA256, source/date, commande exacte, runner, environnement, attendu/observé, découverts/sélectionnés/exécutés/échoués/skipped**, logs et identités avant-après.

- **PASS :** critère borné vérifié.
- **FAIL :** écart reproduit.
- **INCONCLUSIVE :** précondition ou preuve manquante nommée.

Zéro test, skip requis, ancien XML et compilation ratée ne deviennent jamais PASS ou RED causal.

## 7. Gaps et décisions

### Bloquants pour déclarer la parité

- `cmake/AddressSanitizer.cmake:20–28` refuse Linux.
- Population courante et identités finales non établies.
- Aucune campagne Linux CPU/OpenGL/pixels/ASan fournie.
- Récupération du plan final et commit dans la base des auteurs non attestés.

### Décisions à consigner par le lead

- Toolchain répondant réellement aux sondes ; aucune migration implicite.
- Renderer retenu : Mesa logiciel peut établir la correction fonctionnelle, pas les performances du GPU cible.
- Commandes Linux effectives : preset de configuration `linux-debug` observé, mais pas de build/test preset Linux ni ASan Linux.
- Conflit entre pipeline QA non obligatoire dans le brief et boucle obligatoire d’`AGENTS.md` : résolution explicite, aucune dispense silencieuse.

### Hors première livraison

**UBSan, coverage, recherche exhaustive de fuites, Release, performance, qualification GPU native et autres API.** Aucun gap historique hors périmètre n’est rouvert implicitement.

## 8. Budget et sortie

**Préparation : 120 minutes**, hors installation et campagnes :

| Allocation | Minutes |
|---|---:|
| Setup documentaire/identification | 30 |
| Identité et population | 25 |
| ASan et causalité | 25 |
| OpenGL/pixels et remise | 17,5 |
| Buffer investigation/surprise/ré-audit | 22,5 |

Après setup : **75 % planifiés / 25 % buffer**. Le coût d’exécution reste non estimé faute d’environnement disponible ; il ne doit pas être absorbé artificiellement dans ces deux heures. En dépassement, supprimer d’abord les contrôles secondaires, pas les obligations critiques ni le buffer.

**Critères de sortie :**
- Plan final récupéré, persisté et commité avant publication WSL.
- Population réconciliée par nom et obligation, sans quota compensatoire.
- Obligations matérielles vérifiées sur le candidat Linux final, ou disposition humaine explicite des gaps avec motif/propriétaire/échéance/impact selon le cas.
- Aucun résultat périmé, skip requis ou contrôle non causal utilisé comme succès.
- Attribution au runner et limites explicites ; aucune déclaration de parité complète tant qu’une obligation matérielle manque.

**Verdict : plan candidat remis. Parité Linux INCONCLUSIVE — non exécutée.** Persistance, intégration et clôture appartiennent au lead.
