# 32 — Bornes Unicode : RED complet avant validation

Les compléments demandés ont été relus : uniquement tests et documentation, sans modification de production ni des 61 tests acquis. Le coordinateur a exécuté les **14 tests de cible matérielle** : **33 échecs d'assertion avec sous-cas**, aucun error ou skip. Les nouveaux cas atteignent bien les validations absentes de contrôles internes, de dépassement à 4097 caractères et de surrogate isolé.

Le cas nominal à **4096 caractères Unicode multioctets passe**, avec sérialisation et empreinte du plan conservées. La limite est donc vérifiée en caractères, pas en octets UTF-8. Les empreintes des sources contrôlées restent stables pendant l'exécution.

L'implémentation minimale GREEN du modèle et de la planification est autorisée. La prochaine vérification portera sur les 75 tests complets, sans modification des assertions. Le blocage effectif d'une exécution sans cible reste à démontrer dans les tests runtime, non dans ces modèles purs.

## Preuves

Candidat `362dace845d7f8fb647632a17a1841b7624bdc64`, auteur pool-3, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_hardware_contract.py -v`.

Journaux et empreintes : `out/verification-synthetic/core-hardware-bounds-red-2026-10-30T00-14-15-723Z/`. Aucun enfant de test, moteur, compilateur, rendu ou benchmark lancé.
