# 56 — Préciser la capture Windows et le domaine de fraîcheur

## Précisions du ré-audit

Le reviewer détaille le résidu bloquant de capture : sous CPython Windows, le lecteur attend la fin du flux avant de publier son buffer. Si un descendant conserve stdout ouvert, `communicate` peut expirer sans sorties dans son exception, même quand stderr a déjà été collecté. Le candidat conserve seulement les sorties fournies par ces exceptions et laisse volontairement le lecteur vivant pour éviter une fermeture bloquante. Le retour est borné, mais les données et ressources ne sont pas toutes maîtrisées.

Le test actuel utilise des exceptions contenant systématiquement les sorties ; il ne détecte pas ce chemin. Le constat reste statique, appuyé sur le mécanisme CPython cité, sans expérience réelle de descendant ni nouvelle exécution.

Le reviewer borne le deuxième point : une copie supplémentaire de l'exécutable requis rend la population ambiguë pour `_build_files`, mais pas pour `report`. Cela ne démontre pas une altération du binaire utilisé ou du résultat passé. C'est une incohérence du domaine de fraîcheur, pas une preuve d'exécution étrangère ni une exigence de rejeter tout nouveau fichier généré.

## Décision de coordination

Une consultation architecte ciblée est publiée avant correction : choisir la plus petite capture standard-library avec sorties disponibles, retour borné et ressources possédées, sans arrêt arbitraire de descendants ; préciser le même domaine étroit de fraîcheur pour les cibles et fichiers pertinents déjà recensés.

La redirection des deux sorties vers des fichiers temporaires possédés est une option à examiner, pas une décision déjà implémentée. Les tests discriminants doivent précéder le correctif, sans rendre artificiellement rouges les doubles par changement d'API. L'implémentation de capture attend cette ratification ; les autres branches continuent.

## Références

- Avis détaillé `result_mvudvevq_0000_26ng9e`, candidat `8ecb5675825b551ee6beede171ecf48e8351f48b`.
- CPython local cité par le reviewer : `C:/Python313/Lib/subprocess.py:1608–1644` ; runner lignes 45–51, 66–82, 107–116 ; rapport lignes 138–145.
- Consultation `task_mvudy0ld_002g_1en2po`, engine-architect fresh, lecture seule, sans worktree d'écriture, évaluation PM/LLM de lecture 65/75.

Aucun test supplémentaire, moteur, compilation, rendu, benchmark, installation ou cloud exécuté. L'intégration du cœur reste bloquée sur la capture indépendamment du choix de fraîcheur.
