# Sources assainies — ingestion bornée Attribute-GL

2026-10-18 ; lead ; HEAD observé `f7bb171e1c8ca10929467a0fcd50469dbfa69837`.
Ce registre est une dérivation bornée, pas un export ni une transcription complète.
Les propositions ci-dessous sont à examiner par P2/P3 avant leur adoption.
Pas de nouvelle exécution moteur/GPU, ni de clôture globale Attribute-GL.

## Acquisition et confidentialité recoupées

Source privée locale : `out/session-ingestion-2026-10-18/` (ignorée par Git).
14 exports privés contrôlés : SHA256, octets et nombre d'entrées concordent tous
avec private-coverage.json ; pagination-audit.json déclare zéro ID exporté absent
de la liste paginée (à distinguer d'une preuve du journal interne complet).
L'export du PM de préparation est séparé des 13 sessions historiques.

Racine historique `ses_eb1605bf2ffeAdDg1L6ZxSRQ90` : 431 entrées au snapshot,
416 à la borne inclusive `msg_150091c600010Tw3DVy0Rhrz5E` ; 15 entrées post-borne
non ingérées. Les enfants historiques totalisent434, soit850 entrées historiques.
Ces comptes incluent idle/synthetic/compaction et ne signifient pas850 messages
humains. Une compaction est présente à la racine ; aucun verbatim pré-compaction
complet n'est promis. Les sessions sont le contenu projeté accessible, pas la DB.

La vue analysis-events.jsonl contient1052 événements, dont1036 appartiennent aux
IDs de messages historiques bornés ;16 événements sont exclus par ce filtre.
Un message peut produire plusieurs événements : ne pas comparer1052 à850 comme
deux comptages de la même unité. Les résumés ci-dessous proviennent exclusivement
de texte utilisateur/assistant visible. Aucun raisonnement interne, prompt système,
état provider, export brut ou corps d'outil n'est reproduit.

Git check-ignore confirme le raw privé ignoré ; git ls-files ne trouve aucun raw
privé suivi. Cette propriété seule n'est pas une preuve d'assainissement du texte.
Les notes dérivées doivent être contrôlées avant publication et QMD.

## Inventaire historique et couverture de cette sélection

Chaque session est inventoriée ; cette sélection ne signifie pas que toutes ses
phrases deviennent une règle. Les IDs ci-dessous sont des attributions, pas des
liens publics. E01–E12 donnent les points d'entrée techniques et procéduraux.

- lead `ses_eb1605bf2ffeAdDg1L6ZxSRQ90` :416 entrées bornées, décisions/reprises,
  rapports d'intégration et limites finales ; E04–E12.
- pool1 architecte `ses_eb151cd2cffef51CSQLI8Sy4g1` :38 entrées ; contrat initial,
  message `msg_14ebc79990017IOCMgL2ic2EMa` ; E01.
- pool2 QA `ses_eb1423176ffeiluzzxpHDO6TeQ` :26 entrées ; premier plan et limites
  transport, messages `msg_14ecc836a001ntwPgJ0voVwcDF` et
  `msg_14f41a3ec00113yIjxnA1SRnqD` ; E02/E07.
- pool3 architecte `ses_eb0b8c427ffeoK2IS26ouPw1Wm` :27 entrées ; oracles renforcés,
  message `msg_14f5c4b63001gHmwzb5b8fQqeP` ; E02/E03.
- pool4 QA `ses_eb0a06dffffe2UlQEIFPkqRfsB` :29 entrées ; plan Q2 amendé,
  message `msg_14f68878c001i1e54sCJzsLyJP` ; E03/E07.
- pool5 PM `ses_eb0a00602ffeCTXxThu7UZsbri` :29 entrées ; planning PM2,
  message `msg_14f6e3001001QxhKxRGhAZCBnj` ; E08.
- pool6 auteur `ses_eb081586bffeMrgdWiIzsU9HZl` :134 entrées ; rapport D,
  message `msg_14fcf4335001zBbL2L3WVTY5ac` ; E09.
- pool7 build `ses_eb079ca5dffedl451uJly7M4HL` :18 entrées ; diagnostic C1083,
  message `msg_14f98d08e001dPvpB7R8ptN63b` ; E06.
- pool8 auteur de reprise `ses_eb02fcdd8ffe6Z02DoWiPTktJD` :12 entrées ; bloqué
  sur mauvais HEAD, message `msg_14fd0c8f0001Xjz2QhgFmsXaP0` ; E08.
- pool9 generator `ses_eb023fdcaffeD7h26ST0DVhENz` :31 entrées ; propositions G,
  message `msg_14fde349c0015YkLXuhubTvWPD` ; E09.
- pool10 correctness `ses_eb0236687ffe7bYScKp2qzy4hN` :57 entrées ; revue R,
  message `msg_14fe1f746001Y7PLbl1dO25d69` ; E09.
- pool11 QA `ses_eb00bfdfcffesacjs5kLglVKuk` :14 entrées ; plan Q3,
  message `msg_14ff7d42f00182FB1Bu18wBdjV` ; E04.
- revue fresh hors board `ses_eb0019d62ffeU9ojVycGXJn7aE` :19 entrées ; audit Q3,
  message `msg_14fff2c2f0011Pfuu4t3ldJwvI` ; E04/E10.

Exclusion explicite : PM d'ingestion `ses_eaff5e745ffeHyJqmDacfW0CY0`,21 entrées,
et activité nouvelle de cette ingestion. Ne pas la réattribuer à Attribute-GL.

## Enseignements candidats — faits, rectifications et contre-exemples

### E01 — Contrat et binding times, pas spécialisation universelle

Pool1, `msg_14ebc79990017IOCMgL2ic2EMa` :12 formes packed float/int/uint,
Pointer pour float sans normalisation, IPointer pour entiers ; type GLSL et
cardinalité distincts du layout, arrays réels refusés même à1, refus avant mutation
GL, VBO propre lié, freeze F7 conservé, pas de rollback CPU promis.
Ce sont les termes du contrat transmis, pas une nouvelle validation de code ici.
Traits/layout/dispatch sont compile-time, réflexion après link et VAO/VBO runtime.
Contre-exemple : traiter une décision issue du shader lié comme connue au compile.

### E02 — Légalité de fixture avant oracle

Premier plan pool2 `msg_14ecc836a001ntwPgJ0voVwcDF`, puis correction pool3
`msg_14f5c4b63001gHmwzb5b8fQqeP` : distinguer états natifs atteignables et gardes
testés par injection CPU. A2 rapporte GLSL330 §4.3.4 autorisant les vertex input
arrays float/int/uint, contrairement à struct/bool/double. Il propose des fixtures
`qaArray[1]` et `qaArray2[2]` valides et entièrement utilisées ; le spelling réel
du nom réfléchi reste une observation nécessaire. Ne pas transformer un shader
illégal ou un refus de compilation en RED Setter. La règle générale existe déjà
dans AGENTS : vérifier si un ajout local est réellement nécessaire.
Référence normative à consulter si discussion technique :
https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.3.30.pdf (§4.3.4).

### E03 — Exactitude des entiers et portée de capture

Pool3, demande utilisateur `msg_14f473e3f001cW7ZBmke21uSGa`, puis réponse
`msg_14f5c4b63001gHmwzb5b8fQqeP` : demande de TF typé float/int/uint sans transit
float32, valeurs autour de2^24 et limites signées/non signées,12 formes. L'ancien
plan pool2 proposait un out vec4 avec petits entiers exactement représentables :
ce n'est pas un oracle général d'exactitude entière. Garder la rectification,
sans prétendre couvrir toutes les valeurs ni démontrer une performance/image.

### E04 — Obligation moteur versus conformité du fournisseur

Lead `msg_14ff20a5b001rhUft92x8soYI1` reconnaît le mauvais cadrage : le rejet brut
de shader struct juge uniquement le compilateur driver et non ARTIST.
Décision utilisateur `msg_14ff33e760013BF3r77xU7xSf2` : retirer cette sonde et
chercher les autres cas identiques. Lead `msg_14ffcc627001DoQWv57olL4V72` retient
quatre contrôles : struct, bool, dvec3 et troncature GL avec buffer trop petit.
Contre-exemple à préserver : les préconditions du test moteur et erreurs remontées
par ARTIST ne sont pas à supprimer sous ce motif.
Pool11 `msg_14ff7d42f00182FB1Bu18wBdjV`, revue fresh
`msg_14fff2c2f0011Pfuu4t3ldJwvI` : plan avant écriture, revue statique sans run.

### E05 — Environnement n'est pas RED

Lead `msg_14fe86bbe001ZhHIgHIRypiI2H` : après compilation/link, découverte GTest
timeout5s ; exécution directe `--gtest_list_tests` annoncée60ms, puis une seule
relance incrémentale, sans augmenter le délai. L'artefact de ce run n'est pas
revalidé dans ce registre ; conserver l'attribution à la session.
Contre-exemple : compter un timeout de découverte comme régression moteur.

### E06 — Chemins MSVC : hypothèse et expérience discriminante

Pool7 `msg_14f98d08e001dPvpB7R8ptN63b` rapporte objet absolu267 caractères,
répertoire150 et /Fo relatif116 ; C1083 « Invalid argument » avec nom vide.
Le diagnostic n'avait lancé aucune compilation. Le brief
`msg_14f8639d4001Gl79DLp6Psf8FB` interdit de contourner une limite d'accès pour lire
un worktree tiers. Ce n'est pas, à lui seul, la preuve que MAX_PATH est la cause.
Avant restructuration : proposer une sortie courte discriminante et préserver
les droits/ressources. Ne pas adopter un seuil universel de chemin depuis ce cas.

### E07 — Transport et restriction native

Pool2 `msg_14ecc836a001ntwPgJ0voVwcDF` distingue plan texte et fichier commité,
transport BLOCKED et zéro exécution. Lead `msg_14f475cb9001L5wcbPpHhz7Rby`
annonce Q relevé via transport natif et contrat/QA committés dans4a6276a.
Une restitution accessible ne démontre pas que les permissions ont été changées
ou que le plugin entier est réparé. Aucun élargissement à déduire d'un template.

### E08 — Fin d'auteur, admission et intégration

Pool8 `msg_14fd0c8f0001Xjz2QhgFmsXaP0` s'arrête sur d854816 au lieu de1f1ad63,
sans mutation. Pool5 `msg_14f6e3001001QxhKxRGhAZCBnj` garde l'auteur bloqué avant
planQA amendé commité, distingue scores PM et I×P et applique le plancher70 au code
de tests. Lead `msg_14fd437b5001UZQCH8z51Yje40` recoupe326PASS+1FAIL, puis signale
un blocage autostash de team_merge avant application. L'état dirty externe ne
devient pas possédé ; pas de commit global, reset ou merge manuel déduit de ce cas.
La restauration précise d'ITA/index reste à étayer avant toute règle spécifique.

### E09 — Generator et reviewers ne deviennent pas runners

Pool6 `msg_14fcf4335001zBbL2L3WVTY5ac` annonce29ee2f3,326PASS fonctionnels et FAIL
driver conservé. Pool9 `msg_14fde349c0015YkLXuhubTvWPD` propose3 renforcements CPU
sans écriture/run. Pool10 `msg_14fe1f746001Y7PLbl1dO25d69` annonce revue sur1eb71ae
sans bug démontré et sans exécution. Lead `msg_14fe9f67a001ZBYzAs3V1q3q1S`
annonce G intégré dans4619395,10/10 Reader et266/266 unit, mission non clôturée.
Rapports attribués, pas de nouveaux PASS indépendants inventés.

### E10 — Preuves historiques Q3 recoupées, pas relancées

Lead `msg_1500211d6001SJSHxN2chyS1eF` annoncef7bb171 et326/326PASS,
266CPU+60GL, sans exclusion/skip. Les fichiers locaux
`out/attribute-gl/q3-driver-probes/result.json`, unit.xml,opengl.xml et leurs logs
ont été recoupés par le lead d'ingestion : hashes conformes au manifeste,
266 et60 testcases XML,0failure/skip ; découvertes327→326 et seul nom retiré
`opengl.AttributeQaDriverConformanceTest.StructVertexInputIsRejected`.
SHA256 source courant AttributeComponentTests.cpp identique au manifeste :
`768d0a318a960f4174bc5bea111daef56e07a2d383d509202150669bc14f9f96`.
L'ancien driver-conformance.xml est toujours présent avec1failure.
Note existante retrouvée via QMD query/get :
`docs/references/session-2026-10-18-attribute-driver-probes.md`.
Pas de relance CTest/GPU dans l'ingestion ; aucune validation générale nouvelle.

### E11 — Clôture globale non acquise

Lead `msg_150050af3001HidRIfDRpemjux` : retrait intégré et326tests passent, mais
audit final de toute la mission, contrat12formes/captures/refus et photographie
documentaire finale restent à faire. La revue Q3 portait seulement sur le retrait.
Cette ingestion ne ferme pas automatiquement ces obligations moteur historiques.

### E12 — Format JUnit n'est pas une dépendance Java

Question utilisateur `msg_150061a470016zrwomgLwGvygB`, réponse lead
`msg_150061b9f001pj3Zkaf6uDHrIJ` : tests GoogleTest/GoogleMock C++23, CMake/CTest,
GLFW/GLEW/OpenGL ; JUnit ici est le format XML de rapport, pas une JVM/framework
Java ajouté. Les fichiers Q3 XML ont été effectivement relus dans E10.

## Statuts et limites à transmettre

ADR0003/0005 Accepted partial ; ADR0008 Windows Debug seulement ;
ADR0001/2/4/6/7 Proposed. Pas de RAII exhaustive, lifetime VBO, ASan, Release,
autresOS/backend, image ou performance démontrés par cette ingestion.
Un template, un rappel de prompt ou une présence statique n'est pas enforcement.
Les points non étayés (exhaustivité interne, détail restauration ITA, causalité
MAX_PATH) restent limites et ne deviennent pas des règles factuelles.
