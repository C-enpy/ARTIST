# Core runtime — candidat GREEN_READY, non exécuté par l'auteur

## Attribution RED et périmètre

Le lead rapporte sur `7bd17a06f855de43e95de94b3d2d442c7462058f` : hardware
15 tests / 12 échecs whitespace ; runtime 49 tests / 93 échecs assertions/sous-cas,
aucun error/skip, sources stables. Journaux :
`out/verification-synthetic/core-runtime-red-2026-10-30T00-58-40-124Z/`.
Il a autorisé le GREEN runtime consolidé et le correctif NON VIDE.

Ce RED prouve les causes observées (stubs, phases NOT_STARTED, CLI vide,
réconciliation permissive, rejet whitespace). Beaucoup de contrôles aval
n'étaient pas atteints faute de prepare : **leur sensibilité individuelle n'est
pas prouvée par ce RED**. Ré-audit/gaps indépendants restent à faire après
intégration, sans transformer ce candidat en preuve exhaustive.

## Code modifié

- `model.py` : suppression du seul prédicat strip dans la validation de cible,
  limites/contrôles/scalaires inchangés ; ProcessOutcome strict et immuable.
- `runner.py` : argv sans shell, environnement copié, stdout/stderr complets,
  codes natifs et erreurs de lancement, timeout/interruption, arrêt et reap du
  seul enfant possédé. Les streams binaires sont décodés/re-encodés avec
  surrogateescape dans les journaux pour ne pas perdre des octets non UTF-8.
- `evidence.py` : bundles uniques, receipts de phases liées, manifestes remplacés
  atomiquement, logs exclusifs, commandes horodatées/codes natifs, whitelist
  d'environnement (pas de dump global). SHA-256 des fichiers sources, configs,
  scripts, contrat, shaders, binaires et supports build ; relevés avant/après et
  refs Git publiques pour la relecture sans subprocess. Les données brutes
  présentes sont inventoriées avant décodage/rejet. Pas de protocole workload
  recopié : parser injecté, ou import explicite du module scénarios intégré.
- `reporting.py` : lecture des preuves actuelles et vérification des identités,
  populations d'invocations et artefacts ; jamais de cache favorable report.json,
  aucune écriture/relance. États pipeline/test/mesure/fraîcheur séparés.
- `verify.py` : aide/list/plan purs, phases explicites, report en lecture seule,
  options scalaires dupliquées refusées. Un fake adapter sans runner explicite
  ne déclenche aucun fallback réel. Absence de cible hardware : aucun chargement
  d'adapter, aucune commande/sonde avant le refus de run.

Les **125 tests, leurs fixtures et assertions restent inchangés**. Aucun fichier
Linux/Windows d'un autre propriétaire, aucun helper existant, preset, CMake,
workflow, shader ou fichier C++ n'est modifié.

## Réemploi et composition

La réconciliation JUnit réemploie `linux-evidence.py` : result_summary pour
population/failures/skips, reconcile pour la baseline sans échec, jamais main.
Une panne de population/protocole n'est pas une conclusion de test favorable ;
le code natif CTest 8 reste conservé quand l'oracle échoue (CLI domaine 5).
Code20 est une précondition (3), code30 une panne (4), interruption 130 prioritaire.
Code0 sans résultat conforme n'est pas PASS. Les mesures demandées absentes ou
invalides ne deviennent pas zéro ; effet dérivé nul/négatif ≠ panne d'outil.

Pour hardware : validation générique de l'identité/protocole, puis code20 ou
qualification avec les deux attentes optionnelles. Une qualification refusée
n'est pas assimilée à un document mal attribué. Les compteurs restent des
dictionnaires updates/draws ; temps mural CPU, jamais temps thread CPU/GPU.

Les adaptateurs de production ne sont pas présents dans ce worktree. Le parseur
producteur intégré au lead n'y est pas transféré : les tests utilisent toujours
leurs injections explicites. La table fermée CLI attend WindowsAdapter/LinuxAdapter
construits avec source_root ; les noms/constructeurs et la configuration réelle
seront à raccorder/vérifier dans l'arbre intégré. Receipt.details transporte
bundle/source_root/build_root ; BuildReceipt ajoute executables. Aucun plugin
ou import utilisateur, aucun transfert Git d'entrée ou copie de branche.

## Ownership, synchronisation et limites

Un sink possède son bundle et ses logs. Modèles/export JSON ne prêtent pas de
collections mutables. Un fichier exclusif coordonne le writer du bundle et un
autre celui de son arbre de build ; les runs GL d'une même racine source ont
également un marqueur exclusif. Les seuls marqueurs détenus sont retirés, jamais
ceux d'un tiers. Ils coordonnent ce pipeline, pas des commandes externes ou des
processus d'une autre racine : l'opérateur garde l'exclusivité de la machine de
qualification. Aucune ressource GPU/cloud n'est réservée par cette livraison.
Un crash brutal peut laisser un marqueur : pas de nettoyage automatique non prouvé.

Les empreintes détectent des changements persistants, pas une mutation transitoire
annulée. La relecture des refs Git est conservatrice (une réorganisation des refs
peut rendre une preuve stale). Les logs bruts ne sont pas une sandbox/secrets
redactor général : ne pas transmettre de secrets dans argv ou demander aux enfants
de dumper leur environnement. Pas de promesse de récupération complète si le
stockage est indisponible. Un OS/driver qui ne permet pas de reap un enfant tué
peut retenir l'attente/lease ; un timeout ne prouve jamais une retraite GPU.

### Limite structurante à transmettre au coordinateur

Les sélections CTest **hardware** restent BLOCKED/3 faute de preuve structurée
de contexte. Source observée :
`ARTIST/component-tests/include/OpenGLComponentTests.hpp:43-44` journalise
seulement vendor | renderer | GL_VERSION, pas le masque/propriétaire de contexte
structuré du nouveau protocole. Le core ne fabrique pas cette qualification à
partir d'un nom non logiciel. Les sélections logiciel réemploient la garde
llvmpipe historique ; les nouveaux workloads hardware utilisent le parseur.

Le coordinateur doit décider le raccordement d'observations suffisantes pour ces
anciens tests avant d'accepter leur couverture hardware globale. Aucun contrat
n'est affaibli ni nouveau format C++ ajouté dans cette branche ; cette limite
ne bloque pas le contrôle synthétique des 125 tests, mais borne la livraison.

## Validation et commande proposée au runner lead

Auteur : analyse AST, revue des appels/états et comparaison des fixtures/tests
au commit RED ; aucun import d'exécution du candidat, aucun test lancé.
Le script enfant synthétique et les seules commandes autorisées restent ceux
de RUNTIME_RED_READY.md : Python possédé, doubles, données temporaires ; aucun
Git réel dans les tests de phases, helper, moteur, CMake/Conan/CTest réel, GL,
installation, benchmark ou cloud. Symlink conditionnelle : un refus de création
Windows reste un SKIP explicite et un gap, pas une preuve.

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Attendu : les 125 méthodes découvertes satisfaites, sous réserve de la fixture
symlink propre à la plateforme. GREEN/non-régression **NON_EXECUTE par l'auteur**,
à observer par le lead après revue. Compilation moteur, rendu, images, hardware
et performance Windows/Linux restent **NON_EXECUTE**.
