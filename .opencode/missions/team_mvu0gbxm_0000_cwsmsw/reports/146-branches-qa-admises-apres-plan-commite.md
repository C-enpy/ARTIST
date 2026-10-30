# 146 — Deux branches de compléments admises après plan commité

Le plan QA §15 et les mappings sont committés avant admission. Deux branches
indépendantes sont publiées : plan minimal de couture du même bundle, et plan
détaillé de l'environnement Windows/migration des tests historiques. Aucune
nouvelle écriture de tests n'est encore autorisée avant validation de ces propositions.

La couture n'utilisera ni témoin préfabriqué ni juxtaposition de deux cas. L'autre
branche doit préserver les garanties historiques sans réintroduire helper-run,
global skip ou archive invisible. Les programmes de tests resteront relus puis
exécutés uniquement par le coordinateur ; aucune modification production prévue.

La tâche de proposition textuelle QA est close sur son plan effectivement reçu
et persisté, pas sur des tests inexistants. L'audit final indépendant et l'auteur
des tests de couture restent dans le plan, non publiés, jusqu'à leurs entrées.

## Contrats natifs et dépendances

- H0 `040824cff819b074bfe0738efb3e35f66973555f`, QA§15 et rapport145 inclus.
- A0 `task_mvuxjlwk_008e_lz4y5f`, build-engineer fresh sans worktree, recherche
 63/59 ; plan seulement. A1 tests78/73 non publié jusqu'à validation A0.
- B `task_mvuxkk7z_008f_27730j`, build-engineer fresh avec worktree, développement
 73/66, première remise textuelle sans édition. Mapping exact avant code.
- Les deux tâches dépendent du vrai ID producteur de plan terminé
 `task_mvuwy807_0081_u0ae22`, pas d'une simple fin d'auteur non intégrée.
- Ownership distinct : A futurs nouveaux fichiers dédiés de couture ; B seuls
 tests Windows effectifs et trois fichiers historiques explicitement nommés.
- Politique et grille approuvées inchangées, aucun modèle imposé. Exécutions
 partageant un interpréteur ou arbre sérialisées par le lead.

Aucun nouveau test, outil natif, moteur, GPU, rendu, benchmark, installation
ou cloud exécuté. Tous les gaps finaux restent INCONCLUSIVE jusqu'aux preuves.
