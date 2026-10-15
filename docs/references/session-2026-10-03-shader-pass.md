---
title: Session du 2026-10-03 — Conan 2 et Shader→Pass
category: reference
tags: [artist, reference, ownership, testing, tooling]
summary: Décisions utilisateur, travaux des sous-agents, preuves 122/122, diagnostics rectifiés et reprise de la migration Shader→Pass.
created: 2026-10-03
updated: 2026-10-03
source_revision: 887548ba8a2839daf55ca5636f1519110b4624a7
sources: [conversation-checkpoint, docs/_meta/session-2026-10-03-validation.json, ARTIST/include/graphic/pipeline/Shader.hpp, ARTIST/include/graphic/pipeline/Pass.hpp]
---

# Session Conan 2 et migration Shader→Pass

> [!info] Provenance et limite
> Synthèse de la conversation et des rapports de sous-agents disponibles dans le
> checkpoint, recoupée avec le working tree et les JUnit. Ce n'est **pas** une
> transcription verbatim de toutes les sessions. Les identifiants ci-dessous
> permettent de retrouver les historiques OpenCode s'ils sont encore conservés.
> Les changements restent non committés ; HEAD seul ne représente pas cet état.

## Décisions et demandes de l'utilisateur

- Rien en production et aucun consommateur externe : migrer franchement, sans
  shim de compatibilité. Ne pas confondre cela avec une réécriture générale.
- Établir la propriété des objets natifs à partir du code : `Shader` possède le
  shader GL, `Pass` le programme GL. Commencer par [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]].
- Essayer ASan dans un arbre séparé ; ne pas en faire une dépendance Windows-only.
  **Demande non encore exécutée**, pas une validation accomplie.
- Employer la configuration QMD normale ; l'utilisateur confirme CUDA depuis
  sa console. Ne pas réintroduire une politique permanente CPU forcé.
- Mettre le travail bloquant en arrière-plan. Un worker terminé ou une sortie
  tronquée ne prouve pas que sa commande et son travail ont abouti.
- Demande conditionnelle d'un sous-agent « astra max » : le worker effectivement
  lancé est `cpp-engineer`, sans sélection explicite de ce modèle. Ne pas prétendre
  que la demande de modèle a été exécutée.
- Sauvegarder maintenant les décisions, incidents, modifications et travaux des
  sous-agents dans le vault ARTIST et les mémoires Serena du même projet.

## Toolchain avant la migration

Baseline **109/109** : 87 unitaires + 22 OpenGL, Windows x64 Debug, MSVC 19.44 /
toolset 14.44, Conan 2.12.2. GCC MSYS2 13.2 ne fournit pas le `std::stacktrace`
requis dans sa STL ; cela ne constitue pas un bug moteur. Migration Conan 2 avec
`CMakeDeps`/`CMakeToolchain`, dépendances directes inchangées, cache `out/conan2`,
script `scripts/build-windows.ps1`, capability check compile/link C++23.
[[adr/0008-conan2-et-baseline-windows|ADR-0008]] est Accepted/verified **dans cette
baseline Windows Debug** ; [[adr/0005-build-reproductible-et-validation|ADR-0005]]
reste Accepted/partial. Release, CI distante et autres plateformes non exécutés.

## Modifications intégrées

- `graphic/pipeline/Shader.hpp` : contexte unique, copies interdites, déplacements,
  accès moved-from rejeté, lecture de source restaurée si code vide, RAII dérivé.
- `graphic/pipeline/Pass.hpp` : copies interdites, `Pass::shared()` supprimé,
  `withUniform()` retourne `IPass&`, programme libéré côté dérivé, gardes d'accès
  moved-from. **Le membre de contexte reste `shared_ptr`** malgré l'entrée unique.
- `graphic/context/PassContext.hpp` : `vector<unique_ptr<IShader>>`, transfert
  propriétaire, rejet des shaders nulls et emprunts par référence.
- Composants `opengl/pipeline/component/{shader,pass}/` : `on(Context&)`, `free()`
  idempotent avec ID remis à zéro, nettoyage du programme à l'échec de link,
  rejet `RELOAD_WITHOUT_FREE`. Includes self-contained des lecteurs réparés.
- Contextes GL : IDs initialisés à zéro. `HasBorrowedOnMethod` s'applique à
  Shader/Pass ; Uniform/Attribute/Pipeline gardent le contrat partagé hors tranche.
- Consommateurs et mocks adaptés ; contexts non copiables transférés par move ;
  `MockShader::getContext()` non mocké, fixtures divergentes renommées.
- `ShaderLifetimeTests.cpp` : **14** tests ; `ShaderLifetimeComponentTests.cpp` :
  **5** tests réels. `UniformContractTests.cpp` verrouille aussi le workaround MSVC
  par concept nommé, sans élargir le prédicat d'API.

## Sous-agents et portée de leurs résultats

### Implémentation — `cpp-engineer`

Session `ses_efe12d4e3ffedzqoBp4s0XxXWJ`. Le premier retour signalait seulement
une commande encore en cours ; une reprise a ensuite rendu build et suites verts.
Travaux rapportés : retrait des dispatchs purs des destructeurs de base, RAII
dérivé, restauration du reader dans `IShader::load()`, corrections mocks/fixtures
et tests. Résultat intermédiaire recoupé par le coordinateur : **116/116**
(90 unit + 26 OpenGL). Ne pas garder ce compte comme résultat final.

### Revue indépendante — `correctness-reviewer`

Session `ses_efdc83c33ffektX1dnENb8c1Db`. **PASS de lecture**, aucun blocker selon
le reviewer, mais trois should-fix : F1 fuite sur re-load, F2 IDs non initialisés,
F3 accès Pass moved-from non gardé. La revue précède leurs corrections finales ;
elle n'a pas relancé le build/CTest ni ASan et ne certifie pas l'ensemble du moteur.

F2/F3 et la couverture F9 ont ensuite été corrigés par le coordinateur :
**119/119** (93 + 26). F1 a été transmis à l'architecte avant implémentation.
Les autres findings F4–F10 et leurs limites sont conservés dans
[[development/known-incidents|Incidents et suivi]], pas oubliés sous le verdict PASS.

### Contrat de re-load — `engine-architect`

Session `ses_efdbcae34ffeXkOhCExMByCiIT`. Avis sans modification : **B strict**.
Les loaders rejettent un handle vivant avant `glCreate*` ; destruction explicite
via `free()`, sans politique cachée de remplacement/rollback. Implémenté puis
testé : deux tests mockés de non-mutation + un test shader sur le driver.

La garde observe un **état runtime à la préparation** ; elle ne devient pas un
test compile-time par le seul choix d'un profil. `IPass::load()` charge les
shaders avant le programme : erreur shader possible avant l'erreur pass et pas
de transaction globale prouvée. `pass.free()` seul ne libère pas les shaders.

Orientations uniquement, non implémentées : commencer la tranche suivante par
`PipelineContext::m_passes` en `unique_ptr` et `forPass()` par emprunt, puis le
contexte de `IPass`. Durcir `UniformReader` dans `OpenGLPassFlow` lors de la
tranche uniform/attribute, avec test négatif de concept. ADR-0002/0004/0006 restent
Proposed, ces avis ne les adoptent pas implicitement.

## Preuves finales et commandes réellement exécutées

JUnit relus pendant cette ingestion : **95/95 unitaires** (14 lifetime) et
**27/27 OpenGL** (5 lifetime), failures/skipped/disabled à zéro. Timestamps JUnit :
`2026-10-03T17:05:15` et `2026-10-03T17:05:18` (horloge enregistrée par CTest).
Les empreintes des fichiers sont conservées dans
`docs/_meta/session-2026-10-03-validation.json` ; `out/` peut être nettoyé ou écrasé.

Depuis la racine, l'historique rapporte code retour 0 pour les commandes finales :

```powershell
C:/Python313/python.exe out/tooling/run-build.py configure-b cmake -S . -B out/build/windows-msvc-debug
C:/Python313/python.exe out/tooling/run-build.py build-b2 cmake --build out/build/windows-msvc-debug --parallel 1
C:/Python313/python.exe out/tooling/run-build.py ctest-unit-b2 ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error --parallel 1 --timeout 60 --output-junit ctest-unit.xml
C:/Python313/python.exe out/tooling/run-build.py ctest-opengl-b2 ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error --parallel 1 --timeout 120 --output-junit ctest-opengl.xml
```

Le helper `out/tooling/run-build.py` est un artefact local, pas le point d'entrée
portable. Celui-ci reste `scripts/build-windows.ps1`, exécuté pour la baseline ;
ne pas attribuer les 122 tests finaux à une nouvelle exécution de ce script.
Cette ingestion documentaire ne relance pas les tests moteur.

## Incidents et corrections des affirmations de conversation

- Heap corruption / C0000374, abort / purecall et timeouts 60 s ont été rencontrés.
  Les collisions ODR et le dispatch pur sont des défauts trouvés puis corrigés.
  L'attribution exacte de chaque hang au dialogue CRT reste un diagnostic rapporté.
- L'affirmation « GMock sans action par défaut crée une référence pendante » était
  incorrecte : les sources GMock 1.14 montrent erreur/exception ou arrêt, pas une
  référence temporaire synthétisée. Un `ReturnRef` sur objet détruit serait autre
  chose. La suppression de l'accesseur mocké reste un choix valable.
- C1060 a coïncidé avec une mémoire engagée presque épuisée ; compilation réussie
  ensuite. Ne pas répéter un build coûteux sans mesurer RAM **et commit Windows**.
- Le test `PassAccessorsOnLiveObject` oubliait d'insérer le shader ; corrigé sans
  affaiblir l'attendu. Le test d'ordre de destruction seedait des IDs puis appelait
  `load()` ; adapté au contrat B pour créer réellement ses handles via le mock GL.
- `qmd embed -c artist-wiki*` était une faute de commande, non une panne QMD.
  La commande correcte a ensuite rencontré `NoBinaryFoundError` CUDA puis
  allocation CPU 513,50 MiB refusée. **La cause spécifique « sandbox » n'est pas
  établie** ; le succès CUDA de la console utilisateur reste une observation distincte.
- Une sortie bufferisée n'exclut pas une notification de fin de l'outil background.
  Vérifier cette notification et le log ; ne pas tuer tous les processus homonymes
  sans identifier les PID appartenant à cette tâche.

## Reprise à privilégier

1. Lire [[architecture/current-state]], [[adr/0003-ownership-et-lifetime-cpu-gpu]]
   et [[development/known-incidents]], puis inspecter le diff courant.
2. Qualifier en priorité l'assignation move sur un owner déjà chargé et les limites
   de re-load ; ne pas extrapoler le test du move-constructor.
3. Exécuter l'expérience ASan séparée, prouver son instrumentation et rapporter
   sa portée réelle. Pas de dépendance permanente Windows-only.
4. Exécuter les tranches suivantes seulement avec un nouveau contrat borné.
5. Actualiser QMD avec la configuration standard ; ne pas masquer un embedding
   manquant derrière le succès de l'index lexical. Pas d'images/mesures GPU présentes.

Navigation : [[index|Accueil]] · [[development/build-and-test|Build]] ·
[[architecture/correction-plan|Plan]] · [[log|Journal]].
