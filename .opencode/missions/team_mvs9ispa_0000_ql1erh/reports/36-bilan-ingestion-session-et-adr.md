# Session Linux et sous-agents — ingestion documentaire vérifiée

## Résultat

La mémoire réutilisable de la mission Linux QA est intégrée à ARTIST : décisions
utilisateur, contributions attribuées, corrections issues des revues, publication
et limites. Une note créée, douze pages existantes actualisées ; pas de nouveau
moteur, API, permission, politique modèle ou campagne de tests.

- Synthèse : `docs/references/session-2026-10-29-linux-qa.md`.
- Preuves complétées : ADR-0004/0005/0008 et registre. Aucun changement de statut,
  d'implementation ou d'origine d'adoption dans les huit ADR.
- Intégration : principes, photographie, plan de correction, procédure Linux,
  incidents, harnais OpenCode, index et journal.

## Sources et attribution

API native OpenCode : principal et quatorze enfants directs, quinze transcriptions
publiques filtrées, 250 messages, 236188 octets. Pagination jusqu'à épuisement,
borne exclusive avant la demande d'ingestion ; SHA256 par fichier dans
`research/sessions/2026-10-29-linux-qa-session/index.json`.

Les briefs internes et messages user synthétiques, outils, raisonnement privé et
pièces jointes sont exclus. Le texte assistant et les messages humains du principal
restent des sources attribuées, non des preuves d'exécution par eux-mêmes. Certaines
remises d'équipe sont couvertes par les rapports persistés, pas par les outils
exclus de l'export. Aucun inventaire récursif ou backup exhaustif revendiqué.

Vingt-deux nouvelles sources enregistrées dans le manifeste local ignoré, dont
six dossiers de preuves/bilans. Les onze exports float préparé sont inchangés ;
pas de nouvelle ingestion de leur contenu. Statistiques locales réconciliées à
398 sources et 61 pages, sans recopier d'autres vaults ou configuration globale.

## Vérifications réellement exécutées

Runner : lead. Contrôle documentaire ponctuel sous `.tmp/` : syntaxe de l'export,
compteurs et borne des messages, absence de briefs dans les exports enfants,
SHA256/tailles et pagination, correspondance manifeste↔sources/pages, YAML/JSON
sans doublon, tags canoniques, summaries bornées, fractions de provenance,
204 wikilinks de fichiers, journal append-only et statuts ADR inchangés.
**664 assertions documentaires passent** ; ce ne sont pas 664 tests moteur.
La configuration GrepAI et les deux anciens temporaires protégés ont leurs
empreintes initiales. Scan des motifs et de la clé locale connue sans l'imprimer ;
pas de garantie exhaustive de détection de secrets.

Commandes :
- `node --check .tmp/export-linux-qa-session.cjs`, puis export API filtré.
- `C:/Python313/python.exe -B .tmp/check-linux-qa-ingestion.py` (PyYAML 6.0.2).
- `git diff --check` : propre sur les changements suivis.
- `node .opencode/scripts/update-wiki.mjs` : une collection seulement,
  un document ajouté, douze modifiés, quarante-huit inchangés, zéro skip/retrait.
- `qmd embed -c artist-wiki` : 118 fragments de treize documents, code zéro.
- MCP QMD : recherche ciblée puis lecture de la synthèse et d'ADR-0005/0008
  réussies dans `artist-wiki`.

Contrôle de l'index Git : 31 chemins explicitement sélectionnés, les quinze
hashes de transcriptions restent identiques dans les blobs Git. Le diffcheck
indexé complet signale les espaces finaux des transcriptions publiques ; ils
sont préservés avec leurs empreintes, pas « corrigés » dans les archives. Le
diffcheck indexé hors de ce nouveau dossier d'archives est propre. Ce résultat
partiel de formatage ne devient pas un PASS global de formatage.

Preuves locales : `out/logs/wiki-linux-qa-ingest/document-checks.json`,
`qmd-update.log` et `qmd-embed.log`. Les scripts ponctuels restent sous `.tmp/`,
hors publication. Aucun test moteur, contrôleur Python métier ou harnais agentic
relancé pour cette ingestion ; leurs anciens résultats restent attachés aux
candidats, runners et revues documentés.

## Publication et limites

Publication documentaire sur main dans la continuité du mandat utilisateur de
conserver le contexte, par sélection explicite hors `.tmp/` et configuration
active, fetch puis push normal sans force. Le commit documentaire est marqué
skip-ci : aucune nouvelle qualification GPU n'est revendiquée. Le contrôle final
des références Git et des blobs publiés est conservé séparément sous `out/`.

Linux reste qualifié sur llvmpipe, sans détecteur de fuites ; matériel Linux,
Release, UBSan, coverage et performance non validés. RED classification historique
incomplet, cause précise d'incrémentalité Windows et fallback 3.3 ambigu restent
ouverts. Ni archive publique, ni test textuel des profils, ni fermeture de revue
ne démontrent une sandbox ou un enforcement natif.
