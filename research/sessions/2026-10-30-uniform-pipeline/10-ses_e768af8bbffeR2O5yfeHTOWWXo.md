# pool-1 (@engine-architect teammate)

Session: ses_e768af8bbffeR2O5yfeHTOWWXo
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T21:01:00.323Z — msg_18977501e0012b2QovECLptitr

# Contrat candidat à ratifier par le superviseur

**Livrable documentaire uniquement ; aucun fichier modifié.**  
La ratification et le commit de ce contrat précèdent les worktrees auteurs. Aucun build, test moteur, rendu ou benchmark exécuté.

Métadonnées proposées pour une ADR, numéro attribué par le superviseur :

```yaml
title: "ADR-NNNN — Pipeline portable de vérification des uniforms"
adr_id: ADR-NNNN
category: adr
tags: [artist, architecture, testing, tooling, adr]
status: Proposed
implementation: not-started
decision_origin: "Mission mission-becf8aaba29cd624 ; contrat soumis au superviseur"
summary: "Séparer catalogue, plan, préparation, construction, exécution et preuves."
created: 2026-10-29
updated: 2026-10-29
```

## 1. Faits avec chemins et symboles

Révision observée : `fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6`. Le checkout contient des modifications de profils et des fichiers non suivis ; **HEAD seul ne représente donc pas toute son identité**.

### Chaîne réelle des updates

- `ARTIST/include/graphic/pipeline/Pass.hpp`, `IPass::withUniform` :
  recherche par `ctx().getUniform(name)`, puis `Uniform::set<T>`.
- `ARTIST/include/graphic/context/PassContext.hpp` :
  `UniformMap` est une `unordered_map<string, unique_ptr<Uniform<API>>>`.
  `getUniform` effectue `contains`, puis `at` et renvoie un pointeur emprunté.
- `ARTIST/include/graphic/pipeline/Uniform.hpp`, `Uniform::set` :
  sélection statique du setter ; prévalidation éventuelle, modification du miroir CPU, puis `Setter::on`.
- `ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp` :
  le setter float valide de nouveau et appelle
  `glUniform1f(..., uniform.getValue<GLfloat>())`.
- `ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp` :
  `prepare` résout le nom et vérifie réflexion, programme lié, contexte et scalaire float.
  `set(float)` vérifie témoins faibles/générations, contexte et programme courant, modifie le miroir CPU, puis appelle `glUniform1f`.

**Conséquence :** une éventuelle différence entre les routes ne serait pas attribuable au seul lookup. Les validations, lectures du miroir et emprunts diffèrent aussi. Le chemin préparé conserve notamment une modification du miroir CPU ; il ne prouve pas « zéro allocation ».

### Infrastructure réutilisable

- `scripts/build-linux.py --configure-only` et
  `scripts/build-windows.ps1 -ConfigureOnly` fournissent la préparation existante.
- Attention : `--configure-only` Linux appelle **Conan install puis CMake** ; ce n’est pas une simulation sans effets.
- `scripts/linux-evidence.py` fournit des précédents utiles :
  création exclusive des logs, réconciliation découverte/JUnit, empreintes avant/après incluant les shaders, conservation du code enfant.
- Son `main` impose llvmpipe et lance les labels moteur `unit` et `opengl`. **Il ne convient pas comme orchestrateur directement réutilisé** pour les trois profils demandés.
- Scénarios float existants :
  `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`,
  `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp`,
  `PreparedFloatUniformRenderTests.cpp`.

### Autorité documentaire

Vérifiés dans les fichiers locaux, après recherche QMD `artist-wiki` et lecture de `development/linux-qa.md` :

- ADR-0004 : **Proposed / partial** ; seule la tranche float est adoptée.
- ADR-0006 : **Proposed / not-started** ; aucun protocole comparatif exécuté établi.
- `docs/engineering-principles.md` exige préparation distincte de l’exécution, variantes bornées et preuves sans confusion avec les intentions.

Les résultats historiques Windows/Linux ne qualifient pas ce futur pipeline.

## 2. Cible et écarts

### Cible

Livrer un outil Windows/Linux **pour qualification ultérieure**, avec scénarios réels et sélection explicite, mais ne valider pendant cette mission que son orchestration sur données synthétiques et programmes factices.

### Écarts à combler

- Absence des interfaces portables proposées ci-dessous.
- Contrôleur existant spécialisé Linux logiciel et campagnes moteur.
- Pas de séparation actuelle suffisante entre état du pipeline, verdict des tests et validité des mesures.
- Identité et schémas des nouveaux workloads à définir avant leur réalisation.
- Aucune preuve d’exécution des nouveaux workloads sur Windows ou Linux.

## 3. Contrat proposé

### 3.1 Implantation et responsabilités

Chemins proposés, à figer dans le contrat ratifié :

```text
scripts/verify.py
scripts/verification/
    catalogue.py
    model.py
    planning.py
    adapters.py
    evidence.py
    reporting.py
scripts/tests/test_verification_pipeline.py

ARTIST/verification/
    CMakeLists.txt
    CpuUniformWorkload.cpp
    OpenGLFloatWorkload.cpp
    shaders/float.vert
    shaders/float.frag

.github/workflows/verification-pipeline.yml
```

- `verify.py` : CLI seulement.
- `catalogue.py` : descriptions déclaratives, sans découverte système.
- `planning.py` : construction pure d’un plan.
- `adapters.py` : opérations Windows/Linux et lancement des processus.
- `evidence.py` : identités, journaux, résultats bruts et validation de provenance.
- `reporting.py` : lecture de preuves, sans reconstruction ni nouvelle exécution.
- Workloads C++ : cibles isolées, explicitement demandées ; aucun ajout aux suites moteur exécutées par défaut.
- Workflow **manuel**, limité initialement aux autotests synthétiques du pipeline. Aucune campagne réelle automatique.

**Choix soumis à ratification : Python standard library pour l’orchestration.** Aucun ajout de dépendance Python ou C++ de mesure proposé dans cette tranche.

### 3.2 Interfaces d’orchestration

Interfaces conceptuelles, réalisables par dataclasses immuables et fonctions :

```text
list_scenarios() -> tuple[ScenarioSpec, ...]
make_plan(Request, Catalogue) -> VerificationPlan

prepare(VerificationPlan, PlatformAdapter, EvidenceSink) -> PreparationReceipt
build(VerificationPlan, PreparationReceipt, Adapter, Sink) -> BuildReceipt
execute(VerificationPlan, BuildReceipt, ExecutionAdapter, Sink) -> ExecutionReceipt
report(EvidenceBundle) -> QualificationReport
```

Types minimaux :

- `ScenarioSpec` : ID/version, profils permis, opérations, oracle, métriques éventuelles, entrées et sorties attendues.
- `VerificationPlan` : sélection, plateforme demandée, profil, configuration, paramètres, phases et commandes prévisionnelles.
- `CommandSpec` : tableau d’arguments, répertoire de travail et delta d’environnement ; **pas de commande shell interpolée**.
- Receipts : version de schéma, hash du plan, identités des entrées, état de phase et références aux preuves.
- `ExecutionAdapter` interchangeable avec un adaptateur factice pour les autotests ; pas de framework de plugins ni de découverte dynamique.

Une phase refuse un receipt absent, incompatible ou périmé. Un rapport n’exécute jamais une phase manquante.

### 3.3 Modes sans effets

Commandes proposées :

```text
python -B scripts/verify.py list
python -B scripts/verify.py plan --platform windows --profile opengl-hardware ...
python -B scripts/verify.py prepare ...
python -B scripts/verify.py build ...
python -B scripts/verify.py run ...
python -B scripts/verify.py report <bundle>
```

**`list` et `plan` :**

- aucun sous-processus, réseau, téléchargement ou création de dossier/fichier ;
- aucune interrogation Git, Conan, CMake, compilateur ou driver ;
- aucun contexte OpenGL ;
- sortie vers stdout uniquement ;
- imports sans effets ; désactivation des écritures de bytecode avant import des modules locaux ;
- plateforme et paramètres explicitement fournis, inconnues marquées `unresolved`.

Le plan ne prétend pas connaître le driver, l’exécutable final ou l’identité Git effective. Ces informations sont relevées ultérieurement.

**Effets uniquement par sous-commande explicite.** `run` ne prépare ni ne construit implicitement.

La préparation appelant Conan exige un consentement explicite à la résolution de dépendances ; ne pas présenter les helpers actuels comme garantis hors ligne. Aucun provisioning système, installation privilégiée ou réservation cloud.

### 3.4 Profils

- **`cpu-only`** : aucun contexte ni appel OpenGL ; vérifications du miroir CPU uniquement. Ne démontre pas un upload.
- **`opengl-software`** : contexte réel, rendu logiciel explicitement demandé et observé. Première qualification prévue : Linux llvmpipe existant.
- **`opengl-hardware`** : contexte réel et identité du dispositif/pilote vérifiée contre l’environnement attendu.

Pour les deux profils OpenGL, enregistrer version, vendor, renderer, extensions nécessaires et mode de contexte. Un classement ambigu est `UNVERIFIED`, pas « matériel » par défaut.

Un profil indisponible est déclaré non exécuté ou incompatible. **Aucun repli silencieux matériel → logiciel.** Le logiciel Windows n’est pas provisionné implicitement.

Même une mesure **CPU de soumission des uploads OpenGL** exige un contexte réel et le programme courant. Elle n’est ni du CPU seul, ni du temps GPU.

### 3.5 Catalogue borné

IDs proposés :

1. **`cpu.float-mirror`**  
   Séquence déterministe d’écritures/lectures du miroir CPU par les API de contexte existantes. Oracle : valeurs attendues. Aucune prétention de couvrir `Uniform::set` OpenGL.

2. **`gl.float-routes-correctness`**  
   Même programme, mêmes floats et mêmes shaders ; quatre routes :
   - `Pass::withUniform<float>` ;
   - `PassContext::getUniform(name)->set<float>` à chaque update ;
   - `Uniform::set<float>` sur emprunt préalablement résolu ;
   - `PreparedFloatUniform::set`.

   Oracle : miroir CPU, lecture native et sortie pixel fixe discriminante, programme leurre préservé. Les deux premières routes sont proches par construction ; leur comparaison ne crée pas deux mécanismes indépendants.

3. **`gl.float-preparation`**  
   Préparation après link/réflexion, distincte des updates. Cas nominaux et refus : nom absent, type incorrect, array compris taille un, contexte incorrect, métadonnées invalidées.

4. **`gl.float-lifetime`**  
   Emprunt révoqué après remplacement/retrait/destruction autorisée de l’owner. Oracle : refus avant accès au pointee ; aucun handle périmé accepté.

5. **`gl.float-submission-cpu`**  
   Workload réel, mêmes routes et flux d’updates ; instrumentation CPU optionnelle par lots. Oracle de correction hors fenêtre chronométrée. Durée de préparation, durée de soumission et vérification finale séparées.

Les scénarios 2–5 nécessitent un profil OpenGL. Ils sont **livrés sans exécution dans cette mission**.

Exclus : timestamps GPU, autres types d’uniforms, UBO/SSBO, nouveaux backends, évolution générale du render graph et provisioning.

### 3.6 Moments de résolution et bornes

- **Compile-time** : float scalaire, contrats C++ et fonctions des quatre routes. Aucun type généré par valeur, nom, répétition ou taille de lot.
- **Link-time** : assemblage des deux exécutables et dépendances déjà présentes ; options de compilation/link enregistrées. Aucun bénéfice LTO présumé.
- **Préparation du pipeline** : configuration outillage et manifeste.
- **Préparation runtime OpenGL** : contexte, compilation/link GLSL, réflexion et bindings. **Ce n’est pas le link C++ ni un cook offline.**
- **Runtime** : valeurs, programme courant, exécution des lots et observations driver.

Borne initiale : deux exécutables, quatre routes float, un couple de shaders. Sélection d’une route une fois par lot, pas par update. Les tailles et répétitions sont des données runtime. Cette borne limite instanciations et coût de build sans promettre un gain.

Le plan immuable est un **plan de vérification**, pas un render graph ni un objet driver. Aucun graphe d’authoring ou de frame n’est introduit.

### 3.7 Ownership et lifetime

- L’orchestrateur possède son plan et son répertoire de preuves ; un écrivain par bundle, créations exclusives.
- Le workload possède contexte/fenêtre, pass, shaders et ressources graphiques.
- Les pointeurs `Uniform*` sont empruntés ; l’accès préparé porte des témoins/générations locaux, pas une propriété du programme natif.
- Préparation et exécution mono-thread, sérialisées, sans relink/delete natif externe ni mutation concurrente des owners.
- Les générations protègent l’accès CPU périmé ; **elles ne prouvent pas la fin d’utilisation GPU**.
- Le workload termine explicitement les usages nécessaires avant démontage des ressources/contexte, selon les garanties OpenGL ; les attentes et readbacks restent hors mesure de soumission et sont consignés séparément.
- Aucun système général de retrait GPU en vol n’est ajouté.

### 3.8 États indépendants

Chaque phase conserve :

```text
pipeline_state:
  NOT_STARTED | RUNNING | COMPLETED | FAILED | INTERRUPTED | BLOCKED

test_verdict:
  NOT_EXECUTED | PASS | FAIL | INCONCLUSIVE

measurement_validity:
  NOT_REQUESTED | ABSENT | VALID | INVALID

freshness:
  CURRENT | STALE | UNVERIFIED
```

Exemples impératifs :

- Programme exécuté correctement mais oracle faux :
  `COMPLETED / FAIL`.
- Exécutable introuvable :
  `FAILED / NOT_EXECUTED`.
- Processus interrompu sans résultat complet :
  `INTERRUPTED / INCONCLUSIVE`.
- Résultat ancien :
  `STALE`, jamais preuve du candidat courant.
- Mesures manquantes :
  `ABSENT`, jamais zéro.
- Programme retournant zéro sans résultat conforme :
  panne de protocole, pas `PASS`.
- Zéro scénario sélectionné, doublons, omissions ou skips requis :
  sélection rejetée ou verdict non concluant, jamais succès.

Le code de sortie CLI sert à l’automatisation ; il ne remplace pas ces champs ni le code enfant brut.

### 3.9 Preuves et protocole des workloads

Bundle proposé :

```text
out/verification/<UTC-UUID>/
    plan.json
    manifest.json
    identity-before.json
    identity-after.json
    environment.json
    commands.jsonl
    logs/
    raw/
    report.json
```

À conserver :

- HEAD, état dirty, diff et empreintes des fichiers pertinents non suivis ;
- scripts, catalogue, contrat ratifié, configurations et shaders ;
- exécutables réellement lancés et leurs empreintes ;
- compilateur/STL/flags, configuration et dépendances pertinentes ;
- OS, contexte/driver observés, environnement transmis expurgé ;
- argv, cwd, horaires, code enfant, interruption et stdout/stderr bruts ;
- paramètres, comptes attendus/observés, observations et échantillons bruts.

Protocole C++ : demande JSON versionnée en entrée, résultat JSON versionné dans un fichier exclusif ; stdout/stderr réservés aux journaux. Toute réponse doit référencer demande, scénario, route et compte d’opérations.

Les empreintes avant/après détectent les changements persistants ; elles ne constituent pas une garantie transactionnelle contre une modification transitoire annulée. Aucun écrivain concurrent supporté.

### 3.10 Mesure éventuelle

Proposition à ratifier : `std::chrono::steady_clock`, sans nouvelle bibliothèque, pour les durées CPU par lot.

Enregistrer unité, nombre d’opérations, warm-up, répétitions, échantillons et limites de résolution. Vérifications/readbacks hors fenêtre ; pas de `glFinish` par update. Une durée de soumission peut inclure du travail ou des blocages driver : ne pas l’étiqueter temps GPU.

Aucun seuil de gain, classement ou comparaison logiciel/matériel n’est requis. Toute mesure insuffisante reste invalide ou inconclusive.

## 4. Alternatives, conséquences et migration

**Alternative plus simple :** commandes documentées autour des scripts existants. Moins de code, mais insuffisant pour catalogue sélectionnable, modes purs et états/provenances portables.

**Option proposée :** petite couche Python standard library, helpers de préparation conservés, cibles C++ isolées. Ne pas invoquer les helpers dans leur mode par défaut, qui exécute les suites moteur.

La réutilisation des fonctions de `linux-evidence.py` doit rester explicite et testée ; leurs politiques actuelles ne doivent pas être modifiées silencieusement pour satisfaire le nouveau pipeline. Extraction éventuelle d’un helper pur : modification distincte, avec non-régression.

OpenGL reste le seul backend. Vulkan/D3D12/Metal n’apportent aucune preuve nécessaire à cette tranche et augmenteraient fortement le périmètre.

Migration additive : aucun changement de l’API publique moteur, aucun remplacement des campagnes existantes, aucun statut ADR global modifié.

## 5. Validation et barrière d’admission

### Avant auteurs — superviseur

Ratifier et versionner :

1. chemins, interfaces et catalogue ;
2. Python standard library et absence de dépendance de mesure supplémentaire ;
3. schémas/états et règles de provenance ;
4. réutilisation bornée des helpers ;
5. exclusion de toute exécution réelle pendant la mission.

Cette ratification locale **n’adopte pas globalement ADR-0004 ou ADR-0006**.

### Pendant la mission — uniquement synthétique/factice

RED/GREEN puis non-régression du pipeline pour :

- `list/plan` : aucun processus, réseau ou écriture, y compris bytecode ;
- commande factice réussie, échouée, interrompue ou introuvable ;
- résultat absent/malformé/périmé, code zéro trompeur ;
- sélection vide/doublonnée et comptes incohérents ;
- shader/exécutable modifié entre relevés ;
- matériel demandé, logiciel observé ;
- reprise refusant un receipt incompatible ;
- rapport sans effet d’exécution.

Revue indépendante ciblée utile : **fausses réussites et effets cachés des modes purs**, avec résultats synthétiques. Le lead conserve les obligations QA du projet ; la qualification réelle est hors autorisation.

### Limites de cette remise

**Revue documentaire réalisée** : principes, registre, plan de correction, ADR-0004/0006, contrôleur existant et chaîne des quatre APIs recoupés. Serena rattaché à la racine ARTIST ; analyse source, sans preuve machine ou runtime.

**NON_EXECUTE** : autotests du nouveau pipeline, compilation des workloads, Windows/Linux réels, contexte, pixels et mesures. Les échanges historiques `oc://` n’ont pas été récupérés ; aucune conclusion supplémentaire ne leur est attribuée.

**Conclusion : contrat candidat directement utilisable après ratification et commit du superviseur ; aucune admission ni décision globale implicitement acquise.**

## assistant — 2026-10-29T21:02:50.078Z — msg_18978fcd8001Qx4tWUaJaG64xX

Complément intégré à la remise :

- HEAD : `fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6`.
- Diff préexistant profils/rapports/`.tmp` exclu du périmètre et de l’intégration ; aucune modification effectuée.
- Contexte graphique conservé : **GLFW/OpenGL 3.3 core/GLEW**, sans EGL, ANGLE ni nouveau backend.
- Profils : CPU seul, Linux logiciel **llvmpipe**, OpenGL matériel explicitement vérifié ; aucun fallback silencieux.
- **Aucune dépendance de mesure supplémentaire recommandée** : standard library Python et `std::chrono::steady_clock` suffisent à cette tranche. Google Benchmark reste une extension explicitement séparée, non nécessaire à la livraison.
- C++/GL nouveau livré **pour qualification — NON_EXECUTE**. Aucun RED graphique revendiqué ; RED/GREEN réservé aux scripts et à l’orchestration synthétique/factice.
- ADR-0004 **Proposed/partial**, ADR-0006 **Proposed/not-started**, sans adoption globale.

**Barrière inchangée :** ratification, persistance et commit du contrat par le lead avant admission des auteurs. Le contrat candidat précédent constitue la remise complète ; ce complément en précise les contraintes.

## assistant — 2026-10-29T21:03:37.173Z — msg_18979b4cd0019SsauNQI3reYnK

# Contrat candidat — pipeline portable de vérification des uniforms

**Statut : proposé au lead pour ratification. Implémentation : non commencée pour ce nouveau pipeline.**  
Remise documentaire autonome, sans modification de fichier ni exécution moteur. Le lead persiste et committe le contrat ratifié **avant admission des auteurs et création de leurs worktrees**.

## 1. Faits vérifiés et périmètre

HEAD observé : `fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6`. Les modifications préexistantes de `.opencode/agents/*.md`, rapports d’autres missions et `.tmp` sont hors périmètre : ne pas les modifier ni les intégrer.

Documents consultés : `AGENTS.md`, `docs/engineering-principles.md`, `docs/adr/README.md`, `docs/architecture/correction-plan.md`, ADR-0004 et ADR-0006, template ADR et taxonomie. Recherche QMD limitée à `artist-wiki`, identité confirmée par `.wiki.json`.

- ADR-0004 : **Proposed/partial** ; seule la tranche float est adoptée.
- ADR-0006 : **Proposed/not-started** ; aucune adoption globale par cette mission.
- Les preuves historiques Windows/Linux ne qualifient pas le futur pipeline.

### Chaîne réelle des updates

- `ARTIST/include/graphic/pipeline/Pass.hpp`, `IPass::withUniform` : lookup via `ctx().getUniform(name)`, puis `Uniform::set<T>`.
- `ARTIST/include/graphic/context/PassContext.hpp` : `UniformMap` est une `unordered_map<string, unique_ptr<Uniform<API>>>`. `getUniform` effectue `contains`, puis `at`, et renvoie un emprunt.
- `ARTIST/include/graphic/pipeline/Uniform.hpp`, `Uniform::set` : sélection statique du setter, prévalidation éventuelle, modification du miroir CPU, puis `Setter::on`.
- `ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp` : le setter float valide de nouveau et appelle `glUniform1f` après lecture du miroir.
- `ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp` : préparation après link/réflexion OpenGL ; `set(float)` vérifie témoins/générations, contexte et programme courant, modifie le miroir CPU puis appelle `glUniform1f`.

**Une différence entre routes ne doit donc pas être attribuée au seul lookup** : validations, lecture du miroir et emprunts diffèrent aussi. Aucun gain ni absence d’allocation n’est établi.

### Infrastructure existante

- Préparation : `scripts/build-linux.py --configure-only` et `scripts/build-windows.ps1 -ConfigureOnly`.
- Attention : la préparation Linux appelle Conan install puis CMake ; elle peut télécharger.
- `scripts/linux-evidence.py` fournit des garanties réutilisables : logs exclusifs, population découverte/JUnit, codes enfants, identités avant/après incluant shaders.
- Son `main` force llvmpipe et lance les suites moteur : **ne pas l’utiliser comme orchestrateur du nouveau pipeline**.
- Références de scénarios : `PreparedFloatUniformTests.cpp`, `PreparedFloatUniformComponentTests.cpp` et `PreparedFloatUniformRenderTests.cpp` sous `ARTIST/tests/` et `ARTIST/component-tests/`.

## 2. Cible et exclusions

Livrer un pipeline Windows/Linux **pour qualification ultérieure**, avec catalogue sélectionnable et séparation :

1. description ;
2. planification ;
3. préparation ;
4. construction ;
5. exécution ;
6. rapport.

Pendant cette mission, seuls les autotests du pipeline sur données synthétiques et programmes factices peuvent être exécutés. Les nouveaux workloads C++/GL sont livrés **NON_EXECUTE**, sans RED graphique inventé.

Exclusions : suites moteur, rendu réel, benchmark réel, ressources cloud, provisioning, nouveaux backends, EGL/ANGLE, timestamps GPU, autres types d’uniforms, UBO/SSBO et refonte du render graph.

Backend conservé : **GLFW, OpenGL 3.3 core, GLEW**.

## 3. Implantation et ownership

Chemins proposés à figer par ratification :

```text
scripts/verify.py
scripts/verification/
    __init__.py
    catalogue.py
    model.py
    planning.py
    adapters.py
    evidence.py
    reporting.py
scripts/tests/test_verification_pipeline.py

ARTIST/verification/
    CMakeLists.txt
    CpuUniformWorkload.cpp
    OpenGLFloatWorkload.cpp
    shaders/float.vert
    shaders/float.frag

.github/workflows/verification-pipeline.yml
```

Responsabilités :

- `verify.py` : parsing CLI et dispatch, sans logique métier.
- `model.py` : types, enums, versions de schéma.
- `catalogue.py` : descriptions statiques des scénarios.
- `planning.py` : validation et construction pure du plan.
- `adapters.py` : commandes et environnement Windows/Linux, exécution des processus.
- `evidence.py` : identités, journaux, receipts et validation de provenance.
- `reporting.py` : rapport à partir des seules preuves existantes.
- C++ : deux exécutables isolés, proposés comme cibles `ArtistUniformCpuWorkload` et `ArtistUniformOpenGLWorkload`, non ajoutés aux suites exécutées par défaut.
- Workflow manuel : autotests synthétiques seulement pendant cette mission, sans campagne graphique automatique.

Un auteur possède le cycle RED/GREEN d’une tranche. Le lead attribue les fichiers ; aucun partage concurrent d’écriture. Un seul écrivain par bundle et par arbre de build.

## 4. Décisions proposées

### Dépendances

**Recommandation : Python standard library et bibliothèque standard C++ uniquement pour la nouvelle orchestration/instrumentation.**

- Pas de Google Benchmark implicite.
- `std::chrono::steady_clock` suffit à fournir les durées CPU par lots de cette tranche.
- Les dépendances moteur actuelles restent inchangées.
- Toute bibliothèque de mesure supplémentaire est une extension distincte, justifiée et explicitement acceptée par le lead.

### Frontières de résolution

- **Compile-time** : contrats C++ et quatre fonctions de routes float ; aucun type par valeur, nom ou taille de lot.
- **Link-time** : construction des deux exécutables et dépendances présentes ; flags et options conservés, aucun bénéfice LTO présumé.
- **Préparation outillage** : configuration Conan/CMake et identités.
- **Préparation runtime OpenGL** : contexte, compilation/link GLSL, réflexion et binding préparé. Ce n’est ni le link C++ ni un cook offline.
- **Runtime** : valeurs, programme courant et exécution des lots.

Borne initiale : deux exécutables, quatre routes float, un couple de shaders. Répétitions et tailles sont des données runtime. Dispatch de route une fois par lot, pas par update.

Le plan immuable est un **plan de vérification**, pas un graphe d’authoring, un graphe de frame ou un objet driver.

## 5. API Python

Interfaces conceptuelles stables :

```python
list_scenarios() -> tuple[ScenarioSpec, ...]

make_plan(
    request: Request,
    catalogue: tuple[ScenarioSpec, ...],
) -> VerificationPlan

prepare(
    plan: VerificationPlan,
    adapter: PlatformAdapter,
    sink: EvidenceSink,
) -> PreparationReceipt

build(
    plan: VerificationPlan,
    preparation: PreparationReceipt,
    adapter: PlatformAdapter,
    sink: EvidenceSink,
) -> BuildReceipt

execute(
    plan: VerificationPlan,
    build: BuildReceipt,
    adapter: ExecutionAdapter,
    sink: EvidenceSink,
) -> ExecutionReceipt

report(bundle: EvidenceBundle) -> QualificationReport
```

Types minimaux :

- `Request` : plateforme, profil, scénarios/routes, configuration, paramètres, politique d’effets.
- `ScenarioSpec` : ID/version, profils admis, oracle, métriques optionnelles, entrées/sorties et borne d’opérations.
- `VerificationPlan` : sélection validée, paramètres, phases, commandes prévisionnelles et exigences non résolues.
- `CommandSpec` : `argv: tuple[str, ...]`, `cwd`, delta d’environnement, timeout, logs, effets déclarés.
- Receipts : version de schéma, hash du plan, identités des entrées/sorties et état de phase.

Pas de shell interpolé, découverte de plugins ou import ayant des effets. Les adaptateurs factices implémentent les mêmes frontières que les réels.

Une phase refuse un receipt absent, incompatible ou périmé. Le rapport n’exécute jamais une phase manquante.

## 6. CLI et effets

Formes proposées :

```text
python -B scripts/verify.py list [--json]

python -B scripts/verify.py plan
    --platform windows|linux
    --profile cpu-only|opengl-software|opengl-hardware
    --scenario ID [--scenario ID ...]
    --configuration debug|release
    [--route ROUTE ...] [--iterations N] [--repetitions N]

python -B scripts/verify.py prepare
    <même sélection>
    --output-root PATH
    [--allow-dependency-resolution]

python -B scripts/verify.py build --bundle PATH

python -B scripts/verify.py run --bundle PATH

python -B scripts/verify.py report --bundle PATH [--json]
```

### `list` et `plan`

Interdictions strictes :

- sous-processus, réseau et téléchargement ;
- création de dossier/fichier, notamment bytecode ;
- sondes Git/Conan/CMake/compilateur/driver ;
- contexte OpenGL.

Sortie stdout uniquement. Le point d’entrée désactive l’écriture du bytecode avant les imports locaux. Les imports restent purs.

Le plan porte `unresolved` pour les identités et capacités non observées. Il ne prétend pas avoir découvert l’environnement. Sans sous-commande, afficher l’aide ; aucune exécution implicite.

### Phases à effets

- `prepare` crée exclusivement le bundle et configure ; aucun build ni workload.
- `build` construit uniquement les cibles nécessaires ; aucun test ni workload.
- `run` exige une construction compatible ; aucun configure/build automatique.
- `report` lit les preuves ; aucune exécution et aucune réécriture implicite du bundle.

La résolution Conan potentiellement réseau exige `--allow-dependency-resolution`. Sans cet accord, ne pas invoquer un helper dont le mode choisi peut résoudre/télécharger. Aucun provisioning système.

Ces commandes décrivent l’outil livré ; leur exécution réelle reste interdite pendant cette mission.

## 7. Adaptateurs Windows/Linux

Interface :

```text
PlatformAdapter:
    describe_preparation(plan) -> tuple[CommandSpec, ...]
    describe_build(plan, preparation) -> tuple[CommandSpec, ...]
    describe_execution(plan, build) -> tuple[CommandSpec, ...]
    collect_environment(...) -> EnvironmentEvidence

ProcessRunner:
    run(CommandSpec) -> ProcessOutcome
```

`describe_*` reste pure. Seul `ProcessRunner.run` lance un processus.

### Windows

- Réutiliser `build-windows.ps1 -ConfigureOnly` pour la préparation autorisée.
- Construire ensuite explicitement les cibles workload par CMake.
- Conserver l’environnement compilateur et les garanties existantes.
- Aucun lancement du helper dans son mode par défaut exécutant les suites moteur.
- Profil logiciel non provisionné : indisponible tant qu’aucun adaptateur explicitement supporté n’est fourni.

### Linux

- Réutiliser `build-linux.py --configure-only`, profil Conan versionné et presets compatibles.
- Construire les seules cibles workload.
- Pour `opengl-software`, demander llvmpipe explicitement ; Xvfb peut être utilisé s’il est déjà installé.
- Pour `opengl-hardware`, ne pas injecter les variables forçant le logiciel.
- Aucun lancement de `linux-evidence.py main`, ni installation Xvfb/Mesa.

Les presets existants ne prouvent pas une configuration Release : toute configuration supplémentaire nécessite une définition explicite. Pas de conversion silencieuse Debug/Release.

## 8. Profils et catalogue

### Profils

- **`cpu-only`** : aucun contexte/appel OpenGL ; preuve du miroir CPU seulement.
- **`opengl-software`** : contexte réel, logiciel demandé et observé ; première cible Linux llvmpipe.
- **`opengl-hardware`** : contexte réel, dispositif/pilote observés et compatibles avec l’environnement matériel attendu.

Enregistrer vendor, renderer, version et capacités requises. Une classification ambiguë reste `UNVERIFIED`. Aucun fallback matériel → logiciel.

**Mesurer le temps CPU des uploads OpenGL exige aussi un contexte réel et le programme courant.**

### Scénarios

1. **`cpu.float-mirror`**  
   Écritures/lectures déterministes via les API de miroir du contexte. Oracle : valeurs attendues. Ce scénario ne démontre aucun upload.

2. **`gl.float-routes-correctness`**  
   Même programme, shaders et valeurs ; routes :
   - `pass-name` : `Pass::withUniform<float>` ;
   - `lookup-name` : `getUniform(name)->set<float>` à chaque update ;
   - `borrowed-uniform` : `Uniform::set<float>` sur emprunt résolu ;
   - `prepared-float` : `PreparedFloatUniform::set`.

   Oracles : miroir CPU, valeur native, pixel fixe discriminant et programme leurre préservé. Les deux premières routes sont proches par construction, pas deux mécanismes indépendants.

3. **`gl.float-preparation`**  
   Préparation après link/réflexion ; nominal et refus : nom absent, mauvais type, array taille un compris, mauvais contexte et métadonnées invalidées.

4. **`gl.float-lifetime`**  
   Révocation après remplacement/retrait/destruction autorisée de l’owner. Oracle : refus avant accès au pointee périmé.

5. **`gl.float-submission-cpu`**  
   Même flux d’updates et mêmes routes ; mesures CPU optionnelles par lots. Préparation, soumission et vérification séparées. Oracles hors fenêtre chronométrée.

Les scénarios OpenGL sont livrés **NON_EXECUTE** pendant la mission.

## 9. Contrat CLI C++ → Python

Forme commune :

```text
ArtistUniformCpuWorkload --request REQUEST.json --result RESULT.json
ArtistUniformOpenGLWorkload --request REQUEST.json --result RESULT.json
```

Une invocation exécute un scénario et une route ; la sélection de route précède la boucle.

Demande JSON minimale :

```json
{
  "schema_version": 1,
  "request_id": "opaque-id",
  "plan_sha256": "...",
  "scenario": "gl.float-routes-correctness",
  "scenario_version": 1,
  "profile": "opengl-hardware",
  "route": "prepared-float",
  "iterations": 100,
  "repetitions": 1,
  "measurement_requested": false
}
```

Résultat minimal :

```json
{
  "schema_version": 1,
  "request_id": "opaque-id",
  "scenario": "gl.float-routes-correctness",
  "route": "prepared-float",
  "operations_expected": 100,
  "operations_observed": 100,
  "test_verdict": "PASS",
  "measurement_validity": "NOT_REQUESTED",
  "observations": {},
  "samples": [],
  "environment": {}
}
```

Règles :

- résultat créé exclusivement ; aucun fichier ancien réutilisé ;
- stdout/stderr restent des journaux, pas un protocole à parser ;
- validation stricte des versions, identités, population et paramètres ;
- tout chemin d’échantillons/fichiers reste contenu dans le bundle ;
- arrêt anormal ou résultat incomplet conservé comme tel.

Codes C++ proposés : `0` résultat conforme avec oracle satisfait ; `10` oracle faux ; `20` précondition indisponible ; `30` erreur interne/protocole. Signal/exception native : code brut conservé. Un code zéro sans résultat valide n’est jamais PASS.

## 10. États, codes retour et preuves

### États indépendants

```text
pipeline_state:
  NOT_STARTED | RUNNING | COMPLETED | FAILED | INTERRUPTED | BLOCKED

test_verdict:
  NOT_EXECUTED | PASS | FAIL | INCONCLUSIVE

measurement_validity:
  NOT_REQUESTED | ABSENT | VALID | INVALID

freshness:
  CURRENT | STALE | UNVERIFIED
```

Exemples :

- oracle faux : pipeline `COMPLETED`, test `FAIL` ;
- exécutable absent : `FAILED`, test `NOT_EXECUTED` ;
- interruption : `INTERRUPTED`, test `INCONCLUSIVE` ;
- résultat ancien : `STALE`, jamais preuve actuelle ;
- mesure manquante : `ABSENT`, jamais zéro ;
- zéro scénario, doublon, omission ou skip requis : jamais succès.

Codes CLI Python proposés :

- `0` : opération demandée achevée conformément à son contrat ;
- `2` : demande/plan invalide ;
- `3` : précondition bloquante ;
- `4` : panne pipeline/protocole/identité ;
- `5` : test de domaine en échec ;
- `6` : qualification inconclusive ou mesure demandée non valide ;
- `130` : interruption.

Priorité d’agrégation : interruption, panne pipeline, échec domaine, blocage/inconclusif. Le manifeste conserve toutes les causes et codes enfants. `list/plan` retournant zéro ne signifie pas « tests PASS ».

### Bundle

```text
out/verification/<UTC-UUID>/
    plan.json
    manifest.json
    identity-before.json
    identity-after.json
    environment.json
    commands.jsonl
    logs/
    raw/
    report.json
```

Chaque objet possède `schema_version`, `run_id` et référence au hash du plan.

À conserver :

- commit, dirty/diff et empreintes des entrées pertinentes non suivies ;
- contrat ratifié, catalogue, scripts, configurations et shaders ;
- exécutables réellement lancés et leurs empreintes ;
- compilateur/STL/flags, dépendances, OS et contexte/driver ;
- argv, cwd, environnement transmis expurgé, horaires, code enfant et interruption ;
- stdout/stderr bruts, observations, comptes et échantillons.

Le rapport porte son propre schéma et relie chaque conclusion aux artefacts sources. Il ne transforme pas les preuves historiques en exécution du nouveau candidat.

Identités avant/après : protection contre changements persistants, pas garantie transactionnelle contre modifications transitoires annulées. Mutation concurrente non supportée.

## 11. Ownership CPU/GPU et mesure

Le workload possède fenêtre/contexte, passes, shaders et ressources. `Uniform*` reste emprunté ; l’accès préparé porte des témoins/générations locaux, pas la propriété du programme natif.

Contraintes : mono-thread, sérialisé, sans relink/delete natif externe ni mutation concurrente des owners.

**Les générations ne prouvent pas la fin d’utilisation GPU.** Le workload termine les usages nécessaires avant démontage selon les garanties OpenGL. Attentes et readbacks restent séparés de la mesure de soumission et consignés.

Pour les mesures CPU optionnelles : conserver unité, résolution de l’horloge, warm-up, opérations, répétitions et échantillons bruts. Aucun `glFinish` par update ; aucune durée CPU étiquetée temps GPU. Une mesure insuffisante reste invalide/inconclusive. Aucun seuil de gain requis.

## 12. Alternatives, migration et validation

Alternative simple : commandes documentées autour des scripts actuels. Moins de code, mais insuffisant pour catalogue, plan pur et états/provenances portables.

Option recommandée : couche standard library mince, helpers de préparation conservés, workloads isolés. Toute extraction de fonctions de `linux-evidence.py` doit préserver ses politiques existantes et être testée ; ne pas en généraliser silencieusement le `main`.

Migration additive : aucune modification de l’API moteur ni remplacement des campagnes existantes. Vulkan/D3D12/Metal n’apportent aucune preuve nécessaire à cette tranche.

### Validation autorisée pendant la mission

RED/GREEN puis non-régression, exclusivement synthétique/factice :

- absence de processus/réseau/écriture pour `list/plan`, bytecode compris ;
- succès, code non nul, interruption et exécutable absent ;
- résultat absent, malformé, périmé ou code zéro trompeur ;
- sélection vide, doublons, omissions et comptes incohérents ;
- shaders/exécutables modifiés entre relevés ;
- matériel demandé mais logiciel observé ;
- receipts incompatibles ;
- rapport ne déclenchant aucune exécution.

Revue indépendante ciblée : fausses réussites, provenance et effets cachés des modes purs. Le lead conserve les obligations QA du projet.

### État de preuve de cette remise

**Réalisé :** revue documentaire et source des frontières, helpers et quatre routes ; racine Serena ARTIST confirmée.

**NON_EXECUTE :** nouveau pipeline, ses autotests, workloads, compilations Windows/Linux, contexte, pixels et mesures. Aucun résultat runtime ni gain revendiqué.

**Décision attendue du lead :** ratifier les chemins/interfaces, catalogue, schémas, codes retour, standard library sans dépendance de mesure ajoutée, réutilisation bornée des helpers et exclusions ; persister/committer avant admission des auteurs. Cette ratification locale ne vaut adoption globale d’ADR-0004 ou ADR-0006.