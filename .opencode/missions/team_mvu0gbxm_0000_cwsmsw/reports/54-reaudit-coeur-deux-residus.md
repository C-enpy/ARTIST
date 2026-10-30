# 54 — Deux résidus empêchent encore l'intégration du cœur

## Situation et conséquence

Le ré-audit indépendant signale deux problèmes persistants sur le candidat corrigé : des lecteurs et pipes Windows peuvent survivre au retour du runner, avec perte possible de sorties partielles ; le rapport ne détecte pas tous les nouveaux fichiers pertinents de l'arbre de construction. L'intégration reste bloquée sur ces axes, sans annuler les 130 tests réussis ni assimiler leurs deux SKIP à des succès.

Le coordinateur a demandé une remise détaillée des mécanismes et scénarios discriminants du reviewer. La notification reçue ne suffit pas à préciser toute la preuve de perte des sorties Windows. La lecture confirme que le runner laisse volontairement ouverts les streams dont les lecteurs sont encore vivants : le délai de restitution ne démontre donc pas leur retrait.

La lecture recoupe le second défaut : le rapport rescane les sources pour détecter des ajouts, mais pas les fichiers binaires/configurations recensés dans le snapshot de construction. Une nouvelle bibliothèque pertinente ou un deuxième exécutable correspondant à une cible peut échapper à cette vérification de population.

## Suite bornée

Le propriétaire prépare des tests de régression de rapport sur un bundle factice réussi, puis un ajout de fichier ou une ambiguïté de cible, sans relance ni écriture du rapport. Pour les lecteurs Windows, il doit proposer un test discriminant de propriété des ressources et des données disponibles, avec doubles, après précision du reviewer. Aucun nouveau parent/descendant réel ou processus fuyant n'est autorisé.

Aucune correction GREEN anticipée ni modification des 132 tests acquis. Les autres branches indépendantes continuent. Il n'est pas nécessaire de changer l'objectif ou de demander une nouvelle autorisation utilisateur pour ces corrections locales.

## Références et limites

- Avis complémentaire `result_mvuds0a4_0000_81pyqa`, profil correctness-reviewer ; aucune exécution par le reviewer.
- Candidat `8ecb5675825b551ee6beede171ecf48e8351f48b`.
- Code recoupé : `runner.py:107–116`, `evidence.py:362–377,408–420`, `reporting.py:134–146`.
- Derniers résultats exécutés : rapport 52, 132 découverts, 130 satisfaits, deux symlink non exécutés.

Aucun test supplémentaire, moteur, compilation, rendu, benchmark, installation ou cloud exécuté à cette étape.
