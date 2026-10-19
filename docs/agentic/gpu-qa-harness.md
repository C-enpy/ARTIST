---
title: Catalogue GPU et boucle QA du harness ARTIST
category: development
tags: [artist, tooling, testing, rendering, guide]
summary: Raccordement documentaire des expertises GPU et du runner QA séparé ; preuves statiques distinctes du routage et des permissions natifs.
created: 2026-10-16
updated: 2026-10-18
---

# Catalogue GPU et boucle QA

## Portée et autorité

Le coordinateur **existant** `.opencode/agents/orchestrator.md` porte les choix et
les barrières : pas de second coordinateur, routeur exécutable ni agent runner.
Le catalogue comporte **23 profils**, **17 skills** locaux et **19 responsabilités**,
pas 19 alias à créer. Les onze anciens profils et six skills ARTIST sont conservés.
OpenGL seul backend moteur implémenté ; les expertises Vulkan, D3D12 et Metal ne
sont pas des backends ajoutés. Les connaissances Slang n'imposent pas de migration.
Statuts [[adr/README|ADR]] inchangés : 0001/2/4/6/7 Proposed, 0003/5 Accepted partial,
0008 vérifié Windows Debug. Ownership unique et emprunts restent des contrats,
pas une preuve générale de RAII ou de performance.

## Choisir sans tout charger

Description légère → skill pertinent → référence ciblée. Un principal, renforts
motivés ; API/objectif/oracle absent → clarification. Lifetime/hazards précèdent
l'optimisation. CPU seul reste au cpp-engineer ; DDS reste un contenu, pas une
sélection automatique D3D12 ; RTX n'est pas un test de capacité.

- Capabilities et portabilité : gpu-portability-engineer.
- Graphics/compute pipelines : gpu-pipeline-engineer, procédures distinctes.
- Compilation/optimisation shader : shader-engineer, protocole de mesure explicite.
- Ressources et synchronisation : gpu-resource-engineer ; DAG/scheduling/aliasing : render-graph-engineer.
- Géométrie et textures : asset-engineer ; solveurs : gpu-simulation-engineer.
- Frame debugging/validation : rendering-verifier ; profiling : performance-reviewer RO.
- API : vulkan-backend-engineer, d3d12-backend-engineer, metal-backend-engineer,
  gpu-engineer pour OpenGL historique.

Les minimums des douze scénarios sont fixés dans
`.opencode/evidence/mission-gpu-qa/catalog-contract.json` et rappelés par le
coordinateur. Ce JSON est un oracle manuel de tests **statiques**, jamais un
dispatcher ni une trace de sélection. Correctness/shader/performance reviewers
gardent leurs permissions/méthodes ; les DEFER_TO reviennent au coordinateur.

## QA avant les auteurs, puis après intégration

Déclencheur : comportement sous contrat **OU** suite d'intégration étendue **OU**
risque PM >= 50 (échelle 0–100). Le PM garde ses critères et la grille approuvée.
La matrice QA est distincte : impact × probabilité 1–5 → 1–25 ; QA >= 10 exige
déclencheur, blast radius, détection, mitigation et gap. Aucune conversion entre
les deux évaluations, aucune probabilité de succès déduite du score PM.

1. qa-planner propose exigences/oracles/risques, nominal+négatif, intent/invariants,
   budget setup puis 70–80 % planifié + 20–30 % buffer, registre bidirectionnel.
2. Le lead persiste et **commite le plan avant les worktrees auteurs**.
3. Les auteurs gardent RED → GREEN minimal → REFACTOR ; docs/config utilisent
   leurs contrôles adaptés, pas un faux RED comportemental.
4. Le lead intègre réellement les producteurs avant test-engineer/generator de gaps.
5. test-reviewer fresh hors board d'implémentation ; corrections puis ré-audit.
   KEEP/MODIFY/REJECT/DEFER et deux réparations healer maximum ; bug production
   reste rouge, divergence de pin signalée, aucune assertion affaiblie.

### Runner séparé — autonomie dégradée assumée

Révision utilisateur du **2026-10-18** : les trois QA conservent le deny-all et
27 règles locales ; `execute`, `shell` et `subagent` passent à allow, tandis que
`edit`, les exclusions de lecture des secrets et `external_directory` restent
deny. Les consignes bornent l'usage à Context Mode/analyse et au transport Ensemble
autorisé, pas à une sous-délégation ni aux tests moteur. Ils proposent texte/patch ;
**le lead mandaté distinct persiste, relit et exécute les tests**, avec attribution.

L'ouverture d'execute n'accorde aucun droit général aux outils imbriqués. Aucun
allow MCP/`team_*` ajouté : leurs propres règles restent applicables. Si le transport
ou l'analyse est inaccessible, le lead relève la réponse finale ou fournit les
digestions. `edit deny` ne confine pas les effets d'un shell/exécuteur autorisé :
ni sandbox ni isolation filesystem ne sont démontrées.

Vérification locale, OpenCode **2.0.32** / Node **24.11.1** : les trois profils sont
découverts avec les nouvelles règles et consignes, **32 règles résolues chacun**
(dont un refus browser ajouté après les règles locales). Trois sessions QA ont
réussi un calcul mémoire par execute ; leurs recherches n'exposent ni Context Mode,
ni les outils de transport Ensemble demandés, ni le mutateur Serena recherché.
**Absence du catalogue, pas refus natif observé** : aucun appel MCP imbriqué réussi
ou refusé, aucun shell/sous-agent/mutateur exécuté par les QA. Enforcement complet
et transport réel restent **INCONCLUSIVE / NOT_RUN**.

Le validateur historique et l'oracle T05 décrivent encore le template fermé v1.
Contrôle T05 du 2026-10-18 : **55/58 PASS, 3 FAIL, zéro skip** sur les contrats des
trois profils révisés ; l'oracle historique n'a pas été modifié pour les verdir.
Ce résultat ne remplace pas la clôture historique ci-dessous et interdit un PASS
global actuel. Plan préalable commité `230c551`, contrôles/empreintes/probes et revue
attribués : `.opencode/evidence/qa-tools-policy-2026-10-18/validation.md`.

## Preuves et reprise — historique du 2026-10-17

La baseline intégrée **1821af6** a été fournie à T13, avec base opérateur importée
séparément **82edafe** ; aucune copie du canonique dirty. Le rapport candidat
`.opencode/evidence/mission-gpu-qa/t13-result.md` conserve commandes, comptes,
empreintes et limites ; `validation-report.md` garde les gaps par critère.
Les résultats du harness ne sont pas une nouvelle baseline moteur 143/165.

Clôture locale du 2026-10-17 : **588/588 tests Node** recoupés par le lead,
zéro fail/skip/cancel ; union syntax/schema/offline **41/81/215 contrôles**.
T14 propose zéro test supplémentaire : KEEP local/DEFER natif, pas acceptation
humaine des gaps. Deux revues fresh parallèles, QA et technique GPU, ne trouvent
aucun défaut démontré dans leur périmètre. Les reviewers n'ont exécuté aucun test.
Rapports attribués : `.opencode/evidence/mission-gpu-qa/final-reviews.md` et
`final-validation.md` ; livraison **partielle**, aucun PASS global.

Les vrais profils QA sont absents du root d'admission de cette mission et présents
dans le worktree : generator et revue finale ont appliqué leurs méthodes via
substituts disponibles, explicitement signalés. Cela n'est pas une preuve native
des profils QA. Au candidat de revue **245f1f3**, découverte GET : 30 profils
résolus (23 locaux + 7 natifs), 17 skills locaux présents parmi 103 résolus ;
27 règles QA déclarées, 32 résolues avec héritage. Ni découverte ni comptage
ne démontrent sélection, chargement ou enforcement. Voir `qa-admission-limit.md`.

- **Intention** : catalogue, oracles et boucle ci-dessus.
- **Code/config observés** : profils/skills et template QA ; correction textuelle
  Conan 2/CMakeDeps/CMakeToolchain du build-engineer, sans changement de toolchain.
- **Exécuté** : seulement les contrôles Node/offline listés dans le rapport T13.
- **Non exercé / INCONCLUSIVE** : routage/chargement natifs S01–S12, permissions,
  canaris, autonomie, GPU/image/performance, build C++/CTest et CI distante.
- **Mesuré** : aucun gain CPU/GPU ; temps du runner non interprété comme performance.

`native-probe-plan.md` propose douze scénarios, trois exemples et les canaris QA
au lead, **NON EXÉCUTÉS**. Découverte historique 23 locaux/17 skills/30 résolus au
HEAD 82edafe ne vaut pas sélection ni preuve sur le candidat T13. Aucun faux
native-observations.json ; JSON non authentifié reste INCONCLUSIVE.

Commandes de contrôle local depuis la racine du worktree (Node/Git de confiance) :

```powershell
node --test --test-concurrency=1 --test-reporter=tap .opencode/tests/agentic/t13-integration/contract.test.mjs
node .opencode/scripts/validate-agentic.mjs --mode offline --root . --manifest .opencode/tests/agentic/t13-integration/manifest.json
node .opencode/tests/agentic/t04-harness/run-tests.mjs all
```

Le manifest union énumère les entrées validées en lecture, **pas** l'allowlist
d'écriture T13. Aucun T04–T12, moteur, CMake/Conan, permission, politique ou global
n'a été élargi. Les sorties privées sont sous `out/t13-integration/`.

## Provenance et connaissance

Les sources externes sont référencées, pas exécutées/importées : inventaires
`.opencode/evidence/mission-gpu-qa/t02-external-sources.md` et
`t02-source-inventory.md`, contrat `t03-contract.md`, plan `qa-plan.md`, ruling
`qa-policy-boundary.md`, revue `review-r2-contract.md`. Leurs statuts/licences
observés ne sont pas des licences inventées pour ce raccordement. Les crédits
existants en fin de Readme.md (53 dépôts audités) sont préservés, non recopiés.
Les docs officielles V2 agents/permissions consultées le 2026-10-16 expliquent la
déclaration, pas une preuve d'enforcement de ces profils.

QMD `artist-wiki` pointe le vault **canonique**, pas ce worktree. Aucun rebind,
watcher ou indexeur lancé par T13 ; index potentiellement périmé pour ce delta.
Après intégration canonique autorisée, le lead vérifie la garde de chemin du script
de maintenance avant update/embeddings. Serena reste également attaché à ARTIST
canonique ; aucune base C++ inventée pour ces contrôles documentaires.

Navigation : [[opencode-harness|Harness OpenCode]] · [[index|Accueil]] ·
[[engineering-principles|Principes]] · [[development/build-and-test|Build et preuves]] ·
[[_meta/wiki-maintenance|Maintenance QMD]].
