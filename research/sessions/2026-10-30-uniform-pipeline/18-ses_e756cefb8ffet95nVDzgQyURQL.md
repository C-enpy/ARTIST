# pool-7 (@engine-architect teammate)

Session: ses_e756cefb8ffet95nVDzgQyURQL
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T02:12:34.403Z — msg_18a948fdf001EHt2QgVNtmdk6m

## Conclusion proposée au lead

**Retenir B, sous forme minimale : observation et garde additives opt-in dans la fixture historique, sans nouvelle option CMake.** Ratifier d’abord ce raccordement local et son ownership. Ce choix permet de conserver la matrice hardware des deux scénarios sans copier leurs tests ni inventer une preuve externe.

**État : proposition de contrat, non implémentée.** Aucune édition, compilation, exécution de test, rendu ou mesure effectuée.

## Faits vérifiés

- `.opencode/evidence/mission-uniform-pipeline/contract.md:26` interdit à l’autrice scénarios d’éditer les tests existants. Les raccordements et transferts d’ownership relèvent explicitement de l’intégration.
- `hardware-expectation-v1.md:21–23` exige une attribution du **contexte réellement utilisé**, avant programme/upload/draw. Une correspondance vendor/renderer n’est pas une preuve universelle d’accélération physique.
- `scripts/verification/scenarios/descriptions.py:36–62,79–95` décrit :
  - préparation : **1 test OpenGL + 5 tests CPU mockés** ;
  - lifetime : **2 tests OpenGL + 5 tests CPU mockés**.
  Les tests CPU ne deviennent pas des preuves GPU parce qu’ils appartiennent à un scénario GL.
- `ARTIST/component-tests/include/OpenGLComponentTests.hpp` :
  - `OpenGLComponentTest::SetUp` crée et rend courant son contexte GLFW, puis initialise GLEW ;
  - lignes 43–44 : seules les chaînes `vendor | renderer | version` sont imprimées ;
  - aucune observation structurée des versions numériques, masque core ou concordance avec la cible attendue.
- `PreparedFloatUniformComponentTests.cpp:57–60` appelle la fixture commune **avant** les programmes du corps du test.
  - `WrongProgramAndAbsentContextPreserveCpuAndDriver:105–107` détache volontairement le contexte puis restaure **la même fenêtre**. Ce cas négatif doit rester inchangé.
- `ShaderLifetimeComponentTests.cpp:252–280` vérifie la suppression différée d’un programme courant jusqu’à son débind. Cela ne démontre pas une politique générale de destruction des ressources GPU en vol.
- Core isolé, `scripts/verification/evidence.py::_junit:720–730` :
  - logiciel : exige une ligne `[OpenGL]` contenant llvmpipe pour chaque cas OpenGL ;
  - hardware : refuse systématiquement les cas historiques faute d’attribution structurée.
  **Attention : cette garde intervient à la collecte JUnit, donc après l’exécution éventuelle des tests**, pas avant leurs uploads.
- `scripts/verification/scenarios/protocol.py::_hardware_environment:84–108` fournit déjà les invariants de validation hardware des nouveaux workloads.

Le constat du `GREEN_READY.md:86–99` est donc fondé : le refus actuel protège la conclusion de qualification, mais ne constitue pas une garde native préalable des tests historiques.

## Cible et écarts

La cible n’est pas une nouvelle architecture de rendu : c’est une **preuve contextualisée pour trois cas OpenGL sélectionnés**, avec refus préalable lorsque leur contexte ne satisfait pas la demande hardware.

Deux écarts subsistent :

1. producteur historique sans garde/observation équivalente ;
2. collecteur qui ne sait pas accepter une telle observation.

Un préflight dans un autre exécutable ou un autre contexte ne comble aucun de ces écarts.

## Contrat proposé

### Producteur historique

Ajouter dans `OpenGLComponentTest::SetUp`, après GLEW et avant retour au test, un chemin **opt-in explicite de qualification hardware** :

- entrée transmise par les adaptateurs dans `CommandSpec.env_delta`, issue du plan ratifié ; pas de cible devinée ni de configuration cachée ;
- conserver les chaînes exactement, sans normalisation différente du protocole existant ;
- vérifier `glfwGetCurrentContext() == window`, versions numériques et masque core sur le thread qui exécute `SetUp` ;
- observer vendor/renderer/version ; comparer exactement à la cible ; refuser les renderers logiciels connus ;
- produire une observation structurée versionnée, attribuable au cas courant et à la requête/au plan ;
- en cas d’absence, discordance ou ambiguïté : conserver le diagnostic et **ne pas entrer dans le corps du test**.

Hors opt-in, conserver le comportement historique et ses assertions. La voie négative « contexte absent » dans le corps du test reste intentionnelle : ne pas ajouter une garde globale qui empêcherait son exercice.

Transport minimal recommandé : **ligne structurée dédiée dans le `system-out` du cas JUnit**, en conservant la ligne historique. Cela réutilise le transport déjà consommé par `_junit`, sans nouveau fichier partagé concurrent ni hypothèse sur la propagation des propriétés GoogleTest dans CTest.

L’encodage, l’échappement et l’attribution doivent être définis avant édition ; une simple concaténation de chaînes natives en pseudo-JSON serait insuffisante.

### Collecteur

Dans `scripts/verification/evidence.py::_junit` :

- exiger l’observation structurée pour chaque cas `opengl.*` sélectionné en hardware ;
- vérifier son identité et ses champs contre le plan, indépendamment du code de sortie ;
- rejeter absence, duplication ambiguë, discordance ou contexte non conforme ;
- ne pas demander cette observation aux cas `unit.*` ;
- distinguer refus de précondition, échec d’assertion et panne de protocole ; préserver codes et sorties brutes.

Le format proposé est distinct du résultat complet des workloads ; il ne doit pas prétendre fournir leurs compteurs d’opérations ou leurs mesures.

### Binding times et propriété

- **Compilation/link** : fixture et instrumentation additive ordinaires ; aucun nouveau profil template, aucune instanciation par GPU ou cible.
- **Préparation** : plan immuable avec cible et sélection ; aucun contexte ni objet driver dans le plan.
- **Runtime** : observation du contexte réellement créé, une fois par cas sélectionné ; coût amorti sur le cas entier, hors toute fenêtre de mesure.
- **Cook / graphes** : non applicables. Aucun graphe d’authoring ou de frame introduit.
- La fixture conserve l’ownership de sa fenêtre/contexte. L’observation copie des valeurs ; elle ne transporte ni emprunt, ni handle natif réutilisable.
- Aucun changement des owners, générations, invalidations ou règles de destruction CPU/GPU. Une identité de contexte n’est pas une preuve de fin d’utilisation GPU.

## Alternatives et conséquences

**A — deux sélections CTest limitées au logiciel : plus simple, mais réduction explicite du contrat.**

- Coût faible ; aucune édition C++ historique.
- Les workloads nominaux et de mesure restent disponibles en hardware.
- Cependant, logiciel Windows étant indisponible en v1, ces deux scénarios n’y auraient aucune route qualifiable. Ils ne pourraient plus être présentés comme couverture hardware livrée.
- Acceptable uniquement après ratification de cette matrice réduite ; pas une correction silencieuse du catalogue.

**B minimal — recommandé.**

- Coût modéré : fixture commune, collecteur et transport des adaptateurs.
- Risque principal : blast radius de la fixture et classement des refus avant corps de test.
- Aucun changement de CMake nécessaire si le code reste dormant hors opt-in. Une option de compilation ajouterait une variante de build et une identité supplémentaire sans besoin démontré.
- Aucune dépendance ni backend supplémentaire.

**Autre solution plus petite : aucune démontrée pour maintenir la même couverture hardware.** Recréer les tests dans le nouveau workload dupliquerait leurs oracles ; qualifier par préflight externe serait insuffisant.

## Ratification et fichiers

Avant implémentation, le lead peut ratifier dans le mandat existant :

1. l’extension locale du protocole d’observation ;
2. le refus natif avant le corps du test ;
3. le transfert explicite d’ownership de la fixture à un auteur désigné ;
4. les règles d’agrégation précondition/assertion/protocole.

Fichiers concernés :

- `.opencode/evidence/mission-uniform-pipeline/hardware-expectation-v1.md` ;
- `ARTIST/component-tests/include/OpenGLComponentTests.hpp` ;
- `scripts/verification/evidence.py` ;
- `scripts/verification/adapters/windows.py`, `linux.py` pour le transport déclaré ;
- tests synthétiques correspondants ;
- `docs/development/verification-pipeline.md` pour la matrice et les limites.

Pas d’édition nécessaire des corps des trois tests, des sources moteur ou des backends. Ce raccordement n’adopte aucun ADR globalement.

## Validation et limites

Dans cette mission, validation autorisée uniquement sur données synthétiques/processus factices :

- observations nominales, absentes, dupliquées, malformées ou discordantes ;
- masque/version/currentness incorrects ;
- renderer logiciel même si attendu ;
- chaînes exactes et échappement ;
- attribution par cas, mélange CPU/OpenGL, non-zéro et refus de précondition ;
- pureté `list/plan`, transport explicite et non-régression historique.

**C++/GL Windows et Linux : NON_EXECUTE**, y compris après une revue statique favorable. Livraison pour qualification, pas fonctionnement graphique démontré.

Recherche documentaire effectuée dans `artist-wiki`, identité `.wiki.json` vérifiée. Statuts conservés : ADR-0001/2/4/6/7 Proposed ; 0003/5 Accepted/partial ; 0008 vérifié à sa baseline Windows Debug seulement. Navigation Serena rattachée à ARTIST ; lecture ciblée des sources, sans preuve de compilation C++.

**Bloquant pour accepter la couverture hardware historique actuelle : absence du producteur/collecteur équivalent.** Ce défaut ne bloque ni les validations synthétiques autorisées ni la livraison séparée des nouveaux workloads.