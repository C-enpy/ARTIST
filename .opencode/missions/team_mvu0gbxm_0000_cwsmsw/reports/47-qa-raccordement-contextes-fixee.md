# 47 — QA additionnelle des contextes historiques fixée

Le plan QA ciblé est reçu par le canal natif et intégré comme addendum au plan existant. Le profil servi a été contrôlé avant admission : édition, shell et sous-délégation refusés ; lecture et restitution natives observées. Il n'est pas présenté comme une sandbox.

Une lecture a visé un mauvais chemin de digestion, absent. Les contrats et sources nécessaires étaient lisibles et ont permis le plan complet ; aucun accès élargi ni nouvelle tentative n'a été demandé. Cette limite documentaire ne devient pas un blocage d'implémentation.

Les risques prioritaires sont l'arrêt non propagé dans le SetUp dérivé, l'attribution/agrégation faussement favorable et le transport implicite ou divergent. Huit groupes de contrôles ciblent la nouvelle frontière, sans refaire la suite entière ni assimiler les tests workload existants à une preuve de garde native.

Le contrat fixe désormais l'interface pure du parser d'observation et le transport des six variables publiques. L'activation héritée du parent est exclue : seules les valeurs déclarées par les commandes valent pour le pipeline. Les corps de tests et assertions restent hors édition ; l'ownership de la fixture et du seul arrêt dérivé nécessaire sera attribué explicitement après commit de ces entrées.

Source QA `result_mvucmc0f_001x_7w875k`. Contrat d'entrée `452b615d9efbb89e8eab93a9181ee6c017255684`. Nouveau C++/GL toujours NON_COMPILE/NON_EXECUTE ; runner coordinateur pour les tests synthétiques. Aucun nouveau backend, dépendance ou campagne.
