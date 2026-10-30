# 102 — RED du cache Windows, correction Python bornée autorisée

Le coordinateur a relu les 19 méthodes supplémentaires et leur compteur de lecture,
puis exécuté ce seul module. Résultat : **19 tests, 66 échecs d'assertion avec
sous-cas**, aucune erreur ou skip. Les setups produisent des reçus nominaux ;
l'échec causal est le binding d'exécution encore non implémenté. Les négatifs
n'acceptent pas ce placeholder comme un refus valide.

Les empreintes sont stables. Depuis le premier RED, seuls les trois ajouts annoncés
sont présents : tests, fixture de lecture et note. Les 28 premiers tests, le helper,
les pins et les anciennes assertions restent inchangés. Les assertions aval du
parser et de la lecture unique ne sont pas prétendues individuellement discriminées
par ce RED tant que le scaffold les masque.

Le GREEN minimal est autorisé sur l'adaptateur Python et la déclaration des quatre
paramètres privés PowerShell uniquement. **Le helper complet reste fermé** : sa
garde, le dispatch, le chemin historique et les régions pinées ne changent pas.
L'ouverture fonctionnelle attendra ses propres tests de fragments et leur RED.
Les 47 tests candidats et leurs assertions restent intacts ; seul le coordinateur
les exécutera après relecture du candidat corrigé.

## Preuves et limites

- Candidat `c199d80e1ea570f5bd142bd683af650ee40c9a9e`, auteur pool-23 ; runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest -v test_ctest_cache_binding`, cwd `scripts/tests/verification_windows` du worktree auteur.
- Logs et empreintes : `out/verification-synthetic/windows-ctest-cache-red-2026-10-30T06-57-15-246Z/`.
- Contrat du cache : rapport98 et `windows-adapter-binding-v1.md`.

Aucun PowerShell, outil natif, helper complet, moteur, OpenGL, rendu, benchmark,
installation ou cloud exécuté. Identité du binaire CTest et qualification native
restent non démontrées ; l'empreinte du cache n'est pas celle du binaire outil.
