# Réveils après livraison — diagnostic et périmètre de décision

## Situation concrète

Tous les livrables ARTIST de cette mission sont intégrés et publiés. Les quatre
remises tardives ont les mêmes identifiants natifs que les sources déjà adoptées.
État observé : 14 tâches completed, deux anciennes tâches deferred/inactives,
aucun worker actif. Aucune nouvelle implémentation n'est nécessaire pour l'audit.

Le réveil répété consomme des tours de supervision sans nouveau résultat ; coût
et délai cumulés non mesurés. Il ne signale pas un défaut nouveau du moteur.

## Cause observée dans le composant effectivement configuré

Lecture de l'API native de configuration, sans afficher les providers ou secrets :
plugin Ensemble configuré `20261029-longrun-activity-64f1dbd594b0`, différent
du répertoire plus ancien consulté lors du premier diagnostic.

Dans ce bundle, `SupervisorRecovery.snapshot()` (autour des lignes18371–18415) :
- sélectionne les équipes/projets actifs ;
- pour long_run, ne conditionne pas le réveil à mission_state=running ;
- réserve au mode non-long_run le contrôle d'absence de travail/rapport en attente.

Ainsi un rapport final, un push et des tâches achevées ne désactivent pas cette
supervision tant que l'équipe est active. Ceci décrit le code observé ; aucune
modification du plugin, de la base de données ou de la politique n'a été faite.

## Pourquoi ne pas lancer cleanup à l'aveugle

Le contrat de team_cleanup inclut des fusions de secours et du nettoyage de
worktrees/branches ; ce n'est pas une simple clôture administrative sans effets.
Avant nettoyage, les consignes exigent de vérifier les travaux et les changements
locaux. Les quatre chemins de worktree encore déclarés par cette mission
(pool-2/3/4/9, suffixe ql1erh) existent mais `git status` échoue pour chacun :
« not a git repository ». La liste Git courante ne les recense pas comme worktrees
enregistrés ; elle contient par ailleurs des worktrees d'autres missions, hors cible.

La propreté de ces ressources et l'effet d'un nettoyage ne sont donc pas certifiés.
Aucun cleanup, force, purge, suppression de branche, réparation de métadonnées
ou complétion fictive n'est autorisé par ce seul réveil et aucun n'a été effectué.

## Options et recommandation

1. Conserver l'équipe et les ressources : aucun risque de suppression, aucun
   changement d'ARTIST ; les réveils peuvent continuer. Pas de nouveau test requis.
2. Confier la clôture non destructive au chantier de maintenance OpenCode/Ensemble :
   objectif distinct du moteur, avec diagnostic ci-dessus. Préserver les archives,
   branches et tâches historiques ; tout changement global de runtime nécessite
   un mandat explicite. Effets potentiels sur d'autres missions à évaluer,
   temps/coût inconnus. **Option recommandée** plutôt qu'un cleanup mal qualifié.
3. Un archivage/nettoyage de l'équipe demanderait d'abord un inventaire non
   destructif des ressources préservées et une qualification de la voie native.
   Pas d'autorisation générale de supprimer ces branches déduite de ce rapport.

Le travail ARTIST est terminé ; la décision restante concerne le périmètre de
maintenance du harness et la préservation de ses ressources, pas un nouveau GO
pour poursuivre des développements ARTIST déjà autorisés. Aucune nouvelle équipe
ou exécution de maintenance n'a été démarrée.
