# T06 — expertise et skill Vulkan

Date : 2026-10-16. Auteur : pool-5 / gpu-engineer, feuille.
Tâche `task_mvbbntgj_0030_kpzd2u`. PM/LLM : brut C58/R44, retenu
**complexity=70, risk=44** ; aucun modèle/variant choisi ni score modifié.

## Résultat et périmètre

**PASS local pour les contrats documentaires testés** : un profil V2 spécialisé,
un skill à sélection progressive et quatre recettes Vulkan originales avec
nominal/négatif/oracle/limites. **INCONCLUSIVE** pour découverte/chargement/routage
natifs après intégration, enforcement, compilation/offline SPIR-V, API/layers,
GPU/image et performance. Aucun backend Vulkan ARTIST ajouté.

Hypothèse de travail : le manque à combler est l'expertise API et ses frontières,
pas un défaut de rendu reproduit. Cause/documentation observée : l'allowlist T06
ne contenait ni profil Vulkan ni skill correspondant à l'entrée du lot. Les
recettes ne prétendent pas être des exécutables ou résultats GPU.

Base vérifiée avant écriture : `40a7cc3eac80a4a2d344116365384d48bf92ae48` ; worktree
propre. La session pointait initialement la racine canonique : lecture seule du
contrat, puis `session_move` vers le worktree pool-5 avant toute écriture.
Ce worktree était detached à la même base ; branche locale créée ensuite :
`ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-5`.
Le commit de livraison est l'enfant de cette base contenant ce rapport ; son
SHA est remis au lead, non auto-référencé dans son propre contenu.

Sept fichiers nouveaux, aucun fichier préexistant modifié :

- `.opencode/agents/vulkan-backend-engineer.md` ;
- `.opencode/skills/vulkan-backend/SKILL.md` ;
- `.opencode/skills/vulkan-backend/references/recipes.md` ;
- `.opencode/skills/vulkan-backend/references/sources.md` ;
- `.opencode/tests/agentic/t06-vulkan/contract.test.mjs` ;
- `.opencode/tests/agentic/t06-vulkan/manifest.json` ;
- `.opencode/evidence/mission-gpu-qa/t06-result.md`.

Le manifest schemaVersion1 contient les IDs/obligations/liens et sept chemins
exacts, pas une allowlist générale, des commandes ou un routeur. Les tests
importent seulement les modules intégrés de Node ; aucune dépendance T04, branche
voisine, fixture exécutée par import ni stub. L'existence des futurs profils
mentionnés en prose n'est pas testée avant leur intégration.

## Entrées et pin observé

Contrat T03 lu en premier, puis qa-plan, t02-source-inventory,
t02-external-sources, native-preflight et execution-brief intégrés.
Lecture canonique explicite, sans copie du diff utilisateur : AGENTS.md,
docs/agentic/GPU.md, docs/agentic/QA.md ; principes, ADR et correction-plan consultés.
Serena initial_instructions/get_current_config : source ARTIST canonique,
pas de réactivation de projet ni édition symbolique. Aucun nouveau build C++.

Pin en dix points :

1. Backend implémenté : OpenGL, pas Vulkan.
2. Sources moteur : `ARTIST/include/`, header-only C++23.
3. Backend actuel : `ARTIST/include/graphic/opengl/`.
4. Composants réels : `ARTIST/component-tests/`, GLFW/GLEW 3.3 core.
5. PipelineContext : `unique_ptr` et `getPass` emprunt, fait T03 recoupé.
6. ADR-0001/0002/0004/0006/0007 : Proposed, non promus par le lot.
7. ADR-0003/0005 : Accepted/partial ; ADR-0008 : Windows Debug seulement.
8. Node local exécuté : v24.11.1 ; runtime sandbox Context Mode distinct.
9. T02 : RTX 3090 / driver 617.42 ; Vulkan instance 1.4.341/device 1.4.351,
   neuf layers sans `VK_LAYER_KHRONOS_validation` ; pas de nouvelle sonde T06.
10. T02 : SDK Vulkan non identifié aux emplacements sondés, outils SPIR-V absents
    du PATH observé ; cela ne prouve pas une absence machine exhaustive.

Empreintes canoniques recoupées après rédaction, inchangées par rapport au brief :
AGENTS.md `c9022e1e8c9394a7e320a29ac42d7d6e1463772365345f272221ba4503a89c37` ;
GPU.md `be03e5600ecdbfbf6ae58e04f0c2df6433bfadae59d549ca9d2c2f3b134a8873` ;
QA.md `4b62a9c68b9a0d32c962260f115e866591043bd7681330377d006734c6a8d870`.
Le worktree n'a reçu ni copié le diff utilisateur. Aucun changement moteur,
shader, build, global, politique, ensemble.json, AGENTS ou fichier voisin.

## Contrats techniques livrés

- VK-01 : versions/queries/features activées, pNext, queues/familles, refus de
  feature manquante avant submit ; faux support fixture CPU ≠ observation GPU.
- VK-02 : pass offscreen 64² RGBA8_UNORM/D32, triangle et push constants explicités,
  NDC Z [0,1], deux profondeurs 0,25/0,75, oracle rouge/fond noir, négatifs
  depth désactivé et format de rendu incompatible. Layouts/readback séparés.
- VK-03 : 256 uint32, storage descriptor 1024 octets, compute → copy → host,
  scopes/stages/access, fence et invalidation non cohérente ; négatifs type
  descriptor et hazard RAW, génération CPU périmée refusée avant driver.
- VK-04 : transfert EXCLUSIVE entre familles, release/semaphore/acquire,
  scopes ALL_COMMANDS justifiés pour l'ownership par défaut, retraite multiqueue
  et générations ; variante même famille distincte, pas de faux resource states.

Le spécialiste pipeline reste principal au scénario 1 du contrat ; Vulkan est
le skill API complémentaire. Ressources, render graph, shader, portabilité et
vérification gardent leurs responsabilités. Les instructions `DEFER_TO` passent
par le lead sans sous-délégation. Permission V2 subagent deny déclarée ; pas de
modèle ni champ Cursor importé. La forme n'est pas une preuve d'enforcement.

## Tests exécutés : RED → GREEN → non-régression

Répertoire pour toutes les commandes :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-5`.
Exécution/filtrage des sorties par `context-mode.ctx_execute`, via
`child_process.spawnSync`, cwd explicite, sans output partagé.

Commandes exactes :

```text
node --version
node --test .opencode/tests/agentic/t06-vulkan/contract.test.mjs
git diff --check
git diff --cached --check
git diff --cached --name-only
```

Oracles écrits avant les profils depuis qa-plan G-01..G-07/T-07 et
t03-contract §3.16, §4, §5.1, §6 ; attendus explicites, non dérivés du contenu.

1. **RED structurel** avant création profil/skill/références : exit1,
   9 tests découverts/exécutés, 1 PASS / 8 FAIL, 0 skipped/cancelled/todo.
   Première cause : `artefact T06 obligatoire absent :
   .opencode/agents/vulkan-backend-engineer.md` (ERR_ASSERTION).
   Le manifest passe ; Node fonctionne. Ce n'est pas un défaut API reproduit,
   ni un échec d'environnement présenté comme RED GPU.
2. **GREEN documentaire** après ajout minimal des artefacts : exit0, 9/9,
   0 skip. Forme V2 utilisée, liens, pin, frontières, préconditions et recettes
   couvertes par assertions textuelles bornées, pas un parser YAML général.
3. **RED causal du contrôleur local** : ajout d'un mutant en mémoire avec une
   règle subagent allow APRÈS deny. L'ancien contrôle acceptait la sous-chaîne
   deny sans vérifier la fin du bloc : exit1, 8 PASS / 1 FAIL, 0 skip,
   `AssertionError: Missing expected exception`.
4. **GREEN minimal du contrôleur** : égalité de la totalité du bloc permissions
   jusqu'à fin du frontmatter, sans affaiblir les assertions. Les six mutations
   en mémoire (allow, allow après deny, primary, modèle, accès shader supprimé,
   invalidation supprimée) sont rejetées. Aucun fichier fixture n'est modifié.
5. **Non-régression** après précision des positions/push constants/storeOps :
   2026-10-16T19:01:41Z, exit0, **9 découverts / 9 exécutés / 9 PASS / 0 FAIL /
   0 skipped / 0 cancelled / 0 todo**, durée Node 79,1909 ms. Cette durée n'est
   pas un benchmark du harness ou du moteur.

Un premier script de préflight Context Mode avait une chaîne newline mal échappée ;
corrigé dans le même outil avant tests. Aucun de ces essais de script n'est compté
comme RED. Le premier `git diff --check` sans staging ne couvrait pas les nouveaux
fichiers ; le contrôle final porte aussi sur le diff staged (sept additions).
Les deux `diff --check` finaux retournent exit0, sans diagnostic ; la liste
staged égale exactement les sept allowedPaths. Relance après staging : exit0,
9/9 PASS, zéro skip (94,8484 ms, non interprété comme mesure de performance).

## Identité des six artefacts testés

SHA256 des bytes, avant commit ; le rapport n'essaie pas de contenir son propre hash :

- Agent : `dd29256d92bf68d4bf7d751fa513d15126d04665f256afe6ab40e4ed7ea335a3`.
- SKILL : `4703aaa7220d1f369065c2f74aaf41319f3a3458ec8dd2237a75aa4e90e7cfb2`.
- Recipes : `9d0664bcf46289d17af34dce343b59faeb61376c2eedba0f50e0b58869252a64`.
- Sources : `a3c376aaebcab24f7c114abd04b988b5cbc4bf61b7d299aea12634badc0659e5`.
- Test : `5269ea7c0742bc6399757a7e75575ef990fa95c2874a78aed3413677552dd2ca`.
- Manifest : `a3549f4ff189bbef3cb3cc9b6d38bd45598cc8f338aee7f14ad2eef4f4ac5126`.

## Sources, dispositions et niveaux de preuve

Les pages officielles OpenCode V2 agents/skills/permissions et trois chapitres
Vulkan-Guide (enabling_features, synchronization, validation_overview) ont été
récupérés via Context Mode, lecture ciblée le 2026-10-16. Guides main mutables,
CC-BY-4.0 observée, pas de commit résolu ; pages V2 licence de copie non établie.
**Lien seulement**, rédaction originale, aucun extrait/code tiers adapté.
Les refpages Khronos ciblées sont référencées sans prétendre avoir levé le 403
docs.vulkan.org de T02. Vulkan-ValidationLayers : lien seulement, licence exacte
de copie non résolue. Arbre SHA CppStudio ≠ commit ; aucun import superviseur.

- `executed` : 9 tests Node locaux, mutations statiques, lecture/empreintes/diff.
- `static-presence` : permission/config, termes API, sources et oracles, liens.
- `inference` : coût runtime moteur estimé inchangé (aucun code moteur changé).
  Pas de mesure CPU/GPU, ni gain de performance revendiqué.
- G-01/G-02 : provenance et responsabilité 16 livrées, pas couverture globale 19.
- G-03/G-04 : contrats statiques/scénario 1 préparés, découverte/routage natifs non
  exercés sous bail. Une annotation passive de nouveau skill par le client n'est
  pas une trace de sélection/chargement, ni l'observation des douze scénarios.
- G-05/G-06 : niveaux/préconditions explicités ; aucun compilateur shader,
  spirv-val, layer, device, draw/dispatch ou image réellement exécuté par T06.
- G-07/T-07 : allowlist et candidat locaux contrôlés ; confinement et preuves
  globales restent à l'intégration, sans assimilation de texte à une barrière.

## Transfert au lead / limites restantes

`DEFER_TO` lead : shutdown/merge/inspection avant consommateurs ; validation
croisée T04 et découverte native des IDs après intégration ; scénario 1 principal
gpu-pipeline-engineer avec gpu-pipelines+vulkan-backend ; vérification indépendante
du contenu et des règles effectives. Aucun probe modèle/canari/catalogue/GPU
supplémentaire lancé par T06. D-PERM QA ne dépend pas de ce lot documentaire.

Les négatifs Vulkan sont des protocoles non exécutés, donc INCONCLUSIVE comme
preuves driver. Absence de SDK/layer n'empêche pas la livraison documentaire.
Aucun benchmark, installation, modification SDK/driver, capture ou ressource
Docker créé ; aucune entrée Docker à ajouter. Aucun sous-agent/spawn/auto-claim.
