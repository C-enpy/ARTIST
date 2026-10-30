# 108 — Déclarations Windows manquantes reproduites

Le coordinateur a relu puis exécuté les sept tests de description : **7 tests,
11 échecs d'assertion avec sous-cas**, aucune erreur ou skip, empreintes stables.
Les six variables sont effectivement absentes des workloads et de la découverte
CTest ; les cas CPU et transport d'exécution CTest constituent des contrôles
déjà verts. Aucun placeholder ou outil absent n'explique le RED.

Le GREEN minimal est autorisé uniquement dans windows.py : déclarer les six
valeurs par invocation matérielle depuis plan/reçu, avec identité stable et
distincte ; l'identité du workload doit aussi correspondre à son argv visible.
CPU inchangé. Aucun nettoyage du parent dans l'adaptateur : il appartient au
runner commun en revue. Les 54 tests et fixtures restent intacts.

Le helper PowerShell demeure fermé et inchangé. La correction du runner ne sera
pas copiée depuis une branche en cours : son intégration vérifiée précédera les
contrôles d'environnement effectif des consommateurs. Les tests de ce rapport
prouvent les descriptions, pas un contexte matériel réel.

## Preuves

- Candidat `76256132967b7133f57ce02f653ad0b9bf57d68c`, auteur pool-23 ; runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest -v test_hardware_binding_transport`, cwd tests Windows du worktree auteur.
- Logs et empreintes : `out/verification-synthetic/windows-hardware-transport-red-2026-10-30T07-18-03-822Z/`.

Aucun processus, PowerShell, outil natif, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté ; seules des données et doubles en processus.
