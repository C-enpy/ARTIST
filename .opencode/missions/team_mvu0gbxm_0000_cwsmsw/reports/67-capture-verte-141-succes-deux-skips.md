# 67 — Capture par fichiers verte, ré-audit avant intégration

## Vérifications exécutées

Après lecture du runner, de la persistance des erreurs et du contrôle de population du rapport, le coordinateur a exécuté les neuf tests ciblés : **9/9 passent**. La suite complète découvre ensuite **143 tests : 141 satisfaits, deux SKIP de liens symboliques, aucun échec ou erreur**.

Les tests et mappings de doubles sont inchangés depuis le RED. Les empreintes contrôlées restent stables. Les tests ciblés exercent réellement des ouvertures et suppressions de fichiers Windows, avec des handles détenus par le test ; ils ne démontrent pas l'héritage interprocessus ni le nettoyage global de descendants. Les deux SKIP restent INCONCLUSIVE, sans élargissement de droits.

Les deux régressions de population du build passent dans la suite complète. Il n'est pas nécessaire de rejouer leur candidat intermédiaire séparé : l'identité actuelle est celle effectivement vérifiée.

## Barrière maintenue

Le ré-audit indépendant est demandé sur ce candidat, conformément à la stratégie de capture et au domaine de fraîcheur ratifiés. Il doit vérifier notamment les chemins d'erreur, la fermeture des handles et la persistance des sorties/statuts avant panne. L'auteur conserve la source stable ; aucun avis favorable global n'est déduit de ce GREEN.

Le cœur n'est pas encore intégré. Le collecteur et les adaptateurs de production restent à composer ; la garde des anciens tests matériels reste provisoirement fermée à l'exécution depuis ce cœur. Aucun moteur, compilation, OpenGL, rendu, benchmark, installation, cloud ou expérience réelle de descendant lancé.

## Preuves

- Candidat `258a9cbc5523846b42e99a75d6facfc80e19ed2c`, auteur pool-3, runner coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_file_capture_contract.py -v`, puis même commande avec `test_*.py`, dans le worktree auteur.
- Tests/mappings inchangés depuis `c8cc3b6423ee44fb0e69cb36898f9f982cf01072`.
- Logs et empreintes : `out/verification-synthetic/file-capture-green-2026-10-30T03-35-17-227Z/`.
