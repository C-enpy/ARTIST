# 03 — Plan PM importé, décisions avant admission

Date : 2026-10-24. Étape : préparation ; aucune approbation native de démarrage reçue.

## Évolution et preuves disponibles

L'événement de préparation reçu indique un PM `project-manager`, modèle `openai/gpt-6.1-sol`, variante **high**, avec 16 tâches importées et leurs IDs. La variante observée n'est pas xhigh, contrairement au mandat initial ; aucune exécution PM supplémentaire n'est lancée pour masquer cet écart.

Le plan importé remplace l'aperçu provisoire du [rapport 01](01-preparation.md) pour le découpage. Le [rapport 02](02-correction-catalogue.md) conserve ses limites de routage et son brouillon non approuvé. `team_results` consulté une fois : aucun message non lu ; les détails ci-dessous proviennent de l'événement d'import, pas d'une revue de livrables ni d'une preuve exécutée.

L'étiquette importée `authorized; planned` ne vaut ni approbation par `start`, ni autorisation de lever la pause, ni acceptation des gates K. Aucun worker lancé par le coordinateur, aucun changement de source et aucun test exécuté pendant cette étape.

## Plan retenu pour discussion

1. Après décisions et start : `qa-plan` (`task_mvmbkkq8_0001_y6g986`), `f8-contract` (`task_mvmbkkq8_0002_5aeiej`), `asan-protocol` (`task_mvmbkkq8_0003_ft1wq9`) et `c2-investigation` (`task_mvmbkkq8_0004_zbcvze`) sont des enquêtes indépendantes. Pas de worktree auteur pour les profils en lecture seule ; probes éventuelles exécutées par le lead sous mandat explicite.
2. Après plan QA persisté/commité H_Q, et contrat F8 H_A pour F8 : `shuffle`, `f8-real`, `unbind`, `stacktrace` en parallèle sur ownership disjoint confirmé. TestUtils reste immuable pendant cette vague.
3. `integrate-wave1` matérialise H1 ; `strict-errors` attend cette intégration et l'allowlist réelle des consommateurs. Puis `integrate-candidate` matérialise H2.
4. `correctness` et `gaps` peuvent analyser H2 gelé en parallèle, sans mutation ni partage des justifications compromettant l'indépendance. Le lead intègre les propositions acceptées (`integrate-gaps`, H3), exécute la `final-campaign`, puis organise l'audit fresh hors board requis et la `closure`.

Ce graphe est préférable à une consolidation de helpers dans la première vague : il évite un fichier partagé mutable et place la campagne finale sur le candidat intégré. Builds privés, un build par arbre, GL sous bail global ; aucune campagne finale antérieure aux mutations qu'elle prétend valider.

## Réponses du coordinateur aux questions PM

- **mission-active** : mission de préparation visée `team_mvm73qjo_0000_6wzibc`, objectif `docs/architecture/next-mission.md`, périmètre proposé B1–B6 de la fiche du 2026-10-23. Confirmation opérateur encore attendue ; aucun élargissement.
- **gate-k** : B1 reste bloqué sur son autorisation séparée. C2 reste OPEN/DEFER sans reproduction ou sans acceptation explicite ultérieure d'une fermeture motivée.
- **f8-boundary** : autoriser d'abord la conception du contrat de preuve, pas une seam ni une extension d'API. Après `f8-contract`, présenter les options concrètes et leurs limites avant toute décision d'extension. Une injection ne sera pas qualifiée de panne authentique sans justification vérifiable.
- **helper-scope** : `ViewTests` n'est pas identifié par le briefing actuel. Inventaire exact et allowlist des consommateurs avant `strict-errors` ; ne pas traiter ce nom comme une permission sur un sous-arbre. L'égalité stricte reste obligatoire.
- **qa-permissions** : runner lead distinct ; profils QA en lecture seule, sans shell/execute/édition selon les instructions de coordination prioritaires. Restitution uniquement par le canal réellement autorisé et disponible ; si ce canal manque, BLOCKED, sans grant implicite ni contournement. Aucune correction de permissions dans cette mission sans décision séparée.

## Risques et inconnues

Les contrats F8, l'allowlist helpers, le transport QA, les scores détaillés PM et la couverture exacte des preuves importées ne sont pas vérifiés ici. Avant toute publication/admission, recouper ces éléments avec les fiches effectives et les exigences ARTIST ; les IDs seuls ne constituent pas un contrat de travail complet. Aucun résultat RED/GREEN courant n'existe dans cette étape.

QA proposée inchangée : **proportionate**. Le conflit entre la dispense générique de pipeline et les obligations ARTIST reste à arbitrer sans suppression silencieuse de ces obligations. Les options light/reinforced ne changent pas ces exigences de sécurité ; aucune nouvelle infrastructure proposée.

## Décision groupée demandée

Confirmer le périmètre B1–B6 et l'autorisation séparée de l'enquête shuffle B1, avec QA proportionate respectant les obligations ARTIST ; ou préciser les corrections importantes. Recommandation : confirmer ce périmètre borné, garder C2 ouvert sans acceptation distincte, et différer les choix seam F8/allowlist jusqu'aux inventaires pertinents plutôt que demander des décisions abstraites maintenant.

Cette confirmation de contenu n'est pas une approbation d'exécution. Après corrections éventuelles du brouillon, l'utilisateur doit faire représenter la politique par `/ensemble-mission config`, puis décider lui-même de `/ensemble-mission start`. Une pause opérateur exige en outre un GO de reprise distinct.
