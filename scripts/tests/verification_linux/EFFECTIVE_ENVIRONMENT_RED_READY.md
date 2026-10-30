# Linux — environnement effectif TESTS_RED_READY

Auteur pool-24, reprise tests uniquement de `task_mvul0bd4_0053_ptc60a`.
**Aucun test/import candidat/processus exécuté par l'auteur.**

## Asymétrie intentionnelle du RED consommateur

- Worktree : `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-24`.
- Base production Linux conservée : `3e697590c03869f53b64aec8dde0cd706bceff52`.
- Runner ancien conservé sans mise à jour : dernier commit touchant ce fichier
  `107a2d3bbd88ba95a2eb70e904085911b71662bd`, blob Git
  `cfc88742b7bccca9bb868885451bcb91b7400ac9`.
- Entrée commune corrigée annoncée comme intégrée/vérifiée par le lead :
  `358921c42b83f5ae619c97fb8f7c1a66700ef4e6`, rapport109 au HEAD
  `ab488f7111c375658d5d5c75443b6de60f67d724` ; preuves attribuées au lead,
  non rejouées ici. QA §12 commité avant cette reprise (`b891105`).
- Contrat canonique `hardware-child-environment-v1.md` et QA §12 relus en
  lecture seule. Aucun merge/copie de core/main/autre branche dans ce worktree.

Le lead observe d'abord le RED du nouveau test sur ce runner ancien. Après
relecture/revue, intégration **native des seuls nouveaux fichiers Linux** sur
l'arbre corrigé du lead puis GREEN par le lead. Ne pas fusionner ce worktree en
réintroduisant son ancien runner. Le RED ne signifie donc pas que l'entrée
commune désormais intégrée serait encore défectueuse.
Le GREEN doit attribuer explicitement la paire réelle adaptateur Linux candidat
`3e697590` + runner corrigé `358921c4`, assemblée/revue par le lead selon sa barrière
d'intégration. Un adaptateur manquant ou une erreur d'import ne vaut aucun verdict.

## Test proposé sur disque

`test_effective_linux_environment.py` : quatre tests déclarés, avec sous-cas :

1. CPU parent neutre : marqueur conservé, contexte hardware seulement ambiant
   supprimé, aucune activation matérielle ni ajout de variable GL.
2. CPU parent forcé, chaque variable puis paire : héritage inchangé conformément
   au contrat commun ; aucun nettoyage global imposé au profil CPU.
3. Logiciel explicite, parent neutre puis contradictoire : environnement effectif
   `LIBGL_ALWAYS_SOFTWARE=true` et `GALLIUM_DRIVER=llvmpipe`.
4. Hardware, chaque variable puis paire, pour workload, découverte et run CTest :
   **deux clés absentes**, six valeurs déclarées exactes, marqueur ordinaire et
   DISPLAY synthétique conservés, parent intégralement intact. Aucune substitution
   par None/vide/false ni valeur de driver devinée.

Vrais adaptateur/plan/reçus/runner, helpers purs `plan_for`/`receipt_for` et gardes
`guard_processes` existants inchangés. Forme locale minimale du double
`OwnedProcess`/factory de `verification_core/test_context_transport_contract.py`,
avec écriture dans les captures possédées comme les tests capture existants ;
aucun nouveau harness partagé/framework. La factory inspecte `kwargs['env']`
effectif, pas seulement le delta, et refuse tout argv/cwd/options inattendus.

## Confinement

- Tous parents entièrement synthétiques : `patch.dict(os.environ, parent,
  clear=True)`. Comparaisons complètes seulement dans cette portée fermée ; aucun
  snapshot réel de l'environnement ni secret dans les assertions/journaux.
- Popen/run/call/check_*/spawn/shell/socket interdits par sentinelles existantes.
  Seule factory injectée, objet en mémoire, wait fini, sans pipe/enfant réel ou
  factice. Code0 signifie uniquement restitution du faux primaire, pas test GPU.
- Les vrais fichiers de capture du runner sont redirigés vers la fixture
  temporaire possédée avec espaces ; options mode/delete/buffering conservées.
  Marqueurs stdout/stderr minuscules ; vérification de fermeture/suppression.
- Aucun collecteur de versions, helper, Conan/CMake/CTest réel, moteur, GL,
  serveur X, installation/cloud ou producteur/parser de résultat GPU appelé.
- Production Linux/runner/core et tests/fixtures 34+6+19 strictement inchangés.

## Commande réservée au lead, non exécutée par l'auteur

Dans ce worktree, après revue de source et capture de HEAD/delta/SHA256,
via Context Mode avec nouveau dossier de logs :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p 'test_effective_linux_environment.py' -v
```

Attendu sur l'ancien runner : quatre tests découverts ; baselines CPU/logiciel
favorables, échecs causaux hardware sur les clés héritées (neuf sous-cas).
Le nombre réel, les erreurs/skips et la première cause sont à constater par le
lead ; aucune preuve RED exécutée revendiquée ici. Après intégration des seuls
fichiers nouveaux sur le runner corrigé, même commande, attendu quatre succès
et zéro échec/erreur/skip, puis non-régression/revue requises par le lead.

## Matrice et limites

- Auteur Windows/Python3.13.1 : lecture source/Git et AST sans import seulement,
  code0, quatre tests déclarés. Script exact passé à
  `C:/Python313/python.exe -B -c` via Context Mode, cwd worktree ci-dessus :

  ```python
  import ast, pathlib, sys
  path = pathlib.Path("scripts/tests/verification_linux/test_effective_linux_environment.py")
  tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
  count = sum(isinstance(node, ast.FunctionDef) and node.name.startswith("test_") for node in ast.walk(tree))
  print("AST_OK", path.as_posix(), "tests_declared=" + str(count))
  print("AST_ONLY Python", sys.version.split()[0], "no candidate import or test execution")
  ```
- Tests synthétiques nouveaux : **NON_EXECUTE par auteur**, statut TESTS_RED_READY.
- Linux natif/GL/renderer matériel/compilation/mesure : **NON_EXECUTE**.
- F1 avait été constaté statiquement ; ces tests ne prouvent aucun faux PASS GPU.
  L'assainissement de deux variables ne qualifie pas un renderer ni tous les
  autres mécanismes externes de sélection du driver.

Arrêt après remise commitée, avant toute exécution ou modification de production.
