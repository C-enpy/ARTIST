# GREEN_READY — corrections bornées des trois findings de contexte

## Entrées et attribution

Base : `55201be44c07545db972c6380b506b467a7227fb`, qui ajoute les quatre tests
à la production réauditée `50352e5ef096cecbb04a771984890c464073e619`.
Revue fresh : `result_mvuhogem_0043_6tlusy`.

RED observé par le **lead** sur `55201be` : quatre tests, quatre failures, zéro
error/skip, hashes stables, préconditions nominales atteintes. Diagnostics :
manifeste contradictoire0 au lieu de4 ; trois cas INCONCLUSIVE au lieu de FAIL.
Logs lead :
`out/verification-synthetic/context-review-red-2026-10-30T04-56-57-319Z/`.
Les assertions aval de population partielle n'étaient pas atteintes pendant ce
RED : elles restent intégrales, à vérifier réellement sur ce candidat.

Worktree :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-14`.
Branche : `ensemble-3b74f9ed-91ef-410f-8ac6-431ecb896a6d`.

**Aucun test exécuté par l'auteur. GREEN_READY n'est pas GREEN observé.**

## Cause et modifications

### 1. FAIL antérieur et précondition d'un autre producteur

`evidence._junit` remplaçait un FAIL déjà démontré par INCONCLUSIVE lorsque ses
propres findings n'étaient que des préconditions. Il décidait aussi de lever3 à
partir de ses seuls findings, malgré le code global5 déjà acquis.

Correction : préserver FAIL ; combiner les codes indépendamment du verdict et
décider depuis le code global. Une précondition3 ne coupe pas la collecte des
autres invocations/producteurs. Workload FAIL puis CTest précondition conserve
FAIL, causes5+3 et termine COMPLETED/5. Une panne4/interruption130 reste prioritaire
sur5, sans effacer une assertion établie.

### 2. Preuve de première invocation perdue lors d'un arrêt ultérieur

La collecte JUnit attendait la fin de toutes les commandes, puis parsait tous les
XML dans une seule compréhension. Une interruption du second enfant, ou son XML
tronqué, empêchait donc toute attribution de la première assertion pourtant
disponible dans un XML valide avec retour8.

Correction locale dans `execute` : après chaque invocation hardware CTest admise,
appeler le même collecteur avec `complete=False`, avant de lancer la commande
suivante. Le verdict FAIL, les causes et la population actuellement observée sont
alors enregistrés avant que la preuve suivante puisse manquer/être malformée.
La collecte finale reste obligatoire pour vérifier toutes les omissions et ne
donne PASS qu'à la population complète. Aucun nouveau mécanisme de processus ou
de thread et aucun changement de runner.

`complete` est un paramètre **privé** du collecteur, pas un champ de schéma :
`population_matches=false`, `selected_names`, `executed`, `failures`, `skips` et
les retours natifs disponibles restent les champs existants. `executed` représente
les cas JUnit complets observés, pas un nombre natif déduit d'une interruption.
La population partielle est le dernier préfixe validé ; les commandes/logs bruts
conservent aussi le statut et les sorties disponibles de l'invocation ultérieure
interrompue ou malformée, sans lui inventer des cas complets.

`_publish_ctest_population` publie le checkpoint avec le writer JSON atomique
existant. Une mise à jour n'est permise que si l'ancien fichier est déjà attribué
à ce run et possède encore son hash ; lien symbolique, fichier étranger ou preuve
supprimée/modifiée donnent une panne explicite. Le receipt terminal n'est créé
qu'après le chemin de fin existant. Les causes CTest sont attribuées par nom de
cas, puis dédupliquées entre checkpoints ; pas de double comptage d'un même
finding lors de la collecte finale.

Pour plusieurs XML hardware, `linux-evidence.py::result_summary` reçoit désormais
une agrégation XML **en mémoire**, via son entrée file-like déjà supportée par
ElementTree. Cela évite d'écraser/créer une chaîne concurrente de fichiers XML
dérivés à chaque checkpoint. Tous les XML originaux restent conservés/hachés et
référencés par leurs commandes. La voie logiciel/historique stricte reste finale
et inchangée ; pas de campagne ajoutée.

### 3. Contradiction manifeste/receipt hors COMPLETED

Le rapport vérifiait la cohérence de l'état/code d'exécution seulement dans la
branche COMPLETED. Un receipt BLOCKED/3 inchangé pouvait ainsi coexister avec un
manifeste BLOCKED/PASS/0 et ressortir favorable.

`reporting.report` compare maintenant l'état et le code du manifeste à son receipt
d'exécution dès que celui-ci est présent, dans tous les états. Un verdict PASS
avec receipt non COMPLETED est également refusé. Contradiction → protocole4,
sans réparer/écrire les fichiers ni relancer un processus. Les preuves, causes et
retours historiques restent lisibles, pas réécrits pour correspondre au manifeste.
Le gap hérité est corrigé localement, sans audit/refonte générale de la preuve.

## Ownership, erreurs et invariants conservés

Seuls `scripts/verification/evidence.py`, `scripts/verification/reporting.py` et
ce nouveau document changent. Les 171 tests, leurs assertions et le mapping unique
ratifié `f4021cc` restent inchangés. Modèle public, runner, adaptateurs, CLI,
parsers, C++ et API bundle/payload Windows hors modification.

Le sink reste propriétaire des fichiers ; checkpoints et agrégations sont des
données locales sans handle/emprunt GPU ou cache de capacité. Un seul écrivain
par bundle/build ; vérification de l'ancien checkpoint avant remplacement.
Les sorties/logs sont persistés avant validation comme auparavant. Les chemins
de capture/arrêt/retraite et leurs limites ne sont pas modifiés. Aucun résultat
partiel ne devient PASS par absence de preuve suivante.

## Vérification et commandes proposées au lead

Contrôles auteur effectués : AST des deux modules modifiés valide, diff-check
code0, aucun diff contre `55201be` sur les 171 tests et domaines hors ownership,
fichier de remise présent. Aucun import/exécution du candidat dans ces contrôles.
Ils ne prouvent ni le GREEN ni la non-régression. Après lecture du diff, le lead
peut exécuter séquentiellement, depuis ce worktree :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_review_regressions.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p 'test_*.py' -v
```

Inventaire attendu à recouper : quatre ciblés, 171 core, 28 parser. Aucune constante
de comptage ajoutée au pipeline. Conserver codes/logs/nombres effectifs, y compris
les skips historiques non exécutés, puis ré-audit indépendant pool18. L'intégration
reste bloquée jusqu'aux preuves et à cette revue ; aucune auto-revue qualifiée
d'indépendante.

Tests auteur, GREEN et non-régression ici : **NON_EXECUTE**. C++/garde native,
OpenGL, rendu et performances : NON_COMPILE/NON_EXECUTE. Aucun outil moteur,
CMake/CTest, processus de test/workload, installation, benchmark ou cloud exécuté
par l'auteur. Les seules commandes auteur sont diagnostics statiques Python/Git
et persistance du candidat.
Les adaptateurs de production restent une frontière distincte ; aucun résultat
synthétique ne qualifie leur comportement natif Windows/Linux.
