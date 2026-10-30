# Uniform scenarios — socle positif, SEMANTIC_RED_READY

## Autorisation et preuve anterieure

Le lead a rapporte l'execution du commit tests-first
`c36d714e47325331569b6abebcf3d9ecc4d31f41` : 17 tests, 17 echecs, code 1,
sources inchangees, cause `SCENARIO_PROTOCOL_MISSING`. Preuve communiquee par
le runner, pas executee par l'auteur ; journaux annonces :
`out/verification-synthetic/scenario-protocol-red-2026-10-29T23-03-22-970Z/`.
Ce RED etablit la disponibilite manquante, pas encore les branches semantiques.

Suite bornee autorisee par le lead : socle positif seulement, aucun C++, aucune
validation metier complete avant observation du SEMANTIC_RED et nouvelle suite.

## Candidat present

`scripts/verification/scenarios/protocol.py` fournit :

- `build_workload_argv` : tuple nominal dans l'ordre du protocole ratifie,
  chemins avec espaces conserves, flags GL omis pour le miroir CPU ; aucun
  lancement, effet de bord, import core ou dependance externe.
- `parse_workload_result` : `json.loads` seulement ; les observations et samples
  nominaux sont conserves. `expected` n'est volontairement pas encore consomme.

Les annotations expriment l'interface visee, pas une validation runtime.
Enum/charge/identites, schema complet, cles dupliquees, populations, operations
et validite des samples sont **non implementes a ce stade**. Le decodeur JSON
standard refuse deja une syntaxe tronquee ; cela ne prouve aucune validation
metier. Un resultat decode n'est donc pas une preuve acceptee par le pipeline.

**Ne pas integrer ce socle comme producteur qualifie ni l'utiliser pour campagne.**
Il sert a atteindre les assertions semantiques des tests existants, inchanges.

## Remise au runner lead

Depuis le worktree de l'auteur, commande identique, lead uniquement :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Attendu, non observe par l'auteur : les cas nominaux argv/JSON atteignent leurs
assertions et les cas negatifs echouent par `ValueError not raised` pour les
validations absentes (bornes, selections, identites, compteurs et samples).
Aucun filtre de test ni assertion modifie. Empreinte Git du fichier de tests :
`7fe6c23fbac2e63d6f90776e70562aeecb580084`.

Executions auteur : 0. SEMANTIC_RED et GREEN : INCONCLUSIVE, runner attendu.
C++/OpenGL, compilation, rendu, benchmark : NON_EXECUTE. Pas de mesure CPU/GPU,
de nouveaux tests, d'outils moteur, installation ou cloud dans cette tranche.

Le plan initial `RED_READY.md` reste l'historique de la premiere remise ; le
present document est l'etat de phase courant. Le modele core reste une vraie
dependance de raccordement, sans copie ni import dans ce module local.
