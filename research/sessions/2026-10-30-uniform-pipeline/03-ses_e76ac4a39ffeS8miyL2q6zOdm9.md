# Concevoir scénarios QA discriminants

Session: ses_e76ac4a39ffeS8miyL2q6zOdm9
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T20:28:21.248Z — msg_189596b7c001Nm2hxDl4abQYCz

# Plan de lot — comparaison float par nom / préparé

**Destination proposée au lead :** `.opencode/evidence/mission-gpu-qa/qa-plan.md`  
Restitution textuelle uniquement : aucun fichier ni test écrit, aucune commande exécutée.

## BLOCKED

- **Action :** restitution par `team_message` au lead.
- **Refus observé :** `This session is not in a team.`
- Aucun nouvel essai après ce retour. La présente réponse constitue la remise textuelle.
- Aucun grant d’écriture vérifié : persistance et commit restent au lead. L’enforcement autonome demeure **INCONCLUSIVE**, sans bloquer l’analyse ci-dessous.

## 1. Pin observé et périmètre

HEAD lu localement : **`fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6`**, conforme au brief. Delta de travail et SHA256 non établis.

Les anciens pins `e4b0aa7` et `40a7cc3` ne sont pas appliqués. Le canon du 29 octobre indique notamment :

- ADR-0004 : **Proposed/partial**, seule tranche float adoptée ;
- ADR-0006 : **Proposed/not-started** ; protocole local à faire adopter ;
- preset Release présent, **qualification Release absente**.

### Ce que compare réellement le banc

| Route | Travail observé |
|---|---|
| **A — par nom** | `Pass::withUniform` → `PassContext::getUniform` (`contains` puis `at`) → `Uniform::set<float>` : validation avant commit `std::any`, nouvelle validation, extraction puis upload |
| **B — préparée** | `PreparedFloatUniform::set` : deux témoins faibles/générations, contexte, programme courant, commit `std::any`, upload direct |
| **C — diagnostic** | `Uniform&` résolu une fois, puis **le même** `set<float>` que A |

`UniformReader.hpp:95` résout déjà la location native pendant la réflexion. **A/B ne mesure donc pas simplement la suppression de `glGetUniformLocation`.** C aiderait à distinguer l’accès par nom des autres différences, sans isoler à lui seul chaque coût.

Périmètre recommandé : **banc et supports de test uniquement**, aucune modification de `ARTIST/include/`, aucune garde retirée, aucune dépendance ni backend supplémentaire.

## 2. Top 3 des risques QA

Les probabilités reflètent notamment l’absence de banc qualifié et les chemins conditionnels ; elles n’augmentent pas artificiellement l’impact.

| Risque | I / P / priorité | Défaillance, déclencheur et blast radius | Détection / mitigation actuelles | Gap à fermer |
|---|---:|---|---|---|
| **R1 — travail différent déclaré équivalent** | **4 / 4 / 16 — critique** | Update/draw omis, valeur figée ou cible decoy ; décision d’optimisation fondée sur un rendu incorrect | Oracle pixel indépendant existant, miroir CPU et readback ; FBO linéaire et état fixé | Vérifier chaque objet, les transitions et les comptes ; détecter les mutants du banc |
| **R2 — faux gain chronométrique** | **4 / 4 / 16 — critique** | Synchronisation cachée, ordre fixe, timer/export faux ou préparation exclue ; toutes les conclusions de coût deviennent trompeuses | Aucun banc observé ; documentation séparant correctness et performance | Horloge, A/A, répétitions appariées, dispersion, coûts déplacés et export indépendamment contrôlés |
| **R3 — mauvais contexte/programme ou accès périmé** | **4 / 3 / 12 — haut** | Retraite, changement d’owner/génération/contexte ; mauvaises mutations, crash ou échantillons invalides | Tests CPU de révocation et test GL de refus ; gardes existantes et API sérialisée | Raccorder ces préconditions au banc et vérifier la reprise, hors mesure |

Réévaluation obligatoire après intégration, surprise de pilote ou changement de candidat.

## 3. Scénarios du noyau

### S1 — Équivalence observable du travail et de l’image

**Nominal**

- A et B utilisent les mêmes propriétaires vivants, programme, état GL, données et chaîne préconstruite.
- Séquence minimale : `0 → .5 → 0`, puis répétition d’une valeur identique.
- Chaque route satisfait **son oracle fixe**, le miroir CPU et le readback natif ; l’égalité A/B complète ces preuves sans les remplacer.
- Lots initiaux **1 / 16 / 256**, sous plafonds fixés après pilote.

**Recommandation discriminante :** un atlas de cellules de **32×32 par objet**, soit des FBO de 32, 128 et 512 pixels de côté. Chaque objet reçoit un viewport distinct, puis **un update utile suivi d’un draw**. Les points fixes de l’oracle existant sont translatés dans chaque cellule.

Cela évite le near-miss d’un banc ne vérifiant que le dernier draw, qui pourrait laisser passer l’omission d’un update intermédiaire. Le coût commun des viewports fait partie du workload déclaré.

**Négatifs**

Mutants **du banc**, sans toucher aux gardes de production :

- omettre un update intermédiaire ;
- omettre un draw ;
- figer toutes les valeurs ;
- envoyer les updates au decoy.

Oracle attendu : image individuelle incorrecte, cible native incorrecte ou trace de travail incohérente.

L’oracle actuel couvre deux valeurs distinctes. Des valeurs signées supplémentaires demandent un oracle géométrique indépendant, adopté avant comparaison — pas une référence régénérée depuis A.

### S2 — Préconditions, refus et transitions

Réutiliser les tests existants plutôt que recopier leur comportement.

- **Nominal :** accès valide ; restauration du programme/contexte ; nouvelle préparation après renouvellement autorisé.
- **Négatif :** programme nul/incorrect, contexte absent/autre, binding révoqué.
- **Oracle :** refus avant mutation CPU/upload pour les refus déterministes ; ancien accès toujours invalide après renouvellement ; nouvel accès opérationnel.

Ces contrôles restent **hors chrono**. C ne doit jamais conserver sa référence au-delà de l’owner. Pas de stress concurrent inventé pour une API explicitement sérialisée et non réentrante.

### S3 — Comptes et mécanismes, dans un run contrôle séparé

Pour `N` objets et `F` frames :

- uploads = draws = **`N × F`** ;
- ordre observé : update attendu, puis draw correspondant ;
- **zéro lookup natif de location dans la boucle pour A comme pour B**, et C si retenu ;
- calibration positive des spies avant l’intervalle observé, forwarding réel et restauration garantie.

Contrôle CPU séparé avec les observations mock existantes :

| Par update | A | B | C |
|---|---:|---:|---:|
| Recherches de nom observées | 2 | 0 | 0 |
| Extractions type-erased observées | 1 | 0 | 1 |

**Négatif :** compteur/export volontairement faux ou événement manquant. Comparer à une attente calculée depuis les entrées et à la trace indépendante, pas à un second champ produit par le même compteur.

Les mocks prouvent les mécanismes CPU, **pas le GPU**. Aucun spy ni compteur de diagnostic dans le run chronométré.

### S4 — Fiabilité du timer, des statistiques et du rapport

Helpers CPU testables avec données synthétiques connues.

- **Nominal :** calculs et unités exacts ; résultat nul ou négatif accepté comme résultat valide.
- **Négatif :** timer figé/non monotone, unité erronée, lot nul, overflow, donnée manquante/dupliquée, export incohérent ou candidat périmé.
- **Oracle :** rejet explicite, jamais gain infini ni PASS silencieux.

Le mutant « faux timer » doit éprouver la calibration et la séparation entre horloge réelle du banc et doubles de test. Un JSON plausible n’authentifie pas une mesure.

### S5 — Préparation, nettoyage et coût total

- Link/réflexion communs séparés de la préparation du binding.
- Mesurer distinctement résolution initiale de C, préparation de B et nettoyage.
- Présenter le coût total pour **K utilisations**, pas seulement la boucle chaude.
- Calculer un amortissement uniquement si l’économie par utilisation est positive et suffisamment établie.

**Négatif :** préparation échouée, coût oublié ou économie nulle/négative : aucune conclusion d’amortissement favorable.

## 4. Protocole de mesure proposé

À adopter localement avant collecte :

1. **Qualifier Release et le contexte natif**, sans extrapoler depuis Debug/ASan ou llvmpipe.
2. Conserver mêmes shaders, binaire, options et charge entre routes ; warm-up symétrique.
3. Pilote exclu des résultats : fixer répétitions, durée maximale et limites de charge, identiques entre routes.
4. Ordre **ABBA/BAAB**, avec répétitions indépendantes par processus et ordre enregistré.
5. Ajouter un **faux A/A**. Fixer préalablement sa bande de nuisance acceptable.
6. Rapporter médiane, dispersion et intervalle apparié ; les mesures internes d’un processus ne sont pas autant de répétitions indépendantes.
7. Aucun retrait d’outliers non prévu, aucune prolongation « jusqu’au gain ».

Deux métriques distinctes :

- **CPU soumission** : attente finale hors intervalle ; backpressure éventuelle explicitée ;
- **durée murale du lot achevé** : attente finale incluse.

Aucune n’est appelée « temps GPU ». Readbacks, logs, assertions et vérifications GL du banc restent hors boucle chronométrée ; **les gardes de production restent dedans**. L’absence de present sur ce FBO interdit toute conclusion sur present/FPS applicatif.

### Extensions, après le noyau

1. **C**, diagnostic peu coûteux recommandé.
2. Boucle update seule, clairement étiquetée microdiagnostic.
3. Queries GPU récupérées de façon différée, nécessaires avant toute revendication de coût GPU.
4. Valeurs/images supplémentaires.

Allocations, code-size, build, scene-to-code et autres types ne sont pas des obligations implicites de ce lot ni une adoption complète d’ADR-0006.

## 5. Implémentation future et TDD

Aucune implémentation n’est autorisée par cette remise.

Ordre recommandé aux futurs auteurs :

1. Lead : arrêter décisions, chemins exacts et artefacts ; persister/committer ce plan avant leurs worktrees.
2. Caractériser les fixtures existantes ; extraire uniquement le support test-only nécessaire, sans affaiblir l’oracle.
3. Implémenter les helpers CPU en **RED causal → GREEN → REFACTOR**.
4. Ajouter les contrôles GL et mutants du banc ; conserver les preuves RED/GREEN.
5. Ajouter un exécutable de mesure distinct des tests CTest : aucun seuil de vitesse ne transforme la correctness en test fluctuant.
6. Intégration réelle, revue indépendante fresh, puis ré-audit par runner distinct.

Chemins candidats à faire confirmer, non créés :

- `ARTIST/tests-common/include/PreparedFloatBenchmarkScene.hpp`
- `ARTIST/tests-common/include/PreparedFloatBenchmarkProtocol.hpp`
- `ARTIST/tests/src/graphic/pipeline/PreparedFloatBenchmarkProtocolTests.cpp`
- `ARTIST/benchmarks/PreparedFloatUniformBenchmark.cpp`
- `ARTIST/benchmarks/CMakeLists.txt`
- adaptations ciblées de `ARTIST/CMakeLists.txt` et du test de rendu existant.

La cible et la stratégie de mocks/image/performance du banc ne sont pas encore adoptées. Ne pas importer une pyramide générique.

## 6. Registre bidirectionnel

**E** : obligation explicite du brief/contrat. **I** : obligation inférée pour rendre la comparaison discriminante, à adopter humainement.

Les tests cités sont des **présences statiques relues**, pas des exécutions du candidat.

| Obligation / intent | `fichier:symbole` et preuve discriminante | État |
|---|---|---|
| **O1 — E : même image/travail utile** | `PreparedFloatUniformRenderTests.cpp:LegacyAndPreparedMatchFixedNonemptyPixels` : oracle indépendant, image non vide, miroir/readback ; extension S1 nécessaire | **GAP** pour le banc courant |
| **O2 — E : gardes conservées** | `PreparedFloatUniformComponentTests.cpp:WrongProgramAndAbsentContextPreserveCpuAndDriver` ; `PreparedFloatUniformTests.cpp:ValueOnlyChangesRemainCompatibleAndWrongTargetDoesNotCommit`, `RetirementCannotResurrectWithReusedNumericProgramId` | **GAP** d’exécution/intégration |
| **O3 — I : charge et mécanismes vérifiés** | `PreparedFloatUniformRenderTests.cpp:PreparedUpdatesDoNotResolveNativeLocationsAgain` ; `PreparedFloatUniformTests.cpp:RepeatedUpdatesObserveNoCpuNativeOrErasedResolution` ; S3 complète A/C, draws et ordre | **GAP** |
| **O4 — I : mesure valide, résultat non présupposé** | Futurs `PreparedFloatBenchmarkProtocolTests.cpp` et `PreparedFloatUniformBenchmark.cpp` ; S4 et A/A | **GAP** |
| **O5 — I : coût déplacé visible** | `PreparedFloatUniform.hpp:prepare` ; S5 doit couvrir préparation et total | **GAP** |
| **O6 — E : preuves fraîches et attribuées** | Rapport futur lié au candidat/binaire/shaders ; contrôle négatif d’identité/export | **GAP** |
| **O7 — E : aucune modification moteur par défaut** | Contrôle du delta final par le lead ; diff de production inattendu ⇒ arrêt | À vérifier à l’intégration |

**Sens inverse :**

- Les tests image existants servent O1 ; leur égalité seule serait une preuve faible.
- Les tests de refus/révocation servent O2 ; CPU et GL ne sont pas des doublons interchangeables.
- Les tests de résolution servent O3, pas O4 ni une promesse d’absence d’allocation.
- Aucun test sélectionné n’est orphelin ; le reste de la suite demeure **unassessed** pour ce lot.
- Aucune acceptation/defer de gap n’est implicite.

### Qualification des preuves reçues

- **Présences locales :** lecture du 29/10/2026, HEAD ci-dessus, outils `read/grep`, environnement dépôt Windows ; SHA256/delta non établis ; aucune commande moteur, aucun runner de test ; comptages de tests **sans objet**, non un zéro-test PASS.
- **Historique communiqué par le lead, corroboré documentairement :** runtime `78e7b74`, six configurations à 589/589, 513 CPU + 76 GL, zéro échec/skip rapporté. Détails de découverte, commandes, runners, SHA256 et delta non inspectés ici. **Autre candidat : preuve périmée pour le HEAD courant**, et aucune preuve performance.

Pour chaque preuve future : HEAD, SHA256, delta, source/date, commande exacte, runner, environnement, attendu/observé et découverts/exécutés/échoués/skipped.

## 7. Budget et décisions ouvertes

Budget indicatif **100 points**, pas engagement calendaire :

| Poste | Points |
|---|---:|
| Setup : Release, GPU, fixtures, données et identités | 25 |
| Images et comptes — R1 | 20 |
| Gardes/transitions — R3 | 8 |
| Timer, statistiques et export — R2 | 16,25 |
| Préparation et banc | 8 |
| Ré-audit planifié | 4 |
| Buffer : investigation 9, panne 5, ré-audit 4,75 | 18,75 |
| **Total** | **100** |

Après setup : **75 % planifiés / 25 % buffer**. Couper d’abord les extensions ; jamais un risque haut ou le buffer pour conserver du moyen. Capacité réelle inconnue : si la qualification Release déborde le setup, **budget à déclarer irréalisable et replanifier**.

Décisions à obtenir :

- adoption du protocole local, sans ratifier tout ADR-0006 ;
- configuration Release, GPU matériel et plafonds ;
- métriques, bande pratique, règle A/A et répétitions avant résultats ;
- chemins/cible/artefacts exacts et capacité disponible.

## 8. Sortie et arrêt

- **PASS protocole** : travail équivalent, oracles et mutants discriminants, mesures/export fiables et preuves fraîches. Un gain **nul ou négatif reste valide**.
- **FAIL** : écart reproduit d’image, de sûreté, de compte ou de calcul.
- **INCONCLUSIVE** : Release/GPU/identité/horloge indisponibles, précision insuffisante, zéro test ou skip requis. La direction du gain peut rester indécidable malgré un protocole valide.

Arrêt sur erreur de sûreté, image/comptes incohérents, changement de candidat ou budget dépassé. Pas d’assouplissement d’oracle, de retrait de garde ni de réparation/installations implicites. Aucun gain moteur n’est revendiqué par ce plan.

## assistant — 2026-10-29T20:36:07.898Z — msg_189608a560018VtmTL0pGMob38

## Plan révisé — livraison du pipeline portable

**Conversation uniquement.** Aucun chemin de persistance demandé ; aucune écriture, exécution ou location de machine. Le succès devient **livrer un pipeline réceptionnable statiquement**, pas obtenir des tests verts ni un gain.

### 1. Contrat de livraison

Prévoir :
- une orchestration commune, avec adaptateurs Linux/Windows minces ;
- configurations CMake/Conan 2 séparées par OS/toolchain/configuration ;
- étapes explicites : préflight → build → découverte → correctness → mesure **optionnelle** ;
- sélection CPU/OpenGL réel, contexte GPU exclusif ;
- rapports conservant erreurs, sorties partielles, identité du candidat et statut de chaque étape.

Windows possède déjà un preset Release ; cela ne le qualifie pas. La configuration Linux optimisée reste à définir explicitement. Le choix du cloud et du GPU reste hors lot : aucune dépendance implicite à une machine précise.

### 2. Réception par revue statique

Contrôler dans le **diff livré**, sans lancer les commandes :

- existence des cibles, scripts, presets et références annoncés ;
- cohérence des arguments, chemins avec espaces, répertoire courant et propagation des codes de sortie ;
- absence de mocks dans le chemin OpenGL réel ;
- préconditions compilateur/STL, ABI, dépendances et capacités GL explicites ;
- absence de retrait de garde ou de modification moteur non convenue ;
- contrat des rapports : candidat, configuration, device, commande, comptages, erreurs et étapes non exécutées ;
- aucun ancien résultat accepté silencieusement.

**Limite actuelle :** aucun nouveau diff de pipeline n’a été reçu. Ce sont des critères de réception, pas un PASS statique déjà prononcé.

### 3. Minimum de scénarios réellement embarqués

Pas de simples placeholders :

1. **Rendu A/B réel** : triangle/FBO existant, valeurs `0 → .5 → 0`, oracle fixe indépendant par route, miroir CPU et readback ; un update utile puis un draw observable par objet.
2. **Refus et reprise** : programme/contexte incorrect, binding périmé ; réutilisation des tests existants, sans affaiblir leurs oracles.
3. **Contrôle du travail séparé** : comptes uploads/draws et absence de résolution native dans la boucle pour **A et B**.
4. **Contrats CPU du banc** : calculs/export sur données synthétiques connues ; résultats nuls et négatifs valides.

C, queries GPU et analyses statistiques étendues sont des extensions, pas des conditions de livraison initiale.

### 4. Pannes du pipeline à prévoir

Chaque panne doit avoir une branche et une sortie définies :

- dépendance/toolchain/device absent ;
- configuration ou build échoué ;
- zéro test découvert, filtre vide ou skip requis ;
- test réellement échoué ;
- timeout/interruption et rapport partiel ;
- artefact périmé, candidat différent, résultats manquants ou dupliqués.

Aucune de ces situations ne devient un succès par absence de résultat. Les scénarios de panne doivent être embarqués ; leur présence relue ne prouve pas leur exécution.

### 5. Trois verdicts indépendants

| Axe | Réception possible maintenant |
|---|---|
| **Pipeline** | Conformité statique aux contrats ; fonctionnement Windows/Linux **INCONCLUSIVE** |
| **Tests** | Scénarios présents et pertinents ; verdict **non exécuté** |
| **Performance** | Collecte prévue ; résultat **non mesuré**, sans objectif de gain |

Sans exécution, restent non qualifiés : compilation/link effectifs, découverte réelle, comportement des adaptateurs, disponibilité GL et fiabilité chronométrique. **Cela n’empêche pas une livraison statique explicitement acceptée ; cela interdit de promettre “fonctionne sur Windows/Linux”.**

Budget indicatif rebasé : setup 25 %, livraison/revue 56,25 %, buffer 18,75 % ; aucune campagne ni provisioning inclus.