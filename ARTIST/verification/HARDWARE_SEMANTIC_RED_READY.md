# Cible OpenGL — socle nominal, SEMANTIC_RED_READY

## Preuve du runner et autorisation

Le lead rapporte sur `57e47178d4a1d268aac0f2a88caf849df3e1ef01` : 12 tests,
14 echecs d'assertions/sous-cas, 0 error/skip, code 1, sources stables.
Motifs : kwargs cibles absents et ancien schema d'attente refusant les deux
cles cibles ; ce n'etait pas encore le RED des validations semantiques.
Journaux annonces par le lead :
`out/verification-synthetic/scenario-hardware-red-2026-10-29T23-58-12-756Z/`.
Aucune execution de l'auteur. Le lead autorise ensuite le socle nominal seul.

## Seul comportement modifie

`scripts/verification/scenarios/protocol.py` :

- `build_workload_argv` accepte expected_gl_vendor/expected_gl_renderer,
  optionnels None ; sans paire, argv historique inchange ; paire fournie,
  flags ajoutes aux argv GL, chaque chaine conservee dans son argument.
- `parse_workload_result` separe ces deux cles facultatives de `expected`
  avant la validation/comparaison des neuf champs d'identite historiques.
  Les quatre parameters et le JSON resultat ne changent pas.

Les validations historiques restent actives. Aucune validation nouvelle des
paires, profils, types, controles, limite 4096 caracteres ou observations n'est
encore ajoutee. Un champ cible partiel peut donc etre ignore, une cible logicielle
ou discordante pas encore refusee. **Candidat intermediaire non integrable comme
qualification hardware** : aucun resultat decode ne prouve une cible conforme.

Les 12 nouveaux tests et les 17 historiques restent strictement inchanges.
Le natif et les descriptions du commit source `60f32c5` ne changent pas :
hardware natif reste fail-closed avant upload/draw, faute de transport natif.
Aucun core/model/adapter, schema ou hash copie ou modifie.

## Commandes proposees au lead uniquement

Observer les branches de la nouvelle tranche :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_hardware_expectation.py" -v
```

Non-regression historique distincte :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_contract.py" -v
```

Attendu non observe : cas nominaux de cible accessibles ; negatifs atteignant
`ValueError not raised` sur paire/profil/texte/observations ; plus d'assertions
HARDWARE_ARGV_CONTRACT_MISSING ou HARDWARE_RESULT_EXPECTATION_NOT_ACCEPTED.
Les 17 historiques sont attendus verts, sans modifier leurs assertions.
Le lead qualifie le SEMANTIC_RED avant autorisation GREEN des validations.

Executions auteur : 0. RED semantique/GREEN de l'extension : INCONCLUSIVE,
runner attendu. Source native, compilation, CMake/Conan/CTest, moteur, GL,
benchmark, installation et cloud : NON_EXECUTE.
La limite future native sera bien 4096 caracteres Unicode, pas 4096 octets ;
aucun contournement cross-langage ou code C++ anticipe dans cette sous-tranche.
