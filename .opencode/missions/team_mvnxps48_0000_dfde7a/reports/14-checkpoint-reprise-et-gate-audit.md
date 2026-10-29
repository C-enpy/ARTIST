# 14 — Reprise exécutée ; dernier arbitrage d'admission de l'audit

2026-10-25. Backend de présentation arrêté ; lot C++ corrigé/vérifié ; mission non close.

## 1. Résultats intégrés

- **82bf52d** : correctif8lignes dans ShaderLifetimeTests.cpp seulement, contrat/QA commité avant auteur, RED causal86 puis GREEN ciblé1/1. Aucune production modifiée.
- **ASan final555/555**,484CPU+71OpenGL,0failure/error/skip ; noms découverte/sélection/exécution/JUnit identiques,17historiques chacun une fois ; logs tardifs et empreintes source/binaire/config/PDB/DLL contrôlés. L'archive rouge précédente est conservée.
- **CPU normal484/484** nouvellement exécuté ;71GL normaux historiques réutilisés par identité, pas de faux nouveau normal555.
- Revue correctness-reviewer nativefresh **pool-3** réellement reçue, sans défaut bloquant démontré au delta. Le refus du worktree a été résolu par un nouveau paquet local autorisé, sans retenter le chemin refusé ni ouvrir les droits. Revue persistée/acceptée, tâche de revue complétée après intégration.
- Diagnostic QA Windows/Linux livré **par le lead** en analyse locale, rapport13 intégré au727938e ; tâche de recherche complétée après ce vrai livrable, pas sur le failed de pool-2. Aucun correctif concurrent profiles/validateurs ni test Node/Linux lancé.
- Photographie **provisoire**, pas clôture : quatre notes docs et manifeste actualisés au582b156. Statuts ADR et dettes explicites conservés.

Preuves : [RED/GREEN11](11-correction-test-red-green.md), [campagne/revue12](12-h4-asan-pass-et-revue.md), [diagnostic13](13-diagnostic-qa-windows-linux.md), `docs/_meta/session-2026-10-25-qa-continuation.json`.

## 2. Documentation et préservation vérifiées

Commandes réellement exécutées : `node .opencode/scripts/update-wiki.mjs` → exit0, **1collection artist-wiki,4notes mises à jour,52inchangées,0suppression** ; `qmd embed -c artist-wiki` → exit0,60chunks/4notes, modèle existant. Logs out/logs/h4-wiki-{update,embed}-20261025-01.log. Query QMD82bf52d puis get current-state confirment date25octobre, compte ASan et mention mission non close. Aucun index d'une autre collection créé/modifié par ces commandes ciblées.

Contrôles doc : JSON manifeste valide et valeurs confrontées à l'archive ; frontmatter/summary/date préservés, summaries<=200caractères ; `git diff --check` ciblé exit0. Pas de prétention de lint complet du vault ni de rendu Obsidian automatisé.

HEAD documentaire vérifié **582b156cbde1390060a749ba215ce4d21835768e** ; pas de delta C++ non commité ; aucun changement `ARTIST/include` depuis318bf7a. Index concurrent `.grepai/config.example.yaml` intact. Hash du diff des profils/validateurs/tests/CI/AGENTS concurrents encore **89a24c5a69ecad0f67b193989d083838ecf759cb8c255a46046a23d779b64182**, identique au début. Aucun worktree supprimé ou push.

## 3. Dernier gate : audit final test-reviewer

Il reste **un audit différent** de la revue correctness reçue : capacité des tests à détecter les bugs plausibles, lecture du candidat/preuves, profil test-reviewer fresh et restrictif, sans justifications auteurs. Le contrat approuvé exige en plus son admission **hors board d'implémentation**, worktree:false et transport vérifié.

Vérification de fraîcheur du runtime : configuration globale référence maintenant **20261025-pm-review-0f9902409237** ; log natif loading plugin observé17:13:19.473Z. Ce constat de chargement ne prouve pas le hot reload des profils de chaque session.

Dans cette version, routeMissionTask13844–13848/previewMissionTask13887–13895 produit une route d'exécution V2 même pour standalone ; executeTeamSpawn14896–14897 impose toujours **claim_task et restitution collaborative**, et refuse final_response pour un worker autonome. **La voie stricte hors board n'est donc pas établie** ; une nouvelle tentative identique n'apporterait pas d'information. Aucun spawn test-reviewer, droit supplémentaire ou fallback build n'est lancé pour la contourner.

La campagne ASan est réellement terminée et verte ; sa tâche reste retenue côté board pour ne pas déclencher l'admission automatique du consommateur AUDIT sur une configuration incompatible. Ne pas confondre cet état de coordination avec un test encore en cours, ni compléter une tâche fictive pour bloquer le dispatcher. CLOTURE n'est pas déclarée achevée.

## 4. Options, compromis et décision

**A — Conserver le contrat strict actuel** : audit et clôture suspendus jusqu'à disponibilité d'une voie native hors board conforme. Aucune réparation du runtime ni migration d'équipe implicitement engagée. Preuves acquises réutilisables, aucune campagne à refaire pour attendre.

**B — Réviser explicitement le contrat d'audit** : autoriser un test-reviewer natif sur une **tâche dédiée avec claim**, contexte fresh, profil restrictif vérifié et paquet de lecture autorisé, sans auteur/rapport de justification ; tolérer un worktree isolé en lecture seule si le runtime le crée. Cette option retire l'exigence littérale hors board/worktree:false ; elle ne donne pas de droits shell/édition/tests et ne promet pas de sandbox ni d'impossibilité technique d'accéder au board. Il faut vérifier l'admission réelle avant travail et conserver un refus fermé si le profil/routage est perdu.

Recommandation pratique si l'utilisateur souhaite finir avec le runtime actuel : **B**, à condition de cette révision explicite et de l'acceptation de sa limite de séparation du board. Modèles/providers et niveau QA proportionate inchangés. La révision serait persistée dans verification.acceptance puis présentée par **config** ; seul le **start natif de l'utilisateur** approuverait cette nouvelle version avant admission. Aucun override de modèle ou de profil.

**Décision demandée : conserver la suspension stricte (A), ou autoriser la préparation de cette révision bornée de l'audit (B) ?** Aucun des deux choix ne ferme F8/C2/B1/GCC ni ne qualifie Linux/Release/image/performance.
