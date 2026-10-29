# 003 — Correction du coût Grok : unité à confirmer

## Objectif et étape

Corriger uniquement le coût effectif de `cursor/grok-4.7` dans le brouillon de cette mission. Exécution toujours en pause ; la correction ne vaut pas approbation.

## Depuis le rapport 002

Demande reçue : « Le véritable coût de grok, c'est zéro virgule trente centimes ». Aucun champ du brouillon modifié pour l'instant : une ambiguïté d'unité ferait varier le résultat d'un facteur 100.

## Vérifié

Le [brouillon](../model-policy.json), lignes 384–393, contient pour `Grok 4.7 Extra High` : `averageCostPerTask: 3.74`, `amortizedCostPerTask: 0.24933333`, `benchmarkCostPerTask: 3.74`, `cursorBenchCostPerTask: 6.01`.

La QA proportionnée ciblée, ses commandes et critères restent ceux déjà confirmés au [rapport 002](002-configuration-confirmee.md). Aucun worker, classifieur, build ou test lancé.

## Options et recommandation

- **0,30 par tâche (30 centimes)** : interprétation usuelle d'un coût annoncé « zéro virgule trente ».
- **0,003 par tâche (0,30 centime)** : interprétation littérale de « zéro virgule trente centimes ».

Recommandation : confirmer la première interprétation si l'intention est bien trente centimes. Le coût effectif sera représenté de manière cohérente dans `averageCostPerTask` et `amortizedCostPerTask` ; les références benchmark seront conservées comme données distinctes, ainsi que providers, modèles, variantes et QA.

## Décision demandée

Confirmer **0,30** ou **0,003** dans l'unité monétaire des coûts du catalogue. Ensuite : mise à jour limitée au brouillon, contrôle JSON et nouveau `/ensemble-mission config` demandé à l'utilisateur avant tout éventuel `start`.
