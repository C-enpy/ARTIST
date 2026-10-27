# 01 — Accès shader préparé : politique OpenAI et autonomie proposées

Objectif : livrer le float OpenGL préparé selon le contrat déjà adopté, avec
consommateur existant, pixels et invalidation sûre. Étape : préparation uniquement.

## Changements et recommandation

Le brouillon initial autorisait quatre fournisseurs et ne déléguait aucune
décision long run. Le lead l'a corrigé à la demande de l'utilisateur : seul
provider openai, 11 paires modèle/effort déjà présentes, sans changer leurs scores,
coûts ni réserves ; autonomie long_run avec priorités tirées du mandat.

Continuer dans le périmètre sans nouveau GO à chaque jalon, mais conserver start
natif initial, permissions, contrat, QA, intégrations et pauses explicites.
Aucune source moteur/test/shader modifiée, aucun build/test/worker/classifier
lancé, aucune tâche publiée et aucun commit de préparation encore effectué.

## Catalogue réellement persisté

Les nombres suivants sont intelligence / coût estimatif par tâche du catalogue,
pas des probabilités de succès ni un devis ou des mesures du projet :

- openai/gpt-6.1-sol : low 42/0.13, medium 48/0.21, high 50/0.32,
  xhigh 51/0.39, max 52/0.72.
- openai/gpt-6-astra : low 46/0.82, medium 50/1.54, high 51/1.73,
  xhigh 52/2.31, max 53/3.26.
- openai/gpt-6-luna : high 33/0.03.

Seuil de planification conservé à 50. Catalogue et variantes de réserve conservés
selon le brouillon initial, pas d'affectation de modèle à la main ni de fallback
autre fournisseur. Disponibilité native exacte et politique doivent encore être
présentées/validées par config ; lire le JSON n'en prouve pas l'application.
Classification non activée ; estimations PM/LLM ARTIST conservées dans l'aperçu,
sans appel JEV ni changement de méthode de mission.

## QA proposée : proportionnée, avec obligations ARTIST

Recommandation : conserver proportionate. Vérifications ciblées par risque, TDD,
GL réel, pixels, ASan lifetime et revue indépendante utile sur les emprunts.
Le projet impose en plus plan QA avant auteurs, complément des gaps après
intégration et audit fresh hors board : ces obligations sont explicitement
conservées, pas supprimées par le libellé « aucune pipeline générique obligatoire ».
Pas de nouvelle infrastructure ni de campagne fichier/harness hors mission.

Options de niveau à discuter : light ne dispense pas de ces obligations ;
proportionate est recommandé ; reinforced demanderait un élargissement explicite
des vérifications, non supposé ni engagé. Aucune durée ou facture garantie.

## Aperçu du travail

Préparation/QA qualifiée et commit → auteur C++ accès/sûreté → intégration →
auteur OpenGL consommateur/pixels → intégration → revue sûreté et gaps parallèles
→ runner lead → audit fresh/corrections → documentation/clôture.
Deux propriétaires de code successifs, pas d'équipe ajoutée pour refaire les
enquêtes déjà reçues. Les barrières du lead ne sont pas des tâches factices.

## Résultats vérifiés, limites et décision attendue

HEAD relu 2c8e9f2 ; status ciblé moteur/tests/shaders/docs sans modification
préexistante dans ces chemins. Les diffs FOREIGN de profils/harness/.grepai restent
intacts. Les preuves historiques 555/555 ne sont pas une nouvelle campagne.

QA actuelle conditionnelle : lecture/restitution directe reçues, profil/permissions
effectifs non attestés ; refus team_message hors équipe connu, à ne pas répéter.
Admission avant auteurs reste une barrière. Pas de réparation native engagée.
Le raccord require_context/publication doit être vérifié via voie exposée ;
ne pas inventer de champs ni prendre une affinité pour une admission effective.

Décision attendue : après lecture du nouvel aperçu natif config, corrections en
chat ou start par l'utilisateur pour approuver cette version. Le lead ne lance
ni config/start en son nom ni les auteurs. Sans start, exécution toujours gelée.

## Pièces et provenance

- [Politique corrigée](../model-policy.json)
- [Contrat adopté](../contract.md)
- [Aperçu des tâches et ownership](../task-preview.md)
- [Plan QA conditionnel](../qa-plan.md)
- Enquêtes et remises architecte/PM/QA : sessions citées dans task-preview.md.
- Leçons : docs/references/session-2026-10-26-stable-reader.md et rapports
  historiques 05/11/16/33/86/96/97, lus pendant la préparation précédente.
- Proposition initiale annoncée : 6e9b9be20ab479261673856bf48eb7a5a5a94208d6a9d0bee3eb298c727cb916.

## Contrôles réellement exécutés par le lead

Context Mode ctx_execute JavaScript, lecture JSON et assertions : PASS local pour
provider openai unique, 11 paires uniques, intelligence/coûts conservés, long_run,
verification proportionate avec 5 commandes candidates/9 critères, seuil50 et
aucun classifier ajouté. Quatre documents de préparation lus, non vides et sans
espace final au moment de ce contrôle. Ce sont des contrôles de préparation,
pas une validation du schéma natif ni une exécution des commandes candidates.

git rev-parse --short HEAD : 2c8e9f2. git status --short ciblé ARTIST/test-datas/docs/
scripts et entrées CMake/Conan : aucune modification. Aucune source de production
ou test moteur modifiée. Pas de nouveau commit ni de lancement d'auteur.

SHA256 du JSON corrigé :
61f8a9f8977f8f2173acc40428fa4ccc6f987ef97969057adc7ad1dafbe22c99.

Présentation config native, disponibilité effective des modèles et admission QA
runtime : NOT_RUN/INCONCLUSIVE à cette étape. La lecture/validation locale ne les
remplace pas et n'approuve pas l'exécution.
