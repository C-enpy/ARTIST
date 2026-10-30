# 65 — RED de la capture par fichiers confirmé

Le coordinateur a relu les neuf tests, le mapping des trois doubles adaptés et le transporteur privé d'erreur. Les tests utilisent des fichiers temporaires possédés et un double Popen synchrone ; aucun processus, thread, descendant ou attente réelle n'est lancé.

Résultat : **neuf méthodes, 14 échecs d'assertion avec sous-cas**, aucun error ou skip. Empreintes stables. Le runner passe encore des pipes au lieu de fichiers ; le sink ne persiste pas encore le résultat disponible transporté par l'erreur de capture. Ces causes correspondent au RED attendu, sans panne d'import ou de toolchain.

Les assertions situées après l'admission des fichiers ne sont pas encore atteintes : cette exécution ne prouve pas individuellement leur sensibilité, ni le partage/suppression Windows des futurs fichiers ouverts. Ces préconditions seront vérifiées sur le candidat GREEN ; elles ne seront pas assimilées à un RED produit si l'environnement échoue.

Le GREEN minimal de la stratégie ratifiée est autorisé, avec conservation des 143 tests et des assertions métier. Les interfaces publiques ne changent pas. Les sorties disponibles, statuts et flags devront être persistés avant la propagation d'une panne de capture ; aucun quota ou faux code enfant ne sera ajouté. Toute réception reste soumise à relecture, non-régression et ré-audit indépendant.

## Preuves

- Candidat `c8cc3b6423ee44fb0e69cb36898f9f982cf01072`, auteur pool-3, runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_file_capture_contract.py -v`, depuis le worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/file-capture-red-2026-10-30T03-25-57-985Z/`.

Aucun moteur, compilation, OpenGL, rendu, benchmark, installation, cloud ou expérience réelle de descendant exécuté.
