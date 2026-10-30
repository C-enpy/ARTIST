---
title: Pipeline de vérification des uniforms — guide de qualification
category: development
tags: [artist, build, testing, guide]
summary: "Photographie documentaire du pipeline livré : phases explicites, preuves synthétiques attribuées et qualification native NON_EXECUTE."
created: 2026-10-30
updated: 2026-10-30
source_revision: 165aee5f1a1da7db8431555d196081f802558ed2
sources:
  - scripts/verify.py
  - scripts/verification_workflow.py
  - scripts/verification/catalogue.py
  - scripts/verification/adapters/windows.py
  - scripts/verification/adapters/linux.py
  - scripts/verification/evidence.py
  - scripts/verification/reporting.py
  - ARTIST/verification/CMakeLists.txt
  - .github/workflows/verification-pipeline.yml
  - .opencode/missions/team_mvu0gbxm_0000_cwsmsw/reports/142-helper-integre-verification-synthetique.md
  - .opencode/missions/team_mvu0gbxm_0000_cwsmsw/reports/149-tests-windows-migres-integres-verts.md
  - .opencode/missions/team_mvu0gbxm_0000_cwsmsw/reports/152-couture-meme-bundle-verte-avant-audit.md
---

# Pipeline de vérification des uniforms

> [!warning] Livré pour qualification — NON_EXECUTE natif
> Cette photographie précède la QA finale. Aucun C++/moteur, même CPU seul,
> contexte GL, rendu, benchmark, outil natif de build, helper complet, cmd/vcvars,
> installation, workflow ou cloud n'a été exécuté dans cette mission.
> Les tests synthétiques du pipeline ne démontrent pas son fonctionnement natif
> Windows/Linux, ni une performance. Actualiser cette note après intégration et QA.

Le code fait foi. Voir [[development/build-and-test]],
[[engineering-principles]] et [[adr/README]]. ADR-0001/0002/0004/0006/0007 restent
**Proposed**, 0003/0005 **Accepted / partial** ; ADR-0008 ne qualifie que sa
baseline Windows Debug. Les campagnes moteur historiques, y compris les notes
Linux existantes, ne sont pas une exécution de ce nouveau pipeline.

## Catalogue fermé et profils

`scripts/verification/catalogue.py` décrit cinq scénarios sélectionnables :

- `cpu.float-mirror` : profil `cpu-only`, route `pass-name`, cible
  `ArtistUniformCpuWorkload`. Vérifie le miroir CPU et le contrôle indépendant
  0/.5/0 ; ne prouve aucun upload.
- `gl.float-routes-correctness` : cible `ArtistUniformOpenGLWorkload`, routes
  `pass-name` et `prepared-float` ; miroir CPU, readback float natif et quatre
  pixels indépendants par cellule 32×32, avec contrôle 0/.5/0.
- `gl.float-preparation` : cibles `ArtistLib_tests` et
  `ArtistLib_component_tests`, sélection CTest exacte de six noms du catalogue ;
  réflexion scalaire/type/array[1] réelle et gardes CPU explicitement mockées.
- `gl.float-lifetime` : mêmes deux cibles, sept noms CTest exacts ; contexte,
  programme courant et retraite réels, complétés par des gardes mockées.
- `gl.float-submission-cpu` : cible `ArtistUniformOpenGLWorkload`, deux routes,
  **release seulement**, métrique `cpu_wall_submission`. Ce temps CPU de
  soumission n'est ni un temps GPU ni une preuve de gain.

Les autres scénarios acceptent `debug` ou `release`. Les scénarios GL acceptent
`opengl-software` ou `opengl-hardware`, jamais `cpu-only`. Les deux workloads
réels sont distincts des suites GoogleTest. `ARTIST_BUILD_VERIFICATION` est
**OFF par défaut** ; son activation construit les workloads sans les exécuter
ni les enregistrer automatiquement comme tests CTest.

Le profil logiciel Linux impose `LIBGL_ALWAYS_SOFTWARE=true` et
`GALLIUM_DRIVER=llvmpipe` et exige une preuve de contexte 3.3 core. Windows
logiciel n'est pas supporté. Le profil matériel exige le vendor et renderer
exacts attendus, vérifiés dans le résultat/contexte ; ni les labels du runner
ni le nom d'une carte ne prouvent le contexte. Aucun repli logiciel silencieux.
Mesa/Xvfb peut qualifier une fonctionnalité, jamais les performances du GPU cible.

## Préconditions opérateur, avant toute qualification future

- Python adapté aux scripts, CMake ≥ 3.25, Ninja et Conan **2**, dépendances et
  outils déjà préparés selon les scripts existants. Conan installe avant CMake
  via `CMakeDeps`/`CMakeToolchain` (ADR-0008), sans migration implicite.
- Windows x64 : MSVC toolset **14.44**, SDK Windows et tooling attendus par
  `scripts/build-windows.ps1`. La baseline historique MSVC 19.44 / SDK
  10.0.26100 / CMake 3.31.7 / Conan 2.12.2 / Ninja 1.13.0 reste historique,
  pas une observation de cette mission. Le helper prepare/build est intégré,
  mais non qualifié nativement ; l'encodage non ASCII à travers cmd reste ouvert.
- Linux : compilateur et STL C++23 compatibles, Conan/Ninja/CMake et dépendances
  système préparés ; GL nécessite `DISPLAY` et un serveur X préparé par
  l'opérateur. L'adaptateur ne démarre ni ne répare Xvfb/serveur X.
- Vérifier générateur, architecture, ABI, chemins et versions réels ; la sonde
  `cmake/CheckCxx23.cmake` compile/linke `std::format` et `std::stacktrace`.
  `-std=c++23` ne suffit pas ; ne pas affaiblir cette sonde.
- Un build à la fois par arbre : Windows utilise
  `out/build/windows-msvc-{debug|release}-verification`, Linux
  `out/build/linux-{debug|release}`. Sérialiser aussi les autres utilisateurs
  de ces arbres Linux ; ne pas partager de cache entre toolchains ou OS.
  La base `compile_commands.json` doit provenir de la configuration valide,
  sans inventer d'include paths pour clangd/Serena.

La préparation future peut résoudre les dépendances **uniquement par opt-in**
`--allow-dependency-resolution`. Cela n'autorise pas une installation ni une
exécution dans la présente mission.

## CLI : consultation puis phases séparées

Les commandes suivantes sont des **exemples futurs NON_EXECUTE**, depuis la
racine du candidat. `python` désigne le Python préparé ; sous Linux utiliser
`python3`. `list` et `plan` sont consultatifs : pas d'adaptateur chargé,
installation, bundle, processus enfant ou observation GPU.

```text
python -B scripts/verify.py list --json
python -B scripts/verify.py plan --platform windows --profile cpu-only --configuration debug --scenario cpu.float-mirror
python3 -B scripts/verify.py plan --platform linux --profile opengl-software --configuration debug --scenario gl.float-routes-correctness
```

`--scenario` et `--route` sont répétables ; routes admises selon le catalogue.
`--iterations`, `--repetitions`, `--batch-size` valent 1 par défaut.
`--measurement-requested` demande explicitement une mesure ; ne pas l'ajouter
aux scénarios sans métrique. Pour matériel, fournir les deux options
`--expected-gl-vendor` et `--expected-gl-renderer` avec les chaînes exactes de la
machine à qualifier (jamais une valeur supposée).

Exemple futur CPU Windows, **quatre invocations distinctes**, sans cascade :

```text
python -B scripts/verify.py prepare --platform windows --profile cpu-only --configuration debug --scenario cpu.float-mirror --output-root out/verification --allow-dependency-resolution
python -B scripts/verify.py build --bundle CHEMIN_ABSOLU_BUNDLE_RETOURNE
python -B scripts/verify.py run --bundle CHEMIN_ABSOLU_BUNDLE_RETOURNE
python -B scripts/verify.py report --bundle CHEMIN_ABSOLU_BUNDLE_RETOURNE --json
```

Le chemin symbolique ci-dessus doit être remplacé par le bundle retourné par
`prepare`. N'exécuter la phase suivante qu'après reçu compatible réussi :
prepare → build → run. Aucun report ne reconstruit ou ne relance un test.
Une phase déjà reçue n'est pas rejouée pour réparer le bundle ; préparer une
nouvelle tentative. Conserver même machine, racine source, candidat et plan.
Un nouveau checkout/candidat ne réutilise pas l'ancien bundle ; `clean:false`
ou une copie des fichiers ne prouvent pas sa validité.

Windows réutilise le helper PowerShell pour prepare/build. `run` lance
directement les exécutables admis ou CTest, **pas un helper-run**. Linux réutilise
`build-linux.py` pour préparer/construire. Pour CTest, les adaptateurs demandent
la découverte JSON puis la sélection exacte, en série, avec JUnit et
`--no-tests=error` ; noms du catalogue ≠ découverte effectivement constatée.

## Reçus, identité et lecture du rapport

Le bundle conserve `plan.json`, `manifest.json`, `environment.json`,
`identity-before.json`, `identity-build.json`, `identity-after.json`,
`commands.jsonl`, journaux et résultats bruts sous `raw/` et `logs/`.
Les reçus prepare/build/run lient `run_id`, `plan_sha256`, état et empreintes
d'artefacts ; source HEAD/delta, shaders, configuration et exécutables sont
contrôlés. Un artefact absent/modifié ou un prédécesseur incompatible ne devient
pas acceptable parce qu'un upload a réussi.

Windows conserve aussi l'entrée privée `raw/windows-helper-input.json`, les
captures `stdout.bin`, `stderr.bin`, `command.json` par rôle et le témoin
`logs/windows-helper-prepare/toolchain.json`. Ses **12 champs** sont
`format`, `configuration`, `build_directory`, `visual_studio_installation`,
`msvc_version`, `msvc_architecture`, `windows_sdk_version`, `python`, `conan`,
`ninja`, `cmake`, `cl`. Le hash du JSON atteste ces **déclarations**, pas
l'identité binaire des outils ni leurs capacités C++23.

Lire séparément les axes du rapport :

- `pipeline_state` : `NOT_STARTED`, `RUNNING`, `COMPLETED`, `FAILED`,
  `INTERRUPTED`, `BLOCKED` ; terminé ne signifie pas tests réussis.
- `test_verdict` : `NOT_EXECUTED`, `PASS`, `FAIL`, `INCONCLUSIVE`.
- `measurement_validity` : `NOT_REQUESTED`, `ABSENT`, `VALID`, `INVALID`.
- `freshness` : `CURRENT`, `STALE`, `UNVERIFIED`.

Codes CLI : 0 succès de l'opération demandée (list/plan ne prouvent aucun test),
2 arguments/plan refusés, 3 précondition/prédécesseur indisponible,
4 erreur pipeline/preuve/protocole, 5 oracle test en échec,
6 test inconclusif ou mesure demandée absente/invalide, 130 interruption.
Consulter `causes` et les logs pour la première cause ; plusieurs causes peuvent
coexister. Les workloads ont leur propre protocole : 0, 10 oracle échoué,
20 précondition indisponible, 30 panne interne/protocole ; le cœur les traduit.
Zéro test découvert n'est jamais un succès. Ne pas confondre mesure non demandée,
mesure absente et mesure invalide, ni panne du pipeline et échec métier.

## Workflow manuel, non lancé

`.github/workflows/verification-pipeline.yml` est **workflow_dispatch seulement**,
sur `[self-hosted, Windows, X64]` ou `[self-hosted, Linux, X64]`.
`verification_workflow.py` transporte littéralement les entrées vers une seule
phase CLI ; pas de plugin ni shell construit à partir des paramètres.
Les entrées sont platform/profile/phase/configuration/scenario/bundle,
expected_gl_vendor/expected_gl_renderer et allow_dependency_resolution.
La racine est fixée à `out/verification`. prepare exige l'opt-in ;
build/run/report exigent le bundle existant sur le même candidat et la même
machine. L'opérateur doit assurer ce routage : les labels OS ne fixent pas une
machine. Upload `always()` préserve les preuves disponibles, pas leur validité.

## Preuves synthétiques attribuées au coordinateur

Le rapport commité `.opencode/missions/team_mvu0gbxm_0000_cwsmsw/reports/142-helper-integre-verification-synthetique.md`
est la source des comptages suivants, **non réexécutés pour cette note** :

- Helper PowerShell test-owned/mémoire : **56/56** ; adaptateur Windows **54/54**.
- Cœur : **224 = 219 succès + 5 skips** liens symboliques/reparse,
  ces cinq cas restent INCONCLUSIVE.
- Contexte **28/28**, scénarios **31/31**, intégration **11/11** ;
  aucun failure/error, empreintes stables selon le coordinateur.
- Commande synthétique publiée :
  `C:/Python313/python.exe -B -m unittest -v test_native_binding test_review_regressions test_helper_functional`.
  Le rapport renvoie les cwd, commandes exactes restantes, hashes et logs à
  `out/verification-synthetic/windows-helper-integrated-2026-10-30T11-49-13-428Z/`.
  Ces sorties locales ne sont pas requises dans chaque worktree et ne sont pas
  copiées ici ; ne pas reconstruire des commandes exactes à partir des seuls noms.

Compléments ensuite intégrés et exécutés par le coordinateur, avec empreintes
stables et sans échec, erreur ou skip :

- **19 tests** d'environnement effectif Windows, admission fine, migration des
  anciens oracles et garanties wrappers/StartInfo. Rapport149, candidat intégré
  `a130773`, journaux `out/verification-synthetic/windows-migration-integrated-2026-10-30T12-31-46-914Z/`.
- **6 baselines puis 4 caractérisations** de la couture cœur/helper sur le même
  bundle et témoin : vrais reçus, octets/hash/mtime conservés, refus de mutation
  et absence avant build, Conan17 et partiels. Rapport152, candidat `165aee5`,
  journaux `windows-seam-baseline-v2-2026-10-30T12-50-42-192Z/` et
  `windows-seam-characterizations-2026-10-30T12-51-14-668Z/` sous `out/verification-synthetic/`.

Ces compléments utilisent des outils simulés ; le rôle python-pip de préparation
est aussi simulé, aucune résolution ou installation réelle. Les comptages
historiques ne sont pas réattribués à une nouvelle campagne ni additionnés sans
tenir compte des recouvrements. Les commandes exactes et l'attribution
auteur/runner figurent dans les manifestes des rapports cités.

La **réception globale reste EN ATTENTE de l'audit indépendant final des tests**,
dont l'admission du profil restrictif doit être vérifiée avant revue. Les succès
locaux ne constituent ni cet audit ni un PASS natif. Dispose/writers hérités et
encodage non ASCII cmd restent non qualifiés ; cinq skips historiques de liens et
test de jonction06 gardent leurs limites. Aucun verdict global natif Windows
Debug/Release ou Linux, campagne matérielle/logicielle ou performance annoncé.

## Contrôle documentaire

Contrôles adaptés : diff limité à cette note et au lien d'accueil, frontmatter,
liens internes et comparaison des flags/cibles/états aux sources listées.
Pas de RED comportemental fictif pour une documentation, pas de réindexation,
build ou test nécessaire à sa rédaction. La revue du lead et l'actualisation
après QA finale restent requises avant réception globale.
