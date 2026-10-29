# 07 — Correction opérateur : Cursor retiré, reprise visée sur OpenAI Sol

Date : 2026-10-24. Étape : révision du brouillon ; aucune nouvelle admission.

## Décision et diff

Après le [rapport 06](06-capacite-et-preparation-incomplete.md), l'opérateur demande : « Change curseur par Open AI et utilise six point un sol ».

Modification du seul [brouillon de cette mission](../model-policy.json) : retrait du provider `cursor` et des deux entrées `cursor/grok-4.7` et `cursor/claude-opus-5-5`. Les entrées existantes `openai/gpt-6.1-sol` sont conservées, sans duplication ni transfert des métriques Cursor : variantes natives high, medium, max, low, xhigh. Les autres connexions préférées OpenCode Go/Z AI et les alternatives non Cursor restent inchangées.

Le catalogue contient maintenant 27 couples, quatre providers, seuil initial 50. QA `proportionate` inchangée : l'opérateur n'a demandé aucune correction de vérification ni de classification. Aucun champ hors schéma, aucun réglage global ni override de spawn/message ajouté.

## Vérification et portée

Contrôle Context Mode : JSON parsé, clés racine autorisées, providers cohérents, aucun couple dupliqué ni entrée Cursor restante. La découverte native antérieure confirme `openai/gpt-6.1-sol` et ses cinq variantes. La validation native de la nouvelle politique par config/start reste requise ; l'approbation révision 1 ne couvre pas ce brouillon modifié.

La tâche ASan déjà admise sur Cursor n'a pas changé de modèle en place. Après nouvelle approbation, une reprise de recherche sur Sol devra utiliser une admission native distincte et traçable, sans effacer l'échec historique : tâche de protocole documentaire/recherche qualifiée comme telle, décision directe autorisée sur `openai/gpt-6.1-sol` high si le contrat natif l'accepte, sans override au spawn. L'ancienne tâche ne sera ni marquée réussie ni réécrite pour changer son modèle. Les consommateurs resteront bloqués jusqu'à intégration du vrai livrable de remplacement.

Ce mécanisme de reprise est proposé, pas exécuté ; si la voie native ne permet pas cette admission, signaler BLOCKED plutôt que changer silencieusement de modèle. Le retrait de Cursor ne garantit pas que toute tâche de la mission utilisera Sol, puisque les autres connexions demandées restent présentes.

## État et prochaine action

H_Q, H_A et sélection des 17 GL restent non livrés. Aucun test, correctif, changement de permissions ou nouveau worker lancé dans cette étape. Le contenu des rapports 04/06 reste applicable.

Recommandation et action demandée : l'utilisateur exécute `/ensemble-mission config` pour présenter le brouillon corrigé, puis décide lui-même de `/ensemble-mission start`. Pas de nouvelle question QA : le choix proportionate et les obligations ARTIST déjà confirmés sont conservés.
