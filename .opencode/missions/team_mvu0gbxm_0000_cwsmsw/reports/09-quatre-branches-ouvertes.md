# 9 — Quatre branches ouvertes apres integration du plan QA

## Resultat et objectif

Le contrat et le plan QA sont integres ; le brief commun auteurs est commite au HEAD 1fba1c98e3a9750f615deacf37a08eb99d4b4fa6. Quatre auteurs travaillent maintenant dans des worktrees distincts pour livrer l'outil Windows/Linux, sans campagne moteur.

- Orchestration commune : pool-3, profil cpp-engineer, branche core, admission par auto-dispatch.
- Adaptateur Windows : windows-pipeline, profil build-engineer.
- Adaptateur Linux et presets : linux-pipeline, profil build-engineer.
- Scenarios : uniform-scenarios, profil gpu-engineer OpenGL.

Les trois spawns explicites ont ete executes un par un. Ensemble confirme openai/gpt-6.1-sol high pour ceux-ci ; aucun modele, variante, score ou provider n'a ete force au spawn. Chaque tache conserve son ownership et sa completion au lead. Les trois consommateurs integration/gaps/audit restent differes.

## Travail et preuves attendus

Chaque auteur commence par son plan borne et ses tests synthetiques. Le lead relit les tests et observe un RED causal avant implementation, puis GREEN/non-regression. Aucune toolchain absente, aucun module d'une autre branche encore manquant ni aucun compteur zero ne sera rebaptise RED utile.

Les scenarios C++/GL seront livres en source et raccordes, sans compilation/execution ni benchmark. Les tests autorises n'emploient que donnees synthetiques, doubles injectes et processus factices. Aucun helper reel, CMake/Conan/CTest, moteur ou cloud. Les composants cross-branches ne sont pas copies : si une interface runtime manque, sa vraie dependance est signalee et le lead gere l'integration.

Les auteurs disposent du contrat, du QA et du brief dans leur HEAD de depart. Les modifications preexistantes des autres profils et rapports ne sont pas dans leurs branches. Le plan QA natif et son complement sont traces ; seuls leurs textes publics ont ete recuperes pour persistance.

## Limites

Les sources des producteurs ne sont pas encore integrees. Aucun test execute et aucun PASS du pipeline n'est annonce a ce stade. Aucun nouvel accord utilisateur n'est requis pour poursuivre les etapes autorisees.
