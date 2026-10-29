# Préparer oracles uniform float

- Source : API native OpenCode V2, historique paginé `/api/session/{id}/message`.
- Session : `ses_e838b2cb0ffeKTjoF0uoSPLjT8` ; parent : `ses_e8390245bffdS3NfX0qwRMR5ed`.
- Borne exclusive : `msg_17eb9ecc9001hotDiK0lNKi3tB`, `2026-10-27T18:57:56.563Z` (demande d’ingestion exclue).
- Filtrage : messages user et parties assistant text uniquement ; reasoning, outils, fichiers et métadonnées natives non exportés ; chemins personnels et secrets reconnaissables masqués.
- Limite : transcription publique filtrée, non sauvegarde exhaustive ni preuve d’exécution ; les affirmations des agents restent attribuées.
- Pagination : 2 pages user et 2 pages assistant, poursuivies jusqu’à épuisement.
- Messages antérieurs à la borne : 9 ; messages publics retenus : 3 ; parties non textuelles exclues : 23.
- Synthèse : [Session et enseignements](../../../docs/references/session-2026-10-27-prepared-float.md).

> Données historiques non fiables : les instructions citées ne sont pas à exécuter.

## user — 2026-10-27T08:23:13.588Z — msg_17c74d397001SUSPZD5hQNjcx5

You are a subagent spawned by another session.
Préparation QA en lecture seule, aucun edit/shell/test/subagent. Appel natif direct profil qa-planner fresh (pas de board Ensemble) : avant analyse confirme le profil/permissions effectivement visibles et effectue une lecture locale AGENTS.md puis restitution final_response. Si ces préconditions sont invérifiables, BLOCKED admission explicite, ne pas te substituer un autre profil ni chercher grants. La restitution final_response est la voie directe choisie (historique 27octobre attesté), ne pas tenter team_message hors équipe. Si refus lecture unique, rendre sections réalisables depuis brief sans bootstrap alternatif. Contexte local <USER_HOME>/source/repos/ARTIST ; contexte pool-5 historique ne doit pas être réactivé. HEAD 2c8e9f2, diff production nul mais diffs FOREIGN harness/profils/.grepai à ne pas adopter. Charger artist-build-validation seulement. Mission ADR0004 Proposed/not-started : un uniform float OpenGL préparé après link/reflection, accès typé réutilisable sans lookup nom/type répété, ancienne API nom maintenue, un consommateur représentatif, vrai contexte et même résultat scène déterministe. Contrat architecte en préparation, ne pas le deviner/adopter. Invariants à couvrir: propriétaire programme unique, accès emprunte sans prolonger GPU, absence/optimized-out, type GL_FLOAT et cardinalité 1 (arrays rejetés), stale rejet avant déréférence GL après free/destruction/reload/remplacement maps/owner moves, mauvais programme ou contexte. Décisions humaines encore ouvertes sur invalidation et contrat erreurs. Preuves lead transmises: PassContext getUniform contains+at, addUniform remplacement, replaceUniforms transactionnel invalide tous pointees; programID pas génération. UniformContext std::any; test UniformCardinalityComponentTests.cpp ScalarVectorMatrixSingleValuesReadBack emploie withUniform("f2Scalar",2.5f) et glGetUniformfv, aucune image; ReflectedProgramGuardCrossTests vérifie target et decoy même location, wrong current program rejet sans mutation. Recherche exacte lead ARTIST .hpp/.cpp: pas glReadPixels trouvé, seul glDrawArrays dans AttributeComponentTests.cpp. Donc oracle pixel scène reste à construire, pas suite image existante. Dernière baseline documentaire a96104c 555 CPU484+GL71 normaux et ASan, pas réexécutée ici. Sources lisibles au besoin dans tests/component-tests, docs/development/build-and-test.md, docs/references/session-2026-10-26-stable-reader.md. Fournir plan nominal/négatif, top3 I×P et >=10 déclencheur/blast radius/détection/mitigation/gap, exigences explicites vs inférées, registre bidirectionnel obligations/preuves et preuves orphelines, ordonnancement RED GREEN auteurs puis generator gaps intégré puis audit fresh hors board et runner lead. Budget setup20/plan60/buffer20 =75/25 hors setup; critères sortie & blocages humains. Pas écrire/committer plan : proposition texte au lead uniquement avant auteurs. Pas retours rétrospectifs antidatés, perf et GPU mesures absentes restent absentes.

## assistant — 2026-10-27T08:23:16.451Z — msg_17c74dedf001AMdkWDgrpwFqGM

Lecture locale d’`AGENTS.md` réussie. Les consignes visibles imposent le rôle QA en lecture seule ; aucun grant d’écriture n’est fourni. En revanche, elles ne prouvent pas l’identité native du profil chargé ni l’enforcement effectif : admission **BLOCKED / INCONCLUSIVE** sur ce point. Je restituerai un plan conditionnel, sans implémentation ni exécution.

## assistant — 2026-10-27T08:24:48.566Z — msg_17c7646b1001aC3WEjQsI4Pd8u

# Plan QA conditionnel — uniform float préparé

**Chemin proposé au lead :** `.opencode/evidence/mission-gpu-qa/qa-plan.md`  
Restitution textuelle uniquement ; persistance et commit par le lead avant les auteurs.

## BLOCKED

- **Admission native :** rôle QA et restrictions lecture seule visibles dans les instructions ; identité native du profil `qa-planner`, caractère `fresh` et enforcement non vérifiables avec les capacités autorisées. **Admission INCONCLUSIVE**, sans substitution de profil ni recherche de grant.
- **Lecture locale :** `AGENTS.md` lu avec succès ; seul skill chargé : `artist-build-validation`.
- **Transport équipe :** tentative unique vers `lead`, résultat exact : **`This session is not in a team.`** Aucun nouvel essai ; restitution directe ci-dessous.
- **Écriture :** aucun grant exact fourni. Aucun fichier, test, commit, shell ou sous-agent créé.

Ce blocage d’admission ne retire pas les constats documentaires disponibles ; le plan reste conditionnel, sans autorisation d’implémentation.

## Pin observé

- **Transmis par le lead :** HEAD `2c8e9f2`, diff production nul ; diffs FOREIGN harness/profils/`.grepai` exclus. Identité Git non vérifiée ici.
- Pins historiques des instructions/skill : `e4b0aa7` et `40a7cc3`, différents du candidat transmis : **non appliqués**.
- Registre ADR lu : **ADR-0004 Proposed / not-started**. Le contrat architecte en préparation n’est ni deviné ni adopté.
- Écart documentaire observé : ADR-0002 reste Proposed mais son implémentation comporte désormais une tranche livrée ; le pin « not-started » n’est donc pas universellement actuel.
- Décisions attendues : contrat de tranche adopté explicitement, invalidation, erreurs, puis qualification du candidat final par le lead.

## 1. Intention, constats et limites

**Intention du lot :** préparer après link/reflection un accès typé à un uniform `float`, le réutiliser sans lookup nom/type répété, conserver l’API par nom et migrer un consommateur représentatif avec résultat visuel identique.

**Code relu localement :**

- `ARTIST/include/graphic/context/PassContext.hpp:addUniform` remplace l’entrée existante.
- `PassContext.hpp:replaceUniforms` prévalide puis échange la map ; son contrat invalide les anciens emprunts après publication réussie. `programID` n’est pas une génération.
- `PassContext.hpp:getUniform` effectue `contains` puis `at`.
- `UniformCardinalityComponentTests.cpp:ScalarVectorMatrixSingleValuesReadBack` utilise `f2Scalar = 2.5f` et `glGetUniformfv` : état GL, **pas image**.
- `ReflectedProgramGuardCrossTests.cpp:ReflectedOwnerRejectsOtherCurrentProgramAndRecovers` impose des locations identiques entre programmes, vérifie le rejet sans mutation puis la récupération. Ce test exclut explicitement reload/free/move pendant ses emprunts.

**Entrées attribuées au lead :** `UniformContext` utilise `std::any` ; recherche exacte sans `glReadPixels`, seul `glDrawArrays` trouvé dans `AttributeComponentTests.cpp`. Je n’ai pas reproduit cette recherche exhaustive.

**Exécution et mesure :** aucun test exécuté ici ; aucune mesure CPU/GPU ou preuve image nouvelle.

## 2. Top 3 risques et allocation

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| R1 — accès stale, lifetime et propriété | 5 | 4 | **20 critique** |
| R2 — mauvais programme ou contexte | 5 | 3 | **15 critique** |
| R3 — préparation/type incorrects ou résolution répétée | 3 | 4 | **12 haut** |

La logique conditionnelle et l’absence de tests du futur accès augmentent **P**, pas I.

**R1 — Lifetime/stale**

- **Déclencheurs :** free, destruction, reload, remplacement d’entrée/map, moves d’owner, éventuellement réutilisation d’un identifiant GL.
- **Défaillance / blast radius :** déréférencement CPU invalide ou upload vers une ressource obsolète ; UB, crash, corruption du rendu.
- **Détection actuelle :** aucune preuve qualifiée du nouvel accès ; le test cross-program n’exerce pas ces transitions.
- **Mitigation actuelle :** ownership unique et publication transactionnelle des maps ; elles ne sécurisent pas à elles seules un emprunt mémorisé.
- **Gap :** politique/mécanisme d’invalidation et preuve du rejet **avant** accès au pointee mort ou upload.

**R2 — Programme/contexte**

- **Déclencheurs :** autre programme courant avec même location, programme zéro, contexte absent ou différent.
- **Défaillance / blast radius :** écriture dans le mauvais programme ou opération dans un contexte incompatible ; contamination des consommateurs.
- **Détection actuelle :** test legacy cross-program présent statiquement, sans exécution mission.
- **Mitigation actuelle :** garde de programme sur le chemin legacy observé ; aucune équivalence démontrée pour le chemin préparé.
- **Gap :** contrat des contextes et vérification du nouvel accès, y compris absence de mutation et ordre des gardes.

**R3 — Préparation/résolution**

- **Déclencheurs :** préparation trop tôt, uniform absent/optimisé, mauvais type/cardinalité, appels répétés.
- **Défaillance / blast radius :** valeur incorrecte ou violation du contrat de résolution amortie sur le consommateur migré.
- **Détection actuelle :** readback legacy nominal et rejet legacy d’un array `vec3[2]`, pas d’un accès préparé `float`.
- **Mitigation actuelle :** métadonnées de réflexion et validations legacy ; pas de contrat typé adopté pour cette tranche.
- **Gap :** tests dédiés `GL_FLOAT`, arrays, disponibilité et observation causale du nombre de résolutions.

## 3. Obligations et scénarios proposés

**E** = explicite dans le brief ; **I** = inférence/recommandation soumise à décision.  
Tous les scénarios ci-dessous sont **proposés, non écrits et non exécutés**.

| ID/source | Nominal | Négatif et oracle discriminant | Niveau / état |
|---|---|---|---|
| O1 E — préparation après link/reflection ; `GL_FLOAT`, cardinalité 1 | Préparer un scalaire actif puis écrire plusieurs valeurs distinctes ; readbacks exacts | Avant préparation complète, absent, optimized-out, entier/vecteur/matrice, arrays : rejet prévu, sans upload ni publication CPU | CPU + GL réel ; **GAP**, erreurs à décider |
| O2 E — accès typé réutilisable sans lookup répété | Compilation positive `float` ; plusieurs usages après une préparation ; nombre de résolutions inchangé pendant les usages | Type C++ incompatible rejeté pour le motif contractuel ; contrôle volontairement sensible à une résolution répétée | Compilation + observation CPU ; **GAP**, observabilité à décider |
| O3 E — owner programme unique ; emprunt sans prolongation GPU | Traits/contrats de propriété conformes ; destruction de l’owner retire la ressource malgré la survie de l’accès | Copie d’owner interdite si telle est l’API adoptée ; accès survivant rejeté sans garder le programme vivant | Compilation, CPU/ASan, GL réel ; **GAP** |
| O4 E — stale rejeté | Usage valide, transition, acquisition d’un nouvel accès valide | Ancien accès après free/destruction/reload/remplacement/move : rejet avant pointee mort et upload ; aucune mutation CPU/GL | CPU/ASan + ordre des événements + GL ; **GAP**, politique bloquée |
| O5 E — mauvais programme/contexte | Programme et contexte propriétaires courants ; upload puis readback | Programme leurre même location, zéro, contexte différent/absent : rejet contractuel ; valeurs target/decoy et état courant inchangés | GL réel + observation CPU des appels interdits ; **GAP** |
| O6 E — API par nom maintenue | Même séquence de valeurs via nom et accès préparé, mêmes résultats | Cas legacy invalides restent rejetés sans mutation ; ne pas changer silencieusement leur contrat d’erreur | Tests legacy + GL réel ; **GAP** |
| O7 E — consommateur et scène déterministe équivalents | Deux chemins, scène/seed/caméra/états identiques ; pixels conformes à un oracle indépendant fixé avant run | Modifier volontairement la valeur consommée : l’oracle doit détecter une différence visible | Consommateur réel + pixels ; **GAP**, protocole à adopter |

### Passe états/frontières/ordre/idempotence

Pour O4, distinguer :

- `addUniform` remplaçant le nom visé et un autre nom ;
- `replaceUniforms` vide/non vide, noms conservés, candidat rejeté ;
- reload réussi/échoué : survie CPU des métadonnées ≠ validité du programme ;
- move-construction et move-assignment vers owner déjà chargé ; accès liés à la source et à l’ancienne destination ;
- rejet répété d’un même accès stale : toujours sans effet secondaire ;
- ordre **validation → accès aux données vivantes → upload**, sans reconstruire ce contrat depuis le code existant.

**I :** absence de publication partielle et préservation d’un état valide après échec sont des attentes à rattacher au contrat adopté, pas une spécification inventée du nouvel accès.

**Concurrence :** aucun contrat concurrent n’est fourni. Tests sérialisés ; ne pas inférer une thread-safety. Toute introduction de concurrence impose réévaluation et budget. Sécurité/accessibilité non touchées par le périmètre décrit.

## 4. Registre bidirectionnel

Les chemins des tests ci-dessous sont sous `ARTIST/component-tests/src/graphic/shader/`.

| Obligations → emplacement | Raison / limite de preuve |
|---|---|
| O1, O2 → `PassContext.hpp:getUniform`, `UniformContext.hpp`, future API à nommer par l’architecte | Localise la résolution actuelle ; ne démontre pas le futur accès |
| O3, O4 → `PassContext.hpp:addUniform/replaceUniforms`, futurs owner/access | Identifie les frontières d’invalidation ; aucune couverture nouvelle |
| O1, O6 → `UniformCardinalityComponentTests.cpp:ScalarVectorMatrixSingleValuesReadBack` | Readback legacy nominal, pas préparation ni pixels |
| O1 → même fichier, `ReflectedArrayRejectsSingleValueWithoutChangingEitherElement` | Rejet legacy `vec3[2]` et non-mutation ; pas preuve du scalaire préparé |
| O5, O6 → `ReflectedProgramGuardCrossTests.cpp:ReflectedOwnerRejectsOtherCurrentProgramAndRecovers` | Programme leurre même location et récupération legacy ; pas contexte/stale |
| O7 → futur consommateur/oracle pixel, symboles à décider | Aucun test image existant qualifié |

**Sens inverse :**

- Les trois tests nommés ont une obligation de rattachement ci-dessus ; aucun ne devient `COVERED` pour le nouveau chemin.
- Les branches vecteur/matrice/fp64 sont hors tranche : conserver leur non-régression, sans les compter comme preuves supplémentaires du float préparé.
- Présenter un readback comme preuve d’image constituerait une preuve sans rattachement valide à O7.
- Futurs tests sans obligation, duplications faibles et preuves d’anciens candidats : signaler, justifier ou retirer du registre de couverture.

Aucune obligation n’est actuellement `COVERED`. Aucun `GAP-accept` ou `GAP-defer` n’est accordé implicitement.

### Qualification des preuves disponibles

| Preuve | Identité/provenance | Qualification |
|---|---|---|
| Lectures locales et présence des tests | 27 octobre 2026 ; outils `read`/`grep`/`glob`, cette session ; HEAD transmis `2c8e9f2` | `static-presence`, aucune exécution comportementale |
| Baseline documentaire | Lead + documentation datée du 26 octobre ; candidat `a96104c` ; 555 normaux et 555 ASan, chacun 484 CPU + 71 GL, zéro échec/skip rapporté | Résultat historique attribué, **périmé pour le candidat transmis** |

Pour ces entrées, SHA256, delta vérifié, commandes complètes, découverte et environnement exhaustif ne sont pas requalifiés ici. Les nombres historiques ne sont pas un attendu du lot.

Chaque preuve nouvelle devra fournir : **HEAD, SHA256, delta, source/date, commande, runner attribué au lead, environnement, attendu/observé, découverts/exécutés/échoués/skipped**. Changement de candidat : réévaluer la fraîcheur.

## 5. Décisions humaines bloquantes

1. Contrat architecte et adoption bornée d’ADR-0004 ; noms/API non anticipés.
2. Matrice précise d’invalidation, notamment moves, échecs et remplacement d’un autre nom.
3. Contrat d’erreurs et priorité lorsque plusieurs préconditions sont invalides.
4. Définition opérationnelle d’« arrays rejetés », notamment array déclaré de taille 1 : `GLSize == 1` ne suffit pas nécessairement à distinguer cette déclaration.
5. Politique de contexte/partage et récupération après rejet.
6. Observation autorisée pour prouver l’absence de lookup répété et l’ordre des gardes ; pas de mock présenté comme preuve GPU.
7. Consommateur, scène, framebuffer, colorimétrie, pixels/régions et tolérance fixés avant implémentation.

Pas de stratégie image ni de mocks supplémentaires héritée automatiquement.

## 6. Ordre et budget

**Séquence proposée au lead :**

1. Résoudre admission et décisions ; persister/committer le plan avant les worktrees auteurs.
2. Auteurs : RED causal puis GREEN par tranche, suivis de refactoring/non-régression.
3. Intégrer réellement les producteurs.
4. Generator de gaps sur ce candidat intégré, sans remplacer le RED auteur.
5. Audit indépendant `fresh`, hors board d’implémentation.
6. Corrections, ré-audit et preuves finales par runner distinct attribué au lead.

GPU exclusif : contexte/fixtures et verrou `artist_opengl`. Aucun outil de board ou de dispatch utilisé ici.

| Budget normalisé | Unités |
|---|---:|
| Setup : fixtures, scène, données, environnement, observabilité | **20** |
| Lifetime/invalidation | 20 |
| Programme/contexte | 12 |
| Type, compatibilité et résolution unique | 12 |
| Consommateur et oracle pixel | 10 |
| Generator/audit | 6 |
| Buffer : investigation 8, panne 6, ré-audit 6 | **20** |
| **Total** | **100** |

Hors setup : **60/20 = 75 % planifiés / 25 % buffer**. Prioriser les cas hauts peu coûteux avant les cas hauts coûteux. Si setup/image/observabilité excèdent cette hypothèse, budget à renégocier ; ne pas consommer le buffer ou supprimer un risque haut pour conserver du moyen.

## 7. Critères de sortie

- Décisions bloquantes résolues et contrat/plan committés avant auteurs.
- Nominal et négatifs discriminants d’O1–O7 exécutés sur le candidat final identifié.
- Lifetime, ordre des gardes, isolation des programmes/contextes et résolution amortie démontrés à leurs niveaux respectifs.
- Readbacks réels **et** oracle pixel sensible à une altération ; aucun remplacement par mock.
- Registre bidirectionnel à jour ; aucun gap haut non arbitré. Tout report accepté exige propriétaire, échéance et impact.
- Zéro test découvert ou skip requis : **INCONCLUSIVE**, jamais PASS.
- Aucune revendication de gain CPU/GPU, performance ou « zero-cost » sans mesure distincte.

**État de cette remise : plan conditionnel fourni ; aucune validation comportementale nouvelle.**
