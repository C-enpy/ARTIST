---
title: Harness OpenCode d'ARTIST
category: tooling
tags: [artist, tooling, guide]
summary: Utilisation des agents, skills, commandes et MCP OpenCode avec le vault documentaire et la collection QMD artist-wiki.
created: 2026-10-03
updated: 2026-10-10
qmd_collection: artist-wiki
---

# Harness OpenCode d'ARTIST

Navigation : [[index|Accueil]] · [[engineering-principles|Principes]] ·
[[adr/README|ADR]] · [[_meta/wiki-maintenance|Obsidian et QMD]].

## Utilisation

Ouvrir OpenCode à la **racine ARTIST**. L'agent par défaut est `orchestrator`.
Les spécialistes héritent du modèle sélectionné dans la session ; aucun routage
vers un modèle coûteux ou indisponible n'est imposé par ces fichiers.

- `/static-audit [périmètre]` : staticité réelle, frontières et contrat proposé.
- `/engine-review [périmètre]` : revue coordonnée et ciblée, sans modifier les sources.
- `/engine-verify [critères]` : validation indépendante et verdicts gradués.
- `/gpu-diagnose <symptôme>` : enquête GPU et correction proposée, sans patch.

Une demande normale de développement est prise en charge par l'orchestrateur,
avec délégation utile et contrats bornés. Pas de cascade de superviseurs ; les
petites tâches n'ont pas besoin de mobiliser toute l'équipe.

## Organisation

- `AGENTS.md` : cap, style de travail, routage, outils et exigences de preuve.
- `opencode.jsonc` : agent par défaut et MCP propres à ce projet.
- `.opencode/agents/` : un coordinateur et neuf spécialistes.
- `.opencode/skills/` : six procédures activées selon le travail.
- `.opencode/commands/` : quatre points d'entrée explicites.
- `.opencode/evidence/` : preuves locales ignorées, transmission des enquêtes.
- `docs/engineering-principles.md` : synthèse de la philosophie et écarts observés.
- `docs/adr/` : décisions durables lorsque nécessaires.

Les neuf spécialistes sont `engine-architect`, `cpp-engineer`, `gpu-engineer`,
`asset-engineer`, `build-engineer`, `correctness-reviewer`, `shader-reviewer`,
`performance-reviewer` et `rendering-verifier`. Les descriptions précisent quand
les appeler. Les reviewers ne corrigent pas les sources qu'ils évaluent.

Les skills `artist-static-boundary`, `artist-cpp-contracts`, `artist-gpu-debug`,
`artist-performance`, `artist-build-validation` et `artist-asset-cooking` contiennent
les procédures détaillées. Les triggers évitent de charger tout le corpus à chaque
requête. Les invariants indispensables restent dans `AGENTS.md`.

## Adaptation d'AIForAll

Inspirations conservées : coordinateur responsable de l'intégration, spécialistes
feuilles, briefs complets, revue indépendante, distinction intention/réalité,
preuves compactes et verdict INCONCLUSIVE lorsqu'une précondition manque.

Traduction pour ARTIST : contrats C++/GPU et ownership, staticité multi-étapes,
coût des templates, synchronisation, qualité visuelle, build multi-toolchain et
formats cookés. Docker/Kind, Kubernetes, Envoy et les agents métier Rust ne sont
pas les dépendances de travail de ce renderer. Le style est rigoureux, français,
orienté causes et expériences, sans cérémonial pour les tâches simples.

La sélection et les tiers de modèles d'AIForAll ne sont pas recopiés : le modèle
reste choisi par l'utilisateur. Aucun fichier de harness Cursor/Claude/Codex
n'est ajouté. Les outils de recherche restent partagés avec leur configuration
projet explicite.

## MCP et prérequis

### Context Mode local et QMD partagé par projet

Context Mode reste fourni par la configuration globale, sans mutualiser son
état entre clients. Il est requis pour traiter les sorties volumineuses.
QMD est désormais routé vers l'instance 1MCP ARTIST sur le port **3061**,
partagée avec les autres clients de cette racine. Le vault `docs/` reste indexé
dans la collection globale **`artist-wiki`**, sans créer d'index QMD local.
Filtrer les recherches ARTIST sur cette collection et lire les notes récupérées,
en distinguant ADR proposés, décisions adoptées et état du code. Identité portable
dans `.wiki.json` ; procédure dans [[_meta/wiki-maintenance|Maintenance du vault]].
Une autre machine doit disposer de cette configuration globale si elle veut le
même environnement. Les instructions globales restent applicables.

Ne pas recopier l'ancien commentaire d'AIForAll selon lequel Context Mode serait
uniquement un plugin : ici il fonctionne comme MCP. Ne pas réinstaller le plugin
V1, restaurer des hooks ou purger l'index pour configurer ce projet.

### GrepAI — index ARTIST

`grepai` doit être dans PATH. `.grepai/config.yaml` utilise Ollama local et
`nomic-embed-text` (768 dimensions), comme l'environnement de référence. Le
contenu de l'index, les verrous et statistiques sont ignorés ; la configuration
est versionnée. Les tests ne sont pas exclus de la recherche.

Le service GrepAI de 1MCP gère le watcher : premier appel MCP, démarrage ;
**10 minutes après la fin du dernier appel**, arrêt si le lifecycle le possède ;
appel suivant, réveil transparent. L'ouverture et le catalogue n'allument pas
le watcher. Un petit backend `mcp-serve` peut rester présent au repos.
L'autostart Windows a été sorti du dossier Startup ; ne pas le rétablir pour
« corriger » un watcher arrêté au repos.

Diagnostic de cet hôte, sans lancer de watcher :

```powershell
node C:/Users/djden/.config/1mcp/lifecycle/status.mjs artist
```

Le serveur `mcp-serve` expose la recherche ; le watcher maintient l'index.
Le lifecycle utilise les primitives natives et un répertoire de logs dédié.
Il réutilise les watchers manuels détectés sans les adopter ni les arrêter.
Ne pas lancer un second indexeur sur le même GOB, ni supprimer les fichiers PID.
`discover_worktrees: false` conserve un index canonique partagé avec Ensemble ;
les modifications non mergées des worktrees sont explorées directement via
fichiers/symboles, pas via un indexeur supplémentaire.
Le mode de trace `fast` est une aide textuelle, pas une preuve exhaustive pour
templates, macros et appels virtuels. Confirmer avec le code/compilateur/Serena.

### Serena — connexion 1MCP propre à la racine

`opencode.jsonc` pointe vers `http://127.0.0.1:3061/mcp?tags=serena`.
Le plugin global `project-one-mcp` démarre le runtime natif 1MCP au besoin et
maintient ce routage canonique pour les clients et worktrees ARTIST.
Les anciens lanceurs sont conservés comme compatibilité ; ils sélectionnent
désormais le runtime du registre `~/.config/1mcp/lifecycle/projects.json`.
Aucun chemin vers AIForAll n'est utilisé pour cette connexion.

Prérequis locaux : Node dans PATH et installation 1MCP/Serena existante dans le
profil utilisateur et plugin global installé. `.serena/project.yml` déclare C++.
Caches et overrides locaux sont ignorés. Le runtime historique sur **3050**
est arrêté ; ARTIST, AIForAll et retro-eng ont des instances distinctes.
Cette installation locale n'est pas une preuve de portabilité du harness.

Après connexion, appeler `initial_instructions` puis `get_current_config` et
vérifier la racine. Si elle est incorrecte, reconnecter depuis ARTIST : ne jamais
changer le projet d'une instance partagée avec `activate_project`. Un worktree
reste connecté à la racine canonique ARTIST ; ses différences locales doivent
être lues dans ses fichiers plutôt que supposées présentes dans l'index commun.

Les tests locaux du 2026-10-10 prouvent les recherches ARTIST à cinq clients,
l'arrêt idle réel après 602,7 s et le réveil, ainsi que Serena/QMD à deux clients.
Les preuves et le rollback ciblé sont installés sous
`~/.config/1mcp/lifecycle/` et `~/.config/1mcp/docs/mcp-lifecycle.md`.
Ils ne constituent pas une validation du moteur C++/GPU.

Une navigation C++ précise requiert la toolchain/clangd et une base de compilation
cohérente. Voir `artist-build-validation`. L'absence de compilation valide limite
la portée des diagnostics ; le harness ne fabrique pas de base artificielle.

### GPU et autres services

Les outils de capture/profilage sont utilisés via leur CLI/API lorsqu'ils sont
réellement installés. Aucun serveur RenderDoc/Nsight/PIX fictif, ni service cloud
supplémentaire, n'est configuré. Le MCP navigateur ne remplace pas un contexte
OpenGL natif et n'est pas une preuve de rendu pour ARTIST.

Google Ads MCP, Google Ads Write et Playwright sont **désactivés par défaut**
dans la configuration globale et exclus de cette mutualisation. Une activation
ponctuelle dans un autre projet reste explicite ; aucune relance n'est requise
pour vérifier ce harness.

## Vérifier le harness

OpenCode V2 utilise `mcp.servers`, `permissions` ordonnées, `mode`, et le corps
Markdown comme prompt. Les exceptions suivent les règles générales : dernier
match gagnant. `engine-architect` n'a l'édition que sous `docs/adr/` ; les reviewers
ont `edit` interdit ; tous les spécialistes ont `subagent` interdit.
Ces permissions ne constituent pas un sandbox universel pour shell/MCP : leurs
prompts imposent aussi de ne pas modifier les sources par ces autres chemins.

La découverte native charge agents/skills/commandes sous `.opencode/`. Les
instructions permanentes utilisent `AGENTS.md` : le champ `instructions` de la
configuration V2 n'est pas utilisé comme mécanisme de chargement.

Contrôler dans OpenCode les agents/skills/commandes découverts et `/mcps`. Depuis
une CLI V2 disponible dans PATH, `opencode mcp list` contrôle les connexions.
Le Desktop Windows peut embarquer `resources/opencode-cli.exe` même si `opencode`
n'est pas dans PATH. Utiliser la CLI de la même installation pour les diagnostics.

Une modification de configuration doit être effectivement redécouverte par le
client/service ; une liste d'outils déjà figée dans une conversation peut rester
ancienne. Distinguer fichier valide, définition découverte, connexion MCP et
opération utile réussie. Ne pas annoncer l'activation d'un agent sur la seule
existence de son fichier.

Références de format consultées le 2026-10-03 :
[configuration](https://opencode.ai/v2/docs/config),
[agents](https://opencode.ai/v2/docs/agents),
[skills](https://opencode.ai/v2/docs/skills),
[commandes](https://opencode.ai/v2/docs/commands),
[permissions](https://opencode.ai/v2/docs/permissions),
[MCP](https://opencode.ai/v2/docs/mcp-servers).
