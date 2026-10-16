# Dérive observée d'une entrée protégée — GrepAI

Date : 2026-10-16, après admission des huit branches indépendantes.

Le contrôle avant commit du ruling QA a arrêté la suite AVANT staging :

- SHA256 initial `.grepai/config.yaml` :
  `d9d23a9d7a077d2692c48ce5bc9055b93afda93ec2aa85f1edd5e9c43966fd87`.
- SHA256 courant au contrôle :
  `d58ae9d1ef3bb0d277b96862ca94ee646f74a08ae347672c6aaf7aa741b207a9`.
- Les huit autres empreintes des entrées initiales restent identiques.

Inspection exécutée : `git diff -- .grepai/config.yaml`. Le diff COURANT contre
HEAD ne porte que sur `watch.last_index_time`, désormais
`2026-10-16T20:54:42.0608011+02:00` au lieu de la date du HEAD. `discover_worktrees`
reste false ; aucune différence de paramètre fonctionnel contre HEAD observée.
Ce champ est une métadonnée de l'indexeur canonique ; les recherches GrepAI
peuvent activer son watcher lazy. Cette explication est cohérente, pas une preuve
de l'identité du processus écrivain. Le baseline initial conservait un hash,
pas les bytes de configuration : aucune identité octet pour octet initiale
n'est revendiquée après cette dérive.

Décision coordinateur : préserver le fichier courant, ne pas reset/restaurer,
ne pas le commiter, ne pas réindexer ou changer le watcher. La dérive ne change
pas le contrat des auteurs ; elle est explicitement exclue de leurs deltas.
Pour les prochains commits ciblés, contrôler les huit autres hashes et vérifier
que le diff de configuration reste limité à last_index_time ; une nouvelle
différence fonctionnelle nécessite une nouvelle inspection avant admission.
La limite de preuve reste dans le rapport final. Aucun PASS global déduit de ce
contrôle, aucune pause opérateur levée, aucun worker existant interrompu.
