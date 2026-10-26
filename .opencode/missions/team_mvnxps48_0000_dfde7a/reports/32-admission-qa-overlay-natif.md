# 32 — Admission QA : overlay natif plus large que le profil

2026-10-25. Q task_mvo66wuw_0016_9y7cqx publiée après intégration des producteurs fa7a949. Auto-dispatch a affecté pool-6/qa-planner ; le spawn explicite fresh/worktree:false a perdu la course, aucun doublon créé. Aucun auteur de probe/Reader admis.

## Contrôle observé, pas simple déclaration

team_view : session ses_e8ba57542ffea1tdYPD3SqBw3x, agent qa-planner. GET session natif : parent lead attendu, location canoniqueARTIST ; contexte commence par seule affectationpool-6, puis lectureAGENTS/skill/entréeQA et communications. Profil canonique relu : denyall, read/glob/grep, skillartist-build-validation, execute/team_message et contextesARTIST/pool-5 ; edit/shell/subagent et autres MCP fermés.

Mais GET session expose permissions et metadata ensemble.permissions identiques à un jeu générique de planification : webfetch/websearch/question, teamboard/claim/handoff et GrepAI/Serena/ContextMode autorisés. **Admission de fond bloquée**, aucun GO analyse. Ces listes ne prouvent pas à elles seules la composition ultime ni une sandbox ; elles ne permettent pas d'attester le profil restrictif attendu.

Le code installé explique la provenance : planningPermissions14603–14632 construit précisément ce jeu générique ; spawn14987–14993 ajoute cet overlay en modeautonomous et continuity.switchAgent12613–12629 remplace les overrides Ensemble-owned. Ce constat ne vient pas du seul nombre de règles. Aucun fichier runtime/profil n'a été modifié.

## Contrat de correction native bornée envisagé

La voie publique documentée PATCH /api/session/{sessionID} (session.update) accepte permissions et metadata ; cette découverte de schéma n'est PAS une preuve d'application. Le lead peut vérifier que toutes les règles de session sont Ensemble-owned, conserver toutes les autres metadata et **restreindre seulement cette session** aux règles du profil canonique inchangé, sans grant, modèle, phase ou réglage global. Après patch, relire état natif puis vérifier lecture/restitution dans la même session avant analyse. Échec ou divergence = BLOCKED, aucun élargissement/substitution/build.

Ceci est une correction de configuration d'admission sous la politique restrictive déjà exigée, pas un correctif exécutable du plugin ni un test de refus volontaire. Aucun champ/APIgrant supposé. Le profil QA n'effectue lui-même ni appelAPI ni changement de droits. Execute reste une surface non sandboxée, même si les règles restreintes sont restituées. Le runtime pourrait réappliquer son overlay à une transition ultérieure : aucune autorisation de handoff/claim/dispatch dans cette tâche ; contrôle à chaque admission future.

Pendant ce contrôle, les décisions opérateur B3/C2 du31 restent ouvertes ; aucun auteur ni fermeture technique déclenchés.

## Restriction native effectivement appliquée

Le lead a vérifié que permissions==ensemble.permissions, sans règle opérateur non possédée à retirer. Profil canonique parsé sans édition ; PATCH public session.update de **cette seule session** avec permissions et metadata conservées sauf remplacement du jeu Ensemble-owned par le profil canonique. GET après update : agentqa-planner inchangé, permissions ET ensemble.permissions restituent exactement les31règles canoniques, toutes autres metadata conservées. Aucun modèle/phase/configglobal modifié ni droit ajouté hors profil.

Ce résultat prouve la restitution native d'une restriction et non un hotreload depuis disque ou une sandbox. Nouvelle lecture canonique et restitution après restriction demandées dans la même session avant GO de fond. Un nouveau refus restera bloquant ; aucun droit élargi ni nouveau worker. Le défaut d'overlay générique du plugin reste un diagnostic distinct d'une correction exécutable de ce plugin.
