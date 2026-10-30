# 33 — Modèle et plan : 75 tests verts

Le coordinateur a relu les validateurs de cible et leur usage par la planification, puis exécuté les **75 tests du cœur : tous passent**, sans error ou skip. Les trois fichiers de tests sont inchangés depuis le RED ; les empreintes contrôlées restent stables.

Les cibles partielles, incompatibles avec le profil ou invalides sont rejetées. Les chaînes restent exactes, bornées en caractères Unicode ; une cible absente reste admissible pour un plan consultatif et figure dans ses préconditions non résolues. Les champs absents restent omis du JSON et les valeurs présentes participent à l'empreinte.

La prochaine tranche prépare les tests du chemin runtime consolidé : commandes, collecte, reçus, rapport et CLI, avec des processus et adaptateurs factices. Le refus réel d'une exécution sans cible, les codes de préconditions et la conservation des résultats bruts seront vérifiés à cette frontière. Ils ne sont pas déduits du GREEN des modèles.

Le cœur et les adaptateurs ne sont pas encore intégrés. Aucun moteur, compilateur, rendu, benchmark, installation ou cloud exécuté.

## Preuves

Candidat `0600a60368f2dc2d663162f989974bab522c2dd5`, auteur pool-3, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v`.

Journaux et empreintes : `out/verification-synthetic/core-hardware-green-2026-10-30T00-16-38-526Z/`. Assertions inchangées depuis `362dace845d7f8fb647632a17a1841b7624bdc64`.
