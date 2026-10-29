# 33 — Blocage QA : canal collaboratif incompatible avec le profil restrictif

2026-10-24. Constat confirmé par l'opérateur : le worker test-engineer n'a pas les droits pour exécuter team_message. Ce n'est pas une permission à ouvrir automatiquement.

## Contrat et erreur de coordination

Le profil canonique test-engineer conserve deny-all et27 règles : read/glob/skill seulement, execute/shell/edit/subagent fermés. Team Message via Code Mode est donc indisponible par construction. Les instructions de tâche, même envoyées par le lead, ne changent pas ces droits.

Le profil prévoit la restitution `final_response`. La tentative explicite du lead de configurer ce mode a été refusée par le runtime : « Autonomous workers require claim_task and collaborative reporting ». L'auto-dispatch a néanmoins admis pool-15 sur cette tâche. Son profil déclaré et son statut working ne prouvent pas une remise possible. **L'admission aurait dû rester bloquée avant exécution tant que le canal natif n'était pas validé.** Erreur du coordinateur, pas défaut moteur.

Les retours sans contenu ou les messages du lead réaffichés ne sont pas des livraisons du generator. Aucun livrable QA pool-15 n'est accepté ou marqué terminé. La demande précédente de finale unique ne prouve pas que le runtime la récupère.

## Disposition immédiate

Étape generator **BLOCKED sur restitution native**. Pas de nouvelle tentative du canal interdit, pas de recherche de transport alternatif, pas d'ouverture execute/team_*, pas de changement de profil/modèle ou de fausse complétion. Pas de cycle shutdown/réadmission susceptible de recréer la même affectation. La consigne d'arrêter les tentatives a déjà été transmise ; aucune activité utile supplémentaire attendue du worker tant que la remise n'est pas réglée.

H3 et campagne finale restent derrière cette barrière. Résultats existants conservés :555/555 normaux, revue correctness reçue, build ASan propre/canari validés ; suite ASan finale NOT_RUN. Aucun PASS global ou clôture implicite.

## Options pour décision opérateur

1. **Réparer la restitution native**, recommandé : autoriser un diagnostic/correctif ciblé de l'admission/transport Ensemble pour respecter final_response, sans changer les permissions QA, le catalogue ou la politique modèles. Nouvelle mission/harness de réparation si nécessaire, avec périmètre et validation propres ; aucune réparation déjà autorisée par ce rapport.
2. **Suspendre la mission à ce point**, en conservant les livrables et le blocage explicites. Pas de clôture verte ni de dispense QA supposée.

Coût de l'option1 encore inconnu ; ne pas engager une campagne de réparation du plugin sur le seul mandat moteur initial. L'option2 évite cette extension mais laisse la mission inachevée.
