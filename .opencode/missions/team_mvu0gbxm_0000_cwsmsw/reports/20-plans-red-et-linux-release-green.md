# 20 — Plans sous test, préparation Linux Release verte

## Objectif et étape

La mission livre un pipeline Windows/Linux pour qualification ultérieure, sans campagne moteur. Deux remises ont été vérifiées par le coordinateur ; elles ne sont pas encore intégrées.

## Résultats observés

**Catalogue, plans et reçus de phases : RED causal confirmé.** Les 61 tests s'exécutent ; 89 échecs d'assertion, sous-cas compris, détectent le catalogue vide, les empreintes non calculées, les sélections incompatibles acceptées et les données empruntées. Aucune erreur d'import. L'auteur est autorisé à implémenter cette tranche sans changer les assertions. Le runner, la collecte et la CLI restent soumis à leurs propres tests préalables.

**Préparation Linux Release : GREEN, 6 tests sur 6.** Les commandes simulées respectent l'ordre Conan puis CMake, les configurations host/build Release et les arbres distincts. La garde Debug passe ; le code enfant est conservé. Les seules modifications des tests concernent le setup temporaire déjà autorisé. Cela ne prouve ni une compilation Release ni une exécution Linux.

Les empreintes des fichiers contrôlés restent identiques pendant les deux runs. Une première invocation du script d'analyse a échoué en syntaxe avant tout test ; elle a été corrigée dans cette session. Elle n'est pas comptée comme RED produit.

## Suite et limites

Les tests comportementaux Windows sous programmes factices seront relus avant lancement. Les interfaces communes doivent être intégrées avant leurs adaptateurs consommateurs. Les sources C++ attendent leur revue et l'extension explicite de cible matérielle. Aucun moteur, rendu, benchmark, installation ou cloud lancé.

## Annexe technique

- Plans : candidat `7bf86c14dc783e16f8e392cac6e4689c801dbf0d`, auteur pool-3 ; preuves `out/verification-synthetic/core-plan-red-2026-10-29T23-54-47-563Z/`.
- Linux : candidat `42828c3a01f997783a3d86da4013a99bf4ef5309`, auteur linux-pipeline ; preuves `out/verification-synthetic/linux-release-green-2026-10-29T23-55-14-687Z/`.
- Runner réel : coordinateur, `C:/Python313/python.exe -B -m unittest discover`, respectivement dossiers `scripts/tests/verification_core` et `scripts/tests/verification_linux`, filtres `test_*.py` et `test_release_contract.py`, option `-v`. Commandes complètes, logs et empreintes dans les manifestes.
