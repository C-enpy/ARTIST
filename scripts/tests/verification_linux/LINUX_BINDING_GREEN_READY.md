# Linux intégré — GREEN_READY, pas GREEN exécuté

Auteur pool-24 / `task_mvul0bd4_0053_ptc60a`, 30 octobre 2026.
Worktree `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-24`,
branche `ensemble-e15edd1f-f0b4-477a-9077-9a6445817f53`.

## Entrée RED et autorisation

- Base auteur inchangée avant GREEN : `678b21502f6d993ed2bc5d9244caa63d9d4adc18`,
  descendant du HEAD demandé `8c7e51b` et des fondations Release `d2d7be7`.
- RED **exécuté par le lead**, pas par l'auteur : 34 tests, 31 échecs avec
  sous-cas, zéro erreur/skip, empreintes stables. Première cause utile reçue :
  descriptions vides/scaffold, préparation sans commande et flags positifs
  absents ; pas d'ImportError ni d'outil réel. Certaines assertions aval étaient
  masquées ; elles ne sont pas présumées déjà exercées individuellement.
- Preuves lead : `out/verification-synthetic/linux-binding-red-2026-10-30T06-35-03-147Z/`,
  rapport97 `97-red-raccordement-linux-et-surface-helper.md`.
- Surface ratifiée relue dans l'arbre canonique :
  `.opencode/evidence/mission-uniform-pipeline/linux-adapter-binding-v1.md`,
  commit lead annoncé `6540ee642c1cd0bc3801b0dd37e8da6d1c45c870`.
  Aucun checkout/cherry-pick/copie de cette branche ou du brouillon pool-19.
- Le lead a explicitement autorisé le GREEN minimal ; auteur toujours non runner.

## Delta borné

1. `scripts/build-linux.py` : nouveaux flags ratifiés uniquement. Build-only
   exige des cibles explicites, déduplique en conservant l'ordre et ne fait
   qu'un appel build : aucun Conan/configure/CTest/canary/log implicite.
   Validation lexicale d'un nom de cible unique, sans shell/split. Le flag
   verification-workloads exige configure-only et ajoute uniquement
   `-DARTIST_BUILD_VERIFICATION=ON`. Défaut, configure-only historique, ordre
   Conan2 host/build avant CMake et garde Release+ASan conservés.
2. `scripts/verification/adapters/linux.py` : remplace le scaffold par les
   descriptions pures. Aucun accès filesystem/environnement ou sonde dans le
   constructeur ou les descriptions ; aucun cache du dernier bundle.
   Prepare déclare la résolution de dépendances, build n'en déclare pas.
   Reçus completed/plan/source/build-root contrôlés, puis chemins pris dans
   le mapping admis, sous l'arbre core Linux de la configuration sélectionnée.
   Le core reste l'autorité d'admission des octets/identités, leases et captures.
3. Run direct avec le vrai builder de protocole, IDs déterministes issus de
   run_id+plan+invocation, sorties distinctes dans raw. Une découverte CTest puis
   la sélection exacte union/dédupliquée ; regex échappée/ancrée avec **groupes
   ordinaires** CMSys, même arbre/configuration. Les six variables hardware sont
   explicites ; elles ne constituent pas une qualification du matériel.
4. Software llvmpipe explicite ; CPU sans GL ; aucun wrapper/Xvfb/repli logiciel.
   DISPLAY/X server/pilotes restent à préparer par l'opérateur pour qualification
   future. Aucun environnement global modifié, aucune promesse de cleanup de
   tous descendants ajoutée au runner existant.
5. Collecteur de métadonnées séparé : quatre argv fixes `g++-14/cmake/conan/ninja
   --version`, sans shell, timeout10s, absence/erreur/version vide indisponible.
   Variables allowlistées, diagnostics sensibles filtrés. Les seuls extraits
   de version conservés sont bornés à8192caractères par flux avec indicateurs
   explicites `stdout_truncated`/`stderr_truncated`. **Aucun quota ou changement
   des journaux/captures communs.** Ces sondes restent NON_EXECUTE par l'auteur.

Le troisième fichier du delta est cette remise documentaire. Aucun changement
core/base/model/runner/report/verify, presets, linux-evidence, C++, Windows ou CI.
Les CMake parents ne sont pas raccordés ici : leur inclusion optionnelle reste
au futur intégrateur conformément au contrat, pas un contournement par helper.

## Contrôles réellement exercés

- AST seulement des deux fichiers de production modifiés, par
  `C:/Python313/python.exe -B -c <script ci-dessous>` via Context Mode : code0,
  Python3.13.1, aucun import ou exécution du candidat.
- SHA256 des quatre fichiers Python RED (trois tests + fixture) : identiques à
  la remise `678b215`. Aucun test, fixture ou oracle modifié. Les six Release et
  dix-neuf historiques n'ont pas de delta, ni les fichiers exclus.
- Relecture statique ciblée : phases, flags, ordre host/build, mapping, chemins
  à espaces, IDs et syntaxe regex. Ce n'est pas une validation comportementale.

Script AST exact passé en argument `-c` (cwd worktree ci-dessus) :

```python
import ast, pathlib, sys
root = pathlib.Path("C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-24")
for name in ("scripts/build-linux.py", "scripts/verification/adapters/linux.py"):
    path = root / name
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    print(name, "AST_OK", "lines=" + str(len(path.read_text(encoding="utf-8").splitlines())))
print("AST_ONLY Python", sys.version.split()[0], "no candidate import or execution")
```

## Commandes proposées au runner lead uniquement

Après revue du diff/confinement, via Context Mode, même cwd, logs nouveaux et
empreintes séparées de RED :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p 'test_binding_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p 'test_release_contract.py' -v
```

Attendu à vérifier : 34 nouveaux +6 Release, zéro échec/erreur/skip. Puis19
historiques sur copies exactes confinées comme rapport82, pas sur l'arbre partagé.
Le lead conserve commandes/cwd/HEAD/delta/SHA256, comptages et premiers échecs
causaux. Aucun de ces résultats GREEN/non-régression n'est revendiqué ici.
Revue indépendante ciblée avant intégration ; intégration/fermeture restent lead.

## Matrice et limites

- Windows/Python3.13.1 : syntaxe AST et immutabilité des tests vérifiées.
- Tests Python nouveaux, Release et historiques : **NON_EXECUTE par auteur**.
  GREEN comportemental : **INCONCLUSIVE**, en attente du runner lead.
- Linux natif Debug/Release/ASan, versions effectives des outils, ABI/STL C++23,
  compile_commands, CPU moteur, GL logiciel/matériel, images et performances :
  **NON_EXECUTE**. Ni suite moteur, ni processus helper/versiontool/factice,
  installation, affichage ou cloud exécuté par l'auteur.
- Profil source inchangé GCC14/x86_64/libstdc++11/Conan2/Ninja ; ces déclarations
  ne remplacent pas la sonde compile/link format/stacktrace sur le futur hôte.
- Aucune correction de fixture nécessaire, aucun mapping d'oracle demandé.
  Aucun sous-agent/outil d'équipe ou complétion de tâche.

Statut : **GREEN_READY pour revue/exécution lead**, pas fonctionnement démontré.
