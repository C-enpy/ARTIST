# 01 — Préparation bornée et blocage de rattachement

2026-10-25. Préparation uniquement ; politique draft, exécution suspendue.

## 1. Objectif et résultat attendu

Terminer B1–B6 dans leur périmètre engagé : intégrer la remise generator39, figer H3, exécuter l'ASan final, obtenir l'audit fresh des tests puis actualiser les documents. Réutiliser les preuves valides ; ne pas relancer le generator ni les555 normaux sans raison nouvelle. Aucun changement moteur prévu.

## 2. Contrôles réellement effectués

- `team_status()` : cette session est lead de **mission-91cc703bafaaebfc**, sans teammate ; politique **draft** dans `team_mvnxps48_0000_dfde7a`. Elle n'est pas lead de l'équipe du handover **mission-aaae4c9ca30bb8be**. Aucun accès au board ancien établi.
- Context Mode `ctx_execute` fonctionne : marqueur `CONTEXT_MODE_EXECUTION_OK`, commandes Git en lecture seule exit0.
- `git rev-parse --show-toplevel` : racine ARTIST attendue ; `git rev-parse HEAD` : **318bf7a0e5da2a65cc437ccc29baa92418822bc4**.
- `git diff --stat -- ARTIST` vide ; `git status --short` confirme les diffs concurrents profils/harness/AGENTS/GrepAI et de nombreux rapports non suivis. Rien ajouté à l'index, supprimé ou commité ici.
- Lecture des principes, registre ADR, partie pertinente du plan de correction et rapports31/32/39. Rapport39 contient sept parties et résultat terminal, KEEP E5/E6 sans patch. Il reste à accepter/intégrer après reprise autorisée ; aucune complétion native effectuée.
- Présence vérifiée : JUnit et découverte normaux `out/qa-helpers/nonreg-20261024-02/`, résumé canari `out/qa-b2-h3/asan-canary/summary.json`, wrappers `out/tooling/run-build.py` et `msvc-env.cmd`. Présence seulement : contenu et identité binaire non revalidés ici.
- Politique ancienne : SHA256 **762ad6837788b24cc3b34af3721ba310e5852b4ba378798bc9d6bf7f1fc841c7**, cohérent avec handover42. Son approbation native n'est pas transférée à cette équipe.
- Un premier script de lecture a échoué par échappement de newline ; correction ciblée réussie, pas panne MCP.

Sources : [handover42](../../team_mvm73qjo_0000_6wzibc/reports/42-handover-nouvelle-session.md), [H2/RED/GREEN31](../../team_mvm73qjo_0000_6wzibc/reports/31-helpers-stricts-red-green-h2.md), [préflight32](../../team_mvm73qjo_0000_6wzibc/reports/32-revue-h2-et-preflight-asan.md), [generator39](../../team_mvm73qjo_0000_6wzibc/reports/39-h2-generator.md).

## 3. Politique réellement préparée

Le brouillon local réintroduisait Cursor et opencode-go contrairement au mandat de continuité. Première correction persistée uniquement dans ce nouveau brouillon : providers **openai, opencode, zai-coding-plan**. Format2, startingIntelligence50, beta0.35 inchangés ; aucun score catalogue modifié. **Correction initiale incomplète :** conserver les modèles de providers exclus rendait le brouillon invalide ; ces entrées sont désormais retirées, voir [rapport02](02-correction-catalogue-invalide.md). Le `config` natif doit présenter le résultat corrigé avant approbation.

QA **proportionate**, commandes/acceptations bornées persistées dans `verification`. Aucune classification lancée ou proposée. Ce document ne vaut ni config natif, ni approbation, ni transfert d'équipe. L'ancien fichier de politique reste intact.

## 4. Aperçu des tâches proposées — non publiées

Plan préparé localement, sans PM/worker avant approbation. Les estimations PM/LLM ci-dessous satisfont le contrat projet ; ce ne sont ni appels classifier ni modifications de grille. Aucun développement nouveau actuellement autorisé ; si nécessaire, complexité retenue >=70.

1. **R0 — Remise39/H3**, owner lead, contexte explicite handover+31/32/39. Relire preuves et diff ciblé, acter sans patch, intégrer seulement les documents nommés ; manifester identité H3. Dépend de la résolution du rattachement et de l'approbation/reprise native applicable. Analyse/documentation C35/R45, incertitude moyenne (board ancien). Aucun build.
2. **R1 — ASan final**, runner lead ; affinité build-engineer seulement si diagnostic nouveau utile. Dépend de R0 intégré. Contrôler cache/config/outillage/empreintes, figer sélection et manifeste avant run, vérifier les17 historiques, exécuter séquentiellement et recouper découverte/sélection/JUnit/logs tardifs. C55/R65, incertitude moyenne sur fraîcheur des binaires. Ressources exclusives : sortie ASan, logs et bail GL interprocessus. Aucun auteur de code prévu.
3. **R2 — Audit tests**, test-reviewer fresh, lecture seule, hors board d'implémentation ; brief contrat/candidat/preuves, pas justifications auteurs. Dépend de R0 et R1 intégrés, admission native vérifiée avant départ. C55/R60, incertitude forte sur transport/admission. Exiger scénario discriminant avant finding haut. Pas de shell, édition ou tests worker. Si impossible : BLOCKED, pas de substitution générique.
4. **R3 — Cohérence harness**, lead en diagnostic lecture seule des diffs concurrents profils/validateurs/tests, C45/R55. Indépendant de R1 après cadrage du rattachement : lecture parallélisable avec campagne, sans mutation des fichiers ni accès GL. Distinguer état disque, permissions réellement observées et tests harness. Si correction nécessaire : tranche séparée, ownership et plan préalable réutilisé/amendé, RED causal, C brute à estimer puis retenue >=70 ; pas d'annexion automatique du diff concurrent.
5. **R4 — Documentation et disposition finale**, lead, dépend de R1/R2/R3 et de leurs livrables réellement disponibles. C40/R45. Actualiser incidents/build-validation/current-state/correction-plan pertinents par chemins ciblés, maintenir frontmatter et ADR, réindexer uniquement artist-wiki. Réconcilier l'ancien board seulement avec autorité native et preuves, jamais pour le vider. Conserver les dettes même si le lot est clos à sa portée.

Graphe : rattachement/approbation → R0 → R1 → R2 → R4 ; rattachement/cadrage → R3 → R4. Publication des consommateurs seulement après intégration de leurs entrées, avec vrais IDs ; pas de tâche factice. La séquence R1→R2 garantit l'audit du candidat et de sa campagne finale ; R3 peut avancer indépendamment sans écrivain concurrent. Aucun worktree ni nouveau test nécessaire au plan nominal.

## 5. Acceptation et limites

ASan attendu : sélection complète555 si découverte inchangée,484CPU+71GL,17 historiques présents une fois, zéro failure/error/skip, canari et configuration identifiés, environnement canonique et /RTC1/MDd conservés. Aucun run ici. Un changement de population doit être expliqué avant exécution, pas masqué.

Les555 normaux restent le résultat historique rapport31, pas un nouveau PASS. Audit statique rapport32 distinct de l'audit final des tests. Harness concurrent non couvert par les555 C++. F8 catch INCONCLUSIVE/close DEFER ; C2 OPEN/INCONCLUSIVE ; B1 historique non résolu ; GCC natif, Release, image et performance non validés. ADR0001/2/4/6/7 Proposed ;0003/5 Accepted/partial ;0008 Windows Debug seulement.

## 6. Options et décision requise

**Recommandation : rattachement/transfert autorisé à l'équipe existante**, pour conserver son board et son approbation sans recréer la mission. Aucune API de transfert disponible n'a été établie ; ne pas détourner session_move ou team_handoff (qui ne transfèrent pas le leadership).

Alternative : décider explicitement d'une mission de continuation dans l'équipe actuelle, avec provenance de l'ancien board en lecture documentaire et nouvelle présentation `/ensemble-mission config` puis `/ensemble-mission start` par l'utilisateur. Cela ne permet pas de compléter les tâches de l'ancienne équipe. Ce n'est pas une migration déjà acceptée.

Décision groupée : rétablir le rattachement ancien, ou autoriser explicitement cette continuation distincte ? QA proposée : conserver proportionate et la campagne/audit déjà engagés ; light ne supprimerait pas ces obligations, reinforced exigerait un périmètre supplémentaire explicite. L'audit fresh demandé par le handover reste requis, sans imposer de nouvelle chaîne QA ni relancer planner/generator déjà livrés.

Aucun worker, classifier, build, test, installation, commit ou changement de source exécuté. Seuls ce rapport et le nouveau brouillon de politique ont été écrits pendant la préparation.
