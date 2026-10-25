# 18 — Reprise approuvée et préparation de l'audit final

2026-10-25. Révision native2 approuvée, hash b7e5fe216a098cfc79584871a48d10ff0567cd5be160685b628557fc8574d043.

## État et preuves recoupées

`team_status` : lead de mission-91cc703bafaaebfc, approved v2, long_run/running, classification manual, calls0. Aucun changement de catalogue. HEAD canonique582b156cbde1390060a749ba215ce4d21835768e ; aucun diff suivi ARTIST. Diff82bf52d→HEAD vide pour ARTIST, cmake et CMakeLists/Presets. Les neuf empreintes d'artefacts de verification-summary.json ont été recalculées et concordent. Cette vérification ne réexécute aucun test.

Preuves conservées : rapport12 et out/qa-b2-h3/final-h4-20261025-01/ ; ASan555/555,484CPU+71GL,0failure/error/skip,17historiques chacun une fois selon archive recoupée. CPU normal484/484 et71GL normaux historiques distincts, pas555normaux nouvellement exécutés. Remise39/H3 déjà intégrés, generator non relancé. Diffs concurrents et index préservés.

## Plan restant et admission

Audit documentaire/statique uniquement, test-reviewer neuf ; entrée neutre audit-final-input.md, sans rapports auteur ni revue correctness. D'abord restitution de profil/contexte, lecture et transport ; analyse après contrôle lead. Aucun shell, test, édition ou délégation. Adaptation approuvée : claim natif dédié autorisé, pas consultation du board d'implémentation ; worktree RO toléré, sans promesse de sandbox/invisibilité. En cas de refus connu : une seule tentative, voie canonique explicitement autorisée, sinon BLOCKED.

Évaluation locale PM/LLM de cette tâche indivisible d'analyse : complexité brute/retenue55/55, risque60/60 ; incertitude principale accès/profil effectifs, puis capacité des oracles à détecter des mutants plausibles. Aucun développement donc plancher70 non applicable. Consommateur clôture non publié avant remise intégrée. Lectures lead des preuves et audit peuvent être parallèles : aucun écrivain ni build partagé. Aucun nouveau worker pour un travail déjà livré.

Limite catalogue : team_tasks_add exposé n'accepte pas require_context ni context par tâche ; appel vide effectué, sans prétendre avoir activé un champ absent. Fresh sera demandé au spawn et vérifié à l'admission, non déduit de la prose. Pas de nouvel override modèle.

Publication : task_mvo3w8yq_000b_5nrkp7, C55/R60, agent test-reviewer explicite. Le spawn fresh/worktree:false a perdu la course face à l'auto-dispatch (Cannot claim: not unassigned and pending), sans nouveau worker doublon. Contrôle diagnostique unique : pool-4 est réellement routé test-reviewer, worktree isolé ensemble-2dc31e0a-dfde7a-pool-4, tâche dédiée en cours. L'adaptation approuvée tolère ce worktree ; fresh, permissions effectives et transport restent à constater dans la restitution d'admission avant analyse. Aucun PASS d'admission déduit du nom de profil. Attente de cette restitution, pas de polling ni de réadmission.

## Sortie et limites

Intégrer le rapport d'audit avant clôture/documentation finale. Les findings bloquants reviennent au propriétaire avec scénario discriminant et éventuel ré-audit ; aucune correction implicite du harness. F8 catch INCONCLUSIVE/close DEFER, C2 OPEN/INCONCLUSIVE, B1 historique non résolu, GCC natif non validé. Aucun Linux, Release, image, performance, backend nouveau ou PASS global. Statuts ADR inchangés. Aucun arbitrage supplémentaire demandé à ce stade.
