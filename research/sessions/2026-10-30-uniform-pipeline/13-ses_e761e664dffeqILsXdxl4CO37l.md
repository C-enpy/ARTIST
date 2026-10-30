# linux-pipeline (@build-engineer teammate)

Session: ses_e761e664dffeqILsXdxl4CO37l
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T22:59:01.887Z — msg_189e35e7b001Yu2gqXbIgaJmjs

HEAD `1fba1c9` et entrées ratifiées confirmés dans mon worktree isolé.

Première tranche : tests synthétiques de Release (presets explicites, Conan host/build cohérents, configuration seule, propagation d’échec) et garde Debug existante. Tous les processus seront doublés ; je n’exécuterai aucun test ni helper. Je remettrai les tests committés en `RED_READY`, puis attendrai le RED causal observé par le lead avant toute implémentation.

L’adaptateur dépendant du core reste différé : `model.py` et l’interface runtime ne sont pas encore intégrés.

## assistant — 2026-10-29T23:01:02.215Z — msg_189e53483001SQrmxwuUs096KC

## RED_READY — première tranche uniquement

- **Commit :** `2d45a6dac3e12a9c2bfa1cc80cd460130db63162`
- **Worktree :** `<USER_HOME>\.local\share\opencode\worktree\3a086c\ensemble-2dc31e0a-cwsmsw-linux-pipeline`
- **Fichier :** `scripts/tests/verification_linux/test_release_contract.py`
- Base `1fba1c9`, briefs/contrat/QA confirmés. Candidat présent sur disque, arbre propre.

### Commande proposée au lead

Depuis ce worktree, via Context Mode :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p test_release_contract.py -v
```

Interpréteur présent ; **commande non exécutée par moi**.

### Oracle attendu

Six tests synthétiques : **cinq échecs et un succès attendus**, non observés.

Premières causes attendues :
- helper sans option `--configuration release` ;
- presets explicites `linux-release` absents.

Assertions couvertes : Conan avant CMake, `Release` host **et** build, arbres distincts, configuration seule sans build/tests, code enfant préservé, cohérence configure/build/test et garde Debug historique.

Les processus sont obligatoirement doublés ; lancement enfant, shell et réseau piégés.

### Suite bornée, après RED observé et accord du lead

1. Ajouter Release au helper et aux presets, sans modifier le comportement historique.
2. Après intégration du core, tester puis implémenter l’adaptateur Linux pur et ses phases distinctes.
3. Préserver les gardes llvmpipe ; matériel jamais déduit par exclusion. Extraction depuis `linux-evidence.py` seulement après transfert explicite.

### Matrice réellement exercée / limites

- Windows : lecture statique et contrôle Git uniquement.
- Tests synthétiques : **NON_EXECUTE**.
- Configuration/build moteur, Linux, OpenGL logiciel/matériel, benchmarks : **NON_EXECUTE**.

**Dépendance bloquante pour l’adaptateur :** `model.py` et `adapters/base.py` absents. Aucun schéma copié, aucun faux RED d’import. Aucun code GREEN écrit ; attente de revue et RED causal par le lead.

## assistant — 2026-10-29T23:05:27.144Z — msg_189e93f640014f6wqSgPXYorte

## RED_READY_FIXTURE

**Commit :** `0e6a0614dc77e3891e94155e627a923b02152949`

Correction unique : `self.fixture` normalisée par `.resolve()` avant création des fichiers et affectation du faux `__file__`. Assertions `-of` inchangées. Aucun helper/preset GREEN modifié.

**Worktree vérifié, propre :**
`<USER_HOME>\.local\share\opencode\worktree\3a086c\ensemble-2dc31e0a-cwsmsw-linux-pipeline`

Commande lead, depuis ce worktree :
```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p test_release_contract.py -v
```

**Attendu :** garde Debug verte et cinq RED causaux Release. **Non exécuté par moi.** Dépendance core manquante maintenue.

## assistant — 2026-10-29T23:13:26.751Z — msg_189f090d8001uwoSG3w1FzP1Xc

Le lead a confirmé le RED causal : garde Debug verte et cinq échecs Release. Je vais ajouter Release au helper et aux trois presets, avec `build_type` cohérent côté host/build, puis corriger le setup temporaire avant `clear(env)`. Aucun test ne sera exécuté par moi ; l’adaptateur runtime reste différé jusqu’à intégration du core.

## assistant — 2026-10-29T23:14:24.436Z — msg_189f17230001c19uWw3B4ek0Gl

## GREEN_READY

**Commit :** `42828c3a01f997783a3d86da4013a99bf4ef5309`

**Worktree vérifié, propre :**
`<USER_HOME>\.local\share\opencode\worktree\3a086c\ensemble-2dc31e0a-cwsmsw-linux-pipeline`

### Correction bornée
- Helper : `--configuration debug|release`, Debug par défaut ; Conan host/build cohérents.
- Presets `linux-release` configure/build/test, arbre distinct, découverte vide refusée.
- Presets préexistants inchangés ; gardes historiques llvmpipe intactes.
- Release+ASan explicitement refusé.
- Fixture : `tempfile.gettempdir()` capturé avant `clear(env)` et passé via `dir=`, sans chemin figé. Assertions inchangées.

### Vérification proposée au lead

Depuis ce worktree, via Context Mode :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p test_release_contract.py -v
```

**Attendu : six tests verts**, non exécutés par moi.

Non-régression ciblée proposée :
```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests -p test_build_linux.py -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests -p test_linux_evidence.py -v
```

### État des preuves
- RED : confirmé par le lead sur `0e6a061` — garde Debug verte et cinq échecs Release causaux.
- Auteur : inspection statique et `git diff --check` seulement.
- GREEN, build Linux et OpenGL : **NON_EXECUTE** par moi.
- Adaptateur runtime différé : `model.py` et `adapters/base.py` toujours absents ; aucun schéma copié.