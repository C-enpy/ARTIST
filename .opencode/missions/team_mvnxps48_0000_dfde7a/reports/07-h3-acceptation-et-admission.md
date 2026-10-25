# 07 — Remise39 acceptée, H3 documentaire et admission observée

2026-10-25. Politique native approved v1, hash `4997265f7b67549546948a8865ef3292ad7bb06e7b80f077afc49cf35b338b26`. Cette continuation ne modifie ni approbation ni board historiques.

## Résultat H3

Le lead accepte la remise39 complète : sept parties et résultat natif terminal, KEEP E5/E6, aucun patch supplémentaire. Pas de nouvelle demande generator, pas de marqueur FIN_H2_GAPS exigé, pas de reproduction artificielle du TDD déjà établi. Les rapports31/32/39 ont été relus ; leurs limites sont conservées. Le rapport39 original est intégré par chemin nommé avec cette décision et le manifeste [h3-manifest.json](h3-manifest.json).

H3 désigne le candidat **source318bf7a0e5da2a65cc437ccc29baa92418822bc4**, sans delta `ARTIST/` ; le commit documentaire intégrant la décision sera distinct. Aucun patch moteur/test accepté ici. Les conditions GCC/F8/C2/B1 restent inchangées.

## Preuves recoupées sans nouvelle campagne normale

Contrôle canonique 2026-10-25T16:13:40.945Z : HEAD318bf7a ; `git diff HEAD --stat -- ARTIST` vide. Le dépôt reste non propre hors ARTIST ; l'index contient déjà `.grepai/config.example.yaml`, à préserver.

Analyse Python/ElementTree des archives normales : découverte555 noms uniques, exécution555, ensembles identiques,484unit+71OpenGL, zéro failure/error/skipped. Empreintes des deux exécutables normaux, du helper et de ses sept tests identiques au rapport31. Le résultat555/555 est **historique réutilisable**, pas un nouveau run.

Les trois exécutables ASan correspondent exactement aux empreintes de la passation42. Cache existant Debug/ASanON/CXX23_NATIVE=1/racineARTIST/toolchain Conan attendue. Préflight32 et canari conservés, aucune reconstruction lancée. Identité des binaires vérifiée aujourd'hui ; ces empreintes ne deviennent pas rétroactivement des empreintes datées du canari.

Les chemins et hashes décisifs sont dans le manifeste. Configuration et wrapper existants relus ; aucun bootstrap, Conan --build=missing, pip, installation ou build-windows.ps1 lancé. ASan final encore NOT_RUN à cette étape ; il exige sélection/manifeste avant run et bail exclusif.

## Admission native : faits, pas intention du PM

L'auto-dispatch a admis H3 sur **pool-1/build/worktree isolé** et QA-PORTABILITE sur **pool-2/build/worktree isolé**. Les paramètres conseillés dans le discovery n'ont pas configuré ces admissions. Le rôle du superviseur sur la continuation actuelle est bien natif ; aucun rattachement ancien n'est requis.

- pool-1 a signalé BLOCKED avant lecture/écriture, sans livrable H3 ni modification Git. Affectation laissée inactive ; aucun shutdown/réadmission ou fausse complétion.
- pool-2 a signalé les écarts de contexte et le refus unique de lecture externe de la politique Context Mode ; aucun shell/test/édition ni nouveau bootstrap. Son résultat est failed/BLOCKED, pas un diagnostic de fond. Aucun PASS QA/harness reçu.
- Tentative de spawn explicite QA-PORTABILITE avec correctness-reviewer/fresh/worktree:false et vrai claim refusée car tâche déjà affectée. Aucun worker doublon admis.
- Les membres restent inactifs ; aucun changement de modèle, aucune permission ouverte, aucune base interne modifiée. Le lead ne les réveille pas pour répéter un refus.

Le lead effectue **localement l'intégration H3 prévue par son contrat**, avec ownership exclusif des trois chemins documentaires nommés et sans changement de fichiers possédés par un worker. Cela ne qualifie pas les admissions précédentes de conformes et ne fabrique pas un succès auteur.

La campagne ASan reste runner lead uniquement. Le gate d'audit indépendant n'est pas validé ; aucune admission audit/clôture n'est autorisée par cette intégration. Les consommateurs de branches ne doivent pas être libérés sur la seule fin d'un worker : les preuves et cette intégration doivent être disponibles.

## Suite bornée

1. Intégrer cette décision/manifeste et le rapport39, exclusivement par chemins nommés, sans annexer l'index concurrent.
2. Figer sélection et identité ASan ; exécuter la campagne approuvée sous contrôle lead si les préconditions restent satisfaites.
3. Préserver les résultats d'admission BLOCKED et vérifier une voie native conforme avant reviewer. Diagnostic QA/portabilité non livré par pool-2 ; ne pas l'inventer.
4. Si les gates natifs restent incompatibles, fournir un bilan d'inachèvement avec les preuves acquises, pas une clôture globale ni un correctif du runtime implicite.

ADR0001/2/4/6/7 Proposed ;0003/5 Accepted/partial ;0008 Windows Debug seulement. F8 catch INCONCLUSIVE/close DEFER ; C2 OPEN/INCONCLUSIVE ; B1 historique non résolu ; GCC natif non validé.
