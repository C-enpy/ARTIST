# 2 — Valider l'outil sans lancer la campagne

L'utilisateur a choisi « Valider le pipeline seul » : conserver la QA proportionnee et permettre les tests des nouveaux scripts sur donnees synthetiques et programmes factices. Les tests moteur, le rendu, les benchmarks et la location cloud restent exclus.

Ce choix permet de verifier les erreurs, selections, rapports, delais et garanties d'absence d'effets du pipeline avec des tests discriminants. Il ne prouve pas le fonctionnement graphique ou les performances du moteur sous Windows/Linux.

Le critere correspondant est persiste dans verification.acceptance de model-policy.json. Aucun modele, fournisseur, seuil ou niveau QA n'est change. Les obligations projet restent applicables. Le plan et ses dependances sont dans 01-proposition-pipeline-portable.md ; l'arbitrage d'execution qui y etait ouvert est resolu par ce choix.

Execution toujours en pause : ce choix de perimetre ne remplace pas l'approbation native. Prochaine etape : nouvelle presentation par /ensemble-mission config, puis /ensemble-mission start uniquement a l'initiative de l'utilisateur. Aucun worker Ensemble, build, test moteur ou test du pipeline n'a ete lance par cette decision.
