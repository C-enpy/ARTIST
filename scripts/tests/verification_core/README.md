# Core portable — P1 GREEN_READY, cleanup primaire englobant

Remise courante : [PRIMARY_OWNERSHIP_GREEN_READY.md](PRIMARY_OWNERSHIP_GREEN_READY.md).
Le stop/reap du primaire possédé est dans le finally englobant, avant snapshots
et fermeture des captures. Les 144 tests/assertions/mappings du RED aa9c17c sont
inchangés, y compris les repères AST. Aucun test exécuté par l'auteur ; GREEN,
non-régression et ré-audit restent à observer. R2/adaptateurs/collecteur inchangés.

Historique RED : [PRIMARY_OWNERSHIP_RED_READY.md](PRIMARY_OWNERSHIP_RED_READY.md).
Une méthode, une interruption déterministe à la frontière AST d'ownership,
soit 144 méthodes déclarées. Production et 143 acquis/mappings sont inchangés.
Aucun test exécuté par l'auteur ; aucun primaire ou signal OS réel.

Remise précédente : [R1_FILE_CAPTURE_GREEN_READY.md](R1_FILE_CAPTURE_GREEN_READY.md).
Deux fichiers privés et lecteurs indépendants remplacent les pipes/ReadToEnd ;
les erreurs de capture conservent outcome/logs/trace avant l'échec de phase.
Les 143 tests/fixtures/assertions et mappings du RED c8cc3b6 sont inchangés.
Aucun test exécuté par l'auteur ; partage Windows et sensibilité aval à vérifier.
R2 conserve l'identité séparée `b48947ab...` ; collecteur/adapters inchangés.

Historique RED : [R1_FILE_CAPTURE_RED_READY.md](R1_FILE_CAPTURE_RED_READY.md),
neuf tests ciblés fichiers/ownership/erreurs, soit 143 méthodes déclarées.
Contrat R1 ratifié `d4d346af...` ; transporteur privé _CaptureError seul ajouté,
sans capture GREEN ni raccordement sink. Trois anciens doubles mappés vers
wait/fichiers ; assertions métier conservées, RunnerDouble de phases inchangé.
Aucun test exécuté par l'auteur ; aucune expérience parent/descendant réelle.
La composition contexte reste après réception du cœur.

Remise R2 séparée : [R2_GREEN_READY.md](R2_GREEN_READY.md), commit `b48947ab...`.
Le lead a observé
le RED R2 et ratifié le domaine de fraîcheur au contrat §8. Le rapport réutilise
l'énumération privée d'admission, sans nouveau schéma/processus/sonde/écriture.
Les 134 tests/fixtures/assertions sont inchangés ; R1 et le runner sont intouchés.
Aucun test exécuté par l'auteur. L'intégration globale reste bloquée sur R1.

Historique RED : [REAUDIT_RED_READY.md](REAUDIT_RED_READY.md), deux tests ciblés
de population build (nouvelle DLL et cible exécutable ambiguë), soit 134 méthodes
déclarées. Les 132 acquis/fixtures et le code de production sont inchangés.
R1 garde une proposition de discriminant doublé, en attente du scénario précis
du reviewer ; aucun nouveau processus parent/descendant ou correctif anticipé.

Remise précédente : [REVIEW_GREEN_READY.md](REVIEW_GREEN_READY.md). Le lead a
observé les RED ciblés sur `797cc568...` et autorisé les corrections minimales.
Les 132 tests/fixtures/assertions sont inchangés. Aucun test exécuté par l'auteur.
Le finding symlink conserve une preuve runtime INCONCLUSIVE (RED SKIP) et exige
le ré-audit indépendant prévu ; aucun contournement de permission.

Intégration bloquée selon la revue indépendante transmise par le lead.
Historique RED : [REVIEW_RED_READY.md](REVIEW_RED_READY.md), six tests ciblés
dans `test_review_regressions.py`, soit 131 méthodes déclarées au total.
Les 125 tests/fixtures acquis et tout le code de production restent inchangés
dans cette tranche TEST FIRST. Aucun test lancé par l'auteur, aucun correctif
GREEN anticipé. Confinement et expérience de drainage exclusivement doublée
documentés avant le run du lead.

Contrat de transport ratifié `50af7923...` lu en lecture seule :
[CONTEXT_TRANSPORT_RED_READY.md](CONTEXT_TRANSPORT_RED_READY.md). Un test
supplémentaire, exclusivement doublé, vérifie le retrait des six variables
publiques héritées avant application du seul env_delta déclaré. Total courant :
132 méthodes ; les six régressions de revue gardent priorité et sont inchangées.
Aucun collector, parser de contexte, champ core ni correction de production ajouté.

Remise précédente : [GREEN_READY.md](GREEN_READY.md). Le lead a observé le RED
consolidé sur `7bd17a0` et autorisé le GREEN ; les 125 tests/fixtures/assertions
restent inchangés. Aucun test exécuté par l'auteur. Le candidat et sa limite
CTest hardware attendent revue/intégration et observation du GREEN par le lead.

L'historique RED et les décisions de tranches sont conservés ci-dessous.

Remise runtime : [RUNTIME_RED_READY.md](RUNTIME_RED_READY.md), 49 nouvelles
méthodes runner/phases/identités/résultats/JUnit/rapports/CLI. Avec le complément
whitespace ci-dessous, le total courant est de 125 méthodes déclarées.
Les 75 tests acquis sont inchangés. Le lead a observé leur GREEN sur `0600a60`
(75/75, code 0, sans error/skip, assertions et empreintes stables ; journaux
`out/verification-synthetic/core-hardware-green-2026-10-30T00-16-38-526Z/`).
La tranche actuelle expose seulement les interfaces et socles RED ; aucun
runtime fonctionnel ne doit en être déduit. Aucun test exécuté par l'auteur.
Lire le confinement, les imports et le script Python factice avant le run lead.

Complément RED demandé par le lead après finding indépendant : un nouveau test
positif hardware distingue NON VIDE de NON BLANC (espace ASCII, NBSP, EM SPACE
autorisés, sans contrôle). Constructeur/from_dict, JSON et hash exacts sont
exercés. Le validateur actuel utilise encore `.strip()` et doit produire des
échecs d'assertion explicites ; **aucun code de production n'est modifié dans ce
complément**. Les 75 méthodes acquises sont inchangées ; ce complément whitespace
ne modifiait pas les 49 runtime, alignées ensuite sur le producteur intégré.
Nouveaux comptages statiques : 15 hardware et 125 au total.

Commande ciblée runner lead avant toute correction du validateur :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_hardware_contract.py' -v
```

Les cas historiques `"  \t"`/`"\n\t"` restent invalides à cause des contrôles ;
aucune assertion de longueur 4096/scalaires/contrôles n'est affaiblie. Le constat
75/75 précédent reste borné aux cas couverts, pas une preuve exhaustive. Le RED
runtime consolidé décrit ci-dessus reste indépendant et non exécuté par l'auteur.

Le producteur scénarios est maintenant intégré au lead (`cc26104d...`). Une
lecture seule de son API a permis d'aligner le double runtime : attentes vendor/
renderer optionnelles dans expected, hors identité JSON racine, qualification
distincte du protocole. Les données nominales hardware reprennent ses champs
réels ; aucun parseur n'est copié/importé depuis une branche et aucun transfert
Git n'est nécessaire pour ce RED injecté. Les 49 méthodes runtime restent le
même périmètre avant leur premier RED ; détail dans RUNTIME_RED_READY.md.

L'historique des tranches précédentes suit.

## Entrées et limites

Contrat `.opencode/evidence/mission-uniform-pipeline/contract.md`, QA dans le
même dossier, présents au HEAD initial `1fba1c9`. Le brief auteurs impose que
le coordinateur exécute le RED avant autorisation de l'implémentation GREEN.
Ce RED est observé par le lead sur `e88d070c12b116a4c4972ee3936788e516ca84fe`
et son autorisation GREEN porte uniquement sur Request et CommandSpec.
Le lead a ensuite observé leur GREEN sur
`dd20115a8ea40cba9867183a416149bb080c3c9b` : 28/28, aucun failure/error/skip,
code 0, tests inchangés et hashes stables. Journaux communiqués :
`out/verification-synthetic/core-model-green-2026-10-29T23-09-03-404Z/`.
Le lead a ensuite observé le RED de la seconde tranche sur `7bf86c1` : 61 tests
exécutés, 89 échecs d'assertion/sous-cas, aucune erreur, code 1 et empreintes
stables. Journaux communiqués :
`out/verification-synthetic/core-plan-red-2026-10-29T23-54-47-563Z/`.
Il a autorisé le GREEN limité à cette seconde tranche, sans runner/evidence/CLI.
Le GREEN est ensuite observé par le lead sur `8399006` : 61/61, aucun
failure/error/skip, code 0, tests inchangés et empreintes stables. Journaux :
`out/verification-synthetic/core-plan-green-2026-10-30T00-04-03-870Z/`.
Aucun test n'a été exécuté par l'auteur. Aucun moteur, CTest, CMake, Conan,
helper de build, rendu, benchmark ou réseau n'est nécessaire à cette tranche.

## Plan borné

1. **S2, E2/E4/E6 — tranche modèle acceptée** : Request et CommandSpec, aller-retour
   JSON, propriété des données, paramètres stricts, ensembles fermés et bornes
   ratifiées. Tests livrés avant la validation fonctionnelle. Le commit RED
   conserve le socle dataclass permissif ; le candidat GREEN remplace ce socle
   par des validations partagées par constructeur/from_dict, séquences possédées
   et copie immuable d'env_delta. Les copies JSON restent mutables et détachées.
   La compatibilité entre champs, le catalogue et l'autorisation des phases
   restent aux tranches suivantes, non implémentées ici.
2. **S1/S2/S3 — GREEN observé par le lead** : types ScenarioSpec/VerificationPlan et
   receipts, catalogue/planning purs, sentinelles écriture/processus/réseau/import,
   hash canonique, sélection et compatibilité. Le Protocol PlatformAdapter expose
   seulement les signatures ratifiées. Le socle permissif reste dans le commit
   RED ; le candidat GREEN valide les métadonnées, sélections et reçus, calcule
   le hash canonique et possède les mappings JSON imbriqués. La CLI list/plan et les chaînes effectives de phases
   restent aux prochaines tranches RED, pas couvertes par ces tests API.
3. **S3/S4/S6** : runner injecté, sink propriétaire d'un bundle unique,
   receipts par phase et identités avant/après ; processus Python factices pour
   retours, timeout, interruption et sorties partielles. Jamais de repli runner.
4. **S5/S8/S10** : réconciliation noms/JUnit, résultats versionnés et mesures,
   priorités des erreurs, rapport en lecture seule et relecture après mutation.
5. Relire diff/callers et rejouer seulement les tests synthétiques autorisés.

Ownership : uniquement fichiers core ratifiés ; aucun raccordement aux fichiers
Windows/Linux/scénarios d'un autre propriétaire. Pas de modification de
`linux-evidence.py` : sa création exclusive, réconciliation exacte par noms et
comparaison des empreintes servent de contrats de référence, pas de campagne à
appeler. Leurs tests historiques sont lus, non exécutés.

## Invariants locaux

Les modèles possèdent leurs séquences/mappings et exportent des copies ; aucune
mutation d'entrée ou d'export ne modifie un plan. Les futures phases possèdent
leurs logs et leurs enfants, empruntent adapter/runner/sink explicitement et
invalident toute preuve au changement persistant d'identité. Un seul écrivain
par bundle/arbre ; aucun parallélisme de mutation promis. Les échecs partiels
devront conserver les journaux et les codes bruts. Ce core Python ne possède
aucune ressource GPU : contexte et fin d'utilisation GPU restent au workload.

## Commande proposée (coordinateur uniquement)

Depuis le worktree de cette remise :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Fichier : `test_model_contract.py`. **28 méthodes de test déclarées**, plusieurs
contre-exemples via subTest. Les tests n'exécutent aucun sous-processus ni outil
externe. Aucun test ni assertion n'est modifié pour le candidat GREEN.

## Preuve RED communiquée par le runner lead

Python 3.13.1, 28 tests découverts/exécutés, 63 échecs d'assertion sous-cas inclus,
code 1 ; aucune panne d'import/environnement, hashes avant/après identiques.
Causes : paramètres invalides non rejetés et env_delta emprunté/mutable (S2).
Journaux dans l'arbre du lead, non réexécutés par l'auteur :
`out/verification-synthetic/core-model-red-2026-10-29T23-01-29-624Z/`
(`manifest.json`, `stderr.log`, `stdout.log`).

## Seconde tranche : commande, historique RED et candidat GREEN

La même commande de découverte ci-dessus inclut désormais
`test_plan_contract.py` : **33 nouvelles méthodes**, soit **61 déclarées** avec
les 28 méthodes inchangées du modèle. Ce sont des comptages statiques, pas des
tests exécutés par l'auteur.

RED observé par le lead : catalogue vide, sélection non filtrée ou non rejetée, observations
non marquées unresolved, hash vide, métadonnées/receipts invalides acceptés et
mappings empruntés. Les assertions de pureté et les signatures du Protocol
doivent déjà passer ; aucun import manquant ni lancement d'outil n'est attendu.
Le guard vérifie aussi les imports à froid des nouveaux modules, bytecode inclus.

Représentations locales conservées après la revue du RED :

- `ScenarioSpec.producer` est un mapping JSON :
  `{'kind': 'workload', 'target': 'ArtistUniformCpuWorkload'}` ou
  `{'kind': 'selection_ctest', 'names': ['unit.fixture', 'opengl.fixture']}`.
  Ce mapping et ses séquences sont désormais possédés/immuables. Les noms
  réels du catalogue seront explicites, jamais un regex ouvert ou un comptage
  historique. Les tests emploient uniquement des noms synthétiques.
- `VerificationPlan.plan_sha256` est une propriété calculée sur `to_dict()`
  canonique, UTF-8, sort_keys, séparateurs compacts, ensure_ascii=False,
  allow_nan=False ; le hash n'est pas inclus dans son entrée.
- Le plan pur conserve `commands=()` ; les commandes concrètes sont décrites
  lors de la phase explicite par l'adapter. Les observations encore inconnues
  sont des clés `unresolved` (`source_identity`, `toolchain`, `executables` au
  minimum). Il ne sonde pas la machine et ne construit pas un faux reçu.
- Les trois receipts ont les champs normatifs, `state` appartenant à
  PipelineState et leur phase propre. Les chemins sont contrôlés lexicalement
  ici ; confinement réel/symlinks et validation d'une chaîne de receipts restent
  à evidence, pas à un modèle pur.

Les compteurs workload operations_expected/operations_observed restent des
dictionnaires `updates`/`draws`. Aucun parseur concurrent n'est introduit ici ;
le parseur scénarios sera composé à l'intégration. Temps mesurés : intervalles
muraux côté CPU, jamais consommation CPU thread ni temps GPU.

Métadonnées réelles recoupées avec le candidat scénarios commité `60f32c5`,
`scripts/verification/scenarios/descriptions.py` et
`ARTIST/verification/SOURCE_READY.md` : cinq scénarios, CPU pass-name sans upload
ni mesure, submission en Release, métrique `cpu_wall_submission` (samples ns).
`cpu_wall_seconds` dans la fixture Python n'est pas une métrique du catalogue
réel. Les sélections CTest sont converties en mappings de noms explicites et
recoupées statiquement avec TEST_F et les préfixes unit./opengl. ; elles ne sont
pas découvertes ou exécutées dans cette mission. Aucun fichier de la branche
scénarios n'est copié ni modifié, et aucun import interbranche n'est requis.

La combinaison Windows/opengl-software est refusée en planification selon le
contrat initial. La précision hardware (`expected_gl_vendor`/renderer) reste
une tranche de tests séparée AVANT toute extension du Request ou du GREEN.
Ce candidat ne transporte pas encore ces champs et ne prétend pas vérifier
le blocage d'un run hardware : aucune phase run n'est implémentée ici.

Aucun test n'est exécuté par l'auteur ; les preuves RED/GREEN des 61 tests sont
attribuées au lead. Tests et assertions inchangés depuis le RED.

## Troisième tranche : précision hardware, RED_READY

Précision transmise par le lead : deux champs Request optionnels,
`expected_gl_vendor` et `expected_gl_renderer`, omis de to_dict quand absents,
inclus dans le hash quand présents. La cible déclarée ne prouve jamais le
contexte/renderer réellement observé. Un plan sans cible reste consultatif et
doit la déclarer unresolved ; le refus effectif d'exécution reste à tester dans
la tranche runtime, pas dans un test de planning.

`test_hardware_contract.py` ajoute **13 méthodes déclarées** (total statique :
**74**), sans modifier les 61 tests acquis :

- paire omise/None, aller-retour JSON, immutabilité, égalité textuelle exacte ;
- rejet paire partielle, mauvais type, chaînes vides/blanches/NUL et présence
  hors profil opengl-hardware, via constructeur et from_dict ;
- hash attribué aux deux chaînes, aller-retour du plan et observations toujours
  non résolues même avec une cible déclarée ;
- planning sans cible marqué `expected_gl_target` unresolved ;
- sentinelles de pureté réutilisées, sans aucun lancement ni accès GL.

Le socle fournit seulement les deux champs optionnels et leur sérialisation
omise pour None afin de conserver les contrats acquis. **Il ne valide pas encore
la paire et ne marque pas encore la cible manquante unresolved.** RED attendu :
assertions de rejet non levées et absence de `expected_gl_target` dans le plan.
Pas d'import manquant ni erreur d'environnement attendue. Les noms proches mais
non ratifiés restent refusés ; aucun nouveau profil ni backend.

Commande runner lead : identique à celle ci-dessus, discovery verification_core.
Imports : standard library, modules core déjà acquis, guard de
`test_plan_contract.py`. Aucun enfant, helper build, réseau, workload ou outil
natif n'est appelé. La seule correction documentaire concomitante reformule
l'oracle CPU en « UniformContext value mirror for float » (float est le type de
valeur, pas le paramètre template de l'API), sans changer son producteur.

Après RED observé seulement : validation minimale de la paire et unresolved.
La tranche runtime suivante regroupera runner/chaîne de phases/identités,
résultats/rapports et CLI, avec adapter/runner factices injectés, écritures dans
des fixtures possédées et seuls enfants Python factices. Les résultats sans
preuve, populations incohérentes, identités modifiées, timeout/interruption et
refus hardware sans cible y seront vérifiés aux vraies frontières API. Aucun
runner, evidence, reporting ou CLI n'est ajouté dans la présente tranche.

GREEN de cette troisième tranche et tests runtime : NON_EXECUTE par l'auteur.

### Complément RED avant validateurs hardware

Le lead a observé sur `842b108` : 74 tests exécutés, 23 échecs d'assertion/sous-cas,
aucun error/skip et empreintes stables. Journaux communiqués :
`out/verification-synthetic/core-hardware-red-2026-10-30T00-12-09-209Z/`.
Le complément demandé reste TESTS ONLY avant toute validation GREEN :

- dans les deux boucles vendor/renderer existantes, ajout de contrôles C0/DEL/C1
  internes (LF, U+007F, U+0085), 4097 caractères non-ASCII et surrogate isolé ;
- un cas nominal de 4096 caractères Unicode pour chacun des deux champs,
  dont les encodages UTF-8 dépassent 4096 octets, avec aller-retour et hash.

**14 tests hardware déclarés, 75 au total** ; les 61 tests acquis et tout le code
de production sont inchangés par ce complément. Les nouveaux cas doivent échouer
par absence de rejet, sans panne d'import. Aucun test exécuté par l'auteur.
Commande ciblée proposée au runner lead :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_hardware_contract.py' -v
```

Aucune réduction du plafond en octets ; le surrogate représente du Unicode
invalide, pas une nouvelle restriction matérielle. Le refus effectif de run
hardware sans cible reste à prouver dans la future tranche runtime consolidée.

### Candidat GREEN hardware après le complément observé

Le lead rapporte sur `362dace` : 14 tests hardware exécutés, 33 échecs
d'assertion/sous-cas, aucun error/skip, code 1 et sources inchangées ; le cas
nominal de 4096 caractères Unicode passe déjà. Les contrôles C0/DEL/C1,
4097 caractères et surrogate isolé atteignent bien une assertion de rejet.
Journaux :
`out/verification-synthetic/core-hardware-bounds-red-2026-10-30T00-14-15-723Z/`.

Le candidat GREEN ajoute uniquement la validation Request de la paire complète,
réservée au profil hardware, sans conversion de casse ou suppression d'espaces,
1..4096 caractères scalaires Unicode, chaîne non blanche, contrôles C0/DEL/C1
et surrogates refusés. Le plafond porte sur les caractères, pas les octets.
La sérialisation omet toujours les champs absents et le hash conserve les
chaînes exactes. Planning ajoute `expected_gl_target` à unresolved si la cible
hardware est absente ; cela ne vérifie aucune observation matérielle.

Les **75 tests/assertions sont inchangés** depuis le complément RED. Aucun test
n'est exécuté par l'auteur ; commande complète proposée au runner lead :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

GREEN/non-régression courants à observer par le lead. Aucun enforcement de run,
runner, evidence, reporting ou CLI ajouté. La prochaine remise RED doit être la
tranche runtime consolidée, avec vrais appels API sous fake adapter/runner et
petits processus Python factices confinés, pas une tranche documentaire seule.
Windows/Linux graphiques, compilation C++, images et performances : NON_EXECUTE.
