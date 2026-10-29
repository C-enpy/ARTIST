# 17 — Prévisualisation reçue ; mode long_run non confirmé

2026-10-25. Prévisualisation native hash **b7e5fe216a098cfc79584871a48d10ff0567cd5be160685b628557fc8574d043**, politique non approuvée. Aucune nouvelle admission ni exécution.

## Politique et aperçu restant

Catalogue :14couples déjà présentés, providers openai/opencode/zai-coding-plan ; version2, startingIntelligence50, beta0.35 inchangés. QA **proportionate** conservée à la demande explicite, aucune nouvelle question light/reinforced. Commande de campagne et comptes/limites inchangés. Critère d'audit adapté au brouillon : test-reviewer fresh/restrictif sur tâche dédiée, données autorisées sans justifications auteurs, profil/transport à vérifier et refus fermé si incompatibilité ; pas de prétention de sandbox ou de board techniquement invisible.

Travaux acquis et non à rejouer : H3/remise39 intégrés ; correctif test82bf52d revu ; ASan555/555 et CPU normal484/484 ; diagnostic QA Windows/Linux13 ; photographie documentaire provisoire et index artist-wiki actualisés. Restent audit dédié puis bilan de clôture selon résultats, avec F8/C2/B1/GCC et absence de campagne Linux explicitement conservés.

## Écart de configuration observé

Le bloc transitoire demandé au16 était syntaxiquement accepté par le lecteur natif extrait en mémoire. La nouvelle prévisualisation ne contient plus ce bloc, ce qui est compatible avec sa consommation par config, **mais ne prouve pas son application**.

Contrôle effectif `team_status()` après cette prévisualisation : **Mission supervised/running ; classification=manual ; policy draft/not approved**. Le statut expose encore les acceptations de la révision antérieure admise, pas la nouvelle approbation ; ne pas confondre ces deux couches. Aucun message système de changement de classification/mode n'a été récupéré par team_results ; seuls deux anciens messages du lead étaient disponibles.

Le code installé observé attend bien classification.missionMode=long_run et conserve le champ même avec mode off/manual ; sa simple lecture ne démontre pas l'appel natif effectif ni une version active sur chaque chemin. **Cause précise de l'écart encore INCONCLUSIVE** ; ne pas l'attribuer à l'utilisateur ou à une erreur de schéma planning.reason, et ne pas répéter un rewrite identique sans nouvelle observation.

## Conduite retenue

Priorité qualité : ne pas annoncer long_run actif depuis le seul JSON ou une intention. Maintenir le gate d'approbation et toutes les preuves acquises ; aucune réécriture globale, team_message de contournement, accès à la base interne ou modification du runtime. Une nouvelle approbation reste réservée au start natif utilisateur ; son mode doit être distingué du hash catalogue affiché. Le diagnostic de mode ne justifie aucun build/test ni relance worker.

## Résolution effectivement vérifiée dans cette reprise

Une lecture du brouillon a établi que le bloc classification était **encore présent sur disque** : la prévisualisation précédente ne l'avait pas consommé. Pas de nouveau rewrite identique. La session superviseur courante a été recoupée via l'API native publique de contexte (présence de la remise16), sans publication du contenu privé.

Le lead a invoqué **config uniquement**, par la route publique documentée `POST /api/session/<superviseur>/command`, corps `{name:"ensemble-mission",text:"config",delivery:"queue"}` via CLI API ; exit0, présentation mise en file. Aucun start ni modification de base/runtime.

Contrôle `team_status()` immédiatement après : **Mission long_run/running ; classification=manual ; calls=0 ; policy draft/not approved**. Le mode demandé est désormais réellement appliqué, sans nouveau classifier ni worker. L'écart constaté au début de cette note est donc résolu ; ne pas le présenter comme un blocage persistant ni attribuer une panne au runtime sur ce seul décalage de prévisualisation.

La nouvelle politique d'audit/catalogue reste à approuver par le start natif utilisateur, sur la présentation config correspondante. Le hash catalogue peut rester identique puisque le bloc de configuration est transitoire et le mode de résilience est distinct de ce JSON de politique. Aucune campagne valide à rejouer.
