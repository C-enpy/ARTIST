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

## Complément accepté — classification Attribute et route moderne conditionnelle

Date : 2026-10-28. Ce complément précise la correction bornée par `attribute-correction-contract.md`, commité à `38d03b7`. Il ne réécrit pas l'état historique du plan initial : normal et ASan ont depuis exécuté les mêmes 581 tests sous WSL puis en CI propre, avec 506 CPU réussis et 72/75 OpenGL, trois failures et aucun skip. La parité actuelle reste **FAIL**.

### Provenance, admission et limites

Proposition texte du qa-planner pool-8, tâche `task_mvsg0uhi_001w_zcu4t6`, session native `ses_e7c3ef5a5ffe79FU279WXBegrw`. Remise complète réellement récupérée par le lead dans le message assistant `msg_183c1b1ff001EPUjNbjIEmWaFA`, finish stop, 18573 caractères. Une réponse ultérieure ne faisait que renvoyer à cette remise : elle ne la remplace pas.

Original public archivé : `out/logs/mission-linux-qa/qa-attribute-final-msg_183c1b1ff001EPUjNbjIEmWaFA.md`, SHA256 `0c7aea3184d8f3a4020a29f8588026e8bb3bf17db232889624d2c39ca39507ad` (19187 octets UTF-8). Le présent texte est la synthèse adoptée et persistée par le lead après relecture, pas une exécution du qa-planner.

Routage qa-planner et répertoire ARTIST constatés via API native, sans ajout de permission de session ; lectures locales et message effectivement reçus. La récupération automatique Ensemble a échoué, puis le lead a retrouvé la réponse publique finale via l'API native en lecture seule, sans relancer le worker, sans modifier sa configuration ou la base du runtime. Ces faits ne prouvent pas une sandbox ni un enforcement exhaustif des permissions.

Les capacités, la classification et le staging ci-dessous sont des **obligations à tester**, pas des résultats déjà acquis. La sonde préalable ne remplace pas le RED moteur. Aucun auteur admis avant commit de ce complément.

### Exigences explicites et scénarios

- **A1 — clés exactes.** Publier exactement les noms legacy indépendamment énumérés du même programme. Si la route moderne ajoute `[0]`, la clé legacy reste inchangée. Aucun alias supplémentaire ; un nom absent ne devient pas un lookup admis.
- **A2 — capacité réelle.** Tester core4.3 sans extension et3.3 avec extension, fonctions requises chargées. Négatifs :3.3 sans extension malgré pointeurs présents, puis chaque fonction requise absente. Zéro appel moderne et comportement legacy conservé dans ces négatifs. Pointeur ou token seul ne suffit pas.
- **A3 — association.** Même programme et interface PROGRAM_INPUT, association par nom, exact ou unique ajout final `[0]`. Permuter l'énumération moderne ; une location identique ne justifie pas un nom incompatible. Rejeter `[00]`, `[1]`, suffixe arbitraire ou double ajout plutôt que joindre par ordre/location.
- **A4 — réponses valides.** Après sélection de la route moderne : index absent/invalide, propriétés incomplètes, longueur/NUL invalides, nom tronqué/incompatible, type/taille/location contradictoires doivent faire échouer le staging, sans fallback silencieux. Distinguer une fonction manquante (A2, aucun appel) d'un GL_INVALID_INDEX après appel disponible (A4, erreur). Tester capacité minimale valide, longueur capacité−1, égale/supérieure à capacité, NUL absent/interne et cohérence NAME_LENGTH ; tailles0/négative/différente, type différent, location négative/différente. Ne pas ajouter une requête inutile pour inventer une borne d'index. Diagnostics précis figés dans les tests avant code.
- **A5 — classification.** Scalaire vec3/size1 : isGLArray false et upload admis. Array1 de même type/taille/location : true ; array2 : true. Ni size1 ni la localisation `[0]` ne prouvent la scalarité.
- **A6 — refus natif.** Après préparation volontaire des ressources et snapshot : arrays1/2 lèvent ARRAY_NOT_SUPPORTED avant changement des buffers cible/leurre, binding et état natif pertinents. Utiliser type valide et valeur non nulle pour éviter un autre diagnostic. Scalaire : contenu cible exact, leurre intact et consommateur existant conservé. Ce scénario ne promet pas le rollback du miroir CPU du wrapper, qui stage déjà sa valeur avant le Setter.
- **A7 — staging.** Ancienne map non vide, première entrée candidate valide puis seconde moderne invalide : mêmes clés, adresses des pointees, métadonnées et valeurs anciennes ; aucun préfixe nouveau publié. Succès publie toute la génération ; count0 conserve le commit vide. Une réflexion valide après échec reste possible.
- **A8 — frontière.** Réflexion moderne au Reader seulement, pas de nouvelle requête aux set/consommations. Mêmes interfaces, ownership et oracles de rendu. Aucune assertion nouvelle de conservation de l'ancienne valeur CPU du wrapper après rejet ; le Setter direct garde ses contrats propres.

### Inférences de test, distinctes des garanties publiques

Réutiliser les mocks existants et le pattern fp64 de capacité/pointeurs explicitement configurés et restaurés. Une configuration omise ne doit jamais déclencher GLEW réel involontairement. Réfléchir deux fois le même programme conserve la description, pas nécessairement les pointees : un commit réussi peut remplacer la génération. Programme/contexte appropriés et absence de relink/destruction concurrent sont des préconditions, pas une nouvelle garantie de concurrence. Pas de campagne sécurité/accessibilité ajoutée.

### Trois risques prioritaires — matrice QA distincte du risque PM

1. **Array1 classé scalaire ou simple relâchement d'orthographe : impact4 × probabilité4 =16.** Déclencheur : nom legacy nu et size1 ; portée : refus et buffers du consommateur Attribute. Détection/mitigation existantes : sonde moderne discriminante, Setter refusant lorsque son bit est correct. Gap : RED causal sous clé native et refus avant mutation à établir.
2. **Route moderne non supportée ou réponse incohérente admise :4 ×4 =16.** Déclencheur : pointeurs sans capacité, fonction absente ou données malformées ; portée : réflexion, crash ou mauvaise classification. Détection/mitigation : patterns GLEW/mocks et contrat de rejet. Gap : interposition complète, négatifs de gate/réponse à écrire et exécuter.
3. **Publication partielle et anciens emprunts invalidés après erreur tardive :5 ×3 =15.** Déclencheur : première entrée staged puis faute suivante ; portée : map et consommateurs empruntés. Détection/mitigation : staging et tests legacy de conservation existants. Gap : erreurs modernes tardives et transition échec→succès non qualifiées.

### RED obligatoire avant correction

Réutiliser les shaders/fixtures Attribute légaux, établir compile/link/contexte, énumérer le nom legacy d'array1 indépendamment du Reader. Vérifier publication sous cette seule clé avec champs exacts et sans alias, puis exiger isGLArray true. **Avant correctif sur Mesa, la clé et les préconditions doivent passer, et la classification échouer.** Une clé codée en dur `[0]`, une compilation échouée ou un contexte absent ne constituent pas ce RED.

Garder un témoin scalaire comparable et array2. Après GREEN, vérifier refus avant mutations natives. Les trois FAIL initiaux restent archivés ; disposer leurs assertions d'orthographe en remplaçant l'hypothèse fournisseur par la clé native indépendamment observée, sans supprimer les obligations de classification et de refus. Pas d'historique réécrit.

### Traçabilité bidirectionnelle ciblée

- Préflight NativeCapabilitiesAndArraySpelling → A1/A5 : observation utile, pas blocage de conformité fournisseur hors consommation moteur.
- NativeReaderArrayOneRegression → A1/A5 : avant modification échoue trop tôt sur clé, d'où le nouveau RED causal.
- NativeArraysDescribeThenRefuseWithoutTouchingDecoy → A5/A6 : préserver leurre/binding et compléter les observations pertinentes de cible au snapshot approprié.
- ExactNaturalLongNameContract et FaultInjection_InvalidLengthAndTerminator → A1/A4 legacy ; ne prouvent pas la nouvelle réponse moderne.
- FaultInjection_ExactArraySuffixAndUnsupportedEnumDescription → A2/A5/A8 pour fallback configuré explicitement.
- DuplicateNamePreservesPreviousGeneration et LateInvalidLocationKeepsPreviousBorrow → A7 legacy, à compléter pour erreur moderne tardive.
- SuccessRetiresMissingNamesAndOldValues et ZeroCountCommitsEmptyGeneration → A7, transitions conservées.
- Tests typés/transform feedback/pixels → A6/A8, pas preuve des gates.

À l'intégration : rattacher chaque nouveau cas A1–A8 à son défaut plausible et à sa preuve, signaler les orphelins et les preuves périmées par candidat. Ne pas dupliquer les négatifs sans frontière distincte ni déclarer « aucun orphelin » sans inventaire.

### Ordre, ownership et disposition QA

1. Lead : récupération réelle, relecture, persistance et commit avant worktree auteur ; réservation exacte du Reader, de ses tests, du mock `ARTIST/tests/mock/opengl/glFunctionMock.hpp` et des tests composants Attribute. Aucune écriture dans workflow/helpers du producteur indépendant.
2. Auteur OpenGL unique : RED causal Mesa/CPU, correction minimale, GREEN ciblé, reconstruction des TU touchées par les headers. Sorties et bail GPU exclusifs ; aucun build dans l'arbre actif de pool-5.
3. Lead : shutdown/merge, diff intégré relu, candidat commité et vrai résultat disponible avant consommateurs. Refaire les contrôles nécessaires Windows/NVIDIA et Linux/Mesa normal/ASan ; même population rapprochée par nom, pas quota figé581 empêchant les nouveaux tests.
4. Après intégration : test-engineer complète uniquement les gaps avec oracles et propositions KEEP/MODIFY/REJECT/DEFER, sans production ni rôle de runner. Le lead persiste, relit et exécute ; pas de substitution au RED auteur.
5. Revue indépendante sensible ciblée sur gates, GLEW, longueurs et staging ; test-reviewer fresh hors board d'implémentation seulement si profil/permissions/contexte/restitution sont effectivement qualifiés. Corrections au propriétaire puis ré-audit. Une voie native manquante reste BLOCKED, sans substitution générique, grant ou audit fictif.

La QA proportionnée ne supprime pas les exigences du projet : cette disposition conserve les étapes obligatoires avec des contrôles ciblés, sans modifier scores/politique/provider ni créer de harnais ou d'audit généraliste. La barrière hors board reste à qualifier au bon jalon ; sa réalisation n'est pas annoncée ici.

Conan2/C++23 qualifiés, canari, renderer réel et builds distincts restent des préconditions. Les commandes doivent venir des CMake/presets/lanceurs réellement intégrés : ne pas dépendre d'un helper de branche non intégré. Zéro test/skip requis n'est pas une réussite.

### Budget et sortie

Allocation relative100unités, pas estimation de temps : setup25 ; RED/classification/refus17 ; gates/réponses20 ; staging/transitions10 ; rapprochement9,25 ; buffer18,75. Après setup,56,25planifiées et18,75buffer =75%/25% des75restantes. Aucune durée de build ni budget monétaire inventés.

Blocages avant annonce GREEN/parité corrigée : RED causal non exécuté ; gates/réponses/staging non qualifiés ; correctif non intégré ; non-régression normale/ASan manquante. Ces gaps ne bloquent pas la remise du plan.

Pilote3.3 sans capacité moderne à nom ambigu : gap global conservé, disposition du lead avant ajout aux cibles requises, jamais acceptation implicite/skip/PASS. Hors périmètre : Release, UBSan, fuites exhaustives, performance, nouveau framework/driver matériel et minimum global4.3. Diagnostics et interposition locale sont des choix auteur à figer avant code ; aucune API nouvelle.

Sortie : A1–A8 liés à preuves discriminantes, RED conservé, GREEN sur candidat intégré reconstruit, populations rapprochées, anciensFAIL et dispositions conservés. PASS uniquement au critère exécuté ; FAIL prioritaire sur écart reproduit ; INCONCLUSIVE pour preuve/précondition manquante. Aucune validation d'implémentation ni enforcement exhaustif des permissions n'est déduite du présent complément.
