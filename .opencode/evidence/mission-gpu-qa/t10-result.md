# T10 — Pipelines, compilation shader et simulation GPU

Date : 2026-10-16. Auteur : pool-9 / gpu-engineer, feuille.
Tâche : `task_mvbbntgk_0034_rs1ywp`. Évaluation PM/LLM reçue : complexité
brute/retenue **78/78**, risque brut/retenu **59/59** ; aucun modèle imposé.

## Verdict et périmètre

**PASS borné aux contrôles documentaires locaux exécutés** : trois nouveaux
profils, trois skills ciblés, quatre références, manifest et tests autonomes.
**INCONCLUSIVE pour découverte/sélection natives, enforcement, compilation shader,
GPU, image et performance** : aucune de ces exécutions n'a été entreprise par T10.
Ni les regex ni les négatifs textuels ne prouvent ces propriétés du runtime.

Cause du changement : les responsabilités T03 §3 (graphics, compute, compilation,
optimisation shader et simulation) n'avaient pas leurs profils/skills différenciés
dans le HEAD reçu. Pas de bug moteur diagnostiqué ni de correction de pipeline
de production revendiquée. Le patch reste dans l'allowlist T10 §4.

## Candidat, entrées et ownership

- HEAD initial et HEAD de validation avant commit :
  `40a7cc3eac80a4a2d344116365384d48bf92ae48`.
- Worktree :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-9`.
- HEAD détaché et arbre propre constatés à réception. Branche demandée créée au
  même HEAD avant commit :
  `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-9`.
- `t03-contract.md` lu en premier ; qa-plan, inventaires T02, native-preflight et
  execution-brief présents au HEAD exigé, traités via Context Mode. AGENTS.md et
  GPU.md/QA.md canoniques lus en lecture seule, sans copier leur diff utilisateur.
- Serena initialisé/config confirmé sur ARTIST canonique, non réactivé. Aucune
  analyse symbolique C++ ni modification par cette instance. QMD `artist-wiki`,
  intent explicite puis multi_get pour engineering-principles, registre ADR et
  correction-plan ; le contrat T03 et le pin courant priment sur l'historique.
- Pas d'édition moteur/shader de production/build/global/AGENTS/ensemble/politique,
  ni de voisin. Pas de SDK/driver installé, capture, GPU, indexeur de dépôt ou
  probe catalogue/canari. Aucun Docker créé, aucun registre Docker nécessaire.
- Les futurs IDs figés de T03 sont référencés, sans stub ni import de branche
  voisine. Existence/discovery globales et validation T04 réservées à l'intégration.

SHA-256 des entrées canoniques observées à 19:05:31Z (identification de lecture,
pas preuve indépendante de préservation du diff utilisateur initial) :

```text
c9022e1e8c9394a7e320a29ac42d7d6e1463772365345f272221ba4503a89c37  AGENTS.md
be03e5600ecdbfbf6ae58e04f0c2df6433bfadae59d549ca9d2c2f3b134a8873  docs/agentic/GPU.md
4b62a9c68b9a0d32c962260f115e866591043bd7681330377d006734c6a8d870  docs/agentic/QA.md
```

## Livrables et frontières

- `.opencode/agents/gpu-pipeline-engineer.md` : principal commun graphics/compute,
  procédures différentes. Cas 1 : graphics Vulkan/depth → gpu-pipelines +
  vulkan-backend ; API absente → clarification.
- `.opencode/agents/shader-engineer.md` : cas 2 HLSL coûteux → artist-performance
  puis gpu-shader-compilation ; baseline/cible demandées. Cas 4 Vulkan→Metal →
  gpu-shader-compilation + gpu-backend-portability ; Metal natif conditionnel sur
  ABI/capacités, compilation/exécution Apple restent des preuves distinctes.
- `.opencode/agents/gpu-simulation-engineer.md` : cas 5 particules compute →
  gpu-compute-simulation + gpu-pipelines, API/contraintes demandées. CPU seul →
  cpp-engineer via lead. Tous trois mode subagent, deny subagent, sans model.
- `.opencode/skills/gpu-pipelines/` : SKILL.md (75 lignes), références graphics
  (76) et compute (75). Triangle/depth/image versus kernel entier N=257/L=64,
  sentinelles/tail/N=0/limites et consommateur ; attentes et visibilité distinctes.
- `.opencode/skills/gpu-shader-compilation/` : SKILL.md (78), référence ABI (126).
  Slang prioritaire sans adoption, composition/spécialisation/réflexion,
  ParameterBlock et catégories de layout, cible/stage/entry, matrices et packing,
  permutations bornées, invalidation ; shader compilé ≠ pipeline valide.
- `.opencode/skills/gpu-compute-simulation/` : SKILL.md (78), invariants (116).
  Particules/cloth/hair/fluides, seed/dt/substeps, NaN/bornes, stabilité/CFL selon
  modèle, queues/rendu distincts, état runtime plutôt que type par émetteur.
- `.opencode/tests/agentic/t10-pipelines-shaders-simulation/contract.test.mjs`
  et `manifest.json` : oracle explicite T03/plan QA, **22 tests**, aucun import T04,
  aucun routeur parallèle. Aucun nouveau script dans les skills.

Tous précisent création/upload/binding/exécution/consommation/retrait, owner et
emprunt, génération ≠ complétion, `unique_ptr`/getPass emprunté. Ressources/hazards
→ gpu-resource-engineer ; planning des passes → render-graph-engineer ; mapping
et soumission → spécialiste API ; limites/fallback → gpu-portability-engineer.
Ces frontières passent par DEFER_TO au lead, pas de sous-délégation.

## Expériences locales et commandes

Exécutées depuis le worktree ci-dessus via `context-mode.ctx_execute`, qui lance
le binaire Node par `child_process.spawnSync` avec cwd explicite. Le sandbox
Context Mode est Bun (1.4.2 observé), **le test est Node v24.11.1 Windows x64**.
Les logs ont été filtrés dans Context Mode ; extraits utiles ci-dessous.

```text
node --version
node --test .opencode/tests/agentic/t10-pipelines-shaders-simulation/contract.test.mjs
node --check .opencode/tests/agentic/t10-pipelines-shaders-simulation/contract.test.mjs
git diff --check
git diff --cached --check
git diff --cached --stat
git diff --cached --name-only
```

1. **RED contractuel — 18:55:48Z, avant ajout des profils/skills/références.**
   Node v24.11.1 fonctionne ; manifest et rejet du frontmatter malformé passent.
   22 découverts/exécutés, 2 pass, 20 fail, 0 skipped/cancelled/todo, exit 1.
   Motif attendu : assertion `Contrat T10 absent`, pas absence de toolchain/GPU,
   zéro découverte, erreur d'import ou stub.

   ```text
   AssertionError [ERR_ASSERTION]: Contrat T10 absent : .opencode/agents/gpu-pipeline-engineer.md
   AssertionError [ERR_ASSERTION]: Contrat T10 absent : .opencode/skills/gpu-pipelines/references/compute.md
   tests 22 / pass 2 / fail 20 / skipped 0 / exit 1
   ```

   C'est la preuve que le **contrat de présence documentaire** échoue avant ses
   livrables, pas un RED comportemental moteur. Les documents/configs restent
   contrôlés à leur niveau ; aucun scénario GPU non exécuté n'est appelé RED.

2. **Premier contrôle de rédaction — 19:02:42Z.** 22 exécutés, 17 pass/5 fail,
   0 skip, exit 1. Écarts lexicaux (casse vertex input/cloth/licence/pas un type,
   Slang/adoption sur deux lignes) ; rédaction clarifiée. Tests/assertions et
   tolérances inchangés, pas de skip ajouté. Ces échecs ne sont pas des bugs GPU.
3. **GREEN — 19:03:11Z.** 22/22 pass, 0 fail/skipped/cancelled/todo, exit 0.
4. **Non-régression après précision Slang/layout et retouches éditoriales —
   19:05:57Z, deux runs indépendants.** 22/22 puis 22/22, tous exécutés,
   0 fail/skipped/cancelled/todo ; exit 0 pour les deux. Durées observées du runner
   100,6332 ms / 108,19 ms, **pas un benchmark ni une performance moteur**.
   `node --check` exit 0.
5. **Périmètre/whitespace — 19:05:31Z.** `git diff --check` et
   `git diff --cached --check` exit 0, sans sortie. Douze fichiers candidats
   nouveaux, 1047 insertions avant ce rapport, aucune suppression ; liste comparée
   au manifest : `outside allowlist=[]`. Le rapport est le treizième fichier,
   également autorisé. Recontrôle du diff final avant commit.
6. **Recontrôle avec rapport — 19:08:39Z.** Node confirme win32/x64/v24.11.1 ;
   22/22 tests, 0 fail/skip, exit 0 ; deux `git diff --check` (index et worktree)
   exit 0. Treize fichiers indexés autorisés, aucun hors allowlist, aucun delta
   non indexé ; 12 hashes du rapport recoupés, 0 divergence. Le dossier evidence
   étant ignoré, seul `t10-result.md` a été ajouté avec `git add -f -- <ce chemin>`.

Négatifs locaux exécutés : frontmatter absent, permission `deny` remplacée par
`allow`, distinction shader/pipeline retirée et sentinelle compute retirée.
Mutations **en mémoire seulement**, assert.throws causal ; aucune fixture GPU
faussement exécutée. Le sous-ensemble metadata testé est intentionnellement
strict/local : ce n'est pas un parser YAML général ni une preuve V2 native.

## Provenance et dispositions

Rédaction originale ; aucun code, skill, helper ou superviseur tiers copié/adapté.
Inventaires T02 conservés tels quels ; tree SHA ≠ commit. Licence inconnue ou
NOASSERTION → lien seulement ; toute adaptation future exigerait commit résolu,
chemin et notices exacts. Pas de dépendance réseau pendant les tests locaux.

- OpenCode V2 agents/skills/permissions : pages officielles fetch/indexées le
  2026-10-16. Format `.opencode`, name+description ; name=ID convention locale,
  pas exigence native inventée. Permissions ordonnées, uniquement deny subagent
  ajouté ; aucune garantie de sandbox implicite. Guide create-skill utilisé pour
  progressive disclosure, **pas ses chemins/champs Cursor**.
- Slang, glslang, DXC, SPIRV-Tools et SPIRV-Cross : références T02, non importées.
  Matrices Slang et chapitre pipeline Vulkan consultés ; version exacte à fixer
  lors d'une future exécution, aucun commit tiers revendiqué pour ce lot original.
- Docs Khronos/Microsoft/Apple : liens techniques ciblés pour pipelines, compute,
  bindings et synchronisation ; ne prouvent ni device ni feature activée.
- GPUGems fluide et PositionBasedDynamics : références conceptuelles additionnelles,
  licences non réévaluées par T10, **lien seulement**, aucun algorithme importé.
  Modèle à choisir avant implémentation, pas une nouvelle dépendance ARTIST.
- Vérification HTTP HEAD via Context Mode à 19:04:36Z : **19 URL distinctes,
  19 statuts 200**. Cela établit leur accessibilité, pas licence/contenu/actualité.
  Une URL Slang reflection essayée auparavant renvoyait 404 : non retenue, lien
  officiel du projet à la place, sans prétendre avoir lu cette page inexistante.
  Une erreur de regex du script ad hoc de collecte des liens a été corrigée dans
  Context Mode avant relance ; elle ne constitue pas un RED de ce contrat.

## Registre borné et transfert au lead

- **G-01** : provenance/dispositions explicites, liens accessibles ; pas de copie
  autorisée par ces liens. Revue indépendante des références non effectuée.
- **G-02** : procédures distinctes, interfaces, exemples nominaux/négatifs et
  sources présents, contrôlés statiquement. Qualité sémantique auto-relue seulement.
- **G-03** : metadata/liens propres au lot vérifiés ; découverte native et voisins
  non évalués. Manifest contractuel transmis à T04/T13 après intégration.
- **G-04** : cas 1/2/4/5 et clarifications écrits ; aucun choix/appel natif observé,
  aucun programme de lookup annoncé comme routeur. **GAP → T13**.
- **G-05** : limites OpenGL 3.3, IR/ABI/pipeline/GPU/Apple et oracles documentés ;
  aucune exécution des exemples GPU. **INCONCLUSIVE** sur ce comportement.
- **G-06** : preuves CPU texte/offline/GPU/image/perf séparées ; zéro PASS GPU.
- **G-07** : diff local dans allowlist ; refus d'écriture technique et conservation
  globale du diff utilisateur restent la vérification du lead, pas un canari T10.

DEFER_TO lead : shutdown/merge puis validation T04, liens globaux/discovery et
observations natives des cas 1/2/4/5 sous bail ; review fresh des frontières/ABI.
Ne pas lancer de canari ni de GPU en parallèle pour ce lot documentaire. D-PERM
reste ouvert selon T03, aucune permission QA sensible modifiée ici.

Impact CPU/GPU **non mesuré** : aucune source runtime/build/shader de production
modifiée, donc aucun changement direct du chemin moteur dans ce patch. Coût du
chargement des prompts non mesuré ; le chargement sélectif est une intention,
pas un gain de contexte ou de temps prouvé. Aucune qualité visuelle altérée par
un changement de golden/tolérance, puisqu'aucun rendu n'a été modifié/exécuté.

## Identité des fichiers validés

SHA-256 des douze fichiers du candidat (hors ce rapport auto-référent), bytes du
worktree à 19:05:31Z, identiques pendant les deux runs de non-régression.
Le commit de livraison identifie aussi le rapport et sera transmis au lead.

```text
caad25ad03abd093e4f6cb960ef233faf6bb3fc7ff1fb9fd3937ce934834185f  .opencode/agents/gpu-pipeline-engineer.md
cd09433e1b738c5d558cdb2440babaef296414b64b3624c5da0a66ea4cac133c  .opencode/agents/gpu-simulation-engineer.md
3f4c3d55d8a6002e4ca0fc05c68ea80c4419a4632ce50a8b91efc062456236c1  .opencode/agents/shader-engineer.md
03b9299013f7ee1a9c9ed28f6ad23ca828867197d7bc529edaee2a20fb51b875  .opencode/skills/gpu-compute-simulation/SKILL.md
1ae6c25494a04cf8893e4a24d01f597596ddfba83c3c578b5a0d4e387d3b3485  .opencode/skills/gpu-compute-simulation/references/invariants.md
ec0c52850caa941ba39c119cdcf297847dcf9c2957050a32995aef3b6226106d  .opencode/skills/gpu-pipelines/SKILL.md
6911710f1bb42430e1e96ab6b6cce9566adf6ec9d70dd97c3c72b8d9b59face1  .opencode/skills/gpu-pipelines/references/compute.md
1f0437043e3b93002e88e71b0325712a8b4005660426a0076bfa176663dcfde3  .opencode/skills/gpu-pipelines/references/graphics.md
e56a7f1398bfdcba57d0b0328419c53ed60ba7bdb5740125d590be75f4ee9425  .opencode/skills/gpu-shader-compilation/SKILL.md
966cf33b175a807401b177f89328833e814d4bd430cc9304a7fa563cbef5477b  .opencode/skills/gpu-shader-compilation/references/compilation-abi.md
cf43485135c8964a9bb9fede4695ce29c8f036d6d97d0fe490fe91019eb84aaf  .opencode/tests/agentic/t10-pipelines-shaders-simulation/contract.test.mjs
06ae7177b0ebc4a22f10138ddc9d7ce4e45e6484f58d199ebf5f7d23d9e39834  .opencode/tests/agentic/t10-pipelines-shaders-simulation/manifest.json
```
