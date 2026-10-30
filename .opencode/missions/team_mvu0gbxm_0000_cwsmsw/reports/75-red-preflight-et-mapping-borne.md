# 75 — RED du transport et transfert d'une assertion bornée

## Résultat

Après lecture du mapping et des trois tests complémentaires, le coordinateur les a exécutés : **trois méthodes, 13 échecs d'assertion avec sous-cas**, aucun error ou skip, empreintes stables. Production, vingt tests collecteur et régression historique étaient inchangés. Le refus inconditionnel empêche encore la description pure ; les valeurs étrangères et identités dupliquées donnent code3 au lieu de protocole4.

## Mapping ratifié

Le collecteur doit inspecter les commandes pour vérifier leurs six valeurs explicitement déclarées. Un unique appel pur `describe_execution` est donc admis avant refus pour transport absent/partiel. L'ownership de la seule assertion correspondante dans `test_review_regressions.py` est transféré au propriétaire de la tranche : attendre `[*adapter_calls, 'run']` au lieu d'une liste inchangée.

Toutes les autres assertions restent conservées : BLOCKED/code3 pour transport absent, zéro enfant, zéro nouvelle sonde de révision, zéro observation device et zéro appel au parser. L'absence de cible reste refusée avant factory et description. Une description n'est pas une autorisation de sonder, et un contrôle de transport n'est pas une preuve GPU.

Le GREEN minimal peut maintenant appliquer ce mapping explicite et les validations complémentaires, sans nouveau schéma public ni suppression de test. Le reste du fichier de régression, les autres tests acquis et les nouvelles assertions restent hors modification. Les adaptateurs de production demeurent à raccorder séparément.

## Preuves

- Candidat `1101b64707e1f336c61794fc1ffbb394c721fa5b`, auteur pool-14, runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_context_preflight_contract.py -v`, worktree auteur.
- Logs et empreintes : `out/verification-synthetic/context-preflight-red-2026-10-30T04-18-50-495Z/`.
- Mapping source : `CONTEXT_COLLECTOR_ORACLE_MAPPING.md` ; précision persistée dans le contrat local de contexte avant édition de l'assertion.

Aucun enfant réel, CTest, sonde device, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
