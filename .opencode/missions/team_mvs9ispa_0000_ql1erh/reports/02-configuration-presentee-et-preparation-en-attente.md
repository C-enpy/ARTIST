# Rattrapage QA Linux — configuration présentée, préparation en attente

## Objectif et état

Porter les obligations QA Windows sur Linux en réutilisant au maximum le socle existant. La politique corrigée a été présentée par la commande native de configuration. Elle reste non approuvée ; le mandat autonome n'est pas actif.

La préparation PM existante a été annoncée comme mise en file. Son résultat ou erreur doit être transmis au superviseur. Ne pas lancer de préparation concurrente ni confondre cet enregistrement avec un statut natif de session.

## Politique présentée

- Provider : OpenAI uniquement ; 11 couples modèle/variante inchangés après filtrage.
- Version 2, seuil initial 50 ; classification non activée.
- QA proportionnée, exigences obligatoires du projet conservées.
- Autonomie proposée : long run, best effort, parité sans affaiblissement des contrats et preuves, mutualisation prioritaire, réutilisation et absence de surconception.
- Distribution WSL2 dédiée dont le disque virtuel sera situé sur I:, sans modification des distributions existantes.

## Aperçu des tâches à finaliser par la préparation

1. Contrat de parité, candidat intégré et prérequis WSL/CI.
2. Distribution WSL2 dédiée et qualification de la toolchain Linux.
3. Profils/presets communs et qualification CPU, corrections de portabilité bornées.
4. Portage ASan et réutilisation des contrôles sain/faute et de normalisation d'environnement.
5. Qualification OpenGL logiciel et pixels via Xvfb/Mesa.
6. Reproduction en CI propre, avec les mêmes commandes et artefacts de preuve.
7. Intégration, non-régression Windows, revue ciblée et clôture documentaire.

Cet aperçu n'est pas un résultat PM validé ni un board publié. Les fiches exécutables devront préciser ownership, contexte fresh/fork, ressources et dépendances ; les consommateurs attendront l'intégration réelle de leurs entrées.

## Recommandation et limites

Conserver la QA proportionnée : suites nécessaires à la parité, canari ASan, contrôles négatifs pertinents, comparaison des populations et pixels existants sans assouplissement. Pas de nouveau framework QA, de benchmark ou de qualification GPU matérielle implicite.

Attendre la restitution de la préparation existante pour finaliser les tâches. Seul le `/ensemble-mission start` demandé par l'utilisateur peut approuver la révision présentée. Toute correction ultérieure de politique exige persistance et nouvelle présentation par `config`.

## Traçabilité

- Hash de proposition native fourni : `2810ec4cc911a66d72aae53e0a7c7b5c7af5ec5d27b4a73950afa86b5d85aeef`.
- Session de préparation existante : `ses_e7cdc3fa6ffeaCOmqx1gpRuA0J`.
- Contrôles locaux précédents : [corrections OpenAI et autonomie](01-corrections-openai-et-autonomie.md). Aucun nouveau build, installation ou test Linux effectué pour cette présentation.
