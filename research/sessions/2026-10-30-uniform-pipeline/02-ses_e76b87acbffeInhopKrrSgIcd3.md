# Évaluer prochaine mission fonctionnelle

Session: ses_e76b87acbffeInhopKrrSgIcd3
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T20:09:15.976Z — msg_18947f1c50013ddWA0iQrmrlSK

## Recommandation

**Oui à cette mission, comme expérience bornée, pas comme optimisation promise.**

**Résumé non technique :** vérifier si préparer une fois le réglage d’un shader réduit réellement le travail répété, sans changer l’image. Mesurer aussi le coût initial : une préparation plus rapide ensuite peut ne devenir intéressante qu’après plusieurs utilisations. Un résultat sans gain reste utile.

Cette mission est préférable à une extension des types ou à une refonte de `ClosedExecution` : elle exploite une tranche livrée et son oracle image, sans devoir adopter un ADR encore ouvert.

### Fondement et limites

- L’audit documentaire du 29 octobre confirme le manque précis : **image équivalente prouvée, allocations et coût non mesurés** (`docs/references/adr-audit-2026-10-29.md`, lignes 243–283).
- `windows-msvc-release` existe dans `CMakePresets.json` ; cela ne prouve pas sa validation.
- HEAD `fcf14a1`, diff et inspection de `ClosedExecution.hpp` : informations du briefing, non revérifiées ici.
- Les 589 tests de référence sur `78e7b74` ne prouvent ni ce candidat ni ses performances.
- Aucun fichier `model-policy.json` trouvé par recherche locale ciblée. Scores ci-dessous **PM/LLM de préparation**, à confronter à la politique de la future mission avant admission.

## Contrat de mission proposé

**Livrable :** protocole reproductible, harness isolé, données brutes et conclusion limitée au workload et à l’environnement effectivement mesurés.

**Périmètre :**
- float scalaire, OpenGL existant, petite scène fixe ;
- comparaison des deux routes publiques, valeurs et rendu identiques ;
- coûts séparés : préparation, update CPU, total selon nombre d’updates ;
- allocations uniquement si leur périmètre est observable et validé ;
- aucune modification de production, extension de types/backends ou nouvelle dépendance implicite.

**Succès :** comparaison valide et reproductible, pas obligation de gain. Sans Release exploitable ou contexte réel : livrer le diagnostic **INCONCLUSIVE**, sans substituer Debug, mocks ou rendu logiciel à une preuve matérielle.

## Fiches exécutables proposées

Références locales uniquement. Toutes les tâches utilisent **`context: {mode: "fresh"}`** : briefs autonomes, aucune reprise d’historique nécessaire. Estimations en temps actif, hors attentes de compilation.

### Q — Fixer les oracles avant le harness
- **Type / profil :** plan QA, `qa-planner`, `worktree: false`.
- **Livrable :** critères nominaux/négatifs, matrice QA et conditions d’arrêt.
- **Entrées / ownership :** briefing, oracle `PreparedFloatUniformRenderTests.cpp`, contrats documentaires ; aucun fichier mutable.
- **Invariants :** égalité non vide et variation discriminante ; travail A/B égal ; résultats manquants distincts de zéro.
- **Vérification :** revue documentaire par le coordinateur, sans commande imposée au profil restrictif. Celui-ci fournit les extraits autorisés et persiste/commite le plan.
- **Dépendances / exclusivité :** aucune ; aucune ressource d’exécution.
- **Évaluation PM/LLM :** brute **48**, retenue **48**, risque **44**. Difficulté principale : éviter une expérience convaincante mais non discriminante.
- **Prérequis / estimation :** brief accessible, restitution native vérifiée ; **½ journée**.

### E — Qualifier Release et l’environnement de mesure
- **Type / profil :** investigation bornée, `build-engineer`, `worktree: false`.
- **Livrable :** commande reproductible, versions, flags effectifs, identité GL et verdict d’aptitude ; aucun changement de configuration.
- **Entrées / ownership :** HEAD désigné, presets, Conan, scripts existants ; sorties exclusivement dans un répertoire de campagne réservé.
- **Invariants :** Conan avant CMake ; absence d’ASan/capture dans la mesure principale ; matériel et logiciel explicitement distingués.
- **Vérification :** découverte non nulle et tests pertinents en Release ; `cmake --build --preset windows-msvc-release`, puis `ctest --preset windows-msvc-release`, **après** qualification de la commande Conan et du répertoire de sortie. Exécution via Context Mode.
- **Dépendances / exclusivité :** aucune ; bail exclusif sur build/cache écrit et contexte GL lors des tests.
- **Évaluation PM/LLM :** brute **46**, retenue **46**, risque **42**. Présence du preset, mais preuve Release absente.
- **Prérequis / estimation :** toolchain et device accessibles ; **½ journée**. Toute réparation sort du périmètre et revient au coordinateur.

### P — Définir le protocole économique A/B
- **Type / profil :** analyse, `performance-reviewer`, `worktree: false`.
- **Livrable :** scopes chronométrés, charge bornée, warm-up, ordre alterné reproductible, répétitions, dispersion, exclusions et contrôle du surcoût instrumental.
- **Entrées / ownership :** briefing et protocole performance ; restitution uniquement.
- **Invariants :** séparer préparation et update ; ne pas présenter temps CPU comme temps GPU ; conserver les validations propres à chaque route publique.
- **Vérification :** revue du protocole ; définir avant collecte les règles « résultat exploitable / bruit excessif ». Calculer l’amortissement seulement si l’écart d’update est exploitable et favorable.
- **Dépendances / exclusivité :** aucune pour sa rédaction ; aucune exécution.
- **Évaluation PM/LLM :** brute **58**, retenue **58**, risque **52**. Risque de biais et de déplacement de coûts, distinct de la complexité.
- **Prérequis / estimation :** métriques d’allocations facultatives, jamais présumées ; **½ journée**.

### H — Développer et vérifier le harness isolé
- **Type / profil :** développement, `cpp-engineer`, `worktree: true`.
- **Livrable :** harness et tests de son contrat, sans modification de production.
- **Entrées / ownership :** HEAD intégré après Q/E/P ; nouveau sous-répertoire dédié, proposé `experiments/prepared-float-cost/`. Aucun ownership sur les profils, rapports préexistants ou fichiers de production.
- **Invariants :** mêmes séquences de valeurs, draws et état de scène ; préparation exclue du scope update mais incluse dans le bilan total ; observation empêchant l’élimination du travail.
- **RED/GREEN/REFACTOR :** tests du harness constatés rouges avant implémentation : comptages incorrects, résultat manquant, séquences A/B différentes et export incomplet doivent être détectés. Réutiliser l’oracle image existant sans affaiblissement.
- **Vérification :** commandes exactes fournies avec le harness ; découverte non nulle, RED attendu, GREEN et oracle image. Instrumentation d’allocation : contrôle positif obligatoire, portée déclarée, sinon métrique omise.
- **Dépendances / exclusivité :** Q/E/P **intégrés et acceptés** ; build privé, GPU sérialisé.
- **Évaluation PM/LLM :** brute **66**, retenue **70**, risque **57**. Couplage mesure/GL/oracles et risque de faux gain.
- **Prérequis / estimation :** mode de raccordement hors production validé par le coordinateur ; **1–2 jours**.

### G — Rechercher les gaps après intégration
- **Type / profil :** développement de tests, `test-engineer`, `worktree: true`.
- **Livrable :** tests complémentaires seulement si des gaps sont établis ; sinon rapport motivé.
- **Entrées / ownership :** harness H réellement intégré ; uniquement fichiers de tests complémentaires réservés dans son sous-répertoire.
- **Invariants :** ne remplace pas le RED de H ; ne modifie pas les seuils pour obtenir du vert.
- **Vérification :** tests négatifs et commandes du harness ; RED/GREEN/REFACTOR pour tout ajout.
- **Dépendances / exclusivité :** barrière d’intégration H ; build privé et bail GL si nécessaire.
- **Évaluation PM/LLM :** brute **43**, retenue **70**, risque **43**. Surface bornée mais code de test soumis au plancher.
- **Prérequis / estimation :** candidat et plan QA accessibles ; **½ journée**.

### A — Audit indépendant du candidat final
- **Type / profil :** QA lecture seule, `test-reviewer`, `worktree: false`.
- **Livrable :** PASS/FAIL/INCONCLUSIVE par critère, gaps et autorisation méthodologique de collecte.
- **Entrées / ownership :** diff H+G intégré, preuves TDD, protocole et plan QA ; aucun fichier mutable.
- **Vérification :** inspection indépendante des scopes, oracles, instrumentation et preuves. Exécution éventuelle réservée au runner coordinateur.
- **Dépendances / exclusivité :** intégration G ; aucune ressource propre.
- **Évaluation PM/LLM :** brute **52**, retenue **52**, risque **51**. Risque principal : laisser passer un biais systématique.
- **Prérequis / estimation :** profil effectif restrictif vérifié ; **½ journée**.

### M — Collecter puis conclure
- **Type / profil :** campagne expérimentale, `build-engineer` pour le runner ; analyse indépendante par `performance-reviewer`.
- **Livrable :** données brutes identifiées, médiane/dispersion, coût initial et amortissement éventuel ; synthèse documentaire persistée par le coordinateur.
- **Entrées / ownership :** candidat audité A, commandes H, protocole P ; sorties de campagne uniquement.
- **Invariants :** candidat figé ; aucune réparation pendant collecte ; runs exclus conservés et motivés ; aucune conclusion générale « zero-cost ».
- **Vérification :** commandes livrées par H, oracle image avant/après, contrôle d’identité du candidat ; audit indépendant des données. Gain absent ou bruit excessif sont des conclusions recevables.
- **Dépendances / exclusivité :** A accepté ; machine de mesure calme, contexte GL et répertoire de sortie exclusifs.
- **Évaluation PM/LLM :** brute **59**, retenue **59**, risque **56**. Reproductibilité, bruit et portée des conclusions.
- **Prérequis / estimation :** disponibilité machine ; **1 journée**.

## Admission et intégration — responsabilité du coordinateur

- Boucle QA obligatoire : comportement sous contrat et risques PM ≥ 50. **Ne pas convertir ces risques en scores QA impact × probabilité.**
- Le schéma découvert accepte `agent`, `context`, `worktree`, `depends_on`, `complexity`, `risk` et `report_mode: "final_response"`. Utiliser cette restitution pour les profils restrictifs ; pas de clé `profil` inventée.
- Résoudre les références locales en vrais IDs seulement lors d’une publication ultérieure autorisée.
- Avant chaque consommateur : intégrer les commits producteurs, enregistrer HEAD/diff, vérifier les entrées présentes dans son worktree. Aucun transfert implicite du diff non commité.
- Préserver les changements préexistants de profils et rapports/tmp. Une permission ajoutée sur disque ne prouve pas le profil chargé.
- Aucune tâche remplacée identifiée ici. S’il en existe, le coordinateur doit la différer nativement avant remplacement ; ni faux `completed`, ni réadmission générique.

## Graphe et décisions restantes

**Parallèle réel :** Q ‖ E ‖ P. Lectures QA/protocole indépendantes ; seul E utilise build/GPU.

**Chemin critique :** `Q/E/P → intégration du contrat → H → intégration → G → intégration → A → M`.

Le développement du harness reste mono-auteur : séparer artificiellement chronométrage, scène et oracles créerait des conflits et fragiliserait son contrat.

**À valider :** objectif de mission, machine de référence, raccordement isolé du harness et politique native éventuelle. Si ces choix nécessitent une décision architecturale nouvelle, retour au coordinateur avant développement.

**Aucune mission exécutée, tâche publiée, équipe créée ou source modifiée.**

## assistant — 2026-10-29T20:37:29.231Z — msg_18961c80b0015gJFdr3Asj3ccn

## Recadrage

**Mission : livrer un pipeline reproductible Windows/Linux, prêt pour une qualification ultérieure sur ressources louées.**  
Elle ne comprend **aucun build, test, benchmark, provisionnement ni campagne**. La réception porte sur les fichiers livrés, leurs contrats et leur cohérence statique — pas sur un résultat de tests.

Pour l’utilisateur : **livrer l’outil et son mode d’emploi maintenant ; louer les machines et vérifier son fonctionnement ensuite.**

### Architecture proposée, à valider par l’orchestrateur

- Orchestrateur Python commun **stdlib**, adaptant les scripts existants.
- CLI `list` / `plan` sans effets ; `run` explicitement demandé lors d’une phase future.
- Construction, exécution des tests et collecte des mesures séparées.
- Profils explicites :
  - CPU checks ;
  - OpenGL logiciel, correctness ;
  - OpenGL matériel, correctness et mesures CPU submission — ces dernières exigent aussi GL ;
  - timestamps GPU : extension future, non implémentée implicitement.
- Google Benchmark reste une option non adoptée, sans installation ni dépendance obligatoire.
- Runner natif ultérieurement réservé ; aucun Docker imposé.

**Ces orientations restent une proposition : aucune décision architecturale ouverte n’est considérée adoptée par ce plan.**

## Plan — sept lots

Commun à tous : méthode **PM/LLM**, contexte **`fresh`**, aucun modèle imposé. Les chemins nouveaux ci-dessous sont des réservations proposées, à confirmer avant développement. Aucun calendrier chiffré sans contexte d’exécution.

### L1 — Contrat commun et réception statique
**Type / profil :** conception documentaire, `engine-architect` ; `worktree: true`.

- **Livrable / ownership :** contrat sous `docs/validation-pipeline/contract.md` : CLI, profils, interfaces adaptateurs, schémas des artefacts et états.
- **Entrées :** briefing existant, scripts et garanties actuelles décrits ; aucune nouvelle exploration requise ici.
- **Invariants :** distinguer état du pipeline, verdict des tests et validité des mesures ; absence de preuve ≠ succès ; échec et échantillon invalide conservés.
- **Réception :** revue statique des exemples nominaux et négatifs ; compatibilité Windows/Linux et frontières build/collecte explicites. Aucun validateur exécuté.
- **Dépendances / ressources :** aucune ; propriétaire unique du contrat.
- **Contexte :** fresh, brief autonome.
- **Scores :** complexité brute **64**, retenue **64** ; risque **61**. Contrat transversal pouvant fausser toutes les preuves.
- **Incertitude :** validation architecturale préalable obligatoire.

### L2 — Orchestrateur Python et formats de preuves
**Type / profil :** développement, `cpp-engineer` ou auteur Python confirmé ; `worktree: true`.

- **Livrable / ownership :** `tools/validation-pipeline/core/`, point d’entrée CLI et leurs tests propres.
- **Contrat :** stdlib ; `list`/`plan` sans sous-processus, création de fichiers, téléchargement ou détection intrusive ; exécution uniquement via `run`.
- **Artefacts :** logs, JUnit conservés, métriques brutes JSON et manifests HEAD/delta, sources/shaders/binaires/environnement ; éléments indisponibles explicitement signalés.
- **Réception :** revue des chemins d’effets, propagation des erreurs, schémas et tests livrés couvrant résultats absents, collecte partielle et sample invalide.
- **Dépendance :** contrat L1 intégré et versionné ; aucun besoin d’attendre les adaptateurs.
- **Ressources :** fichiers core exclusifs ; aucun build partagé utilisé dans cette livraison.
- **Scores :** brute **76**, retenue **76** ; risque **67**. États, provenance et portabilité fortement couplés.
- **Incertitude :** comportement runtime non qualifié.

### L3 — Adaptateur Windows
**Type / profil :** développement, `build-engineer` ; `worktree: true`.

- **Livrable / ownership :** adaptateur Windows dédié, `scripts/build-windows.ps1` si adaptation nécessaire, tests propres de l’adaptateur.
- **Entrées :** contrat L1, preset Release existant.
- **Invariants :** réutiliser le script ; séparer build et collecte ; distinguer session Windows apte à GL, pilote et identité du renderer ; aucune disponibilité GPU présumée.
- **Réception :** inspection des commandes générées, quoting, codes de sortie, chemins et profils ; aucun appel réel.
- **Dépendance :** L1 intégré ; interface stable permettant développement parallèle avec L2.
- **Ressources :** ownership Windows exclusif ; futurs répertoires de build et contexte GL sous bail.
- **Scores :** brute **69**, retenue **70** ; risque **58**. Sessions graphiques et erreurs de configuration difficiles à prouver statiquement.
- **Incertitude :** qualification future sur runner natif Windows.

### L4 — Adaptateur Linux et profils GL explicites
**Type / profil :** développement, `build-engineer` ; `worktree: true`.

- **Livrable / ownership :** adaptateur Linux, `build-linux.py`, `linux-evidence.py`, `CMakePresets.json` et tests propres.
- **Contrat :** ajouter la capacité Release Linux sans confondre présence du preset et validation ; rendre l’exigence llvmpipe propre au profil logiciel.
- **Invariants :** conserver les garanties historiques du profil logiciel ; le profil matériel refuse une substitution silencieuse par llvmpipe ; CPU checks n’exige pas GL.
- **Réception :** revue des prédicats renderer/profil, commandes et manifest ; cas négatifs livrés pour profils incompatibles et preuves manquantes.
- **Dépendance :** L1 intégré ; parallèle avec L2/L3/L5.
- **Ressources :** propriétaire unique du preset partagé ; futurs builds, X server et device sous exclusion.
- **Scores :** brute **78**, retenue **78** ; risque **71**. Risque de perdre les garanties existantes en généralisant la détection.
- **Incertitude :** X server/GLX matériel versus Xvfb/llvmpipe à qualifier ultérieurement.

### L5 — Workload float et catalogue de scénarios
**Type / profil :** développement, `gpu-engineer` ; `worktree: true`.

- **Livrable / ownership :** workload isolé, catalogue et tests propres sous `tools/validation-pipeline/workloads/` ; aucun changement de production.
- **Entrées :** oracle existant `LegacyAndPreparedMatchFixedNonemptyPixels`, scénarios float et tests moteur.
- **Contrat :** A par nom, B préparé ; C via `Uniform&` résolu seulement si retenu comme diagnostic clairement séparé. Même scène, valeurs et travail observable.
- **Invariants :** séparer préparation, update CPU et collecte ; allocations uniquement si instrumentables ; aucun temps CPU présenté comme temps GPU.
- **Réception :** traçabilité des scénarios existants, oracles embarqués/référencés, contrôles négatifs et export brut prévus ; aucun gain attendu imposé.
- **Dépendance :** L1 intégré ; raccordement via interface contractuelle sans édition du core.
- **Ressources :** fichiers workload exclusifs ; GPU réservé à la qualification future.
- **Scores :** brute **77**, retenue **77** ; risque **68**. Comparabilité A/B et frontières de chronométrage sensibles.
- **Incertitude :** instrumentation, bruit et qualité des mesures non évalués.

### L6 — Intégration, packaging et entrée manuelle
**Type / profil :** développement/outillage, `build-engineer` ; `worktree: true`.

- **Livrable / ownership :** raccordement final, workflow dédié `workflow_dispatch`, guides Windows/Linux et commandes futures.
- **Contrat :** aucune extension automatique des workflows existants ; aucun runner cloud provisionné ; ressources et préconditions explicites avant un futur `run`.
- **Réception :** revue des dépendances, chemins et options ; exemples `list`, `plan`, build et collecte ; documentation sessions/pilote Windows, GLX matériel et Xvfb logiciel Linux. Aucun exemple exécuté.
- **Dépendances :** **L2–L5 réellement intégrés**, pas simplement terminés.
- **Ressources :** workflow et guides exclusifs ; adaptations des fichiers précédents seulement après transfert explicite d’ownership.
- **Scores :** brute **73**, retenue **73** ; risque **65**. Risque de déclencher involontairement une exécution ou de livrer des raccordements incohérents.
- **Incertitude :** runner et commande cloud définitifs différés.

### L7 — Audit indépendant de livraison
**Type / profil :** QA lecture seule, `test-reviewer` ; `worktree: false`.

- **Livrable :** matrice critères → fichiers/preuves statiques → limites ; aucune modification ni exécution.
- **Entrées :** candidat L6 intégré, diff complet, contrat L1 et tests livrés.
- **Réception :** vérifier absence d’effets des chemins consultatifs, séparation des états, garanties llvmpipe conservées, provenance, workflow uniquement manuel et absence de dépendance adoptée implicitement.
- **Dépendance :** barrière d’intégration L6.
- **Ressources :** aucune ressource d’exécution ; restitution au coordinateur par réponse finale.
- **Scores :** brute **61**, retenue **61** ; risque **59**. L’audit peut détecter des incohérences, pas démontrer le fonctionnement.
- **Contexte :** fresh impératif pour l’indépendance.
- **Incertitude :** runtime explicitement **NON_EXECUTÉ**.

## Réception, TDD et admission

**Tension explicite :** développer sans aucune exécution ne permet pas de satisfaire le RED → GREEN exécuté exigé par AGENTS.md. Livrer les tests et leurs attendus n’en constitue pas la preuve. L’orchestrateur doit faire valider ce mode de livraison limité ou séparer développement et qualification ; ne jamais fabriquer un RED/GREEN.

La clôture documentaire peut constater :
- sources, tests, scripts, workflows et guides livrés ;
- cohérence statique auditée ;
- **runtime NON_EXECUTÉ** ;
- qualification cloud future distincte.

Ni PASS benchmark, ni 589 tests passants ne sont des critères de cette livraison.

Les risques ≥ 50 déclenchent la préparation QA par le coordinateur avant les auteurs, sans convertir les scores PM en matrice QA. La politique native éventuelle doit être fournie avant admission ; les scores ci-dessus ne valent ni approbation ni démarrage.

## Graphe et barrières

`L1 → validation/intégration du contrat → [L2 ‖ L3 ‖ L4 ‖ L5] → intégration coordinateur → L6 → intégration → L7`

Les quatre branches sont indépendantes **par interfaces et ownership**, pas par transfert implicite de fichiers. Chaque worktree devra recevoir le HEAD contenant ses entrées ; le diff préexistant des profils et rapports reste exclu.

**Chemin critique :** contrat → branche productrice la plus longue → packaging → audit.

**Décisions restantes :** adoption de l’architecture Python, emplacement des fichiers, maintien ou exclusion du scénario C, dérogation/phasage TDD et cible de qualification future.

**Aucune écriture, exécution, publication, admission ou délégation effectuée.**