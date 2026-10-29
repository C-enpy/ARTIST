# 04 — Confirmation du périmètre et de la QA

Date : 2026-10-24. Étape : préparation/configuration, exécution toujours suspendue.

## Décision reçue

L'opérateur répond « Oui, je confirme » à la décision groupée du [rapport 03](03-plan-pm-importe.md). Sont donc confirmés :

- le périmètre B1–B6 de la mission `team_mvm73qjo_0000_6wzibc` ;
- l'autorisation spécifique de l'enquête shuffle B1 ;
- la QA **proportionate**, avec les obligations ARTIST explicitées dans le rapport 03, sans extension d'infrastructure ou de permissions.

Le brouillon porte déjà `verification: { "level": "proportionate" }` ; aucune nouvelle correction de commande, de modèle ou de critère n'est demandée dans ce message. Les préférences de connexions du rapport 02 restent à présenter par config.

## Bornes maintenues

- Pas de seam F8 ni d'extension d'API autorisée : décision après le contrat de preuve concret.
- Allowlist des helpers après inventaire ; égalité stricte conservée.
- C2 non fermé sans reproduction ou acceptation explicite ultérieure d'une fermeture motivée.
- Runner lead distinct ; aucune permission QA élargie.
- Aucun worker, classifier, build, test ou changement de source lancé à cette étape. Aucun nouveau résultat technique.

## Suite

Cette décision confirme le contenu, pas l'exécution. Recommandation : l'utilisateur relance `/ensemble-mission config` pour présenter le brouillon modifié, puis choisit lui-même `/ensemble-mission start`. La pause opérateur n'est pas levée par cette confirmation ; son éventuelle reprise exige un GO distinct.
