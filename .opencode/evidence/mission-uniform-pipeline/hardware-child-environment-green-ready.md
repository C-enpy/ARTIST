# Environnement enfant matériel — GREEN_READY

Auteur pool-26, tâche `task_mvumhg9g_005j_81env1`, 2026-10-30.
**Correctif candidat pour revue lead, GREEN NON_EXECUTE par l'auteur.**

## Entrée et RED fourni par le lead

Entrée vérifiée propre : `eefb1fdda5e06e581bdebcb74df7ce80c3e86f8a`, branche
`ensemble-56a896a3-6298-423c-bfa5-014fa7983cb2`, worktree
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-26`.
QA §12 et contrat privés présents depuis `b891105`, antérieurs à l'auteur.

Message lead autorisant GREEN minimal : RED sur cette entrée, **10 tests,
33 failures avec sous-cas, 0 error/skip**, empreintes stables, production et
anciens194 inchangés. Journaux fournis par le lead :
`out/verification-synthetic/hardware-child-environment-red-2026-10-30T07-13-08-668Z/`.
Causes rapportées : forçages parent transmis, ValueError attendu absent et factory
atteinte pour commandes invalides ; pas import/outil/environnement manquant.
Preuve attribuée au lead, pas exécution ni audit des logs par l'auteur. Certaines
assertions aval étaient masquées : aucun audit individuel exhaustif revendiqué.

## Correctif borné

`scripts/verification/runner.py::SubprocessRunner.run` uniquement, dix lignes
ajoutées après copie parent/nettoyage des six contextes ambiants et avant
application du delta :

1. Déclenchement seulement si le sélecteur explicite du delta vaut exactement
   `opengl-hardware`.
2. Chaque champ de transport doit être explicitement non vide ; sinon ValueError
   nommant la clé, avant acquisition de captures et avant factory.
3. Présence d'une des deux clés de forçage dans le delta refusée quelle que soit
   sa valeur, par ValueError nommant la clé.
4. Les deux clés sont retirées de la copie enfant puis le delta est appliqué.

Aucune validation sémantique hash/vendor/renderer, aucun fallback ou choix de
driver. CPU/logiciel/sélecteur absent/autre/ambiant ne déclenchent pas cette garde.
Aucune mutation parent ni valeur None/vide de substitution. Schémas, captures,
causes, lifetime primaire, adaptateurs, cache, CMake, PowerShell et C++ inchangés.
Les dix méthodes, fixtures et assertions du nouveau test restent strictement
identiques à l'entrée RED, ainsi que tous les anciens tests.

## Contrôles auteur réellement effectués

- Windows : analyse AST sans import du runner ni du test, **PASS syntaxique** ;
  dix méthodes de test toujours présentes. Python3.13.1 du sandbox Context Mode,
  `C:/Python313/python.exe`. Lectures absolues dans le worktree ; cwd retourné
  par le sandbox : `C:/Users/djden/source/repos/ARTIST`.
- SHA256 runner :
  `99db8ad1a196badcda78678c8bee321eeff7c0da6443a821c06e0fe0aa5fd86d`.
- SHA256 nouveau test inchangé :
  `8caaeaa342177d5d2711c153d051af59913cfe186e558ca22c6b5bf34bdd9383`.
- `git diff --check` et comparaison de périmètre réservés aux fichiers de cette
  remise ; aucune exécution unittest/import cible/processus factice par l'auteur.
- Windows GREEN/non-régression : **NON_EXECUTE**. Linux : **NON_EXECUTE**.
  Aucun outil moteur/natif, build, Conan/CMake/CTest, GL, rendu, benchmark,
  installation, cloud ou sous-agent ; aucune nouvelle base compile_commands.

Analyse AST exacte transmise à Context Mode (`language=python`) :

```python
import ast, pathlib, hashlib, sys
r = pathlib.Path('C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-26')
for relative in ('scripts/verification/runner.py', 'scripts/tests/verification_core/test_hardware_child_environment_contract.py'):
    p = r / relative
    b = p.read_bytes()
    t = ast.parse(b, filename=str(p))
    print(relative, 'AST_ONLY_NO_TARGET_IMPORT: PASS', 'sha256:', hashlib.sha256(b).hexdigest())
    if relative.startswith('scripts/tests/'):
        print('test_methods:', sum(isinstance(n, ast.FunctionDef) and n.name.startswith('test_') for n in ast.walk(t)))
print('interpreter:', sys.executable)
print('version:', sys.version.split()[0])
print('sandbox_cwd:', pathlib.Path.cwd())
```

## Commandes proposées au lead, non exécutées par l'auteur

Cwd : worktree ci-dessus ou intégration vérifiée du même candidat, identités
conservées. PowerShell, exécution séquentielle :

```powershell
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_core -p 'test_hardware_child_environment_contract.py' -v
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_transport_contract.py' -v
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_core -p 'test_file_capture_contract.py' -v
```

Attendu : dix nouveaux tests passent sans skip, factory strictement en mémoire,
refus avant lancement et phase FAILED/code4 ; transport/capture existants restent
verts. Ce sont des attentes, pas des résultats. La suite204 core/parser et la
revue indépendante fresh restent au lead après sa relecture. Consommateurs
Linux/Windows en attente d'une entrée intégrée vérifiée, aucune fusion native ici.

## Localisation / impact / limites

Correction limitée à la construction de l'environnement dans runner.py, avant
toute capture/process ; aucun contrat public nouveau. Lecture ciblée du worktree,
Serena canonique C++ seul non applicable à Python comme déjà consigné RED_READY.
Pas de preuve de renderer matériel ni qualification plateforme ; livraison
candidate seulement. ADR0001/2/4/6/7 Proposed,0003/5 Accepted/partial,0008 baseline
Windows Debug restent inchangés.
