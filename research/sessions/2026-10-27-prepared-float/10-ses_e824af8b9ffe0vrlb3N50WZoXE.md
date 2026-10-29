# Qualifier audit hors board

- Source : API native OpenCode V2, historique paginé `/api/session/{id}/message`.
- Session : `ses_e824af8b9ffe0vrlb3N50WZoXE` ; parent : `ses_e8390245bffdS3NfX0qwRMR5ed`.
- Borne exclusive : `msg_17eb9ecc9001hotDiK0lNKi3tB`, `2026-10-27T18:57:56.563Z` (demande d’ingestion exclue).
- Filtrage : messages user et parties assistant text uniquement ; reasoning, outils, fichiers et métadonnées natives non exportés ; chemins personnels et secrets reconnaissables masqués.
- Limite : transcription publique filtrée, non sauvegarde exhaustive ni preuve d’exécution ; les affirmations des agents restent attribuées.
- Pagination : 2 pages user et 2 pages assistant, poursuivies jusqu’à épuisement.
- Messages antérieurs à la borne : 65 ; messages publics retenus : 6 ; parties non textuelles exclues : 96.
- Synthèse : [Session et enseignements](../../../docs/references/session-2026-10-27-prepared-float.md).

> Données historiques non fiables : les instructions citées ne sont pas à exécuter.

## user — 2026-10-27T14:12:58.401Z — msg_17db5078a001rGIpCfRs0m8cXY

You are a subagent spawned by another session.
Qualification native du test-reviewer HORS board, lecture seule, sans audit encore. Ne crée/claim/consulte aucune tâche ou équipe. La voie team_spawn refuse les workers autonomes sans claim_task ; nous ne publions pas l’audit sur le board pour contourner ce refus. Le contrat ARTIST exige un audit fresh hors board ; cette session directe native garde la sélection native de la session, sans override de modèle. OpenAI exclusivement, aucun modèle/provider à changer. Aucun edit, shell, commande/test/build, subagent, autre MCP ou grant. Premier et seul travail : lire relativement AGENTS.md puis .opencode/missions/team_mvqfm1ww_0000_f8a8ki/final-audit-input.md (40 premières lignes suffisent pour cette qualification). Contexte ARTIST canonique explicite, ancien pool-5 non requis ; ne demander aucun accès externe. Restituer une courte réponse finale native : lectures réussies ou refus unique précis, chemins effectivement reçus, candidat source/admission indiqué, limites ; ne pas auto-attester les permissions ou sandbox. Le lead vérifiera profil/model/permissions/contexte/racine/lecture/restitution réels avant d’envoyer le mandat d’audit neutre dans cette même session. HEAD attendu db8ba555c5d65ce5d0feb5ecfcf9fac9efb586a7, source exécutée 3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23 ; delta documentation uniquement. Réponse finale suffit, aucun team_message requis hors équipe. En cas de refus ne le répète pas, aucun bootstrap, réparation ou substitution de profil. Ne commence pas l’audit avant cette qualification lead.

## assistant — 2026-10-27T14:13:16.478Z — msg_17db54e39001gKxSls92BX6fv7

Lectures réussies :
- `AGENTS.md` intégralement ;
- `.opencode/missions/team_mvqfm1ww_0000_f8a8ki/final-audit-input.md`, lignes 1–40.

Skill `artist-build-validation` chargé.

**Pin observé :** source indiquée `3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23` ; HEAD d’admission attendu `db8ba555c5d65ce5d0feb5ecfcf9fac9efb586a7`. Delta annoncé documentaire uniquement, non vérifié. Pins historiques non appliqués.

Transport prescrit essayé une fois : `This session is not in a team.`

Aucun audit commencé, aucune modification ni exécution de tests. Admission effective et permissions restent à vérifier par le lead.

## user — 2026-10-27T14:17:05.857Z — msg_17db8ce2f001pYbKsnCw3f5nZh

Qualification lead acquise pour cette même session directe fresh hors board. GET natif confirme test-reviewer/openai gpt6.1sol high/racine ARTIST. Une exception héritée browserallow a été RÉVOQUÉE par session.update permissions=[] ; relecture native confirme[], aucun grant/profil/harness/modèle changé. Conserve exactement cette session et ses restrictions, aucune permission à modifier. Le refus team_message hors équipe est identifié : NE PAS le répéter, réponse finale native uniquement. Le lead reçoit cette réponse directement puis la persiste. Aucun board à consulter/claim ni tâche à publier. Tu peux maintenant réaliser l’AUDIT indépendant neutre, pas exécuter.\n\nHEAD d’admission db8ba555c5d65ce5d0feb5ecfcf9fac9efb586a7 ; SOURCE exécutée 3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23. Comparaison faite par lead : delta = seulement final-audit-input.md et rapports17/18, aucun delta code/tests/shaders/build. Rapport19 ajouté non commité ensuite est uniquement preuve de qualification, pas changement source. Pas de mise à jour pin/profil à faire.\n\nLIRE intégralement .opencode/missions/team_mvqfm1ww_0000_f8a8ki/final-audit-input.md et contract.md/qa-plan.md dans ce dossier. Ils fournissent contexte ARTIST/pool-5 explicitement non requis, obligations et digestion sourcée des données volumineuses ; aucune commande ou chemin externe/MCP interdit nécessaire. Ne lis pas conclusions correctness-reviewer, propositions generator, board ou justifications auteurs. Utilise tests/production/fixtures nécessaires via read/grep/glob locaux. Skill artist-build-validation autorisé pour méthode seulement. execute ne contourne pas shell via CLI/fetch/autreMCP. Aucun edit, shell, test/build, sous-agent ou réparation.\n\nOBJECTIF : capacité des tests à détecter bugs plausibles et portée exacte des preuves de la tranche. Avant tout finding haut, proposer scénario/mutant discriminant précis. Séparer défaut bloquant, suggestion non bloquante, gap hors contrat ; KEEP/MODIFY/REJECT/DEFER, fichier:lignes, oracle et statut observé/inféré. Tu ne dois pas exécuter ces scénarios, simplement les analyser. Verdict borné pour chacune des7obligations du fichier neutre, pas acceptation globale moteur.\n\nCONTRAT : préparation runtime après link/réflexion floatGL_FLOAT exact strictementscalaire, arrays[1]/bool/nom absentoptimisé/invalide/métadonnées manuellesaltérées refusés ; ownersuniques, emprunt faibleslocaux/générations avantpointee, contexte/programme avantmiroir/upload. Mono-thread sérialisé nonréentrant, fenêtrevivante, aucun relink/deleteGLexterne. Invalidation metadata/maps/free/destruction/moves/destinationchargée/IDréutilisé ; rejetprépublication conserveancienne identité, map+estampille avantretrait, pasrollbackGLglobal. Mirrorstd::any et APIparnom conservés ; updates3uploads exacts sans lookupCPU(C)/résolutionnative(L)/any_cast(A), paspromessezéroalloc/perf. targetpréparéunefois, decoyparnom, miroirs/readbacksaprèsmismatchreprise. Pixels attendusindépendantsfixésavantcode, FBOlinéaireRGBA8 32x32 : (8,8)blanc→fond,(24,24)fond→blanc,(18,16)blanccommun,(2,2)fondcommun à0/.5 ; égalitébufferscomplément seulement.\n\nPREUVES : runnerlead final579normaux=504CPU+75GL et579ASan, mêmes579noms uniques,0failure/error/skip. Cinqcommandesconvenuesexit0, définitions exactes dans fichier neutre. PréflightASanhealthy0/UAF86/optionscanoniquesdetect_leaks0 ; Linux/Release/autresAPI/perf/détectionfuites nonexécutés. final-validation.json local out/qa-prepared-float/lead-runner-20261027/ SHA256312642425d7fd039a0469a15be373fde7a55f651b3a6bf5b7b6a0b2261eb048c.15hashessource+5binaires recoupés, headersproduction inchangésdepuisH2. Trois mutants séparés compilés,1failure/CTest8 chacun : Clookup réel ΔC6, LnativeTimes0dépassé3fois, AextractionΔA3 ; restaurationshashées puis clean-first69étapes normales+ASan, nominalfinalrepassé. EXCLURE essai07L initial qui réutilisait binaireC, preuveL=07brecompilé. Ninja objetdeps0 observé, contourné par cleanfinal sanscorrectiftoolchain. PremierpatchtestC2248accèsprotégé = pasREDcomportemental, correctiontestonlytémoinpublic, XMLancienSTALEexclu. Les logs/JUnit/diffs sourceexpérimentale sont accessibles localement par les chemins du fichier neutre ; pasbesoinrecompterlesXMLcomplets.\n\nLIMITES : REDinitialmécanismebornéarray1/bool/APIabsente, empreintesourceREDabsenteetcertainslifetimeajoutésaprèsgardes. Mutantsne reconstruisentpasl’historique. Négatifpixelupdateomis/drawomis chacun1FAIL dansarchiveconsommateurlocale, finalrestauré3PASS ; pasmockGPU. ADR0004globalProposed seuletrancheadoptée,0003/5partial,0008WindowsDebug ; aucunbackendnouveau,RAIIexhaustiveougainannoncé. PM/LLM72/72risque81, horsroutageEnsemble conserve sélectionsesssionnativeOpenAIhigh, aucunoverride.\n\nSORTIE : rapport final texte seulement, qualification factuelle, 7obligations/périmètre, findings prouvés et scénarios discriminants ou absencedefindingavec limites. Unrefusnouveau se rapporte unefoissansrépéter/grant. Leadtraite findings/ré-audit ; pas de développement niquestionutilisateurà créer.

## assistant — 2026-10-27T14:20:50.351Z — msg_17dbc3b29001XtemYcgAIPrrC0

# Audit indépendant — float préparé

**Verdict : MODIFY / INCONCLUSIVE pour la clôture complète.**  
Aucun défaut de production reproduit, aucun finding haut. Deux gaps contractuels de tests nécessitent traitement ou arbitrage explicite du lead. Les résultats GREEN finaux restent recevables dans leur périmètre.

## Qualification et pin observé

Lectures locales réussies : `AGENTS.md`, entrée neutre intégrale, contrat, plan QA, tests, production et fixtures nécessaires. Aucun code ni test écrit dans cette session ; aucune exécution, modification ou délégation.

- HEAD d’admission **fourni par le lead** : `db8ba555c5d65ce5d0feb5ecfcf9fac9efb586a7`.
- Source exécutée **déclarée dans le manifeste** : `3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23`.
- Delta documentaire et empreintes recoupées : attribution au lead, sans vérification Git/hash autonome.
- Pins historiques `e4b0aa7` / skill `40a7cc3` non appliqués. Seule la tranche float du contrat est adoptée ; aucun statut ADR global réécrit.

Les constatations ci-dessous portent sur la **capacité discriminante des tests**, pas sur une certification du moteur.

## Findings

### F1 — Moyenne — MODIFY : contexte incorrect non testé pendant la préparation

**Localisation :**
- `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp:286–309`
- `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp:91–118`
- Garde concernée : `ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp:47`

**Références :** contrat `35–41` ; plan QA `69–71`, `82–83` ; obligation 1.

**Fait — static-presence :** les négatifs contexte absent/incompatible portent sur `set`, après préparation réussie. Les préparations relues sont effectuées sous le bon contexte.

**Mutant plausible — inference :** supprimer `requireContext(pass.m_reflectionContext)` de `prepare`. Après réflexion sous A, passer à un contexte nul ou B puis préparer `"scalar"`. Les tests actuels ne sollicitent pas cette frontière ; le mutant paraît survivre.

**Assertion manquante :** refus de préparation **avant** `glGetProgramiv`, sans publication d’accès ni mutation. Le mock autorisant actuellement un link valide permettrait de rendre ce scénario discriminant.

**Impact :** perte possible du contrôle de contexte avant interrogation native, non couverte par les négatifs d’écriture.

**Vérification minimale proposée :** deux préparations négatives — contexte nul et autre contexte — avec zéro interrogation native, miroir inchangé, puis préparation réussie après restauration de A.

**Destination :** `DEFER_TO: test-engineer`, via le lead. Aucun changement de production demandé.

### F2 — Moyenne — MODIFY : révocation avant détachement partiel insuffisamment verrouillée

**Localisation :**
- `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp:173–183`
- `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp:630–694`
- Frontière concernée : `ARTIST/include/graphic/opengl/pipeline/component/pass/Freer.hpp:35–47`

**Références :** contrat `72–73`, `78–80` ; plan QA `76–81` ; obligation 2.

**Fait — static-presence :** le test avec accès préparé injecte l’échec à `glDeleteProgram`, sans shader. Les scénarios de nettoyage partiel avec shaders vérifient la conservation des owners, mais pas l’invalidation d’un accès préparé.

**Mutant plausible — inference :** déplacer `retirePreparedUniforms()` après la boucle de détachement, juste avant `glDeleteProgram`. Le test préparé existant reste discriminant sur la suppression, mais pas sur ce déplacement.

**Input discriminant :** programme réfléchi avec accès préparé ; premier shader détaché, puis second wrapper déplacé provoquant l’erreur déjà modélisée à `ShaderLifetimeTests.cpp:675–677`. Avec ce mutant, aucune révocation n’a encore eu lieu.

**Assertion manquante :** après l’échec partiel, ancien accès rejeté comme périmé, sans nouveau query/upload et sans modification du miroir. Le mutant pourrait au contraire accepter une écriture.

**Impact :** la garantie explicite « retrait partiel ⇒ aucun ancien accès réhabilité » peut régresser sans casser les assertions actuelles.

**Vérification minimale proposée :** joindre un accès préparé au scénario existant d’échec après détachement ; contrôler son refus répété et les effets nuls.

**Destination :** `DEFER_TO: test-engineer`, via le lead.

Ces deux findings sont des **gaps de couverture inférés**, non des FAIL exécutés ni des défauts constatés du code courant.

## Verdict par obligation

Les références abrégées ci-dessous désignent les fichiers déjà cités ou ceux du répertoire `ARTIST/component-tests/src/graphic/shader/`.

| Obligation | Verdict borné | Pouvoir discriminant constaté |
|---|---|---|
| **1. Préparation strictement scalaire** | **INCONCLUSIVE — F1** | `PreparedFloatUniformTests.cpp:141–259` et `PreparedFloatUniformComponentTests.cpp:64–88` arrêtent bool, autres types, array[1], absent/optimisé, métadonnées fabriquées et réflexion périmée. Retirer le contrôle scalaire ferait accepter l’array[1]. Contexte de préparation restant à couvrir. |
| **2. Lifetime et invalidations** | **INCONCLUSIVE — F2** | Tests métadonnées, maps, ID réutilisé, moves et destruction, notamment `PreparedFloatUniformTests.cpp:262–415`. Le témoin conservé avec uniform vivant rend la destruction du pass non vacuement testée. Frontière détachement partiel manquante. |
| **3. Publication/préservation** | **PASS borné** | `PreparedFloatUniformTests.cpp:418–468` construit un préfixe valide puis un doublon : génération, pointeur, valeur et ancienne location restent contrôlés. Une publication incrémentale serait détectée. Ordre sans exception également examiné statiquement dans `PassContext.hpp`. |
| **4. Trois updates sans C/L/A** | **PASS borné** | `PreparedFloatUniformTests.cpp:470–511` calibre les témoins, isole l’intervalle, exige trois uploads et leurs payloads exacts. Mutants séparés C/L/A effectivement arrêtés selon les preuves du runner. Corps réel/mock de `set` commun ; aucun contournement observé. |
| **5. Target préparé / decoy legacy** | **PASS borné** | `ReflectedProgramGuardCrossTests.cpp:101–169` vérifie même location, miroirs et readbacks des deux programmes avant/après mismatch puis récupération. Retirer la garde programme ferait accepter `17` ou corrompre le decoy. |
| **6. Pixels indépendants non vides** | **PASS borné** | `PreparedFloatUniformRenderTests.cpp:132–139,203–262` applique les quatre attendus fixes à chaque route et chaque valeur, avant égalité. Update omis et draw omis sont discriminés par les expériences archivées. Aucun assouplissement de tolérance constaté. |
| **7. Identité et traçabilité des preuves** | **PASS final / INCONCLUSIVE historique RED** | Découverte, populations normales/ASan, exclusions et restaurations sont documentées. L’absence d’empreinte RED initiale et les tests lifetime ajoutés après gardes empêchent d’attester rétrospectivement un RED complet. |

## Qualification des preuves

**Executed — runner lead, pas cette session :**

Source principale :  
`out/qa-prepared-float/lead-runner-20261027/final-validation.json`

SHA256 fourni :  
`312642425d7fd039a0469a15be373fde7a55f651b3a6bf5b7b6a0b2261eb048c`

- **579 normaux = 504 CPU + 75 OpenGL ; 579 ASan**, mêmes noms uniques, zéro failure/error/skip, cinq commandes finales exit 0.
- Environnement corroboré par le log final : Windows x64 Debug, MSVC 14.44.35207, CMake 3.31.7, Conan 2.12.2 ; OpenGL réel RTX 3090, 3.3.0, driver 621.01.
- JUnit OpenGL `14-final-opengl.junit.xml:673–692` : pixels attendus des deux routes et zéro résolution native pendant les updates.
- Mutants **C**, **L recompilé**, **A** : chacun un failure/CTest 8 ; diffs expérimentaux relus.
- Préflight ASan : healthy 0 / UAF 86 ; `detect_leaks=0`.

**REJECT comme preuves positives :** premier essai L réutilisant le binaire C, XML stale, échec de compilation C2248. Ils restent correctement exclus.

**Static-presence :** assertions, fixtures, découverte CMake et corps nécessaires relus.  
**Inference :** survie des mutants F1/F2 ; aucune mutation exécutée ici.

Les readbacks, compteurs et pixels répondent à des obligations distinctes : ils ne constituent pas des doublons interchangeables. Les suites générales vector/matrix/fp64 restent des non-régressions, pas des preuves supplémentaires du float préparé.

## Décisions et suite proposées au lead

- **KEEP** les oracles et preuves finales actuels.
- **MODIFY** les tests pour F1/F2, ou faire arbitrer explicitement ces gaps contractuels.
- Après intégration, runner uniquement : tests ciblés puis campagnes normales/ASan convenues. Exemple de sélection ciblée après reconstruction :
  ```text
  ctest --test-dir out/build/windows-msvc-debug -R "^unit\.(PreparedFloatUniformTest|ShaderLifetimeTests)\." --output-on-failure --no-tests=error --parallel 1
  ```
- Tout candidat modifié rend les preuves finales antérieures périmées ; reconstruire proprement, renouveler identités/JUnit puis ré-auditer. Ne pas se fier aux dépendances Ninja précédemment défaillantes.
- **DEFER hors preuve livrée :** Linux, Release, autres API, fuites, performance et RAII exhaustive. Aucune extension de mission proposée.

**Limite finale :** les GREEN sont solides pour les scénarios effectivement exercés ; ils ne ferment pas seuls les deux frontières négatives identifiées ni l’historique RED incomplet.

## user — 2026-10-27T14:50:01.421Z — msg_17dd6f338001X726eKLCu0JKYV

RÉ-AUDIT ciblé de vos F1/F2, même session indépendante hors board, permissions restrictives inchangées. Réponse finale native uniquement ; ne répéter aucun team_message hors équipe, pas de board/claim. Aucun edit/shell/test/build/subagent/grant ou réparation. Vous n’avez reçu aucune conclusion d’autre reviewer ou justification auteur.\n\nEntrée neutre actuelle : .opencode/missions/team_mvqfm1ww_0000_f8a8ki/reaudit-input.md à lire entièrement, puis les deux nouveaux tests et preuves nécessaires. HEAD de ce ré-audit 12db49f4e0b1d497b34fea78927a596b90ed400d ; source exécutée finale3b1b1028dcd64ca46e0a359365ae096be1a38ac4. Comparaison lead source→HEAD : deux fichiers de preuves seulement, aucun delta ARTIST/test-datas/scripts. Documents docs/ en préparation non committés ne sont PAS données d’audit, leurs anciens décomptes en brouillon ne remplacent pas ce candidat.\n\nDepuis votre premier audit source3d0e145 : seul fichier C++ modifié ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp. Deux tests ajoutés et includes existants, aucun header production/mock/fixture/CMake/script modifié. F1 : preparation contexte nul/autre, optional vide, zéro query GL, miroir/témoin/génération conservés puis préparation et update .75 après restauration. F2 : programme7 shaders61/62, second wrapper déplacé, premier detach réellement observé, témoin révoqué vérifié dans premier effet sans réentrer, erreur second puis wrapperrestauré ; deux refus stale sans query/upload/mutation, cleanup attendu. Jugez assertions et scénarios, pas la seule description. Pas d’accesseur protected utilisé.\n\nPreuves runnerlead NOUVELLES sous out/qa-prepared-float/lead-followup-20261027/ : final-validation.json SHA25629a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b. Nominal02deuxPASS ; mutantF1 03compilation0/test8/failure1 suppressionrequireContextprepare détectée(accèsretourné+querysupplémentaire). MutantF2 COHÉRENT04bcompilation0/test8/failure1 aprèsrebuildTOUSobjetsunitaires : révocationtardiveaprèsboucle détectée témoinencorevalide/doublewriteaccepté/mirrorqueryuploadmutés. EXCLURE04F2initial : seulTUrecompiléavecautresdéfinitionsinlineFreerd’origine, testPASSnonprobant ; expérience04buniformeestlapreuve. Headersrestaurés/hashés avantnormal05cleantotaletASan12cleantotal,69étapeschacun.\n\nCinq commandesconvenues ànouveauexit0 :06scriptDebug/unit506,07CTest-N581,09unit506,10OpenGL75,13scriptAddressSanitizer581. JUnit09/10/13etdécouvertesJSON08/14 mêmes581nomsuniques0failure/error/skip. CanariASanfinal0/86 detect_leaks0.15hashessourceet5binaires/proberecoupés, aucunemarqueurerreurASantardive. TestC/L/Abodycomparéexactementidentiqueà3d0e145,tousheadersproductioninchangés : expériencesC/L/Aprécédentesréutiliséesavecidentité,runsnormaux/ASanCOMPLETSentièrementrenouvelés581.\n\nHASHES actuels : test2bf8ded00380889471ef851197fe3ec3826e68a4a15c77a26f611d6ac28c958d ;PreparedFloatUniforma791eb6bb0ebd092112e3557a00a41416d59baf3df48455d738e9650e2b1c02d ;Freer8bfe24d6525a204d40d47d67caf333d8be41fc6bf2b3731031844ee4f0cd7c62.\n\nAttendu : fermer ou maintenir vos deux findings avec fichier:lignes/scénario discriminant ; confirmer portée des autresobligations de votre audit précédent àidentitérenouvelée, sans nouvelélargissement. Scénario/mutant AVANTtoutfindinghaut, distinguerbloquant/suggestion/gaphorscontrat. Vous ne devez exécuter aucunecommande. LimiteshistoriquesREDincomplet/fuites/ReleaseLinuxautresAPI/performanceinchangées, pasréinterprétéesenPASS ; aucuneRAIIgénéraleougainn’estpromis. Réponsefinalebornéesource/preuves/limites, leadpersistesansdéclarerclôtureavantvotreretour.

## assistant — 2026-10-27T14:51:01.033Z — msg_17dd7dc23001kmwTGZXeGWKVLT

# Ré-audit ciblé — F1/F2

**PASS borné : F1 et F2 fermés. Aucun finding bloquant maintenu ni nouveau finding dans ce delta.**

Même session indépendante, lecture seule. Aucun test exécuté, fichier modifié ou agent lancé par ce reviewer.

## Identité

- HEAD d’admission fourni : `12db49f4e0b1d497b34fea78927a596b90ed400d`.
- Source exécutée : `3b1b1028dcd64ca46e0a359365ae096be1a38ac4`.
- Delta source→HEAD documentaire uniquement, selon comparaison du lead.
- Entrée neutre intégralement lue ; brouillons `docs/` exclus. Aucun pin/profil réécrit.

## Fermeture des findings

Les références ci-dessous concernent `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`.

### F1 — Fermé / KEEP

**Lignes 180–233** ; contrat `35–41`, plan QA `69–71`, `82–83`.

**Static-presence :** les deux préparations sous contexte nul/autre exigent le diagnostic exact, un `optional` vide, aucun appel GL supplémentaire, miroir/pointeur/génération conservés. La réponse native de link reste valide : une autre erreur ne peut masquer la suppression de garde. Après restauration du contexte, préparation et upload `.75`, location et payload sont contrôlés.

**Executed — runner lead :** `03-mutant-F1.diff` retire précisément la garde. Son JUnit constate l’accès indûment retourné et les interrogations supplémentaires : **un test échoué, CTest 8**, compilation réussie. Le nominal ciblé passe.

Le scénario manquant est désormais discriminant.

### F2 — Fermé / KEEP

**Lignes 235–329** ; contrat `72–73`, `78–80`, plan QA `76–81`.

**Static-presence :** premier détachement effectivement observé ; témoin déjà révoqué contrôlé dans cette action, sans réentrer l’accès. Le second wrapper déplacé provoque le diagnostic attendu, puis est restauré. Deux écritures périmées doivent échouer sans query, upload ou mutation du miroir. Le cleanup final est attendu explicitement.

**Executed — runner lead :** `04b-mutant-F2-coherent.diff` déplace la révocation après les détachements. Après reconstruction cohérente des objets unitaires, le JUnit détecte le témoin encore valide, les écritures acceptées et leurs effets : **un test échoué, CTest 8**, compilation réussie.

**REJECT comme preuve causale :** l’essai initial `04-mutant-F2`, mélangeant des définitions inline différentes. Il n’intervient pas dans cette fermeture.

## Portée des sept obligations

| Obligation | Verdict actualisé |
|---|---|
| 1. Préparation stricte et préconditions | **PASS borné**, F1 fermé |
| 2. Lifetime, invalidations et retrait partiel | **PASS borné**, F2 fermé |
| 3. Publication et préservation après rejet | **PASS borné maintenu** |
| 4. Trois uploads exacts, sans C/L/A | **PASS borné maintenu** |
| 5. Target préparé, decoy legacy, récupération | **PASS borné maintenu** |
| 6. Pixels fixes indépendants et non vides | **PASS borné maintenu** |
| 7. Traçabilité | **PASS final ; historique RED toujours INCONCLUSIVE** |

Les obligations 3–6 sont reconduites sur l’identité renouvelée fournie : headers et consommateurs inchangés, corps C/L/A relu sans changement constaté. Les expériences C/L/A antérieures restent **réutilisées**, pas présentées comme nouvellement exécutées.

## Preuves finales et limites

Source : `out/qa-prepared-float/lead-followup-20261027/final-validation.json`, SHA256 fourni :

`29a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b`

**Executed, attribué au lead :** nouvelles campagnes complètes **581 normaux = 506 CPU + 75 GL**, puis **581 ASan** ; mêmes noms uniques, zéro failure/error/skip. Restaurations et clean-first documentés ; canari ASan **0/86**, fuites désactivées. Les empreintes sont celles recoupées par le lead, non recalculées ici.

Aucune nouvelle exécution demandée pour fermer F1/F2. Les limites restent inchangées : RED historique incomplet, fuites, Linux/Release, autres API, performance et RAII générale non validés. Ce PASS ne constitue pas une acceptation globale du moteur.
