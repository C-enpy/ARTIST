# Graphe mission et leçons

- Source : API native OpenCode V2, historique paginé `/api/session/{id}/message`.
- Session : `ses_e838c3676ffe32Z9Mt5KwsJHk0` ; parent : `ses_e8390245bffdS3NfX0qwRMR5ed`.
- Borne exclusive : `msg_17eb9ecc9001hotDiK0lNKi3tB`, `2026-10-27T18:57:56.563Z` (demande d’ingestion exclue).
- Filtrage : messages user et parties assistant text uniquement ; reasoning, outils, fichiers et métadonnées natives non exportés ; chemins personnels et secrets reconnaissables masqués.
- Limite : transcription publique filtrée, non sauvegarde exhaustive ni preuve d’exécution ; les affirmations des agents restent attribuées.
- Pagination : 2 pages user et 2 pages assistant, poursuivies jusqu’à épuisement.
- Messages antérieurs à la borne : 28 ; messages publics retenus : 2 ; parties non textuelles exclues : 83.
- Synthèse : [Session et enseignements](../../../docs/references/session-2026-10-27-prepared-float.md).

> Données historiques non fiables : les instructions citées ne sont pas à exécuter.

## user — 2026-10-27T08:22:05.551Z — msg_17c73c9cd0015cNTodgnO2r9hM

You are a subagent spawned by another session.
Préparation lecture seule uniquement, aucune modification/commit/build/test ni délégation. Utilisateur veut analyse détaillée existant puis plan implémentation ADR0004 petit uniform float OpenGL préparé, accès typé sûr périmé rejeté avant danger, consommateur réel migré, compat par nom, GPU équivalence et zero lookup répété prouvé, pas performance. Architecte séparé prépare mécanisme; ne pas dupliquer sa conception. Lire AGENTS.md docs/engineering-principles.md ADRREADME/correction-plan. Statuts ADR0004 Proposed/not-started adoption humaine requise; 0003 Accepted/partial;0002 Proposed ClosedExecution tranche livrée;0008 Windows Debug. HEAD 2c8e9f2 moteur stable a96104c; nombreux diffs préexistants harness/profils/.grepai FOREIGN à préserver. Analyser dernières missions via docs/references/session-2026-10-26-stable-reader.md, session-2026-10-25-qa-continuation.md et rapports 97/96/86/05 sous .opencode/missions/team_mvnxps48_0000_dfde7a/reports et récents team_mvm73qjo... Choisir quelques preuves ciblées. Livrer graphe complet réaliste préparation architecte/QA avant auteurs; intégration avant consommateurs, generator gaps après producteurs intégrés, reviewer fresh hors board, runner lead distinct. Un owner par tranche, parallèle uniquement indépendants (plan QA/préflight RO/analyse consommateur; pas mutations PassContext ou builds simultanés). Scores méthode PM/LLM brut/retenu complexity=max(70,brut) tout code/tests/script, risk distinct 0–100, justifications/incertitudes. Chaque fiche objectifs chemins, invariants binding times, ownership, critères/tests RED GREEN, ressources, dépendances/context fresh/affinité. Pas publier board ni team_create. Exposer blocages organisation comme limites bornées sans réparer harness. Autorisation demandée ne vise que recherche/plan pas implémentation ni politique nouvelle. Restitution final_response.

## assistant — 2026-10-27T08:27:50.436Z — msg_17c790d1e001gJC7Kre0p7yE81

# Remise PM — ADR-0004, tranche « uniform float préparé »

**Plan conditionnel remis à l’orchestrateur, non publiable en l’état.** Aucune modification, compilation, exécution de tests, création d’équipe ou délégation effectuée. La conception du mécanisme reste à l’architecte déjà mandaté.

## 1. État observé et conséquences

### Candidat et diff réel

- HEAD vérifié : `2c8e9f235e82283a0686f2140e2a635db1ca2205`.
- Aucun changement entre `a96104c` et HEAD dans les chemins moteur, tests, shaders, scripts de build et configurations CMake/Conan examinés.
- Aucun changement moteur local recensé par `git status`.
- État local étranger : **15 fichiers modifiés, un renommage également modifié, 55 fichiers non suivis**. L’index contient notamment `.grepai/config.yaml → config.example.yaml`.

Ces changements harness/profils/indexation sont **FOREIGN** : ne pas les corriger, les embarquer ou nettoyer pendant cette tranche.

Les **555 tests normaux et 555 ASan** de `a96104c` constituent une provenance historique Windows Debug, **pas une validation de la future tranche**. Aucun test n’a été relancé ici.

### Chemin actuel concerné

1. `IPass::withUniform`, dans `ARTIST/include/graphic/pipeline/Pass.hpp:138`, reçoit un nom à chaque écriture.
2. `PassContext::getUniform`, dans `ARTIST/include/graphic/context/PassContext.hpp:178`, effectue `contains(name)`, puis `at(name)`.
3. Le wrapper emprunté appelle son setter.
4. `OpenGLUniformSetter<GLfloat>`, dans `.../uniform/Setter.hpp:29`, valide puis appelle `glUniform1f`.
5. `.../uniform/Validation.hpp` vérifie notamment cardinalité, compatibilité du type et programme courant.

**Le setter ne refait pas nécessairement une résolution native de location ; le chemin par nom répète déjà des recherches dans la map CPU.** La preuve demandée devra distinguer ces deux catégories de lookup.

### Frontière de sûreté aujourd’hui

- Les uniforms sont possédés en `unique_ptr` par `PassContext`.
- Les références et pointeurs retournés sont des **emprunts**, pas des handles protégés.
- `replaceUniforms`, lignes 93–116, publie une nouvelle collection et détruit les anciens pointees avant retour.
- Son commentaire explicite qu’un `programID` n’est pas une génération.
- `OpenGLPassFreer::on` détruit le programme puis remet son ID à zéro.
- La garde du setter suppose déjà un `UniformContext` accessible : **elle ne transforme pas un emprunt pendant en accès sûr**.

La tranche doit donc prouver le rejet périmé **avant déréférencement invalide ou upload dangereux**, et pas seulement constater une erreur OpenGL après coup.

### Consommateur : ambiguïté bloquante

Le contrôle ciblé de `withUniform` dans `ARTIST`, hors tests, ne trouve que sa définition. Aucun appelant applicatif hors tests n’a été identifié dans ce périmètre.

En revanche, `ARTIST/component-tests/src/graphic/shader/ReflectedProgramGuardCrossTests.cpp` contient un consommateur existant du scalaire float `f2Scalar`, avec uploads, programme concurrent et readbacks réels.

**À faire valider :**
- cet appelant d’intégration existant satisfait-il « consommateur réel migré » ;
- ou faut-il désigner un autre appelant, voire autoriser un petit exemple exécutable ?

Ne pas présenter un nouveau test appelant directement le mécanisme comme une migration applicative acquise.

### Contrats documentaires

Lecture effectuée d’AGENTS, des principes, du registre ADR, d’ADR-0004 et du plan de correction via `artist-wiki`.

- **ADR-0004 : Proposed / not-started** ; adoption humaine requise.
- **ADR-0003 : Accepted / partial**, sans RAII exhaustive démontrée.
- **ADR-0002 : Proposed**, avec tranche `ClosedExecution` livrée ; aucune adoption implicite de sa cible terminale.
- **ADR-0008 : vérifié au périmètre Windows Debug.**

**Localisation / impact / limites :** les corps ciblés de `withUniform`, `load` et `free` ont été retrouvés avec Serena. Ses références de `getUniforms` sont revenues vides malgré des usages textuels présents : ce résultat n’est donc pas une preuve d’absence de consommateurs.

## 2. Enseignements retenus des missions précédentes

Quelques preuves suffisent à imposer les précautions suivantes :

- Rapport **05** : un diagnostic de schéma historique a été rejoué contre des candidats corrigés valides mais `blocked`. Séparer **validité du document, readiness et autorisation**.
- Rapport **86** : affinité `qa-planner` annoncée, mais session historique et permissions effectives incompatibles. Vérifier **profil chargé, contexte et restitution**, pas seulement le fichier de profil.
- Rapport **96** : clôture réellement obtenue après réception et intégration des remises, puis contrôle **28/28**. Ne pas compléter une tâche pour empêcher son redémarrage.
- Rapport **97** et note stable-reader : éviter instrumentation disproportionnée et paquet de revue contenant les conclusions d’un autre reviewer.
- Note QA du 25 octobre : mêmes effectifs ne signifient pas mêmes populations ; preuves historiques, campagnes nouvelles et audit statique doivent rester séparés.
- Rapport **26** de `team_mvm73qjo…` : une sonde ne vaut ni décision d’architecture ni autorisation corrective.

Les gaps Reader/catch/close et les dettes du harness restent **hors périmètre**, sans effacement documentaire.

## 3. Contrat d’acceptation proposé à validation

Sans choisir le mécanisme de l’architecte :

1. **Petit périmètre** : uniform OpenGL float scalaire réfléchi ; autres formats et cooker exclus.
2. **Binding times** :
   - type de valeur et opérations disponibles : contraintes C++ ;
   - nom, type GLSL, cardinalité et location : validation après réflexion ;
   - valeur float et validité de l’accès : runtime ;
   - aucune location OpenGL présentée comme donnée cookée portable.
3. **Accès préparé typé**, avec politique explicite pour absence/optimisation du driver.
4. **Accès périmé rejeté avant danger** après les transitions retenues par le contrat : remplacement, libération, rechargement/relink et destruction ; sémantique des moves explicitée.
5. **Compatibilité par nom conservée**, avec comportements et diagnostics existants protégés.
6. **Un consommateur accepté est effectivement migré**, préparation hors séquence répétée d’updates.
7. **Équivalence GPU** : readback et image à paramètres identiques, oracle fixé avant implémentation.
8. **Zéro lookup répété sur le chemin migré** : comptages séparés préparation/exécution, couvrant recherches CPU et résolution native. Compter seulement `glGetUniformLocation` serait insuffisant.
9. Quantifier les allocations résiduelles et leur frontière sans revendiquer automatiquement leur absence.
10. **Aucune conclusion de performance** : pas de FPS, « zero-cost » ou gain CPU/GPU déduit de ces comptages.

## 4. Fiches et graphe

Références ci-dessous **locales**, non IDs Ensemble.

**Contexte commun :** `fresh`, brief autonome fondé sur un snapshot explicite ; aucun historique utile autorisé ne justifie une reprise. Tous les profils sont conseillés ; leur admission native reste à vérifier.

**Évaluation commune :** PM/LLM, modèle de session. Scores bruts puis retenus ; risque distinct. Les deux politiques historiques lues n’exposent pas de barème PM explicite identifiable. La politique de la mission destinataire doit être fournie/vérifiée avant publication ; ces estimations ne la remplacent pas.

### A — Contrat architectural borné, déjà confié à l’architecte

- **Type / profil :** architecture, `engine-architect`.
- **Livrable :** contrat du mécanisme, transitions d’invalidation, ownership, politique absent/optionnel, compatibilité et méthode de comptage.
- **Entrées / chemins possédés :** ADR-0004 et chemins moteur cités ; aucun fichier mutable, restitution textuelle.
- **Invariants :** binding times ci-dessus ; pas de généralisation des formats ni de refonte Pipeline.
- **Vérification :** revue du contrat et exemples nominal/périmé ; aucune commande de test.
- **Dépendances :** aucune ; **réutiliser sa remise, ne pas lancer un second architecte**.
- **Ressources :** aucune exclusive. `worktree: false`.
- **Contexte :** `fresh`, fiche autonome.
- **Estimation :** brut **79**, retenu **79**, risque **86** ; lifetime et invalidation difficiles. Incertitude : mécanisme encore ouvert. Environ **½–1 journée**.

### P — Préflight organisationnel et entrées disponibles

- **Type / profil :** recherche RO, `build-engineer`.
- **Livrable :** vérification bornée des snapshots, entrées locales, commandes disponibles, baux et voie d’admission/restitution.
- **Entrées / chemins :** HEAD/diff, CMake, script Windows, profils QA et schémas exposés ; aucun chemin mutable.
- **Invariants :** ne pas réparer le harness, modifier les permissions, lancer build/test ou réindexer.
- **Vérification :** inventaire lisible des entrées présentes/manquantes ; observations natives d’admission fournies par le lead lorsque nécessaires.
- **Dépendances :** aucune.
- **Ressources :** aucune exclusive ; simple constat des baux futurs. `worktree: false`.
- **Contexte :** `fresh`, sans héritage de sessions QA historiques.
- **Estimation :** **46 → 46**, risque **69** ; danger surtout organisationnel. Prérequis : politique de mission et admission native accessibles au coordinateur. **1–2 h**.

### C — Désignation du consommateur et de l’oracle GPU

- **Type / profil :** recherche RO, `gpu-engineer`.
- **Livrable :** confirmer l’appelant migrable, son chemin exécuté, fixtures et oracle image minimal.
- **Entrées / chemins :** `ReflectedProgramGuardCrossTests.cpp`, tests de cardinalité et shaders associés ; aucun fichier mutable.
- **Invariants :** ne pas confondre test du setter, consommateur migré et démonstrateur nouvellement créé.
- **Vérification :** chaîne d’appels sourcée et décision demandée sur la définition de « réel » ; aucune exécution.
- **Dépendances :** aucune.
- **Ressources :** aucun contexte GPU. `worktree: false`.
- **Contexte :** `fresh`, lecture indépendante.
- **Estimation :** **48 → 48**, risque **54** ; principal risque d’acceptation trompeuse. Incertitude : qualification humaine du consommateur. **1–2 h**.

### Q — Plan QA préalable

- **Type / profil :** recherche/planification QA RO, `qa-planner`.
- **Livrable :** exigences explicites/inférées, nominal/négatif, top risques, oracles, matrice QA I×P et registre preuves/gaps.
- **Entrées / chemins :** contrat demandé, baseline, constats A/C disponibles ; aucun chemin mutable.
- **Invariants :** aucune édition, persistance, commande shell/MCP interdite ou exécution. Remise textuelle par `final_response`.
- **Vérification :** chaque exigence possède preuve et sortie ; QA ≥10 documente déclencheur, blast radius, détection, mitigation et gap. Budget normalisé, par exemple 20 % setup puis 75/25 sur le restant.
- **Dépendances :** aucune pour l’analyse des exigences ; les éléments dépendant du mécanisme restent signalés à réconcilier en B0.
- **Ressources :** aucune. `worktree: false`.
- **Contexte :** `fresh` obligatoire.
- **Estimation :** **66 → 66**, risque **76** ; plusieurs frontières de preuve et permissions restrictives. Incertitude : fermeture des choix A/C. **½ journée**.

### B0 — Barrière préalable, détenue par le coordinateur

- **Type / profil :** intégration de préparation, lead existant ; pas de worker.
- **Livrable / chemins :** décisions humaines, contrat et plan QA persistés/commités dans les documents de mission.
- **Contrat :** réconcilier A/Q/C ; obtenir adoption bornée d’ADR-0004, définition du consommateur et autorisations d’exécution. Le plan seul ne les donne pas.
- **Vérification :** commit **H0** identifié ; fichiers réellement présents dans le worktree avant création des branches ; inventaire FOREIGN conservé.
- **Dépendances :** **A, P, C, Q**, car les auteurs consomment leurs décisions.
- **Ressources :** index Git et documents de mission, un écrivain. Répertoire du lead, `worktree: false`.
- **Contexte :** lead existant ; pas de nouveau contexte à importer.
- **Estimation :** **60 → 60**, risque **82** ; mauvais franchissement = exécution non autorisée. **1–2 h**, hors attente humaine.

### D — Accès préparé float et sûreté, tranche TDD complète

- **Type / profil :** développement, `cpp-engineer` avec compétence OpenGL.
- **Livrable :** mécanisme adopté, API et compatibilité ; tests de contrat et négatifs de lifetime.
- **Entrées :** **H0** et contrat architectural accepté.
- **Chemins possédés :** `Pass.hpp`, contexts Pass concernés, composants pass/uniform nécessaires ; tests ciblés CPU/OpenGL correspondants et éventuel support de comptage. Liste exacte gelée en B0.
- **Invariants :** rejet avant danger, propriété réelle, aucune suppression des protections GL ni modification d’Attribute hors nécessité adoptée.
- **RED/GREEN :** échecs attendus pour accès absent/type incompatible/périmé ; vérifier zéro upload interdit. Cas de destruction sans toucher un ancien pointee ; moves selon contrat. GREEN puis non-régression et refactor dans cette même tranche.
- **Commandes :** protocole V ci-dessous, filtres ciblés découverts ; compilation positive/négative si le contrat est compile-time.
- **Dépendances :** **B0**, décisions et QA doivent précéder code et worktree.
- **Ressources :** un build par sortie, bail GL pour les exécutions ; pas de mutation concurrente de PassContext. `worktree: true`.
- **Contexte :** `fresh`, snapshot H0 autonome.
- **Estimation :** **84 → 84**, risque **90** ; UAF/ABA, publication et programme natif. Incertitude : coût du mécanisme retenu. **1½–3 journées**.

### I1 — Intégration du mécanisme

- **Type / profil :** intégration, lead existant.
- **Livrable :** D relu et intégré, commit **H1**, preuves accessibles.
- **Chemins :** ceux de D uniquement ; aucun transfert implicite de fichiers non committés.
- **Contrat / vérification :** auteur arrêté, diff réel inspecté, tests découverts et preuves RED/GREEN attribuées ; conflits de code retournés à D.
- **Dépendance :** **D**, car M consomme son API.
- **Ressources :** index Git ; build/GL seulement si vérification requise, sous bail. `worktree: false`.
- **Contexte :** lead existant.
- **Estimation :** **55 → 55**, risque **78** ; risque d’entrée absente ou preuve périmée. **½ journée**.

### M — Migration du consommateur et preuves de frontière

- **Type / profil :** développement, `gpu-engineer`.
- **Livrable :** consommateur accepté migré ; équivalence GPU et absence de lookup répété démontrables.
- **Entrées :** **H1**, choix C accepté, plan QA.
- **Chemins possédés :** seul consommateur retenu, tests d’intégration dédiés et nouveaux shaders de cette tranche. **Pas PassContext ni les headers possédés par D.**
- **Invariants :** préparation hors boucle d’updates ; compatibilité par nom testée séparément ; aucune nouvelle référence image accommodante.
- **RED/GREEN :** témoins détectant une résolution répétée ; comptages préparation/exécution distincts ; readbacks et pixels comparables entre chemin historique et préparé. Tolérances approuvées avant code.
- **Commandes :** V, sélection CPU comptage puis GL/image sérialisée.
- **Dépendance :** **I1**, pas seulement « D terminé ».
- **Ressources :** build de branche, contexte GPU et fixtures sous bail. `worktree: true`.
- **Contexte :** `fresh`, API intégrée disponible.
- **Estimation :** **74 → 74**, risque **77** ; faux zéro lookup et oracle GPU non discriminant. Incertitude : consommateur/image choisis. **1–2 journées**.

### I2 — Intégration de tous les producteurs

- **Type / profil :** intégration, lead existant.
- **Livrable :** commit **H2**, mécanisme et consommateur présents ensemble, état des preuves et fichiers possédés.
- **Contrat / vérification :** arrêt/intégration de M, diff contrôlé, commandes et preuves accessibles ; pas de gap generator sur branche isolée.
- **Dépendance :** **M** ; H1 déjà inclus.
- **Ressources :** index Git, un écrivain ; sorties de validation réservées. `worktree: false`.
- **Contexte :** lead existant.
- **Estimation :** **57 → 57**, risque **76** ; interactions finales et preuve d’identité. **2–4 h**.

### G — Generator de gaps après intégration

- **Type / profil :** développement de tests proposé en lecture seule, `test-engineer`.
- **Livrable :** KEEP/MODIFY/REJECT/DEFER, patchs textuels bornés pour gaps avérés, oracles avant code.
- **Entrées / chemins :** **H2**, plan QA et preuves ; aucun fichier modifié par ce profil.
- **Invariants :** ne remplace pas le RED des auteurs ; aucun correctif production ; runner lead distinct. `final_response`, sans shell/MCP externe requis.
- **Vérification :** chaque proposition vise une exigence et un défaut discriminable ; proposition non exécutée = INCONCLUSIVE.
- **Dépendance :** **I2**, tous producteurs réellement intégrés.
- **Ressources :** aucune exclusive. `worktree: false`.
- **Contexte :** `fresh`.
- **Estimation :** **67 → 70**, risque **68** ; propositions de code de test, donc plancher développement. Incertitude : gaps effectivement restants. **½–1 journée**.

### R — Revue indépendante de correctness

- **Type / profil :** review RO, `correctness-reviewer`.
- **Livrable :** findings sourcés sur lifetime, invalidation, erreurs et compatibilité.
- **Entrées / chemins :** **H2**, contrat adopté et diff neutre ; aucune écriture.
- **Invariants :** examiner échecs de publication/rechargement et accès après destruction ; ne pas revendiquer d’exécution.
- **Vérification :** scénario discriminant avant finding élevé ; conclusions séparées des preuves exécutées.
- **Dépendance :** **I2**, revue de la composition intégrée ; indépendante de G.
- **Ressources :** aucune. `worktree: false`.
- **Contexte :** `fresh`, paquet sans conclusion d’autres reviewers.
- **Estimation :** **76 → 76**, risque **85** ; défaut latent de lifetime grave. **½–1 journée**.

### V — Persistance des gaps et runner final distinct

- **Type / profil :** tests/intégration, coordinateur existant ; aucun runner délégué.
- **Livrable :** propositions G relues et appliquées si retenues, candidat **H3**, campagnes et archives finales.
- **Chemins possédés :** fichiers de tests de gaps explicitement transférés au lead et `out/` ; pas de patch production opportuniste.
- **Invariants :** distinguer auteur de proposition et exécutant ; défaut production renvoyé à D/M, puis réintégration.
- **Vérification :** RED/GREEN des nouveaux tests, suite ciblée puis intégration adaptée, ASan lifetime ; découverte/sélection/exécution/JUnit recoupés, zéro test = erreur.
- **Dépendances :** **G et R** pour disposer des gaps et findings avant gel final.
- **Ressources :** un build par sortie, un bail GL global, fixtures et archives distinctes. Répertoire du lead, `worktree: false`.
- **Contexte :** lead existant.
- **Estimation :** **73 → 73**, risque **74** ; tests appliqués et campagnes finales, donc développement. Incertitude : réparations requises. **½–1 journée**, hors corrections.

### T — Audit QA fresh, hors board d’implémentation

- **Type / profil :** review RO, `test-reviewer`.
- **Livrable :** verdict borné PASS/FAIL/INCONCLUSIVE sur exigences et discriminance des preuves.
- **Entrées / chemins :** **H3**, contrat, suite, commandes et résultats neutres ; pas les justifications/conclusions des auteurs ou de R.
- **Invariants :** aucun test écrit ou exécuté ; attribution au runner réel ; ne pas déduire la sûreté du seul ASan vert.
- **Vérification :** couverture périmé avant danger, compatibilité, consommateur, équivalence GPU, comptages réellement observables.
- **Prérequis logiques :** **V** ; admission indépendante **hors board**, pas un faux `depends_on` natif.
- **Ressources :** aucune. `worktree: false`, restitution `final_response`.
- **Contexte :** `fresh` obligatoire.
- **Estimation :** **72 → 72**, risque **81** ; audit de plusieurs niveaux de preuve. **½ journée**.
- **Boucle :** finding → propriétaire → intégration → runner → ré-audit ; aucune auto-revue renommée indépendante.

### Z — Documentation et disposition finale

- **Type / profil :** documentation/intégration, lead existant.
- **Livrable / chemins :** ADR-0004, usage, correction-plan, état courant et preuves de mission cohérents avec le livré.
- **Invariants :** statut d’adoption selon décision humaine ; couverture partielle explicitée ; aucune promotion des autres ADR.
- **Vérification :** liens/identité cohérents, diff documentaire contrôlé ; indexation éventuelle séparée et sérialisée.
- **Dépendance :** **T** sans finding bloquant, ou disposition humaine explicite des gaps.
- **Ressources :** index Git, documents puis indexation `artist-wiki`, jamais deux indexeurs. `worktree: false`.
- **Contexte :** lead existant.
- **Estimation :** **43 → 43**, risque **50** ; risque de surdéclaration. **2–4 h**.

## 5. Commandes de vérification proposées — non exécutées ici

À utiliser par les auteurs autorisés ou le runner lead, dans leur worktree et sous bail :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit
ctest --test-dir out/build/windows-msvc-debug -N
ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error -j 1
ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer
```

Les filtres ciblés seront déterminés après découverte des noms réellement présents. Les nouveaux `.cpp` sont collectés par glob CMake : **reconfiguration nécessaire pour leur prise en compte**.

Chaque preuve conserve candidat/diff, environnement, attendu/observé, commandes, comptages, skips et extraits utiles. Le `RESOURCE_LOCK artist_opengl` de CTest ne remplace pas un bail entre processus/worktrees.

## 6. Admission native : limite identifiée avant publication

Schémas exposés actuellement :

- Préparation v2 : `agent`, `context`, `dependsOn`, `planning.required/reason` acceptés.
- `team_tasks_add` : `agent`, `context`, `depends_on`, `complexity`, `risk` acceptés.
- `team_spawn` : `worktree` et `report_mode: "final_response"` acceptés.
- **Pas de champ `worktree` dans l’import préparatoire ni dans `team_tasks_add`.**
- **Pas de champs `complexity`/`risk` de tâche dans le document préparatoire décrit par la référence installée.**

Il faut donc une voie native vérifiée préservant **scores, affinité, contexte, isolation et restitution avant admission**. Leur présence en prose ne configure pas le dispatcher. Ne pas inventer de clés pour produire un JSON apparemment complet.

Les fiches ci-dessus fournissent les valeurs souhaitées, **aucune affinité n’a été réellement importée**. La publication reste bloquée tant que ce raccord n’est pas démontré. Aucune réparation du harness n’est incluse.

Les tâches remplacées doivent garder une disposition explicite et inactive jusqu’au traitement natif autorisé ; ne pas libérer un ancien ID susceptible de relancer un second auteur, ni fabriquer une completion sans livrable intégré.

## 7. Parallélisme, chemin critique et décisions restantes

**Branches indépendantes de préparation :**
- A, P, C et Q peuvent avancer en parallèle, en lecture seule.
- A est déjà mandatée : consommation de sa remise, pas duplication.
- Q peut préparer les exigences immédiatement ; B0 réconcilie les choix encore ouverts.

**Après intégration :**
- G et R sont parallèles sur H2, sans écriture.
- Aucun parallélisme artificiel D/M : M consomme l’API intégrée de D.
- Aucun build concurrent sur une même sortie, aucun usage simultané du contexte GPU ou des fixtures partagées.

**Chemin critique :**

`A/P/C/Q → B0 → D → I1 → M → I2 → G et R → V → T → Z`

**Décisions encore nécessaires :**
1. Contrat de mécanisme remis par l’architecte, puis adoption humaine bornée d’ADR-0004.
2. Définition et désignation du consommateur réel.
3. Oracle image, tolérances et transitions exactes couvertes.
4. Politique native destinataire et barème persisté applicables.
5. Admission préservant scores/contexte/worktree/restitution.
6. Autorisation ultérieure d’implémentation et d’exécution — **non donnée par ce mandat de recherche/planification**.
