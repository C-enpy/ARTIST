---
title: Harness OpenCode d'ARTIST
category: tooling
tags: [artist, tooling, guide]
summary: Missions ARTIST préparées avec le PM, politique Ensemble persistée et approuvée avant exécution, évaluation LLM/JEV, TDD et QA indépendante.
created: 2026-10-03
updated: 2026-10-16
qmd_collection: artist-wiki
---

# Harness OpenCode d'ARTIST

Navigation : [[index|Accueil]] · [[engineering-principles|Principes]] ·
[[adr/README|ADR]] · [[_meta/wiki-maintenance|Obsidian et QMD]].

## Utilisation

Ouvrir OpenCode à la **racine ARTIST**. L'agent par défaut est `orchestrator`.
Les profils natifs ne fixent aucun modèle. Le PM et le travail hors routage
héritent de la sélection de session ; les tâches Ensemble sont routées selon
leur complexité/risque et les modèles/efforts autorisés par l'opérateur.

- `/ensemble-mission <objectif> [evaluation=llm|jev]` : prépare une nouvelle mission
  native avec un brouillon de politique ; aucune exécution n'est encore approuvée.
- `/ensemble-mission config [corrections]` : discute providers, modèles/variantes,
  équivalents ordonnés et critères ; les corrections sont écrites dans le brouillon.
  Relancer `config` sans corrections pour présenter la version résultante.
- `/ensemble-mission start` : approuve la version présentée et libère les admissions.
- `/engine-mission <objectif> [evaluation=llm|jev]` : entrée ARTIST de préparation
  et de plan PM ; sans mission native préparée, indique la commande native à lancer
  puis s'arrête. Ce n'est pas un second mécanisme d'approbation.
- `/engine-tdd <comportement> [evaluation=llm|jev]` : même circuit, centré sur les
  preuves RED/GREEN/REFACTOR d'une fonctionnalité ou correction.
- `/static-audit [périmètre]` : staticité réelle, frontières et contrat proposé.
- `/engine-review [périmètre]` : revue coordonnée et ciblée, sans modifier les sources.
- `/engine-verify [critères]` : validation indépendante et verdicts gradués.
- `/gpu-diagnose <symptôme>` : enquête GPU et correction proposée, sans patch.

Une demande normale de développement est prise en charge par l'orchestrateur,
avec planification PM et contrats bornés. Une nouvelle mission Ensemble passe par
le cycle natif ; un travail local indivisible peut rester hors équipe et appliquer
directement le contrat PM/TDD. Les branches indépendantes sont
recherchées systématiquement, y compris pour les missions de harness, tests et QA.

## Organisation

- `AGENTS.md` : cap, style de travail, routage, outils et exigences de preuve.
- `opencode.jsonc` : agent par défaut et MCP propres à ce projet.
- `.opencode/ensemble.json` : auto-dispatch et auto-scaling des tâches admissibles.
- `.opencode/agents/` : un coordinateur, un PM et neuf spécialistes.
- `.opencode/skills/` : six procédures activées selon le travail.
- `.opencode/commands/` : six points d'entrée explicites.
- `.opencode/evidence/` : preuves locales ignorées, transmission des enquêtes.
- `docs/engineering-principles.md` : synthèse de la philosophie et écarts observés.
- `docs/adr/` : décisions durables lorsque nécessaires.

Les neuf spécialistes sont `engine-architect`, `cpp-engineer`, `gpu-engineer`,
`asset-engineer`, `build-engineer`, `correctness-reviewer`, `shader-reviewer`,
`performance-reviewer` et `rendering-verifier`. Les descriptions précisent quand
les appeler. Les reviewers ne corrigent pas les sources qu'ils évaluent.
`project-manager` prépare le graphe et les évaluations en lecture seule ;
`orchestrator` valide, publie, coordonne et intègre.

Les skills `artist-static-boundary`, `artist-cpp-contracts`, `artist-gpu-debug`,
`artist-performance`, `artist-build-validation` et `artist-asset-cooking` contiennent
les procédures détaillées. Les triggers évitent de charger tout le corpus à chaque
requête. Les invariants indispensables restent dans `AGENTS.md`.

## Missions, évaluation et TDD

0. **Préparer et configurer** : utiliser le cycle natif `/ensemble-mission`. La
   politique effective est matérialisée dans
   `.opencode/missions/<team-id>/model-policy.json`, à partir du défaut explicite
   `missionPolicy` hérité ou, à défaut, du catalogue global. Ne pas recopier une
   table de modèles dans la configuration ARTIST. Le brouillon doit être présenté
   et approuvé ; ce n'est pas une validation automatique de l'adéquation des modèles.
1. **Planifier** : le PM transforme l'objectif en fiches concises et exécutables,
   avec livrable, entrées, ownership, profil, critères/tests, dépendances, ressources
   exclusives et contexte motivé. Les phases TDD d'une même tranche restent ordonnées.
2. **Évaluer** : PM/LLM par défaut, JEV direct sur demande explicite. Conserver les
   scores bruts et la justification, puis appliquer `complexity = max(70, estimation)`
   à tout développement, y compris code de test, shader, script et harness exécutable.
   Appliquer les critères persistés de la mission ; le risque est distinct. Pour
   JEV, convertir une fois les scores natifs [0,1] en
   entiers 0–100 (`ceil(100 * score)`) avant le plancher ; la confiance n'est pas le
   risque. Une évaluation indisponible bloque, sans changement silencieux de méthode.
3. **Publier et router** : l'orchestrateur valide et transmet les deux scores à
   `team_tasks_add`. Cette évaluation préalable contourne volontairement le
   classificateur automatique à barème fixe : l'état devient `not_required` sans
   appel helper ni notification de fin de classification. Pour une mission native,
   la grille complexité × risque approuvée choisit le modèle/variant exact, sous les
   restrictions opérateur. Une demande de modèle différent passe par une révision
   présentée/approuvée, pas par un override de spawn. Les profils n'imposent pas de
   modèle. Les équipes legacy gardent leur routage catalogue/préférences existant.
4. **Paralléliser** : `.opencode/ensemble.json` active `autoDispatch` et `autoScale`.
   Le verrou du plugin bloque les admissions des missions natives avant `start` :
   publier des tâches ne vaut pas approbation. Après approbation, les branches
   prêtes peuvent partir immédiatement. Les équipes legacy sans ce verrou gardent
   leur comportement ; ne pas les recréer ni les convertir implicitement.
   Seules les tâches évaluées, aux dépendances satisfaites et au contexte décidé,
   sont publiées pour exécution. Exiger les contextes avant publication avec
   `team_tasks_add({tasks: [], require_context: true})`. Utiliser `fresh` par défaut,
   notamment pour la QA indépendante, et respecter la capacité configurée.
   Séparer les worktrees des auteurs ; sérialiser les mutations, builds/indexeurs
   du même arbre et tests partageant le GPU ou des fixtures.
5. **Intégrer et vérifier** : le coordinateur organise une barrière d'intégration
   avant les consommateurs d'un worktree. Une fin de tâche auteur ne rend pas son
   diff disponible ailleurs : garder ces consommateurs dans le plan, non publiés,
   jusqu'à l'intégration vérifiée, puis les publier avec les IDs des producteurs
   et l'état intégré. Cela évite une course avec l'auto-scaling.
   Les changements sensibles reçoivent une revue ou vérification indépendante.

Le contrat détaillé et le barème sont dans `.opencode/agents/project-manager.md` ;
la politique durable est dans `AGENTS.md`. Les tâches portent les IDs de dépendance
réellement retournés par Ensemble, pas les références locales du plan PM.
Le contexte `pending` est réservé à une décision de contexte encore ouverte.
Aucune tâche de développement n'échappe au plancher en
changeant son intitulé en « QA » ou « outillage ».

Le développement suit **RED → GREEN → REFACTOR** : constater le test qui échoue
pour la bonne raison, implémenter le contrat, puis restructurer avec non-régression.
Un manque de toolchain ou de contexte GPU est INCONCLUSIVE. Refactorings purs et
changements documentaires utilisent leurs contrôles adaptés. Les preuves gardent
commandes, environnement, attendu/observé et nombres de tests ; les exigences GPU,
image et performance restent celles de [[development/build-and-test|la validation]].

Exemples : `/ensemble-mission sécuriser l'assignation move Shader/Pass`,
`/engine-mission plan seul pour la prochaine tranche Pipeline evaluation=jev`,
`/engine-tdd rejeter un re-load sans mutation de l'état OpenGL`.
Pour une coordination explicite : `@opencode-ensemble <mission>`.

### Approbation, révisions et pauses

Les commandes ARTIST restent des prompts de préparation. Elles ne créent pas une
équipe legacy via `team_create` pour éviter le cycle natif. Un plan seul ne lance
aucun worker d'exécution et ne modifie aucune source. Le PM reste en lecture seule ;
l'orchestrateur persiste les corrections dans le brouillon puis demande sa nouvelle
présentation avec `config`. Ni un GO en prose, ni `/engine-mission start`, ni une
approbation de plan de teammate, ni une propriété JSON ne remplacent le `start`
natif de l'utilisateur. Le coordinateur ne l'invoque pas à sa place.

Les groupes sont des équivalences acceptables pour cette mission, avec principal
puis alternatives ordonnées, pas une affirmation de performances universelles.
Les alternatives ne sont pas des retries automatiques. Une nouvelle configuration
pause les admissions ; les tâches déjà admises conservent leur révision, et les
changements de critères font réévaluer les scores automatiques non attribués.
Le worker suivant est routé à nouveau au départ ; un auto-claim en cours de tour
n'est pas une frontière fiable de changement de modèle. Utiliser spawn du lead ou
dispatch idle pour les missions natives.

Le GO de reprise d'une pause opérateur reste distinct de l'approbation de grille.
Un superviseur dont les requêtes sont bloquées ne peut pas lever lui-même ce verrou
par un message qu'il ne peut pas lire : faire lever uniquement la pause de l'équipe
concernée par un opérateur autorisé, puis reprendre son chat. Ne pas modifier les
autres missions ni élargir les permissions d'un spécialiste.

Le SDK permet l'invocation programmatique des commandes : ce verrou impose une
route d'approbation explicite, sans garantir l'identité humaine de l'appelant.

Ensemble et son skill opérationnel sont fournis par l'installation globale
existante. La configuration projet hérite du catalogue de modèles, de sa politique
d'autorisation et du budget de workers ; elle ne les remplace pas.
La présence des outils et fichiers ne prouve pas l'exécution complète d'une mission.

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

Vérification native du **2026-10-16** : le plugin global Ensemble
`20261016-mission-policy-5504fe8e8df8` est actif dans ARTIST ; `/ensemble-mission`,
les commandes ARTIST actualisées, les onze profils et une seule copie du skill
`opencode-ensemble` sont découverts. Le cache ARTIST seul a été rechargé, sans
redémarrage global ni prompt de mission. Modèles, modes et permissions des profils
sont conservés. Les preuves locales sont dans `.opencode/evidence/` ; cette
validation de découverte n'est ni un test d'exécution d'une mission, ni une
validation C++/GPU.

Références de format consultées le 2026-10-03 :
[configuration](https://opencode.ai/v2/docs/config),
[agents](https://opencode.ai/v2/docs/agents),
[skills](https://opencode.ai/v2/docs/skills),
[commandes](https://opencode.ai/v2/docs/commands),
[permissions](https://opencode.ai/v2/docs/permissions),
[MCP](https://opencode.ai/v2/docs/mcp-servers).
