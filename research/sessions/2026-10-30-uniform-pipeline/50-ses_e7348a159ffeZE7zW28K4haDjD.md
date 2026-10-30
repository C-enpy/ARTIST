# pool-39 (@build-engineer teammate)

Session: ses_e7348a159ffeZE7zW28K4haDjD
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T12:12:25.893Z — msg_18cb9bfa10016cvaLpCtbA8CUU

## Candidat A0 — plan de couture G1, sans code

**Décision proposée : KEEP G1 ; réalisation A1 après acceptation et persistance de ce plan par le lead.** A1 reste **NON PUBLIÉ**.

Lecture seule respectée : aucune édition, import Python, exécution PowerShell, test, installation ou commande native. Complexité PM/LLM brute/retenue **63/63**, risque **59**, incertitude moyenne.

### 1. Entrées et constat

Entrée fournie : H0 `040824cff819b074bfe0738efb3e35f66973555f`, helper `52e7330`, collecteur `9aedc0a`, guide `9a45bf6`. Les rapports142–145 et QA§14–15 confirment leur intégration. **Ces commits ne sont pas revérifiés par Git ici.**

Sources examinées : contrats demandés, fixtures core/Windows, `evidence.py`, `adapters/windows.py`, admission JSON, fragments du helper, CMake/presets/Conan/CI.

Constats déterminants :

- `RuntimeFixture` existe, mais initialise un plan Linux et des doubles : réemploi de son **installation temporaire**, pas de son runner.
- `NativeBindingCase.run_case` recrée son bundle et peut préfabriquer le témoin : **incompatible tel quel avec G1**.
- `HelperRunner.run` fabrique journaux/témoin : utile aux contrats core, **pas producteur admissible pour cette couture**.
- `evidence.prepare` collecte après commande et avant `_finish`, sous lease.
- `evidence.build` appelle `_admit_windows_helper` **avant** `adapter.describe_build`.
- `WindowsAdapter` décrit une seule commande helper par phase.
- L’admission existante distingue les hashes source, NATIVE, fragments et exporter. Son `reference_head` historique ne doit pas être remplacé par H0.

### 2. Trois nouveaux fichiers seulement

Noms proposés, **à réserver par le lead** sous `scripts/tests/verification_seams/` :

1. `g1_windows_fixture.py` — installation avant snapshot, runner fermé, observations.
2. `g1_windows_memory_driver.ps1` — programme test-owned borné et effets mémoire.
3. `test_g1_windows_helper_seam.py` — baselines et quatre caractérisations G1.

Aucun changement aux fichiers production, fixtures historiques, admissions/pins ou fichiers B. Aucun framework ni seconde API publique.

### 3. Installation avant toute identité

Pour chaque cas :

1. Créer un `RuntimeFixture` distinct ; conserver sa racine temporaire possédée et ses fixtures requises.
2. Construire un vrai plan Windows `cpu-only`, scénario `cpu.float-mirror`, configuration Debug, résolution explicitement autorisée.
3. Installer **avant `evidence.prepare`** :
   - les fragments réellement extraits par `test_native_binding.fragments` ;
   - le programme composé sous `<root>/scripts/build-windows.ps1` ;
   - le driver mémoire et les données fixes nécessaires ;
   - profil Windows, requirements, fichiers synthétiques requis ;
   - faux outils et `vcvarsall.bat`, tous portant des octets explicitement inertes.
4. Le programme composé contient paramètres, wrappers, données, binding et **entrée réelle admise**, sans corps legacy ni helper complet. Le root reste dérivé du véritable `$PSScriptRoot`.
5. Conserver les identités du helper original, admission, pins, nouveaux fichiers et programme composé. Ne plus générer/modifier de programme dans `scripts/` après snapshot.

Les entrées/sorties variables du driver résident sous `<root>/out/seam-control/<phase>/`, hors population source. Aucun filtrage supplémentaire de `_source_files`, aucune neutralisation du delta.

### 4. Dataflow proposé

```text
RuntimeFixture installé
  → vrai plan Windows + WindowsAdapter
  → vrai BundleSink
  → evidence.prepare
      → création bundle unique + leases
      → snapshot réel
      → transport privé créé/fermé/hashé par le sink
      → SeamRunner.run(CommandSpec helper)
          → un appel au driver PS test-owned
          → programme composé déjà installé
          → entrée/binding/exporter source réels
          → factory + capture mémoire
          → journaux/témoin directement dans CE bundle
      → collecte réelle
      → observation avant _finish
      → PreparationReceipt réel
  → evidence.build(même plan, même receipt, même sink)
      → réadmission réelle avant description/runner
      → un appel helper synthétique
      → collecte réelle + BuildReceipt réel
```

Le runner accepte exclusivement les descriptions prepare/build attendues du véritable adaptateur : argv, cwd, chemins de transport/bundle/logs, phase et flags. Toute autre commande échoue sans fallback.

Il mappe explicitement cette description vers le driver synthétique ; cette substitution est conservée dans les preuves du test. Le lancement réel du helper décrit reste **NON_EXECUTE**.

**Capture commune :** un `ProcessOutcome` par invocation helper, traité une fois par `BundleSink.command`. Le runner ne rappelle pas `sink.command` et ne relance rien pour produire une preuve. Le résultat JSON du driver contient des observations, pas une seconde capture stdout/stderr.

### 5. Port borné des effets PowerShell

Porter seulement les effets nécessaires de `native_binding_fixture.ps1` :

- `ArtistBindingMemoryProcess` et factory ;
- horloge finie, observation hôte synthétique, métadonnées synthétiques ;
- capture mémoire et effets filesystem des commandes factices ;
- garde des chemins, sentinelles et contrôles d’admission nécessaires.

Conserver les fonctions de production : contexte, admission, sélection, tooling, wrappers, rôles, écriture des traces et `Write-VerificationToolchainWitness`.

Substitutions limitées aux feuilles déjà utilisées dans la fixture :

- `New-VerificationProcessObjectLeaf` ;
- `Read-VerificationClockLeaf` ;
- `Read-VerificationHostObservationLeaf` ;
- `Read-VerificationToolMetadataLeaf` ;
- `Invoke-VerificationNativeProcess`.

**Ne remplacer ni `Invoke-VerificationChild`, ni la logique de préparation/build, ni le producteur du témoin.**

Le véritable corps `Write-VerificationMsvcExport`, extrait du littéral admis, reçoit une map synthétique. Son entrée qui lit l’environnement OS n’est pas exécutée. Aucun `expected_witness` complet fourni au producteur : seulement des chemins/métadonnées d’entrée et des assertions indépendantes après production.

Le faux processus ne possède aucun `Start`, `Kill` ou `WaitForExit`. Les `.exe`, `.cmd` et `.bat` restent des données : aucun lancement.

### 6. Filesystem, rôles et confinement

Un cas possède une racine, un arbre Windows Debug et **un seul bundle créé par le vrai sink**. Le driver ne crée pas de bundle ni ses répertoires privés.

Avant chaque invocation, observer :

- `<bundle>/.phase.lock` ;
- `<build>/.verification-writer.lock` ;
- payload déjà fermé et inscrit dans le manifeste ;
- répertoire privé de phase déjà possédé par le core.

Population nominale avec tooling préinstallé et découverte VS synthétique :

- prepare : `vswhere`, `msvc-env`, `conan-install`, `cmake-configure` ;
- build : `msvc-env`, `cmake-build`.

Chaque rôle produit directement ses `stdout.bin`, `stderr.bin` et la trace source `command.json`. Le faux Conan écrit seulement le toolchain synthétique ; le faux configure écrit cache/Ninja ; le faux build écrit uniquement l’exécutable cible inerte nécessaire au vrai inventaire core.

Allowlist des transports : `export-msvc.ps1`, `load-msvc.cmd`, `msvc-env.json`. Aucun parcours récursif supplémentaire, adoption ou écrasement. Les gardes refusent les chemins étrangers et liens/reparse ; elles ne prétendent pas constituer une sandbox hostile.

### 7. Oracles et cas minimaux

**Baselines avant caractérisation :**

- **Extraction/admission :** pins inchangés, population/ordre des fragments, syntaxe du programme composé, exporter exact. Divergence ⇒ `HARNESS/INCONCLUSIVE`, jamais RED production.
- **Lease/identité :** programmes préinstallés et inventoriés ; mutations persistantes de source détectées ; observateurs capables de refuser une lease absente.
- **Données/factory :** factory exclusivement mémoire, compteurs exacts, octets bruts binaires conservés, writers fermés, aucun effet interdit.

**Quatre caractérisations :**

1. **Nominal prepare→build.**  
   Au retour du runner puis avant le vrai `_finish`, relire les octets réellement produits, calculer SHA256 indépendamment, comparer au manifeste. Après retour, comparer aux `PreparationReceipt.artifacts`. Build reçoit exactement ce receipt ; payload et témoin conservent octets, hash et mtime. Aucun bootstrap/configure en build.

2. **Mutation du témoin après prepare favorable.**  
   Nouveau cas/bundle. Mutation explicite sans réempreinte réparatrice. Build refuse avant `describe_build`, runner et factory ; octets altérés préservés.

3. **Absence du témoin après prepare favorable.**  
   Nouveau cas/bundle. Même oracle zéro description/runner/process ; aucune recréation.

4. **Conan nonzero après écriture de partiels.**  
   Code synthétique17 et bruts écrits directement dans le rôle. Aucun configure, aucun témoin favorable. Partiels hashés avant receipt défavorable ; cause enfant17 première, diagnostic de collecte éventuel secondaire.

L’observateur de `_finish` délègue toujours à la vraie fonction : il ne construit aucun receipt. Les tests ne font appel ni à `witness_data`, ni à un receipt copié, ni à une recapture.

**Contrôles discriminants d’oracle :** contre-exemples test-owned de lease absente, hash divergent et compteur de second appel doivent être rejetés. Une altération des fragments reste un refus de harnais. Aucun faux RED sur la production intégrée correcte ; défaut production découvert ⇒ transmission au propriétaire, sans correction dans A1.

### 8. Commandes futures proposées — pas exécutées

Après intégration des trois fichiers, contrôle du candidat et relecture lead, depuis la racine ARTIST :

```powershell
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_seams -p test_g1_windows_helper_seam.py -v
```

Puis non-régression ciblée, sérialisée, avec commandes/cwd du rapport142 et contrats core concernés. Exécuter via Context Mode, conserver logs, identités, découvert/exécuté/skips et première cause. Le chemin Python provient des preuves historiques : sa disponibilité actuelle reste à vérifier avant exécution.

**Aucune commande CMake/Conan/CTest moteur proposée pour A1.** Leur intégration existante demeure inchangée ; aucune base `compile_commands.json` synthétique n’est créée.

### 9. Réception et limites

- **Matrice réellement exercée ici :** lectures statiques sous Windows uniquement ; zéro test/processus.
- **Résultat G1 :** plan proposé, **INCONCLUSIVE / NON_EXECUTE**.
- Preuves rapport142 réutilisées comme historiques, pas comme validation de cette couture.
- Qualification native Windows/Linux, outils/STL/ABI, GPU et performances : **NON_EXECUTE**, hors A1.
- Serena est rattaché à ARTIST, mais `evidence.py` est ignoré par sa couverture active C++ ; navigation Python/PowerShell effectuée par lectures et recherches exactes bornées.
- **Acceptation lead nécessaire :** réserver les trois noms, valider la composition/port des effets et persister A0 avant worktree A1. B reste indépendant et intact. Revue fresh après intégration effective des branches, conformément au §15.

**Aucune nouvelle frontière production ni modification de statut ADR proposée.**