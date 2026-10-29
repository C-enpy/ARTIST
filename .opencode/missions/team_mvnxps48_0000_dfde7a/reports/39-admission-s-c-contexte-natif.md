# 39 — Admission S/C : auto-dispatch a conservé les historiques

2026-10-26. Après intégration du graphe37/contrat38 au commit4b0626e, P-CAL complétée sur livrable réellement intégré. Tâches prêtes publiées : S task_mvpdowje_000b_13u1km (cpp,70/66) et C task_mvpdowje_000c_1g59xr (architecte,64/69). Scores PM validés, aucun modèle explicite.

Contrôle d'admission : auto-dispatch a repris pool-5 et b3-contract. GET session/context natif confirme mêmes sessions qu'E1/B1 et E3/B3, avec historique auteur et location canonique ARTIST. La prose fresh/worktree n'a donc pas configuré ces propriétés ; aucune admission auteur/source accordée. Messages explicites de suspension envoyés aux deux affectations : RO admission seulement, aucun code/recherche de fond/handoff/claim. Aucun shutdown répétitif ni completed fictif.

## Correction native proposée par le lead

Utiliser des spawns explicitement fresh et worktree:false pour C / worktree:true pour S, spécialisés identiques, mêmes scores et politique approuvée, aucun modèle/provider forcé. Ne pas réclamer une tâche déjà attribuée ni lancer un double producteur : les deux affectations auto restent inactives, les nouveaux spécialistes reçoivent explicitement l'unique mandat et rapportent au lead. La liaison de board ancienne ne sera pas prétendue réaffectée ; tâche complétée seulement après intégration du véritable livrable et contrôle de provenance. Si le runtime exige une affectation incompatible ou refuse cette voie native, rester BLOCKED et porter le besoin de réaffectation opérateur ; aucune modification de base interne ou droit.

Ce changement corrige l'admission, pas la branche fonctionnelle : mêmes obligations S/C, contrat38 et scores, aucune substitution catch/close. Les nouveaux contextes/isolation doivent encore être vérifiés nativement avant GO de fond ; S reste d'abord protocole RO sans code. Les étapes I/V/D/A ne sont pas publiées.

## Résultat de la tentative native unique

team_spawn fresh engine-architect/worktree:false/mêmes scores64/69, sans claim de la tâche déjà attribuée, a été **refusé : Autonomous workers require claim_task and collaborative reporting**. Aucun nouveau worker créé. Ne pas réessayer en changeant report_mode/modèle ou en inventant un claim ; la voie proposée ci-dessus est donc BLOCKED, non une réaffectation réalisée. Les affectations S/C restent suspendues, sans source ni nouvelle recherche de fond. Besoin d'une réaffectation native autorisée, ou d'une décision explicite sur la continuation RO avec historiques conservés ; pas d'édition de base interne.
