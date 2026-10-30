# 157 — Livraison acceptée, équipe native conservée

Le pipeline reste livré et accepté pour qualification ultérieure au rapport156.
Le réveil du superviseur ne signale aucun défaut nouveau : les sept notifications
persistées concernent les candidats déjà reçus, intégrés ou reconnus comme
réaffectations obsolètes aux rapports149–156. Aucun test n'a été relancé.

Le runtime indique encore une mission long_run/running. Son statut courant
confirme des membres historiques inactifs avec des worktrees conservés, dont
pool-19/29/30, et les trois anciennes fiches explicitement deferred. Cette ouverture
administrative est distincte de la réception du livrable.

## Frontière vérifiée avant fermeture

Le catalogue n'expose pas de commande de clôture préservant séparément les
ressources. Le handler natif ensemble-mission inspecté en lecture seule ne traite
que préparation/configuration/start. Il ne faut pas inventer une commande close
ou modifier la base interne pour arrêter le réveil.

L'opération disponible team_cleanup n'est pas une simple clôture : elle vérifie
les membres et changements non committés, peut fusionner les branches non intégrées,
retirer worktrees/espaces de travail et supprimer des références, puis archive
l'équipe seulement si ces étapes réussissent. L'appel inspecté n'expose pas
d'option d'archivage seul ou de préservation de toutes les branches. Les brouillons
historiques non intégrables sont précisément conservés, pas à fusionner avec le
candidat accepté ou effacer pour rendre l'état vert.

Le lead n'appelle donc ni cleanup/force/acknowledge_uncommitted/purge, ni shutdown
en boucle, ni API interne ou modification de permissions/configuration. Le
pipeline validé, les profils préexistants, les branches et les preuves restent
inchangés. Aucune nouvelle exécution ou admission auteur n'est justifiée.

## Choix administratif restant

Recommandation : conserver l'équipe et ses brouillons. Cela garde la livraison
intacte mais laisse la mission native ouverte, avec possibilité de nouveaux réveils.
Alternative : ouvrir une étape distincte de préparation d'une fermeture sûre
(inventaire/sauvegarde et proposition bornée), sans aucune fusion ou suppression
avant décision précise. Cette étape dépasse la livraison du pipeline et ne vaut
pas autorisation de modifier le runtime ou de purger des ressources.

Condition nécessaire à une clôture sans risque : une voie native d'archivage
préservant les brouillons, ou un plan de traitement de ces ressources examiné
et explicitement autorisé. Aucun délai/coût n'est établi pour cette alternative.

## Preuves de diagnostic, lecture seule

- team_results : sept notifications historiques, toutes déjà traitées.
- team_status : mission long_run/running, membres sans travail nouveau ; trois
  anciens suivis deferred conservés, aucune tâche d'auteur en cours.
- Plugin installé, index.js22343–22417 : handler ensemble-mission ;
  index.js16981–17164 et17709–17737 : effets et paramètres de team_cleanup.
  Lecture ciblée seulement, aucun code du plugin modifié ou fonction interne appelée.
- Pas de changement des sources du pipeline depuis code165aee5. Dernière
  photographie de réception99da28f et preuves du rapport156 conservées.

Qualification native Windows/Linux/GPU, moteur, rendu, benchmark, installation
et cloud restent NON_EXECUTE. Ce point administratif n'invalide aucun résultat
synthétique et n'autorise aucune campagne supplémentaire.
