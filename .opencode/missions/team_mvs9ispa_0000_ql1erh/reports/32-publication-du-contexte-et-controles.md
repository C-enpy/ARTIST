# Publication du contexte local — contrôles avant main

L'utilisateur autorise la publication des changements restants hors `.tmp/`.
Le scan initial porte sur107fichiers,1,1Mo : aucun exemplaire de la clé locale
connue ni préfixe de clé privée/GitHub trouvé hors configuration active. Ce scan
limité n'est pas une garantie exhaustive.

La configuration GrepAI active contient un identifiant réel. Elle sera conservée
sur disque et retirée du suivi, conformément aux ignores déjà préparés ; seul
l'exemple sans secret sera publié. Les deux temporaires restent locaux.

Les changements ne sont pas uniquement du texte : les validateurs QA doivent
être contrôlés. Baseline réelle :594tests Node,589PASS,5FAIL,0skip. Cause bornée :
les trois profils ont déjà la communication v2, mais validateur/oracles ne
l'admettent que pour le planner. La publication est suspendue jusqu'à correction
cohérente et revue, sans ouverture de droits ou affaiblissement des assertions.

Le plan QA indépendant est reçu et adopté dans
`.opencode/evidence/context-publication/qa-plan.md`. Le lead garde le mandat local
indivisible de raccordement et publication (PM/LLM C70/R60), avec RED ciblé avant
correction, non-régression, puis revue indépendante fresh. Aucun nouveau moteur,
modèle, profil ou démarrage de mission Ensemble.
