# T07 — expertise D3D12, résultat borné

Date : 2026-10-16. Auteur : pool-6 / gpu-engineer, tâche
`task_mvbbntgj_0031_beqf5y`. PM/LLM : complexité brute **61**, retenue **70** ;
risque brut/retenu **48**. Pas de modèle/variant fixé ni sous-délégation.

## Verdict et périmètre

**PASS local, statique seulement** : profil V2 + skill progressif documentaires,
manifest explicite, 15 contrats Node réussis, zéro skip. **INCONCLUSIVE** pour
chargement/sélection native vérifiés, enforcement, D3D12 device/GPU/image/capture/
performance et validation globale T04/T13 : non exécutés dans ce lot.

Aucun bug moteur reproduit. Hypothèse de travail : le manque à combler est une
expertise API distincte, pas un backend à ajouter. Preuves : contrat T03 §3/§5,
audit T02 (DirectXTex/DirectXMesh = cook ; DXC 1.8 ≠ device/frame), et absence
initiale des nouveaux artefacts détectée par les tests de contrat. ARTIST demeure
OpenGL. Aucun shader, source moteur, build, configuration globale ou ADR modifié.

## Candidat, entrées et isolation

HEAD de départ vérifié : `40a7cc3eac80a4a2d344116365384d48bf92ae48`.
Worktree isolé propre au départ :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-6`.
Git l'annonce **detached** ; le nom de branche fourni par la mission est
`ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-6`.
Le commit exact de livraison est transmis au lead, sans inventer une ref Git.

Le shell initial pointait la racine canonique ; aucune écriture n'y a été faite.
Le worktree a été identifié par `git worktree list --porcelain`, puis la session
y a été déplacée avant la première édition. Context Mode brièvement absent du
catalogue pendant ce déplacement est revenu ; aucun contournement hors MCP.
Une erreur d'échappement du premier script de lecture a été corrigée dans Context
Mode ; ce n'est ni un échec de test ni une preuve RED.

Entrées lues : T03 en premier, qa-plan, T02 source-inventory/external-sources,
native-preflight, execution-brief ; AGENTS et prescriptions GPU.md/QA.md canoniques
en lecture seule. Principes d'ingénierie, index ADR/correction-plan consultés.
Serena initial_instructions/get_current_config : racine canonique ARTIST conservée,
aucun activate_project ni édition symbolique partagée. Pas d'analyse C++/build
revendiquée. Skills chargés : artist-gpu-debug, artist-build-validation, opencode,
create-skill, opencode-ensemble et serena. Format V2 officiel recoupé ; champs
Cursor du guide générique non repris.

## Patch, ownership et oracles

Sept fichiers seulement, tous nouveaux dans l'allowlist T03 §4 :

- `.opencode/agents/d3d12-backend-engineer.md` : subagent, pas de modèle, deny
  subagent ; responsabilités API et DEFER_TO via lead, sans superviseur ajouté.
- `.opencode/skills/d3d12-backend/SKILL.md` : triggers/exclusions, pin réel
  unique_ptr/emprunt, compile/cook/runtime, préconditions et niveaux de preuve.
- `.opencode/skills/d3d12-backend/references/diagnostic.md` : root signatures,
  heaps, PSO/ABI, allocators/lists, queues, states/enhanced gate, fences et résidence ;
  un nominal D12-N1 et cinq contre-exemples D12-X1..X5 conditionnels.
- `.opencode/skills/d3d12-backend/references/sources.md` : références officielles,
  provenance et dispositions sans copie/adaptation tiers.
- `.opencode/tests/agentic/t07-d3d12/contract.test.mjs` et `manifest.json` : oracle
  fixé à partir du plan/contrat avant les documents, sans import T04/lot voisin.
- Le présent `t07-result.md`, seul artefact evidence écrit (ajout ciblé nécessaire
  car ce répertoire est ignoré).

Mapping soumis au lead : gpu-resource-engineer conserve owner/borrow, générations,
ledger RAW/WAR/WAW et retraite toutes queues ; D3D12 traduit states/subresources,
scopes/access/layouts enhanced conditionnels, Signal/Wait, fences, allocator et
descriptors. Le scénario 3 garde le principal ressources + les deux skills,
DDS seul reste assets. Aucun stub d'un futur ID n'est créé.

D12-N1 décrit Windows/device identifié, VS/PS SM6.0 supporté, triangle 64 × 64,
frame 0 et plusieurs réemplois de slots ; RT linéaire R8G8B8A8_UNORM, centre rouge
opaque exact, clear noir extérieur, lecture après fence. GPU/driver/adapter et
shaders hashés sont des champs à relever, **pas des observations de ce lot**.
Le nominal n'est pas matérialisé/exécuté ; aucun contexte OpenGL ni device D3D12
n'a été ouvert. D12-X1/X4 refusent côté CPU le réemploi en vol/périmé plutôt que
de provoquer de l'UB GPU. X2 state et X3 binding sont distincts d'une attente ;
X5 sépare feature interrogée, fallback/refus et preuve matérielle.

## Commandes et résultats réels

Commandes lancées depuis le worktree via `ctx_execute` puis `spawnSync` de **Node
v24.11.1**, Windows. Le runtime Bun du sandbox n'est pas présenté comme Node.

```text
node --version
node --test .opencode/tests/agentic/t07-d3d12/contract.test.mjs
git diff --check
git status --short --untracked-files=all
```

1. **RED de contrat statique**, 18:55:49Z : manifest + tests ajoutés avant profil/
   skill ; 15 découverts/exécutés, **3 PASS / 12 FAIL / 0 skipped**, exit 1.
   Motifs attendus : `Artefact contractuel non livré : .opencode/agents/d3d12-backend-engineer.md`
   et fichiers skill absents. Manifest et deux gardes négatifs en mémoire passent.
   Ce RED ne représente ni défaut moteur ni panne d'environnement ; les documents
   ne nécessitent pas de faux test comportemental GPU.
2. Contrôle après rédaction, 18:59:29Z : **14 PASS / 1 FAIL / 0 skipped**, exit 1 ;
   `Clause absente : estimé`. La phrase de retour a été rendue explicite « estimé
   ou mesuré » ; aucune assertion/tolérance/référence diminuée. Distinction
   allocator/list complétée pour les références enregistrées non encore soumises.
3. **GREEN**, 19:00:25Z : **15 PASS / 0 FAIL / 0 skipped**, exit 0.
   Aucun skip conditionnel. `git diff --check` exit 0 à cette étape ; les nouveaux
   fichiers étaient encore non suivis, donc ce premier diff check n'est pas à lui
   seul le contrôle du patch final indexé.
4. **Non-régression**, 19:02:58Z, payload inchangé et rapport ajouté : **15 PASS /
   0 FAIL / 0 skipped**, exit 0. `git diff --check` et `git diff --cached --check`
   exit 0. Comparaison de `git diff --cached --name-only` à l'allowlist exacte :
   **7/7**, aucun fichier non indexé modifié. Seul ce compte-rendu est ensuite
   enrichi des résultats ; les six empreintes testées ci-dessous restent valides.

Les deux négatifs retirent en mémoire chacune des 10 clauses lifetime et des
10 clauses enhanced, et vérifient un diagnostic exact. Ils prouvent la sensibilité
du garde textuel, **pas** la validité sémantique de toute phrase, un rejet driver,
le routage natif ou un confinement. Le test n'exécute aucun fixture/outil externe,
ne parcourt pas les branches voisines et ne vérifie pas l'existence des futurs
profils T11. Validation croisée T04 puis découverte/routage : à faire après merge.

## Empreintes du payload au GREEN

SHA256 des bytes locaux, HEAD de départ + delta ; rapport exclu de son propre
hash pour éviter une autoréférence. Un changement de payload invalide ce run.

```text
e0fe02e846d2becf8ea0ed9dc11f08e0fd001d76c0e4eef28d9f2a94dce855d7  .opencode/agents/d3d12-backend-engineer.md
edc77b9d81fe97af7c87ab80b7d4ccc4f25145260f78a81bb8fefabd6fbe0955  .opencode/skills/d3d12-backend/SKILL.md
4c4d68aceee7adcbeb13ffffefd9681aecbfd329782472d6c8c8ef6be5bca7e4  .opencode/skills/d3d12-backend/references/diagnostic.md
8dda2b259d50b89937a80b812cdb8b80c2a141c6f6119be7091a0921610080d9  .opencode/skills/d3d12-backend/references/sources.md
f5fe5855a1d9f6f6f7cd7bd9df9c6364771edaa827180d950987b7a346cac9bb  .opencode/tests/agentic/t07-d3d12/contract.test.mjs
491dcdacb86e51e13108613401a7b8a75b028247c216cc778bda9c49056a0955  .opencode/tests/agentic/t07-d3d12/manifest.json
```

Contrôle canonique RO à 19:01:10Z contre le brief : **8/9 empreintes concordent**.
Seule `.grepai/config.yaml` diverge : actuel
`d58ae9d1ef3bb0d277b96862ca94ee646f74a08ae347672c6aaf7aa741b207a9`,
brief `d9d23a9d7a077d2692c48ce5bc9055b93afda93ec2aa85f1edd5e9c43966fd87`.
Aucune écriture/édition de cette configuration par T07, origine non attribuée ;
lead informé, aucun reset/rétablissement/copier-coller de diff utilisateur.

## Sources, dispositions et limites

Douze pages Microsoft Learn listées dans `references/sources.md` récupérées en
HTTP 200 le 2026-10-16 via Context Mode, ainsi que agents/skills/permissions V2.
Contrats particulièrement recoupés : Reset allocator interdit en vol, Reset list
possible pendant une exécution précédente, queue Wait ≠ attente CPU,
OPTIONS12.EnhancedBarriersSupported optionnel, debug layer avant device.

Tout le texte livré est original ; les six dépôts D3D12 du corpus T02 sont
**référencés seulement**. Licence dépôt/date reprises en tant qu'observations de
T02 ; ni arbre SHA présenté comme commit, ni licence inconnue présentée comme
autorisation de copie. Aucun sample/import/download exécutable ni dépendance.

Couverture locale : G-01 sources/dispositions ; G-02 procédures/exclusions ;
G-03 format/liens statiques ; G-04 scénario 3/DDS textuels ; G-05/G-06 limites et
oracles conditionnels ; G-07 allowlist/diff ; T-07 identité de ce payload.
Ces résultats ne ferment pas globalement les obligations G-/T- de la mission.

Natif/enforcement/revue indépendante : non réalisés, à demander au lead sous
bail après intégration. Pas de sondes catalogue, canaris, capture, SDK/driver,
benchmark, indexeur ou Docker créés. Pas de claim PIX automatisé. Aucune attente
globale ajoutée. Impact CPU/GPU moteur **estimé nul** car aucun code moteur ou
shader modifié ; **non mesuré**, aucun gain annoncé. Le temps des tests Node est
un coût de validation documentaire, pas une mesure de frame ni de performance.
