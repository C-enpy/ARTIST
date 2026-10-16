# T08 — Expertise et skill Metal

Date : 2026-10-16. Auteur : pool-7, tâche `task_mvbbntgj_0032_2ucjul`.
PM/LLM : brut **C64/R48**, retenu **C70/R48** (plancher développement appliqué,
risque distinct). Aucun modèle/variant imposé ni sous-agent utilisé.

## Périmètre, candidat et entrées

Base vérifiée avant écriture : `40a7cc3eac80a4a2d344116365384d48bf92ae48`.
Worktree isolé :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-7`.
Branche : `ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-7`.
Le worktree était propre, initialement detached ; la branche ci-dessus a été
créée localement avant commit, sans changer la base.

T03 lu en premier ; qa-plan, inventaires T02, native-preflight et execution-brief
présents à cette base, lus via Context Mode. AGENTS.md et prescriptions
`docs/agentic/GPU.md`/`QA.md` canoniques lus uniquement, sans transfert de leur
diff utilisateur. Principes/ADR/correction-plan consultés ; aucun statut changé.
Serena `initial_instructions`/configuration : racine canonique ARTIST, aucune
activation ni édition symbolique ; ce lot ne dépend pas de son index C++.
Le pin `PipelineContext` unique_ptr/getPass emprunt a été recoupé aux lignes
42/67/73/96 ; aucune photographie shared_ptr recopiée.

Livrables, tous nouveaux et dans l'allowlist T03 §4 :

- `.opencode/agents/metal-backend-engineer.md` : profil V2 subagent, deny subagent,
  expertise et frontières, chargement sélectif, DEFER_TO au lead seulement.
- `.opencode/skills/metal-backend/SKILL.md` : entrée légère, triggers/exclusions,
  pin/binding times, niveaux de preuve, références conditionnelles T10/T11.
- `.opencode/skills/metal-backend/references/procedures.md` : six procédures et
  quatre scénarios/oracles (nominal, en-vol, famille, ABI), Metal 3/4 distingués.
- `.opencode/skills/metal-backend/references/sources.md` : sources/dispositions.
- `.opencode/tests/agentic/t08-metal/contract.test.mjs` et `manifest.json` : oracle
  explicite G-01..G-07/T-07, sans dépendance au script T04 ou branche voisine.
- Ce rapport, seul artefact de preuve du lot hors répertoire tests.

Aucune source moteur, shader production, build, global, politique, AGENTS,
commande ou fichier voisin modifié. Aucun SDK/driver/package installé, aucune
capture, sonde catalogue/canari/GPU, benchmark, indexation du dépôt ou ressource
Docker créée. Les index de lecture gérés par Context Mode ne sont pas un build.

## Hypothèse et contenu technique vérifié documentairement

Le défaut visé est un manque d'expertise/procédure Metal dans le harness, pas un
bug moteur reproduit. Le moteur reste OpenGL. Rétention CPU metal-cpp, résidence,
visibilité, ordre GPU et attente CPU sont des contrats différents.

Sources Apple officielles consultées HTTP 200 : core API Metal 4, resource
fundamentals/synchronization, retainedReferences, residency sets, triangle Metal 4
et capture. Elles établissent notamment que MTL4CommandBuffer ne retient pas ses
ressources, que hazardTrackingMode est sans effet sur MTL4CommandQueue, et qu'un
allocator ne se reset qu'après complétion de son travail. Recyclage du command
buffer et recyclage de la mémoire de l'allocator ne sont pas confondus.
La procédure drawable Metal 4 wait/commit/signal/present est séparée du present
classique. Les oracles ABI/packing, formats, espaces/depth et sRGB restent explicites.

Disposition : rédaction originale, liens officiels ; aucun code ou skill Apple
copié/adapté. Le tree SHA T02 n'est pas traité comme commit. Le corpus
game-porting-toolkit est référencé uniquement, sans superviseur ; toute adaptation
future exige commit, fichier, licence et notice exacts. Licence inconnue = lien.
Sources V2 agents/skills/permissions consultées ; aucun champ Cursor ni adoption
Slang dans ARTIST. Les URLs et dates sont dans `references/sources.md`.

## Exécution locale et RED/GREEN

Environnement observé : Windows, **Node v24.11.1** lancé par child_process dans
Context Mode (le sandbox Bun n'est pas le runner des tests). Toutes les commandes
ci-dessous utilisent le worktree absolu ci-dessus comme cwd.

Commande test exacte :

```text
node --test .opencode/tests/agentic/t08-metal/contract.test.mjs
```

- **RED statique préalable** : test/manifest écrits avant profil/skill ; exit 1,
  16 découverts/exécutés, 1 PASS, 15 FAIL, 0 skipped/cancelled. Première erreur :
  `profil requis absent : .opencode/agents/metal-backend-engineer.md`.
  Node fonctionnel, pas d'échec SDK. Ce constat prouve l'absence des livrables
  contractuels, pas un RED comportemental GPU ; documents/configs gardent leurs
  contrôles de présence/cohérence, sans fabrication d'un bug moteur.
- Premier contrôle après rédaction : 15 PASS/1 FAIL/0 skipped. La phrase
  « drawable nul » était coupée par un saut de ligne ; correction éditoriale du
  document, aucune assertion/tolérance affaiblie et aucun skip ajouté.
- **GREEN** : exit 0, **16/16 PASS**, 0 FAIL, 0 skipped/cancelled. Quatre mutations
  inertes en mémoire retirent respectivement family gate, toutes queues, packing,
  tracking Metal 4 : les contrôles textuels les rejettent comme attendu.
- **Non-régression finale** : même commande sur le candidat final, mêmes 16/16,
  puis `git diff --check` et `git diff --cached --check` sans erreur. Le contrôle
  cached porte sur les ajouts stagés, contrairement au premier diff vide des
  fichiers encore non suivis. Aucun refactoring de production.

Les tests sont des assertions Node de contenu et de manifest. Ils ne parsèrent
pas tout YAML V2, ne compilent pas MSL et ne démontrent ni chargement natif,
enforcement, routage, ownership GPU réel, image, ni performance. Les quatre
scénarios P6 sont des procédures/oracles livrés, **pas quatre tests Apple exécutés**.

## Empreintes du candidat testé

SHA-256 des six entrées consommées par les tests, relevées le
2026-10-16 à 19:00:49Z ; rapport exclu de son propre hash, aucun test ne le consomme :

```text
b60ccfe102d4005885a7a42190c40ccc6d16a1c9e73b9e7bb86c2c631680bc18  .opencode/agents/metal-backend-engineer.md
64dc030b469a8091de95008dc2798f7a5b989765b9146e080db4ad4711eed851  .opencode/skills/metal-backend/SKILL.md
140d809a77ce7928752c3425c575a326e831365c36c8ab8251f1f27cf191bd2b  .opencode/skills/metal-backend/references/procedures.md
531db2134cc6edb2b1de064f0b8390e5b23a0de262323189dab3298ab614af16  .opencode/skills/metal-backend/references/sources.md
f85f078774b7c714cc9c0367c287d741c9217e708e99cc32878f7dea0e1dcc88  .opencode/tests/agentic/t08-metal/contract.test.mjs
d430fc4e9bc098a8c3af6a6514f01759277c7a84d048955208aa6d0a40021dab  .opencode/tests/agentic/t08-metal/manifest.json
```

## Couverture et limites à transférer

- G-01/G-02/G-05 : présence statique des sources/procédures/oracles Metal contrôlée.
  Pas de preuve globale des 19 responsabilités ni de quatre backends implémentés.
- G-03 : metadata/liens locaux contrôlés ; liens vers les skills futurs T10/T11
  déclarés, existence globale/schema T04/discovery natif **GAP-defer à T13**.
- G-04 : frontières et demande « shader Vulkan vers Metal » documentées ; aucune
  observation de sélection native, ni trois exemples natifs revendiqués.
- G-06 : niveaux/préconditions séparés dans le texte. Compilation Apple, exécution
  GPU, image, capture et performance **INCONCLUSIVE**, aucun essai ni métrique.
- G-07/T-07 : allowlist, base et empreintes contrôlées localement ; intégration,
  candidate final partagé et enforcement/canaris restent au lead. D-PERM non résolu
  par un deny subagent ou par ce rapport. Aucune auto-revue dite indépendante.

Préconditions de preuve Apple : machine/OS/SDK/device compatibles et voie/bail
autorisés. Les outils Apple non résolus sur PATH dans T02 ne prouvent pas leur
absence absolue. L'expertise documentaire n'est pas bloquée par ce constat ; la
validation Apple finale l'est faute de preuve obtenue dans ce lot.

Impact estimé : aucun changement du chemin CPU/GPU moteur, seulement chargement
de texte à la demande et tests Node offline. Aucun gain ni coût CPU/GPU mesuré.
`DEFER_TO lead` : shutdown/intégration, validation croisée T04, découverte/routage
natifs, puis reviewer fresh si une preuve d'API/qualité est demandée. Ne pas
présenter 16 tests texte comme 16 tests Metal ni comme PASS de mission global.
