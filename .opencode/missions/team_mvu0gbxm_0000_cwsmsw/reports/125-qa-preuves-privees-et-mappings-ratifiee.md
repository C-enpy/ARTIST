# 125 — Plan QA et mapping des fakes privés ratifiés

Le complément QA est reçu intégralement, relu et ratifié au §14 du plan. Il couvre
le collecteur core sur tous les chemins, l'ownership des fichiers, la conservation
des causes et le nouveau câblage PowerShell avec factory/horloge mémoire. Aucun
nouveau schéma public ou test natif n'est ajouté.

Le coordinateur a lu les deux fakes de succès concernés et fixe leur mapping
avant modification : produire les nouvelles données privées sous les chemins
possédés, conserver assertions payload/phases/codes/capture et ne rien ajouter aux
doubles génériques sans transport helper. Les tests RED nouveaux précèdent core
et adaptations de ces fakes ; leur passage actuel sans témoin ne justifie aucun
affaiblissement du contrat.

Après ce commit, un propriétaire core unique peut préparer ses tests sans processus,
pendant que le propriétaire PowerShell prépare la fixture de ses feuilles réelles.
L'ownership des fichiers est distinct. Les deux entrées devront être intégrées
avant couture et réception du helper. Son GREEN de production reste non autorisé
à ce stade ; les baselines et RED antérieurs ne prouvent pas ces nouveaux oracles.

## Références

- Proposition `result_mvuq7fp5_0000_3vnwya`, qa-planner/pool-21, lecture seule,
  aucun refus ni exécution. Contrat lu au commit fourni `b8d036a`.
- Plan ratifié `qa-plan.md` §14, convention `windows-helper-evidence-v1.md`.
- Mappings recoupés : `OwnedBundleRunner.run` lignes82–100 et `BindingRunner.run`
  lignes55–90, fichiers de tests existants ; assertions inchangées pour l'instant.

Aucun nouveau test, outil natif, moteur, GPU, rendu, benchmark, installation
ou cloud exécuté ; l'identité du témoin reste déclarative, pas binaire outil.

## Admission après commit

Plan et mappings committés `6989261563d28502524d3d01c4ef2b2662739c78`. Nouvelle
tâche core `task_mvuqdtqz_006q_uoqqv9`, contexte fresh/worktree/completion lead
explicites, tests RED seulement ; statut natif confirme pool-34 build-engineer.
La reprise de pool-31 sur sa tranche PowerShell conserve78/68 et son candidat
44d3a079 ; elle ne reçoit que les nouveaux tests de feuilles, après ce commit,
sans copie de core ou modification de production. Le statut confirme les deux
auteurs actifs en parallèle et leurs worktrees distincts.

Le core possède aussi les adaptations de données Python des deux fakes mappés ;
PowerShell possède ses nouveaux scripts/fixtures, pas ces fakes. Aucun écrivain
concurrent sur evidence.py, helper ou support Python existant. Toutes les commandes
de test restent réservées au coordinateur après remise et relecture.
