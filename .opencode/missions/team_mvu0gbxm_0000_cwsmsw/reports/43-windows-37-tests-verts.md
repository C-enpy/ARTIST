# 43 — Windows : 37 contrôles verts, raccordement réel encore distinct

## Vérification

Après revue du diff d'audit, le coordinateur a exécuté dans l'ordre les quatre tests d'audit, sept ciblés, onze des fonctions, dix du dispatch et cinq d'interface : **37/37 passent**, sans error ni skip. Les tests antérieurs sont inchangés ; empreintes stables.

L'audit autorise seulement les deux lectures exactes de clés brutes dans la fonction prévue et rejette accès hors périmètre, méthodes, écritures et incréments. Les maps `Keys`/`Count` sont restaurées, les quatre cibles sont admises et la configuration des tests est activée seulement lorsque nécessaire. La découverte différée est demandée par les arguments simulés ; aucune découverte CTest réelle n'a lieu.

## Suite et limites

Les réserves identifiées sont levées dans la portée des maps et commandes factices. Le helper complet reste bloqué, les fonctions natives et la ligne de commande publique restent à raccorder ; l'adaptateur attend le cœur intégré. Aucun fonctionnement réel MSVC n'est déduit de ces résultats.

Une tranche consolidée des frontières restantes est demandée avant code : transport, lancement d'enfants factices, sorties/erreurs et environnement enfant. Préférer les environnements explicitement transmis aux enfants aux mutations du parent évite une complexité inutile ; le choix sera relu dans le contrat concret proposé. Aucun nouveau banc ou permission générale n'est autorisé.

## Preuves

Candidat `769a55a8ae98d5425ec5625c3577fb39acca31a4`, audit isolé `de08042b6e644a7a2084c2bb49ff363c226fcb49`, auteur windows-pipeline, runner coordinateur.

Découverte unittest : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p <filtre> -v`, filtres séparés `test_psbase_audit.py`, `test_targeted_contracts.py`, `test_wrappers.py`, `test_dispatch.py`, `test_helper_contract.py`. Commandes exactes et logs : `out/verification-synthetic/windows-targeted-green-2026-10-30T01-54-56-375Z/`.

Aucun moteur, outil de construction réel, MSVC/vcvars, rendu, benchmark, installation ou cloud lancé.
