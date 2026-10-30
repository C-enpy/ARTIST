# pool-41 (@build-engineer teammate)

Session: ses_e73429ca8ffeC3KVaTn5yuMsDR
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T12:38:09.543Z — msg_18cd14d83001UeohFKrDxwCmOD

## TESTS_READY — candidat sur disque, NON_EXECUTE

**Worktree :** `<USER_HOME>/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-41`  
**HEAD vérifié :** `d19c720638bc56eb46f2862874110d0e1b6f2852`.

Exactement trois nouveaux fichiers sous `scripts/tests/verification_seams/`, aucun fichier suivi modifié :

- `g1_windows_fixture.py`
- `g1_windows_memory_driver.ps1`
- `test_g1_windows_helper_seam.py`

**Persistance Git restante :** fichiers non suivis, non committés. La demande de commit propre contredit l’interdiction explicite des effets Git auteur ; je n’ai effectué ni ajout ni commit. Le lead doit persister le candidat avant intégration.

### Contenu livré

- Vrai plan Windows `cpu-only / cpu.float-mirror / Debug`, vrai `WindowsAdapter`, `BundleSink`, `evidence.prepare/build`.
- Installation des programmes et fragments avant identité ; pins historiques inchangés.
- Programme composé avec vrai `PSScriptRoot`, sans exécution du helper complet.
- Une commande décrite → un driver PowerShell → un `ProcessOutcome` commun, sans seconde capture.
- Observateur `_finish` déléguant à l’original dans un `finally`.
- Bruts binaires, fermeture des writers, leases, empreintes et continuité payload/témoin observés.
- Arbres originaux conservés sous `out/verification-synthetic/g1-windows/`, contrôles sous chaque `out/seam-control/`.

**Substitutions exactes :**

- `New-VerificationProcessObjectLeaf` → `New-G1MemoryProcess`
- `Read-VerificationClockLeaf` → `Read-G1Clock`
- `Read-VerificationHostObservationLeaf` → `Read-G1Host`
- `Read-VerificationToolMetadataLeaf` → `Read-G1Metadata`
- `Invoke-VerificationNativeProcess` → `Invoke-G1MemoryCapture`

La population nominale et ordonnée est définie dans `WRAPPER_NAMES`, `DATA_NAMES`, `BINDING_NAMES`, `PORTS`, puis contrôlée indépendamment par `Assert-G1Program`. Son allowlist exacte est dans cette fonction : commandes dynamiques, processus natifs, accès global/environnement OS et redirections refusés. Le véritable exporter est appelé uniquement sur dictionnaire synthétique.

### Tests proposés

**Six baselines :** pins/extraction/ordre ; AST/exporter/programmes installés ; identité divergente sans mutation des scripts ; factory/fermeture/lease absente ; hash divergent ; second appel refusé.

**Quatre caractérisations :**

1. Prepare/build nominal, même bundle et mêmes octets/hash/mtime payload+témoin.
2. Témoin modifié : refus avant description build/runner/factory, sans réparation.
3. Témoin absent : même refus précoce.
4. Conan17 : partiels collectés avant reçu défavorable, absence de configure/témoin, cause primaire conservée malgré anomalie secondaire.

### Point à relever avant run

Le code réel `Initialize-VerificationTooling` demande **`python-pip` même avec le tooling préinstallé**. Le candidat conserve et simule ce rôle autorisé par le contrat privé, sans remplacer cette fonction. Cela diffère de l’énumération simplifiée des rôles prepare dans A0. Build reste strictement `msvc-env`, puis `cmake-build`.

### Matrice réellement exercée

- Git : HEAD, branche, absence de modifications suivies, présence des trois fichiers vérifiés.
- Python : **AST valide des deux fichiers**, dix méthodes de test identifiées ; aucun import cible.
- PowerShell, tests synthétiques et non-régression : **NON_EXECUTE**.
- Moteur CPU, Windows/Linux natifs, CMake, STL/ABI, GPU et performance : **NON_EXECUTE**.

Aucune cause d’échec runtime observée ; aucun GREEN revendiqué.

### Commandes futures — après relecture lead

Depuis la racine du candidat, sérialisées :

```powershell
C:/Python313/python.exe -B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1BaselineTests -v
```

**Seulement si les baselines réussissent :**

```powershell
C:/Python313/python.exe -B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1CharacterizationTests -v
```

Puis non-régression ciblée décidée par le lead. Les pannes du harnais restent `HARNESS/INCONCLUSIVE`, jamais un RED production fabriqué.

### SHA256 finaux

- Fixture : `7f922f4725a4e478ed1acdac0ee281f3b54857ee77612038fc2bee3e8216b22c`
- Driver : `36608e7b289e97e42502d634e351045b03e6b9e1d08406146e919498716f3167`
- Tests : `f5771dade1023f5d30ccd4b80b0400583286d432eda85706d71fe59db6d4a41e`

Aucune modification production, admission, pin, profil, CMake/CI ou branche B ; aucun sous-agent ni ressource cloud/Docker.