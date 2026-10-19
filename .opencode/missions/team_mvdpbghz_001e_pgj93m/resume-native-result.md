# Reprise après mise à jour Ensemble — preuve bornée

Politique approuvée v1 inchangée, hash
0c97a464a620c155e2bb9cdcbadad4fd232104fb5b970f5d66a74bd882ae8a13.
GO de reprise explicite utilisateur. Mission existante conservée, pas de team_create,
override modèle, modification globale ou élargissement de permissions QA.

L'ancien Q était idle avec board in_progress. Le lead a demandé uniquement une
restitution finale textuelle du plan existant, sans outil ni nouvelle enquête.
Le premier team_message a indiqué que le membre avait déjà terminé et n'était
pas réveillé ; le second, force:true, l'a réactivé pour cette restitution bornée.

Le transport natif a livré :
- candidat result_mvdvnqvf_0006_enochl ;
- session ses_eb1423176ffeiluzzxpHDO6TeQ ;
- message msg_14f41a3ec00113yIjxnA1SRnqD ;
- producteur Q task_mvdqhtoe_001j_edgftw, pool-2.

Le résultat résume les obligations, risques, budget et DH1–DH5 du plan déjà
produit. Relu et accepté par le lead via team_tasks_complete avec candidate_id.
Board Q clôturé ; pool-2 ensuite arrêté via team_shutdown. Aucune branche auteur
à intégrer : Q travaillait sans worktree et sans écriture.

Le lead a persisté la synthèse attribuée et le contrat, puis exécuté git diff
--cached --check (succès) et le commit ciblé 4a6276a. Seulement deux fichiers :
attribute-contract.md et qa-plan.md, 324 lignes ajoutées. Les changements externes
des profils/harness, AGENTS.md, docs et .grepai/config.yaml sont préservés hors commit.

Ce résultat prouve une restitution native et une acceptation lead sur cet essai.
Il ne prouve ni correction universelle du plugin ni enforcement des permissions,
et n'est pas un test CPU/GPU du moteur. Aucun test moteur exécuté.

La barrière de commit QA est satisfaite, mais les arbitrages DH2–DH4 restent
explicitement ouverts : tableau natif potentiellement impossible en GLSL330 ;
refus INVALID_LOCATION/DUPLICATE_NAME/troncation mockés ; plage d'entiers au-delà
des valeurs float32 exactes non consommée par shader dans ce lot. Avant auteur,
obtenir décision humaine sur ces limites et conserver la précondition DH1 :
sondes légalité par runner lead, divergence => arrêt. Aucune tâche D publiée.

Décision humaine recueillie ensuite : « Renforcer avant l'auteur ». DH2–DH4
non acceptés. Révision de contrat d'oracles et Q amendé nécessaires avant D ;
ne pas confondre fin de Q avec acceptation de ses limites recommandées.
