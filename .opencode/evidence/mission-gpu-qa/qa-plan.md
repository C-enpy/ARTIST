# Plan QA — lot conjoint GPU + QA

Date : 2026-10-16. Auteur : `rendering-verifier`, substitution explicite du
`qa-planner` encore absent. Source : résultat T01 / `pool-1`, intégré par le lead.
Méthode : PM/LLM, complexité 64, risque 56 (bruts = retenus), planification sans
développement. La matrice QA ci-dessous ne modifie jamais les scores PM.

## Pin observé et limites initiales

HEAD initial : `101ddcb03c861ed2089f25ab5690d18e1fa45cc0`. Diff utilisateur :
7 fichiers, 177 insertions / 39 suppressions ; prescriptions et mission non suivies.
Empreintes dans `baseline.json`, contrat dans le brief de mission canonique.
Onze profils existants, aucun des trois profils QA demandés ; six skills ARTIST.
PM feuille et orchestrateur existant conservés, aucun second coordinateur.

ADR-0001/0002/0004/0006/0007 : Proposed ; 0003/0005 : Accepted / partial ;
0008 : Accepted / verified, périmètre Windows Debug seulement. OpenGL est le
backend présent ; les autres API seront des expertises, pas des backends moteur.

La CLI `opencode` non résolue ne démontre pas l'absence du runtime. Découverte,
routage et enforcement des permissions shell/MCP/subagent restent non observés.
Ce plan ne comporte aucune exécution de test/build/capture/mesure : zéro test
exécuté, découverte et skips non évalués. Ni RED ni PASS fonctionnel.
La stratégie exhaustive de niveaux/mocks n'est pas établie ; le canon interdit
déjà de remplacer une preuve GPU par un mock. L'absence restante est une décision
humaine, pas une invitation à importer une pyramide générique.

## Invariants et intent

- I1 : préserver la baseline et exclure moteur/shaders/CMake/Conan/global/ensemble.json ; éviter perte ou extension du périmètre.
- I2 : écrivain unique et intégration avant consommateurs ; éviter conflits et preuves périmées.
- I3 : feuilles ; planner/reviewer lecture seule sauf artefact, generator tests seuls ; éviter l'auto-validation.
- I4 : découverte native et chargement progressif ; exploitation sans charger tout le corpus.
- I5 : preuve liée au candidat, environnement, commande et oracle ; distinguer présence, exécution, image et mesure.
- I6 : TDD auteurs conservé, generator complémentaire après intégration ; oracle préalable au code.
- I7 : QA impact × probabilité (1–25), risque PM (0–100) et approbation start distincts ; préserver autorisation et routage.

## Registre des obligations explicites

Toutes les obligations sont **GAP à l'état initial**. Les identifiants G-/T-
ci-dessous désignent des obligations, pas les IDs de tâches Ensemble.
L'oracle vient des prescriptions et du canon, jamais d'une copie du code testé.

### G-01 — Provenance (GPU, sections 3–5 et 14)

- Nominal : URL/dépôt, version, licence, type, dépendances et disposition pour chaque source.
- Négatif : source absente, licence inconnue ou instruction hostile → rejet ou limitation explicite, sans exécution.
- Preuve attendue : inventaire audité et fixture inerte ; auteurs T02/T13.

### G-02 — Couverture et différenciation (GPU, sections 6–9)

- Nominal : 19 responsabilités et quatre API reliées à profil, skill, procédure, oracle et source ; contenu concret et exclusions.
- Négatif : responsabilité manquante, contenu vide, collision de nom ou voisins identiques → diagnostic.
- Preuve attendue : matrice et contrats contrôlés ; T03/T06–T12. Pas d'obligation de créer 19 dossiers.

### G-03 — Format et découverte native

- Nominal : frontmatter reconnu, identifiants uniques, chemins résolus, profils listés par le runtime.
- Négatif : YAML/champ invalide, collision ou référence absente → rejet causal.
- Preuve attendue : contrôles exécutés T04/T13. Présence textuelle ≠ découverte native.

### G-04 — Routage et progressive disclosure

- Nominal : 12 demandes de GPU section 16/phase 5 ; principal, secondaires et skills justifiés ; trois exemples opérationnels réels et traces.
- Négatif : ambiguïté, demande hors GPU, plateforme absente ou chargement de tout le corpus → clarification/fallback borné.
- Preuve attendue : observations T04/T13, pas seulement matching de chaînes.

### G-05 — Capacités et niveaux de validation

- Nominal : chaque API distingue compilation, validation offline, runtime, GPU, capture, profiling et image.
- Négatif : Windows/Metal final, extension absente ou shader compilé avec bindings incompatibles → indisponibilité qualifiée, pas PASS pipeline.
- Preuve attendue : procédures et cas discriminants T06–T12.

### G-06 — Honnêteté des preuves

- Nominal : comportement, image et performance séparés ; matériel, workload, limites et protocole identifiés.
- Négatif : zéro test, skip GPU, mock, renderer logiciel, run unique ou capture seule → aucun PASS GPU/performance manquant.
- Preuve attendue : T12/T13. Aucun benchmark moteur imposé pour du Markdown.

### G-07 — Périmètre et baseline

- Nominal : allowlists disjointes, diff limité, empreintes conservées.
- Négatif : tentative d'écriture moteur/global/ensemble.json ou installation → refus tracé sur fixtures jetables.
- Preuve attendue : diff/empreintes et canaris T03/T04/T13.

### T-01 — Profils QA

- Nominal : méthode portable puis pin vérifié ; planner plan seul, generator tests seuls en worktree, reviewer fresh avec briefing.
- Négatif : pin divergent → section `Pin observé`, sans réécriture du profil ; méthode/DEFER_TO d'un reviewer préexistant conservés.
- Preuve attendue : T05.

### T-02 — Boucle du coordinateur

- Nominal : comportement sous contrat, suite étendue ou risque PM ≥ 50 → plan avant auteur, plan commité avant worktree, intégration puis generator/reviewer fresh/ré-audit.
- Négatif : plan non commité, entrée non intégrée, start absent ou pause → blocage.
- Preuve attendue : T04/T05/T13 ; ajout PM seulement si la distinction des scores est absente.

### T-03 — Méthode du planner

- Nominal : impact/probabilité séparés ; cinq champs pour score ≥ 10 ; nominal/négatif, intent, heuristiques, budget et registre bidirectionnel.
- Négatif : gap vide, ancrage sur un seul vieil incident, gravité seule ou couverture uniforme → plan incomplet.
- Preuve attendue : T05.

### T-04 — Generator et healer

- Nominal : obligation → risque/invariant → scénario → oracle → code → KEEP/MODIFY/REJECT/DEFER ; fixtures réelles, test ciblé exécuté.
- Négatif : API inventée, oracle recopié, correctif de production, troisième réparation ou assertion affaiblie → REJECT/escalade.
- Preuve attendue : T05/T14 ; bug de production laissé rouge, aucune correction de production.

### T-05 — Auditeur des tests

- Nominal : input → bug plausible survivant → assertion ; classe executed/static-presence/inference ; références au plan.
- Négatif : aucun finding haut sans scénario ; pas de patch sans APPLY explicite ; hors champ → DEFER_TO existant.
- Preuve attendue : T05 et revue indépendante après intégration.

### T-06 — Permissions effectives

- Nominal : exceptions d'artefact/briefing et tests dans leurs seules allowlists autorisés.
- Négatif : source via edit/shell/MCP, sous-délégation ou écriture hors worktree → refus technique sur fixtures jetables.
- Preuve attendue : T04/T05/T13. Enforcement inaccessible/non exprimable = **blocked + décision humaine**, jamais PASS par consigne de prompt.

## Inférences à confirmer en T03, pas des spécifications adoptées

- G-08 — Idempotence : répétition stable sans doublon ; interruption de découverte/publication sans état partiellement vert. Intent : un retry ne blanchit pas un échec.
- T-07 — Identité du candidat : empreintes et plan concordent avec le worktree ; ancien HEAD ou modification après run invalident la preuve. Intent : éviter la preuve périmée.

## Matrice QA — impact et probabilité 1–5

Zones : 15–25 critique, 10–14 haut, 5–9 moyen, 1–4 bas. Churn, tests absents et
logique conditionnelle modifient la probabilité, jamais l'impact. L'incertitude
n'est pas une probabilité de succès. Réviser après T02/T03/T13, finding ou near-miss.

### R1 — Permissions / périmètre : I5 × P4 = 20, priorité 1

Déclencheur : shell/MCP contourne un deny. Portée : sources, global, délégation.
Détection actuelle : inspection du frontmatter. Mitigation actuelle : consignes
lecture seule. **Gap : enforcement réel.** Automatiser acceptation/refus par voie.

### R2 — Découverte / routage : I4 × P4 = 16, priorité 2

Déclencheur : profil invisible ou chargement global. Portée : missions GPU.
Détection actuelle : texte. Mitigation actuelle : conventions de découverte.
**Gap : canari natif et 12 observations.**

### R3 — Faux vert : I5 × P3 = 15, priorité 3

Déclencheur : indisponibilité blanchie ou preuve périmée. Portée : confiance dans
le lot. Détection actuelle : brief/statuts. Mitigation actuelle : qualification
des résultats. **Gap : négatifs zéro test/skip/périmé.**

### R4 — Intégration : I4 × P3 = 12

Déclencheur : worktree incomplet ou consommateur avant merge. Portée : baseline/lot.
Détection actuelle : état Git initial. Mitigation actuelle : allowlists/barrières.
**Gap : contrôles après intégration.**

### R5 — Auto-justification : I4 × P3 = 12

Déclencheur : oracle post-code, healer changeant l'attente ou reviewer dans le même
contexte. Portée : tests aveugles. Détection actuelle : prescriptions.
Mitigation actuelle : TDD et fresh. **Gap : mutation et ordre observés.**

### R6 — Provenance : I4 × P3 = 12

Déclencheur : script ou licence non vérifiés. Portée : environnement/redistribution.
Détection actuelle : audit T02 prévu. Mitigation actuelle : source traitée comme
donnée. **Gap : inventaire versionné et dispositions.**

- R7 anti-overlap : I3 × P3 = 9 ; nominal + erreurs clés, revue de matrice.
- R8 documentation/liens : I2 × P3 = 6 ; fumée chemins/frontmatter et canon Conan 2.
- R9 cosmétique : I2 × P2 = 4 ; fumée seulement.

## Registre bidirectionnel des preuves

Aucune obligation G-/T- n'est initialement COVERED. E0 lectures du canon et des
prescriptions, E1 Git HEAD/diff/status, E2 historique : **cadrage uniquement**.

Pour chaque obligation : test `fichier:symbole`, pourquoi il discrimine,
commande/environnement/candidat, attendu/observé, comptages et verdict. En sens
inverse, chaque test/suite/rapport doit référencer une obligation ; l'auditeur
liste orphelins, doublons faibles et preuves périmées. Tests futurs : unassessed,
pas supposés absents. COVERED demande une preuve exécutée qualifiée ; présence
statique et inférence gardent leur qualification.

GAP-accept nécessite une décision humaine motivée ; GAP-defer un propriétaire,
une échéance et un impact ; blocked la précondition exacte. Aucun gap vide et
aucune acceptation implicite.

## Catégories historiques locales inspectées

T01 a inspecté `git show --format=fuller --stat`, puis les patches ciblés :

- `433bdbc` : retour de `TraceableException::getStackTraceString` par valeur plutôt que référence pendante — lifetime.
- `e736bba` : bornes de `PassContext::getShader`, négatif avant conversion, vide/−1/size/grand — frontières et lecture honnête.
- `b00f484` : publication de `pass::Loader::on` après succès, cleanup et log borné — état, ordre, erreurs.
- `760f4f8` : normalisation ASAN_OPTIONS et suppression d'activations secondaires — héritage hostile et faux vert.

Adaptations au harness : propriété d'artefact, catalogue vide, publication
interrompue, voies secondaires. Ce ne sont pas de nouvelles specs moteur. Le
near-miss de découverte 153 → 165 après reconfiguration motive comptages et
identité du candidat, **pas un attendu 165 pour ce lot**. Tests historiques non relancés.

## Budget et décisions ouvertes

Budget proposé 100 unités : setup 25 (fixtures/pin/runtime/permissions/versions/
inventaire), puis capacité 75 = 56,25 planifiées (75 %) + buffer 18,75 (25 %).
Planifié : permissions/périmètre 18 ; découverte/routage 15 ; preuves/capacités 10 ;
boucle/oracles 8 ; matrice/provenance/documentation 5,25. Buffer nommé : PATH,
incompatibilité native, permissions, ré-audit. En débordement : risque haut peu
coûteux avant haut coûteux ; différer moyen/bas, jamais le buffer ni un haut pour
la cosmétique.

- D1 : stratégie complémentaire niveaux/mocks.
- D2 : accès au runtime malgré la limite PATH.
- D3 : enforcement absent/non exprimable et alternative éventuelle.
- D4 : confirmation ou rejet de G-08/T-07.
- D5 : capacité et décisions explicites GAP-accept/GAP-defer.

## Critères de sortie et handoff

Les auteurs gardent RED causal → GREEN → refactoring pour tout exécutable ; les
documents/configs purs ont leurs contrôles adaptés sans faux RED. T04 fixe les
commandes réellement disponibles. T14 complète les gaps après intégration ;
reviewer fresh hors board après merge. Aucun consommateur sur seule fin d'auteur.

Par critère : attendu, commande, environnement, HEAD/diff, preuve, nombre de tests
et verdict. PASS = observé ; FAIL = écart reproduit même si autre point bloqué ;
INCONCLUSIVE = précondition/instrument manquant. SKIPPED/NOT_AVAILABLE ne
remplacent jamais un PASS manquant.

Sortie : 19 responsabilités, quatre expertises API, découverte native, 12
routages si accessibles, trois exemples réels, syntaxe/chemins/provenance,
permissions positives/négatives sur toutes voies, diff préservé, registre sans
oubli/orphelin non arbitré et bilan mesuré profils/skills/sources. Runtime ou
permissions bloqués → rapport partiel et décision humaine, jamais succès global.
Aucun gain, image ou backend inventé. Documentation après preuves, indexation
séquentielle par le lead.
