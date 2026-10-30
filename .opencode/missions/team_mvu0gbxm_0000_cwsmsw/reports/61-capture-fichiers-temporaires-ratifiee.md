# 61 — Capture par fichiers temporaires ratifiée

## Décision et raison

La proposition architecte complète est reçue. Le coordinateur retient deux fichiers temporaires privés directement transmis comme stdout/stderr, puis des snapshots de taille finie après attente et arrêt bornés du primaire. Cette solution standard-library supprime la dépendance à l'EOF extérieur et les threads lecteurs parent abandonnés, sans nouveau framework ni supervision générale des descendants.

Les handles de lecture sont ouverts séparément, sans position partagée avec les écrivains. Le runner ferme tous ses handles ; le partage Windows et la suppression automatique doivent être vérifiés. Les sorties disponibles sont celles visibles aux snapshots, pas les écritures tardives ni les buffers utilisateurs non flushés. Un descendant peut conserver son propre handle et prolonger le stockage : aucun nettoyage global des descendants n'est promis.

Une erreur de capture ou une récolte non confirmée ne peut pas devenir succès simplement parce que l'enfant a retourné 0. Le code natif et les sorties disponibles doivent être préservés via un mécanisme privé, puis une panne pipeline explicite propagée. Les schémas publics restent inchangés.

## Tests et barrières

L'auteur prépare d'abord les tests discriminants et un mapping des seuls doubles dépendant de `communicate()`. Les assertions métier de codes, délai, interruption, logs et absence d'effets restent conservées ; les tests ne doivent pas devenir rouges pour une simple incompatibilité d'interface. Aucun GREEN de capture anticipé.

Les premiers tests restent doublés et sur fichiers possédés. Tout futur essai parent/descendant factice devra faire l'objet d'une revue préalable du confinement et du nettoyage ; la proposition architecte n'en autorise pas implicitement l'exécution. Aucun processus tiers n'est concerné.

La définition étroite de CURRENT est déjà ratifiée au rapport 59 ; sa correction peut avancer indépendamment. L'intégration du cœur reste soumise aux corrections, non-régression et ré-audit.

## Références et limites

Proposition `result_mvue5dz2_002p_nh8q4o`, consultation engine-architect en lecture seule. Précisions persistées dans le contrat §8 avant implémentation. Aucun test supplémentaire, moteur, compilation, rendu, benchmark, installation ou cloud exécuté lors de cette décision. Aucun gain de performance revendiqué.
