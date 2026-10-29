# 004 — Coût Grok corrigé dans le brouillon

## Objectif et étape

Corriger le coût effectif de Grok pour cette mission uniquement. **Exécution toujours en pause, non approuvée.**

## Décision et diff depuis le rapport 003

L'opérateur a confirmé **« 0,30 par tâche »**, soit trente centimes dans l'unité monétaire du catalogue.

Dans [model-policy.json](../model-policy.json), uniquement pour `cursor/grok-4.7` / `Grok 4.7 Extra High` :

- `averageCostPerTask` : **3.74 → 0.30**.
- `amortizedCostPerTask` : **0.24933333 → 0.30**.

Références benchmark conservées comme données distinctes : `benchmarkCostPerTask: 3.74`, `cursorBenchCostPerTask: 6.01`. Aucun changement global, modèle, variante, provider, seuil ou classification. La QA proportionnée ciblée, les commandes et critères déjà confirmés restent inchangés ; pas de nouveau questionnaire QA.

## Vérifications et limites

- JSON parsé ; aucun champ top-level hors schéma ajouté ; couple Grok exact et unique, deux coûts effectifs égaux à 0.30.
- Contrôle d'invariance : rétablir seulement ces deux anciennes valeurs en mémoire redonne le hash précédent `8fc6882f5c67e950ab735ddb5fa1725867ad141a3be9808fbd9b020d24d1c7ba`. Tous les autres champs du brouillon sont donc inchangés.
- Nouveau hash SHA-256 du JSON compact : **`758707722d0081a369d328005e68501a6578b81a7371a7d333faa8bfb357afc7`**.
- Aucun worker, classifieur, build ou test exécuté. Contrôle local de configuration seulement ; preview natif et start non invoqués. Aucune nouvelle preuve moteur/GPU n'est revendiquée.

## Suite recommandée

Lancer **`/ensemble-mission config`** pour présenter le brouillon corrigé avant tout éventuel **`/ensemble-mission start`**. Cette correction de coût ne constitue pas une approbation d'exécution. Les décisions de périmètre/QA du [rapport 002](002-configuration-confirmee.md) et le [plan des tâches](../task-preview.md) restent valables.
