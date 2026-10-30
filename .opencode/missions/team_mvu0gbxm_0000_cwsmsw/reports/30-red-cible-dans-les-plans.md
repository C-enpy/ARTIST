# 30 — La cible graphique entre dans les plans : RED confirmé

Le coordinateur a relu la nouvelle tranche du cœur puis exécuté **74 tests** : **23 échecs d'assertion avec sous-cas**, aucun error ou skip. Les paramètres invalides et les paires partielles sont encore acceptés ; le plan ne signale pas encore la cible manquante. Ce sont les échecs attendus du socle avant validation. Les empreintes restent stables.

La revue des tests relève un manque borné : la limite déjà ratifiée de 4096 caractères et les caractères de contrôle hors NUL ne sont pas couverts dans les modèles du cœur. Des cas complémentaires sont demandés avant implémentation GREEN, ainsi qu'une borne Unicode nominale. Il ne s'agit ni d'une nouvelle fonctionnalité ni d'un durcissement de périmètre.

Les 61 tests précédemment acquis restent inchangés. Le refus d'exécution sans cible sera testé séparément dans la chaîne runtime ; un simple champ `unresolved` ne le démontre pas.

## Preuves

Candidat `842b1081cfbef1da8a0b25b760ebdb412f9b3835`, auteur pool-3, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v`.

Journaux et empreintes : `out/verification-synthetic/core-hardware-red-2026-10-30T00-12-09-209Z/`. Aucun processus enfant dans cette tranche, aucun moteur, build, rendu ou benchmark lancé.
