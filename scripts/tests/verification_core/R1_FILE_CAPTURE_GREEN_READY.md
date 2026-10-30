# R1 — GREEN_READY, capture privée par fichiers

RED observé par le lead sur `c8cc3b6423ee44fb0e69cb36898f9f982cf01072` :
9 méthodes / 14 failures de sous-cas, zéro error/skip, hashes stables.
Journaux : `out/verification-synthetic/file-capture-red-2026-10-30T03-25-57-985Z/`.
Causes atteintes : PIPE au lieu de fichiers, _CaptureError non persistée au sink.
Les contrôles aval snapshot/read/close étaient pour plusieurs non atteints :
ce RED ne prouve pas leur sensibilité individuelle, ni le partage Windows.

## Localisation / cause / correction

L'ancienne capture utilisait communicate et ses lecteurs Windows ReadToEnd :
un writer hérité pouvait retenir l'EOF, laisser lecteur/stream parent vivant et
consommer des octets sans les restituer dans TimeoutExpired. La stratégie R1
ratifiée au contrat §8 (`d4d346af8071c707be00147b322b1c08f57ad7ca`) remplace ce
couplage par deux fichiers privés, sans lecteur ou tâche parent en arrière-plan.

- `runner.SubprocessRunner.run` crée deux NamedTemporaryFile binaires non bufferisés,
  delete=True, directement passés à stdout/stderr de l'unique primaire.
- Attente initiale à la limite CommandSpec, puis au plus deux tentatives de
  stop/reap à délai fini. Timeout -> kill ; interruption -> terminate, puis kill
  si nécessaire. Aucun communicate, PIPE, thread, os.dup, groupe ou descendant.
- Après les attentes, relever les deux tailles puis lire ces seuls préfixes par
  os.open/os.fdopen indépendants. Sous Windows : O_BINARY et O_TEMPORARY pour le
  lecteur, compatibles avec la suppression partagée des temporaires nommés.
  Un préfixe raccourci est une erreur explicite, pas une capture silencieuse.
- Chaque lecteur/descriptor est fermé avant les writers parent, y compris en
  erreur. Une fermeture échouée est enregistrée et peut être réessayée une fois
  seulement si le stream reste ouvert ; ce retry n'efface jamais l'erreur.
- `_CaptureError` transporte l'outcome disponible et la cause ; plusieurs erreurs
  restent visibles dans une ExceptionGroup privée. Le statut enfant n'est jamais
  remplacé par un code synthétique. Les octets sont décodés avec surrogateescape.
- `evidence.BundleSink.command` intercepte uniquement cette erreur privée,
  conserve logs bruts/empreintes et la trace normale code/flags avant EvidenceError.
  Code130 si interruption, sinon code4. Les phases existantes publient l'échec et
  sa cause, jamais un receipt favorable ; pas de champ ou schéma supplémentaire.

La recherche textuelle des consommateurs locaux trouve le seul appel production
`BundleSink.command`; verify.main compose le runner. Les appels directs de tests
restent adaptés par le mapping déjà relu, sans nouvelle modification de ces tests.
Serena est rattaché au canon ARTIST avec serveur C++ seulement ; pas de preuve
symbolique Python exhaustive ni de reconfiguration du MCP pour ce worktree.

## Ownership et limites

Le runner possède writers, lecteurs et descriptors jusqu'à leur fermeture. Le
primaire emprunte les handles pendant Popen ; son héritage OS peut survivre au
scope C++/Python ou à l'attente primaire. Un descendant peut garder son propre
handle et prolonger le stockage temporaire ; aucune promesse de kill d'arbre,
de fin d'usage GPU ou de récupération de ses buffers non flushés n'est faite.
Les logs sont les octets visibles aux snapshots : ni EOF, ni suivi des écritures
tardives. Pas de quota silencieux ; espace/mémoire proportionnels aux sorties.

Les délais bornent les wait de processus, pas une panne bloquante de filesystem.
Un arrêt/reap non confirmé, une lecture/création/snapshot/fermeture échouée reste
une panne de capture explicite. Les tentatives de fermeture ne prouvent pas que
l'OS en panne a effectivement libéré chaque ressource ; aucun échec de nettoyage
n'est masqué comme un succès. Une panne de stockage lors de la persistance peut
elle-même empêcher une preuve complète : elle n'autorise pas un résultat favorable.

## Identités et validations

R2 reste identifiable séparément au commit
`b48947abecf7f65f48a92df8a9691dccdadf4ac9` : énumération source/build partagée
pour report readonly. Son code n'est pas changé ici ; ne pas attribuer le run
d'un autre candidat à cette tranche R1. Les 143 tests/fixtures/assertions et les
trois mappings relus sur le RED c8cc3b6 sont byte-identiques.

Auteur : AST, revue du diff/callers, comparaison des tests et des fichiers hors
périmètre uniquement. Aucun import/exécution du candidat ou test. Aucune action
moteur, CMake/Conan/CTest, GL, benchmark, cloud, grant/junction, nouveau processus
ou thread réel ; aucune expérience parent/descendant. Collecteur/adapters inchangés,
garde hardware maintenue jusqu'à réception du cœur et raccordement coordonné.

Commandes proposées au lead après revue du code :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_file_capture_contract.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Attendu : neuf méthodes ciblées, puis non-régression des 143 méthodes déclarées,
avec tout gap/skip explicite. Aucun résultat GREEN revendiqué par l'auteur.
Le partage/suppression Windows attend son test fichier réel confiné par le lead ;
précondition environnement défaillante != RED produit causal à inventer. Ré-audit
indépendant et gaps de sensibilité aval restent nécessaires avant réception R1.
Qualification native Windows/Linux, compilation C++, rendu et performance NON_EXECUTE.
