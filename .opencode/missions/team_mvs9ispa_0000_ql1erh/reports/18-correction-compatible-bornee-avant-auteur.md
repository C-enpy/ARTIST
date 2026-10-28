# QA Linux — correction compatible bornée, plan QA à compléter avant auteur

## Décision et conséquence

L'architecte recommande une correction locale de la classification : utiliser la réflexion moderne lorsqu'elle est réellement disponible, conserver les noms publics natifs de l'ancienne requête et garder le chemin historique sinon. Le lead adopte ce périmètre compatible, dans le mandat de correction Linux ; **aucune implémentation n'est encore admise**.

Cela évite d'ajouter des alias publics, un parseur de shader ou une nouvelle source cook de métadonnées. La sonde a écarté la solution fondée sur la seule localisation `[0]`. Le correctif devra maintenant démontrer la classification et le refus moteur, pas seulement accepter une autre orthographe dans les assertions.

## Garanties et limites conservées

- Scalaire admis ; tableaux1/2 refusés avant modification nativeGL du buffer/binding/leurre.
- Clés natives exactes, API et ownership inchangés ; réponses modernes incohérentes rejetées avant publication de la nouvelle map.
- Version/extension effectives et fonctions chargées vérifiées ; aucun appel moderne lorsque sa route n'est pas disponible.
- La conservation de l'ancienne valeurCPU après rejet du wrapper n'est pas ajoutée : le wrapper la modifie déjà avant le Setter, sujet distinct.
- La correction vise les configurations Windows/NVIDIA et Linux/Mesa effectivement qualifiées, pas tous les pilotes3.3. Un autre3.3 sans extension à nom ambigu reste un gap explicite, pas un PASS global ou un skip autorisé.

Le contrat technique et les oracles sont persistés dans `.opencode/evidence/mission-linux-qa/attribute-correction-contract.md`. Les statuts ADR ne changent pas. Aucun nouveau backend, gestionnaireglobal de capacités, architecturecook ou benchmark n'est engagé.

## Prochaine barrière et parallélisme

Un complément QA doit préciser les risques de sélection de route et de publication partielle avant le worktree auteur. Le lead le persistera et commitera avant publication de la tranche OpenGL. L'auteur gardera tout le cycleRED/GREEN, avec un RED qui échoue sur isGLArray d'array1 au nom natif indépendamment observé, puis non-régressionWindows/Mesa.

Pendant cette préparation, le workflow/helper et la CI distante peuvent continuer : fichiers et ressources distincts. Le bail GPUlogiciel reste à pool-5 tant que sa campagne n'est pas rendue ; aucune compilation ou expérience concurrente dans son arbre. Les consommateurs de la future correction restent différés jusqu'à son intégration vérifiée.

## Preuves et traçabilité

- Avis finalRO reçu de pool-7, tâche `task_mvsfp3ei_001o_j80gqi`, aucun fichiermodifié ni testexécuté par le spécialiste.
- Sonde exécutée et recoupée dans rapport17, normesprimaires dans rapport14, campagnesnormale/ASan dans rapport15. Les troiséchecs restentFAIL.
- Contratadopté : cléslegacyexactes, classificationinternegated, validationindex/nom/type/taille/location et stagingconservé. Le lead vérifie le commit avant de clôturer l'avis et d'admettre le complémentQA.
- PlanificationlocalePM/LLM : développementcomplexité84/risque76 ; complémentQAROcomplexité55/risque72. Plancherdéveloppement respecté, scoresdistincts deQAI×P, politique/provider/modèlesinchangés.
