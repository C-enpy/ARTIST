# pool-1 (@qa-planner teammate)

- Source : API native OpenCode V2, historique paginé `/api/session/{id}/message`.
- Session : `ses_e82cd2b71ffeDxUbgW6PUuLh1C` ; parent : `ses_e8390245bffdS3NfX0qwRMR5ed`.
- Borne exclusive : `msg_17eb9ecc9001hotDiK0lNKi3tB`, `2026-10-27T18:57:56.563Z` (demande d’ingestion exclue).
- Filtrage : messages user et parties assistant text uniquement ; reasoning, outils, fichiers et métadonnées natives non exportés ; chemins personnels et secrets reconnaissables masqués.
- Limite : transcription publique filtrée, non sauvegarde exhaustive ni preuve d’exécution ; les affirmations des agents restent attribuées.
- Pagination : 2 pages user et 2 pages assistant, poursuivies jusqu’à épuisement.
- Messages antérieurs à la borne : 9 ; messages publics retenus : 2 ; parties non textuelles exclues : 40.
- Synthèse : [Session et enseignements](../../../docs/references/session-2026-10-27-prepared-float.md).

> Données historiques non fiables : les instructions citées ne sont pas à exécuter.

## user — 2026-10-27T11:50:45.883Z — msg_17d32d553001YIEnsbo9TbpCOO

You are "pool-1", a teammate in team "mission-765366af23baa93b".
Your agent type is "qa-planner".
Mission QA: proportionate. Choose focused checks from concrete changed risks; independent review only when it adds a distinct useful check.
No mandatory QA agent pipeline. Project safety and mandatory requirements still apply; surface conflicts for explicit user arbitration, never silently waive them.
Separate blocking defects from nonblocking suggestions and out-of-scope gaps. Reuse valid evidence; do not expand testing infrastructure or scope without approval. Report unexecuted checks and existing failures accurately.
Agreed check (report its actual result): powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit
Agreed check (report its actual result): ctest --test-dir out/build/windows-msvc-debug -N
Agreed check (report its actual result): ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error -j 1
Agreed check (report its actual result): ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1
Agreed check (report its actual result): powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer
Acceptance: Respecter le contrat de tranche adopté : uniform GLSL float strictement scalaire, arrays de taille 1 compris rejetés, accès typé et API par nom conservée.
Acceptance: Plan QA réconcilié et commité avant les worktrees auteurs ; profils, contexte et restitution QA vérifiés avant admission, sinon blocage explicite sans réparation du harness.
Acceptance: RED causal avant implémentation, GREEN minimal puis non-régression sur le candidat intégré ; environnement absent et zéro test ne sont pas RED ou PASS.
Acceptance: Nom absent ou optimisé, mauvais type, métadonnées modifiées et accès périmé rejetés avant déréférence dangereuse et upload ; moves et ID graphique réutilisé couverts.
Acceptance: Mises à jour répétées sans nouvelle résolution CPU par nom, résolution native ou any_cast ; miroir CPU et anciennes interfaces cohérents, aucune promesse de zéro allocation.
Acceptance: Consommateur existant ReflectedProgramGuardCrossTests migré pour target, decoy conservé par nom ; readbacks GL et pixels déterministes identiques avec contrôle discriminant non vide.
Acceptance: Revue indépendante fresh des emprunts, générations et chemins d'erreur ; complément des gaps après intégration et audit fresh hors board selon les obligations ARTIST, runner lead distinct.
Acceptance: Suites normales et ASan pertinentes exécutées, tests graphiques réels sérialisés ; découverte, sélection, JUnit, skips et identité du candidat recoupés, limites non exécutées explicitement rapportées.
Acceptance: Documentation ADR-0004, registre, usage et prochaine mission actualisés seulement à la portée effectivement livrée ; aucun gain de performance annoncé sans mesure.
Native-profile candidate mode; fixed model: openai/gpt-6.1-sol. Use only your native profile permissions for the assigned planning/review. Return the candidate in your final response and stop. No phase transition, team-tool grant or task completion is implied; the lead reviews and accepts it.

Reporting mode: final response for lead review.
Use only the tools permitted by your native agent profile. Ensemble grants no additional permissions in this mode.
Return your full findings, plan, or blocker in your final response, then stop. Ensemble will retrieve it and notify the lead.
Do not call team tools or mark tasks complete. The lead must review, persist/integrate the candidate and explicitly close tasks; consumers stay blocked until then.

Your task:
Qualifier le qa-planner fresh puis fermer le protocole QA

Type: research
Goal: Première et seule tâche importable : après start utilisateur, qualifier le profil restrictif dans une nouvelle session par lecture locale et restitution effectives, puis réconcilier la proposition QA avec le contrat adopté et fixer les oracles avant les auteurs.
Planning: required
Planning reason: La qualification reste à exécuter ; les oracles et l'observation des résolutions doivent être fermés avant le code.

Acceptance criteria:
- Le lead vérifie profil natif effectivement chargé, contexte fresh et permissions restrictives conservées ; une lecture locale autorisée et la restitution finale sont observées. Ni l'ancien rapport ni l'admission sans lecture du test séparé ne suffisent.
- Remise par final_response, candidat reçu puis validé par le lead avant complétion ; aucun team_message, team_handoff, shell, test, édition ou grant requis.
- En cas de refus ou d'indisponibilité, restitution bornée unique avec action, ressource et limite ; aucun bootstrap répété, substitution de profil ou réparation du harness. Les auteurs restent bloqués.
- Exigences explicites/inférées et registre bidirectionnel couvrant préparation, invalidation, compatibilité, résolution amortie, consommateur et preuves.
- Nom absent ou optimized-out, GL_FLOAT exact, mauvais type et arrays de taille 1 ou supérieure couverts, sans modifier les compatibilités historiques.
- Risques QA distincts des scores PM : lifetime 5×4=20, cible/contexte 5×3=15, préparation/résolution 3×4=12 ; déclencheur, blast radius, détection, mitigation et gap pour chacun.
- Fixer l'oracle pixel avant auteur : FBO RGBA8 32×32 proposé, triangle (-0.75,-0.75,0), (0.75,-0.75,0), (0,0.75,0), programme f2Scalar existant, matrice identité et autres valeurs initialisées. Vérifier les pixels (8,8)/(24,24) pour 0 puis 0.5, un fond et un intérieur communs, coordonnées, états et lecture avant de geler les attendus.
- Fermer le protocole séparant recherches CPU, glGetUniformLocation et any_cast ; contrôle discriminant borné, sans plateforme générale d'instrumentation ni promesse de zéro allocation.
- Le coordinateur réconcilie, persiste et commit le plan et le contrat en H0 avant publication de prepared-float et création de son worktree. Aucune des cinq fiches différées n'est importée maintenant.

Verification:
- Lire contract.md, qa-plan.md et les extraits fournis dans des chemins locaux autorisés ; les digestions longues sont attribuées au lead, pas à une recherche MCP exécutée par le profil restrictif.
- Le lead constate distinctement chargement natif, contexte fresh, lecture et réception/validation de la restitution ; aucun lint ou fichier de profil seul n'est présenté comme preuve runtime.
- Rattacher ScalarVectorMatrixSingleValuesReadBack au readback legacy et ReflectedOwnerRejectsOtherCurrentProgramAndRecovers à l'isolation des programmes ; aucun ne prouve déjà l'accès préparé ou les pixels.
- Budget indicatif 20 setup / 60 planifié / 20 buffer, soit 75/25 hors setup ; aucune consommation passée inventée.
- Qualification non acquise, oracles ouverts ou plan non commité : ne pas publier les auteurs. Après succès, suivre publication-plan.json, avec vrais IDs et entrées intégrées, sans seconde importation globale.

Discovery document:
FICHE IMPORTABLE — qa-contract. Profil conseillé : qa-planner natif local ; agent est l'affinité d'admission, pas une simple recommandation en prose. Contexte fresh pour une qualification nouvelle, sans reprise d'une ancienne session QA. worktree false et final_response sont structurés ; aucune transition d'implémentation ou outil d'équipe requis. Aucun fichier mutable possédé, build/GPU non utilisés. Brief autonome : contrat adopté, proposition conditionnelle, extraits de tests et limites fournis localement ; digestion des sources longues par le lead. Méthode PM/LLM : complexité brute 66, retenue 66, risque 76, inchangés. Justification : frontières de preuve et permissions, sans code écrit. Estimation : une demi-journée hors résolution d'accès. Incertitudes : qualification runtime et fermeture pratique des oracles. La qualification est une barrière à exécuter, pas une preuve héritée.

ANNEXE COORDINATEUR — CINQ FICHES DIFFÉRÉES, HORS TASKS IMPORTABLES. Leurs contrats natifs sont conservés structurellement dans publication-plan.json. Les références ci-dessous sont locales au plan, jamais des IDs Ensemble. Après start, le lead utilise team_tasks_add uniquement pour les tranches réellement prêtes ; il transmet les champs de nativeTaskContract et remplace producerPlanIds par les vrais IDs retournés. Ne pas publier un consommateur avant intégration de son entrée, même si une dépendance pourrait empêcher son départ.

1. prepared-float — Implémenter l'accès float préparé et son invalidation sûre. Type implementation ; profil conseillé/agent cpp-engineer ; contexte fresh ; complexity 84, risk 90, worktree true, report_mode team_message ; planning.required true. Dépendance locale qa-contract, après qualification réussie, validation lead et commit H0. Objectif : API OpenGL float préparée, témoins révocables, upload typé, interface par nom et miroir CPU conservés. Entrées : H0 identifié, contrat/plan présents dans le worktree. Ownership exclusif : PassContext commun, contexts OpenGL Pass/Uniform, composants pass Loader/Freer/UniformReader et uniform Validation/Setter nécessaires, nouvelle API et tests CPU/lifetime/GL dédiés ; Pass.hpp/Uniform.hpp seulement si nécessaire. Aucun Attribute, Pipeline, Reader fichier, toolchain, profil ou configuration globale. Geler les nouveaux chemins avant édition, distincts du consommateur. Invariants : préparation après link/réflexion de l'incarnation actuelle ; nom exact, provenance réelle, GL_FLOAT exact, location GLint valide, scalaire strict ; métadonnées fabriquées, arrays[1] et ambiguïtés rejetés. Accès opaque non constructible publiquement depuis GLuint/location. Témoins faibles et générations vérifiés avant toute déréférence empruntée ; owners uniques, sans prolongation GPU ni concurrence ajoutée. Révoquer sur addUniform réussi, replaceUniforms réussi même vide, mutations de métadonnées, retrait/destruction et nouvelle incarnation même ID ; rejet avant publication du même programme vivant conserve ses accès, retrait effectué interdit toute réhabilitation. Moves suivent le contexte transféré par unique_ptr ; ancienne destination invalidée, pas de copie d'identité ou retour silencieux d'une génération ancienne. set vérifie validité/contexte/programme courant, conserve le miroir CPU puis glUniform1f reçoit directement l'argument float ; aucun lookup, réflexion, résolution de type ou any_cast pendant les updates. Préserver ERROR::SHADER::UNIFORM_NOT_FOUND et erreurs uniform ARRAY_NOT_SUPPORTED, TYPE_MISMATCH, NO_CURRENT_CONTEXT, PROGRAM_NOT_CURRENT ; nouveaux diagnostics déterministes testés. RED causal avant implémentation, GREEN minimal puis REFACTOR/non-régression dans la même tranche, sans assertion affaiblie. Tests : préparation et miroir CPU, ordre des gardes sans upload interdit, maps rejetées/publiées, destruction, toutes métadonnées, moves et même ID simulé ; GL réel pour type actif et contexte/programme. Commandes candidates : powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit ; ctest --test-dir out/build/windows-msvc-debug -N ; filtres réellement découverts avec --no-tests=error --output-on-failure -j 1. Reconfigurer pour nouveaux cpp collectés par glob. Charger artist-cpp-contracts et artist-build-validation ; références/implémentations à examiner, limites LSP complétées par matching exact. Ressources exclusives : fichiers possédés, une sortie de build, bail GL inter-processus/worktrees et fixtures. Barrière lead : arrêt auteur, intégration native, diff/preuves contrôlés, H1 fourni avant publication du consommateur. Méthode PM/LLM brute/retenue 84/84, risque 90 ; ownership, ABA et erreurs partielles difficiles et graves. Estimation 1,5 à 3 journées ; prérequis QA commise et observabilité fermée. Contexte fresh justifié par un brief autonome sur H0, pas besoin d'héritage.

2. consumer-pixels — Migrer le consommateur et démontrer l'équivalence graphique. Type implementation ; profil conseillé/agent gpu-engineer ; contexte fresh ; complexity 74, risk 77, worktree true, report_mode team_message ; planning.required true. Dépendance locale prepared-float, publication seulement après intégration effective H1 et validation de l'API, des fixtures et du plan disponibles dans le worktree. Objectif : préparer f2Scalar une fois pour target, garder decoy par nom et prouver valeurs, pixels et absence de résolution répétée. Ownership : ReflectedProgramGuardCrossTests.cpp, nouveau test pixel dédié, seules fixtures nécessaires et exemple compilé ; aucun header moteur de prepared-float. Bug production retourné à son propriétaire, pas de second écrivain concurrent. Critères : migrer target dans ReflectedOwnerRejectsOtherCurrentProgramAndRecovers ; préparation hors séquence répétée ; floats distincts et cohérence miroir CPU/readbacks GL ; mauvais programme à même location rejeté sans mutation target/decoy ni miroir, puis récupération avec la bonne cible. Séparer préparation/updates et recherches CPU/native/any_cast ; contrôle négatif discriminant. Petite fixture pixel du programme existant, états et valeurs explicites, attendus non vides, égalité ancien/nouveau et changement visible entre valeurs ; aucun framework général d'image. Exemple compilé réutilisable dans la documentation, component-test jamais décrit comme application produit. RED/GREEN/non-régression sans tolérance accommodante ni performance revendiquée. Vérification : OpenGLComponentTest GLFW/GLEW 3.3 core réel, programme courant et erreurs GL contrôlés ; glGetUniformfv target/decoy ; autres uniforms f2-single-values.vert et testUniform du fragment initialisés. Reconfiguration/découverte CTest, puis ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1. Archiver résolutions, pixels, états, candidat, attendu/observé et comptages ; readback seul ne prouve pas l'image. Ressources exclusives : build de branche, bail GPU global et fixtures ; RESOURCE_LOCK CTest seul ne protège pas plusieurs processus. Barrière lead : intégration réelle et H2 avec diff composé/preuves avant publication des lecteurs. PM/LLM brute/retenue 74/74, risque 77 ; faux zéro lookup et oracle faible peuvent masquer l'échec. Estimation 1 à 2 journées sous réserve de l'oracle fermé. Contexte fresh autonome sur H1, sans fork inutile.

3. lifetime-review — Revoir indépendamment les emprunts et chemins d'erreur intégrés. Type review ; profil conseillé/agent correctness-reviewer ; contexte fresh ; complexity 76, risk 85, worktree false, report_mode final_response ; planning.required true. Dépendance locale consumer-pixels, publication après intégration H2. Objectif : détecter UAF, résurrection d'accès, mauvaise cible et régression de compatibilité. Aucun chemin mutable possédé ni build/GPU/fixture utilisé. Entrées : H2 stable, contrat et diff neutre, sans conclusions d'autres reviewers. Vérifier témoins/générations avant pointee et upload, publication/révocation, destruction, copies/moves, réutilisation d'ID, mutations publiques, contexte/programme et miroir CPU. Distinguer échec de réflexion du même programme vivant, échec après retrait et absence de transaction globale load ; aucun rollback inventé. Findings bloquants sourcés et scénario discriminant ; suggestions et gaps hors mission séparés. Lecture ciblée des corps/références et tests positifs/négatifs ; références indexées vides non assimilées à absence d'usage. Restitution statique reçue puis validée par le lead, sans correctif, exécution, grant ou transition. Lecture parallèle possible avec test-gaps sur H2 stable, sans échange de conclusions ni groupe separate-worktrees. Si H2 change, nouvelle identité et réévaluation des conclusions. PM/LLM brute/retenue 76/76, risque 85 ; lifetime grave parfois invisible aux tests. Estimation une demi-journée à une journée ; limites LSP et disponibilité du candidat à qualifier. Fresh requis pour indépendance, pas de fork auteur. Ne remplace pas l'audit final hors board.

4. test-gaps — Proposer les tests discriminants manquants après intégration. Type implementation, même si propositions seulement ; profil conseillé/agent test-engineer ; contexte fresh ; complexity 70, risk 68, worktree false, report_mode final_response ; planning.required true. Dépendance locale consumer-pixels, publication seulement après H2 intégré et profil/restitution restrictifs qualifiés. Objectif : propositions textuelles/patchs bornés pour gaps avérés, sans écrire les fichiers ni remplacer le RED auteur. Aucun fichier mutable, build, GPU ou fixture possédé ; aucune édition, shell, exécution, délégation ou transition de phase exigée. Entrées H2, registre et preuves lisibles localement, digestion fournie par le lead sans bootstrap interdit. KEEP/MODIFY/REJECT/DEFER pour chaque proposition, obligation/défaut/oracle avant code et absence de doublon faible. Examiner préparation, arrays[1], périmé avant pointee, leurres, moves, publication, résolutions CPU/native et pixels non vides. Pas d'élargissement production, infrastructure générale ou reprise Reader/stdio/close. Relire assertions réelles, pas seuls noms/effectifs. Restitution finale reçue et validée, code explicitement non appliqué/non exécuté. Lecture parallèle avec lifetime-review sur H2 stable, sans échange de conclusions. Le lead applique seulement les propositions retenues avec ownership transféré, constate leur RED/GREEN et non-régression ; ses exécutions ne sont pas attribuées au generator. Runner sous bail/environnement normalisé, découverte/sélection/exécution/JUnit et H3 conservés. PM/LLM brute 67, retenue 70 par plancher développement, risque 68 ; les patchs textuels restent du code de test. Estimation une demi-journée à une journée, gaps exacts encore inconnus. Contexte fresh pour analyse indépendante des preuves intégrées.

5. delivery-docs — Documenter seulement la tranche effectivement livrée. Type documentation ; profil conseillé/agent cpp-engineer pour auteur documentaire sans code modifié ; contexte fresh ; complexity 43, risk 50, worktree true, report_mode team_message ; planning.required false. Dépendances locales lifetime-review et test-gaps, remplacées par leurs vrais IDs lors de l'ajout ; ces IDs seuls ne satisfont pas la barrière. Publication seulement après H3 intégré, résultats runner et audit test-reviewer fresh hors board sans finding bloquant, ou disposition explicite des limites. Objectif : aligner ADR-0004, registre, usage, état courant et prochaine mission sur le livré. Ownership : docs/adr/0004-layouts-types-et-donnees-preparees.md, docs/adr/README.md, docs/usage/uniform-attribute.md, docs/architecture/current-state.md, docs/architecture/correction-plan.md au périmètre utile ; rapports/clôture restent au lead. Distinguer adoption limitée, implémentation et preuves Windows Debug ; aucune adoption globale ou vérification exhaustive des ADR. Exemple issu du code compilé ; contexte/programme, emprunts, invalidation, mono-thread et erreurs explicités. Consommateur component-test accepté, pas application produit ; pixels/lookups/allocations reliés à leurs preuves, aucun gain inventé. Préserver frontmatter/liens/sources/dates, archives et diffs étrangers. Vérifier diff documentaire ciblé, liens et identité des preuves ; pas de campagne cosmétique. Intégration lead avant clôture ; un indexeur pour node .opencode/scripts/update-wiki.mjs puis qmd embed -c artist-wiki si retenu. PM/LLM brute/retenue 43/43, risque 50 ; surdéclaration documentaire, pas développement exécutable. Estimation 2 à 4 heures après preuves ; fresh autonome sur H3. Index Git et indexation réservés au lead après intégration.

OPÉRATIONS LEAD, NON TÂCHES À IMPORTER : après les deux lectures, corriger via les owners, appliquer les tests retenus et exécuter les régressions normales/ASan pertinentes, un job par build et bail GL global. Runner PM/LLM brute/retenue 73/73, risque 74, une demi-journée à une journée hors corrections. Audit test-reviewer fresh hors board : 72/72, risque 81, worktree false, final_response, une demi-journée ; paquet contrat/diff/preuves neutre, aucune écriture ni exécution par le reviewer. Finding bloquant → propriétaire → intégration → runner → ré-audit. Conserver commandes, environnement, attendu/observé, candidat/diff/empreintes, découverte/sélection/exécution/JUnit et skips ; zéro test ou précondition absente reste INCONCLUSIVE. Disposer nativement des anciennes tâches sans fausse completion ni réactivation d'une production supersédée. Clôture seulement après livraison intégrée, preuves et contrôle de l'état natif.

Sources:
- .opencode/agents/qa-planner.md: Profil local, à distinguer du profil réellement chargé et des permissions effectives.
- .opencode/missions/team_mvqfm1ww_0000_f8a8ki/contract.md: Contrat adopté, ownership, matrice d'invalidation et consommateur accepté.
- .opencode/missions/team_mvqfm1ww_0000_f8a8ki/qa-plan.md: Proposition conditionnelle ; qualification runtime nouvelle toujours à exécuter.
- .opencode/missions/team_mvqfm1ww_0000_f8a8ki/publication-plan.json: Arbitrage confirmé : seul qa-contract initialement importable ; contrats natifs structurés et scores inchangés des cinq fiches différées.
- .opencode/missions/team_mvqfm1ww_0000_f8a8ki/reports/04-admission-corrigee-et-publication-echelonnee.md: Décision du coordinateur et barrières de publication après intégration, sans nouvelle autorisation d'exécution.
- .opencode/missions/team_mvqfm1ww_0000_f8a8ki/task-preview.md: Graphe documentaire complet, ownership et opérations du lead ; aucune seconde importation globale.
- ARTIST/include/graphic/context/PassContext.hpp: getUniform contient contains/at ; addUniform et replaceUniforms peuvent détruire les pointees.
- ARTIST/include/graphic/opengl/context/UniformContext.hpp: Setters publics de métadonnées constituant des frontières d'invalidation.
- ARTIST/include/graphic/opengl/pipeline/component/pass/UniformReader.hpp: Réflexion candidate et publication ; INVALID_LOCATION, DUPLICATE_NAME et INVALID_SIZE.
- ARTIST/include/graphic/opengl/pipeline/component/pass/Freer.hpp: Révocation attendue avant les effets natifs de retrait.
- ARTIST/include/graphic/opengl/pipeline/component/pass/Loader.hpp: Publication programme et chemins d'échec à distinguer de la transaction de réflexion.
- ARTIST/include/graphic/opengl/pipeline/component/uniform/Validation.hpp: Gardes historiques de cardinalité, type, contexte et programme courant.
- ARTIST/include/graphic/context/UniformContext.hpp: Miroir CPU std::any et erreurs GET à conserver, sans any_cast sur le nouvel upload.
- ARTIST/component-tests/src/graphic/shader/UniformCardinalityComponentTests.cpp: Readback réel existant de f2Scalar, pas preuve du futur accès ni des pixels.
- ARTIST/component-tests/src/graphic/shader/ReflectedProgramGuardCrossTests.cpp: Consommateur target/decoy choisi et contrôle du mauvais programme.
- ARTIST/component-tests/include/OpenGLComponentTests.hpp: Fixture réelle GLFW/GLEW OpenGL 3.3 core.
- test-datas/shaders/vertex/good/f2-single-values.vert: f2Scalar actif déplace gl_Position ; autres uniforms à initialiser pour l'oracle.
- test-datas/shaders/fragment/good/minimal.frag: testUniform contrôle la couleur ; initialisation nécessaire pour une image non vide.
- ARTIST/tests/src/graphic/pipeline/UniformValueTests.cpp: Valeurs CPU et diagnostics GET historiques pour la non-régression.
- ARTIST/tests/src/graphic/opengl/pipeline/component/pass/UniformReaderTests.cpp: Réflexion transactionnelle, rejets et isolation des mocks.
- docs/adr/0004-layouts-types-et-donnees-preparees.md: Statut global et critères à actualiser seulement à la portée effectivement livrée.
- docs/references/session-2026-10-26-stable-reader.md: Leçons sur admission réelle, indépendance des revues, effectifs et clôture prématurée.

You have been assigned task task_mvqmfwpt_0001_vim66t. Return its candidate for lead review; do not close it.

## assistant — 2026-10-27T11:52:18.931Z — msg_17d3440ed0012NHVdJIJ0Svs3y

# Candidat — plan QA réconcilié « float préparé »

**Lot :** `qa-contract` — tâche `task_mvqmfwpt_0001_vim66t`  
**Chemin proposé au lead :** `.opencode/missions/team_mvqfm1ww_0000_f8a8ki/qa-plan.md`  
**Statut :** restitution textuelle, non persistée et non commitée. Aucun test écrit ou exécuté, aucune édition, aucun appel d’équipe, aucune clôture.

## 1. Qualification réellement observée

| Contrôle | Résultat |
|---|---|
| Lecture locale autorisée | **PASS limité à cette capacité** : lectures effectives d’`AGENTS.md`, du contrat, du plan et des sources ciblées |
| Skill requis | `artist-build-validation` chargé |
| Profil sur disque | `.opencode/agents/qa-planner.md` lu ; règles restrictives présentes |
| Profil natif effectivement chargé et composition des permissions | **INCONCLUSIVE** : vérification native à fournir par le lead ; le fichier lu ne suffit pas |
| Contexte réellement fresh, `worktree:false`, `final_response` | Déclarés dans l’affectation et `publication-plan.json` ; attestation native à vérifier par le lead |
| Restitution | Cette réponse constitue le candidat ; réception et validation restent à constater par le lead |
| Refus d’accès | Aucun refus observé pendant cette tâche |
| Enforcement/sandbox | Non démontré ; aucune tentative d’écriture interdite faite pour le tester |

Le mode spécifique de cette affectation est **`final_response` sans outil d’équipe**. Les prescriptions génériques anciennes demandant un essai `team_message` ne sont donc pas appliquées à cette qualification.

**Barrière maintenue :** ne pas publier `prepared-float` avant vérification native, réception/validation de cette restitution et commit H0 du contrat et du plan.

### Pin observé

- `.git/HEAD` référence `refs/heads/main`.
- Référence lue : **`2c8e9f235e82283a0686f2140e2a635db1ca2205`**, conforme au HEAD déclaré dans le contrat.
- Ce HEAD diffère des anciens pins `e4b0aa7` et `40a7cc3` : ils ne sont pas appliqués.
- Aucun inventaire Git du delta ni SHA256 n’a été produit ici. Le delta FOREIGN et l’identité des fichiers sont à contrôler par le lead avant H0.
- Le registre ADR actuel confirme notamment ADR-0002 globalement **Proposed**, avec une tranche déjà livrée. L’ancien résumé « not-started » n’est pas repris.
- Le contrat du 27 octobre adopte **cette seule tranche ADR-0004**, sans preuve d’implémentation ni promotion globale de l’ADR.
- Le chemin du plan de cette mission remplace, pour ce candidat, l’ancien pointeur documentaire `.opencode/evidence/mission-gpu-qa/qa-plan.md` ; conserver **un seul plan actif**, sans copie divergente.

## 2. Contrat et rectifications du plan précédent

**Sources locales lues :** `contract.md`, `qa-plan.md`, `publication-plan.json`, rapport `04-admission-corrigee-et-publication-echelonnee.md`, principes, registre ADR, plan de correction et corps ciblés des sources/tests. Aucune recherche MCP exécutée.

### Obligations explicites

- Préparation après link et réflexion de l’incarnation courante.
- Uniform actif **GL_FLOAT exact et strictement scalaire** ; arrays `[1]` comme arrays plus grands rejetés.
- Nom exact, non vide, sans NUL embarqué ; absent ou optimisé rejeté.
- Provenance réelle de réflexion ; métadonnées fabriquées ou ambiguës refusées.
- Emprunt révocable, sans prolonger les ressources CPU/GPU.
- Invalidation conforme à toutes les transitions du contrat, notamment moves et même ID numérique réutilisé.
- Gardes avant déréférence dangereuse, mutation CPU et upload.
- Updates préparés sans recherche CPU par nom, réflexion, `glGetUniformLocation`, résolution de type ou `any_cast`.
- Miroir CPU et API par nom conservés.
- Consommateur target préparé, decoy legacy, readbacks et pixels discriminants.
- RED causal, GREEN minimal, intégration, régressions, revue indépendante et audit des preuves.

### Inférences et recommandations, non nouvelles spécifications

- Les compteurs seuls ne suffisent pas : leur sensibilité et l’absence de chemin de contournement doivent être vérifiées.
- L’égalité de deux images noires est un faux oracle ; un intérieur blanc commun et des pixels qui changent sont nécessaires.
- ASan complète l’analyse d’ordre des gardes ; il ne prouve pas à lui seul l’absence de mauvaise cible GPU.

**Correction importante :** publication atomique, préservation après rejet avant effets, ordre des gardes, récupération après mauvais programme et mono-thread ne sont plus de simples inférences : ils sont explicités dans le contrat adopté.

Aucun rollback GL global, rejet compile-time de toute conversion vers `float`, zéro allocation ou gain de performance n’est ajouté.

## 3. Top 3 des risques et allocation

Les scores ci-dessous sont QA **I×P**, distincts des scores PM inchangés.

| Risque | Score | Défaillance, déclencheur et portée | Détection/mitigation actuelles et gap |
|---|---:|---|---|
| Lifetime, accès périmé, ABA | **5×4=20 — critique** | Destruction, remplacement de map, métadonnées, retrait/reload, move ou réutilisation d’ID provoquant UAF ou upload vers une ancienne incarnation | Code lu : owners uniques, remplacement destructif possible ; pas de témoin préparé démontré. Détection disponible : tests CPU/mocks et ASan à exécuter. Mitigation contractuelle : témoins/générations avant pointee. **Gap : aucun nouveau scénario exécuté.** |
| Mauvaise cible/contexte | **5×3=15 — critique** | Programme leurre à même location, programme zéro, contexte absent ou différent ; corruption d’une ressource tierce | Gardes legacy observées dans `Validation.hpp` ; test target/decoy présent statiquement. Mitigation préparée exigée à chaque set. **Gap : nouveau chemin et contexte autorisé non prouvés.** |
| Préparation incorrecte/résolution répétée | **3×4=12 — haut** | Type permissif, array `[1]`, réflexion ancienne/fabriquée, résolution déplacée dans chaque update ; valeur erronée ou contrat d’amortissement non tenu | Réflexion existante valide candidat avant publication ; compatibilité legacy volontairement plus large. Mitigation nouvelle : provenance, scalaire strict, accès typé. **Gap : préparation absente ; observation séparée à implémenter selon §6.** |

Réévaluer aux intégrations H1/H2/H3, à toute surprise et à tout changement de candidat. Automatiser nominaux **et erreurs** des deux risques critiques ; ne pas leur appliquer une simple fumée.

## 4. Registre obligations → preuves à produire

**Tous les nouveaux comportements restent `GAP`.** Les noms de nouveaux tests sont à fixer par l’auteur avant édition, sans modifier les fichiers du consommateur possédés par l’autre tranche.

| ID | Localisation et intention | Nominal | Négatif/frontière et oracle discriminant |
|---|---|---|---|
| O1 — préparation stricte | Nouvelle API ; `pass/UniformReader.hpp:OpenGLPassUniformReader::on` ; contextes OpenGL | Préparer le vrai `f2Scalar`, location signée valide, puis écrire/lire une valeur | Nom absent, optimisé, vide/NUL ; GL_BOOL/int/vector/matrix/double ; arrays float `[1]` et `>1`, alias/canonicalisation ambiguë, location invalide et métadonnées fabriquées : diagnostic déterministe, aucun accès publié ni upload. CPU/mocks pour les frontières ; GL réel pour réflexion active et arrays, sans imposer fp64 au contexte 3.3 |
| O2 — accès opaque et ownership | Nouvelle API ; wrappers Pass/Uniform et contextes | Accès utilisable tant que son contexte est vivant | Construction publique depuis GLuint/location impossible ; accès conservé après destruction : erreur attendue avant pointee et GL. Compilation positive/négative sur la vraie toolchain ; CPU/ASan et lecture indépendante |
| O3 — invalidation/publication | `PassContext.hpp:addUniform/replaceUniforms` ; setters de métadonnées ; `Loader.hpp:on`, `Freer.hpp:on` | Nouvelle publication puis nouvelle préparation valides ; `setValue` seul conserve l’accès | Chaque mutation de métadonnée ; add réussi, replace vide/non vide/même nom ; destruction, free/reload, même ID simulé : ancien accès définitivement rejeté. Rejet prépublication du même programme vivant : map, pointees et accès conservés. Après retrait partiel : aucune réhabilitation |
| O4 — moves et générations | Wrappers Pass/Uniform ; contextes et témoins | Move construction/assignation : accès source suit le contexte transféré | Ancienne destination chargée expirée après transfert réussi ; cleanup refusé conserve la garantie existante ; pas de copie d’identité ni retour d’ancienne génération à la frontière du compteur. Vérifier état et appels interdits, pas seulement absence de crash |
| O5 — gardes/cible | Nouvelle méthode `set` ; `uniform/Validation.hpp` ; consommateur target/decoy | Bonne cible courante : miroir puis upload exact de l’argument | Accès périmé avec contexte absent ; contexte absent/différent ; programme zéro/leurre : ordre de rejet attendu, aucun upload ou miroir modifié, état courant inchangé. Répéter le rejet puis récupérer avec la bonne cible |
| O6 — résolution amortie et compatibilité | `PassContext.hpp:getUniform` ; `UniformContext.hpp:getValue/setValue` ; `uniform/Setter.hpp` ; nouvelle API | Préparation séparée, trois updates distincts ; arguments directement envoyés et miroir cohérent | Compteurs séparés sensibles, deltas nuls pendant updates ; alternance legacy/préparé ; GET uninitialized/type mismatch et compatibilités legacy préservés. La nouvelle exigence GL_FLOAT ne durcit pas rétroactivement GL_BOOL ou arrays legacy de taille 1 |
| O7 — consommateur/readbacks/pixels | `ReflectedProgramGuardCrossTests.cpp:ReflectedOwnerRejectsOtherCurrentProgramAndRecovers` ; nouveau test pixel dédié | Target préparé une fois, decoy par nom ; readbacks et pixels selon §5 | Mauvaise cible sans mutation des deux programmes ; contrôle de changement de float et intérieur blanc obligatoire. Readback seul et égalité noire/noire refusés |
| O8 — admission, TDD et livraison fidèle | Contrat, plan, manifestes runner, documents ADR/usage | Admission attestée, H0 avant worktree, RED causal puis GREEN, preuves du candidat intégré, documentation bornée | Précondition absente, zéro test, skip requis ou candidat différent : pas PASS. Contrôler les barrières et l’identité ; documentation ne présentant ni component-test comme application produit ni absence de lookup comme gain mesuré |

**Passe heuristique retenue :** états vivant/publié/révoqué ; frontières `[1]`, location `-1`, génération ; ordre des gardes/publication/retrait ; répétition des rejets et stabilité après échec. Concurrence, accessibilité et nouveau périmètre sécurité ne sont pas ajoutés : le contrat est mono-thread et non réentrant.

## 5. Oracle pixel fixé avant auteur

### Justification source

Le shader lu calcule :

```text
gl_Position = vec4(identity * position + vec3(f2Scalar), 1)
```

après mise à zéro de `f2Vector` et des deux éléments de `f2Array`. Le fragment produit blanc lorsque `testUniform=1`.

### Fixture bornée

- FBO **32×32, GL_RGBA8 linéaire, mono-échantillon**, complet ; draw/read buffer `GL_COLOR_ATTACHMENT0`.
- Triangle : `(-0.75,-0.75,0)`, `(0.75,-0.75,0)`, `(0,0.75,0)`.
- VAO/VBO explicites ; attribut 0 = trois floats ; `GL_TRIANGLES`, trois sommets.
- `f2Matrix=identité`, `f2Vector=0`, `f2Array[0]=f2Array[1]=0`, `testUniform=1`.
- L’initialisation native de l’array appartient au setup : ne pas élargir l’API single-value pour la permettre.
- Viewport `(0,0,32,32)` ; fill ; color mask complet.
- Désactiver blend, dither, framebuffer sRGB, multisample, cull, scissor, depth, stencil, rasterizer discard, polygon smooth, color logic op et sample coverage/alpha-to-coverage.
- Clear avant **chaque** rendu : `(0,0,0,1)`.
- Lecture du FBO explicite, `GL_PIXEL_PACK_BUFFER=0`, pack alignment 1, row length et skips nuls ; `glReadPixels(..., GL_RGBA, GL_UNSIGNED_BYTE, ...)`.
- Vérifier les erreurs GL après setup, upload, draw et lecture ; ne pas les purger pendant les intervalles vérifiés.

### Attendus indépendants

Coordonnées de lecture OpenGL, origine **en bas à gauche** ; centres `(x+0.5,y+0.5)`.

À `f2Scalar=0`, les sommets écran sont `(4,4)`, `(28,4)`, `(16,28)`. À `0.5`, ils deviennent `(12,12)`, `(36,12)`, `(24,36)` avant clipping ; `z=0.5` reste dans le volume de clipping.

| Pixel | Float `0` | Float `0.5` |
|---|---|---|
| `(8,8)` | blanc | fond |
| `(24,24)` | fond | blanc |
| `(18,16)` | blanc | blanc |
| `(2,2)` | fond | fond |

- Blanc : **`(255,255,255,255)`**.
- Fond : **`(0,0,0,255)`**.
- Comparaison exacte, sans tolérance ajustée après résultat.
- Les quatre positions sont hors des arêtes dans les deux configurations ; `(18,16)` garantit un intérieur commun non vide.
- Exécuter séparément legacy et préparé, mêmes états et clear ; chaque route satisfait d’abord ces attendus, puis leur égalité est vérifiée.
- Contrôle discriminant : supprimer le second update doit casser les pixels qui permutent ; rendre tout noir doit casser l’intérieur commun.

**Qualification :** calcul et protocole vérifiés statiquement ici ; aucune lecture GPU exécutée. Les attendus sont fixés, pas calibrés sur une future image. Un désaccord GPU entraîne investigation, pas réécriture accommodante de la référence.

## 6. Protocole d’observation des résolutions

### Trois observations distinctes

| Mesure | Frontière observée |
|---|---|
| **C — recherches CPU par nom** | Opérations de recherche dans la map utilisées par `getUniform` et la préparation ; distinguer une résolution logique des opérations `contains/at/find` réellement faites |
| **L — résolution native** | `glGetUniformLocation`, séparément de la réflexion `glGetActiveUniform` |
| **A — extraction type-erased** | Site `std::any_cast` de `UniformContext::getValue` et tout autre site atteignable depuis le nouvel upload |

**Implémentation d’observation retenue :** petits compteurs passifs de test aux sites CPU concernés, sous le mode mock existant, et attentes du mock GL existant. Pas de nouveau framework, cible de build ou modification de la STL. Les hooks ne changent pas le comportement du chemin testé et ne deviennent pas une API produit.

Une revue du graphe d’appels et des corps complète les compteurs : elle doit exclure recherche directe via `getUniforms()`, extraction alternative ou branche conditionnelle rendant le mock différent du vrai upload.

Pour le consommateur GL réel, un espion local RAII pass-through du point d’entrée GLEW `glGetUniformLocation`, sauvegardé/restauré, permet de compter **L** sans remplacer le driver. Il reste limité au test, sous bail GPU exclusif.

### Intervalles et oracles

1. **Setup/réflexion :** journal séparé ; aucune exigence de zéro résolution ici.
2. **Préparation :** journal C/L/A distinct ; une location déjà réfléchie peut éviter une nouvelle résolution native.
3. **Updates :** trois valeurs exactes distinctes, par exemple `0`, `0.5`, `-0.25`. Aucune assertion de lecture ou action legacy dans l’intervalle :
   - `ΔC=0`, `ΔL=0`, `ΔA=0` ;
   - aucune nouvelle réflexion/résolution de type ;
   - trois uploads recevant exactement leurs arguments.
4. **Après intervalle :** contrôler miroir CPU et readbacks. Leurs propres casts/lookups ne contaminent pas les compteurs.
5. **Sensibilité :**
   - recherche CPU connue → C augmente ;
   - appel natif connu → L augmente ;
   - GET historique valide → A augmente.
6. **Contrôle causal borné par le runner :** réintroduire séparément une recherche CPU, une résolution native, puis le GET type-erased dans le chemin préparé doit faire échouer l’assertion correspondante. Archiver ces deltas distinctement du candidat final, puis revenir au candidat sans mutation.

Ne pas annoncer « zéro lookup » à partir du seul compteur L. Aucun compteur d’allocations, benchmark ou engagement zéro allocation n’est requis.

## 7. Registre inverse des preuves existantes

| Test/source existant | Rattachement utile | Limite |
|---|---|---|
| `UniformCardinalityComponentTests.cpp:ScalarVectorMatrixSingleValuesReadBack` | O6 : oracle legacy scalar/vector/matrix, uploads puis `glGetUniformfv` | **static-presence** seulement ici ; ni accès préparé, ni pixels |
| `ReflectedProgramGuardCrossTests.cpp:ReflectedOwnerRejectsOtherCurrentProgramAndRecovers` | O5/O7 : même location target/decoy, isolation, miroir et récupération | **static-presence** ; doit migrer target ; aucun nouvel emprunt préparé prouvé |
| `UniformValueTests.cpp:GetValueReturnsTheStoredValue`, tests uninitialized/type mismatch/remplacement | O6 : diagnostics et miroir CPU historiques | Corps lus ; aucune exécution de mission |
| `UniformReaderTests.cpp:UniformReaderTests::reflect` et tests de réflexion | O1/O3 : comportement driver simulé et résolution native observable | Mock, pas preuve de réflexion GPU réelle |
| `UniformCardinalityComponentTests.cpp:ReflectedArrayRejectsSingleValueWithoutChangingEitherElement` | O6 : non-régression array legacy | Ne prouve pas le rejet du **nouvel** accès pour array `[1]` |
| `DoubleSingleValuesReadBackWhenFp64IsAvailable` | Non-régression historique hors nouvelle API float | Skip explicite possible ; ne remplace aucune obligation float |
| 555 normaux/555 ASan de `a96104c` | Contexte historique attribué au runner précédent par les documents | Preuve périmée pour la nouvelle tranche ; pas de transfert automatique |

Aucun test orphelin n’est démontré dans cet échantillon ; l’inventaire n’est pas exhaustif. Ajouter des variantes vector/matrix pour prétendre couvrir le préparé float serait un doublon faible.

Pour ces présences statiques : **source locale lue le 2026-10-27, HEAD observé ci-dessus ; SHA256, delta vérifié, commande d’exécution, runner, environnement et comptages non fournis**. Elles ne sont donc pas `COVERED` pour le comportement de mission.

Chaque preuve nouvelle devra porter : obligation, HEAD, SHA256, delta, source/date, commande, runner, environnement, attendu/observé, découverts/sélectionnés/exécutés/échoués/skipped et JUnit. Candidat différent : réévaluation obligatoire.

## 8. Runner et résultats actuels

**Aucune des cinq commandes convenues n’a été exécutée par ce profil.**

| Commande convenue | Résultat actuel |
|---|---|
| `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit` | **NOT_RUN / INCONCLUSIVE** |
| `ctest --test-dir out/build/windows-msvc-debug -N` | **NOT_RUN / INCONCLUSIVE** |
| `ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error -j 1` | **NOT_RUN / INCONCLUSIVE** |
| `ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1` | **NOT_RUN / INCONCLUSIVE** |
| `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer` | **NOT_RUN / INCONCLUSIVE** |

Comptages : **non recueillis**, et non « zéro test découvert ».

Le runner lead inspecte d’abord CMake/presets/Conan/CI et le wrapper ASan, sans migration implicite. Il établit toolchain/STL/ABI/CRT/SDK, dépendances, GPU/driver et environnement normalisé. Reconfiguration pour nouveaux `.cpp`, puis découverte et sélection réellement recoupées.

Un build par sortie/configuration ; un écrivain par sortie ; GL sérialisé avec **bail inter-processus/worktrees**, pas seulement `RESOURCE_LOCK artist_opengl`. Conserver les archives normales et ASan séparées. ASan sans erreurs ne prouve ni cible GPU correcte ni pixels.

## 9. Budget et barrières de remise

Budget indicatif, sans consommation passée inventée :

| Poste | Unités |
|---|---:|
| Setup : fixtures, observation, environnement et identité | **20** |
| Lifetime/publication/moves/ASan | 20 |
| Cible/contexte | 12 |
| Préparation/résolution/compatibilité | 12 |
| Consommateur/pixels | 10 |
| Gaps et audit | 6 |
| Buffer nommé : investigation, panne runner, ré-audit | **20** |
| **Total** | **100** |

Hors setup : **60 planifiés / 20 buffer = 75/25**. Prioriser les négatifs hauts peu coûteux, puis transitions lifetime coûteuses ; ne pas retirer un contrôle haut ou le buffer pour préserver une extension moyenne. Si setup ou corrections dépassent l’enveloppe, déclarer le budget irréalisable et réestimer.

### Critères de sortie et séquence

1. **H0 avant auteur :** qualification native attestée par le lead, restitution reçue/validée, contrat et présent plan réconciliés puis commités sélectivement ; FOREIGN préservé.
2. Publier seulement `prepared-float`, avec le vrai ID QA et le contrat structuré de `publication-plan.json` ; vérifier H0 dans son worktree.
3. Auteur : RED causal avant implémentation, GREEN minimal, refactor/non-régression ; aucun environnement absent ne vaut RED.
4. **H1 intégré** avant publication du consommateur ; **H2 intégré** avant revue lifetime fresh et génération des gaps.
5. Propositions de gaps dispositionnées explicitement ; corrections intégrées, runner lead et audit test-reviewer fresh hors board ; ré-audit si candidat modifié.
6. Documentation seulement à la portée livrée et prouvée ; clôture par le lead après intégration.

Les cinq fiches différées restent **non importées maintenant**. Pas de seconde importation globale.

### Points restant ouverts

**Bloquants pour l’admission des auteurs :**
- attestations natives profil/fresh/permissions et réception/validation de cette réponse ;
- vérification du delta, persistance et commit H0 par le lead.

**Gaps de livraison, attendus avant implémentation :**
- tous les nouveaux tests, RED/GREEN, observations C/L/A, pixels et campagnes normales/ASan restent à produire ;
- la correspondance des nouveaux chemins/symboles/tests sera gelée avant édition et complétée dans le registre.

**Hors portée :** autres backends, concurrence, relink/delete externes non notifiés, Linux/Release, système général d’images/instrumentation, benchmark et promesse zéro allocation.

**Conclusion :** protocole proposé réconcilié, attendus pixel chiffrés et observation CPU/native/type-erased séparée. **Aucun PASS comportemental du futur accès, aucune admission globale auto-certifiée.** Le lead conserve la barrière jusqu’à validation native et commit H0.
