# 16 — Revue de sûreté reçue, complément CPU compilé et passant

## Résultat

La revue C++ fresh indépendante du snapshot intégré ne démontre aucun défaut
bloquant ni suggestion nécessaire dans le périmètre examiné. Ce verdict reste
une lecture statique bornée, pas une preuve GPU ou exhaustive.

Le lead a appliqué les deux scénarios et trois assertions QA retenus, sans
modifier la production. Après une correction test-only de compilation, la
commande build Debug/unit s'est terminée à zéro avec 504 tests passants.
Les contrôles discriminants et la campagne finale normale/ASan restent à produire.

## Revue indépendante

Correctness-reviewer, session `ses_e82689115ffeJlKfuUPt54xUyz`, candidat
`dd2a2fa576ef1d4e6ffaf19a7ebf9722655b9a3f`, base `49005ed2`. Le reviewer a
examiné témoins/générations avant pointees, contexte/programme avant commit,
invalidation metadata/maps/free/destruction/moves, génération saturante et publication
map/estampille avant retrait. Aucune conclusion auteur ou du board utilisée.
Aucun edit/build/test/sous-agent exécuté. Références Serena partielles complétées
par lectures et grep exact, sans prétention d'exhaustivité des templates.
Contrat mono-thread, fenêtre vivante et absence de relink/delete externe impératifs.

## Incident du patch QA et réparation 1/2

Le patch candidat utilisait deux fois un accesseur protected pour observer une
génération. MSVC C2248 l'a rejeté ; sortie build wrapper 1, CMake build 2.
Ce n'est pas un RED comportemental ni un échec du moteur. La réparation remplace
ces appels par le témoin public déjà employé dans l'autre scénario, et ajoute
la vérification accepts de la génération conservée. Aucune visibilité production
ou assertion existante modifiée pour contourner le compilateur.

Le JUnit standard préexistant n'avait pas été renouvelé après l'échec de build.
Sa copie est gardée sous suffixe STALE-NOT-EXECUTED, exclue de tout décompte courant.
Relance avec hypothèse corrigée : build et 504 CPU passent, zéro skip annoncé
par CTest ; XML et identité seront encore recoupés avant la clôture.

## Exécution et suite

Exécutant réel : lead, propositions test-engineer. Commande identique aux deux
essais : `powershell -NoProfile -ExecutionPolicy Bypass -File
scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit`.
Journaux et JSON de commandes : `out/qa-prepared-float/lead-runner-20261027/`,
préfixes `01-build-unit` (échec compilation) et `02-build-unit-heal1` (exit 0).
Toolchain observée CMake 3.31.7, Conan 2.12.2, MSVC 19.44/toolset 14.44/SDK 26100.

Les headers de production sont identiques à ceux relus indépendamment. Le lead
peut maintenant réaliser les trois mutants séparés sur ce baseline, conserver
leur échec causal puis restaurer les sources et rejouer le nominal. GL sérialisé,
audit fresh hors board ensuite, sans approbation supplémentaire de jalon.

- [Propositions et réparation](../qa-gap-proposals.md).
- [Contrat](../contract.md), [plan QA](../qa-plan.md).
