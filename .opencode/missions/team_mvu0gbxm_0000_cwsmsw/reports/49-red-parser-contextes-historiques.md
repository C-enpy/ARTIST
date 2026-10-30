# 49 — Le parseur de contexte détecté incomplet par ses tests

## Résultat et suite

Le coordinateur a relu les tests et le socle du parseur, puis exécuté les **28 tests synthétiques : 167 échecs d'assertion avec sous-cas**, aucun error ou skip. Les cas nominaux et les imports fonctionnent ; les doublons, identités étrangères, types invalides et logiciels présentés comme qualifiés ne sont pas encore rejetés. C'est le RED attendu du socle volontairement non validant.

Les empreintes des trois fichiers livrés sont stables pendant le run. La méthode de pureté échoue sur un doublon accepté, non sur un effet de bord détecté. Aucun processus enfant ou outil natif n'est lancé par ces tests.

Le GREEN minimal du seul parseur Python est autorisé, sans modifier les 28 tests ni leurs assertions. Le producteur C++ reste interdit à l'écriture jusqu'au GREEN observé et à la suite explicite du coordinateur. Le collecteur et les adaptateurs attendent toujours l'intégration effective de leurs entrées.

## Preuves et limites

- Auteur : pool-10 ; runner réel : coordinateur.
- Candidat : `886354a6f6320033a9ea6dc85bdb2ea88c7bf30e`, base `50af7923ff9094ac8e7a175d3256126204b4da41` ; arbre propre avant exécution, ownership limité aux trois fichiers annoncés.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p test_*.py -v`, depuis le worktree auteur.
- Journaux, commande et empreintes : `out/verification-synthetic/legacy-context-red-2026-10-30T02-37-50-773Z/`.

Le résultat ne prouve aucune garde du contexte réel. Aucun C++ compilé, moteur, OpenGL, rendu, benchmark, installation ou cloud exécuté. Le socle ne doit pas être intégré comme parseur validant.
