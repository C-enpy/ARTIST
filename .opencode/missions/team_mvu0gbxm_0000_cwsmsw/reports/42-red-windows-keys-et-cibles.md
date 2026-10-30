# 42 — Variables d'environnement et cibles de tests : RED ciblé confirmé

## Résultat

Le coordinateur a relu les nouveaux tests et confirmé que le helper, les audits et les 26 tests précédents sont inchangés. Les sept nouvelles méthodes s'exécutent : cinq sont rouges, soit **8 échecs d'assertion avec sous-cas**, et deux passent. Aucun error ou skip ; empreintes stables.

Le refus de restauration de `Keys`, l'absence de la cible unitaire dans la liste autorisée et la configuration des tests actuellement désactivée expliquent les échecs. Le snapshot `Keys`/`Count` fonctionne déjà.

La caractérisation nominale de deux arguments de 4096 scalaires Unicode supplémentaires passe, y compris leur transmission exacte au petit enfant Python. Ce contrôle ne qualifie pas les futurs workloads ; il vérifie ici le transport factice sous la limite de ligne Windows.

## Suite bornée

Le GREEN est autorisé pour ces défauts. L'audit peut admettre seulement la lecture exacte des clés brutes sur les deux maps validées dans la fonction de restauration. Les autres accès, méthodes et affectations doivent rester refusés, avec contre-exemples dédiés. Ce n'est pas une ouverture générale de permissions ni une sandbox.

Les quatre cibles réelles doivent être admises. La configuration active les tests seulement lorsque la sélection le nécessite et demande une découverte différée `PRE_TEST` ; le CPU seul conserve ses assertions sans tests. Le helper complet et les primitives OS restent non raccordés.

## Preuves

Candidat `d048c7de6112acf45f7cd47f655456146c1d1ceb`, auteur windows-pipeline, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_targeted_contracts.py -v`.

Logs et empreintes : `out/verification-synthetic/windows-targeted-red-2026-10-30T01-40-04-240Z/`. Aucun moteur, vrai outil de construction, MSVC, rendu, benchmark, installation ou cloud lancé.
