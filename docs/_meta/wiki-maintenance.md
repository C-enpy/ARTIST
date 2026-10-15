---
title: Maintenance Obsidian et QMD
category: metadata
tags: [artist, documentation, tooling, guide]
aliases: [QMD ARTIST, Collection artist-wiki]
summary: Identité du vault, schéma des notes, provenance, recherche et indexation ciblée de la collection artist-wiki.
created: 2026-10-03
updated: 2026-10-15
qmd_collection: artist-wiki
---

# Maintenance du vault

## Identité à retrouver en premier

- Projet : **ARTIST**.
- Vault : **`docs/`**, relatif à la racine du dépôt.
- Accueil : [[index]].
- Collection QMD : **`artist-wiki`**, masque **`**/*.md`** à partir de `docs/`.
- Index : **`index` global**, le même que le MCP QMD déjà configuré.
- Configuration portable : **`.wiki.json`** à la racine ; rappel dans **`AGENTS.md`**.
- Variables locales : `.env` (ignoré), modèle `.env.example` ;
  `QMD_WIKI_COLLECTION=artist-wiki`, chemins absolus `OBSIDIAN_VAULT_PATH` et
  `OBSIDIAN_SOURCES_DIR` propres au checkout.

Ouvrir `docs/` comme vault Obsidian existant. Les plugins communautaires ne sont
pas nécessaires : propriétés, wikilinks, backlinks, callouts et graphe sont natifs.
Les fichiers d'état UI sous `.obsidian/` sont ignorés, sauf la configuration minimale.

## Trois niveaux de connaissance

1. **Sources** : code/révision et quatre rapports bruts sous `research/`.
   `_meta/research-sources.json` conserve leurs tailles/hashes. Ne pas éditer ces
   originaux ; une nouvelle version est une nouvelle source.
   Les sessions sont distillées dans `references/` avec leurs identifiants et
   limites de disponibilité ; ne pas présenter un checkpoint comme un export
   verbatim. Exemple : [[references/session-2026-10-03-shader-pass]].
2. **Documentation** : notes réconciliées avec liens internes et provenance,
   propositions d'architecture distinguées des observations.
3. **Schéma** : ce guide, [[_meta/taxonomy|taxonomie]], [[adr/template|modèle d'ADR]]
   et instructions de projet. `.manifest.json`, ignoré car il contient les chemins
   absolus locaux, suit l'ingestion des rapports pour les outils wiki.

Les chemins de code cités sont relatifs à la racine du dépôt. Une source rédigée
par un agent ne devient pas une preuve d'exécution. Marquer explicitement les
hypothèses et propositions ; ne pas inventer des fractions de confiance.

Les mémoires Serena du projet ARTIST servent de points de reprise compacts,
avec liens vers ces notes ; elles ne remplacent pas les ADR ni leurs preuves.
Confirmer le projet actif et ne pas réaffecter une instance 1MCP partagée.
Depuis la préparation de publication du 2026-10-15, les trois mémoires Markdown
sous `.serena/memories/` sont versionnées ; les caches et autres états locaux
Serena restent ignorés. Ne pas y mettre de secrets ou d'identifiants d'accès.
Un incident et sa correction d'interprétation sont conservés ensemble : voir
[[development/known-incidents|le registre d'incidents]].

## Schéma des notes

Chaque note a un frontmatter avec `title`, `category`, `tags`, `summary`, `created`
et `updated`. Ajouter `sources` ou une section de preuves, `source_revision` pour
une photographie source, et des `aliases` seulement utiles. Les tags proviennent
de [[_meta/taxonomy]], au plus cinq. Les dates suivent `YYYY-MM-DD`.

Les liens internes sont des wikilinks avec chemin depuis le vault, par exemple
`[[adr/README|Registre des ADR]]`. Garder les liens Markdown pour les références
externes. Conserver les chemins existants cités par le harness.

Les ADR ajoutent `adr_id`, `status`, `implementation` et `decision_origin` selon
[[adr/README|le registre]]. Distinguer ces deux axes de statut lors des recherches.
Les notes racine du dépôt restent de courts points d'entrée vers ce vault pour
éviter deux documentations complètes contradictoires.

## Chercher avec QMD

Après découverte de la signature MCP `query`, utiliser une requête structurée :

```json
{
  "searches": [
    {"type": "lex", "query": "ownership lifetime ADR-0003"},
    {"type": "vec", "query": "propriété des contexts et destruction des ressources GPU"}
  ],
  "intent": "Retrouver le contrat proposé pour corriger ARTIST, pas une preuve de son implémentation ni les conventions Rust d'un autre projet.",
  "collections": ["artist-wiki"],
  "limit": 5,
  "rerank": false
}
```

Récupérer les documents trouvés avec `get`/`multi_get`, puis citer les sections
et chemins. Les snippets servent à choisir une note, pas à conclure seuls.
Pour un identifiant exact :

```powershell
qmd search "ADR-0003" -c artist-wiki -n 5
qmd get qmd://artist-wiki/adr/0003-ownership-et-lifetime-cpu-gpu.md
```

## Créer la collection sur une autre machine

Depuis la racine, avec QMD installé et le même index global choisi :

```powershell
qmd collection add ./docs --name artist-wiki --mask "**/*.md"
qmd context add qmd://artist-wiki "Documentation ARTIST C++23/OpenGL : état observé, principes, ADR proposés et procédures. Vérifier les statuts et les preuves."
qmd embed -c artist-wiki
```

Vérifier d'abord `qmd collection show artist-wiki` : si ce nom existe déjà,
contrôler son chemin au lieu de le remplacer. Un worktree différent ne doit pas
réaffecter silencieusement la collection du checkout principal.

## Après une modification

1. Mettre à jour les notes liées, les dates et [[log|le journal]], puis vérifier
   frontmatter, tags, wikilinks et sources. Un renommage exige la mise à jour des liens.
2. Réindexer uniquement ARTIST, puis actualiser ses embeddings :

```powershell
node .opencode/scripts/update-wiki.mjs
qmd embed -c artist-wiki
```

Le script utilise le SDK QMD 2.2 avec `update({ collections: ["artist-wiki"] })`,
vérifie le chemin enregistré et ne modifie pas la configuration des collections.
Il charge le package QMD local/global Windows existant ; `QMD_PACKAGE_ROOT` peut
désigner une autre installation du package. Il ferme sa connexion après l'indexation.

La CLI QMD 2.2 inspectée ne filtre pas `qmd update` par `-c` : ce dernier ne doit
pas être présenté comme une mise à jour ciblée. Le script évite de déclencher
les hooks de mise à jour et l'indexation des autres projets.

3. Vérifier `qmd collection show artist-wiki`, `qmd ls artist-wiki`, une recherche
   lexicale et, après embedding, une recherche sémantique. Ne pas lancer les
   mutations QMD en parallèle sur le même index.

Ne pas créer un `.qmd/` local par réflexe : il changerait l'index utilisé par la
CLI et pourrait diverger du MCP global. Le nom versionné ne remplace pas
l'enregistrement réel de la collection sur la machine.

## Validation initiale — 2026-10-03

- **PASS** : 21 notes indexées ; recherche lexicale via le MCP, lecture des notes
  et mise à jour limitée à `artist-wiki`. Métadonnées, tags, 127 wikilinks et
  empreintes des quatre rapports contrôlés.
- **PASS en CPU** : embeddings des 21 notes, puis requête `vec` seule via le SDK
  QMD, avec `collections: ["artist-wiki"]`, intention explicite et reranking
  désactivé. Les ADR-0001/0006 et le registre sont retrouvés ; aucun document de
  la collection ne reste à vectoriser à l'issue de cette validation.

## Configuration QMD courante — 2026-10-03

Utiliser la configuration QMD standard de la console et la commande
`qmd embed -c artist-wiki`. L'utilisateur confirme que CUDA fonctionne dans sa
console. L'erreur d'allocation observée pendant le bootstrap est un événement
ponctuel, pas une restriction permanente ni une consigne de forçage CPU.

**PASS avec la configuration standard** : après le build Windows, la commande
a vectorisé 19 fragments de 9 documents modifiés en 11,5 secondes (code retour 0).
La recherche `vec` via le MCP QMD retrouve ADR-0008 et le guide de build dans
`artist-wiki`. Le vault contient désormais 22 notes. Preuve locale :
`out/logs/qmd-final-embed.log`.

## Ingestion de la session Shader→Pass — 2026-10-03

[[references/session-2026-10-03-shader-pass|Synthèse attribuée]] et
[[development/known-incidents|registre d'incidents]] ajoutés, photographie et
ADR réconciliés : **24 notes** dans ce vault. Trois mémoires de reprise écrites
et relues via Serena ARTIST : `project/artist-reprise`,
`architecture/shader-pass-adr0003`, `engineering/incidents-2026-10-03`.

Après les échecs d'allocation historiques, **PASS standard** pendant ingestion :
`qmd embed -c artist-wiki`, 38 chunks de 10 documents en 6 s (code 0), puis
`vsearch` ciblé retrouve ADR-0003 et recherche lexicale/`get` la synthèse.
Log : `out/logs/qmd-session-ingest-embed.log`. Pas de configuration CPU forcée,
pas de modification des autres collections. Les mises à jour documentaires
ultérieures exigent encore leur propre cycle update/embed.
