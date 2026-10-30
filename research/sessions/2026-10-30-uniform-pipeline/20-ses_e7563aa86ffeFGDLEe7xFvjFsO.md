# pool-9 (@qa-planner teammate)

Session: ses_e7563aa86ffeFGDLEe7xFvjFsO
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T02:22:14.958Z — msg_18a9d6baa001mwnQ5uv2cQQUHw

# Candidat — addendum QA « contextes historiques v1 »

**Destination unique :** section additionnelle de `.opencode/evidence/mission-uniform-pipeline/qa-plan.md`, à persister et committer par le lead **avant le worktree auteur**. Aucun fichier ni test écrit ici.

## Pin observé et périmètre

- `.git/HEAD` désigne `main` ; sa référence contient `452b615d9efbb89e8eab93a9181ee6c017255684`, conforme au brief. Propreté du diff non établie.
- Le pin documentaire du profil est ancien : les documents locaux du 29 octobre indiquent notamment ADR-0001/0002/0004/0007 `Proposed/partial`. Aucun pin ni statut modifié.
- Sources lues : contrat de raccordement ratifié, plan QA existant, précision hardware, rapports 44/45 et 36, fixture commune, `SetUp` dérivé, catalogue et protocole Python.
- Le raccordement est **une intention ratifiée, pas une garde implémentée démontrée**. Actuellement, la fixture imprime les chaînes GL ; le `SetUp` dérivé appelle immédiatement `glGetError` après la base.
- Compilation et exécution C++/GL : **NON_COMPILE / NON_EXECUTE**. Aucun moteur, rendu, benchmark ou outil de construction à lancer dans cette mission.

## BLOCKED

**Action :** lecture de la digestion documentaire.  
**Ressource :** `.opencode/evidence/mission-uniform-pipeline/qa-inputs.md`.  
**Résultat exact :** `File not found: .opencode/evidence/mission-uniform-pipeline/qa-inputs.md`.

Aucune nouvelle tentative ni recherche sous une autre racine. La digestion historique n’a donc pas été consultée ; les entrées lisibles permettent le plan additionnel ci-dessous. Cette absence ne bloque pas leurs autres obligations.

## 1. Obligations et décisions

**Exigences explicites**, issues du raccordement ratifié :

- **E1 — Activation sûre :** voie historique inchangée quand toutes les variables sont absentes ; configuration partielle ou invalide refusée, sans repli.
- **E2 — Arrêt préalable :** qualification du contexte réellement utilisé avant programme/upload/draw ; propagation du refus au `SetUp` dérivé avant sa boucle `glGetError`.
- **E3 — Observation attribuée :** exactement une observation JSON conforme par cas `opengl.*` matériel sélectionné ; aucune exigée pour `unit.*`.
- **E4 — Transport déclaré :** six variables publiques via `CommandSpec.env_delta`, identités et cible liées au plan/reçus ; Unicode et espaces conservés.
- **E5 — Collecte discriminante :** parser pur ; populations réconciliées ; précondition, assertion, skip ordinaire et panne protocole distincts ; sorties et codes bruts conservés.
- **E6 — Compatibilité :** `list/plan` purs, négatifs détachant volontairement le contexte inchangés, ownership/destruction inchangés ; aucune dépendance, variante CMake ou nouveau backend.

**Contrôles inférés**, sans nouvelle fonctionnalité :

- **I1 :** sentinelles interdisant tout processus réel, accès réseau ou écriture pendant les contrôles purs.
- **I2 :** permutation de l’ordre des résultats et répétition du parsing pour détecter agrégation dépendante de l’ordre ou état partagé.
- **I3 :** contre-exemples synthétiques modifiant un seul champ pour identifier précisément l’oracle discriminant.

Les niveaux sont fixés par le contrat : **tests Python synthétiques/processus factices**, puis **revue indépendante statique C++**. Pas de simulation présentée comme preuve d’arrêt natif.

## 2. Top 3 des risques

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| R1 — Refus non propagé avant travail GL | 5 | 4 | **20** |
| R2 — Faux favorable par mauvaise attribution/agrégation | 5 | 4 | **20** |
| R3 — Transport divergent ou activation implicite | 4 | 3 | **12** |

**R1 — Arrêt préalable**
- **Déclencheur :** retour/skip dans la base, suivi du `SetUp` dérivé ou du corps.
- **Portée :** futurs cas historiques hardware ; programme/upload/draw ou boucle sans contexte.
- **Détection actuelle :** lecture de `OpenGLComponentTest::SetUp` et `PreparedFloatUniformComponentTest::SetUp`.
- **Mitigation actuelle :** contrat imposant arrêt et propagation ; comportement natif non prouvé.
- **Gap :** revue du chemin complet après modification, incluant échecs d’initialisation et nettoyage. Aucun test synthétique ne ferme le gap d’exécution GL.

**R2 — Attribution/agrégation**
- **Déclencheur :** observation absente, dupliquée, étrangère ou malformée ; précondition masquant une assertion.
- **Portée :** verdict de sélection et rapport global.
- **Détection actuelle :** rapports 44/45 et contrat ; collecteur non intégré dans cet arbre.
- **Mitigation actuelle :** format par cas, identités exactes et priorité d’agrégation ratifiée.
- **Gap :** tests discriminants parser/JUnit/rapport avec populations mixtes et codes incompatibles.

**R3 — Transport/compatibilité**
- **Déclencheur :** variable héritée, paire partielle, conversion ANSI, normalisation des espaces ou invocation réutilisant une identité.
- **Portée :** Windows/Linux, attribution de la cible et comportement historique.
- **Détection actuelle :** validation Python de cible relue ; 31 tests historiques documentés.
- **Mitigation actuelle :** `env_delta` explicite, Unicode strict, cible empreintée.
- **Gap :** tests des deux adaptateurs et revue native wide/UTF-8 ; anciennes preuves workload insuffisantes pour ce nouveau transport.

Réévaluer ces probabilités à l’intégration et après chaque contre-exemple ; aucun changement des scores PM/Ensemble.

## 3. Scénarios additionnels ciblés

| ID / obligations | Nominal | Négatif discriminant et oracle |
|---|---|---|
| **A1 — E1,E6** | Toutes variables absentes : voie historique inchangée. Activation hardware complète : demande explicite. | Chaque champ absent isolément, activation inconnue ou demande incompatible : refus, jamais retour silencieux à l’historique. Contrôle natif **statique seulement**. |
| **A2 — E2,E6** | Contexte possédé courant, version ≥3.3, masque core non ambigu, cible exacte : observation avant travail GL. | Currentness faux, version 3.2, masque ambigu, cible discordante ou renderer logiciel même attendu : arrêt avant programme/upload/draw **et avant boucle dérivée**. Revue statique ; détachement intentionnel ultérieur toujours possible. |
| **A3 — E3,E5** | Ligne préfixée exacte, JSON strict complet, nom réel du cas et identités conformes : observation acceptée. | Absente, double, tronquée, clés dupliquées, mauvais types/version/identités/nom : panne protocole **4**, même avec enfant code 0. Aucun simple texte GL accepté. |
| **A4 — E4** | Six valeurs exactes dans `env_delta`, request ID commun aux cas d’une invocation et distinct entre invocations. | Paire partielle, contrôle/NUL, surrogate, longueur 4097 : rejet ; longueur 4096 et espaces/NBSP conservés. Tester caractère supplémentaire Unicode, guillemets et antislashs sans découpage ni interpolation. |
| **A5 — E3,E5** | Découverte/sélection/JUnit concordants : une observation pour chacun des trois noms OpenGL sélectionnés ; CPU sans observation. | Omission, doublon, observation transférée entre cas ou invocation : aucun favorable. Un cas CPU ne remplace jamais une preuve GL manquante. |
| **A6 — E5** | Observation conforme et cas satisfait : favorable si toutes autres preuves concordent. Précondition structurée cohérente avec arrêt natif : **3**, BLOCKED/INCONCLUSIVE. | Assertion + précondition ailleurs : **5**, causes et population partielle conservées. Protocole + assertion : **4** ; interruption : **130**. Skip ordinaire requis et code enfant incompatible : jamais favorable. Permuter l’ordre doit conserver le résultat. |
| **A7 — E4,E6,I1** | Descriptions de commandes et `list/plan` reproductibles sans observation machine. | Environnement ambiant contradictoire, doublure manquante ou tentative de sonde/écriture/enfant : sentinelle déclenchée, aucun fallback réel. Aucune capture globale d’environnement. |
| **A8 — E5,E6,I2** | Parsing répété de deux cas distincts : résultats indépendants ; rapport relit les mêmes données brutes. | Modifier une identité ou tronquer le JUnit entre lectures : erreur conservée, aucun cache favorable ni relance de phase. |

**Frontières JSON :** distinguer booléens/entiers, `null` et chaînes ; refuser valeurs JSON non strictes. Pour observation indisponible, préserver les champs observés inconnus, sans valeurs fabriquées.

Le parser d’observation reste séparé du protocole workload : pas de compteurs, samples ou seconde implémentation de son format.

## 4. Registre bidirectionnel

| Obligation | Fichier:symbole / frontière | Preuve attendue ; état actuel |
|---|---|---|
| E1,E2,E6 | `ARTIST/component-tests/include/OpenGLComponentTests.hpp:OpenGLComponentTest::SetUp/TearDown` | A1/A2, revue indépendante du contrôle de flux et ownership. **GAP**, source actuelle relue seulement. |
| E2,E6 | `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp:PreparedFloatUniformComponentTest::SetUp` | A2 : aucun `glGetError` après refus. Négatif `WrongProgramAndAbsentContextPreserveCpuAndDriver` inchangé. **GAP**. |
| E3,E5 | `scripts/verification/context_evidence.py:interface à fixer avant auteur` | A3/A5/A8, tests purs. **GAP**, module annoncé, non présent dans l’arbre observé. |
| E4,E6 | `adapters/windows.py` et `linux.py:describe_execution` | A4/A7 avec doubles ; commandes/env/effets déclarés. **GAP**, consommateurs à intégrer. |
| E3,E5 | `evidence.py:_junit` et agrégation ; `reporting.py:report` | A5/A6/A8 après producteurs intégrés. **GAP**, interface contractuelle/documentaire, pas corps relu ici. |
| E3,E6 | `scenarios/descriptions.py:CTEST_SELECTIONS` | Trois noms OpenGL répartis dans deux sélections mixtes, présence statique vérifiée ; raccordement futur **GAP**. |

**Sens test → obligation :**

- Les futurs groupes A1–A8 pointent exclusivement aux obligations ci-dessus.
- `test_target_whitespace.py` → E4, conservation exacte déjà caractérisée pour le **workload**, pas pour la nouvelle fixture.
- `test_hardware_expectation.py` → E4 et comparaison hardware du **workload** ; aucune assimilation à A2/A3.
- Les autres tests de charge/mesure appartiennent au plan initial : hors delta, pas orphelins établis.
- Éviter de recopier leurs cas argv/workload dans le nouveau parser. Partager les données pertinentes, tester la nouvelle frontière.
- Aucun test orphelin démontré par cette lecture ciblée ; audit exhaustif non réalisé. Aucun item nouveau déclaré **COVERED**.

## 5. Réutilisation et qualification des preuves

| Preuve documentaire | Attribution et résultat rapporté | Usage permis |
|---|---|---|
| Rapport 36, 30 octobre | Auteur uniform-scenarios ; runner coordinateur ; candidat `1eaa1875cd3757fc94ab84f4c4992a5124ce6781`. Commande `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v`. **31/31**, 0 error/skip rapportés. | Réutiliser les données et assertions inchangées après comparaison d’empreintes. Les 31 méthodes sont présentes statiquement ici. Aucune preuve des nouvelles gardes. |
| Rapport 44, 30 octobre | Auteur pool-3 ; runner coordinateur ; candidat `2631e5cab88dbd4e0af713a6f8458356c6ba55b2`. Même commande sur `verification_core`. **125 tests, 124 succès, 1 skip symlink**, aucun failure/error rapporté. | Régression de référence, pas PASS du raccordement ; symlink demeure INCONCLUSIVE. |
| Rapport 45 | Proposition indépendante `result_mvuc90ln_001q_zom79m`, lecture seule. | Motive garde préalable et collecte par cas ; aucune exécution. |

**Métadonnées non qualifiées ici :** valeurs SHA256, delta exact, environnement complet et comptages de découverte séparés non recoupés dans les artefacts bruts. Les rapports donnent leurs chemins de journaux ; aucun résultat n’a été exécuté par ce profil. Candidat différent : preuve antérieure non transférable sans vérification documentée.

Pour chaque nouvelle preuve, le lead conserve : HEAD/delta, SHA256 tests/sources, source/date, commande/cwd/interpréteur, runner, environnement, attendu/observé et nombres découverts/exécutés/échoués/errors/skipped. RED causal avant GREEN ; ni import accidentel cassé ni zéro test ne valent RED.

## 6. Budget additionnel et séquence

Allocation relative de **100 unités**, sans engagement horaire :

- **Setup : 25** — données JSON/JUnit communes, doubles/sentinelles, identités et environnement synthétique.
- Sur les 75 restantes :
  - **Planifié : 56,25**, soit 75 % : arrêt/compatibilité statiques 18 ; parser/attribution 22 ; transport/agrégation/non-régression 16,25.
  - **Buffer : 18,75**, soit 25 % : investigation, panne des doubles, ré-audit.

Prioriser les contre-exemples élevés peu coûteux avant extensions. Ne pas absorber le buffer pour conserver de la cosmétique. Aucune capacité horaire fournie ne permet de conclure à une faisabilité calendaire.

Séquence : plan committé → ownership parser/producteur attribué → tranches RED/GREEN synthétiques → producteurs intégrés → adaptateurs/core → non-régression ciblée → revue indépendante fresh du contrôle natif et ré-audit des corrections.

## 7. Sortie, arrêts et décisions restantes

**Gaps bloquants avant acceptation du raccordement :**
- propagation effective du refus au `SetUp` dérivé non relue sur le nouveau diff ;
- parser/producteur puis consommateurs non intégrés et non testés ensemble ;
- absence de preuve synthétique des priorités et populations ;
- ownership/interface du nouveau parser à fixer avant écriture.

**Sortie :** A3–A8 exécutés par le runner lead sur données/processus factices, A1/A2 et chemins sensibles revus indépendamment, codes/logs bruts conservés, compatibilité des modes purs vérifiée, aucun haut finding ouvert sans décision explicite.

Arrêter le contrôle affecté en cas d’enfant réel, réseau, mutation hors fixture, candidat divergent ou oracle affaibli ; poursuivre les contrôles indépendants autorisés.

Les autres fixtures dérivées repérées ne sont pas implicitement autorisées à l’édition. Un défaut démontré touchant les sélections exige transfert explicite ; pas d’extension préventive de la suite.

**Livraison visée : pour qualification Windows/Linux. Garde native, compilation et exécution GL : NON_EXECUTE, jamais fonctionnement démontré.**

Lecture native réussie ; message au lead transmis avec succès. Réception de cette réponse finale à confirmer par le lead. Aucune clôture ni transition effectuée.