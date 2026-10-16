# Contrat intégré T03 — profils, skills et preuves GPU + QA

Date : 2026-10-16. Proposition engine-architect/pool-3, tâche
`task_mvbatabf_001t_6bzdur`, PM/LLM C82/R69 bruts = retenus ; restitution complète
en onze messages, puis intégration éditoriale par le coordinateur.
Ce contrat n'est ni une ADR, ni une nouvelle politique modèle, ni une preuve GPU.

## 1. Décisions du coordinateur et état des préconditions

- **APPROUVÉ pour T04 et T06–T12** : frontières, IDs, allowlists, interfaces et
  oracles ci-dessous. Ces huit branches sont indépendantes et publiables après
  commit du contrat. T05 a attendu le ruling explicite D-PERM ci-dessous.
- G-08 (idempotence du validateur) et T-07 (identité du candidat) sont retenus
  comme obligations du harness, pas comme nouvelles fonctionnalités moteur.
- D-NATIVE accès catalogue : résolu par le préflight lead, commit `1759a5f`.
  CLI actif hors PATH et requêtes deepObject authentifiées, location ARTIST,
  18 agents/92 skills dont onze profils/six skills locaux préexistants.
  Cela ne prouve ni nouveaux profils, ni douze routages, ni enforcement.
- D-PIN : recoupé dans le code ; PipelineContext stocke déjà
  `vector<unique_ptr<IPass<API>>>` et getPass emprunte
  (`ARTIST/include/graphic/context/PipelineContext.hpp:42,67,96`). Ne pas recopier
  l'ancienne photographie shared_ptr dans les nouveaux pins.
- **D-PERM / T-06 : MODE DÉCIDÉ PAR L'UTILISATEUR**, 2026-10-16, réponse
  « Runner séparé (Recommended) ». T05 devient publiable après intégration de ce
  ruling : profils fail-closed, sans shell/Code Mode libre, droits d'écriture
  exacts seulement après vérification native ; à défaut retour texte/patch et
  persistance lead. Tests relus exécutés par runner lead DISTINCT, attribution
  explicite, autonomie dégradée déclarée. Aucun sandbox OS/global/plugin/SDK
  autorisé ici. Cela n'accepte pas un faux PASS d'enforcement ou d'autonomie.
- L'OpenAPI du runtime expose effectivement permissions: Permission.Ruleset sur
  session.create/session.update et Session.Info (règles action/resource/effect).
  **Schéma observé seulement** : composition avec règles d'agent, applicabilité
  Ensemble et enforcement par lot non vérifiés. Aucun grant appliqué par cette
  observation ; à défaut profils sans edit et restitution au lead.
- Le refus `permission.rejected: edit` de T03 concerne CE profil seulement.
  L'architecte n'a pas contourné le refus ; le lead persiste son texte.

Entrées : QA plan `4287cb7`, audits `3c17f0c`, préflight `1759a5f`, brief canonique
`.opencode/missions/team_mvb9l9ux_001g_mzvoa2/execution-brief.md`.
Neuf hashes initiaux et diff utilisateur sept fichiers 177+/39− préservés.
Prescriptions GPU.md/QA.md non suivies : lecture canonique explicite, pas héritage
automatique dans un worktree. Aucun changement des statuts ADR.

## 2. Infrastructure et cible sans duplication

Conserver `.opencode/agents/<id>.md` et `.opencode/skills/<id>/SKILL.md`.
Descriptions natives, affinités et instructions de l'orchestrateur portent le
routage ; pas de routeur exécutable, registre concurrent, plugin ou superviseur.
Un manifest de test est un oracle, jamais un dispatcher.

Progressive disclosure : description légère → skill pertinent → référence ciblée.
Agents : frontmatter V2 description/mode/permissions ; nouveaux profils subagent.
Skills : name/description, name égal ID par convention locale, pas exigence native
inventée. Corps : triggers/exclusions/pin/preuves. Aucun modèle/variant fixé.
La grille approuvée reste souveraine. Ne pas importer les champs/chemins Cursor
d'un guide générique à la place du format OpenCode V2 observé.

Conserver les onze profils. Créer douze IDs :

- qa-planner, test-engineer, test-reviewer (T05, activation bloquée par D-PERM) ;
- vulkan-backend-engineer, d3d12-backend-engineer, metal-backend-engineer ;
- gpu-pipeline-engineer, shader-engineer, gpu-simulation-engineer ;
- gpu-resource-engineer, render-graph-engineer, gpu-portability-engineer.

Cible résultante : 23 profils, pas un quota. gpu-engineer reste l'expert OpenGL
et l'ID historique : pas de doublon opengl-backend-engineer. rendering-verifier
porte aussi le diagnostic frame/capture ; performance-reviewer porte diagnostic
et protocole, jamais le correctif ; asset-engineer couvre géométrie/textures.
Les reviewers existants restent sans droit d'écrire les correctifs/tests.

Conserver les six skills ARTIST, dont artist-static-boundary et
artist-cpp-contracts inchangés. Créer onze skills : vulkan-backend,
d3d12-backend, metal-backend, opengl-backend, gpu-pipelines,
gpu-shader-compilation, gpu-compute-simulation, gpu-resource-lifecycle,
gpu-render-graph, gpu-capability-detection, gpu-backend-portability.
Cible : 17 skills locaux. Les 19 noms du catalogue sont des responsabilités,
pas des alias natifs ni 19 dossiers obligatoires.

## 3. Couverture fonctionnelle et frontières

1. Capabilities → gpu-portability-engineer / gpu-capability-detection : observé,
   supporté, activé et compatible distincts ; feature absente → fallback/refus.
2. Graphics pipelines → gpu-pipeline-engineer / gpu-pipelines : stages, vertex
   input, attachments, depth/blend/raster ; mismatch rejeté, pas allocation générale.
3. Compute pipelines → même principal/skill, procédure compute distincte :
   dispatch/workgroup/layout/cache ; limites et bindings testés, pas solveur.
4. Compilation → shader-engineer / gpu-shader-compilation : expertise Slang,
   HLSL/GLSL/MSL, SPIR-V/DXIL, réflexion/ABI ; source/options/cible/entry identifiés ;
   aucun Slang imposé au moteur et shader compilé ≠ pipeline valide.
5. Optimisation shader → shader-engineer / artist-performance puis compilation :
   baseline A/B, qualité/workload/options égaux, dispersion ; revue perf fresh.
6. Ressources → gpu-resource-engineer / gpu-resource-lifecycle : owner/borrow,
   générations, alignement/résidence/retrait ; périmé/en-vol rejetés.
7. Synchronisation → même principal/skill : ledger RAW/WAR/WAW, ordre/visibilité,
   stages/access/queues ; aucune barrière universelle. Le graphe calcule, l'API traduit.
8. Render graph → render-graph-engineer / gpu-render-graph : dépendances, scheduling,
   aliasing/transients ; cycle/read-before-produce rejetés ; plan ≠ graphe frame.
9. Frame debugging → rendering-verifier / artist-gpu-debug : pass réduite,
   entrées/sorties/capture falsifiables ; pas auteur correctif ni gain par capture.
10. Profiling → performance-reviewer / artist-performance : CPU submit/GPU/waits,
    workload et mesures répétées ; auteur du goulot identifié ensuite via lead.
11. Géométrie → asset-engineer / artist-asset-cooking : source/IR/cook/runtime,
    indices/bornes/qualité ; meshlets ≠ support mesh shaders garanti.
12. Textures → même principal/skill : DDS/BC/ASTC/mips/colorimétrie,
    dimensions/blocs/pitch/offsets ; tronqué/non supporté rejetés ; upload au propriétaire ressources.
13. Simulation → gpu-simulation-engineer / gpu-compute-simulation : état runtime,
    seed/pas de temps/invariants/NaN/stabilité ; rendu et simulation séparés.
14. Portabilité → gpu-portability-engineer / gpu-backend-portability : formats,
    clip/depth/coordonnées/features/fallback ; écarts non représentables explicites.
15. Validation → rendering-verifier / artist-build-validation enrichi : CPU,
    offline/GPU/image/perf, candidat/comptages ; zéro test n'est pas PASS.
16. Vulkan → vulkan-backend-engineer / vulkan-backend : queues/families,
    descriptors/layouts, barriers/semaphores/fences, features/layers ; contre-exemples API.
17. D3D12 → d3d12-backend-engineer / d3d12-backend : root signatures/heaps/PSO,
    lists/queues/states/fences/résidence, debug layer conditionnelle ; DXC ≠ device.
18. Metal → metal-backend-engineer / metal-backend : metal-cpp lifetime,
    heaps/encoders/pipelines/families/drawable/capture ; Metal 3/4 conditionnels,
    MSL généré Windows ≠ compilation ou exécution Apple.
19. OpenGL → gpu-engineer / opengl-backend : contexte/thread/profil/extensions,
    VAO/VBO/FBO/états/compile-link/completeness/sync/destruction ; 3.3 core actuel,
    compute/SSBO/persistent mapping gated par capacités/version.

Chaque skill expose triggers ET exclusions, préconditions/platformes/outils,
procédure concrète, nominal/négatif, pièges, oracle, réussite, références/limites,
handoff. Un script inutile n'est pas une preuve de valeur ajoutée.
Compile/link/cook/runtime distincts ; axes de spécialisation bornés, pas de type
par asset. Cook immuable/versionné/cache invalidé sur source/options/ABI ; état
frame et objets driver ont une vie séparée. Génération ne signifie pas attente
GPU : destruction/réutilisation après complétion des dernières utilisations,
toutes queues concernées. Décrire ce qu'amortit un dispatch par lot.

Feuilles : DEFER_TO au lead, aucun spawn/handoff pour accroître les droits.
Brief et retour : backend/HEAD/delta/hashes, chemins/ownership/binding times,
capacités/oracle/essais/inconnues/bail, faits et preuves qualifiées.
T02 n'autorise aucune copie : adaptation exige commit résolu, chemin et notices
exacts ; arbre SHA ≠ commit. Licence inconnue = lien seulement. Pas d'import des
superviseurs CppStudio/Apple. rdc-cli candidat documentaire non activé, fallback
manuel/offline ; CLI-Anything alternative référencée. Aucun install/daemon au chargement.

## 4. Allowlists auteurs disjointes

Toutes relatives au dépôt ; répertoire skill = SKILL.md et références, aucun
exécutable nouveau sauf contrat explicite. Chaque lot possède
`.opencode/tests/agentic/tNN-<nom>/contract.test.mjs`, `manifest.json`, fixtures
inertes et son seul `.opencode/evidence/mission-gpu-qa/tNN-result.md`.

- T04 : `.opencode/scripts/validate-agentic.mjs`, tests `t04-harness/`.
- T05 : agents qa-planner/test-engineer/test-reviewer, tests `t05-qa/` ; mode
  fail-closed/runner séparé ci-dessus, aucune autonomie non démontrée.
- T06 : agent vulkan-backend-engineer, skill vulkan-backend, tests `t06-vulkan/`.
- T07 : agent d3d12-backend-engineer, skill d3d12-backend, tests `t07-d3d12/`.
- T08 : agent metal-backend-engineer, skill metal-backend, tests `t08-metal/`.
- T09 : agents gpu-engineer/rendering-verifier, skills opengl-backend et
  artist-gpu-debug, tests `t09-opengl-debug/`.
- T10 : agents gpu-pipeline-engineer/shader-engineer/gpu-simulation-engineer,
  skills gpu-pipelines/gpu-shader-compilation/gpu-compute-simulation,
  tests `t10-pipelines-shaders-simulation/`.
- T11 : agents gpu-resource-engineer/render-graph-engineer/gpu-portability-engineer,
  skills gpu-resource-lifecycle/gpu-render-graph/gpu-capability-detection/
  gpu-backend-portability, tests `t11-resources-graph-portability/`.
- T12 : agents asset-engineer/performance-reviewer, skills artist-asset-cooking/
  artist-performance/artist-build-validation, tests `t12-assets-performance-validation/`.

Aucun auteur ne modifie un voisin, catalogue commun, AGENTS, commandes, T01–T03,
ensemble.json, politique, presets/CMake/Conan moteur, code/shaders de production
ou prescriptions GPU.md/QA.md. Pas de package/dépendance globale ni capture parlot.
Références vers futurs IDs figés autorisées sans stub ; existence prouvée après
intégration. Helper partagé nécessaire → demande au lead/T04.

T13, seulement après intégration : fichiers partagés autorisés au plan, seul
paragraphe PM de séparation des risques si absent, correction Conan 2 du profil
build-engineer, raccordement reviewers et permissions après décision. Toute
extension de la liste initiale exige contrôle de nécessité et d'ownership par le
lead. Synthèse durable unique `docs/agentic/gpu-qa-harness.md` ; preuves
catalog-contract.json/native-observations.json/validation-report.md ; tests
t13-integration. T14 : seuls tests `t14-generator/` et rapport convenu.

## 5. Douze oracles de sélection, pas des observations exécutées

Principal unique ; minimum au premier raisonnement utile, autres skills/spécialistes
seulement avec besoin explicité. La sélection et les appels doivent être observés
nativement ; aucun programme de lookup de ce texte ne prouve un routeur.

1. Graphics Vulkan + depth : gpu-pipeline-engineer ; gpu-pipelines + vulkan-backend.
2. HLSL coûteux : shader-engineer ; artist-performance + gpu-shader-compilation ; baseline/cible demandées si absentes.
3. Resource barrier D3D12 : gpu-resource-engineer ; gpu-resource-lifecycle + d3d12-backend ; mapping API si nécessaire.
4. Shader Vulkan vers Metal : shader-engineer ; gpu-shader-compilation + gpu-backend-portability ; Metal natif seulement si ABI/capacités touchées.
5. Particules compute : gpu-simulation-engineer ; gpu-compute-simulation + gpu-pipelines ; API/contraintes à clarifier.
6. Frame RenderDoc : rendering-verifier ; artist-gpu-debug ; capture/outils utilisables vérifiés, aucun install/correctif.
7. DDS compressé : asset-engineer ; artist-asset-cooking ; DDS ne sélectionne pas D3D12 à lui seul.
8. Render graph shadow/post : render-graph-engineer ; gpu-render-graph + artist-static-boundary ; ressources sur hazards concrets seulement.
9. OpenGL noir : gpu-engineer ; artist-gpu-debug + opengl-backend ; reproduction avant profiling/migration.
10. Goulot GPU : performance-reviewer ; artist-performance ; diagnostic RO puis auteur du goulot identifié.
11. Mesh shaders capacités : gpu-portability-engineer ; gpu-capability-detection ; API/device requis, nom RTX insuffisant.
12. Artefact d'un seul backend : gpu-portability-engineer ; gpu-backend-portability + artist-gpu-debug ; demander backends/images/conventions.

Négatifs : CPU seul → cpp-engineer ; API absente → clarification ; Metal Windows
documentaire possible mais exécution Apple INCONCLUSIVE ; texte d'un skill sans
appel natif = présence statique ; tous les skills chargés systématiquement = défaut.
Seul le lead lance les éventuels probes modèle approuvés : pas de sous-agent/API
session de contournement par un auteur, ni copie de l'orchestrateur.

## 6. Interfaces et preuves T04

Node découvert par T03 : v24.11.1 (pas le runtime du sandbox). Interfaces figées :

```text
node .opencode/scripts/validate-agentic.mjs --mode syntax --root <root> --manifest <manifest.json>
node .opencode/scripts/validate-agentic.mjs --mode schema --root <root> --manifest <manifest.json>
node .opencode/scripts/validate-agentic.mjs --mode offline --root <root> --manifest <manifest.json>
node .opencode/scripts/validate-agentic.mjs --mode native-evidence --root <root> --manifest <manifest.json> --evidence <native-observations.json>
node --test .opencode/tests/agentic/t04-harness/contract.test.mjs
```

- syntax : parser sans importer/exécuter une fixture. Sous-ensemble YAML strict
  permis si documenté et syntaxe non supportée rejetée, pas prétention YAML complet.
- schema : champs/types natifs utilisés et contrat local versionné ; convention
  ARTIST distinguée d'exigence V2. Pas de download schema à chaque test ni V1.
- offline : syntax/schema, collisions, chemins/liens, six IDs préservés,
  coverage/allowlists et analyse permissions. Aucun natif/GPU/commande citée exécuté.
- native-evidence : observations EXTERNES du runtime autorisé, candidat,
  complétude/contradictions/traces originales ; JSON seul n'authentifie rien.
  Sans capture vérifiable : INCONCLUSIVE. Pas de seconde orchestration.

Sortie JSON versionnée stdout, aucune écriture arbitraire. Exit 0 : tous contrôles
demandés PASS, nombre non nul ; 1 : au moins un FAIL ; 2 : aucun FAIL mais requis
INCONCLUSIVE/blocked/zéro contrôle ; 64 : usage invalide. FAIL+blocage conservés,
exit1. Offline peut PASS pendant que mission reste INCONCLUSIVE.

Manifest par lot : schemaVersion:1, lot, agents[{id,path,skills}], skills[{id,path}],
obligations[IDs G-/T-], requiredLinks[{from,to}], allowedPaths[paths]. Données jamais
commandes ; oracle explicite, non auto-dérivé du texte testé. Union contrôlée T13.

Résultat : mode/niveau/obligation/critère, attendu/observé/verdict,
evidenceKind executed/static-presence/inference, préconditions, commande/outil,
environnement/version, candidat HEAD+hashes+delta, comptages découverts/exécutés/
échoués/skipped, artefacts. SKIPPED/NOT_AVAILABLE requis → INCONCLUSIVE.
Natif : runtime/version/voie autorisée/location/session/appel/heure/candidat,
catalogue et règles effectives ; prompt/principal/secondaires/skills appelés/
références/trace par scénario. Canari : action/ressource/allow-deny/résultat du
gate distinct d'erreur scanner, sentinelles avant/après. Substitution textuelle
d'un profil ne prouve pas son chargement natif.

T04 RED causal avant validateur, GREEN minimal puis non-régression. Fixtures :
frontmatter valide/malformé/type/champ/clé dupliquée/YAML non supporté ; ID/casse/
shader-programming en collision ; lien/skill absent ; .. et junction/symlink
évasion rejetés ou précondition explicitement indisponible ; permissions ordre
inversé/large MCP/shell/execute/subagent ; zéro test/skip/catalogue vide/runtime
absent/trace simulée/ancien candidat/hash modifié/scénario manquant ou dupliqué.
Réexécution stable, preuve partielle jamais complète, validateur sans mutation.
T05–T12 ont des tests locaux indépendants de T04 pendant parallèle ; validation
croisée après intégration. Tout code de test/helper respecte le contrat TDD ;
documents/configs purs gardent les contrôles adaptés sans faux RED.

## 7. Permissions : frontière ouverte, fail-closed et runner distinct

Les permissions V2 sont des gates, pas un sandbox filesystem de code arbitraire.
DENY-ALL puis exceptions minimales est la base recommandée. Dernière règle
correspondante gagne ; chemins edit normalisés, multi-ressources, casse Windows
et external_directory distincts à tester. Préserver les protections héritées
des secrets : read:* ne doit pas annuler silencieusement les asks/denies env.

Planner : plan nommé seulement. Reviewer : briefing nommé seulement, pas tests/
source/golden. Generator : tests/fixtures/config de test énumérés, rapport nommé,
pas permission générale par extension ni `.opencode/*`. Per-lot variable/grant
non documenté interdit. Wildcard d'artefacts ne prouve pas isolement entre lots.

Shell/subagent/spawn/dispatch/handoff, mutateurs Serena, ctx_execute,
ctx_execute_file, ctx_batch_execute/purge/upgrade interdits aux QA restrictifs.
Code Mode denied par défaut : ses seuls MCP de lecture deviennent aussi
inaccessibles. Ne pas prétendre les autoriser par une règle imbriquée tout en
barrant leur seule voie. execute libre + fetch HTTP n'est pas confinement prouvé.
MCP directs safe seulement si exposés et actions EXACTES observées ; pas préfixe
serveur entier ni action 1MCP inventée. Stockage géré index/search/fetch n'est pas
zéro écriture machine ; effets compatibles à qualifier. Besoin de rapport
Ensemble : transport/action minimal à vérifier, pas allow team_* arbitraire.

D-PERM : mode utilisateur retenu, aucun edit sans grant exact vérifié ; retour texte
et persistance lead, tests relus exécutés par runner distinct de confiance et
attribués à celui-ci. Autonomie dégradée/critère artefact autonome blocked à
décider explicitement. Une vraie isolation OS/montages/identité/réseau/canaris
serait une nouvelle mission autorisée ; approval shell non confinée ne satisfait
pas T-06 et exige dérogation humaine, jamais PASS. Aucun contournement si Context
Mode indisponible au profil : lead digère les longues sorties, limite déclarée.

Canaris seulement fixture/clone jetable sans secrets ni source canonique cible,
sous bail lead : profil réellement sélectionné, règles effectives, edit permis/
interdit, patch mixte, .., absolus, casse/slashes, junction si disponible, shell
composé/redirection, MCP mutateur/exécuteur et sous-délégation. Outil absent ou
abstention n'est pas refus technique. Sentinel/hash avant-après. T04 prépare,
seul le lead admet les probes ; pas de nouvelle capacité installée implicitement.

## 8. Boucle QA et transfert T13

Conserver intégralement les méthodes QA.md : deux risques distincts, champs QA
≥10, nominal/négatif et intent, budget/setup/buffer, registre bidirectionnel,
oracles avant code, KEEP/MODIFY/REJECT/DEFER, healer deux réparations maximum,
bug production laissé rouge, reviewer fresh horsboard, scénario mutation concret
avant finding haut, DEFER_TO existants, pin divergent signalé sans réécriture.
Comportement/suite étendue/risque PM≥50 déclenche plan avant auteurs ; plan lead
commité avant worktree ; intégration → generator gaps → reviewer → ré-audit.

T13 : bail unique ; pour chaque fichier autorisé prendre bytes S canoniques,
hash, blob HEAD et delta utilisateur initial. Auteur travaille sur copies de S
explicitement transmises, pas HEAD nue ; remet seulement S→A, hashes et contrôles.
Ne pas intégrer S comme son changement. Lead confirme canonique==S, applique
seul delta ciblé ; divergence → arrêt/reconstruction, pas reset/merge aveugle.
Prouver préservation des hunks utilisateur et hashes des exclus ; commit ciblé
seulement. Modifier un lot intégré exige transfert d'ownership/revalidation.
Fin d'auteur ≠ intégration : consommateurs non publiés avant shutdown/merge,
contrôle, HEAD/diff et entrées disponibles. Un seul écrivain par fichier ;
catalogue/canaris/indexeur/GPU et outputs partagés sous bail exclusif.

## 9. Verdict et limites

T03 a produit un contrat, pas de test/build/canari/routage/GPU/performance ; aucun
RED/GREEN revendiqué. Quatre expertises documentaires livrables sans SDKs.
Avant sortie globale : 19 responsabilités/procédures/oracles/sources non vides,
six skills préservés, découverte et douze observations natives/trois exemples,
canaris QA positifs/négatifs, candidat/preuves/comptages, périmètre et registre.
Inaccessible ou D-PERM ouvert → rapport partiel blocked/INCONCLUSIVE, jamais PASS
global. Un gain, backend, adoption ADR ou sandbox ne découle pas de ce contrat.
