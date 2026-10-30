# P1 — GREEN_READY, cleanup englobant du primaire possédé

RED lead sur `aa9c17c68fff5ecd29190139b308177049be47f8` : une méthode,
un failure attendu `primary.stopped=False`, zéro error/skip, sources stables.
Logs : `out/verification-synthetic/primary-ownership-red-2026-10-30T03-51-26-312Z/`.
Ce RED reproduit l'interruption après acquisition ; il ne prouve pas isolément
chaque assertion aval ou toutes les interruptions possibles.

## Cause et changement minimal

Le stop/reap était dans le chemin normal après wait. Une interruption extérieure
après l'affectation de child ou entre le handler du wait et le stop pouvait donc
être enregistrée puis rejoindre snapshot/close sans nettoyer le primaire.

Seul fichier production modifié : `runner.SubprocessRunner.run`.

- Initialiser `reaped=False` avant toute acquisition, pas après l'affectation.
- Garder la garde `if child is not None` et le premier wait protégé : les repères
  AST du test continuent à viser la même frontière, sans adaptation du test.
- Déplacer le bloc stop/reap dans le finally qui englobe acquisition, premier
  wait et handlers. Un primaire possédé sans reap confirmé y passe avant tout
  snapshot ou ouverture de lecteur, quelle que soit l'exception extérieure.
- Garder deux tentatives finies, terminate après interruption sans timeout,
  kill après timeout ou pour la seconde tentative ; aucun descendant énuméré,
  signalé ou attendu. Pas de boucle ni attente d'EOF supplémentaire.
- Une erreur du cleanup est explicite ; le finally de capture continue de tenter
  les lectures disponibles et fermetures. Statut enfant, flags et octets sont
  toujours transportés par l'outcome, sans code synthétique.

La frontière `_CaptureError` -> BundleSink.command -> EvidenceError (130 si
interruption, 4 sinon) est inchangée, comme les logs/trace et leurs schémas.
Aucune modification de sink, modèle public, CLI, adaptateur ou collecteur.

## Ownership, interruption et limites

Le primaire est possédé dès son affectation, et non à l'entrée du premier wait.
Le finally protège les deux fenêtres signalées dans acquisition/exécution avant
de passer au retrait des handles de capture parent. Le reap confirmé normal
n'entraîne aucun stop supplémentaire.

Le cleanup lui-même reste best-effort et borné : interruptions répétées ou panne
OS pendant arrêt/reap peuvent empêcher sa confirmation. Elles restent explicites,
sans garantie d'immunité SIGINT absolue ni promesse de fin de vie d'un descendant
ou d'une ressource GPU. Les tentatives de snapshot/close ne transforment pas un
reap non confirmé en succès. Les garanties et limites filesystem de R1 restent.

La fenêtre entre wait-handler et stop n'a plus de travail normal non protégé :
le stop se fait dans le finally englobant, avec captures encore possédées. Le
test cible précisément la fenêtre après acquisition ; la seconde et les limites
de répétition restent à recouper au ré-audit, pas à déclarer exécutées ici.

## Identité et preuve attendue

Les 144 tests/fixtures/assertions/mappings du RED aa9c17c sont byte-identiques,
y compris le test AST d'ownership. Aucun changement du point d'injection ni test
affaibli. R2 conserve son identité séparée
`b48947abecf7f65f48a92df8a9691dccdadf4ac9`, et son code n'est pas modifié.
La preuve précédente «141 + 2 SKIP» reste attachée à son candidat ; aucun rejeu
R2 ni recyclage de résultat pour ce diff P1. Le nouveau GREEN reste à observer.

Auteur : syntaxe AST, comparaison des fichiers acquis, revue du diff et de
l'ordre cleanup -> snapshot -> close seulement. Pas d'import/exécution candidat,
test, processus/descendant réel, signal OS, outil moteur, GL ou installation.

Commandes proposées au lead après revue :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_primary_ownership_interrupt.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
```

Attendu : une méthode ciblée, puis non-régression des 144 méthodes déclarées avec
skips/limites explicites. Aucun test exécuté par l'auteur. Ré-audit indépendant
pool8 après la validation lead, avant réception du cœur. Collecteur et adaptateurs
restent en attente ; Windows/Linux natif, C++, rendu et performance NON_EXECUTE.
