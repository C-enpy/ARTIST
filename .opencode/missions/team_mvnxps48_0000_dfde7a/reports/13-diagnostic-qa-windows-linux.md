# 13 — Diagnostic critique QA Windows/Linux, sans correction concurrente

2026-10-25. Auteur : lead en lecture/analyse locale, **pas une revue indépendante**. Le livrable pool-2 restait un blocage ; cette analyse ne lui est pas attribuée et ne transforme pas son admission en succès. HEAD documentaire5098653, candidat C++82bf52d ; diffs concurrents profils/validateurs/tests non committés et non possédés, aucun modifié ici.

## Méthode et portée

Lecture ciblée des trois profils QA, validate-agentic.mjs/qaRuleMismatch/QA_DECLARED_RULES/QA_PLANNER_RULES, tests t04/t05/t13, CI Ubuntu, presets et CheckCxx23. Comparaison des triplets réellement présents en frontmatter avec les deux constantes de l'oracle, puis appel en mémoire de la **fonction pure qaRuleMismatch extraite du script courant**. Aucune suite Node/harness ou campagne Linux exécutée, aucun parseur de sécurité remplacé : l'extraction ponctuelle porte sur les scalaires des trois fichiers lus.

L'archive C++555 n'est pas une preuve harness. Déclaration, disponibilité catalogue, appel permis, permissions résolues, profil chargé et comportement exécuté sont des faits distincts. Les réussites de lecture/transport de pool-3 ne prouvent pas celles d'un test-reviewer futur.

## C1 — REVISE : oracle incohérent avec la politique adoptée

**Défaut de cohérence statique établi, pas simple suggestion.** Les trois profils ont les mêmes31triplets, dont execute/team_message/grep autorisés et edit/shell/subagent refusés. Le validateur applique pourtant QA_PLANNER_RULES au seul planner, QA_DECLARED_RULES aux deux autres (validate-agentic.mjs452). QA_DECLARED_RULES58 refuse execute ; QA_PLANNER_RULES64 le permet et ajoute les canaux/contexte.

Évaluation réellement effectuée sur les données courantes : planner → mismatch null ; test-engineer et test-reviewer → **règle25 : execute attendu deny, observé allow**. Comparés tous trois à la politique communicationv2, mismatch null. Les nombres27/31 expliquent l'histoire des listes, **ne constituent pas l'invariant de sécurité**.

Scénario discriminant avant future réparation : un profil test-reviewer nominal selon le contrat adopté doit être reconnu statiquement ; ses mutations shell/edit/subagent ou contexte trop large doivent rester rejetées. Propriétaire futur : build/harness avec ownership explicite sur validateur et oracles. RED à constater par un test ciblé du validateur courant, pas en recopiant les assertions ni en raccourcissant le profil. Aucun patch de ces fichiers concurrents autorisé par ce diagnostic.

## C2 — REVISE : t04/t05 consolident encore la séparation obsolète

- t04-harness/contract.test.mjs493–494 attend communicationv2/version2 pour planner seulement ;516–523 construit les deux autres profils avec l'oracle historique ;503 vérifie encore27.
- t05-qa/contract.test.mjs12–20 réserve plannerOracle au seul planner. Les tests synthétiques peuvent donc continuer à valider leur propre ancien oracle, indépendamment de la conformité des profils réels.
- t13-integration/contract.test.mjs15–16/64/71–72 distingue correctement oracle statique, nativeStatus NOT_RUN et fresh-reviewer-off-board ; **KEEP** cette distinction et les pins historiques d'oracle, ne pas les réécrire vers le HEAD moteur par principe.

Détection proposée : fixture nominale communicationv2 pour chaque rôle + même matrice négative de permissions ordonnées ; vérifier les fichiers réels, pas seulement des spécimens produits depuis le même oracle. Coût faible de ciblage Node existant ; aucun nouveau framework. Les suites n'ont pas été exécutées ici, donc pas de nombre de tests inventé ni de PASS/FAIL process annoncé.

## C3 — KEEP / REVISE : frontières de permissions et données nécessaires

KEEP deny-all initial, refus secrets/env, shell/edit/subagent fermés ; conserver lecture/glob/skill ciblé, execute et transport nécessaires selon la politique adoptée. **execute n'est pas une sandbox** : Code Mode peut exposer fetch/utilitaires sans garde fine. Un template reconnu ne démontre pas enforcement ni absence de fuite ; les oracles doivent conserver ces limites.

REVISE les briefs et l'admission, pas les droits par défaut : les exceptions externes portent une racine ARTIST et un **ancien pool-5** littéral. Aucun lien ne prouve qu'elles couvrent tous les nouveaux worktrees. Le refus réel pool-3 sur son worktree est cohérent avec cette limite ; il a été résolu en lui fournissant un snapshot autorisé dans le canonique et des extraits, sans retenter le chemin refusé ni ouvrir external_directory:*.

Scénario nominal/négatif : reviewer lit un paquet explicitement fourni et restitue ; worktree non autorisé reste refusé ; aucun shell/bootstrap externe demandé au profil. Coût faible de fourniture par lead, preuve runtime à renouveler pour le **profil réellement admis**, pas par simple fichier. DEFER toute conclusion de hot reload ou permission résolue depuis un comptage/lint.

## C4 — REVISE : admission native distincte du texte du discovery

Constat réel de cette continuation : H3/QA-PORTABILITE auto-affectés build/worktree isolé alors que le discovery recommandait coordinateur/local ou reviewer/fresh/RO. Les champs agent/context/worktree écrits en prose ne configurent pas le runtime. Le schéma team_tasks_add courant porte agent et depends_on, pas context/worktree ; team_spawn les expose, mais une admission auto déjà effectuée empêche le claim explicite.

Sur la tâche supplémentaire, agent **correctness-reviewer explicite** a effectivement routé pool-3 vers ce profil. Le snapshot source commité et les entrées locales ont permis sa revue ; pas un test-reviewer hors board et pas une preuve générale de tous les champs.

Scénario discriminant de future réparation native : importer une tâche avec affinité/contexte/isolation/restitution, inspecter l'admission effective avant travail, vérifier refus fermé si ces obligations ne sont pas représentables. Propriétaire runtime/importeur à décider séparément ; pas une modification d'ARTIST C++ ni des scores/providers. Coût/risque supérieur à un lint, car la preuve doit être native. **Gate final audit reste BLOCKED** si claim/restitution/séparation hors board ne sont pas compatibles. Le code natif observé impose claim_task et refuse final_response en exécution autonome ; ne pas fabriquer une tâche terminée ni substituer build.

## C5 — KEEP : socle Windows/Linux de preuves non ambiguës

Socle minimal existant à conserver : identité candidat/config/toolchain ; découverte non vide ; sélection nommée avant run ; noms exécutés et comptes/failures/errors/skips ; XML et logs complets ; contrôles tardifs ; verdict limité au niveau effectivement exécuté. Séparer les preuves CPU, mocks et OpenGL réel, et distinguer compilation de rendu/image/performance.

Windows actuel : preuve normale CPU484 et ASan555 au candidat82bf52d ; GL normal71 antérieur réutilisé par identité, pas nouvellement exécuté. ASan canari0/86+diagnostic et environnement canonique ; /RTC1/MDd gardés selon contrat MSVC. Pas d'extrapolation des flags à GCC/Clang.

Linux : CI existante ubuntu-24.04, toolchain GCC14 et dependencies installées dans son propre job, CheckCxx23 compile/link, suites unit/opengl séparées et no-tests=error ; GL sous Xvfb, parallèle1. Cela décrit **un chemin**, pas une exécution observée ici. Les installations CI ne sont pas une autorisation d'installation locale. La version GCC ne prouve pas la disponibilité de std::stacktrace ; formats/GL/ASan demandent leurs préconditions propres. Aucun WSL/Linux/env nouveau créé.

REVISE seulement dans une tranche CI autorisée : rendre explicites archives JUnit/logs et découverte avant exécution sur Linux si elles deviennent critères d'acceptation. DEFER campagne Linux, GCC natif/libstdc++, ASan Linux, coverage et Valgrind supplémentaires dans cette continuation. Coût conditionnel à un environnement existant et à un oracle demandé ; pas besoin de tout exécuter pour chaque petit diff Windows.

## C6 — KEEP / DEFER : notifications et preuve de résultat

KEEP persistance du résultat natif effectivement reçu, provenance auteur/runner et fragments courts pour le transport. Les notifications vides et messages du lead réaffichés ne sont pas des livrables workers. Une fin d'auteur ne remplace pas une intégration.

La répétition d'anciennes erreurs de préparation et des notifications sans nouveau contenu a été observée (rapports04/05) ; résolution du catalogue au02 distincte. DEFER tout correctif natif de déduplication/reprise : non possédé dans ce lot, aucun patch de base interne ni faux completed pour le stopper.

## Risques principaux et sortie proportionnée

QA I×P : oracle refusant la bonne politique4×4=16 ; admission erronée/profil générique5×4=20 ; preuve transférée à une autre plateforme4×3=12. Pour chacun, déclencheur/blast radius/détection/mitigation/gap sont explicités ci-dessus. Ne pas ajuster scores, providers ou grille pour satisfaire ces risques.

Ordre utile futur : figer l'oracle documentaire adopté → tests négatifs du validateur → correction ciblée sous ownership → suite Node pertinente → admission/lecture/transport natifs réels. Pas de nouveau dispatcher, service, catalogue GPU, framework de tests ou pipeline QA fixe. Ce diagnostic est un livrable de recherche, **pas un RED de correction harness ni une QA finale indépendante**.

Sortie actuelle : correctif C++ vérifié/revu, diagnostic de cohérence livré localement ; oracles harness à corriger seulement dans une tranche séparée, audit final encore bloqué. F8/C2/B1/GCC et ADR inchangés. Aucun fichier concurrent modifié, aucune campagne Linux ou Node lancée.
