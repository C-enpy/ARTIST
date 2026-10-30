# RED_READY — quatre contre-exemples de la revue du collecteur

## Entrée et périmètre

Revue indépendante fresh `result_mvuhogem_0043_6tlusy`, candidat
`50352e5ef096cecbb04a771984890c464073e619`. Les reproductions étaient NON_EXECUTE
à réception de l'avis. Le lead a confirmé statiquement les chemins1/3 et demandé
quatre tests discriminants, sans GREEN anticipé : un croisement workload/CTest,
deux variantes d'invocation partielle et une contradiction manifeste/receipt.

Worktree :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-14`.
Branche : `ensemble-3b74f9ed-91ef-410f-8ac6-431ecb896a6d`.

Ownership de cette remise : nouveau `test_context_review_regressions.py` et ce
document seulement. Les 167 tests acquis, le mapping d'une assertion ratifié
dans `f4021cc`, la production et les interfaces restent inchangés. Le troisième
finding est un gap hérité, pas une régression attribuée au dernier collecteur.
Aucune API bundle/payload Windows, aucun nouvel adaptateur, schéma ou runner de
production. Aucun test ni processus exécuté par l'auteur.

## Plan borné et oracles

### 1. FAIL du workload puis précondition CTest

Précondition de fixture : un parcours mixte entièrement nominal termine
COMPLETED/PASS/0 avec les deux vrais parsers et les fichiers attribués.

Sur une nouvelle fixture : les routes de `gl.float-routes-correctness` produisent
de vrais JSON synthétiques FAIL/code natif10, validés par `parse_workload_result`
et par la qualification hardware exacte. Avant le premier CTest, le double
enregistre le manifeste : FAIL/5 et cause native10 doivent déjà être établis.
Une observation CTest attribuée UNAVAILABLE avec skip cohérent suit ces résultats.

Attendu : FAIL conservé, causes5+3 et population CTest complète avec skip ;
COMPLETED/5 cohérent dans receipt et rapport, comme la priorité5 existante à
l'intérieur d'un lot JUnit. Le RED causal attendu est l'effacement en INCONCLUSIVE
et/ou le BLOCKED lié à l'exception3, pas une erreur de protocole des fixtures.

### 2a/2b. Assertion de la première invocation puis preuve incomplète

Précondition : deux invocations JUnit entièrement nominales donnent PASS/0.
Dans chaque variante, la première invocation possède ensuite un vrai élément
JUnit failure, une observation conforme validée séparément avec le vrai parser,
et un retour natif8 propre à cette invocation. Le double calcule les codes par
groupe : la seconde invocation sans assertion ne reçoit pas8 par contamination.

- Variante interruption : seconde invocation sans JUnit disponible, retour natif
  -9/interrupted et stdout/stderr partiels propres à la fixture ; priorité130.
- Variante XML tronqué : seconde invocation retourne0 mais son XML est incomplet ;
  priorité protocole4. Le fichier tronqué et ses empreintes restent conservés.

Dans les deux cas, le rapport readonly et le receipt gardent leur code prioritaire
et l'état INTERRUPTED/FAILED attendu, **sans effacer FAIL/cause5**. Le fichier
`raw/ctest-population.json` doit conserver les champs existants de la population
disponible : sélection complète, population_matches=false, cas complets connus,
nom de la première assertion et code natif8 disponible. Aucun nouveau champ de
schéma n'est demandé ; `executed` compte ici les cas JUnit complets observés,
pas une estimation des corps natifs réellement exécutés après l'interruption.

Le RED causal attendu est l'absence d'interprétation de la première preuve :
INCONCLUSIVE au lieu de FAIL, cause5 et population partielle manquantes. Les
assertions de précondition vérifient d'abord le XML, son attribution et ses logs.

### 3. Contradiction du seul manifeste sur un bundle BLOCKED

Avant mutation : vrai bundle hardware synthétique, précondition structurée,
receipt BLOCKED/3 et rapport BLOCKED/INCONCLUSIVE/3 cohérents ; bruts inchangés.
Modifier uniquement `manifest.test_verdict=PASS` et `manifest.exit_code=0`, en
conservant pipeline_state=BLOCKED et tous les receipts exactement identiques.

Attendu : protocole4, aucun verdict favorable, aucune écriture/relance/sonde.
Les sentinelles readonly, le compteur du runner et tous les octets du bundle sont
comparés avant/après le rapport. Le RED attendu est le retour0/PASS actuellement
possible hors de la branche COMPLETED. Ce contrôle local de contradiction n'est
pas une demande d'audit général ni de nouveau format de preuve.

## Confinement et réemploi

Chaque méthode possède ses répertoires temporaires avec espaces, leurs faux
binaires inertes et fichiers de données. Les doubles/fixtures existants sont
réutilisés par sous-classes locales ; le générateur d'argv et le parser workload
intégrés sont réellement appelés, comme le parser de contexte intégré au cœur.
La description CTest existante est réutilisée via une vue locale conservant le
hash et la Request originaux, pas une copie de VerificationPlan/public schema.

Le seul vocabulaire runner accepté est `owned-fake`. Les services sont injectés
explicitement ; Popen et subprocess.run sont interdits pendant les phases de
fixture. Aucun helper natif, CMake/CTest, Git enfant, chargeur GL, moteur ou rendu.
Les imports sont les modules Python purs déjà intégrés, pas une branche copiée ou
un ImportError utilisé comme RED. Pas d'installation, cloud ou expérience de
descendants. Les contrôles nominaux sont des préconditions des quatre méthodes,
pas une campagne ou une extension de l'infrastructure.

Le sink possède les fichiers ; les snapshots de preuve dans le double sont des
copies de données, pas des owners/handles GPU. Aucun changement de synchronisation
ou de lifetime du runner. Les ressources temporaires sont fermées/nettoyées par
les fixtures existantes et les cleanups unittest, y compris en cas d'assertion.

## Exécution et prochaine frontière

Commande proposée au **lead**, après lecture du diff :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_review_regressions.py' -v
```

Inventaire statique attendu : quatre méthodes, sans nouvelles variations/campagne.
Le lead doit conserver résultats, diagnostics et hashes effectifs, puis seulement
autoriser une correction minimale. Aucun GREEN de ces quatre points n'est écrit
ou exécuté dans cette remise. Les anciens résultats ne prouvent pas ces scénarios.

Contrôles auteur réalisés sans import du candidat : AST valide, quatre méthodes
recensées, cinq sources d'imports locales vérifiées présentes ; diff-check code0
et diff contre `50352e5` vide sur la production et tous les fichiers déjà suivis
des tests core. Ces contrôles statiques ne sont ni une découverte unittest ni un
RED exécuté. Aucun résultat de la future commande n'est présumé.

C++/garde native/GL/performances : NON_COMPILE/NON_EXECUTE. Tests auteur : zéro.
Les vérifications statiques de syntaxe/diff, le commit exact et l'existence des
fichiers sont rapportés séparément dans la restitution finale.
