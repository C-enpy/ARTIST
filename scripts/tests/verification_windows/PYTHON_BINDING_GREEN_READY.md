# Windows Python + paramblock — GREEN_READY, pas GREEN observé

## Autorité et tranche

Auteur unique pool-23, tâche `task_mvukzhsc_0052_f1bbfq`. Point de départ propre
`c199d80e1ea570f5bd142bd683af650ee40c9a9e`, worktree
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-23`,
branche `ensemble-4b686588-c97c-46ab-99a1-e176a14b9184`.

Autorisation lead : GREEN minimal Windows Python et les quatre déclarations
privées de paramètres seulement. Aucun test ni programme factice par l'auteur.
Décision cache ratifiée au canonique dans windows-adapter-binding-v1.md, commit
lead71465aa2 ; pas de copie/merge de branche, contrat commun inchangé.

Preuves RED réutilisées, exécutant **coordinateur** :

- f14ea711 :28 tests,25 failures avec sous-cas, zéro error/skip ; logs
  `out/verification-synthetic/windows-binding-red-2026-10-30T06-41-49-105Z/`.
- c199d80 :19 méthodes,66 failures avec sous-cas, zéro error/skip ; logs
  `out/verification-synthetic/windows-ctest-cache-red-2026-10-30T06-57-15-246Z/`.
  Manifeste et stderr relus via Context Mode : candidat/cwd/commande concordent,
  première assertion `CAUSAL RED: Windows direct execution binding is not
  implemented`, `Ran 19 tests in 7.245s`, `FAILED (failures=66)`.

Les fixtures/receipts de setup ont fonctionné au RED. Plusieurs assertions aval
étaient masquées par le scaffold : ces RED ne sont pas un audit de mutations
complet du parser/no-clobber/lecture unique. Leur satisfaction GREEN attend le lead.

## Correction bornée

`scripts/verification/adapters/windows.py` remplace le scaffold :

- Constructeur lexical, descriptions sans écriture/processus/device probe ni
  mutable dernier-bundle. Source root absolue ; arbre Windows MSVC config
  `-verification` cohérent avec les receipts. Pas de resolve/stat d'exécutable.
- Prepare/build décrivent le helper existant avec payload et logs fournis par le
  sink. Prepare déclare bootstrap/résolution et ajoute l'opt-in seulement s'il
  est présent dans Request ; le core refuse avant effet sinon. Build n'installe
  rien et exige l'entrée de payload dans le PreparationReceipt. Aucune écriture
  ou rematérialisation du payload par l'adaptateur.
- Workloads directs : mapping cible→chemin relatif du BuildReceipt, confinement
  lexical dans le bon arbre et entrée SHA256 dans identity.files, jamais simple
  concaténation de nom. Reprise du build_workload_argv intégré, valeurs visibles,
  request_id déterministe issu du run pour chaque invocation, sorties distinctes
  sous raw. L'admission physique/hashes/lease reste au core.
- CTest seulement si sélection CTest : cache du même arbre inventorié ; UNE
  read_bytes → SHA256 comparé → parsing des mêmes bytes. Les deux définitions
  INTERNAL sont uniques/non vides ; mauvais type, définition malformée ou doublon
  ne sont pas ignorés. Comparaison lexicale ntpath indépendante de l'hôte,
  exe attendus/absolus Windows et répertoires normalisés identiques, argv0 CTest
  conserve exactement la graphie déclarée. Pas de PATH/cwd/expansion/fallback.
  Workloads-only ne lit pas le cache.
- CTest direct : découverte JSON depuis le même arbre/-C, puis sélection union
  exacte ancrée des noms catalogue, -j1, --no-tests=error et JUnit exclusif sous
  raw. Une seule sélection combinée évite un double lancement de populations
  communes ; le core garde la réconciliation et les captures.
- Hardware CTest : exactement les six entrées de contexte issues du plan/reçu
  dans env_delta du run sélectionné. Pas de forçage logiciel, pas de déclaration
  via valeurs parent. CPU/workloads ne reçoivent aucune variable GL ajoutée.
- collect_environment rapporte seulement l'interpréteur observé (sys.platform,
  os.name, version/exécutable Python, taille de pointeur), variables vide. Ce
  n'est pas un environnement enfant MSVC/GPU, ni une attestation compiler/STL.

`scripts/build-windows.ps1` : seules les déclarations de
VerificationInputFile, VerificationBundleRoot, VerificationLogDirectory et
VerificationTimeoutSeconds (1..3600, défaut3600) sont ajoutées, avec la virgule
requise après le dernier ancien paramètre. Aucun autre code PowerShell modifié.

## Confinement et garde toujours fermée

Le helper fonctionnel est **TOUJOURS FERMÉ** avant dispatch. Le nouveau Python
peut être testé en processus avec les doubles, mais ses commandes helper réelles
resteraient refusées. Ce candidat ne prétend donc pas que la chaîne Windows
prepare/build native fonctionne. Son ouverture attend le propre mapping précis
et les tests de fragments entrée/dispatch/env/restore exécutés en RED par le lead.

Les47 tests et leurs fixtures, anciennes51 assertions, native_source.sha256,
WRAPPERS/NATIVE/DISPATCH/legacy et la garde de fermeture restent inchangés.
Core/model/base/runner/verify/reporting/C++/CMake/presets/workflows exclus.
F1 (environnement effectif) reste une correction commune à la responsabilité
du lead : aucun contournement/copieur d'environnement parent ajouté ici.

Le hash cache admet une déclaration, pas le binaire CTest, sa présence, son
installation ni un contexte. Les chemins outils réels ne sont pas exercés.
Pas de base de compilation inventée ou de contournement des sondes C++23.

## Commande pour le lead — pas exécutée par l'auteur

Depuis le worktree auteur :

```powershell
Set-Location scripts/tests/verification_windows
C:/Python313/python.exe -B -m unittest -v test_adapter_binding test_binding_helper_interface test_ctest_cache_binding
```

47 méthodes attendues selon les deux lots source déjà exécutés en RED, pas un
nouveau comptage découvert par l'auteur. Attendu GREEN synthétique, zéro failure/
error/skip requis, compteurs de lecture et interdictions d'effets satisfaits.
L'observé sera celui du lead, avec nouveaux logs exclusifs et empreintes.

Après revue : non-régression cœur/contexte/scénarios synthétiques par le lead,
puis les51 fondations via sélection nominative hors06 du rapport81/manifeste.
Aucun unittest discover natif global, helper complet, cmd/vcvars, outil réel,
moteur CPU/GL, rendu, benchmark, installation ou cloud autorisé par cette note.

## Matrice réellement exercée par l'auteur

- Lecture Git/contrats et analyse des bruts RED via Context Mode, pas de rerun.
- AST de windows.py réellement accepté sous Python3.13.1 via Context Mode, sans
  import projet ; aucun test ou enfant factice exécuté.
- Diff PowerShell5 ajouts/1 retrait (virgule +quatre paramètres), ancien corps
  après paramblock identique en texte LF-normalisé, SHA256
  `4cbaea96032b984e981c5a59b61e2a6bce25d2261e1aad4eddedf9e641958d3c`.
  Diff des anciens tests/fixtures/pins vide contre c199d80 ; la seule nouvelle
  documentation de ce répertoire est cette remise, pas un changement d'oracle.
- GREEN et non-régression : NON_EXECUTE par l'auteur. Windows natif Debug/Release,
  Linux, ASan, outils/compilateur/STL/ABI/SDK, GPU matériel/logiciel : NON_EXECUTE.

Remise GREEN_READY pour revue/exécution lead, pas de clôture et pas d'ouverture
de la tranche helper. Aucun sous-agent/team tool ni score/politique modifié.
