# Autorisation locale Luna OpenAI — 2026-10-18

Intervention locale de configuration autorisée séparément par l'utilisateur.
Ce consentement n'approuve pas l'exécution de la mission d'ingestion.
Plan local indivisible, PM/LLM : complexité brute/retenue 58/58, risque 42/42.
Configuration uniquement, aucun développement exécutable ni worker : le plancher
de développement ne sert pas à modifier ces scores. Aucun appel JEV.

## Cause et diff

Le catalogue OpenCode expose `openai/gpt-6-luna` avec `high`, mais la sélection
opérateur Ensemble héritée ne contenait Luna que sous `opencode-go`.
Le contrôle `assertMissionModelAllowed` compare exactement ID et effort.
Le brouillon de mission était déjà correct ; il reste inchangé.

`.opencode/ensemble.json` conserve autoDispatch/autoScale et ajoute un catalogue
projet : les 52 objets hérités, sans changement de champs/ordre, puis l'entrée
`openai/gpt-6-luna#high`. Le format compact limite la duplication textuelle.
Le remplacement complet du tableau est requis par la sémantique de loadConfig ;
ce snapshot n'héritera plus automatiquement des futures modifications du tableau
global. Ne pas le resynchroniser aveuglément.

Valeurs de Luna High reprises du catalogue utilisateur existant : intelligence33,
averageCostPerTask0.03, benchmarkCostPerTask0.03. Pas de nouvelle évaluation,
d'extrapolation d'effort, de prix réel OpenAI ou de test natif de qualité.
AccessMode unknown, coûts effectif/amorti absents. Déclaration structuredOutput
du modèle conservée avec sa date originale et strictVerification unverified.
La borne d'intelligence, beta et costMode restent hérités, non recalibrés.

Sauvegarde du fichier projet initial : `project-ensemble.before.json` adjacent.
Global non écrit ; SHA256 avant/après :
`ca7e0fd838f0d03dded1747731da64ee5948d6f2129ff9496f658caedd2eedc1`.

## Contrôles exécutés par le lead

Via Context Mode `ctx_execute(language: javascript)`, lecture JSON avec fs,
assertions node:assert/strict et empreinte node:crypto. Pour le contrôle du plugin,
extraction en mémoire des fonctions nommées du bundle installé
`20261016-mission-policy-5504fe8e8df8/index.js`, sans import/démarrage du plugin.

- Avant modification : light absent/indisponible ; standard et strong présents.
- Validation finale : validateModelRoutingConfig accepte le catalogue fusionné ;
  52 entrées profondément égales, 53 entrées au total, global et scheduling intacts.
- Prédicat operator-selection du plugin : refus exact light reproduit avec le
  catalogue avant, puis 3/3 groupes acceptés après ; Luna max reste refusé.
  Évaluation sans teamId : n'affirme pas une approbation de mission ni une levée
  de pause, ni la validation des éventuelles préférences SQLite propres à l'équipe.
- Routeur réel routeNormalizedModel, simulation pure avec paramètres hérités :
  0/0 → step-5-preview-free/default, seuil18 ; 50/50 → même modèle, seuil41.5 ;
  100/100 → Cursor Opus5.5Max, seuil58. Choix/seuils identiques avant/après.
  Ces simulations legacy ne remplacent pas la grille native approuvée de mission.

Incidents de diagnostic conservés : une erreur d'échappement du script a été
corrigée ; une sonde cherchait routeModel absent du bundle, puis a utilisé le
symbole observé routeNormalizedModel. Le premier contrôle partiel avait réussi
avant cette sonde ; la validation finale complète ci-dessus s'est terminée sans
erreur. Aucun incident n'est présenté comme RED comportemental du moteur.

Pas de test/build ARTIST, de demande provider ou de worker lancé. Le paquet local
est un bundle installé sans suite source : pas de gate build/typecheck du fork
revendiqué. Le rechargement effectif dans le service et start sont NOT_RUN.
Le code contient une lecture dynamique du routage, mais cela ne prouve pas sa
prise en compte par l'hôte actif. Faire représenter par config avant start.

## Suite humaine

`/ensemble-mission config`, puis seulement après vérification de la proposition,
`/ensemble-mission start` utilisateur. Toute pause opérateur distincte demeure.
Pas de modification globale, plugin, identifiants provider ou fichier moteur.
