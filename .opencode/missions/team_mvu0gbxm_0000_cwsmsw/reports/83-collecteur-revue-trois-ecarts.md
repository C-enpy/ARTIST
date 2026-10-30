# 83 — La revue bloque le collecteur sur la conservation des échecs

Le pipeline doit conserver les échecs et leurs causes même si une autre invocation manque de contexte ou s'interrompt. La revue indépendante établit trois écarts par lecture du candidat ; leurs reproductions ne sont pas encore exécutées.

1. Un workload déjà en échec suivi d'une précondition CTest indisponible peut perdre son verdict FAIL, alors que le code agrégé conserve l'échec. État, verdict et causes deviennent contradictoires.
2. Une première invocation CTest avec assertion échouée n'est interprétée qu'après toutes les commandes. Une interruption ultérieure ou un deuxième XML tronqué empêche donc de conserver sa cause d'échec et sa population partielle, malgré les fichiers bruts disponibles.
3. Le rapport contrôle certains accords manifeste/receipt seulement lorsque l'état est COMPLETED. Un manifeste BLOCKED modifié en PASS/code0 peut ainsi sortir favorable malgré son reçu inchangé. Ce dernier gap est hérité, pas une régression nouvellement introduite.

Le coordinateur a recoupé les chemins du premier et du troisième point. Il demande quatre tests discriminants, couvrant les deux variantes du deuxième point, sur données JUnit et doubles existants avant correction. Les 167 tests acquis, les assertions et le mapping ratifié restent inchangés. Aucun nouveau schéma ou audit général n'est autorisé ; le raccordement au bundle Windows reste différé.

Les résultats verts du rapport 80 restent acquis dans leur portée, mais ne couvraient pas ces combinaisons. L'intégration du collecteur est bloquée, pas les préparations indépendantes déjà autorisées. Aucun accord utilisateur supplémentaire n'est nécessaire pour ces corrections locales conformes au contrat.

## Références et limites

- Avis fresh `result_mvuhogem_0043_6tlusy`, tâche `task_mvuhh0f9_003z_78xct7` ; aucune édition ou exécution par le reviewer.
- Candidat `50352e5ef096cecbb04a771984890c464073e619`.
- Localisation : `evidence.py::_junit` vers 849 et 897–905, `execute` vers 946–968 ; `reporting.py::report` vers 155–163 et 197–209.
- Nouveau fichier de régressions proposé : `test_context_review_regressions.py`, auteur pool-14, runner réservé au coordinateur après remise RED_READY et relecture.

Aucun test supplémentaire, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté à cette étape.
