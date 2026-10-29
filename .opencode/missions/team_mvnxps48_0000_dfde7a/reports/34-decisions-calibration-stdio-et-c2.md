# 34 — Décisions opérateur : calibration stdio et fermeture motivée C2

2026-10-25. Réponses explicites au questionnaire précédé du checkpoint33.

## B3 : investigation autorisée, pas correction Reader

Choix opérateur : **Calibration stdio**. Autorisés : calibration de l'erreur native et de la compatibilité texte/chemins, recherche OS bornée d'un échec de fermeture authentique en parallèle. Plan QA final commité et contrat de sonde avant auteurs ; runner lead, aucune installation/service/dépendance nouvelle.

Le contrat Reader demeure proposé, non adopté. Aucune correction Reader, suppression/substitution de catch, abandon de l'obligation close ou seam artificielle autorisés par cette réponse. Un échec d'expérience sera conservé, pas transformé en RED moteur. Protocoles et commandes exactes validés avant essais ; pas de répétition sans hypothèse nouvelle.

## B5/C2 : fermeture motivée explicitement acceptée

Choix opérateur : **Fermeture motivée** selon le rapport33 et la provenance du rapport28. L'enquête B5/C2 est donc close administrativement avec disposition motivée acceptée : défaut historique de préparation de fixture établi, explication forte pour Empty et probable pour MultiBlock ; pas de causalité exhaustive ni verdict de correction moteur.

Les XML primaires, valeurs complètes MultiBlock et identité binaire pré-run manquants restent des limites archivées, non des preuves obtenues. Réouverture sur primaire contradictoire ou nouveau symptôme identifié. Aucun effacement des FAIL historiques, aucune nouvelle chasse identique ou reconstruction actuelle prétendant restituer les binaires perdus.

## Suite dans le contrat existant

Finaliser et commiter l'amendement ciblé de QA17 et le contrat de calibration ; publier seulement les branches prêtes avec ownership séparé (sonde / recherche OS close). Aucun auteur Reader avant adoption distincte du contrat et RED causal. B1 conserve ses preuves et attend sa revue finale. B2/B4/B6 restent acquis par identité ; aucun PASS global ni nouveau verdict ASan/GPU/GCC/Release/image/performance.
