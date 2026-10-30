# R1 — RED_READY fichiers privés / ownership / propagation

Base de cette tranche : `67ba57ca34049895161a24df9d4397a0f7c634fa`, avec le
GREEN R2 remis séparément dans `b48947ab...`. Le contrat §8 R1 ratifié au lead
`d4d346af8071c707be00147b322b1c08f57ad7ca` a été lu sans transfert de branche.
Cette tranche n'implémente pas la capture et ne constitue pas un nouveau candidat
GREEN pour le run R2. R1 reste bloquant ; collecteur contexte inchangé/absent.

## Livrable TEST FIRST

`test_file_capture_contract.py` ajoute neuf méthodes :

1. Deux temporaires réguliers distincts donnés directement à Popen, contenu brut
   exact (octets non UTF-8 inclus), primaire reaped, deux lecteurs indépendants,
   positions des writers intactes, tous handles parent fermés et stockage retiré.
2. Timeout/interruption : préfixes réellement écrits dans les fichiers avant wait,
   sans aucun buffer stdout/stderr dans TimeoutExpired. Arrêt/reap finis, flags
   et statut natif conservés, aucun lecteur ou thread abandonné.
3. Tailles relevées avant lecture : le double écrit un suffixe immédiatement après
   chaque fstat de snapshot. Les lectures sont bornées à ces tailles, le suffixe
   est exclu. Deux handles writers ouverts séparément restent détenus par le test
   pendant le retour, puis sont fermés par celui-ci ; aucun descendant réel.
4. Échec de création du second temporaire : aucun enfant lancé, premier fermé,
   _CaptureError explicite, returncode None et cause OSError préservés.
5. Échec snapshot, création du lecteur ou lecture stderr après enfant0 : erreur
   de capture explicite, sans transformer le statut natif0 ; stdout déjà lu gardé.
6. PermissionError de nettoyage injecté : le vrai handle est d'abord fermé pour
   ne rien laisser au test. L'erreur ne peut être ignorée : résultat transporté
   avec code0 et sorties exactes, aucune conclusion favorable.
7. Reap jamais confirmé : wait du double expire à chaque appel, au maximum quatre
   appels permis, erreur explicite avec flags/statut connus, handles fermés.
8. BundleSink.command : runner séparé qui lève l'erreur de capture avec outcome
   disponible. Exiger persistance des deux logs byte-exacts, inventaire/trace avec
   code/flags d'origine, puis EvidenceError et cause lisible. En interruption,
   code130 garde priorité ; autrement panne code4, jamais code enfant inventé.
9. Préparation : erreur de capture malgré enfant0 => receipt FAILED/code4 et
   deux logs disponibles persistés avant l'échec de phase.

Ces tests ne récupèrent pas les sorties d'un double qui rejoue communicate :
les données sont écrites avec os.write dans les vrais fichiers possédés. Les
spies autour de NamedTemporaryFile, os.open/os.fdopen et fstat exposent les
opérations de propriété et les fautes ; ils ne font pas la capture à la place
du runner. `os.dup` et `communicate` sont refusés explicitement, de même que
la création d'un thread ou le fallback vers un vrai subprocess.Popen. Les lectures
sans taille finie sont refusées immédiatement, aucun wait réel ou EOF attendu.
Le seam proposé pour le GREEN est NamedTemporaryFile + os.open/os.fdopen pour
les lecteurs : sous Windows, le flag O_TEMPORARY est vérifié et les ouvertures
simultanées exercent réellement le partage/suppression, sur fichiers seuls.

## Socle minimal privé, pas implémentation GREEN

Seule addition production : `runner._CaptureError(outcome, cause)`, transporteur
privé sans I/O ni branche d'exécution. Ce socle évite un RED d'ImportError et
permet de tester la frontière sink sur un outcome réel. Ni CommandSpec ni
ProcessOutcome ni schéma de receipt ne changent. Le runner actuel continue
explicitement à utiliser ses pipes ; BundleSink.command n'est pas encore adapté.
Le RED attendu est donc causal : PIPE au lieu de fichiers ; propagation brute
de _CaptureError sans trace/logs ; échec de phase sans sorties disponibles.

## Mapping borné des doubles acquis

Les 134 méthodes existantes et leurs assertions métier sont conservées.
Trois adaptations de doubles seulement :

- `test_runtime_contract.py`, interruption : l'événement KeyboardInterrupt passe
  de communicate à wait ; les mêmes chaînes sont écrites dans stdout/stderr via
  les handles fournis au factory. Assertions interrupted/stopped/un seul enfant/
  chaînes disponibles inchangées. Les tests utilisant déjà un vrai programme
  Python possédé ne sont pas modifiés ; aucun n'est exécuté par l'auteur.
- `test_review_regressions.py`, drainage borné : les trois événements sont portés
  par wait, les mêmes préfixes écrits au factory plutôt que retournés dans
  TimeoutExpired. Les contrôles positifs/finis, nombre <=4, arrêt, flags et
  préfixes restent. Aucun mock de descendant n'a de PID ou handle tiers.
- `test_context_transport_contract.py` : ajout du seul wait nominal, communicate
  conservé comme compatibilité du double pendant le RED ; toutes assertions
  des six variables/env_delta et de non-mutation du parent sont inchangées.

`runtime_fixtures.RunnerDouble` retourne toujours ProcessOutcome directement et
reste byte-identique, comme les fixtures/contrats modèle/planning/hardware/R2.
Le fichier de tests nouveau porte neuf méthodes, total statique attendu143.
Un détail du socle/mapping proposé se corrige avant RED s'il ne reflète pas la
frontière ratifiée ; ni assertion métier affaiblie ni faux RED import/TypeError.

## Confinement et limites

Uniquement double Popen synchrone + temporaires sous un TemporaryDirectory
possédé, avec chemins imposés au factory. Aucun subprocess réel, pipe, thread,
signal, sleep, réseau, moteur, GL, build, installation, benchmark ou cloud lancé.
Les handles writers retenus sont ceux du test, pas ceux d'un processus. Cleanup
les ferme avant suppression du répertoire et ne masque pas PermissionError.
Une erreur filesystem réelle reste un échec, pas un SKIP ni un contournement.

Ces tests visent les octets visibles aux instants de snapshot, pas les buffers
utilisateur non flushés ou écritures ultérieures. Le stockage peut rester vivant
tant qu'un handle hérité réel existe : pas de promesse de kill/retrait d'arbre.
Aucune expérience parent/descendant réelle admise ici. Le test de fichiers sous
Windows ne prouvera ni l'héritage interprocessus ni une exécution Linux.
Serena est rattaché au canon ARTIST, serveur C++ seulement : les changements
Python de ce worktree sont relus dans leurs fichiers et références textuelles,
sans prétendre à une analyse symbolique Python exhaustive.

## Commandes proposées au lead

Après revue du socle/mapping, RED ciblé seulement (fichiers/doubles, aucun enfant) :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_file_capture_contract.py' -v
```

Après observation RED et autorisation GREEN séparée, puis correctif :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Auteur : vérifications statiques AST/diff/mapping seulement, aucun import candidat
ou test exécuté. RED/GREEN de cette tranche NON_EXECUTE par l'auteur ; ne pas
recycler le résultat R2 pour cette identité. Qualification Windows/Linux native,
C++ et GL NON_EXECUTE. Ré-audit indépendant requis avant réception globale R1.
