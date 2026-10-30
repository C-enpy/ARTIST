# pool-18 (@correctness-reviewer teammate)

Session: ses_e74e48a98fferO0tfRrKoI35SX
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T04:45:18.004Z — msg_18b206330001rvld2tHxNdWhGR

## Réception réservée — candidat `50352e5`

HEAD du worktree confirmé. Revue statique uniquement : aucune édition ni exécution. Les scénarios ci-dessous sont établis par lecture ; leurs reproductions restent **NON_EXECUTE**.

### Écarts bloquants

**1. P2 — Une précondition CTest efface un FAIL déjà collecté**

`scripts/verification/evidence.py:897–905`, `_junit`.

- **Déclencheur :** sélection mixte workload + CTest hardware. Le workload établit `FAIL/code5`, puis un cas CTest produit une observation indisponible conforme avec skip.
- **Attendu :** conserver `FAIL`, les causes 5 et 3, priorité finale 5.
- **Observé dans le code :** `elif findings` remplace inconditionnellement le verdict par `INCONCLUSIVE`. L’exception locale code3 conduit ensuite à `pipeline_state=BLOCKED`, tandis que le code agrégé reste 5.
- **Impact :** verdict, état et causes se contredisent ; une précondition masque un échec démontré.
- **Vérification minimale :** amorcer `_junit` avec l’état légitime laissé par `_workload` après un oracle faux, puis fournir un JUnit conforme contenant une précondition structurée. Vérifier conservation de `FAIL` jusque dans receipt et rapport.

**2. P2 — Une exception ultérieure fait disparaître les assertions des invocations précédentes**

`scripts/verification/evidence.py:946–968`, `execute` ; `:849`, `_junit`.

- **Déclencheur :** première invocation CTest terminée avec assertion échouée et JUnit valide ; deuxième invocation interrompue, ou produisant un XML tronqué.
- **Attendu :** code130 ou4 prioritaire, mais verdict `FAIL`, cause5 et population disponible conservés.
- **Observé dans le code :** aucune interprétation JUnit avant la fin de toutes les commandes. Une interruption sort avant `_junit` ; un XML tronqué interrompt sa compréhension initiale. L’assertion précédente n’est jamais agrégée : verdict `INCONCLUSIVE`, sans cause5.
- **Impact :** perte de diagnostic métier et de population partielle malgré la conservation des fichiers bruts.
- **Vérification minimale :** deux invocations factices : première avec failure/code8, seconde interrompue ; variante seconde XML tronqué. Vérifier priorité, cause5, verdict et relecture sans relance.

### Gap de cohérence hérité, pertinent pour le rapport

**3. P1 — Un manifeste contradictoire peut ressortir PASS/code0**

`scripts/verification/reporting.py:155–163,197–209`, `report`.

- **Déclencheur :** bundle hardware réellement `BLOCKED/code3`, receipt inchangé ; modifier seulement les champs agrégés du manifeste en `test_verdict=PASS`, `exit_code=0`, en conservant `pipeline_state=BLOCKED`.
- **Attendu :** protocole4 pour contradiction avec le receipt et la précondition enregistrée.
- **Observé dans le code :** comparaison état/code des receipts et réinterprétation CTest réservées à `COMPLETED`. La branche `BLOCKED` restitue les valeurs favorables du manifeste.
- **Impact :** faux favorable du rapport sans preuve graphique favorable.
- **Vérification minimale :** mutation synthétique limitée à ces deux champs ; exiger code4 sans processus ni écriture.
- Ce garde incomplet préexistait dans le chemin général ; ne pas le présenter comme une régression nouvellement introduite.

### Preuves et limites

Journaux lead recoupés : **20 + 3 tests ciblés**, **167 core dont 2 skips symlink**, **28 parser**, conformes aux résultats annoncés. Ils ne couvrent pas les combinaisons discriminantes ci-dessus.

Lecture du candidat et des contrats réalisée ; comparaison Git exacte à la base RED non régénérée sous l’interdiction de processus. Aucune preuve native Windows/Linux, C++ ou GL ajoutée. Revue non exhaustive ; aucun besoin de délégation structurelle, shader ou performance identifié.

## assistant — 2026-10-30T05:14:19.608Z — msg_18b3af654001Qk2nPBUuUCj1Q4

## Ré-audit `617e939` — réception encore réservée

HEAD confirmé ; lecture seule, aucun processus lancé.

### Trois findings précédents levés

- **FAIL mixte conservé** : `_junit`, `evidence.py:921–932`, préserve désormais `FAIL` et décide depuis le code agrégé.
- **Assertion avant interruption/XML tronqué conservée** : checkpoint par invocation, `evidence.py:997–1000`, avant l’enfant suivant.
- **Contradiction hors COMPLETED rejetée** : `reporting.py:164–170`, contrôle désormais état/code du receipt et interdit PASS hors COMPLETED.

### Nouveau bloquant — P2 : l’empreinte ne prouve pas la propriété du checkpoint

**Localisation :** `scripts/verification/evidence.py:843–848`, `_publish_ctest_population` ; admission des sorties `:639–646`.

**Scénario discriminant :** l’adaptateur déclare comme sortie JUnit `<bundle>/raw/ctest-population.json`, initialement absente. Cette destination est admise. L’enfant factice y écrit un XML conforme ; `execute` l’enregistre comme artefact avant `_junit`.

**Attendu :** refuser cette collision avant l’enfant, ou conserver intégralement le résultat brut sans l’écraser.

**Observé par lecture :** le helper trouve un fichier existant dont l’empreinte correspond au manifeste et le considère comme son checkpoint. `_write_json(..., replace=True)` remplace alors le **JUnit brut** par le résumé JSON. L’agrégation finale échoue ensuite en reparsant ce fichier ; les octets originaux sont déjà perdus.

**Impact :** violation du no-clobber et de la conservation des preuves brutes. Le contrôle d’empreinte détecte une modification externe, mais ne distingue pas un résultat enfant d’un checkpoint précédemment publié.

**Vérification minimale proposée, NON_EXECUTE :** reprendre la fixture nominale hardware et modifier uniquement sa destination `--output-junit` vers ce chemin. Vérifier refus préalable ou conservation exacte des octets enregistrés dans `runner.written`. Une réservation privée de cette destination à l’admission suffirait, sans nouvelle API.

### Preuves et limites

Journaux lead recoupés : **4/4 régressions**, **171 core dont 169 satisfaits et 2 skips symlink**, **28/28 parser**. Aucun nouveau résultat exécuté indépendamment ; aucune qualification native Windows/Linux ou GPU. Pas d’autre finding établi dans ce ré-audit ciblé, non exhaustif.

## assistant — 2026-10-30T05:28:25.121Z — msg_18b47dd1d001BTFnYM3aFMM3Lo

## Dernier blocage levé — `bdcaac8`

HEAD confirmé. La collision est désormais refusée dans `_execution_requests` (`evidence.py:609,641–647`) :

- comparaison des chemins canoniques pour `--result` et `--output-junit` ;
- refus code4 avant découverte/enfant (`execute:971–976`) ;
- destination partagée avec `_publish_ctest_population`, dont l’empreinte reste correctement décrite comme contrôle d’intégrité.

Le test ciblé couvre destination absolue et relative normalisée, absence d’enfant et conservation des XML nominaux.

**Aucun nouveau finding dans ce petit delta.** Les trois levées précédentes restent acquises ; avis favorable à l’intégration sur ce périmètre.

Journaux lead recoupés : **1/1 ciblé**, **172 core : 170 satisfaits + 2 skips symlink**, **28/28 parser**. Aucune exécution indépendante par moi, aucune modification. Qualification native Windows/Linux et GPU toujours **NON_EXECUTE** ; revue non exhaustive.