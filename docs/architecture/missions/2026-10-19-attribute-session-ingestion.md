---
title: "Mission documentaire — ingestion de la session Attribute-GL"
category: architecture
tags: [artist, architecture, documentation, reference]
summary: "Archive de l'ingestion Attribute-GL : rétrospective sourcée et retrouvable, garde-fous proposés seulement ; aucune clôture moteur implicite."
status: completed
implementation: documented
source_revision: 070e20d9157b9d90978e51053a59f843c0e93250
created: 2026-10-19
updated: 2026-10-19
sources:
  - ".opencode/missions/attribute-session-ingestion-preparation.md"
  - ".opencode/missions/team_mve3tz7l_000i_xtayy5/source-ledger.md"
  - ".opencode/missions/team_mve3tz7l_000i_xtayy5/ingestion-result.md"
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Archive — ingestion Attribute-GL

## Résultat et périmètre réellement terminé

L'ingestion documentaire a produit la
[[references/session-2026-10-18-attribute-gl|rétrospective de session Attribute-GL]],
son backlink depuis la note Q3 et le manifeste local d'ingestion.
Le statut « completed » de cette archive concerne **cette ingestion réduite**,
pas l'implémentation ou la clôture générale du chantier moteur Attribute-GL.

L'objectif initial associait ingestion et adoption de garde-fous locaux. Après
demande utilisateur de proportionner le travail, puis « go », la portée a été
réduite à une note sourcée et des **propositions** : aucun agent, skill, permission
ou garde-fou opérationnel n'a été modifié au titre de l'ingestion.
Les deux rappels candidats concernent obligation moteur versus conformité driver,
et choix d'un oracle typé/discriminant lorsque des conversions perdent l'information.

## Historique conservé et confidentialité

Racine : `ses_eb1605bf2ffeAdDg1L6ZxSRQ90` ; borne inclusive
`msg_150091c600010Tw3DVy0Rhrz5E`. Treize sessions historiques accessibles,
850 entrées natives (416 lead + 434 enfants), et non850 messages humains.
Le nouveau PM d'ingestion et la rétrospective elle-même sont exclus.
Quatorze exports acquis ont été recoupés avec tailles/hashes/comptes ; les bruts
privés restent hors Git et hors QMD. Une compaction empêche de promettre un
verbatim historique intégral. Les sources versionnées sont des dérivés assainis.

## Livraison et contrôles exécutés

- `d063ec9` : registre de sources assaini et checklist, avant rédaction.
- `070e20d` : note de session, backlink Q3 et compte rendu documentaire.
- Lead auteur et runner : frontmatter YAML parsé, summary141 caractères,
  4wikilinks résolus,18 IDs de messages vérifiés dans la borne, hash/size du
  registre et comptages du manifeste local recoupés ;16 fichiers dirty protégés
  inchangés, index/log externes non repris.
- `node .opencode/scripts/update-wiki.mjs` : artist-wiki seulement,
  1document ajouté,1misàjour,38inchangés,0retiré/skip.
- `qmd embed -c artist-wiki` :7chunks/2documents. Recherche avec intent puis
  get ont retrouvé la nouvelle note et confirmé ses limites de clôture.

Le manifeste `docs/.manifest.json` est ignoré : mis à jour localement, pas publié
par ajout forcé. Le plan QA reçu a été ramené à une checklist documentaire ;
aucun nouveau worker de rédaction ni nouvel audit indépendant final n'a suivi.
Le compte rendu et les commandes détaillées restent dans les sources ci-dessus.

## Preuves moteur historiques, non relancées

Le retrait Q3 est intégré dansf7bb171. Les artefacts historiques recoupés donnent
326/326PASS (266CPU+60OpenGL),0skip, découverte327→326 ; l'ancien FAIL de conformité
driver reste archivé. Voir la
[[references/session-2026-10-18-attribute-driver-probes|note du retrait des sondes]].
Ce recoupement n'est ni un nouveau run GPU ni un nouvel audit indépendant.

## Obligations non fermées par cette archive

La clôture globale Attribute-GL reste à établir : audit final de toute la mission,
contrat des12formes/captures/refus et photographie technique finale. Ni nouvelle
exécution moteur/GPU, ni image, performance, ASan, Release, autre OS/API ne sont
prouvés par cette ingestion. Les garde-fous restent proposés, non adoptés.

Les [[adr/README|statuts ADR]] ne changent pas :0003/0005 Accepted/partial,
0008 Windows Debug seulement,0001/0002/0004/0006/0007 Proposed.
La mission lifetime, depuis close, est partie de ces limites et du code
courant. Le pointeur suivant est [[architecture/next-mission|le chemin
d'exécution]], pas une reprise d'Attribute-GL.
