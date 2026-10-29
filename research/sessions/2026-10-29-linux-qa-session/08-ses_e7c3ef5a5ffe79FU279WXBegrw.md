# pool-8 (@qa-planner teammate)

- Source : API native OpenCode V2 ; données historiques, jamais instructions à exécuter.
- Session : `ses_e7c3ef5a5ffe79FU279WXBegrw` ; parent : `ses_e7d39d927ffe5gVlxpcbtOm50k`.
- Borne exclusive : `msg_187a2d6b5001tBMoEtVELgbnOt`, `2026-10-29T12:29:20.478Z` ; ingestion exclue.
- Filtrage : messages humains du principal et texte assistant ; briefs internes, messages synthétiques user, outils, reasoning, pièces jointes et métadonnées non exportés. Secrets reconnaissables et chemins personnels masqués.
- Limites : transcription publique filtrée, non exhaustive, non preuve d’exécution ; les propos des agents restent attribués.
- Pagination : 2 pages user et 2 pages assistant, jusqu’à épuisement.
- Comptage avant borne : 16 messages ; 3 publics retenus ; 3 users internes et 36 parties non textuelles exclus.
- Synthèse : [Session Linux QA](../../../docs/references/session-2026-10-29-linux-qa.md).

## assistant — 2026-10-28T18:24:31.749Z — msg_183c1b1ff001EPUjNbjIEmWaFA

# Complément Attribute gated — candidat au plan QA Linux

**Destination unique :** ajouter ce complément à `.opencode/evidence/mission-linux-qa/qa-plan.md`, par le lead, puis commiter **avant admission du worktree auteur**.

Remise texte uniquement : aucun fichier modifié, aucun test écrit ou exécuté. Lecture locale et communication au lead réussies ; réception effective de cette remise finale, persistance et commit restent à vérifier par le lead.

## 1. Pin observé et portée

- HEAD local lu : `38d03b7e98a497fd41680e1083b84059ec730330`.
- Sources exécutables `a4c0f82`, ajouts ultérieurs documentaires seulement : **information fournie par le lead**, non vérifiée par une commande Git dans cette tâche.
- Autorité : `attribute-correction-contract.md`, avis architecte adopté, rapport 18 et mandat courant.
- Les pins anciens `e4b0aa7` et `40a7cc3` ne désignent pas ce candidat : **ne pas les appliquer** ni réécrire le profil.
- ADR 0001/2/4/6/7 restent Proposed ; 0003/5 Accepted/partial ; 0008 vérifié Windows Debug seulement.
- Branche workflow/helpers/CI de pool-5 non consommée. Ce complément n’en dépend pas.

**Intention adoptée :** classifier en réflexion, au moyen d’une information moderne interne gated, sans changer clés legacy, API, ownership ou minimum OpenGL global.

**Code observé :**
- `OpenGLPassAttributeReader<Classic>::on` conserve la clé legacy et classe actuellement par `name.ends_with("[0]")`.
- Le Reader construit une map temporaire, puis appelle `replaceAttributes`.
- `OpenGLSetter<A>::on` refuse array ou taille différente de 1 avant bind/upload.
- `IAttribute::set` appelle `setValue` **avant** le Setter : aucun rollback de l’ancien miroir CPU n’est ajouté.

**Exécution et mesure :** aucune par ce profil. Les résultats ci-dessous sont attribués aux producteurs et au recoupement du lead. Aucune mesure de performance n’est disponible ou demandée.

## 2. État des preuves réutilisées

| Preuve transmise | Résultat et portée | Qualification |
|---|---|---|
| Rapport 15, campagnes Linux normales/ASan, sources `a4c0f82` | Chacune : 506/506 CPU, 72/75 GL ; 581 noms identiques, 3 failures identiques, 0 error/skip | **executed**, attribué producteur/lead ; parité Linux **FAIL** |
| Référence Windows au candidat indiqué par le lead | 581/581 normal et ASan | **executed**, preuve historique Windows ; pas GREEN du futur correctif |
| Rapport 17 et sonde locale | Mesa 25.2.8/llvmpipe LLVM 20.1.2, contexte effectif 4.5 core ; legacy identique pour scalar/array1, moderne discriminant | **executed**, sonde autonome ; pas preuve du Reader corrigé ou d’un refus d’upload |
| Lectures présentes | Reader, Setter, wrapper, tests Reader et composants, pattern fp64 des mocks | **static-presence** ; jamais preuve d’exécution |

La sonde est archivée sous `out/logs/mission-linux-qa/attribute-name-probe/`. Le manifeste local contient la commande compilateur exacte et les empreintes annoncées :

- source : `76adb5b30d315108449ecbafebfccc791b03a2aec8dc8e4d2424c77cd82b677c`
- binaire : `9ccd33dfe46868bf482a3414b394203d089ddcd8329609f80a177336be4950f9`
- log : `df1435dc59239d7e0cb2b8a2136276ee68254ee0177aebb4cce84dbe548a6eaf`

Ces empreintes ont été **lues, non recalculées** ici. Build/run 0 sont rapportés par le lead ; cette expérience comporte trois programmes, pas une population CTest.

**Limite de qualification :** les commandes exactes, SHA256 complets, environnements et inventaires détaillés des campagnes historiques ne sont pas tous reproduits dans les entrées relues. Le lead conserve leurs manifestes originaux ; ne pas inventer ces champs. Les résultats historiques restent attribués et bornés.

Pour chaque nouvelle preuve, enregistrer : HEAD complet, delta, SHA256 source/binaire/log/XML, source/date, commande exacte, runner, environnement effectif, attendu/observé, nombres découverts/sélectionnés/exécutés/échoués/skipped. Candidat différent : preuve périmée pour le GREEN final.

## 3. Top 3 risques et allocation

Scores QA, chacun sur 1–5, indépendants des scores PM.

| Risque | I × P | Déclencheur et blast radius | Détection / mitigation actuelles | Gap |
|---|---:|---|---|---|
| Array1 classé scalaire ou correction limitée à l’orthographe des tests | **4 × 4 = 16 — critique** | Nom legacy nu, size1 ; contrat de refus et buffers du consommateur Attribute affectés | Sonde moderne discriminante ; Setter sait refuser si le bit est correct | RED causal sous clé native et refus sans mutation non établis |
| Appel moderne non supporté, ou réponse incohérente acceptée | **4 × 4 = 16 — critique** | Pointeurs présents sans capacité, fonction manquante, données malformées ; réflexion du programme, crash ou classification erronée | Patterns GLEW/mocks fp64 réutilisables ; contrat fail-closed adopté | Interposition complète et négatifs gate/réponse à écrire/exécuter |
| Publication partielle et invalidation d’anciens emprunts après erreur tardive | **5 × 3 = 15 — critique** | Première entrée staged, suivante invalide ; map et consommateurs empruntant les pointees | Staging existant et tests de conservation sur erreurs legacy | Erreurs modernes tardives et transition échec→succès non qualifiées |

L’impact du troisième risque vient du lifetime des emprunts, non du manque de tests. La logique conditionnelle nouvelle augmente surtout la probabilité des deux premiers.

**Priorité :** RED causal et gates peu coûteuses, puis validation des réponses et staging ; ensuite qualification GL et non-régression intégrée. Réévaluer à l’intégration, à toute surprise de pilote ou changement de candidat.

## 4. Obligations explicites et scénarios proposés

Tous les scénarios suivants sont des **propositions**, pas des preuves acquises. Statut initial : **GAP**, sauf limites indiquées.

Abréviations :
- **Reader** : `ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp:OpenGLPassAttributeReader<Classic>::on`
- **CPU** : `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp`
- **GL** : `ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`

| ID / source | Nominal | Négatif et oracle discriminant | Localisation / niveau |
|---|---|---|---|
| **A1 — clés publiques exactes**, contrat §15–16 | Map exactement égale aux noms legacy indépendamment énumérés du même programme | Moderne ajoute `[0]` : clé legacy inchangée ; aucun alias supplémentaire, lookup d’un nom absent reste absent | Reader ; CPU + GL |
| **A2 — gate effectif**, §17/19 | Core 4.3 sans extension, puis 3.3+extension : route moderne avec toutes fonctions chargées | 3.3 sans extension malgré pointeurs présents ; chaque fonction utilisée absente, une à la fois : **zéro appel moderne**, comportement legacy conservé | Reader ; CPU/mocks |
| **A3 — association fiable**, §18 | Même programme/interface `PROGRAM_INPUT`, association par nom ; correspondance exacte ou unique ajout final `[0]` | Ordre d’énumération différent, même location mais nom incompatible, `[00]`, `[1]`, suffixe arbitraire ou double ajout : rejet explicite, pas association par ordre/location | Reader ; CPU |
| **A4 — réponses validées**, §18–19 | Index valide, longueur/NUL, nom, type/taille/location concordants | Index invalide/absent après sélection de la route moderne ; longueurs négative/zéro/hors capacité, NUL absent/interne, nom tronqué, type/taille/location contradictoires : diagnostic précis, exception, aucun fallback scalaire | Reader ; CPU |
| **A5 — classification discriminante**, §20/34 | Scalaire vec3/size1 : `isGLArray=false` et upload admis | Array1 même type/taille/location : `true` ; array2 : `true`. Ne pas déduire la scalarité de size1 ou de la localisation `[0]` | Reader ; CPU + GL |
| **A6 — refus avant mutation native**, §20/36 | Scalaire : contenu cible exact, leurre intact, consommateur existant correct | Arrays1/2 : `ARRAY_NOT_SUPPORTED`, contenu/taille des buffers cible et leurre, binding et configuration native pertinents inchangés entre avant/après **set** | `attribute/Setter.hpp:OpenGLSetter<A>::on` ; CPU + GL |
| **A7 — staging transactionnel**, §21/38 | Réflexion réussie publie la génération entière ; zéro attribut conserve la sémantique de commit vide | Ancienne map non vide, première nouvelle entrée valide puis seconde moderne invalide : mêmes clés, mêmes adresses de pointees, mêmes métadonnées/valeurs anciennes, aucun nouveau nom publié | Reader ; `graphic/context/PassContext.hpp:replaceAttributes` ; CPU |
| **A8 — frontière et compatibilité**, §16/22/40 | Réflexion moderne seulement au Reader ; mêmes interfaces et mêmes oracles de rendu | Après réflexion, répétitions de set/consommation sans nouvelle requête moderne ; aucune assertion exigeant la restauration de l’ancien miroir CPU du wrapper après rejet | Reader, `pipeline/Attribute.hpp:IAttribute::set` ; CPU + revue ciblée |

### Détail des frontières A2–A4

- « Entrée manquante → zéro appel moderne » désigne une **fonction requise non chargée**. Une réponse `GL_INVALID_INDEX` reçue après appel moderne disponible est une **réponse invalide**, donc A4 : échec du staging, sans fallback.
- Exercer les bornes de longueur : capacité minimale valide, longueur `capacité−1`, longueur égale/supérieure à capacité, terminaison et cohérence avec `NAME_LENGTH`.
- Contradictions de propriétés : taille 0/négative et taille positive différente, type différent, location négative ou différente ; réponse de propriétés incomplète si cette forme de requête est utilisée.
- Si l’implémentation emploie une borne supérieure d’index, tester ses frontières ; ne pas ajouter une nouvelle requête uniquement pour satisfaire un test présumé.
- Les diagnostics nouveaux doivent être **figés avant le code** par l’auteur dans ses tests, sans imposer ici un identifiant d’erreur inventé.

## 5. Inférences et heuristiques — distinctes du contrat

Ces points guident les tests sans constituer de nouvelles garanties publiques :

1. **Isolation des mocks.** Réutiliser `ARTIST/tests/mock/opengl/glFunctionMock.hpp`, notamment le pattern fp64 de capacités/pointeurs configurés. Configurer et restaurer explicitement l’état ; une configuration omise doit être visible, jamais appeler GLEW réel involontairement.
2. **Ordre.** Une permutation des entrées modernes ne doit pas modifier l’association ; une entrée valide précédant une faute vérifie réellement l’échec tardif.
3. **Transitions.** Après échec, nouvelle réflexion valide possible ; après succès, anciennes règles de remplacement et retrait des noms inchangées.
4. **Répétition.** Réfléchir deux fois le même programme conserve la description, **pas nécessairement l’identité des pointees** : un commit réussi peut remplacer la génération.
5. **Concurrence.** Aucun nouveau scénario de relink/destruction concurrent : programme/contexte appropriés et sérialisation sont des préconditions du contrat, non une promesse de sûreté concurrente.

Sécurité et accessibilité non touchées ; pas de campagne ajoutée.

## 6. RED causal avant correction

L’auteur conserve son cycle RED→GREEN→REFACTOR.

1. Réutiliser les fixtures et shaders Attribute existants ; compiler/lier le programme et établir les préconditions GL.
2. Énumérer **indépendamment du Reader** le nom legacy, type, taille et location de l’array1.
3. Appeler le Reader ; vérifier publication sous **cette seule clé native**, avec champs exacts et sans alias.
4. Exiger `isGLArray=true`.

**RED attendu sur Mesa de départ :** les préconditions et la clé passent ; l’assertion de classification échoue. Une recherche codée en dur de `qaArray[0]`, un échec de compilation ou un contexte absent ne constituent pas ce RED.

Contrôles :
- scalaire comparable : `false`, upload permis ;
- array2 : `true`, refus ;
- après GREEN, arrays1/2 refusés avant mutation native.

Les trois FAIL historiques restent archivés. Leurs assertions d’orthographe doivent recevoir une disposition explicite : remplacement de l’hypothèse fournisseur par un oracle de clé native indépendante, **sans supprimer l’obligation de classification ni de refus**. Leur historique n’est pas réécrit.

Pour A6, créer/binder les ressources nécessaires **avant** le snapshot : l’absence de mutation concerne le `set` rejeté, pas la préparation volontaire du test. Utiliser une valeur non nulle et un type valide afin que l’erreur observée soit bien `ARRAY_NOT_SUPPORTED`.

## 7. Registre inverse : tests existants → obligations

| Test / famille existante | Obligation et raison | Limite actuelle |
|---|---|---|
| `NativeCapabilitiesAndArraySpelling` | A1/A5 : observation native | L’orthographe `[0]` imposée n’est pas un oracle portable de consommation moteur |
| `NativeReaderArrayOneRegression` | A1/A5 | Échoue actuellement sur clé avant classification ; ne remplace pas le nouveau RED causal |
| `NativeArraysDescribeThenRefuseWithoutTouchingDecoy` | A5/A6 | Clé figée ; vérifie leurre/binding, pas à elle seule toutes les mutations du buffer cible |
| `ExactNaturalLongNameContract`, `FaultInjection_InvalidLengthAndTerminator` | A1/A4 | Validation legacy, pas réponse moderne |
| `FaultInjection_ExactArraySuffixAndUnsupportedEnumDescription` | A2/A5/A8 | Pertinent au fallback explicitement configuré ; ne prouve pas la classification moderne |
| `FaultInjection_DuplicateNamePreservesPreviousGeneration`, `ReadAttributes_LateInvalidLocationKeepsPreviousBorrow` | A7 | Précédent utile, mais pas injection moderne tardive |
| `ReadAttributes_SuccessRetiresMissingNamesAndOldValues`, `ReadAttributes_ZeroCountCommitsEmptyGeneration` | A7 | Transitions existantes à préserver |
| Tests typés, transform feedback et pixels existants | A6/A8 | Oracles consommateur à conserver, pas preuve de gate |

**Audit inverse à l’intégration :**
- rattacher chaque nouveau cas A1–A8 et sa raison discriminante ;
- signaler tout test orphelin ;
- ne pas dupliquer un même négatif dans plusieurs tests sans frontière distincte ;
- identifier les preuves périmées par candidat ;
- ne pas déclarer « aucun orphelin » avant inventaire final.

Aucune obligation nouvelle n’est **COVERED** par la seule présence de ces tests.

## 8. Validation proposée au runner distinct

### Ordre

1. **Avant auteur :** lead récupère, relit, persiste et commite ce complément ; réserve les fichiers de mocks exacts.
2. **Auteur :** RED causal Mesa et CPU ciblés, correction minimale, GREEN ciblé ; reconstruction des TU touchées par les headers.
3. **Après intégration réelle :** candidat commité, delta identifié, reconstruction normale/ASan sur Windows/NVIDIA et Linux/Mesa.
4. Découverte puis populations CPU/GL requises ; rapprocher noms, JUnit et sorties. Les 581 historiques sont une référence, **pas un quota empêchant les nouveaux tests**.
5. Revue indépendante ciblée utile : gates, interposition GLEW, validation de longueurs et conservation des emprunts. Pas de nouvel audit généraliste ajouté.

Avant les commandes, le runner inspecte les CMake/presets/Conan/CI réellement intégrés. Réutiliser les lanceurs existants ; ne pas inventer une commande Linux dépendant du helper pool-5 non intégré.

Sous Windows, les commandes documentées restent des propositions au runner : découverte CTest puis labels `unit`/`opengl`, `--no-tests=error`, GL sérialisé. Sous Linux, même séparation et mêmes exigences de comptage avec commandes réellement présentes.

**Préconditions :** Conan 2, toolchain/STL/ABI et sonde C++23 valides, builds normal/ASan distincts, canari ASan qualifié, contexte effectif journalisé, propriétaire unique du bail GPU et des sorties. Aucun build concurrent dans l’arbre de pool-5.

### Hors périmètre

Release, UBSan, fuites exhaustives, performance, nouveau framework, nouveau pilote 3.3 matériel, minimum global 4.3. Les oracles existants de rendu restent inchangés, sans élargissement de tolérance.

## 9. Budget proportionné

Estimation relative d’effort humain, **pas un timing de build ou une capacité approuvée** :

| Poste | Unités |
|---|---:|
| Setup : fixtures, données, mocks GLEW, snapshots et identité | 25 |
| RED causal, clés/classification/refus | 17 |
| Gates et réponses invalides | 20 |
| Staging et transitions | 10 |
| Réconciliation Windows/Mesa, normal/ASan | 9,25 |
| Buffer investigation interposition/panne/ré-audit | 18,75 |
| **Total** | **100** |

Après setup : **56,25 planifiées / 18,75 buffer**, soit **75 % / 25 %** des 75 restantes.

Pas de coupe d’un risque critique ni du buffer pour conserver un contrôle secondaire. Si cette capacité ne suffit pas, signaler le budget irréalisable et réestimer la tranche ; ne pas convertir les obligations en skips.

## 10. Gaps, décisions et critères de sortie

### Bloquants pour annoncer GREEN/parité corrigée

- RED causal non encore exécuté.
- Gates, réponses invalides et staging moderne non qualifiés.
- Correctif non intégré ; non-régression Windows/Mesa normale/ASan non exécutée.
- Réception, persistance et commit de cette remise à confirmer avant auteur.

Ces gaps ne bloquent pas la **remise du plan**.

### Limite globale conservée

**GAP de qualification :** pilote 3.3 sans capacité moderne, nom legacy ambigu. La route historique doit être prouvée inchangée par mocks, mais cela ne prouve pas la scalarité universelle.

Propriétaire de la disposition : lead ; point de décision : avant ajout d’un tel pilote aux cibles requises. Ce gap n’est ni accepté implicitement, ni transformé en skip/PASS.

### Décisions restantes

- Auteur : diagnostics précis avant code, fonctions modernes effectivement utilisées et interposition minimale dans les mocks existants.
- Lead : réservation des fichiers communs, capacité d’effort et bail GPU après pool-5.
- Gouvernance : le mandat « QA proportionnée » ne vaut pas dérogation silencieuse à la boucle QA écrite dans `AGENTS.md`. Le lead doit expliciter la disposition des étapes aval avant leur admission ; cette divergence n’empêche pas ce plan pré-auteur.

### Sortie

- A1–A8 associés à des preuves qualifiées et discriminantes.
- RED causal conservé ; GREEN sur candidat commité intégré et TU reconstruites.
- Populations réconciliées par nom ; aucune exclusion, zéro test ou skip requis présenté comme succès.
- Anciens FAIL conservés et dispositions documentées.
- **PASS** seulement au critère exécuté ; **FAIL** pour l’écart reproduit ; **INCONCLUSIVE** pour la précondition ou preuve manquante.
- Lecture/restitution observées ne prouvent pas l’enforcement exhaustif des permissions : celui-ci reste **INCONCLUSIVE**.

**Verdict de cette remise : plan ciblé proposé, aucune validation d’implémentation revendiquée. Parité Linux actuellement FAIL selon les preuves transmises.**

## assistant — 2026-10-28T18:26:53.124Z — msg_183c3da3e00142WdsOmC1TVLo4

Lecture locale réussie du contrat à `38d03b7`, du plan initial et des sources ciblées. Aucun refus observé, aucun edit ni test exécuté.

Le complément complet figure dans ma remise finale précédente : gates, classification, clés natives exactes, staging et gap 3.3, sans garantie nouvelle de rollback CPU du wrapper. Persistance, commit et qualification de réception restent au lead.

## assistant — 2026-10-28T18:27:14.850Z — msg_183c42f1d001ZPg2sVg6TQC2bz

# Complément Attribute gated — candidat à intégrer au plan QA Linux

**Chemin unique de persistance :** `.opencode/evidence/mission-linux-qa/qa-plan.md`, en complément du plan existant. Le lead doit récupérer, relire, persister et commiter cette remise **avant admission de l’auteur**.

Lecture locale du contrat, du plan initial et des sources ciblées réussie. Aucun refus observé. Aucun edit, test, build, shell ou délégation exécuté.

## 1. Pin observé, autorité et limites

- HEAD local lu : `38d03b7e98a497fd41680e1083b84059ec730330`.
- Sources exécutables toujours `a4c0f82`, changements ultérieurs documentaires seulement : information attribuée au lead, sans vérification Git exécutée ici.
- Autorité : `.opencode/evidence/mission-linux-qa/attribute-correction-contract.md`, rapport 18 et avis architecte adopté.
- Pins anciens `e4b0aa7`/`40a7cc3` non applicables à ce candidat ; aucun profil ou pin réécrit.
- ADR 0001/2/4/6/7 Proposed ; 0003/5 Accepted/partial ; 0008 vérifié Windows Debug seulement.

**Intention :** utiliser la réflexion moderne uniquement comme information interne de classification, lors de la réflexion du même programme lié. Conserver les clés legacy exactes, interfaces, ownership et minimum OpenGL global.

**Code observé :**
- `OpenGLPassAttributeReader<Classic>::on` conserve le nom legacy et utilise actuellement `name.ends_with("[0]")`.
- Le Reader construit une map temporaire avant `replaceAttributes`.
- Le Setter refuse array/taille différente de 1 avant bind/upload.
- `IAttribute::set` stage la valeur CPU avant le Setter : **aucune conservation de l’ancien miroir CPU après rejet n’est ajoutée**.

Branche pool-5 non consommée. Aucun résultat moteur exécuté par ce profil, aucune mesure de performance.

## 2. Preuves existantes et qualification

| Source | Résultat réutilisé | Portée |
|---|---|---|
| Rapport 15, Linux normal et ASan | Pour chaque campagne : 506/506 CPU, 72/75 GL, mêmes 581 noms, 3 failures identiques, 0 error/skip | **executed**, attribué producteur/lead ; parité Linux **FAIL** |
| Référence Windows fournie | 581/581 normal et ASan au candidat indiqué | **executed**, historique Windows ; pas GREEN du futur correctif |
| Rapport 17 et sonde | Mesa 25.2.8/llvmpipe, contexte effectif 4.5 core ; legacy scalar/array1 indiscernable par nom/size/location, moderne discriminant | **executed**, sonde autonome ; pas preuve du Reader corrigé ni d’un refus d’upload |
| Dernier message du lead | CI distante reproduisant les mêmes trois failures normal/ASan | Résultat transmis par le lead, artefacts non relus ici ; aucun rerun demandé |
| Lectures de cette tâche | Reader, Setter, wrapper, tests et pattern fp64 des mocks | **static-presence**, jamais exécution |

Sonde locale : `out/logs/mission-linux-qa/attribute-name-probe/`. Le manifeste contient la commande compilateur exacte et ces SHA256 annoncés :

- source : `76adb5b30d315108449ecbafebfccc791b03a2aec8dc8e4d2424c77cd82b677c`
- binaire : `9ccd33dfe46868bf482a3414b394203d089ddcd8329609f80a177336be4950f9`
- log : `df1435dc59239d7e0cb2b8a2136276ee68254ee0177aebb4cce84dbe548a6eaf`

Empreintes **lues, non recalculées** ici. Build/run 0 rapportés ; trois programmes comparatifs, pas une population CTest.

Les commandes, hashes et environnements complets des campagnes historiques restent dans leurs archives : ne pas compléter les champs absents par supposition.

**Toute nouvelle preuve doit porter :** HEAD complet, SHA256, delta, source/date, commande exacte, runner, environnement effectif, attendu/observé, découverts/sélectionnés/exécutés/échoués/skipped. Candidat différent : preuve périmée pour le GREEN final.

## 3. Top 3 risques

Scores QA 1–5, distincts des scores PM.

| Risque | I × P | Déclencheur / blast radius | Détection et mitigation actuelles | Gap |
|---|---:|---|---|---|
| Array1 classé scalaire ; correction limitée aux noms des tests | **4 × 4 = 16** | Nom legacy nu, size1 ; refus Attribute et buffers consommateurs | Sonde moderne discriminante ; garde Setter existante | RED causal et refus sans mutation non établis |
| Appel moderne non supporté ou réponse invalide acceptée | **4 × 4 = 16** | Pointeurs sans capacité, fonction manquante, réponse incohérente ; réflexion du programme, crash/classification erronée | Patterns GLEW/mocks existants ; contrat fail-closed | Gates et validation des réponses à qualifier |
| Publication partielle après faute tardive | **5 × 3 = 15** | Première entrée staged puis erreur ; anciens emprunts et map publiée | Staging existant, tests d’erreurs legacy | Erreurs modernes tardives et reprise après échec non prouvées |

Tous trois sont critiques : automatiser nominal et erreurs. Priorité aux gates peu coûteuses et au RED causal, puis réponses/staging, puis qualification GL intégrée. Réévaluer à l’intégration et aux surprises ; ne modifier aucun score de routage.

## 4. Obligations explicites et scénarios

Les scénarios suivants sont **proposés**, tous **GAP** avant exécution.

Abréviations :
- **Reader** : `ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp:OpenGLPassAttributeReader<Classic>::on`
- **CPU** : `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp`
- **GL** : `ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`

| ID / intent | Nominal | Négatif et oracle discriminant | Preuve proposée |
|---|---|---|---|
| **A1 — conserver l’identité publique**, contrat §15–16 | Map exactement égale aux noms legacy obtenus indépendamment du même programme | Moderne ajoute `[0]` : clé legacy inchangée, aucun alias supplémentaire ; nom absent toujours absent | Reader ; CPU + GL |
| **A2 — ne jamais appeler une route indisponible**, §17/19 | Core 4.3 sans extension, puis 3.3+extension, toutes fonctions chargées : moderne utilisé | 3.3 sans extension malgré pointeurs présents ; chaque fonction requise absente séparément : **zéro appel moderne**, route historique | Reader ; CPU/mocks |
| **A3 — associer la bonne ressource**, §18 | Même programme et `PROGRAM_INPUT`, association par nom exact ou unique suffixe final `[0]` ajouté | Ordre différent, même location mais nom incompatible, `[00]`, `[1]`, suffixe arbitraire/double : rejet, pas association par ordre/location | Reader ; CPU |
| **A4 — refuser les réponses incohérentes**, §18–19 | Index, longueur/NUL, nom, type/taille/location concordants | Index invalide, longueur invalide, troncature, NUL absent/interne, propriétés contradictoires : exception précise et **aucun fallback** | Reader ; CPU |
| **A5 — distinguer scalar/array1**, §20/34 | Scalar vec3/size1 : `isGLArray=false`, upload admis | Array1 comparable : `true` ; array2 : `true`. Size1 et localisation `[0]` ne suffisent pas | Reader ; CPU + GL |
| **A6 — protéger les ressources natives**, §20/36 | Scalaire : contenu cible exact et consommateur existant correct | Arrays1/2 : `ARRAY_NOT_SUPPORTED`, buffers cible/leurre et binding inchangés autour du `set` | `attribute/Setter.hpp:OpenGLSetter<A>::on` ; CPU + GL |
| **A7 — publication transactionnelle**, §21/38 | Succès : génération entière publiée ; zéro attribut : commit vide existant | Ancienne map non vide, première nouvelle entrée valide puis faute moderne : anciennes clés, adresses, valeurs et métadonnées conservées ; aucun nouveau nom publié | Reader ; `graphic/context/PassContext.hpp:replaceAttributes` ; CPU |
| **A8 — respecter les frontières**, §16/22/40 | Moderne uniquement à la réflexion ; interfaces et oracles consommateurs inchangés | Après réflexion, set/consommation répétés sans nouvelle requête moderne ; aucun oracle imposant le rollback CPU du wrapper | Reader, `pipeline/Attribute.hpp:IAttribute::set` ; CPU + revue ciblée |

### Frontières obligatoires A2–A4

- Une **fonction requise absente** rend la route indisponible : zéro appel moderne.
- Un `GL_INVALID_INDEX` reçu **après sélection d’une route disponible** est une réponse invalide : échec staging, pas fallback.
- Longueurs : négative, zéro, `capacité−1`, égale/supérieure à capacité ; terminaison et cohérence `NAME_LENGTH`.
- Propriétés : taille zéro/négative ou différente, type différent, location négative/différente ; nombre de propriétés retournées incomplet si cette forme de requête est utilisée.
- Tester les bornes d’index effectivement employées, sans ajouter de requête uniquement pour un test présumé.
- L’auteur fixe les diagnostics précis dans les tests **avant le code** ; aucun identifiant nouveau inventé ici.

## 5. RED causal avant correction

1. Réutiliser fixtures et shaders Attribute existants ; établir compilation/link et contexte utilisable.
2. Obtenir indépendamment du Reader le nom legacy, type, taille et location d’array1.
3. Vérifier publication sous **cette seule clé native**, champs exacts, sans alias.
4. Exiger `isGLArray=true`.

**RED attendu sur Mesa de départ :** préconditions et clé passent ; classification échoue. Une assertion figée sur `qaArray[0]`, une toolchain absente ou un contexte manquant ne constituent pas ce RED.

Contrôles :
- scalaire comparable : `false`, upload permis ;
- array2 : `true`, refus ;
- après correction, arrays1/2 refusés avant mutation native.

Les trois FAIL historiques restent archivés. Disposer explicitement les assertions d’orthographe : remplacer l’hypothèse fournisseur par l’observation native indépendante, **sans retirer classification/refus**, ni réécrire l’historique.

Pour A6, préparer les ressources avant snapshot ; observer ensuite le `set` rejeté. Fournir valeur non nulle, buffer valide et type compatible afin que le diagnostic causal soit bien `ARRAY_NOT_SUPPORTED`. Vérifier contenu/taille des buffers cible et leurre, binding et configuration native pertinente. Le miroir CPU du wrapper n’entre pas dans cette garantie.

## 6. Inférences et heuristiques supplémentaires

Distinctes des obligations adoptées :

- **Mocks :** réutiliser `ARTIST/tests/mock/opengl/glFunctionMock.hpp`, pattern fp64 de capacités/pointeurs configurés ; reset explicite, aucune fuite d’état ni appel GLEW réel involontaire.
- **Ordre :** permutation des ressources modernes ; entrée valide avant faute pour prouver l’échec tardif.
- **Transitions :** échec puis succès ; succès retirant les anciens noms ; génération vide.
- **Répétition :** même description après deux réflexions, sans exiger l’identité des pointees après commit réussi.
- **Concurrence :** programme/contexte appropriés et sérialisation sont des préconditions. Aucun relink/destruction concurrent à ajouter.
- Sécurité/accessibilité non touchées : aucune campagne dédiée.

## 7. Registre inverse et preuves faibles

| Tests existants | Obligations / raison | Limite |
|---|---|---|
| `NativeCapabilitiesAndArraySpelling` | A1/A5, observation native | Orthographe imposée non portable comme oracle moteur |
| `NativeReaderArrayOneRegression` | A1/A5 | Échec actuel avant classification ; pas RED causal suffisant |
| `NativeArraysDescribeThenRefuseWithoutTouchingDecoy` | A5/A6 | Clé figée ; leurre/binding seuls ne couvrent pas toutes mutations cible |
| `ExactNaturalLongNameContract`, `FaultInjection_InvalidLengthAndTerminator` | A1/A4 | Réponses legacy, pas modernes |
| `FaultInjection_ExactArraySuffixAndUnsupportedEnumDescription` | A2/A5/A8 | Pertinent au fallback explicitement configuré |
| `FaultInjection_DuplicateNamePreservesPreviousGeneration`, `ReadAttributes_LateInvalidLocationKeepsPreviousBorrow` | A7 | À compléter pour faute moderne tardive |
| `ReadAttributes_SuccessRetiresMissingNamesAndOldValues`, `ReadAttributes_ZeroCountCommitsEmptyGeneration` | A7 | Transitions à préserver |
| Consommateurs typés, transform feedback et pixels existants | A6/A8 | Oracles conservés, pas preuve de gate |

À l’intégration : rattacher chaque nouveau test à A1–A8 ; relever orphelins, doublons faibles et preuves périmées. Ne pas déclarer leur absence avant inventaire. Aucun item nouveau n’est **COVERED** par présence statique seule.

## 8. Runner et séquence proposée

1. Lead : réception réelle, relecture, persistance et commit du complément ; réservation des fichiers de mocks.
2. Auteur unique : RED causal, tests CPU ciblés, correction minimale, GREEN, reconstruction des TU affectées par les headers.
3. Après intégration réelle : candidat commité, delta identifié, reconstruction normale/ASan Windows/NVIDIA et Linux/Mesa.
4. Découverte puis populations CPU/GL requises ; réconciliation par noms et JUnit. Les 581 historiques ne sont pas un quota empêchant les nouveaux tests.
5. Revue indépendante ciblée utile sur gates, interposition GLEW, longueurs et emprunts ; pas de nouveau dispositif généraliste.

Avant les commandes, le runner inspecte CMake/presets/Conan/CI intégrés. Réutiliser les lanceurs existants ; ne pas inventer une commande Linux dépendant du helper pool-5 non intégré.

Préconditions : toolchain/STL/ABI et sonde C++23 valides, Conan 2, builds normal/ASan distincts, canari ASan qualifié, contexte effectif journalisé, sorties uniques et bail GPU exclusif. Aucune compétition avec la campagne pool-5.

Hors périmètre : Release, UBSan, fuites exhaustives, performance, nouveau framework, nouveau driver 3.3 matériel, minimum global 4.3. Oracles existants et tolérances conservés.

## 9. Budget relatif

Estimation d’effort humain, pas timing build ni budget déjà approuvé :

| Poste | Unités |
|---|---:|
| Setup : fixtures, données, interposition et identité | 25 |
| RED causal, clés/classification/refus | 17 |
| Gates et réponses invalides | 20 |
| Staging et transitions | 10 |
| Réconciliation Windows/Mesa normal/ASan | 9,25 |
| Buffer investigation interposition/panne/ré-audit | 18,75 |
| **Total** | **100** |

Après setup : 56,25 planifiées et 18,75 buffer, soit **75 % / 25 %** des 75 restantes.

Si la capacité ne suffit pas, réestimer explicitement ; ne pas sacrifier un risque critique ou le buffer pour conserver un contrôle secondaire.

## 10. Gaps, décisions et sortie

### Bloquants pour GREEN/parité corrigée

- RED causal non exécuté.
- Gates, réponses et staging moderne non qualifiés.
- Correctif non intégré ; non-régression intégrée Windows/Mesa normale/ASan non exécutée.
- Réception/persistance/commit du présent complément à confirmer avant auteur.

Ces gaps n’empêchent pas la remise du plan.

### Gap global 3.3

Pilote 3.3 sans capacité moderne et nom legacy ambigu : **GAP de qualification**, pas PASS universel ni skip autorisé. Les mocks peuvent prouver la conservation du fallback, pas sa capacité universelle à distinguer array1.

Propriétaire : lead ; décision avant ajout d’un tel pilote aux cibles requises. Aucune nouvelle politique publique de rejet/support adoptée.

### Décisions restantes

- Auteur : diagnostics exacts avant code, liste des fonctions utilisées, interposition minimale.
- Lead : fichiers communs, capacité et bail GPU.
- Gouvernance : le mandat « proportionné » n’abroge pas silencieusement la boucle QA écrite du projet. Le lead explicite la disposition des étapes aval avant leur admission ; cela ne bloque pas ce complément pré-auteur.

### Critères de sortie

- A1–A8 reliés à des preuves qualifiées et discriminantes.
- RED causal conservé ; GREEN sur candidat intégré commité et TU reconstruites.
- Populations réconciliées par nom ; aucun zéro test, skip requis ou preuve périmée présenté comme succès.
- FAIL historiques conservés avec dispositions explicites.
- **PASS** : critère exécuté vérifié ; **FAIL** : écart reproduit ; **INCONCLUSIVE** : précondition/preuve manquante.
- Lecture et restitution observées ne prouvent pas l’enforcement exhaustif des permissions : celui-ci reste **INCONCLUSIVE**.

**Verdict : complément de plan proposé, aucune implémentation validée. Parité Linux actuellement FAIL selon les preuves transmises.**
