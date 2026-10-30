# 74 — RED du collecteur observé, un seul auteur retenu

## Résultat

Le coordinateur a relu les vingt tests du raccordement au vrai parseur intégré, puis exécuté leur fichier seul : **20 méthodes, 33 échecs d'assertion avec sous-cas**, aucun error ou skip. Le code de production est inchangé et les empreintes restent stables. La garde matérielle provisoire retourne encore code 3 au lieu du nominal attendu ; aucune panne d'import n'explique le RED.

De nombreuses assertions aval d'attribution et d'agrégation restent non atteintes. Ce premier RED ne démontre pas leur sensibilité individuelle. Le GREEN minimal du collecteur est autorisé, avec observation par cas, preuves brutes, codes et causes distincts, rapport en lecture seule et aucune qualification inventée.

## Affectation en doublon

Le runtime a repris la tâche initiale du cœur sur pool-14 après l'arrêt de pool-3, alors qu'une fiche consommateur distincte avait été publiée sur pool-15. Le conflit est constaté à la remise de pool-14. Un seul auteur est retenu : pool-14. Pool-15 a reçu l'instruction de cesser les écritures, restituer son état puis rester inactif ; aucune complétion fictive ou cycle shutdown/réadmission n'est utilisé pour masquer le doublon.

La mise en attente native de la fiche supplémentaire reste à traiter si une voie autorisée est disponible ; elle n'est pas annoncée comme déjà effectuée. Aucune seconde implémentation ou fusion n'est autorisée.

Confirmation reçue de pool-15 : STOP respecté, aucune nouvelle écriture ou exécution, diff non commité vide au contrôle. Quatre fichiers de tests/proposition sont préservés dans `d4f697bcc9fae3507bfbde3519e888d13d7cd051`, sans code de production. Cette alternative reste non exécutée et non intégrée ; le worker demeure inactif, sans clôture fictive de sa fiche (`result_mvugipck_003i_kqhbal`).

## Sécurité avant retrait de la garde provisoire

Les vingt nouvelles assertions et les 144 tests acquis sont conservés. Si la régression historique « matériel non pris en charge » devient intentionnellement obsolète, le propriétaire doit proposer son mapping avant modification : absence ou déclaration partielle de contexte doit rester refusée avant tout enfant, sans fallback. Une description pure d'adaptateur nécessaire à ce contrôle ne doit pas être assimilée à une preuve de device ou introduire une sonde.

Aucun runner, schéma public, adaptateur ou C++ ne fait partie de cette tranche. Les raccordements de production et la QA finale attendent leur intégration effective.

## Preuves

- Candidat `19085a9ba1be1909be949eb74c96408930b6246e`, auteur pool-14, tâche initiale `task_mvu0yczu_0005_li9i9z` ; fiche supplémentaire `task_mvug6j0v_003c_eve8lc`.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_context_collector_contract.py -v`, worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/context-collector-red-2026-10-30T04-13-37-465Z/`.

Aucun enfant réel, CTest réel, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
