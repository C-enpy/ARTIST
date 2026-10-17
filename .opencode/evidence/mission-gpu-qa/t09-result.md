# T09 — OpenGL historique et diagnostic frame

Date : 2026-10-16. Auteur : pool-8 / `gpu-engineer`.
Tâche : `task_mvbbntgj_0033_ov2e5d`. PM/LLM : brut C66/R58, retenu C70/R58
(plancher développement), sans modèle/variant imposé par le lot.

## Candidat, entrées et périmètre

- HEAD de départ vérifié : `40a7cc3eac80a4a2d344116365384d48bf92ae48`, worktree
  initialement propre. La session arrivait dans le dépôt canonique ; déplacement
  natif vers le worktree dédié **avant toute écriture**. HEAD identique confirmé.
- Branche livrée :
  `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-8`.
- CWD des commandes :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-8`.
- Contrat T03 lu en premier, puis `qa-plan.md`, `t02-source-inventory.md`,
  `t02-external-sources.md`, `native-preflight.md`, brief
  `.opencode/missions/team_mvb9l9ux_001g_mzvoa2/execution-brief.md`.
  `AGENTS.md`, `docs/agentic/GPU.md` et `QA.md` canoniques consultés en lecture
  seule via Context Mode ; aucun diff utilisateur copié. Principes, registre ADR
  et correction-plan consultés ; pin T03 unique_ptr/emprunt, pas shared_ptr ancien.
- Allowlist T09 respectée : deux profils, deux skills et leurs références,
  test/manifest locaux et ce rapport. Aucun moteur, shader de production, build,
  AGENTS, commande, voisin, politique, ensemble.json ou configuration globale modifié.
- Pas d'installation, capture, benchmark, sonde catalogue/canari ou ressource Docker.
  Aucun sous-agent, spawn, auto-claim, handoff ou appel de session modèle.

## Cause documentaire et patch borné

L'ancien profil GPU gardait une formulation multi-backend, sans skill OpenGL
spécialisé. La méthode de capture existante ne détaillait pas event/attachment/
canal/replay ni le statut du candidat rdc-cli. C'est un écart de contrat du harness,
**pas un défaut driver/image reproduit**.

- `gpu-engineer` reste l'unique ID OpenGL historique : chargement sélectif du debug
  puis backend pour « OpenGL noir », reproduction avant profiling/migration,
  API/états/GLSL/ressources/lifetime et DEFER_TO préservés/enrichis.
- `rendering-verifier` garde ses permissions exactes et son indépendance : méthode
  frame RenderDoc en lecture seule, jamais auteur du correctif/tests/goldens.
  Limite D-PERM/écriture indirecte déclarée ; runner/lead si voie absente.
- `opengl-backend` créé, avec référence ciblée contexte/thread/share group,
  version/profil/extensions/points d'entrée/limites, gates compute/SSBO/4.3,
  persistent mapping/4.4 et KHR_debug, VAO/VBO/EBO/FBO, upload/GLSL/logs/états,
  ordre/visibilité/attente CPU et destruction compatible avec les règles GL.
- `artist-gpu-debug` enrichi sans perdre réduction, mathématiques, colorimétrie,
  resize/multi-frame, erreurs et niveau des preuves. Référence frame/capture,
  fallback manuel, rdc-cli candidat documentaire seulement, sources et limites.
- Les six IDs historiques existent ; les cinq autres SKILL.md sont identiques
  octet par octet au HEAD de départ dans ce worktree. Aucun doublon
  `opengl-backend-engineer` créé. Aucun helper/import depuis T04 ou un lot voisin.

## Commandes et preuves RED → GREEN → contrôle final

Environnement exécuté : Windows_NT 10.0.26300 x64 ; Node **v24.11.1**,
`C:/Program Files/nodejs/node.exe`. Le sandbox Context Mode utilise Bun, mais les
tests ci-dessous lancent bien **node** par `child_process.spawnSync`, avec CWD
explicite, stdout/stderr capturés et résultat résumé. Aucune toolchain C++ requise.

```text
node --version
git rev-parse HEAD
node --test .opencode/tests/agentic/t09-opengl-debug/contract.test.mjs
node --check .opencode/tests/agentic/t09-opengl-debug/contract.test.mjs
git diff --check
git diff --cached --check
```

- **RED exécuté avant édition des profils/skills** : exit **1**, 12 tests
  découverts/exécutés, **1 PASS / 11 FAIL / 0 skipped**, 0 cancelled/0 todo.
  Manifest/oracle écrits en premier depuis T03, non générés depuis les fichiers
  cibles. Raisons causales observées :
  - `gpu-engineer.md doit référencer opengl-backend` ;
  - `frontière RO manquante : lecture seule` (permissions déjà inchangées) ;
  - description OpenGL absente ;
  - livrables backend/références absents ;
  - lien progressif GPU → debug absent.
- **GREEN minimal** après les seuls documents/configs : exit **0**, **12/12 PASS**,
  **0 FAIL / 0 skipped / 0 cancelled / 0 todo**. Les assertions du RED sont
  inchangées : aucune tolérance, fixture ou référence affaiblie.
- **REFACTOR éditorial** : retour à la ligne et précision sur visibilité du
  persistent mapping CPU→GPU/GPU→CPU ; aucune restructuration moteur.
- **Non-régression finale**, deux relances séquentielles de la même commande :
  chacune exit **0**, **12/12 PASS**, **0 skipped**, 0 cancelled/0 todo.
  Troisième relance après staging : **12/12 PASS**, **0 skipped**, exit **0**.
  `node --check`, `git diff --check` et `git diff --cached --check` : exit **0**,
  sans diagnostic.
- Vérifications complémentaires via Context Mode : cinq skills hors propriété
  identiques au blob `40a7cc3`, bloc permissions du verifier strictement identique,
  liste des changements limitée à l'allowlist. Tests locaux sans mutation de source.
  Staging : **9/9 fichiers autorisés**. Le rapport est ignoré par la règle existante
  `.gitignore:20` ; seul ce chemin explicitement demandé a été ajouté avec `git add -f`.
- Des erreurs de quoting/regex dans des scripts de lecture/filtrage Context Mode
  ont été corrigées avant la commande RED effective. Elles ne sont comptées ni
  comme RED causal, ni comme panne d'infrastructure, ni comme résultat de test.

**Portée** : c'est le contrat éditorial qui passe. Les regex contrôlent présence,
liens et invariants simples ; elles ne prouvent ni validité sémantique exhaustive
du YAML, ni chargement natif, ni enforcement, ni exécution OpenGL. T04 reste
indépendant/non importé ; validation croisée et catalogue après intégration au lead.

## Registre borné attendu / observé / verdict

- G-01, sources/dispositions : **PASS documentaire** (`static-presence`) pour
  liens et limites de réutilisation. Texte original, aucune copie/adaptation tiers.
  Docs V2 agents/skills consultées via `ctx_fetch_and_index`, pas schéma Cursor.
  Khronos registry/refpages/GLFW et RenderDoc référencés ; pas de code récupéré.
  T02 rdc-cli/renderdoc-skill et CLI-Anything : références seulement, SHA d'arbre
  explicitement distinct du commit. Licence inconnue → lien ; commit/notices
  exacts encore requis pour toute future adaptation.
- G-02/G-05 : **PASS local** pour couverture textuelle des procédures/oracles
  OpenGL et frame ; 12 contrôles exécutés, pas une matrice GPU quatre API.
- G-03/G-04 : **PASS local** pour metadata attendue, IDs, liens résolus et règles
  documentées des scénarios 6/9. **INCONCLUSIVE natif** pour sélection effective
  après intégration et les douze scénarios complets. Le chargement explicite du
  skill par son auteur n'est pas une observation indépendante du routage.
- G-06/T-07 : **PASS local** pour présence des exigences de candidat/comptages et
  distinction `executed`/`static-presence`/`inference`. Les hashes ci-dessous
  identifient ce payload ; ils n'authentifient pas des traces natives absentes.
- G-07 : **PASS**, diff T09 borné et cinq skills hors lot préservés. Les canaris
  d'écriture sont **non exécutés / INCONCLUSIVE**, pas déduits de ces checks.
- T-06 : **PASS de non-modification** du bloc de permissions ; enforcement réel
  **INCONCLUSIVE / D-PERM non résolu par T09**, aucune permission élargie.
- API GPU, image/capture, performance : **INCONCLUSIVE**, hors exécution de ce
  lot documentaire : **0 test moteur/API**, **0 capture**, **aucune mesure**.
  Aucun contexte/device/driver obtenu et aucun message driver collecté ici.
  L'absence de messages n'est pas une preuve d'absence d'erreurs.
- Impact CPU/GPU : **non mesuré**, aucun changement au moteur. Aucun gain annoncé.
  Les durées des tests Node ne constituent pas un benchmark de rendu.

Pas de PASS global mission. Revue indépendante et contrôles natifs restent au
lead après shutdown/intégration : `DEFER_TO` pour revue fresh du contenu API/
capture, vérification catalogue/scénarios et décision/enforcement D-PERM.

## Empreintes du payload final (SHA-256, fichiers bruts)

Le rapport est exclu de son propre digest. Empreinte agrégée : SHA-256 de ces
huit lignes triées par chemin, UTF-8, séparateur LF, sans LF final :
`e6c3ea8fb05d893935b673391e02824fd8a9af33ed26f6da2f32fb7d76e1b133`.

```text
ddcb2d4126e0b05c3d3bb2fd7bbf8cfeec09aba94aaf10632699f051bcde3add  .opencode/agents/gpu-engineer.md
ba3cc92423e6260a8e21c4ec57eb1e88b29f60c70115efb2eb91bd31bfbf7bd1  .opencode/agents/rendering-verifier.md
d9b0773d883bf8301bb70654a9de8c39dcf1df0dc93b309e00ed40a5958575ee  .opencode/skills/artist-gpu-debug/SKILL.md
a5ccff0fcf9b5f7a79f261abbfb65f9e033c3925d97881b1ef2a5db09821776f  .opencode/skills/artist-gpu-debug/references/frame-capture.md
bc332da5846113e80ff36423081d83f1cb69e490f6712ab94f38b708fce20ace  .opencode/skills/opengl-backend/SKILL.md
50621902499b69c3aede09b61cf59839beb98d44e75efdc76d6eee88ec790dc1  .opencode/skills/opengl-backend/references/context-lifetime.md
67d3346129e1517bcdcb953b4109cd1ec40e1f4dc085850daccc8203b2525922  .opencode/tests/agentic/t09-opengl-debug/contract.test.mjs
6803e5682fb880c2ba431978944bd1929d3c8b5bdc372edd316d6613e2dfae77  .opencode/tests/agentic/t09-opengl-debug/manifest.json
```
