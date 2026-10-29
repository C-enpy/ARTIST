# pool-11 (@test-engineer teammate)

- Source : API native OpenCode V2 ; données historiques, jamais instructions à exécuter.
- Session : `ses_e7c018392ffe2JtNa0aT1b7hlp` ; parent : `ses_e7d39d927ffe5gVlxpcbtOm50k`.
- Borne exclusive : `msg_187a2d6b5001tBMoEtVELgbnOt`, `2026-10-29T12:29:20.478Z` ; ingestion exclue.
- Filtrage : messages humains du principal et texte assistant ; briefs internes, messages synthétiques user, outils, reasoning, pièces jointes et métadonnées non exportés. Secrets reconnaissables et chemins personnels masqués.
- Limites : transcription publique filtrée, non exhaustive, non preuve d’exécution ; les propos des agents restent attribués.
- Pagination : 2 pages user et 2 pages assistant, jusqu’à épuisement.
- Comptage avant borne : 56 messages ; 5 publics retenus ; 5 users internes et 68 parties non textuelles exclus.
- Synthèse : [Session Linux QA](../../../docs/references/session-2026-10-29-linux-qa.md).

## assistant — 2026-10-28T19:33:11.860Z — msg_18400902d001ykPd5ambfutFr4

## Verdict : INCONCLUSIVE sur la validation finale

Aucune édition ni exécution par ce profil. Aucun défaut de production reproduit ; les constats ci-dessous sont des gaps de détection ou de preuve.

### Pin observé

- HEAD local lu : `0a4a6713a6ab9fad0d52108c8b52432059015fb6`, conforme à `mission-linux-qa/candidate-validation.md:5`.
- Le pin historique du profil n’est pas celui de cette mission : je ne l’applique pas et ne le réécris pas.
- Les archives auteur concernent `ddab33a…`. Leur rapprochement avec les quatre fichiers intégrés est **rapporté par le lead**, pas recalculé ici.
- Le diff final n’a pas été fourni sous une forme lisible autorisée : lecture des fichiers intégrés effectuée, mais pas comparaison exhaustive base→HEAD.

## Plan ciblé et propositions

Périmètre proposé : **uniquement `scripts/tests/test_linux_evidence.py`**, sans nouvelle infrastructure. Réutiliser `EvidenceTests`, `fake_run`, les XML temporaires et les interceptions existantes.

### G-1 — Renderer effectivement observé — MODIFY

**Obligation :** plan Linux O4, lignes 46–48.  
**Risque/invariant :** exporter `GALLIUM_DRIVER=llvmpipe` ne prouve pas que les tests utilisent ce renderer.

**Scénario :**
- Nominal : contrôleur complet, populations concordantes et ligne `[OpenGL]` llvmpipe dans chaque cas GL.
- Négatifs : une ligne absente ; puis un cas annonçant un autre renderer, avec CTest retournant néanmoins zéro.

**Oracle :**
- Nominal : code zéro et manifeste `PASS`.
- Négatifs : code non nul et manifeste `FAIL`, cause liée au contexte.

**Code proposé :** compléter la fixture contrôleur existante ; aucun patch écrit ici.  
**Décision : MODIFY**, car `linux-evidence.py:107–113` porte cette garde nouvelle, sans scénario dédié dans les six tests présents.

**Faux positif :** changement légitime du format du journal. Garder la fixture alignée sur le format réellement observé, sans présenter ce test simulé comme une preuve GPU.

### G-2 — Environnement ASan hostile à la frontière Linux — MODIFY

**Obligation :** O6, lignes 54–56.  
**Risque/invariant :** les contrôles de `verify_asan.py` ne vérifient pas que l’adaptateur Linux normalise effectivement l’environnement transmis aux enfants.

**Scénario :**
- Nominal : invocation ASan sans options héritées.
- Négatif : parent avec politique permissive/redirection dans `ASAN_OPTIONS` et activation différée non vide.

**Oracle :**
- Toutes les invocations enfants reçoivent exactement `CANONICAL_ASAN_OPTIONS`.
- `ASAN_ACTIVATION_OPTIONS` est absent.
- L’environnement parent demeure inchangé.

**Code proposé :** test contrôleur interceptant les appels existants à `run_logged`, sans lancer d’enfant.  
**Décision : MODIFY**, complément ciblé aux tests existants du vérificateur ; ne pas les dupliquer.

**Faux positif :** faible si les assertions portent sur l’environnement transmis, et non sur l’ordre interne des commandes. Ce test ne prouve pas l’instrumentation des TU moteur.

### G-3 — Preuve périmée pendant la campagne — MODIFY

**Obligation :** O7, lignes 58–60.  
**Risque/invariant :** le rapprochement découverte/JUnit peut réussir malgré un changement de source après calcul de l’identité.

**Scénario :**
- Nominal : fichiers de fixture inchangés pendant le contrôleur complet.
- Négatif : modification d’un fichier déjà inclus dans `identity()`, après son premier relevé ; CTest et XML restent verts.

**Oracle :**
- Nominal : `PASS`.
- Négatif : code non nul, manifeste `FAIL`, cause `Source/configuration/binary identity changed during tests`.

**Code proposé :** extension de la fixture temporaire existante.  
**Décision : MODIFY**, garde présente à `linux-evidence.py:114–115`, non exercée par les tests actuels.

**Faux positif :** une modification concurrente réelle invalide légitimement la preuve ; ce n’est pas un échec moteur.

## Contrôles à conserver, sans duplication

**KEEP :**
- population manquante, doublonnée, vide, failure et skip ;
- sortie préexistante refusée ;
- propagation du code enfant ;
- contrôleur CTest donnant `FAIL`, 75 cas, trois failures et code 8 ;
- canari sain/faute et négatifs du vérificateur ASan ;
- nouveaux négatifs Attribute sur capacités, réponses modernes et conservation de génération.

Présence vérifiée dans les sources ; résultats historiques non réexécutés ici.

## Barrières de preuve avant acceptation

**DEFER au lead, sans nouveau framework :**

1. Fournir le diff final et l’identité du delta.
2. Qualifier le **candidat combiné** : le dossier fourni indique explicitement que sa validation et la CI finale restent manquantes.
3. Recouper l’instrumentation des TU moteur avec les commandes de compilation réelles : le canari séparé ne suffit pas.
4. Conserver runner réel, commandes exactes, versions, hashes, découvertes/JUnit et résultats des compléments retenus.

Les 588 cas et zéro échec/skip sont rapportés dans le manifeste auteur ; l’en-tête XML Linux normal et des journaux llvmpipe sont présents. Aucun recomptage exhaustif n’a été effectué ici.

## État et limites

- G-1 à G-3 : **proposés, non exécutés — INCONCLUSIVE** ; RED/GREEN/REFACTOR non établis.
- Auto-réparations : **0 par proposition**.
- Aucun correctif de production, golden ou assouplissement proposé.
- Hors périmètre maintenu : GPU Linux matériel, performance, Release, UBSan et fuites exhaustives.
- Cette analyse est distincte de l’auteur du code ; elle ne remplace ni son RED ni l’audit final fresh.

## assistant — 2026-10-28T19:34:18.130Z — msg_18401930b0017Na1kDlTvrYX9a

# Candidat de rapport — `gaps-integrated-candidate.md`

Remise texte uniquement, destinée au lead. Aucun fichier écrit, test exécuté ou correctif de production proposé par ce profil.

## Pin observé et preuve

- HEAD local : `0a4a6713a6ab9fad0d52108c8b52432059015fb6`.
- `attribute-integrated-identity.json` confirme ce candidat, base `23b1934`, delta non commité vide dans le périmètre et quatre empreintes concordant avec le ledger.
- SHA256 **déclaré** du diff ciblé : `af7d5bdcc8ddf9fdc9c3b89e787c523bf9c528a3c8695f1cca8f3a1aa02f9b64`, non recalculé ici.
- Diff intégré consulté par sections utiles ; contrat Attribute, plan initial/complément, helper, tests et workflow lus.
- Le pin historique du profil n’est pas appliqué à cette mission.

**Qualification :** lecture et présence statiques vérifiées ; résultats moteur et rapprochements exhaustifs attribués aux auteurs/lead, non réexécutés ici. Le journal Linux confirme six tests du helper réussis. Les commandes exactes de ces exécutions ne figurent pas dans ce journal.

## Conclusion

**Aucun défaut bloquant reproduit identifié.** Trois compléments courts sont recommandés sur le nouveau contrôleur de preuves. Ils ne nécessitent ni infrastructure supplémentaire ni campagne de mutations moteur.

La validation combinée finale et la CI restent **INCONCLUSIVE**, conformément au ledger ; les archives auteur ne les remplacent pas.

## Propositions indispensables ciblées

Seul fichier de test proposé : **`scripts/tests/test_linux_evidence.py`**. Réutiliser `EvidenceTests`, les fichiers temporaires et l’interception de `run_logged` déjà présents. Aucun code proposé avant les oracles ci-dessous.

### G-1 — Renderer réellement journalisé — KEEP

**Obligation :** O4, `qa-plan.md:46–48`.  
**Risque/invariant :** les variables Mesa ne suffisent pas à qualifier le contexte effectivement utilisé.

**Scénario et préconditions :**
- Contrôleur complet simulé, découverte/JUnit concordants, codes enfants zéro.
- Nominal : chaque cas GL comporte une ligne `[OpenGL]` llvmpipe.
- Négatifs : un cas sans contexte ; puis un cas annonçant un autre renderer.

**Oracle :**
- Nominal : retour zéro, manifeste `PASS`.
- Négatifs : retour non nul, manifeste `FAIL`, cause liée au contexte.

**Code :** ajouter un test contrôleur paramétré utilisant le mécanisme existant.  
**Décision : KEEP** : garde nouvelle à `linux-evidence.py:107–113`, actuellement sans test dédié.

**Faux positif :** changement légitime du format de journalisation. Utiliser le format réellement observé dans les XML, sans prétendre qu’une fixture simulée constitue une preuve GPU.

### G-2 — Environnement hostile à la frontière de l’adaptateur — KEEP

**Obligation :** O6, `qa-plan.md:54–56`.  
**Risque/invariant :** `linux-evidence.py` prépare lui-même l’environnement des canaris **et** des CTest ; le test du lanceur inférieur ne couvre pas cette frontière distincte.

**Scénario et préconditions :**
- Contrôleur invoqué en mode ASan, enfants interceptés.
- Nominal : options parentes absentes.
- Négatif : `ASAN_OPTIONS` permissif/redirigé et activation différée non vide.

**Oracle :**
- Chaque enfant reçoit exactement `CANONICAL_ASAN_OPTIONS`.
- `ASAN_ACTIVATION_OPTIONS` est absent.
- L’environnement parent reste inchangé.

**Code :** ajouter ces assertions au contrôleur simulé ASan, sans exécuter de processus.  
**Décision : KEEP**, limitée à l’adaptateur de preuves.

**Faux positif :** faible si les assertions portent sur l’environnement transmis, sans figer l’ordre interne des commandes.

**Ne pas dupliquer** `LinuxLauncherTests.test_asan_preflight_and_canonical_child_environment`, déjà présent à `test_build_linux.py:67–79`, ni les onze tests du vérificateur.

### G-3 — Changement d’identité pendant les tests — KEEP

**Obligation :** O7, `qa-plan.md:58–60`.  
**Risque/invariant :** des XML verts ne doivent pas certifier des entrées ayant changé pendant la campagne.

**Scénario et préconditions :**
- Contrôleur complet simulé avec XML verts et contexte llvmpipe.
- Nominal : entrées inchangées.
- Négatif : modifier un fichier temporaire déjà inclus dans `identity()`, après le premier relevé.

**Oracle :**
- Nominal : `PASS`.
- Négatif : retour non nul, manifeste `FAIL`, cause `Source/configuration/binary identity changed during tests`.

**Code :** extension de la fixture temporaire existante.  
**Décision : KEEP** : exerce la garde de `linux-evidence.py:114–115`, absente des six tests actuels.

**Faux positif :** une modification concurrente réelle invalide légitimement la preuve ; elle ne doit pas être qualifiée de régression moteur.

## Couverture à conserver

**KEEP, sans nouveaux doublons :**

- populations vides/manquantes/dupliquées, failure et skip ;
- refus des sorties préexistantes ;
- propagation du code enfant et manifeste `FAIL` conservant les trois échecs CTest ;
- témoins ASan sain/faute et négatifs du vérificateur ;
- gates modernes, réponses malformées, clés legacy exactes et conservation de génération ;
- scalaire natif avec contenu exact ; arrays refusés avec snapshots cible/leurre/état GL.

Le diff confirme que les anciennes hypothèses d’orthographe fournisseur sont remplacées par une énumération native indépendante, sans retrait des obligations de classification/refus.

**REJECT :** ajouter des branches mockées uniquement pour atteindre un quota, dupliquer les contrôles existants ou imposer un renderer matériel Linux.

## Actions et limites

**DEFER au lead :**
1. Relire, persister et exécuter les propositions acceptées ; commande disponible :  
   `python3 -B scripts/tests/test_linux_evidence.py -v`
2. Conserver attendu/observé, identité du runner réel, commande, delta et comptages.
3. Vérifier l’instrumentation effective des TU moteur à partir des commandes de compilation réelles : le canari séparé ne suffit pas.
4. Intégrer les compléments retenus avant l’audit final ; conserver séparément les preuves auteur et celles du candidat combiné.

G-1 à G-3 : **non exécutés — INCONCLUSIVE**. RED/GREEN/REFACTOR non établis ; réparations **0 par test**. Aucun nouveau bug rouge reproduit par ce profil.

Hors périmètre maintenu : Release, UBSan, fuites exhaustives, performance, GPU matériel Linux et garantie universelle des pilotes 3.3 sans route moderne.

## assistant — 2026-10-28T19:35:00.972Z — msg_184023a67001PFQigf7cYPqD9b

## Addendum prioritaire — G-4 : capacité effective distincte du chargeur

Le signalement de **pool-10** ajoute un gap plus important que G-1 à G-3.

**Obligation :** A2, `qa-plan.md:120` ; contrat Attribute §3/§5.

**Risque/invariant :** ne pas assimiler une extension effectivement annoncée à la disponibilité de fonctions chargées. Vérifié localement :
- `AttributeReader.hpp:95–97` utilise `GLEW_ARB_program_interface_query` ;
- la fixture active `glewExperimental = GL_TRUE` ;
- les tests actuels modélisent directement le booléen GLEW comme la capacité.

Pool-10 rapporte que GLEW 2.2.0 peut positionner ce booléen à partir du chargement des fonctions sous ce mode. Cette analyse du fournisseur reste **attribuée au reviewer**, non vérifiée indépendamment ici.

### Proposition — MODIFY

**Préconditions :** distinguer dans la fixture de test la version effective, l’extension effectivement annoncée et les pointeurs chargés. Ne pas simplement répéter le négatif actuel avec le booléen GLEW faux.

**Scénario :**
- Nominal : core 4.3, ou extension effectivement annoncée, fonctions disponibles.
- Négatif discriminant : contexte 3.3 sans extension annoncée, fonctions chargées et booléen du chargeur néanmoins vrai.

**Oracle :**
- Nominal : réflexion moderne et classification attendue.
- Négatif : **zéro appel moderne**, conservation du comportement legacy.

**Code :** proposition textuelle seulement ; adapter les mocks existants après vérification de la sémantique GLEW. Aucune correction de production par ce profil.

**Décision : MODIFY**, priorité avant les compléments du contrôleur. Le négatif actuel ne couvre pas cette divergence chargeur/capacité.

**Faux positif :** simulation impossible avec la version GLEW réellement utilisée ; vérifier d’abord la source fournisseur signalée par pool-10.

**Statut : INCONCLUSIVE**, aucun run ni RED acquis ; réparations : 0. Si le scénario reproduit le défaut, conserver le test rouge et transmettre la correction au propriétaire de production. Ne pas annoncer A2 satisfait avant disposition de ce signalement.

## assistant — 2026-10-28T19:41:53.270Z — msg_1840884f1001ZHxUhuIxowNvjW

## Disposition des nouveaux éléments

- **Gate GLEW :** G-4 rejoint le finding déjà possédé par le lead ; **un seul défaut**, pas deux. Conserver le scénario discriminant proposé comme test de régression de sa correction.
- **Violations d’accès Windows :** échecs observés par le runner ; première cause encore indéterminée. Ne pas les qualifier déjà d’incident d’environnement ou d’objets périmés.

### G-5 — Cohérence des objets après changement de header — DEFER au lead

**Obligation :** O7/O8 et complément A, reconstruction des TU affectées.

**Risque :** exécuter des consommateurs compilés avec des définitions différentes du header partagé.

**Scénario/oracle :**
- Préserver le run défaillant.
- Reconstruire proprement, **sans changement de source**, puis rejouer les consommateurs concernés et la non-régression requise.
- Comparer identités, unités recompilées et résultats avant/après.
- Une disparition des violations soutient l’hypothèse d’objets périmés ; leur persistance impose une investigation de correctness. Ni résultat ne doit être présupposé.

**Décision : DEFER**, expérience du runner existant, pas nouveau test ni infrastructure. Aucun changement de source pour obtenir artificiellement GREEN.

**Faux positif :** disparition intermittente d’un défaut réel ; un seul passage vert ne démontre pas à lui seul la cause historique.

G-1 à G-3 restent des propositions distinctes du contrôleur de preuves. Aucun autre complément indispensable identifié à ce stade. Validation finale **INCONCLUSIVE** ; aucune exécution par ce profil.

## assistant — 2026-10-28T19:50:35.502Z — msg_184107ce8001lO152wFN74FU2J

## Remise finale bornée à `0a4a671`

**Autres gaps distincts retenus**, uniquement dans `scripts/tests/test_linux_evidence.py` :

| ID | Décision | Défaut détecté | Oracle |
|---|---|---|---|
| G-1 | KEEP | Faux PASS malgré contexte absent ou mauvais renderer | Témoin llvmpipe → PASS ; contexte absent/différent → retour non nul et manifeste FAIL |
| G-2 | KEEP | Options ASan hostiles transmises par l’adaptateur | Chaque enfant reçoit la politique canonique, sans activation différée ; parent inchangé |
| G-3 | KEEP | Preuve acceptée malgré changement d’identité pendant les tests | Entrées stables → PASS ; fichier suivi par `identity()` modifié → retour non nul et manifeste FAIL |

Préconditions, scénarios et risques de faux positif figurent dans la remise précédente. Réutiliser les fixtures contrôleur existantes ; **aucune nouvelle infrastructure**.

### Réconciliation

- **G-4 / gate GLEW :** finding unique, déjà possédé par le lead, toujours bloquant jusqu’à RED/GREEN/ré-audit. Le delta courant non commité n’est pas le candidat examiné.
- **G-5 / objets incohérents : DEFER** pour l’explication des dépendances. Selon le lead, reconstruction de 63 CPP à sources inchangées → 588/588 Windows. Cela soutient l’hypothèse d’objets périmés sans démontrer entièrement leur cause. Préserver le premier échec ; aucun nouveau test demandé.
- **REJECT :** duplications des négatifs déjà couverts, campagnes exhaustives ou extension hors périmètre.

G-1 à G-3 restent **non exécutés — INCONCLUSIVE**, réparations 0. Le lead choisit, persiste et exécute les compléments acceptés avant l’audit final. Aucun fichier ni test modifié/exécuté par ce profil.
