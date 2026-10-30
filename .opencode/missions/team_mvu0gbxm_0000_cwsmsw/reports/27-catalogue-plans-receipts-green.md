# 27 — Catalogue, plans et reçus de phases verts

## Résultat vérifié

Le coordinateur a relu les modèles, le catalogue fermé et la planification pure, puis exécuté les **61 tests : tous passent**, sans error ni skip. Les deux fichiers de tests sont strictement inchangés depuis le RED. Les empreintes des fichiers contrôlés restent stables pendant le run.

Le candidat sélectionne les scénarios dans l'ordre demandé, vérifie leurs compatibilités, calcule l'empreinte canonique du plan et conserve des données possédées et immuables. Les reçus contrôlent leurs identités, leur phase et les chemins lexicaux. Les imports et opérations consultatives sont soumis aux sentinelles de pureté des tests.

Les noms CTest du catalogue sont des sélections issues des sources, pas une découverte exécutée. Le confinement réel des fichiers, les symlinks, la chaîne de reçus et les résultats des processus restent à valider dans les prochaines tranches.

## Suite

L'auteur prépare les tests de l'extension de cible graphique puis du chemin runtime complet : CLI, runner, collecte et rapport. Les adaptateurs restent en attente des interfaces réellement intégrées ; aucune branche non intégrée n'est implicitement transmise.

Le défaut du producteur CPU et le risque d'encodage Windows restent à corriger côté scénarios. Aucun moteur, compilateur, rendu, benchmark, installation ou cloud lancé.

## Preuves

Candidat `8399006fac21f9ba91a866ea549a5b9ca48b4577`, auteur pool-3, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v`.

Logs, commande et empreintes : `out/verification-synthetic/core-plan-green-2026-10-30T00-04-03-870Z/`. RED correspondant : `core-plan-red-2026-10-29T23-54-47-563Z/`.
