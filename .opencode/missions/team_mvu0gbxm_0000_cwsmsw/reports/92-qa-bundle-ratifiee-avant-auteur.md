# 92 — Complément QA ratifié avant la tranche bundle

Le coordinateur a récupéré la réponse finale publique complète du planner, et non seulement sa notification rectificative, puis l'a relue et intégrée au plan QA, section11. L'absence de `qa-inputs.md` est corrigée comme erreur de brief ; le planner confirme qu'aucun oracle de ce delta n'en dépend. Aucun droit ou périmètre GPU n'est élargi.

Le plan retient B1–B6 : bundle explicite et descriptions pures, transport sink exclusif fermé/empreinté avant commande, réemploi build sans réparation, opt-in/phases bornés, bruts conservés et schémas publics inchangés. Les risques20/15/12 portent sur attribution, collisions/effets et publication interrompue. T1–T7 restent GAP jusqu'aux preuves du nouveau delta, avec registre bidirectionnel, setup25 puis75%planifié/25%buffer et critères de sortie. Les preuves historiques sont rattachées aux manifestes existants, sans nouvelle campagne.

Après ce commit seulement, l'auteur core peut recevoir le brief de tests RED. Le worktree historique déjà créé automatiquement reste propre et sans écriture source ; son point Git devra être explicitement aligné sur le plan commité avant la reprise. Aucun adaptateur Windows/Linux, script moteur ou workflow n'est libéré par cette décision.

Exécution de cette barrière : plan commité `222a34164ec91fde54dfdafc288004ea21c99f75`. Le lead a constaté l'arbre auteur propre à `8ddb85e`, vérifié son ascendance et zéro commit unique, puis l'a avancé avec `git reset --keep` vers ce commit, sans suppression ni abandon de travail. Il s'agit du worktree historique possédé, pas d'un nouveau worktree d'implémentation créé avant le plan. HEAD et propreté sont contrôlés après l'alignement. Un brief explicite de tests RED seulement a ensuite été envoyé à pool-20 ; production, anciens tests et adaptateurs restent interdits à l'écriture initiale.

La tranche conserve l'évaluation et la politique de la tâche core déjà admise (76/67), sans modèle explicite ni score modifié pour le routage. Périmètre local de développement borné, plancher70 respecté ; la QA est une analyse distincte. Les seules adaptations initiales d'anciens tests seront des signatures de doubles, avec mapping annoncé, assertions métier conservées. Le coordinateur reste runner.

## Traçabilité

- Planner pool-21, tâche `task_mvujeqpu_004m_3sfeit`, profil natif restrictif confirmé, première lecture et restitution réussies.
- Réponse complète publique `msg_18b4ddef4001bn4XbXogv09VOP`, session `ses_e74b2e63bffe2V2PnsBSflHrRv`, 11656caractères ; rectification `result_mvujhmp8_004p_513f22`.
- Récupération par CLI/API native de messages publics : seules parties `text` retenues, aucun raisonnement privé lu ou persisté.
- Plan ratifié : `.opencode/evidence/mission-uniform-pipeline/qa-plan.md`, section11 ; binding inchangé `windows-adapter-binding-v1.md`.

Aucun test supplémentaire, moteur, compilation, CTest réel, OpenGL, rendu, benchmark, installation ou cloud exécuté à cette étape. Le pipeline reste en cours, pas qualifié nativement.
