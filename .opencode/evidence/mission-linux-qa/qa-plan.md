# Plan QA accepté — parité Linux Debug sous WSL2 et CI

Date : 2026-10-28. Auteur de la proposition : qa-planner pool-1. Relecture, décisions, persistance et exécutions futures : superviseur ARTIST. Tâche : `task_mvsa43rc_0003_5x254n`.

## Provenance et état

Proposition complète récupérée en lecture seule dans la session native `ses_e7ca47812ffeHh4wj56IDQaTVO`, message assistant `msg_1835f8abd001FOG8O48ZL0wzeB`, finish stop, texte public de 12 837 caractères. La récupération automatique Ensemble a échoué ; cette récupération manuelle ne répare pas le transport. Aucun raisonnement privé ni corps brut d'outil n'est reproduit ici.

Le serveur confirme le profil qa-planner dans ARTIST, provider OpenAI, sans ajout de permission de session. Les lectures locales et la réception d'un message de progression sont observées ; les restrictions sur disque ne prouvent pas une sandbox ni l'enforcement de chaque refus. Aucun test de refus ou mutation volontaire n'est demandé au profil restrictif. Il n'a ni écrit le plan ni exécuté les tests moteur. Le lead accepte cette remise réellement récupérée après relecture.

Candidat de départ : HEAD `bcd3857494b361fab079092a9880b35522130022`. Sources/tests moteur sans diff local ; changements préexistants de harness/profils/instructions/provenance hors ownership des auteurs. Les nouveaux worktrees ne les héritent pas automatiquement : fournir ce plan commité et les briefs explicites, sans commiter les travaux étrangers.

Référence Windows : `docs/_meta/session-2026-10-27-prepared-float.json`, source `3b1b1028dcd64ca46e0a359365ae096be1a38ac4`, 506 CPU + 75 OpenGL en normal et ASan, zéro failure/error/skip rapporté. C'est une référence datée, pas une nouvelle exécution ou un quota Linux. Archives et hashes historiques non recalculés pendant la planification. ADR-0005/0003 Accepted/partial, ADR-0008 vérifié Windows Debug seulement, ADR-0004 Proposed/partial ; autres statuts Proposed conservés.

## Périmètre et décisions du superviseur

- Parité des obligations CPU, contrats C++23, OpenGL logiciel, pixels et ASan sous Ubuntu 24.04/GCC14 WSL2 puis CI propre.
- Nouvelle distribution dédiée, VHDX sous I:/WSL/ARTIST-Ubuntu ; checkout, caches et sorties dans son filesystem Linux. Aucun arrêt/migration/unregister de docker-desktop ou d'une distribution existante.
- Mutualisation prioritaire des CMake/Conan/CTest, vérificateurs et fixtures existants. Cette priorité est explicite dans le mandat utilisateur, et non une nouvelle inférence QA.
- Pas d'UBSan, coverage, Release, recherche exhaustive de fuites, performance, nouveau backend ou qualification de GPU Linux matériel dans cette livraison. Le jalon matériel reste NOT_RUN.
- QA proportionnée, exigences obligatoires du projet conservées : plan avant auteurs, TDD par auteur, intégration avant analyse de gaps, audit fresh conditionnel à une voie native conforme. Aucun élargissement implicite de droits.
- Préserver le canari ASan sain/faute et les populations CPU/GL requises. Ne pas ouvrir des campagnes de mutations exhaustives : négatifs locaux ciblés sur les risques modifiés ; réutiliser les preuves Windows à leur portée sans les appeler preuves Linux.

## Top 3 risques : impact × probabilité, distincts des scores PM

1. **Fausse parité par artefact périmé/population tronquée : 5 × 4 = 20.** Déclencheurs : build échoué, XML ancien, sélection vide ou exclusions silencieuses. Portée : verdict entier. Détection existante : inventaires et identités Windows recoupés. Mitigation : sorties/runs distincts, propagation des erreurs et rapprochement par nom. Gap initial : chaîne Linux source/configuration/binaire/découverte/JUnit non démontrée.
2. **ASan absent ou neutralisé : 5 × 4 = 20.** Déclencheurs : flags manquants, environnement permissif, runtime incorrect. Portée : obligations mémoire CPU/GL. Détection : vérificateur sain/UAF existant et garde fail-closed. Mitigation : sonde positive et faute causale avant campagne, environnement contrôlé. Gap initial : module refusant explicitement Linux, preuve d'instrumentation manquante.
3. **Faux succès graphique : 4 × 4 = 16.** Déclencheurs : renderer supposé, mock, contexte absent, deux images également vides. Portée : OpenGL et pixels. Détection : contexte réel, verrou artist_opengl, pixels fixes non vides. Mitigation : identifier le contexte du test, vérifier qualité et négatifs discriminants. Gap initial : renderer/campagnes Linux non qualifiés.

Réévaluer à l'intégration, aux surprises et aux changements de candidat. Ne pas modifier la politique de modèles pour cette matrice QA.

## Obligations explicites et scénarios

### O1 — Environnement et isolation

Nominal : versions Ubuntu/WSL, compilateur/STL/ABI, Python/Conan/CMake/Ninja et dépendances consignées ; destination et checkout propres identifiés. Négatif : cache Windows ou toolchain incompatible ne provoque pas de repli silencieux. Preuves : manifestes, CMakePresets/conan profile et sonde réelle. État initial GAP.

### O2 — Contrats C++23

Nominal : std::format et std::stacktrace compilent ET lient avec la sonde cmake/CheckCxx23.cmake ; contrats positifs existants acceptés. Négatif : toolchain incapable rejetée avec diagnostic ciblé ; contrats négatifs rejetés pour leur motif après témoin positif. La présence d'un numéro GCC n'est pas une preuve. État initial GAP.

### O3 — Population CPU

Nominal : découverte et exécution non vides, assertions Debug conservées, noms sélectionnés et JUnit concordants. Négatif : zéro test, test requis absent ou skip requis interdit un PASS. Expliquer ajouts/retraits/renommages par nom et obligation, sans compensation par cardinalité. Preuve : ARTIST/tests/CMakeLists.txt, ArtistLib_tests, label unit. État initial GAP.

### O4 — OpenGL réel logiciel

Nominal : Xvfb/Mesa llvmpipe, fournisseur/renderer/version/profil effectivement utilisés par les tests, fixtures et verrou existants conservés. Négatif : contexte absent ou renderer différent de celui revendiqué invalide la preuve ; jamais remplacer par un mock. Preuve : ArtistLib_component_tests et contexte journalisé. Verrou inter-worktrees si ressource partagée ; le seul verrou CTest d'un arbre ne couvre pas les autres arbres. État initial GAP.

### O5 — Pixels et consommateur

Nominal : routes legacy/préparée concordantes, RGBA8 linéaire et points fixes non vides, résultat variant avec la valeur. Réutiliser PreparedFloatUniformRenderTests.cpp:LegacyAndPreparedMatchFixedNonemptyPixels et le consommateur ReflectedProgramGuardCrossTests.cpp. Négatifs ciblés : omission d'update ou absence de draw détectée pour la raison attendue ; ne pas élargir tolérances/références pour Mesa. Les compteurs de lookup ne prouvent pas les pixels. État initial GAP.

### O6 — ASan

Nominal : arbre Debug ASan distinct, instrumentation effective des TU concernées, contrôle sain code 0, faute heap-use-after-free code 86 et diagnostic attendu, puis populations CPU/GL requises. Négatifs : instrumentation absente, diagnostic absent, mauvais code, options héritées hostiles dont exitcode=0/redirection/activation différée détectés ou normalisés avant les enfants. Réutiliser cmake/AddressSanitizer.cmake et scripts/verify_asan.py. Contrats PowerShell de restauration conservés sous Windows ; Linux peut isoler l'environnement dans un processus enfant sans imiter inutilement PowerShell. Dépendances/driver non instrumentés et detect_leaks=0 explicités ; pas de preuve d'absence de fuites. État initial blocked par la garde Windows-only, à corriger en TDD.

### O7 — Identité, fraîcheur et propagation d'échec

Nominal : run neuf identifié, build réussi, identités source/config/binaire, commandes/logs/XML cohérents. Négatifs : XML préexistant, ancien binaire après échec de compilation, code enfant perdu ou changement de candidat invalide la preuve. CTest JSON/JUnit et bibliothèque standard, pas nouveau parser de console GoogleTest. État initial GAP.

### O8 — Sensibilité des négatifs et TDD

Nominal : témoin positif, modification unique, reconstruction réussie et cohérente, échec causal, restauration, GREEN. Négatifs : crash arbitraire, panne d'invocation, compilation échouée, objet inline divergent ou mutant non reconstruit ne valent pas RED. Reconstruction de toutes les TU affectées par un inline ; identité du binaire vérifiée. Les mutations de calibration sont distinctes des tests positifs et ne reconstruisent pas un RED historique manquant. État initial GAP.

### O9 — Remise QA et intégration

Nominal : lecture locale permise, remise réellement récupérée, relue, persistée et commitée avant publication WSL/worktrees auteurs. Négatif : refus signalé une fois ; absence de récupération/commit ne devient pas une réussite. La réception du plan est désormais acquise par API native read-only ; le commit est encore une barrière à vérifier au moment de publication. L'enforcement exhaustif reste INCONCLUSIVE.

### O10 — Mutualisation et compatibilité Windows

Nominal : mêmes obligations, outils et données réutilisés avec adaptateurs fins ; modifications communes vérifiées Windows normal/ASan au périmètre concerné. Négatif : duplication divergente d'un oracle ou affaiblissement détecté par tests/revue. État initial GAP ; exigence explicite du mandat.

## Registre inverse et limites de couverture

- artist_check_cxx23 → O1/O2 ; présence initiale ne vaut pas compilation Linux.
- Découverte CTest CPU/GL → O3/O4/O7 ; enregistrement ne vaut pas exécution.
- LegacyAndPreparedMatchFixedNonemptyPixels → O5/O8 ; assertions existantes ne valent pas résultat Linux.
- PreparedUpdatesDoNotResolveNativeLocationsAgain → contrat préparé O3/O4 ; pas oracle image ou mesure de performance.
- Canari et vérificateur ASan → O6/O8 ; pas substitut aux suites moteur.
- JUnit Windows historiques → obligations Windows au candidat indiqué ; jamais preuve Linux.
- Nouveaux tests/artefacts → obligation précise et défaut plausible à renseigner par l'auteur/runner. Examiner les orphelins lors du rapprochement ; ne pas déclarer leur absence sans inventaire.

## Runner et barrières

Le lead est le runner distinct des profils QA. Les auteurs gardent leur tranche RED/GREEN/REFACTOR ; le generator n'est pas leur substitut.

1. Figer environnement/candidat ; installer Conan2 avant configuration CMake. Sorties normales/ASan/coverage distinctes ; pas de cache binaire Windows utilisé comme Linux.
2. Vérifier sonde et canari, construire ; découvrir les noms, puis CPU et OpenGL requis avec --no-tests=error et JUnit uniques.
3. Conserver pour chaque preuve : HEAD/delta, versions, identités pertinentes, source/date, commande exacte, runner, attendu/observé, découverts/sélectionnés/exécutés/échoués/skipped et logs.
4. Intégrer les producteurs build/ASan et contrôler le checkout Linux avant OpenGL/CI. La fin d'auteur ne libère pas les consommateurs ; clôture lead seulement, publication explicite.
5. Gaps sur candidat intégré/exécuté ; compléments acceptés par lead, persistés/exécutés/intégrés avant audit fresh. L'audit reste deferred tant que hors-board, profil natif et restitution ne sont pas qualifiés ; pas de fausse complétion d'un audit externe.
6. Reproduction CI propre avec les mêmes commandes, résultats conservés même en échec. Les services Sonar/Codecov facultatifs ne bloquent pas la baseline obligatoire. Un workflow linté n'est pas un run distant.

## Budget de planification et sortie

Proposition qa-planner : 120 minutes de préparation, dont 30 de setup ; reste 90 minutes réparti à 75 % planifié (identités/populations, ASan, pixels/remise) et 25 % buffer. Estimation de préparation seulement, pas durée de build/installation/campagne ni garantie de temps. En cas de dépassement, retirer d'abord les contrôles secondaires sans affaiblir les obligations critiques.

Sortie : populations réconciliées par nom et obligation ; critères matériels vérifiés ou écarts explicitement disposés avec motif/propriétaire/impact ; aucun zéro test, skip requis, résultat périmé ou négatif non causal présenté comme PASS. Les limites non exécutées restent INCONCLUSIVE/NOT_RUN. PASS partiel n'est pas PASS global ; FAIL reproduit reste prioritaire.

Au moment de cette acceptation documentaire, aucune exécution Linux n'est prouvée. Parité Linux : INCONCLUSIVE, à établir par les tranches autorisées après le commit du présent plan.
