# 79 — Windows : 51 tests verts, réception intermédiaire en revue

## Résultats vérifiés

Après lecture du correctif V2, le démonstrateur textuel échoue d'abord à distinguer vide et null avec sa comparaison ordinale. Une micro-expérience fixe montre le comportement de liaison PowerShell : `StringComparer.Ordinal.Equals("", null)` donne True dans cet appel, tandis que `Object.Equals` et `ReferenceEquals` donnent False. Aucune mutation d'environnement n'est effectuée.

Le coordinateur corrige uniquement le snippet diagnostique exécuté en remplaçant la comparaison par Object.Equals ; le démonstrateur distingue alors exactement vide/absence et changement de casse. Aucun fichier produit, fixture ou assertion Python n'est changé par cette correction. Le propriétaire reçoit la correction textuelle à persister dans sa documentation.

La séquence autorisée passe ensuite : **auto-test 1/1, méthodes natives 13/13, contrôles précédents 37/37**, soit **51 tests satisfaits**, sans échec, erreur ou skip. Empreintes stables. Les mêmes assertions comparent exactement les dictionnaires parent avant/après. Le cas de jonction est toujours NON_EXECUTE, pas un skip ajouté.

## Réception et suite

Une revue indépendante fresh est ouverte pour les fondations PowerShell intermédiaires, notamment transport, quoting, environnement, erreurs et ownership du processus factice. Le helper complet reste fermé ; ces tests ne qualifient ni MSVC, ni les vrais outils ou workloads.

La capture privée du helper ne participera pas au run commun ratifié, qui doit utiliser le runner Python et les exécutables admis. Sa limite Dispose/lecteurs face aux writers hérités reste explicite ; aucun nettoyage général de descendants ou délai absolu n'est démontré. Prepare/build et l'adaptateur attendent encore leur entrée core/QA intégrée.

## Preuves

- Candidat vérifié `78c91d698be97b7d38ac780971924e0c246ee5ac`, auteur windows-pipeline ; correction du démonstrateur et runner réel : coordinateur.
- Premier démonstrateur arrêté : `out/verification-synthetic/windows-observation-v2-green-2026-10-30T04-33-22-670Z/`.
- Démonstrateur corrigé, commandes unittest exactes, traces et empreintes : `out/verification-synthetic/windows-observation-v2-object-compare-2026-10-30T04-34-29-285Z/`.
- Sélection native explicite hors06 ; puis `C:/Python313/python.exe -B -m unittest -v test_psbase_audit test_targeted_contracts test_wrappers test_dispatch test_helper_contract`, cwd tests Windows.
- Revue publiée `task_mvuhccyg_003w_5s1040`, correctness-reviewer fresh sans worktree d'écriture, évaluation PM/LLM de revue 68/70.

Aucun helper complet, MSVC/vcvars, outil moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
