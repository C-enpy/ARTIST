# T11 — ressources, graphes, capacités et portabilité

2026-10-16, auteur pool-10 / gpu-engineer, feuille. PM/LLM : estimation brute et
retenue **complexity 84 / risk 64** ; grille inchangée, aucun modèle imposé.

## Verdict et périmètre

**PASS borné documentaire/offline local** : trois profils, quatre skills et quatre
références originales, manifest explicite, 22 tests Node de contrat statique.
**INCONCLUSIVE** pour découverte/chargement/routage natifs des nouveaux profils,
enforcement de permissions, GPU, image, compilation shader et performance.
Ce retour ne clôt pas globalement G-01..07/T-07 ni la mission.

Hypothèse initiale : les responsabilités 19.1/6/7/8/14 et scénarios 3/8/11/12
n'ont pas encore leurs profils/skills T03 dans cette base. Preuve : oracle local
testé avant contenus, 21 échecs de présence, manifest valide. Patch borné :
procédures et frontières explicites, pas nouveau backend/RHI/compilateur de graphe.
La génération ne sert jamais de fence ; dernières utilisations de toutes les
queues, ordre/visibilité/attente CPU séparés. Les capacités et conventions quatre
API sont une matrice **conceptuelle**, pas une matrice matériel exécutée.

## Candidat, ownership et entrées

- Base intégrée vérifiée avant écriture :
  `40a7cc3eac80a4a2d344116365384d48bf92ae48`, worktree initial propre.
- Worktree :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-10`.
  Git initialement detached HEAD (worktree géré par Ensemble) ; identité de branche
  de mission `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-10`.
- Commit des oracles avant les contenus dans l'historique :
  `460e5db6ebaf6cfb4be008f1dbdb28a48b6711ff` (deux fichiers de test seulement).
  Le commit de livraison ajoute les contenus et ce rapport ; son ID est donné au lead.
- T03 lu en premier ; plan QA, audits T02, native-preflight et execution-brief
  présents/indexés/consultés. Canonical AGENTS.md, GPU.md et QA.md lus sans copie
  du delta utilisateur. QMD artist-wiki avec intent puis multi_get pour principes,
  registre ADR et correction-plan. Serena bound ARTIST vérifié, pas d'activation.
- Préflight natif T02/lead est une preuve historique d'accès aux profils
  préexistants uniquement. Aucun probe catalogue, canari, GPU, build/indexeur ou
  output partagé réservé n'a été pris par cet auteur.
- 14 fichiers nouveaux dans l'allowlist ; aucun fichier existant modifié.
  `platform-capabilities.md`, moteur/shaders, build, global, AGENTS, commandes,
  politique et voisins intacts. Aucun Docker créé, aucun SDK/outil installé.

## Livrables et registre bidirectionnel

Préfixe agents `.opencode/agents/` : `gpu-resource-engineer.md`,
`render-graph-engineer.md`, `gpu-portability-engineer.md`.
Préfixe skills `.opencode/skills/` :

- `gpu-resource-lifecycle/SKILL.md` et `references/ledger-and-hazards.md` :
  owners/borrows, générations, plages/descripteurs, résidence, tokens par queue,
  destruction au contexte valide, RAW/WAR/WAW, mappings API conditionnels.
- `gpu-render-graph/SKILL.md` et `references/graph-contract.md` : authoring,
  plan cook immuable, runtime-frame, imports/exports, cycle/read-before-produce,
  scheduling/aliasing/fusion, clé et invalidation/rebuild/retrait.
- `gpu-capability-detection/SKILL.md` et `references/api-capabilities.md` :
  observé/supporté/activé/compatible/inconnu, limites/formats/usages, queries et
  fallback par API ; mesh shader jamais inféré de RTX.
- `gpu-backend-portability/SKILL.md` et `references/conventions.md` :
  clip/depth, orientation/handedness, matrices/ABI, couleur linéaire/sRGB,
  qualité/fallback ou écart non représentable, pass/image/oracle.

Tests : `.opencode/tests/agentic/t11-resources-graph-portability/contract.test.mjs`
et `manifest.json`, indépendants du validateur T04. Aucun import ni stub voisin.
Le manifest contient schemaVersion 1, lot, 3 agents, 4 skills, 8 obligations,
11 liens requis et 14 chemins exacts, fixés depuis T03/QA avant rédaction.

Chaque test est nommé avec ses obligations, sans test orphelin :

- G-02/G-07 : `manifest explicite…` vérifie IDs, chemins, skills et allowlist
  exacts. Complété par contrôle Git des chemins effectivement ajoutés.
- G-03 : 3 tests `format et rôle feuille…`, 4 `contrat de skill…`, 1 `liens
  requis…`. Vérification de forme locale étroite, **pas parser YAML V2 complet**.
  Nom égal ID est convention ARTIST, pas exigence native inventée.
- G-02/G-04 : `principal et skills minimum…` vérifie le texte des quatre scénarios,
  pas le comportement d'un routeur. Aucun dispatch supplémentaire implémenté.
- G-02/G-05 : `ledger…` et `graphes distincts…` ; G-05/G-06 : `capacités quatre
  API…` et `conventions…` : clauses matérielles de procédure présentes.
- G-01 : `références officielles…` vérifie dispositions explicites, pas une
  autorisation juridique ni la validité distante des URL. Contrôle HTTP distinct.
- G-06/T-07 : `pin observé…` impose pin actuel/limites dans les profils ; hashes
  ci-dessous identifient le contenu testé, sans authentifier une future trace native.
- Six tests négatifs par mutation **en mémoire** : mode primary, subagent allow,
  modèle imposé, procédure absente, négatif absent, frontmatter non fermé/refusé.
  Ils prouvent ces assertions locales, pas un gate natif ou un sandbox.

États des obligations : contrôles statiques couverts dans ce seul périmètre ;
preuve native G-03/G-04 et permissions → **unassessed**, à réaliser par lead après
intégration sous bail. Preuve GPU/image/perf G-05/G-06 → **unassessed**, hors besoin
du lot documentaire ; aucune acceptation de gap ou adoption ADR par l'auteur.
La suite par chaînes ne détecte pas toute contradiction de prose : revue
technique indépendante encore nécessaire, pas remplacée par ces 22 tests.

## Exécution reproductible et causalité

Windows, Node **v24.11.1** vérifié par `node --version`. Le sandbox Context Mode
utilise Bun pour lancer Node ; les tests ont réellement été exécutés par Node,
sans dépendance installée. Commandes dans le worktree ci-dessus, via Context Mode :

```text
node --test .opencode/tests/agentic/t11-resources-graph-portability/contract.test.mjs
git diff --check
git diff --cached --check
```

1. **Contrat RED statique avant contenus** : 22 découverts/exécutés, 1 PASS,
   21 FAIL, 0 skipped/cancelled/todo, exit 1. Extrait causal :
   `AssertionError: Contrat T11 absent : .opencode/agents/gpu-resource-engineer.md`.
   Les autres échecs désignent les fichiers T11 absents. Node/manifest fonctionnels.
   C'est la détection de l'absence des livrables, **pas une régression moteur/GPU**
   ni une preuve comportementale fabriquée pour du Markdown.
2. Première passe après rédaction : 21 PASS / 1 FAIL, même 22 tests, exit 1.
   `Clause obligatoire absente : attente CPU` (la référence avait seulement un
   titre capitalisé). Ajout d'une phrase explicitant les trois preuves séparées ;
   oracle et assertions inchangés, aucune tolérance/skip modifié.
3. **GREEN local** : 22/22 PASS, 0 FAIL/skipped/cancelled/todo, exit 0. Les six
   mutations négatives refusées comme attendu. Tests et manifest byte-identiques
   au RED (hashes ci-dessous). Pas de refactor exécutable requis.
4. Contrôle documentaire : un lien Apple proposé retournait HTTP 404 ; remplacé
   par la page officielle `MTLFence` vérifiée HTTP 200. Aucune concession de test.
   Relance finale après ce delta et ce rapport : **22/22 PASS**, zéro échec,
   skip, cancelled ou todo ; exit 0. Les deux diff-check ont aussi retourné 0.

`git diff --check` exit 0 sur le worktree ; le contrôle indexé est également
nécessaire pour les nouveaux fichiers, un diff non indexé vide ne les inspecte
pas. Contrôle staging contre la base : **14 fichiers, 1344 insertions**, zéro
chemin hors allowlist avant cet ajout de résultat final. Le rapport, ignoré par
`.gitignore:20`, est ajouté explicitement avec `git add -f` sur ce seul chemin.
La restitution finale au lead porte les résultats après staging et le commit.
Les extraits ci-dessus sont les preuves conservées ; pas de faux log de driver.

## Empreintes SHA-256 du candidat testé

Chemins des trois agents, quatre skills et références dans l'ordre donné plus haut :

```text
0b55f1f6152accc834d4cdb2f84d9bc3059717b687121eb0add47a0c040ccca4 agents/gpu-resource-engineer.md
bf381454c8813ad3d5797e5dd9c5a350f742772725690410ed6a3494f4e8a7ce agents/render-graph-engineer.md
a1e1f59e1b82113a9a481182fd64ae40f44b5559b73e8847f6f9be66d1f7eba5 agents/gpu-portability-engineer.md
45907936afb7a7ebcb8a0f12a4e2391e381af2468b28813e72dc8f6c2738b3e9 skills/gpu-resource-lifecycle/SKILL.md
c026a9ee9e5657d4692449d38a76951d3f8858ad5f58c212d1e857773bb3cf5c skills/gpu-resource-lifecycle/references/ledger-and-hazards.md
9d556df5dfe70149f8c72c9a198458437d928352bc1e4fcf7ebdcde4d7a7ec1b skills/gpu-render-graph/SKILL.md
d1aad0cc4f1de17e9088df26404d322a3dc502a5371739293713bffc92c7294c skills/gpu-render-graph/references/graph-contract.md
2efbda8373af7b598b1a166a2ab632efdaedfe749fa2290c415842d7fe6aa374 skills/gpu-capability-detection/SKILL.md
c977a3efa69921b3d5c04d46f78b4c6e873ba66fbd3065bafa4e375337300d00 skills/gpu-capability-detection/references/api-capabilities.md
4344d779ab11b2b5f8c5544c56220b11cc9e80a9f37ce366c7e2c38b02dba265 skills/gpu-backend-portability/SKILL.md
a50d60c0aa793e66d36b4366653da71503620b08e012745fa7341f5b1b74afa8 skills/gpu-backend-portability/references/conventions.md
d632d08fc810d473bba9bb8b2d0116ba234773c2fc22f4ec87d044ef84d25951 tests/agentic/t11-resources-graph-portability/manifest.json
33bb70ae89bf66698e2d762208466453755545f1cc2b0696ea74a675e16d975e tests/agentic/t11-resources-graph-portability/contract.test.mjs
```

Préfixe de ces chemins : `.opencode/`. Ce rapport ne s'auto-hashe pas ; Git
identifie sa version. Une modification après run exige revalidation/hashes neufs.

## Sources, dispositions, préconditions restantes

Format natif : pages OpenCode V2 agents/skills/permissions récupérées/indexées
le 2026-10-16 ; descriptions, mode subagent et règles action/resource/effect
conformes à la forme observée. Pas de modèle/variant, pas de champs Cursor importés.
Permissions subagent deny inscrites, mais enforcement non testé par cet auteur.

Corpus T02 consulté : dispositions **créer rédaction originale / référencer**,
aucune copie de CppStudio, GPU Capabilities, Slang RHI, bgfx, Filament ni des couches
de traduction. Leurs licences signalées au dépôt ne sont pas des notices de chaque
fichier ; tree SHA ≠ commit. Aucune nouvelle dépendance ni superviseur importé.

16 URL officielles uniques dans les références : 7 fetch/index ; corps utilisables
pour Vulkan synchronization/features et D3D12 barriers/features, titres seuls pour
Metal supportsFamily et GL memoryBarrier/clipControl. Neuf autres URL finales
vérifiées par HTTP HEAD 200 (après remplacement du 404 Metal), **pas lecture complète**
des PDF ou preuve d'exactitude de chaque clause. Liens officiels de consultation,
licence de copie non déduite. Les mécanismes doivent être recoupés à la version
de l'API ciblée lors d'une future exécution ; aucune source tierce exécutée.

DEFER_TO lead après shutdown/merge/contrôle : validation T04 croisée, existence
globale des liens voisins, découverte effective et observations natives scénarios
3/8/11/12. Références aux futurs IDs figés autorisées par T03 ; aucun stub créé.
Demander revue indépendante ressources/synchronisation et conventions ; celle-ci
n'a pas été exécutée par cet auteur feuille. Ne pas présenter l'auto-lecture comme
une telle revue. Aucun build/CTest/shader/GPU/capture/benchmark exécuté ici.

Impact CPU/GPU runtime **estimé inchangé**, car aucun code moteur/shader ni état
driver modifié ; pas de mesure. Coût de contexte limité par quatre skills courts
(88–92 lignes) et références sur demande, sans gain de tokens mesuré. Qualité
visuelle exigée par les procédures mais non observée sur une image dans ce lot.
