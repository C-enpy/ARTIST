# 04 — Clarifications du handover et rejet de schéma non expliqué

2026-10-25. Préparation uniquement, aucune tâche importée/exécutée.

## Évolution depuis03

L'utilisateur a clarifié que ce chat est consacré au handover. L'ancienne mission sert de provenance des livrables et preuves, pas d'équipe à ressusciter. La réponse du superviseur a corrigé l'interprétation précédente : préparer une suite autonome, sans transfert d'approbation ni modification du board ancien. **Le rattachement historique n'est donc plus un prérequis de cette analyse/préparation.** Les arbitrages du rapport03 ne doivent pas être reposés à l'identique.

Linux : le mandat vise une QA robuste Windows/Linux et autorise son examen critique. Aucune campagne Linux, machine Linux ou installation n'a été explicitement sélectionnée. Préparer l'analyse de portabilité et les gaps ; ne pas annoncer ni lancer une campagne Linux. Si une exécution Linux devient nécessaire à une acceptation proposée, identifier alors l'environnement et faire arbitrer cette extension.

Audit : distinguer capacité à préparer le plan et capacité à admettre son futur reviewer. Le catalogue de spawn accepte profil/context/worktree ; le schéma de team_tasks_add ne suffit pas à prouver leur perte. La restriction hors board reste à respecter et sa voie native n'est pas démontrée. La documenter comme gate d'exécution/clôture, pas comme nécessité de rattacher l'ancienne équipe. Ne pas annoncer un plan totalement exécutable si ce gate est encore ouvert ; ne pas contourner par tâche factice, autre profil ou permissions élargies.

## Nouveau diagnostic de version

La configuration globale actuelle référence désormais le plugin **20261025-pm-preparation-da5345a2385c**, et non le chemin fdbbe025cbf1 initialement consulté. Lecture ciblée du log natif : entrées `loading plugin` de cette version à15:30:52.321Z et15:37:40.989Z. C'est une preuve de chargement, pas la preuve de la version ayant traité chaque rejet historique.

SHA256 de son index.js : `da5345a2385c33aa090f1ac5e6fd7c430e18b69a19b1d56451c29b7e3b31b51c`.

Le validateur de cette version accepte bien les seuls champs planning `required` et `reason` (ligne19963), puis exige une chaîne reason de1 à2000caractères (19964). Le constat PM « le candidat contient cinq chaînes » ne suffit pas à prouver leur conformité : longueur, objet effectivement analysé et message de validation exact manquent encore.

La frontière réelle est `runBoundedLoop` : lecture via `port.readAssistant` (20907), passage de `reply` à `importReply` (20910), puis `documentIssue(reply)` (20914). L'adaptateur lit `lastAssistant(ctx.session.context(...))` (21824–21831). Cette chaîne précise où comparer l'entrée ; aucune altération ou extraction fautive n'est démontrée ici.

`team_results({})` ne fournit aucun message non lu. Aucun candidat PM brut ni erreur détaillée n'est présent dans les fichiers de cette mission découverts ici. La valeur exacte reçue par le validateur reste donc **INCONCLUSIVE**. Aucune base interne lue/modifiée, aucun prompt privé ou log brut publié, aucune nouvelle boucle de réécriture du candidat lancée.

## Clarification à transmettre au PM

Utiliser le handover comme provenance ; préparer la suite autonome sans rattachement ni mutation du board historique. Linux = analyse critique de portabilité/gaps dans ce périmètre, aucune campagne ou installation implicite. Conserver les exigences de revue et leur gate natif non résolu, sans les remplacer par une fausse preuve d'admission. Réutiliser rapport17 et remise39 ; ne pas relancer les travaux acquis.

Pour le schéma, ne pas réécrire les cinq raisons à l'identique : restituer le candidat JSON exact et le message précis de rejet par la voie native de préparation, afin de comparer la frontière de validation (champs, types et longueurs uniquement dans les diagnostics partagés). Version actuelle configurée et chargée da5345a2385c ; les deux versions consultées acceptent planning.reason. Sans entrée exacte, conclure INCONCLUSIVE plutôt qu'attribuer le défaut au PM ou au runtime.

Le prochain resume est une clarification de préparation, pas un start, une admission worker ou une approbation de source. Aucune réparation du runtime ni infrastructure supplémentaire engagée.
