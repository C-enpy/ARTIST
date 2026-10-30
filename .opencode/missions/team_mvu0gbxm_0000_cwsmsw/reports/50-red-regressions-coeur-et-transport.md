# 50 — Régressions du cœur reproduites avant correction

## Résultats observés

Le coordinateur a relu les deux fichiers de tests puis exécuté leurs sept méthodes. Les six méthodes de régression donnent **8 échecs d'assertion avec sous-cas et un SKIP**. Le test de transport donne **2 échecs d'assertion**, sur les déclarations de contexte absentes et partielles. Aucun error ; empreintes stables et production inchangée depuis le candidat revu.

Les tests reproduisent le drainage sans borne, le rapport favorable malgré la contradiction entre manifeste et reçu, l'ajout de configuration ignoré, le mauvais répertoire Windows et le refus matériel intervenant après les commandes. Le transport reproduit la conservation indue des variables héritées. Tous les processus sont des doubles ; aucune attente réelle de descendant n'est nécessaire à cette preuve.

Le test de lien symbolique est non exécuté : Windows refuse sa création. Ce SKIP n'est pas un RED produit. Le défaut de confinement établi statiquement demeure ouvert et sa preuve runtime reste INCONCLUSIVE, sans modification de permissions.

## Suite autorisée

Le GREEN minimal des défauts reproduits est autorisé avec conservation des 132 tests et assertions. La correction minimale de contrôle avant append est également autorisée sur le finding statique de lien symbolique, sans prétendre disposer de son RED exécuté. Un ré-audit indépendant sera requis avant intégration.

Le refus précoce des anciens tests matériels reste provisoire jusqu'à intégration du nouveau producteur et du parseur de contexte. Aucun consommateur n'est autorisé à copier leur branche en cours. L'intégration du cœur reste bloquée jusqu'à correction, non-régression et ré-audit.

## Preuves

- Candidat : `797cc568978e936de4d81a8c94ee4396c0534358` ; auteur pool-3 ; runner coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_review_regressions.py -v`, puis même commande avec `test_context_transport_contract.py`, depuis le worktree auteur.
- Journaux complets et empreintes : `out/verification-synthetic/core-review-regressions-red-2026-10-30T02-39-25-367Z/`.

Aucun enfant réel, moteur, compilateur, outil de construction, OpenGL, rendu, benchmark, installation ou cloud exécuté.
