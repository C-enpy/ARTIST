# 70 — RED : primaire non arrêté après interruption d'acquisition

Le coordinateur a relu puis exécuté le test ciblé d'interruption : **un test, un échec d'assertion attendu**, aucun error ou skip. L'injection ponctuelle est atteinte après acquisition du faux primaire ; le contrôle confirme qu'aucun arrêt n'a été tenté. La trace antérieure est restaurée et les empreintes restent stables. La production est inchangée depuis le candidat réaudité.

Le GREEN minimal du nettoyage englobant est autorisé. Le primaire possédé doit être arrêté/récolté avec attentes bornées sur les sorties exceptionnelles après acquisition et entre attente et nettoyage, avant snapshot et fermeture. Codes, flags et sorties disponibles restent conservés ; aucun arrêt de descendants n'est ajouté. Les 144 tests et assertions sont maintenus, avec toute adaptation structurelle d'injection déclarée explicitement avant exécution.

Une interruption arbitrairement répétée pendant le nettoyage n'est pas rendue magiquement impossible ; les limites best-effort doivent rester explicites. L'intégration attend le GREEN, la non-régression et le ré-audit du correctif.

## Preuves

- Candidat `aa9c17c68fff5ecd29190139b308177049be47f8`, auteur pool-3, runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_primary_ownership_interrupt.py -v`, worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/primary-ownership-red-2026-10-30T03-51-26-312Z/`.

Aucun processus réel, signal OS, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
