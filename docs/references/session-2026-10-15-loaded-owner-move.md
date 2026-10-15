---
title: "Mission 2026-10-15 — loaded-owner move et ASan"
category: reference
tags: [artist, cpp, ownership, testing, reference]
summary: Contrat approuvé, RED CPU/GL, correction et validation indépendante du diff intégré ; 137 tests normaux et 115 cas uniques ASan.
sources: [research/sessions/2026-10-15-loaded-owner-move/session-capture.md, research/sessions/2026-10-15-loaded-owner-move/H-manifest.json, research/sessions/2026-10-15-loaded-owner-move/D-manifest.json, research/sessions/2026-10-15-loaded-owner-move/R-review.md, research/sessions/2026-10-15-loaded-owner-move/V-report.txt, research/sessions/2026-10-15-loaded-owner-move/S-report.md]
provenance:
  extracted: 0.99
  inferred: 0.0
  ambiguous: 0.01
source_revision: e589aeece2c5abc46aedb089c6a88a6f7dbaa239
source_diff_fingerprint: a1e66dd728c8e6b0700cf7db694040c9592ce18acaf53145590ad0e85053c234
created: 2026-10-15
updated: 2026-10-15
---

# Mission loaded-owner move et expérience ASan

Synthèse de travail, pas export verbatim ni certification de tout le moteur.
Contrat initial : [[architecture/missions/2026-10-15-loaded-owner-move]]. Décision appliquée :
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 Accepted/partial]], pas les
ADR-0002/0004/0006 `Proposed`.

## Provenance et intégration

HEAD principal **`e589aeece2c5abc46aedb089c6a88a6f7dbaa239`**, inchangé. Livrables :

- D, correction/tests : `0bbca5042af3a52994c9540f6092a84d557f8db3`.
- H, harness opt-in : `a423fc250ff2040a47d52f2402019b5f35abee75`.

Après shutdown et `team_merge`, le coordinateur a vérifié le diff réel et les
15 fichiers source/build avant de publier R/V/S en `fresh`, `worktree:false`.
La QA porte sur ce **diff intégré non commité**, pas sur le seul HEAD ni sur un
worktree auteur. Aucun commit des changements utilisateur pour fabriquer un HEAD.
Empreintes et commandes : `_meta/session-2026-10-15-loaded-owner-move-validation.json`.
Preuves locales sous `out/validation/loaded-owner-move-2026-10-15/` ; les hashes ne
constituent pas un archivage de ces fichiers ignorés. L'ingestion de la session
a ensuite conservé neuf snapshots de rapports/manifestes/contrat sous
`research/sessions/2026-10-15-loaded-owner-move/`, avec une capture structurée
de la conversation et des retours PM/A. Ce paquet ne contient pas les 238 artefacts
H/D/V/S ni un export verbatim des historiques natifs ; les limites sont conservées.

## Plan, ownership et ressources

PM en lecture seule : `ses_ec1424e6cffe36qPb8XXwRebOx`. Méthode **PM/LLM**, deux
scores distincts, heuristiques et non probabilités. Plancher développement appliqué.

- A, `engine-architect`, C78/R84 : avis lecture seule ;
  `task_mv9b17uo_0001_pma8ei`.
- H, `build-engineer`, brut C67 → retenu **70**, R63 : CMake/presets/scripts/probe ;
  `task_mv9b17uo_0002_140zct`.
- D, `cpp-engineer`, C86/R91 : deux headers et deux fichiers de tests lifetime ;
  `task_mv9bjax3_000a_twfkse`. Aucun Freer modifié.
- R, `correctness-reviewer`, C77/R87 :
  `task_mv9d5o3a_000t_4s2bz7`.
- V, `rendering-verifier`, C57/R74 : QA normale, pas de nouveau code ;
  `task_mv9d5o3a_000u_0heb5g`.
- S, `rendering-verifier`, C64/R73 : QA ASan, pas de nouveau harness ;
  `task_mv9d5o3b_000v_iwa7w1`.
- W, documentation C44/R48 : exécutée localement par le coordinateur après
  acceptation des preuves, sans nouveau script exécutable.

Graphe : A // H ; contrat A approuvé avant D ; D // H pour l'authoring ;
intégration D+H avant R // V // S. Baux réels : H puis D puis V puis S pour la
compilation hôte, un job/arbre ; S a pu compiler pendant les tests de V après
libération de son bail compilation. GPU V puis S, jamais simultanés. Indexation
documentaire seulement après libération GPU. Les workers sont des feuilles.

Préférence persistée `opencode-go` puis `openai`, GLM-5.3-Flash disponible ; aucun
modèle/effort forcé, routage opérateur conservé. Les six tâches ont des scores
manuels (`not_required`), aucun appel JEV.

## Contrat d'assignation approuvé

Les bases libéraient seulement leur stockage CPU au remplacement de `m_context`,
sans requête de suppression de l'ancien objet GL. Le fix appelle le hook concret
`free(Context&)` **avant** transfert et hors destruction de base.

- Opérateurs de base `protected`, owners concrets `Shader`/`Pass` `final` ;
  assignation publique entre concrets identiques seulement. Rupture API bornée
  approuvée, aucun héritier concret trouvé, pas de shim requis.
- Self-move sans effet dans les états chargé, contexte/handle zéro et moved-from.
  Source vidée après transfert ; pas de suppression répétée du handle transféré.
- Constructeurs move `noexcept`, assignations potentiellement levantes.
  Si cleanup jette, source non consommée et pointeurs conservés ; garantie de base,
  aucun rollback des detach. Les destructeurs restent non-throwing : une exception
  de cleanup dans ce chemin peut toujours terminer le processus.
- Alias d'ancien PassContext : programme invalidé (`passID=0`), stockage/shaders
  éventuellement retenus jusqu'au dernier alias. Alias source suit le contexte
  transféré. Anciennes vues Shader/bindings programme invalidées selon ce contrat.
- Contexte/thread GL compatible courant et accès sérialisés restent caller-owned.
  Aucun `loaded` concurrent du handle, aucune attente GPU globale, pas de migration
  Pipeline/Uniform, aucune promesse de coût nul.

## TDD et revues : preuves distinctes

D a conservé les headers H0 pour RED, recoupés par hashes contre les blobs H0 :

- `D/d-red-tests-01.xml` : **5/5 échecs attendus**, dont deux CPU sans delete
  destination et deux driver où `glIsShader/glIsProgram` restent vrais.
- `D/d-red-lifetime-02.xml` : 31 tests, 21 PASS/10 FAIL, zéro skip.
- `D/d-red-failures-03.xml` : 3/3 échecs attendus, cleanup non appelé.
- GREEN : lifetime **34/34**, puis auteur **137/137** ; 7 probes autonomes de
  compilation, positif accepté et rejets pour `protected`/`final`/profil croisé
  avec deux profils de fixture valides. Ce ne sont pas sept tests CTest de plus.

Matrice C/E/M 3×3 pour chaque owner, self-move, aliases, ordre
detach → deleteProgram → deleteShader et trois échecs de cleanup. Les tests GL
interrogent les IDs avant toute recréation ; programme courant testé delete-pending
puis réellement supprimé après `glUseProgram(0)`. Aucune hypothèse d'ID jamais réutilisé.

H a son propre RED du vérificateur Python (8 tests, 4 échecs de faux positifs),
puis GREEN 8/8. H n'avait pas construit les exécutables moteur : ses placeholders
CTest `NOT_BUILT` ne prouvaient pas la découverte. V/S ont ensuite construit et
découvert les vraies suites. Les 92 artefacts H et 58 D ont été vérifiés par hash.

R : **PASS de lecture**, aucun bug bloquant démontré ; rapport
`R/review.md`, SHA-256 `b5a9659b029280a65f6f323ab89c696572bd8afc60b100221837af9dfb5e44a9`.
Le vérificateur Python a aussi été réexécuté indépendamment, 8/8. R n'a pas
recompilé ni exécuté le moteur/GPU et ne remplace pas V/S.

## Validation normale indépendante V

Configuration/build OFF, découverte **137**, lifetime **34/34** (25 CPU + 9 GL),
unit **106/106**, OpenGL **31/31**. Zéro fail/error/skip/disabled/timeout,
137 noms uniques, 171 invocations avec le ciblé répété. Les 40 artefacts V et
JUnit ont été recoupés ; certains `system-out` JUnit sont tronqués, les LastTest
complets sont conservés. CompileDB 35 TU, aucun flag ni runtime ASan.

Commandes exécutées depuis le principal, via `out/tooling/run-build.py` existant
pour l'environnement MSVC (préfixes de logs distincts dans V) :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -ConfigureOnly -Jobs 1
cmake --build out/build/windows-msvc-debug --parallel 1
ctest --test-dir out/build/windows-msvc-debug -N
ctest --test-dir out/build/windows-msvc-debug -R ShaderLifetime --parallel 1 --no-tests=error --output-on-failure --timeout 120 --output-junit out/validation/loaded-owner-move-2026-10-15/V/v-04-lifetime.xml
ctest --test-dir out/build/windows-msvc-debug -L unit --parallel 1 --no-tests=error --output-on-failure --timeout 120 --output-junit out/validation/loaded-owner-move-2026-10-15/V/v-05-unit.xml
ctest --test-dir out/build/windows-msvc-debug -L opengl --parallel 1 --no-tests=error --output-on-failure --timeout 120 --output-junit out/validation/loaded-owner-move-2026-10-15/V/v-06-opengl.xml
```

## ASan indépendant S

Arbre neuf `out/build/windows-msvc-debug-asan`, jamais cache auteur copié.
Préflight sain 0, faute 86 avec `heap-use-after-free` sur la ligne 47 de la sonde.
**36/36 TU/objets** instrumentés, GTest/GMock compris ; trois PE x64 importent le
runtime et les copies DLL correspondent au toolset. `/RTC1`, CRT Debug, C++23 et
assertions sont conservés ; pas de `NDEBUG`, `/INCREMENTAL:NO`.

Commandes réellement exécutées via le même runner :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -AddressSanitizer -AsanPreflightOnly -Jobs 1
cmake --build --preset windows-msvc-debug-asan --parallel 1
ctest --preset windows-msvc-debug-asan --show-only=json-v1
```

Puis `ctest --preset windows-msvc-debug-asan --parallel 1 --no-tests=error
--output-on-failure --timeout 120 --output-junit <nom-S-neuf>` avec :

- `-R ShaderLifetime -L unit` : **25/25** ;
- `-L unit` : **106/106**, aucune exclusion ;
- `-R ShaderLifetime -L opengl` : **9/9**.

Les noms complets horodatés S05/S06/S10 et commandes sont dans le manifeste.
**115 cas uniques**, 140 invocations ; 22 autres GL hors sélection ASan,
nominativement listés dans `S/S13-test-reconciliation.json`. Aucun skip ni
diagnostic ASan moteur observé ; les 48 artefacts S ont été recoupés.

### Pourquoi 22 GL n'ont pas été exécutés sous ASan

Clarification demandée par l'utilisateur après clôture : **périmètre prévu, pas
abandon après échec**. Le contrat initial de [[architecture/missions/2026-10-15-loaded-owner-move|mission]]
demandait une expérience ASan séparée, les tests ciblés et la suite CPU applicable.
Le plan PM S le précisait avant le développement ; les nombres 9/22 n'étaient pas
fixés avant l'ajout des tests. Les 31 GL ont tous été exécutés normalement ; seuls
les 9 lifetime faisaient partie de la sélection ASan. Aucun retrait pour contourner
un échec ou un blocage n'est rapporté. Étendre ASan aux 22 autres serait une nouvelle
extension de validation, non la reprise d'un livrable abandonné ; aucune telle
extension n'est démarrée implicitement.

### Réserve Q1 : environnement du canari et de CTest

R a relevé que `verify_asan.py` fixe les options des enfants du canari, pas celles
de CTest dans un parent arbitraire. Les métadonnées S05/S06/S10 consignaient déjà
exactement `ASAN_OPTIONS=halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0`,
identique au préflight, sans `log_path` ni option de récupération permissive.
Le coordinateur l'a recoupé ; aucun rejeu supplémentaire n'a été nécessaire.
Cette campagne est qualifiée ; la robustesse du script à d'autres environnements
reste une réserve connue, pas un correctif implémenté ni un faux PASS constaté.

## Environnement et limites restantes

Windows 11 Pro 10.0.26300 x64 ; MSVC 19.44.35221/toolset 14.44.35207, STL143,
SDK10.0.26100.0 ; CMake3.31.7, Conan2.12.2, Ninja1.13.0, Python3.10.11.
Driver réellement exécuté : **RTX3090 / OpenGL3.3.0 NVIDIA617.42**, pas le 616.92
de la baseline du 2026-10-03.

- ASan ne reconstruit pas les binaires Conan GLFW/GLEW, le système/driver ou tout
  le CRT ; GLM/magic_enum seulement via leurs instanciations. Pas de preuve de
  fuite d'objets driver ni d'absence générale d'UB/RAII défaillante.
- Validation API/lifetime sous contexte réel, aucun draw/readback/image,
  benchmark, Release, autre plateforme, coverage, UBSan/TSan ou CI distante.
- Bornes, logs GL vides, fichier manquant, transaction reload et partage du contexte
  Pass restent dans [[development/known-incidents|le registre]].
- Les modifications utilisateur sont préservées. Seul `watch.last_index_time`
  de `.grepai/config.yaml` a évolué pendant la QA ; reste du fichier identique,
  attribution exacte non établie, pas de rollback. Les 15 entrées restent identiques. ^[ambiguous]
- Aucun Docker créé. Aucune mission suivante sélectionnée arbitrairement.

Retour : [[index|Accueil]] · [[architecture/current-state|État réel]] ·
[[development/build-and-test|Build]] · [[architecture/correction-plan|Plan]].
