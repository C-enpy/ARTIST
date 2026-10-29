# 002 — Choix QA persistés, démarrage non approuvé

## Objectif et étape

2026-10-23 — mission QA & harness : préparation/configuration terminée au périmètre demandé. **Toujours en pause, aucun démarrage approuvé.** Le [rapport 001](001-preparation-decision.md) contient l'enquête et le catalogue ; l'[aperçu des tâches](../task-preview.md) détaille le plan borné.

## Décisions reçues et changements

L'opérateur a choisi explicitement :

1. **« Proportionnée ciblée »** : dérogation limitée à cette mission à la chaîne obligatoire `qa-planner → test-engineer → test-reviewer` ; plan du lead, TDD, runner séparé et revue fresh ciblée conservés.
2. **« Confirmer ce périmètre »** : les six branches, les commandes existantes Windows Debug/ASan et les critères proposés ; pas de PASS global si une preuve requise manque. C2 sans reproduction garde sa décision de clôture distincte.

Corrections inscrites dans [model-policy.json](../model-policy.json), sous `verification` uniquement : `level: proportionate`, **9 commandes**, **11 critères d'acceptation**. Le plan conserve sa barrière de persistance/commit avant worktrees auteurs. Les instructions globales et projet ne sont pas modifiées.

## Vérifications réellement effectuées

- JSON parsé et structure `verification` acceptée par la fonction pure `validateVerificationPolicy`, isolée du bundle installé `C:/Users/djden/.config/opencode/local-plugins/ensemble/20261023-planning-v3-fd3e61e-fde124341d9c/index.js`. Aucun chargement/démarrage du plugin ; ce contrôle ne remplace pas le preview natif.
- Nouveau hash SHA-256 de `JSON.stringify(policy)` : **`8fc6882f5c67e950ab735ddb5fa1725867ad141a3be9808fbd9b020d24d1c7ba`**.
- Contrôle d'invariance : en rétablissant uniquement l'ancien `verification: { level: proportionate }` en mémoire, le hash redevient **`847ff3b9cc3c2f09051f76b7a15bb251f16492e8b774df1d32cc1ccf74033652`**. Tous les champs hors QA sont donc identiques au brouillon présenté initialement, y compris les 29 couples modèle/effort, providers, réserves, seuil 50 et métadonnées.
- Aucun worker ni classifieur admis, aucune tâche publiée ; aucun build/test exécuté. Sources moteur et modifications utilisateur préexistantes intactes.

## Politique et tâches à prévisualiser

Politique effective : version 2, providers `cursor/openai/opencode/opencode-go/zai-coding-plan`, 29 couples inchangés (catalogue complet au rapport 001 et dans le JSON), seuil de planification 50, pas de classification, même modèle/session par tâche avec effort autorisé ajustable ; QA proportionnée ciblée selon les critères persistés. Disponibilité native des couples non revalidée ici.

Tâches : B1 isolation shuffle, B3 F8 réel, B6 const-correctness libstdc++ en branches indépendantes ; B4 unbind/égalité stricte après intégration B1/B3 ; B5 diagnostic C2 par le lead ; B2 ASan finale après intégration et éventuelles corrections. Revue indépendante ciblée, ressources build/GPU exclusives, documentation et réindexation ARTIST en clôture. Aucun nouveau framework, backend, périmètre Release/image/performance ou adoption ADR-0004.

## Inconnues et limites conservées

Les limites de preuve sont celles du rapport 001 : GCC non validé, F8 authentique restant à établir, cause C2 inconnue, transport et environnement GPU/ASan non vérifiés sur le candidat courant. Leur absence ne devient pas un PASS par cette confirmation. Les métriques catalogue ne sont ni un devis total ni une probabilité de réussite.

## Suite recommandée — commande humaine

Le conflit QA est arbitré ; aucune nouvelle question n'est nécessaire pour le plan actuel. Les corrections de chat restent possibles.

1. L'utilisateur lance **`/ensemble-mission config`** pour présenter la politique corrigée exacte et son nouveau hash.
2. Après lecture de ce preview, l'utilisateur lance **`/ensemble-mission start`** s'il souhaite approuver et libérer l'exécution.

Ces commandes n'ont pas été invoquées par le lead. Les réponses au questionnaire n'ont pas valeur de démarrage de mission.
