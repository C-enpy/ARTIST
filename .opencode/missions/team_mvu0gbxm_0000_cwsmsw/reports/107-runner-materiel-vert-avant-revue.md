# 107 — Correction d'environnement verte, revue avant intégration

Le coordinateur a relu les dix lignes ajoutées au runner : le contrôle ne
s'active que sur déclaration matérielle explicite, refuse le transport incomplet
ou le forçage réintroduit, puis nettoie la copie enfant. La capture, les modèles
et les adaptateurs ne changent pas ; tous les tests sont inchangés depuis le RED.

Résultats : **10/10 tests ciblés, 1/1 transport, 9/9 capture**, puis cœur
**201 succès et trois SKIP sur204**, contexte **28/28**, scénarios **31/31**.
Aucun échec ou erreur ; empreintes stables. Les trois SKIP concernent les liens
symboliques indisponibles sous Windows, sans contournement. Le test de transport
singulier est recoupé dans son journal, même si le filtre de résumé ne reprenait
que le pluriel ; aucune relance nécessaire.

Une revue indépendante fresh est publiée avant intégration. Le candidat reste
stable et non intégré. Les adaptateurs attendent encore cette entrée commune,
et Windows doit aussi compléter ses déclarations matérielles par invocation.

## Preuves

- Candidat `ed952b45ad36b6edde91df5927807112fedf3422`, auteur pool-26 ; runner coordinateur.
- Commandes : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p <filtre> -v`, filtres nouveau contrat hardware, transport, capture, puis `test_*.py` ; même découverte des suites contexte et scénarios, depuis worktree auteur.
- Manifestes, commandes exactes et empreintes : `out/verification-synthetic/hardware-child-environment-green-2026-10-30T07-15-47-422Z/`.
- Revue `task_mvun2n2m_005p_r6z75z`, fresh, lecture seule ; PM/LLM58/65.

Les nouveaux tests emploient un parent synthétique et un faux processus en mémoire.
La non-régression réutilise les enfants Python factices finis déjà admis. Aucun
outil natif, moteur, GL, rendu, benchmark, installation ou cloud exécuté ; aucune
qualification matérielle déduite de cet assainissement.
