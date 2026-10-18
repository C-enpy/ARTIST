# Attribute-GL — plan QA Q, restitution persistée par le lead

> Révision renforcée en préparation dans attribute-oracles-a2.md. Rectification
> vérifiée : GLSL3.30 révision6 §4.3.4 autorise les tableaux d'entrées vertex.
> L'ancienne hypothèse de refus compilation ci-dessous est erronée ; elle reste
> l'historique de Q, pas l'oracle à transmettre à l'auteur. Q2 doit la remplacer.

## Attribution et candidat

Proposition : qa-planner / pool-2, tâche task_mvdqhtoe_001j_edgftw,
contexte fresh, C46/R52, grille approuvée v1. Synthèse fidèle par le lead des
réponses finales natives, sans exécution par la QA :
- ses_eb1423176ffeiluzzxpHDO6TeQ, msg_14ecc836a001ntwPgJ0voVwcDF ;
  SHA256 texte 30a80ea78c282c8ba6d08931921a9f31a9d8905186f6e0478092b53ec5f23776 ;
- complément msg_14ed646e4001X4QKwJz3l6EDTj ;
  SHA256 texte 1256465a52d7f1cfd64b3336b3417457c9a16be5620c30c733ad4f14b4f6ec23.

Textes complets conservés dans la session native ; les empreintes portent sur
ses parties textuelles concaténées par LF, pas sur cette synthèse.
Base : HEAD 4a9b722a8f1b64470172686aee961221fa6384c0 et diff de cadrage partagé.
Reprise : HEAD observé 230c551dd05a4fdbeff0a8cce3b78bc39e734688. Comparaison
git 4a9b722..HEAD sur ARTIST/, scripts/, cmake/, CMakeLists.txt,
CMakePresets.json et conanfile.txt : aucun changement ; même constat du diff
non commité sur ces chemins. Les modifications externes du harness restent
hors de ce commit de plan. La vérification d'absence de delta moteur n'est pas
une nouvelle exécution des tests historiques.
Entrées : attribute-contract.md, docs/architecture/next-mission.md,
AGENTS.md, engineering-principles, ADR et build-and-test. `.grepai/config.yaml`
hors périmètre. Plan à commiter par le lead avant tout worktree auteur.

Pin observé : profil au 2026-10-16/e4b0aa7, candidat réel plus récent. Ne pas
réécrire le profil pour masquer cet écart. ADR0003/5 Accepted/partial,
0008 vérifié Windows Debug seulement ; 0001/2/4/6/7 Proposed.
Historique 299/299 sur 6d6dcb7 : périmé pour le nouveau contrat, pas un GREEN.

## Exigences explicites et intent

Les exigences sont celles du contrat A ; elles ne prouvent pas encore le moteur.

E1 : famille fermée des 12 types packed float/int32/uint32, scalaire et vec2–4.
E2 : sizeof/alignof et offsets vérifiés ; is_same des alias, pas une 13e forme.
E3 : types exclus sans on, set hors famille => UNSUPPORTED_TYPE, sans mutation GL.
E4 : Reader conserve enum GLSL brut et cardinalité distincts, type non supporté publié.
E5 : taille buffer de nom liée à GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, nom exact/NUL valide.
E6 : INVALID_NAME/DUPLICATE_NAME/INVALID_LOCATION/INVALID_SIZE avant publication invalide.
E7 : location0 valide, -1 refusée avant cast GLuint.
E8 : flag tableau sur suffixe exact [0], y compris size1, pas uniquement size>1.
E9 : état initial déterministe zéro, array false.
E10 : refus Setter ordonnés buffer, valeur, array/size, type, usage.
E11 : refus => zéro appel GL, y compris BindBuffer, BufferData, Pointer/IPointer.
E12 : float Pointer avec scalaire GL_FLOAT, composantes CPU, normalized false, stride sizeof(T).
E13 : entiers IPointer, scalar GL_INT/GL_UNSIGNED_INT ; jamais Pointer.
E14 : succès bind explicitement le VBO propre avant upload ; binding cible après succès.
E15 : leurre et son contenu intacts après liaison intercalée, succès et refus atteignables.
E16 : freeze/shared_ptr stocké avant refus conservés, pas rollback CPU implicite.
E17 : ownership unique/emprunts F7 conservés, pas nouveau lifetime/destructeur VBO.
E18 : Binder/Unbinder et diagnostics existants non concernés conservés.
E19 : 12 formes réellement interrogées sur GL natif ; consommation de trois domaines.
E20 : fixtures nettoient VAO/VBO/programmes même après ASSERT fatal.
E21 : RED causal avant production puis GREEN minimal et non-régression intégrée.
E22 : oracles futurs attribués, commandes/comptages/skips et candidat conservés.
E23 : aPos/GL_FLOAT_VEC3/location0/size1/arrayfalse dans PassTest reste valide.

Intent : séparer interface shader et layout CPU, refuser avant effets driver,
empêcher upload sur buffer tiers ; pas performance, image ou RAII exhaustive.

## Exigences inférées — propositions de méthode, pas extension du contrat

F1 : glEnableVertexAttribArray dans la fixture après set, jamais ajouté au Setter
pour faire passer le draw ; vérifier ARRAY_ENABLED au point de consommation.
F2 : transform feedback 3.3 d'un out vec4 ; entrées vec3(0.25,0.5,0.75), int -7,
uint42, sortie attendue (0.25,0.5,-7,42), valeurs exactement représentables float32.
F3 : avant capture, interroger INTEGER/TYPE ; erreur acquise si voie entière fausse.
F4 : 12 entrées actives dans un vrai programme #version330, locations explicites ;
second programme vec3+mat4 pour décrire la matrice puis la refuser à l'utilisation.
F5 : les métadonnées injectées CPU restent cohérentes avec réflexion, pas size3/GL_FLOAT
pour un vec3 réussi. Les mocks glGetProgramiv distinguent les pnames.
F6 : observation octets/taille du leurre et du VBO cible, pas seulement GL_NO_ERROR.
F7 : ordre et combinaisons d'erreurs testés, premier diagnostic gagne ; usage initial0
couvert sans appeler setGLUsage (qui peut avoir ses propres gardes).
F8 : deux set du même T observent le second payload ; autre T reste TYPE_MISMATCH.
F9 : suffixes foo[1], foo[00], foo[0]bar ne sont pas [0].
F10 : sondes de légalité séparées du RED comportemental, pas une fixture illégale
placée dans shaders/vertex/good pour prétendre prouver un refus Setter.

## Légalité et préflight natif — DH1

Inférence QA de GLSL3.30 §4.3.4, pas encore testée : entrée vertex tableau/struct
interdite ; double sans extension non admis. Une erreur shader est une
précondition négative attendue, pas un RED Setter. Pas de #version430 dans ce lot.
Runner lead, contexte 3.3 core, avant toute fixture shader de l'auteur :
1. #version330 core, in vec3 qaArray1[1] utilisé par gl_Position + fragment minimal :
   échec compilation attendu, aucun programme lié, donc aucune réflexion attribut.
2. #version330 core, in dvec3 qaDouble utilisé : même attente.
3. Programme vec3 et mat4 légaux et tous deux utilisés : link réussi attendu,
   matrice publiée GL_FLOAT_MAT4/size1/nom sans suffixe tableau.

Si 1/2 lie ou 3 diverge, enregistrer compilation/link/log et nom/length/size/type/
location réellement observés, puis arrêt pour arbitrage ; ne pas adapter l'oracle.
Un contexte absent ne produit ni résultat de sonde ni RED.
Matrice et tableaux mockés prouvent les gardes, pas leur atteignabilité native.

## Plan nominal/négatif et registre bidirectionnel

Toutes les nouvelles preuves restent GAP ; aucune ligne COVERED ou executed.
Les noms ci-dessous sont des symboles proposés, pas des tests déjà présents.

- E1–E3 <-> AttributeContractTests : static_assert 12 layouts/is_same ; négatifs
  qualifiers non canoniques, vec1 (extension dans ce TU), dvec3, mat4, structs et
  conteneurs ; IAttribute::setUnsupportedType pour exception sans GL.
- E4–E8 <-> AttributeReaderTests : vrai MAX_LENGTH mocké, exact long nom, NUL
  absent/interne et troncation refusés sans clé, doublon, location-1/0,
  size<=0 et suffixes, mat4 publié sans rejeter Pass.
  ReadAttributes_InvalidPassID ne prouve que l'ancien INVALID_PROGRAM_ID.
- E9/E16 <-> AttributeContextTests : zéros/array false, payload après refus,
  changement T freeze et messages existants inchangés.
- E10–E15 <-> AttributeSetterTests : bind puis BufferData puis Pointer/IPointer,
  12 mappings, toutes les erreurs et leur priorité, strict zéro appel GL au refus,
  y compris BindBuffer ; nouveau symbole mock IPointer fait partie du setup.
- E17/E18 <-> AttributeOwnershipTests + tests Unbinder existants : caractérisation
  et non-régression, pas preuve RAII globale.
- E19/E20 + F1–F4 <-> AttributeComponentTests.TwelveFormsNativeQueries,
  .ShaderConsumption, .DecoyBuffer : chaque forme SIZE/TYPE/STRIDE/BUFFER_SIZE,
  float NORMALIZED false, entiers INTEGER true, source VBO du VAO, leurre intact,
  capture représentative des trois domaines, cleanup même après ASSERT.
- E4/E19 <-> AttributeComponentTests.MatrixReflectedThenRejected : vrai programme
  mat4 publié puis rejet par Setter avec leurre inchangé.
- E23 <-> PassTest.AttributeReadingTest : renforcement aPos complet sur minimal.vert.
- E21/E22 <-> registre RED/GREEN et JUnit du lead sur candidat final, non vide.

Sens inverse : chaque test proposé référence son groupe ci-dessus ; tests sans
obligation discriminante => orphelins à revoir. Rien ajouté => aucun nouvel
orphelin exécuté ; les anciens succès size3/GL_FLOAT et SetIntUploadsOneIntWithoutGLError
sont des preuves faibles/périmées pour la réflexion et IPointer. Ne pas les compter
comme preuve du nouveau contrat. Commandes historiques et comptages ne sont pas
rejoués par cette proposition.

## Top 3 risques : I et P sur 1–5, distincts des scores PM

R1 I5×P4=20 : shader tableau illégal pris pour oracle. Déclencheur : fixture
négative avant préflight. Blast radius Reader/Setter et validité de la campagne.
Détection actuelle : statique seulement, aucune sonde. Mitigation : isoler trois
sondes et exiger arbitrage en divergence. Gap : atteignabilité native du flag
tableau non établie, DH1/DH2 bloquants.

R2 I4×P4=16 : Pointer reste utilisé pour entier tout en ayant GL_NO_ERROR.
Déclencheur : reprendre oracle taille/absence d'erreur. Blast radius huit formes
entières. Détection actuelle insuffisante, mock IPointer absent. Mitigation
planifiée : mock et INTEGER/TYPE avant capture. Gap : RED causal à établir sur
toolchain fonctionnelle ; symbole mock non linké n'est pas RED comportemental.

R3 I4×P3=12 : mauvais VBO touché au succès/refus. Déclencheur : liaison intercalée
ou validations tardives. Blast radius buffer tiers et VAO. Détection actuelle
ne couvre pas leurre, BindBuffer ni IPointer au refus. Mitigation existante
limitée buffer0/null ; ajouter leurre et séquence stricte. Gap : tests absents,
ordre des cinq diagnostics à vérifier.

Autres risques : grand entier converti en float32 non exact (DH4), NUL/strncpy,
flags GLM, cleanup ASSERT. Réévaluer après intégration et surprises ; ne pas
faire passer des preuves CPU pour GPU. Zéro test/skip requis => INCONCLUSIVE.

## Budget

Total100 : setup28, puis planifié54 et buffer18 (75/25 du reste72).
Setup : sondes8 ; mocks/RAII de fixtures14 ; préconditions layouts/alias6.
Planifié : layouts5 ; exclus3 ; Reader8 ; Setter12 ; 12 natifs14 ; leurre4 ;
capture6 ; non-régression2. Haut risque et faible coût en premier.
Buffer : driver inattendu, mat4 size, NUL, link TF, ré-audit RED, contexte absent.
Ne pas réduire un item haut pour ajouter du moyen. ASan/lifetime VBO ou 12 captures
complètes exigeraient un autre lot, pas une extension implicite de ce budget.

## Décisions humaines à obtenir, non approuvées par la seule politique modèles

**Décision utilisateur à la reprise : « Renforcer avant l'auteur ».** Les limites
DH2–DH4 proposées ci-dessous ne sont PAS acceptées. Ce plan committé constitue
la baseline de proposition, pas l'autorisation de démarrer D. Préparer une
révision des oracles, la faire passer par Q puis commiter le plan amendé avant
tout worktree auteur. Aucune couverture native impossible ne sera simulée pour
faire disparaître un gap. La politique modèles approuvée reste inchangée.

DH1 : résultat des trois sondes avant shader auteur ; divergence => arbitrage.
DH2 : si sonde tableau échoue, accepter preuve du drapeau tableau uniquement
mockée pour ce lot ; aucune réflexion native tableau prétendue COVERED.
DH3 : accepter mock seul pour INVALID_LOCATION, DUPLICATE_NAME et nom tronqué,
avec nominal réel des noms/locations sur 12 formes et aPos.
DH4 : reporter la consommation d'entiers hors plage exacte float32 ; lead à F,
impact : pas de preuve de toute la plage int/uint, INTEGER/TYPE restent obligatoires.
DH5 : alias mediump identique au canonique, vec3 aligné !=4 ou mat4 size inattendue
=> arrêt, pas treizième forme ou affaiblissement d'oracle.
Recommandations QA pour DH2–DH4, pas décisions utilisateur déjà obtenues.

## Sortie, ressources et commandes proposées

Plan et contrat committés avant worktree auteur ; arbitrages bloquants résolus.
Auteur TDD : setup fonctionnel, RED causal, GREEN minimal, refactoring et
non-régression sur diff intégré. G/R après intégration réelle ; T fresh hors board.
Un écrivain par fichier, un compilateur par sortie, contexte/fixtures GL exclusifs,
verrou CTest artist_opengl. Lead runner pour propositions QA, QA aucun test.

Commandes proposées par Q, à vérifier contre script réel avant exécution :
- powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -TestLabel unit -Jobs 1
- ctest --test-dir out/build/windows-msvc-debug -N
- ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error
- ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error --parallel 1

Conserver HEAD/delta/empreintes binaires, commandes exactes/runner, environnement,
attendu/observé, découverte/exécution/échecs/skips et JUnit. Toute case non native
non exécutée reste INCONCLUSIVE ; DH2–DH4 acceptés/reportés écrits, jamais couverts
par analogie. Ni ASan, image, perf, Release ou autre OS déduits.

État de cette restitution : aucun test exécuté (0/0/0/0), pas GREEN. Autonomie
d'écriture et enforcement QA non prouvés. Le plan est une proposition persistée
par le lead ; son commit et les arbitrages seront enregistrés séparément.
