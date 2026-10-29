# 18 — H_Q intégré ; première vague bornée et routée explicitement

Date : 2026-10-24. Étape : prérequis QA livré, préparation TDD des auteurs. Aucune preuve moteur nouvelle.

## Livrable QA réellement intégré

Les cinq parties et `FIN_H_Q` de pool-5 ont été reçus. Le lead a contrôlé obligations explicites/inférées, risques I×P, scénarios nominaux/négatifs, oracles, registre bidirectionnel/orphelins, budget et sortie, puis persisté le plan en [17-plan-qa.md](17-plan-qa.md).

Normalisations signalées : chemin de cette mission au lieu du plan historique mission-gpu-qa ; bloc transport antérieur conservé comme historique et dépassé par l'essai reçu ; tableaux reformulés en listes ; preuve B6 non réalisable ne peut pas devenir RED par simple argument statique. Aucun oracle affaibli. Budget24h reste hypothèse, pas nouveau budget de dépense ratifié.

**Commit H_Q : `64d97dc3004d3aad2f69492e832da052b9d4d70a`**, un seul fichier/246 lignes. Contrôles documentaires et `git diff --cached --check` propres, puis `git commit --only` et `git show --format=oneline --stat HEAD -- <plan>` exécutés. Pas de test, build ou modification moteur. H_A reste au commit354b53f ; protocole B2 au4f8385e.

La reprise Q `task_mvmc5s53_000v_ct63sq` a été complétée par le lead **après** réception, persistance et commit. Pool-5 informé de l'intégration, pas d'autre tâche/collecte autorisée. La barrière H_Q est satisfaite comme livrable ; elle ne prouve ni moteur vert ni environnement prêt.

## Publication des seules branches prêtes

Les anciennes fiches auteur importées ne portaient pas d'affinité machine vérifiable et restent derrière leur dépendance historique, pour ne pas relancer le routage générique observé. Des fiches d'exécution explicites reprennent exactement leurs périmètres et références, avec vrais producteurs intégrés et sans redéfinir les tâches métier. Aucun consommateur H1/helper/revue/final n'est libéré avant intégration des sorties réelles.

- **B1** `task_mvmhqczu_0009_sb99l2` → cpp-engineer, PM/LLM73→73/R55 ; dépend de la reprise Q. LoaderTests pass/shader seuls, mock partagé sous inventaire/accord, Reader/TestUtils exclus.
- **B3** `task_mvmhqczv_000a_iszkxc` → cpp-engineer,78→78/R60 ; dépend Q+contrat F8. Reader.hpp/ReaderTests seuls ; calibration native lecture acceptée, close réel DEFER. Aucune nouvelle seam/API.
- **B4-U** `task_mvmhqczv_000b_5jc7pv` → gpu-engineer,49→70/R45 ; dépend Q. ProgramTests seul ; garde existante, ordre cleanup/F10 conservé, pas de helper partagé modifié.
- **B6** `task_mvmhqczw_000c_fm4fmx` → cpp-engineer,48→70/R50 ; dépend Q. TraceableException.hpp/tests seuls ; dispositif RED causal à proposer avant correction, pas de GCC/macro forcée/installation implicite.

Contexte fresh explicite dans les contrats et sur le spawn tenté ; worktree auteur isolé, base64d97dc et présence effective H_Q/H_A à vérifier avant écriture. Diff canonique gitignore/GrepAI et corrections de profil QA non commises ne sont pas transférés ni possédés par ces auteurs. Statuts ADR transmis :0003/0005 Accepted partial,0008 Windows Debug seulement,0004 exclu.

## Affectations observées, pas supposées

Un `team_status` de contrôle d'admission confirme B1 sur **pool-8 / cpp-engineer**, worktree créé après H_Q. Trois nouvelles tâches étaient encore pending à cet instant. Un spawn explicite B3 avec cpp-engineer/fresh/worktree et sans modèle a ensuite été refusé car sa tâche était déjà **in_progress** : aucun doublon créé et aucun choix de modèle contourné. La propriété native B3 reste à recouper au prochain retour.

B4-U/B6 sont publiées prêtes, pas annoncées exécutées. Le dispatcher reste responsable des admissions selon la politique v2 ; pas de répétition de spawn pour concurrencer son attribution. Les vrais IDs permettent de relier les futures intégrations aux fiches historiques sans attribuer un succès à un auteur antérieur en échec.

## Cycle initial et ressources

Premier mandat de chaque auteur : vérifier HEAD/contrats/accès, inspecter impact et proposer la preuve discriminante, éventuellement un delta de test seulement. **Aucune correction de production avant preuve RED causale du runner lead.** Refactoring pur sur caractérisation préalable. Deux essais sans information nouvelle imposent une nouvelle hypothèse/expérience.

Tests/builds/sondes exécutés par le lead sous mandat runner distinct, pas par les profils QA ni par ces auteurs dans cette tranche. Baux : un build par sortie et un job, fixtures possédées, GL exclusif interprocessus. Les analyses/tests disjoints peuvent progresser en parallèle ; les exécutions partageant environnement/GPU sont sérialisées. Environnement du jour non encore qualifié, donc aucune panne d'outillage ne sera comptée RED.

## Limites et suite

H_Q est livré, transport pool-5 observé sur accusé et cinq messages ; aucun besoin de redemander K/H_A/QA. Les autres profils restrictifs ne sont pas réputés débloqués par cette seule preuve. C2 reste OPEN/INCONCLUSIVE ; fermeture réelle DEFER. Pas de GREEN courant, campagne H3 NOT_RUN.

Prochain contrôle : premiers retours d'accès/plan auteur et préconditions runner, puis RED minimal. Toute divergence de racine/worktree ou perte d'affinité bloque l'auteur avant mutation, sans correction aveugle de permissions ni nouvelle admission générique.
