# 88 — Collision reproduite : les octets JUnit sont écrasés

Le coordinateur a relu puis exécuté la seule régression de collision : **une méthode, quatre échecs d'assertion avec sous-cas**, aucun error ou skip. La précondition nominale passe. Production, 171 tests acquis et empreintes restent inchangés.

Pour les destinations absolue et relative normalisée, deux commandes supplémentaires atteignent le runner au lieu d'un refus préalable. Le contrôle des octets confirme ensuite le remplacement du XML enfant par du JSON interne. Le RED établit donc réellement la perte de preuve brute, pas seulement un code final incorrect.

Le GREEN minimal est autorisé : réserver le chemin privé du checkpoint à l'admission, après canonicalisation, avant découverte ou exécution. La même règle doit interdire son utilisation comme sortie JUnit ou workload, sans nouvelle API ou schéma. Les 172 tests et assertions restent inchangés.

La correction sera relue puis vérifiée par le test ciblé et la non-régression, avant ré-audit indépendant. L'intégration du collecteur et les consommateurs d'interface restent bloqués jusque-là.

## Preuves

- Candidat `a4aa6bbe58650f789e7f0e41325b23802452416d`, auteur pool-14 ; runner : coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_context_checkpoint_ownership.py -v`, worktree auteur.
- Logs et empreintes : `out/verification-synthetic/context-checkpoint-red-2026-10-30T05-21-49-998Z/`.

Aucun processus réel, CTest réel, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
