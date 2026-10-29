# QA Linux — remise finale récupérée, plan accepté avant auteurs

## Problème résolu et limites

Le qa-planner est devenu inactif sans remise récupérée automatiquement par Ensemble. Une inspection native en lecture seule a retrouvé sa réponse finale complète. Le plan a été relu et accepté avec adaptations bornées du superviseur, puis persisté à `.opencode/evidence/mission-linux-qa/qa-plan.md`. Le commit doit être vérifié avant publication WSL.

Aucun agent relancé, droit élargi, profil modifié, résultat de test inventé ou changement de base interne. La récupération manuelle ne répare pas le transport automatique d'Ensemble.

## Preuves de récupération et admission

- L'exécutable CLI natif a été localisé à partir des chemins des processus OpenCode : il existe, mais n'était pas dans le PATH de la première tentative. Aucune installation/réparation de CLI.
- API session GET : parent superviseur ARTIST attendu, agent qa-planner, modèle OpenAI, répertoire ARTIST, outcome succeeded ; permissions de session vides (pas de grant ajouté). Cela n'est pas une preuve d'enforcement exhaustif du profil.
- Découverte de l'OpenAPI live, puis GET des messages et de leur contenu : réponse publique finale `msg_1835f8abd001FOG8O48ZL0wzeB`, assistant, finish stop, 12 837 caractères, session `ses_e7ca47812ffeHh4wj56IDQaTVO`.
- Seules les métadonnées nécessaires et parties texte publiques ont été traitées ; raisonnements privés et corps bruts d'outils non restitués.
- Lectures locales effectivement rapportées, message de progression effectivement reçu et texte final effectivement récupéré. Le lead accepte le livrable réel, pas un statut de transport.

## Relecture et décisions du superviseur

Le plan couvre les obligations, scénarios nominaux/négatifs, top3 risques 20/20/16, traçabilité bidirectionnelle, budget de préparation et critères de sortie. Il distingue référence 506 CPU + 75 GL du candidat courant et exclut UBSan/coverage/Release/fuites exhaustives/GPU natif de la première livraison.

Adaptations explicites : mutualisation classée obligation utilisateur et non simple inférence ; pas de mutations exhaustives ; QA proportionnée dans les exigences obligatoires du projet ; aucune attente d'autorisation utilisateur supplémentaire pour les choix ordinaires délégués. Le choix Ubuntu24.04/GCC14 et Mesa logiciel reprend le périmètre approuvé, tout en conservant leurs sondes réelles.

Le plan est une proposition du qa-planner réconciliée/persistée par le lead. Il ne constitue aucun PASS Linux. Les écarts ASan/presets sont des entrées de correction, pas un RED moteur exécuté.

## Prochaine barrière

Commit limité au plan et au présent rapport, sans incorporer les modifications préexistantes de profils/harness/docs/GrepAI. Après vérification du commit : clôture réelle de la tâche de contrat par le lead, puis publication explicite de la seule tâche WSL. Tous les autres consommateurs restent deferred ; build et ASan ne partiront qu'après WSL intégré/qualifié. Pas de nouveau GO de jalon.

L'audit reste conditionnel : son contrat hors board et la restitution native devront être démontrés au bon jalon. La récupération manuelle réussie de ce qa-planner ne qualifie pas automatiquement les autres profils ou leur transport.
