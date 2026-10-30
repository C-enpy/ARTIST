# Windows — transport hardware : GREEN_READY, pas GREEN observé

## Candidat et autorité

Auteur pool-23, tâche `task_mvukzhsc_0052_f1bbfq`, départ propre
`76256132967b7133f57ce02f653ad0b9bf57d68c`, worktree
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-23`,
branche `ensemble-4b686588-c97c-46ab-99a1-e176a14b9184`.

Le lead autorise uniquement le GREEN minimal de windows.py et cette note ;
les54 tests/fixtures/assertions, PowerShell et les pins restent inchangés.
Pas d'import cible, test, programme factice ou outil natif par l'auteur.

RED observé par le lead sur7625613 :7 tests,11 failures avec sous-cas, zéro
error/skip, empreintes stables. Manifeste et stderr relus via Context Mode :
candidat/cwd/commande concordent, statut1, error:null, `Ran 7 tests in 0.771s`,
`FAILED (failures=11)`. Logs
`out/verification-synthetic/windows-hardware-transport-red-2026-10-30T07-18-03-822Z/`.
Cause : six champs explicites absents des workloads et de la découverte CTest,
pas un import/scaffold/setup. CTest sélectionné et CPU étaient les contrôles verts.

## Correction privée bornée

Un seul builder `_hardware_environment(plan, build, request_id)` renvoie :

- `{}` hors profil opengl-hardware ; CPU garde son delta vide.
- En hardware, exactement six champs : profil, run_id, plan_sha256, request_id,
  vendor et renderer attendus. Cible exacte absente : ValueError avant assemblage
  des invocations/cache ; aucune variable parent ne répare la demande.

Il est consommé pour CHAQUE invocation :

1. Workload : identité inchangée `<run>-workload-<index>`, strictement la même
   valeur que --request-id visible en argv.
2. Découverte CTest : `<run>-ctest-discovery-0`, distinct de chaque workload/run,
   répétable pour le même run, changeant avec run_id.
3. CTest sélectionné : identité existante `<run>-ctest-0` conservée.

La construction précoce du delta CTest sélectionné valide le target hardware
avant tout mapping/cache/commande, y compris pour un plan workloads-only. Ce
delta n'est utilisé comme commande CTest que si ce producteur est sélectionné.
Aucun état dernier-bundle, random, fichier ou effet ajouté.

Pas de lecture os.environ, suppression/forçage logiciel, None/vide de substitution,
nouveau schéma/API, modification de collector/core/runner ni changement d'argv
workload/CTest. Le cache reste une seule lecture bytes→SHA256→parse ; mapping des
exécutables et captures communes inchangés.

## Frontières conservées

Le runner commun ed952b45 annoncé GREEN attend sa revue indépendante/intégration.
Il n'est ni copié ni modifié ici. Cette remise déclare ce qu'il doit consommer ;
elle ne démontre PAS encore son assainissement effectif avec Windows. La preuve
end-to-end attend l'entrée commune réellement intégrée, puis revalidation/revue.
Pas de diagnostic GPU, renderer matériel ou faux PASS reproduit.

Helper fonctionnel toujours fermé ; scripts/build-windows.ps1, ses quatre
paramètres, WRAPPERS/NATIVE/DISPATCH/legacy et les anciennes51 assertions/pins
inchangés. Une future tranche helper aura ses propres mapping/fragments RED,
indépendamment du transport/runner. Aucun core/modèle/verify/C++/CMake/workflow.

## Commandes du lead — NON exécutées par l'auteur

Depuis le worktree auteur :

```powershell
Set-Location scripts/tests/verification_windows
C:/Python313/python.exe -B -m unittest -v test_hardware_binding_transport
C:/Python313/python.exe -B -m unittest -v test_adapter_binding test_binding_helper_interface test_ctest_cache_binding test_hardware_binding_transport
```

Attendu :7 puis54 méthodes satisfaites, sans failure/error/skip, empreintes
stables. Ceci reste un attendu GREEN, pas un comptage découvert/exécuté par
l'auteur. Le lead conserve nouveaux journaux exclusifs/commande/cwd/interpréteur,
comptages, bruts et SHA256 sous out/verification-synthetic. Pas de discovery
native globale, jonction06 ou lancement du helper complet.

## Matrice réelle de l'auteur

- Analyse des bruts RED existants via Context Mode, aucune réexécution.
- AST windows.py réellement accepté via Context Mode sous Python3.13.1 sans
  import projet : six clés littérales dans le builder, zéro référence os.environ
  dans le module. Ce contrôle statique n'exécute pas les comportements testés.
  Contrôles Git de confinement dans la remise. Aucun test, enfant factice ou
  PowerShell exécuté.
- Nouveau GREEN54, environnement effectif commun et non-régression : NON_EXECUTE.
- Windows/Linux natifs, toolchain/STL/ABI/SDK, moteur CPU/GL, rendu/GPU, benchmark,
  installation/cloud : NON_EXECUTE ; aucun gain ou qualification revendiqué.
- Aucun sous-agent/team tool/auto-claim/clôture ou changement de politique/modèle.

Remise GREEN_READY au lead pour revue/exécution puis STOP. Pas d'intégration ni
d'ouverture fonctionnelle du helper décidée par cette remise.
