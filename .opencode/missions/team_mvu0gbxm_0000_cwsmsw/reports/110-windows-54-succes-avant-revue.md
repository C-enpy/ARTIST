# 110 — Déclarations Windows vertes, revue avant intégration partielle

Le coordinateur a relu le petit correctif : un constructeur privé de six valeurs
est appliqué à chaque invocation matérielle, avec identités distinctes pour les
workloads, la découverte et l'exécution CTest. CPU conserve son environnement
décrit inchangé ; aucune lecture ou réparation depuis le parent n'est ajoutée.

Les **7/7 régressions passent**, puis **54/54 tests Windows**, sans échec, erreur
ou skip. Tests, fixtures, runner et PowerShell sont inchangés depuis le RED ;
empreintes stables. Ces tests prouvent les descriptions et les reçus synthétiques,
pas encore la combinaison avec le runner commun corrigé dans l'arbre principal.

Une revue indépendante fresh de l'adaptateur complet et des seuls paramètres
privés est publiée avant intégration partielle. Le helper fonctionnel reste fermé :
son ouverture attend son propre mapping, tests de fragments et RED avant code.
Les contrôles antérieurs du corps PowerShell inchangé ne sont pas relancés pour
ce seul delta Python. Le candidat demeure stable et non intégré en attendant avis.

## Preuves

- Candidat `a0d3251d3f84754a8712e2dcc2f10fb424b2db68`, auteur pool-23 ; runner coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest -v test_hardware_binding_transport`, puis modules `test_adapter_binding test_binding_helper_interface test_ctest_cache_binding test_hardware_binding_transport`, cwd tests Windows du worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/windows-hardware-transport-green-2026-10-30T07-24-37-121Z/`.
- Revue `task_mvundor3_005w_ayxb7i`, fresh, lecture seule ; PM/LLM68/71.

Aucun processus, PowerShell, outil natif, moteur, OpenGL, rendu, benchmark,
installation ou cloud exécuté. Le cache identifie ses déclarations, pas le binaire
CTest lui-même ; qualification native et GPU NON_EXECUTE.
