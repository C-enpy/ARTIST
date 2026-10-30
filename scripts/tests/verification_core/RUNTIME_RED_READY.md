# Runtime consolidé — RED_READY, pas une implémentation terminée

## Entrée et autorisation

Base acceptée `0600a60368f2dc2d663162f989974bab522c2dd5` : le runner lead rapporte
75/75, code 0, aucun failure/error/skip, tests inchangés et empreintes stables.
Journaux :
`out/verification-synthetic/core-hardware-green-2026-10-30T00-16-38-526Z/`.
Le lead autorise les TESTS RED consolidés avant toute implémentation runtime.
Aucune exécution de test par l'auteur ; les comptages ci-dessous sont statiques.

## Fichiers et périmètre

- `test_runtime_contract.py` : 49 nouveaux tests (8 runner, 33 phases/rapports,
  2 réconciliation JUnit, 6 CLI) ; total initial avec les 75 acquis : **124**.
  Le complément hardware whitespace ajoute depuis une méthode : total courant
  **125** ; les 49 méthodes runtime restent inchangées.
- `runtime_fixtures.py` : racine temporaire possédée, adapter/runner/parser et
  lecteur de révision explicitement injectés. Import sans création de fixture.
- `scripts/verification/model.py` : stockage ProcessOutcome ajouté ; les autres
  modèles et les trois fichiers des 75 tests sont inchangés.
- `runner.py`, `evidence.py`, `reporting.py`, `scripts/verify.py` : interfaces
  minimales non fonctionnelles pour atteindre les assertions, sans import manquant.

Le runner retourne actuellement un résultat vide sans lancer d'enfant, les phases
NOT_STARTED sans créer de bundle et la CLI `{}`/code0. **Ces socles sont uniquement
des entrées RED : ils ne qualifient rien et ne doivent pas être intégrés comme
runtime livré.** Leurs validations/GREEN attendent l'observation causale du lead.

## Plan de GREEN après le RED seulement

1. Runner : argv sans shell, environnement enfant copié, codes natifs, deux
   streams complets, erreur de lancement, timeout et interruption avec arrêt/reap
   du seul enfant possédé. Aucun arrêt de processus étranger.
2. Sink/étapes : un propriétaire par bundle unique, receipts liés au plan/run,
   commandes de la seule phase demandée ; reprise explicite depuis bundle, jamais
   reconstruction favorable à partir d'un ancien dossier. Avant/après réels des
   empreintes sources/config/scripts/contrat/shaders puis exécutables/cache build.
3. Collecte : conserver les fichiers bruts même rejetés, déléguer les workloads
   au parseur scénarios intégré, réconcilier découverte/sélection/JUnit par noms,
   réutiliser les gardes pures de linux-evidence sans appeler son main de campagne.
4. Rapport autonome en lecture seule : états indépendants, codes et toutes causes,
   fraîcheur relue (pas cache de report.json). CLI list/plan purs ; prepare/build/
   run explicitement isolés et report sans aucune relance.

Contrats précis exercés : refus hardware sans cible avant toute commande/sonde,
cible déclarée ≠ contexte observé, code20 précondition ≠ panne protocole, code10
oracle faux ≠ panne outil, interruption prioritaire, code0 sans résultat ≠ PASS.
Rien dans ces tests ne lance du GL ni ne mesure le moteur. Les durées synthétiques
zéro/négatives invalides restent brutes ; un effet dérivé nul/négatif ne devient
pas une panne de pipeline. Le temps est mural côté CPU, jamais thread CPU/GPU.

## Interfaces locales proposées pour revue d'intégration

Les Protocols ratifiés et evidence.prepare/build/execute/reporting.report sont
conservés. Les seams privés de cette tranche sont :

- `BundleSink(source_root, output_root, *, runner, revision_reader=None,
  result_parser=None)` ; aucun fallback de runner. `sink.bundle` n'existe qu'après
  un prepare réel. `BundleSink.load(bundle, *, runner, ...)` reprend les données
  persistées, pas une preuve inventée.
- Le lecteur de révision injectable renvoie `{head, delta}` ; seul ce relevé Git
  est doublé. Les tests exigent les vrais SHA-256 des fichiers de leurs fixtures,
  pas des empreintes données par un mock. En production, les lectures Git doivent
  passer par le runner explicite, jamais un second subprocess caché.
- `receipt.details['bundle']` transmet le chemin de bundle à l'adapter pour ses
  sorties brutes uniques. Le plan reste immutable ; aucune nouvelle méthode n'est
  ajoutée au Protocol PlatformAdapter. Ce champ doit être partagé aux adaptateurs
  à l'intégration coordonnée, sans copier leur branche ici.
- `result_parser(text, *, expected=dict)` correspond à la signature du parseur
  scénarios effectivement intégré en `cc26104d1ccc6c064218d84ebd22f35db389e3d2`.
  Ses deux clés optionnelles expected_gl_vendor/renderer sont des attentes du
  plan, PAS des champs d'identité à ajouter à la racine du résultat JSON.
  Le double contrôle uniquement l'attribution et l'acheminement
  de son rejet ; **ce n'est pas une preuve du protocole scénarios**. La validation
  de schéma/populations du vrai parseur reste sa preuve séparée et son raccordement.
  operations_expected/observed restent des dictionnaires updates/draws.
  Le double possède un rejet de qualification explicitement configuré, sans
  recopier les règles du parseur. Les données nominales hardware emploient ses
  champs réels expected_gl_*, qualification=MATCHED_EXPECTATION et contexte.
  Le cas nominal exige la transmission des attentes au callback ; le code20
  avec précondition non satisfaite reste distinct d'un document mal attribué.
  Pour composer sans protocole concurrent : valider d'abord le document avec
  l'identité seule ; distinguer ensuite code20 et qualification avec attentes.
  Une qualification indisponible ne devient pas une panne de JSON, tandis qu'un
  rejet d'identité/protocole ne doit pas être masqué par le code20.
- `reconcile_junit(discovered, selected, xml)` est une frontière pure de collecte,
  sans CTest réel et sans population historique. L'agrégation garde le code natif
  CTest (dont 8) tout en distinguant oracle faux et population/protocole invalide.
- CLI `main(argv=None, *, adapter_factory=None, runner=None, source_root=None,
  revision_reader=None, result_parser=None)` permet une composition explicite en
  test. Fournir un fake adapter mais pas son runner doit échouer avant effet.
  Une sélection de production sera une table fermée après intégration effective
  des adaptateurs ; aucun plugin ou nom de module provenant d'une entrée utilisateur.

Aucun nouveau schema workload, backend, helper de build ni fichier d'adapter.
Les interfaces partagées non intégrées restent une dépendance réelle du lead.
Le producteur scénarios est désormais intégré, mais absent de ce worktree :
inspection du commit en lecture seule, aucun checkout/cherry-pick/transfert.
L'injection reste suffisante pour ce RED. Le vrai parseur sera composé dans
l'arbre d'intégration coordonné ; aucun accès au code natif ne sera exécuté.

## Exécution autorisée et confinement à relire avant lancement

Imports top-level : bibliothèque standard, modules core, guard existant et
chargement de `scripts/verify.py`. Aucun adapter de production ou module workload
n'est importé. Les fixtures créent seulement des petits textes/binaires factices
dans TemporaryDirectory, dont le préfixe contient des espaces.

Tous les tests de phases utilisent RunnerDouble : toute commande ne commençant
pas par `owned-fake` lève immédiatement AssertionError. Aucun Git, CMake, Conan,
CTest, helper, exécutable moteur, installation, réseau ou cloud n'est appelé.
Le Git de provenance est un callback de données synthétiques, pas un subprocess.
Les résultats, JUnit et longs logs sont fabriqués sous la seule racine possédée.

Les seuls enfants réels prévus au GREEN sont dans RunnerContractTests :
`sys.executable -B <temp possédé>/owned fake child.py ...`. Ce script est écrit
dans setUp et contient seulement echo/env/exit/sleep et écriture de deux streams
synthétiques de 200000 caractères (pas une mesure de performance). Timeout : 2 s pour un sleep
30 s, avec stdout/stderr flush ; interruption : process double injecté (pas de
signal envoyé au runner lead). Le test timeout observe l'objet Popen possédé,
vérifie son arrêt et prévoit un cleanup kill/wait de secours sur cet objet seul.
Aucun descendant, ressource GPU ou arbre partagé.
L'exécutable absent est également un chemin dans cette fixture possédée.

La symlink d'évasion vise un autre dossier de la même fixture possédée. Si Windows
refuse sa création, le test la marque explicitement SKIP ; aucun contournement de
permissions, junction shell ou modification système. Cette couverture serait alors
non exécutée, pas acquise. Les autres tests de chemins/ancien résultat sont portables.

Les modes consultatifs sont testés avec sentinelles globales writes/subprocess/
réseau/imports natifs, et via l'entrée CLI réelle runpy en imports core froids,
bytecode compris. Les lectures de sources Python restent permises ; création de
bundle/cache ou chargement de workload est interdit. Report garde les fichiers
inchangés même après mutation de source ou présence d'un ancien report favorable.

## Commandes proposées au runner lead

RED runtime ciblé après revue des imports, script enfant et confinement :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_runtime_contract.py' -v
```

Puis non-régression/ensemble (125 méthodes déclarées avec le complément whitespace) lorsque demandé :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

RED attendu : assertions runner (résultat vide), phases NOT_STARTED, aucune preuve
créée, faux succès des populations ou CLI vide ; ni import manquant ni erreur de
toolchain. Aucun test exécuté par l'auteur. Windows/Linux graphiques, compilation
C++, rendu et performance restent **NON_EXECUTE**, y compris après un GREEN Python.
