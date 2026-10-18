# F7 — archives publiques de session et preuves, 2026-10-18

Synthèse Obsidian : `docs/references/session-2026-10-18-f7.md`.
Résultat technique : `docs/architecture/missions/2026-10-18-f7-uniform-attribute.md`.

## Contenu archivé

- `sessions-manifest.json` : coordinateur et28sous-agents,29exports gzipJSON,
  1965entrées au total, IDs/modèles/outcomes, compteurs et empreintes. Pagination
  native complète ; les28enfants ont été vérifiés sans descendant supplémentaire.
- `sessions/*.json.gz` : messages publics et traces d'outils de chaque session.
  Format dérivé de l'export OpenCodeV2 `experimental.session.export`, CLI2.0.32.
  Raisonnements internes, instructions système/skill/synthetic/compaction et
  snapshots internes ne sont pas publiés ; marqueurs d'omission conservés.
  Les champs/tokens d'authentification détectés sont masqués. Les compteurs de
  tokens numériques ne sont pas des secrets et restent des métadonnées.
- `evidence-manifest.json` :271fichiers de preuves textuelles (logs,JUnit,
  découvertes,identités,commandes,notes,inventaires),17 799 656octets, chemins
  d'origine et SHA256 source/publication. Les logs UTF16/ANSI restent octet-identiques.
- `evidence/{t1,t2,t3,t4,te,f2,f1,final}/` : copies des preuves ignorées sous out/,
  complétées par le rerunT4 au chemin relatif historique et les logs du canariASan.
- `evidence/policy/model-policy.json` : copie du brouillon/politique nativev3.

Les deux manifestes décrivent le snapshot au moment de l'archivage. Ils ne sont
pas un journal vivant des actions ultérieures d'ingestion, commit ou push.
Les sous-dossiers ASan preflight datés du15/16octobre proviennent de l'arbre
partagé préexistant : historiques auxiliaires, pas des runs F7 supplémentaires.
Les dates/chemins du manifeste distinguent ces contrôles du canariT4 du17octobre.
L'export natif sanitize=true masque aussi le dialogue ; pour préserver les
échanges utiles, un export public dérivé a été filtré avant écriture dans Git.
Ce n'est pas une reproduction brute des contextes internes des modèles.

## Exclusions explicites

Sept `.exe` de contrôles négatifs/final restent dans `out/mission-f7/` et ne sont
pas supprimés. Leurs tailles/SHA256 sont dans `excludedBinaries`. Aucun cache
Conan, arbre de build, base de sessions native ou credential store n'est publié.
La configuration `.gitattributes` bornée à cette archive empêche les transformations
EOL des preuves. Une compression gzip des exports n'est pas un exécutable.

## Vérifier et relire

Vérifier chaque fichier avec le SHA256 de son manifeste. Pour les sessions,
décompresser le gzip puis charger le JSON ; `messages` conserve les IDs/horodatages,
les parties publiques d'assistant et les traces d'outils. `excludedInternalParts`
quantifie les omissions, pas des échanges publics manquants.

Le statut natif `outcome=succeeded` signifie qu'un tour s'est terminé, pas qu'un
livrable a été accepté : les deux `test-engineer` terminent par une restitution
BLOCKED. Le coordinateur n'avait pas reçu ces textes via team_results. À l'inverse,
pool-7 possède53entrées et des lectures malgré le diagnostic initial « vierge ».
Les détails et la rectification sont dans la note Obsidian, sources à l'appui.

## Portée des preuves moteur

Candidat final `6d6dcb7` :299/299natifs (251CPU+48OpenGL), zéro skip, WindowsDebug.
ASan245est un historiqueT4 avantTE/F2/F1, pas une validationASanfinale. Aucune
prétention de performance/image/Release/autreOS ou couverture driver exhaustive.
Les exports ne remplacent ni les JUnit ni l'identité du candidat testé.

Le reçu des commandes d'indexation de cette ingestion est conservé séparément
dans `publication/indexing-receipt.json` (hors des271preuves moteur du snapshot).
