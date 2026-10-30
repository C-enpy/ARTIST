# pool-2 (@qa-planner teammate)

Session: ses_e7626883effe2VyjXAO2qEvRRk
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T22:48:52.821Z — msg_189da1351001zr1oaU6F5shSug

# Plan QA candidat — pipeline uniforms Windows/Linux

**À persister par le lead avant admission des auteurs**, dans le plan déjà nommé `.opencode/evidence/mission-gpu-qa/qa-plan.md`, comme section datée du 29 octobre 2026, sans supprimer l’historique.

**Statut : préparation statique, aucun test écrit ou exécuté.** Livraison visée : **pour qualification ultérieure**, pas fonctionnement graphique démontré.

## 1. Admission, sources et pin observé

- Lecture locale `AGENTS.md` réussie ; skill `artist-build-validation` chargé.
- Contrat ratifié lu : `.opencode/evidence/mission-uniform-pipeline/contract.md`.
- Politique et digestion du lead lues : `model-policy.json`, `qa-inputs.md`, rapport d’intégration du contrat.
- Profil sur disque compatible avec un planner sans édition/shell/délégation. Cela **ne prouve pas** les règles effectivement appliquées par le runtime.
- Cette réponse constitue le candidat de restitution ; **réception effective à constater par le lead**. Un message d’information au lead a également été transmis avec succès.
- Aucun refus d’accès observé. Une première localisation de fichier inexistante a été corrigée par recherche locale.
- Aucun edit, shell, test, rendu, benchmark, installation ou provisionnement effectué.

### Pin observé

Le pin historique du profil est dépassé : les documents locaux actualisés au 29 octobre décrivent notamment ADR-0001/0002/0007 `Proposed/partial`, et un runtime documentaire plus récent. Le skill cite encore une autre référence historique.

**Décision appliquée :** ne pas appliquer ces anciennes photographies au candidat, ne modifier aucun profil et ne reprendre aucun comptage historique comme attendu.

Le rapport du lead attribue le contrat au commit `003ad9c5804a31523b46150579589abba3a30f65`. **HEAD courant, delta et SHA256 non vérifiés par ce profil** : le lead devra les fixer pour la remise aux auteurs.

## 2. Périmètre et obligations

### Obligations explicites

Le contrat §§1–9 impose :

1. Tests exécutables limités aux données synthétiques et programmes factices.
2. Catalogue fermé, sélection explicite et producteurs réellement raccordés.
3. Séparation préparation/construction/exécution/rapport.
4. `list`/`plan` sans effet de bord ; `report` en lecture seule.
5. Identités du plan, candidat, shaders, exécutables et environnement conservées.
6. Distinction CPU seul / OpenGL logiciel / OpenGL matériel, sans substitution silencieuse.
7. États pipeline, verdict test, validité de mesure et fraîcheur indépendants.
8. Scénarios C++/OpenGL livrés mais **NON_EXECUTE**, avec revue statique.
9. RED causal, GREEN et non-régression synthétiques attribués au runner lead.
10. Contrat et plan committés avant les worktrees auteurs ; audit fresh après intégration.

### Inférences de contrôle, non nouvelles spécifications

- Injecter des doubles qui échouent immédiatement sur toute tentative d’appeler un outil réel.
- Vérifier une seconde lecture du même bundle pour détecter une mutation ou une reclassification favorable.
- Utiliser des mutants synthétiques ciblés pour démontrer que les assertions distinguent réellement les erreurs.

Ces contrôles servent les obligations précédentes ; ils n’ajoutent ni backend, ni campagne, ni banc générique.

## 3. Top risques et allocation

La matrice QA **I × P, chacun 1–5**, reste indépendante des scores PM.

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| R1 — Faux résultat favorable | 5 | 4 | **20** |
| R2 — Mauvaise identité ou preuve périmée | 5 | 3 | **15** |
| R3 — Substitution silencieuse du renderer | 5 | 3 | **15** |
| R4 — Effets implicites ou lancement interdit | 5 | 3 | **15** |
| R5 — Phases/adaptateurs incorrectement raccordés | 4 | 3 | **12** |
| R6 — Présentation documentaire ambiguë | 2 | 3 | 6 |

La probabilité reflète les chemins conditionnels et l’absence actuelle de preuves du nouveau pipeline ; elle n’augmente pas artificiellement l’impact.

### Risques ≥ 10

| Risque | Défaillance / déclencheur | Portée | Détection actuelle | Mitigation actuelle | Gap |
|---|---|---|---|---|---|
| R1 | Code enfant 0 avec résultat absent, sélection vide, omission, doublon ou mesure invalide | Conclusions de livraison | Contrat ; garde historique `linux-evidence.py:reconcile` | États séparés et populations exigées | Tests synthétiques de chaque contre-exemple et agrégation |
| R2 | Shader, binaire, configuration ou identifiant modifié ; ancien receipt réutilisé | Attribution de toutes les preuves | Collecte historique d’empreintes, relue statiquement | Hashes avant/après et receipts liés au plan | Contrôles du nouveau bundle et mutations ciblées |
| R3 | Renderer ambigu ou logiciel accepté comme matériel | Qualification matérielle et interprétation des mesures | Garde llvmpipe historique seulement | Profil explicite ; ambiguïté → `UNVERIFIED` | Matrice synthétique demande/observation/environnement |
| R4 | Import/list/plan lance une sonde ; doublure absente déclenche runner réel | Machine locale, restriction de mission | Inspection des scripts existants | Doubles explicites ; phases à effets séparées | Sentinelles processus/réseau/écriture et branche CLI |
| R5 | Build sans receipt compatible, environnement MSVC perdu, helper lançant CTest implicitement | Portabilité et séparation des phases | Code des helpers relu | Adaptateurs distincts ; commandes déclarées | Tests de commandes et environnement sous doubles |

**Constat important :** les helpers existants peuvent appeler Conan puis build/CTest. Leur réutilisation ne vaut donc pas autorisation de les lancer pendant cette mission.

Réévaluation obligatoire après intégration, modification du candidat ou découverte d’un contre-exemple.

## 4. Registre obligation → scénario → oracle → preuve

Tous les contrôles ci-dessous sont **GAP**, sauf les lectures documentaires qualifiées `static-presence`. Les noms de fichiers de tests sont des **propositions aux auteurs**, pas des fichiers réputés présents.

### A. Contrôles synthétiques autorisés

| ID / obligation | Nominal | Négatif discriminant | Oracle et futur point de preuve |
|---|---|---|---|
| S1 — Pureté et catalogue | Import, aide, `list`, `plan` pour une sélection valide | Entrée inconnue, doublon, sélection vide ; tentative de sonde ou d’écriture | Sortie déterministe ; aucune création, aucun enfant/réseau/chargement workload ; `catalogue.py:list_scenarios`, `planning.py:make_plan`, `verify.py` |
| S2 — Modèles et bornes | Aller-retour JSON, tuples, hash canonique stable | Champs inconnus ; 0/négatif ; batch hors 1/16/256 ; dépassement du plafond ; hash divergent | Rejet causal sans commande exécutée ; `model.py`, `planning.py:make_plan` |
| S3 — Phases et receipts | Enchaînement valide avec doubles | Build sans préparation ; run sans build ; receipt d’un autre plan/run ; ancien dossier | Aucune phase implicite ni receipt reconstitué ; `evidence.py:prepare/build/execute` |
| S4 — Runner et erreurs | Processus factice émettant résultat conforme et logs | Échec enfant, timeout, interruption, sortie partielle ; double absent | Codes natifs/logs conservés, cause distincte, aucun fallback réel ; `runner.py:ProcessRunner`, `reporting.py:report` |
| S5 — Résultats et populations | Ensemble exact de résultats conformes | Code0 sans résultat ; JSON tronqué ; résultat manquant/dupliqué ; skip requis ; opérations différentes | Aucun succès favorable ; noms/populations et compteurs réconciliés ; `evidence.py:execute`, `reporting.py:report` |
| S6 — Identité | Plan, receipts et empreintes concordants | Modifier séparément shader, binaire, source/config ; échanger run/request/plan ; résultat périmé | Écart identifié, fraîcheur non `CURRENT`, comparaison non valide ; `evidence.py` |
| S7 — Renderer | CPU sans GL ; llvmpipe demandé/observé ; hardware déclaré/observé concordant | GL absent, renderer ambigu, logiciel sous demande hardware, variables forçant logiciel, Windows logiciel | CPU ≠ upload GL ; ambiguïté non hardware ; aucune substitution/reprise silencieuse ; adaptateurs `collect_environment`, collecte des résultats |
| S8 — Mesures et agrégation | Mesure finie conforme ; zéro et delta négatif légitimes ; mesure non demandée | NaN/inf, unité/population incohérente, échantillons absents, oracle faux | Zéro n’est pas absence ; oracle faux → mesure `INVALID` ; code global selon priorité contractuelle, toutes causes conservées ; `reporting.py:report` |
| S9 — Artefacts et rapport | Bundle relatif valide ; rapport répété identique sans mutation | Absolu, `..`, lien sortant, résultat préexistant, manifeste incomplet | Rejet sans écrasement ni sortie du bundle ; rapport ne relance rien et expose les manques ; `evidence.py`, `reporting.py:report` |
| S10 — Adaptateurs | Commandes décrites sous doubles, chemins avec espaces, cibles exactes | Bootstrap sans autorisation ; contexte MSVC perdu ; mélange Debug/Release ; invocation du main de campagne | `argv` sans shell libre ; environnement approprié/restauré ; zéro appel moteur réel ; `adapters/windows.py`, `adapters/linux.py` |

**Frontières supplémentaires :**

- Tester les bornes minimales/maximales ratifiées et juste au-delà.
- Tester l’ordre des phases, l’interruption et une reprise sans ancien résultat accepté.
- Tester plusieurs causes simultanées pour l’ordre d’agrégation.
- Vérifier le refus d’un bundle déjà possédé, sans lancer de campagne concurrente.
- Conserver une fixture de journal long et de chemins/caractères à échapper.
- Filtrer l’environnement enregistré ; aucun dump complet contenant potentiellement des secrets.

Les plafonds numériques de charge, répétitions et timeout doivent être fixés par les auteurs **avant leurs tests de frontières** et inclus dans le plan.

### B. Scénarios réels livrés, exclusivement revus statiquement

| Scénario contractuel | Contrôle nominal de livraison | Contre-exemple que l’oracle doit détecter |
|---|---|---|
| `cpu.float-mirror` | API existante, données déterministes, aucun contexte GL | Valeur figée ou miroir non mis à jour ; aucune prétention d’upload |
| `gl.float-routes-correctness` | Deux routes, phases `0 → .5 → 0`, chaque cellule vérifiée pour lots 1/16/256 | Deux routes pareillement fausses, dernière cellule seule vérifiée, absence de draw ou retour à zéro omis |
| `gl.float-preparation` | Producteur/sélection existante raccordé, préparation après link/réflexion | Nom absent, mauvais type, array taille1, mauvais contexte, métadonnées invalidées non refusés |
| `gl.float-lifetime` | Sélection explicite des tests existants, owners et ordre de destruction identifiés | Ancienne incarnation réutilisée, révocation ignorée ou déréférence après retrait |
| `gl.float-submission-cpu` | Travail égal, chauffe, répétitions, ordre reproductible, A/A ; diagnostics hors chrono | Travail différent, readback/assertions chronométrés, temps CPU nommé GPU, nouvelle résolution native dans une route |

Référence source effectivement relue :  
`ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformRenderTests.cpp:expectOracle` et `TriangleTarget`.

Les points fixes existants distinguent déplacement, dessin effectif et fond. Leur présence est une preuve **statique**, pas une image obtenue pour ce candidat.

Revue distincte requise sur l’oracle graphique et la fenêtre chronométrée. **Aucune compilation C++, découverte CTest ou exécution graphique** pour réaliser cette revue.

## 5. Registre inverse et preuves

Proposition de répartition :

| Futur fichier de tests | Obligations |
|---|---|
| `scripts/tests/verification_core/test_contract.py` | S1–S6, S8–S9 |
| `scripts/tests/verification_windows/test_contract.py` | S7, S10 Windows |
| `scripts/tests/verification_linux/test_contract.py` | S7, S10 Linux |
| `scripts/tests/verification_scenarios/test_contract.py` | Cohérence du catalogue/protocole et fixtures synthétiques ; aucune preuve GPU |

À l’intégration, chaque **symbole de test réel** doit porter ses IDs. L’auditeur liste tests orphelins, obligations sans assertion discriminante, doublons faibles et preuves périmées. Le seul nom d’un test ou la présence d’un fichier ne vaut pas exécution.

Pour chaque preuve, enregistrer :

- HEAD, delta, SHA256 des entrées et artefacts ;
- source/date, commande exacte, auteur et runner ;
- plateforme, runtime et environnement filtré ;
- attendu/observé ;
- découverts/exécutés/échoués/skipped ;
- journaux et résultats bruts.

**État présent :** zéro test exécuté par ce profil ; découverte et skips non évalués ; aucune preuve RED/GREEN du nouveau candidat. Aucun résultat historique transféré.

## 6. Exécution autorisée au runner distinct

Après validation des fichiers et du confinement des doubles, le lead peut exécuter séparément les quatre suites synthétiques proposées :

```text
python -B -m unittest discover -s scripts/tests/verification_core -p test_contract.py -v
python -B -m unittest discover -s scripts/tests/verification_windows -p test_contract.py -v
python -B -m unittest discover -s scripts/tests/verification_linux -p test_contract.py -v
python -B -m unittest discover -s scripts/tests/verification_scenarios -p test_contract.py -v
```

Ces commandes restent des **propositions**, dépendantes des fichiers intégrés.

Avant tout lancement, vérifier que les tests bloquent les appels réels à Conan, CMake, CTest, PowerShell de construction, workloads et réseau. Les seuls enfants autorisés sont des programmes factices appartenant aux tests.

**Exclus :** suites moteur même CPU, rendu logiciel ou matériel, benchmark, installation, exécution distante et cloud. Les commandes historiques du skill ne sont pas autorisées pour cette mission.

## 7. Budget et conditions d’arrêt

Budget proposé : **4 heures actives**.

- Setup : **60 min** — fixtures, doubles, identité et environnement.
- Planifié : **135 min**, soit 75 % du restant :
  - faux résultats/agrégation : 45 ;
  - identité et renderer : 35 ;
  - pureté/phases/adaptateurs : 35 ;
  - revue statique graphique et traçabilité : 20.
- Buffer : **45 min**, soit 25 % du restant — investigation, panne et ré-audit.

Ce budget est une allocation cible, pas une estimation acquise du développement. Si irréalisable, différer le moyen/bas ou réestimer ; ne pas supprimer un contrôle haut pour conserver du cosmétique.

**Arrêt du contrôle concerné** si : appel réel imprévu, réseau/bootstrap implicite, mutation hors fixture, candidat divergent, code0 sans preuve, renderer substitué, logs perdus ou oracle affaibli. Conserver les éléments disponibles ; poursuivre les analyses indépendantes sans élargir les droits.

## 8. Gaps bloquants et sortie

### Avant auteurs

- Lead : constater les permissions effectives et la réception de cette restitution.
- Persister/committer le présent plan ; communiquer le HEAD commun aux auteurs.
- Fixer les plafonds de charge et les symboles/tests réels.
- Maintenir la barrière entre producteurs et consommateurs intégrés.

### Avant réception

- RED causal puis GREEN et non-régression synthétiques, attribués au runner lead.
- Catalogue complet avec producteurs raccordés ; aucun scénario seulement imprimable.
- Revue fresh des tests et revue statique ciblée des scénarios graphiques.
- Aucun résultat manquant, périmé, skip requis ou mesure invalide présenté favorablement.
- Limites par plateforme explicites.

**Verdict actuel : plan candidat, contrôles fonctionnels INCONCLUSIVE faute d’exécution.**  
La réception pourra annoncer **« livré pour qualification Windows/Linux »** ; toute compilation/exécution graphique non réalisée restera **NON_EXECUTE**. Aucun gain de performance n’est requis ni revendiqué.

## assistant — 2026-10-29T22:50:36.481Z — msg_189dba839001B9MnU3RZJb3wUn

Complément au plan candidat remis :

- **Première lecture autorisée confirmée** : `AGENTS.md`, puis contrat ratifié, `qa-inputs.md` et politique locale.
- **Admission native attribuée au lead** : règles effectivement consultées via API ARTIST — `edit/shell/subagent deny`, `read/execute/team_message allow`. Ce constat ne prouve pas une sandbox.
- **HEAD du contrat confirmé par le lead** : `003ad9c5804a31523b46150579589abba3a30f65`. Les empreintes du candidat final resteront à enregistrer.
- **Restitution** : plan complet dans la réponse finale précédente, pour persistance/commit par le lead avant auteurs.
- **Exécution** : aucun fichier modifié, aucun test écrit ou exécuté. Seuls les tests synthétiques Python/processus factices pourront être exécutés par le runner lead ; C++/GL restent **NON_EXECUTE**.

Les critères, risques, registres, budget et conditions d’arrêt du plan restent inchangés.