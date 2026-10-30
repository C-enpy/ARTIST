# 64 — Le GREEN Windows échoue, diagnostic ciblé demandé

## Résultats exécutés

Après retrait du quota et revue du nouveau pin, l'auto-test du harnais passe. Les treize méthodes autorisées suivantes donnent **14 échecs d'assertion avec sous-cas**, aucun error ou skip. Les empreintes contrôlées restent stables ; tests et fixtures sont inchangés. Les 37 contrôles précédents ne sont pas relancés après cet échec.

Une première invocation du script d'analyse a échoué en syntaxe JavaScript avant tout test ; elle a été corrigée dans la session et n'est pas comptée comme RED produit.

## Symptômes et hypothèses

La capture refuse l'objet fourni comme déjà associé à un processus. Le contrôle lit une propriété adaptée PowerShell ; une lecture retournant null sans exception pourrait suffire à déclencher ce refus erroné. Le marqueur du harnais « vivant après appel » ne prouve donc pas, à lui seul, qu'un enfant a réellement démarré ou fuit.

Une variable d'environnement vide devient absente sur les chemins nominaux de flags, payload et construction de StartInfo, mais reste vide sur les rejets et dans l'auto-test. Comme les chemins de flags ne manipulent pas l'environnement, la cause n'est pas attribuée au produit à ce stade. Une expérience minimale doit distinguer retour de fonction, traitement PowerShell et harnais avant toute correction de ce symptôme.

Les assertions sont conservées. Aucun dump d'environnement utilisateur, nouvelle permission, jonction ou relance plus coûteuse n'est proposé. Le propriétaire doit fournir une hypothèse et une expérience discriminante sur les seuls marqueurs factices ; le coordinateur reste runner unique.

## Preuves et portée

- Candidat `90c26f8ff99cd809b487a8c53c31f7d70f134891`, source `80f5a2f4f4f28be5cf0c7113e55a2ba8630ae87d`.
- Auteur windows-pipeline ; runner coordinateur. Auto-test isolé puis treize noms unittest explicites, commandes et cwd complets dans le manifeste.
- Preuves : `out/verification-synthetic/windows-native-selected-green-2026-10-30T03-21-46-777Z/`.
- Données brutes : worktree, `windows-native-fixtures/20261030T032147Z-f26450d69ecd49319537a762b722e69a`.

Cas reparse toujours NON_EXECUTE ; helper complet, MSVC, moteur, compilation, OpenGL, rendu, benchmark, installation et cloud non exécutés. Aucun GREEN ni réception Windows complète accordé.
