# QA Linux — helper intégré, correction Attribute admise

## Progression vérifiée

Le workflow et son helper commun WSL/CI sont intégrés. Le lead a rejoué leurs **6 tests sous Windows et 6 sous Linux**, tous réussis, en vérifiant l'identité des sources finales. Cela valide le reporting ciblé, pas la parité graphique : les trois échecs Attribute restent ouverts.

La tranche de correction a maintenant été admise au profil OpenGL, après contrat et plan QA commités, puis confirmation explicite de la libération des compilations et contextes GPU par le producteur précédent. Le nouveau propriétaire dispose de ses arbres privés et doit commencer par le RED de classification, pas par une modification de production.

## Intégration du workflow et des preuves

Livrable auteur final : quatre fichiers, aucun C++/CMake/preset/Conan/wrapper Windows modifié. Le lead a relu le diff et le test du contrôleur qui vérifie réellement manifestFAIL,75cas graphiques/3failures et propagation du code8. Le run WSL corrigé existant est recoupé ; son hash du helper correspond au fichier intégré.

Avant shutdown/merge, les preuves ont été copiées :110fichiers,13416897octets. Trois fichiers temporaires possédés par la mission ont été retirés du worktree ; la source de sonde était déjà archivée à l'identique, hash vérifié. Aucun travail préexistant ou inconnu supprimé.

Le premier merge natif a refusé des notes de mission locales modifiées. Le lead a commité uniquement ces deux notes, sans stash ou inclusion des travaux étrangers, puis le merge natif a réussi. Le livrable intégré a été relu, testé et commité séparément. Aucun git merge manuel ou suppression forcée de worktree.

La tâche de raccordement est clôturée pour son livrable propre : mêmes commandes, résultats locaux/distants réels et échecs conservés. **Cette clôture n'annonce pas un PASS de parité.** Le défaut de classification est suivi par sa tranche distincte encore en cours.

## Correction admise et parallélisme

Le runtime a routé un nouvel auteur gpu-engineer, contextefresh, worktree isolé. Il a reçu le contrat et le plan déjà commités avant création du worktree, les données de sonde, les invariants et fichiers exclusifs, les commandes possibles, les résultats antérieurs et les preuvesRED/GREEN requises.

Les baux Linux build/GPU et Windows build/GPU lui sont accordés dans ses seuls arbres privés ; un job par build et aucune sortie canonique partagée. Le lead a seulement exécuté les tests Python du helper, sans build moteur ni contexteGPU, pendant cette admission.

Une copie locale complète de la politique Context Mode est disponible dans son worktree pour éviter la lecture externe refusée lors des phases précédentes. Sa lecture et celle du contrat doivent être effectivement confirmées, pas déduites de la copie. Aucune réparation de MCP, modification de permissions ou politique de modèles.

L'auteur peut conserver sa base de RED pendant l'intégration indépendante du helper : le C++ de départ est identique. Le lead combinera les branches avant validation finale ; pas de rebase pendant une expérience sans contrôle du candidat.

## Résultats et limites restantes

- WSL et CI au candidat initial :506CPU réussis et72/75GL, normal/ASan, mêmes trois échecs, aucun skip ; voir rapport19.
- Le helper final a été exercé sous WSL pour son verdictFAIL et par tests unitaires sur les deux OS. Sa correction finale n'a pas encore été rejouée en CI distante ; ce contrôle attend le candidat combiné corrigé, pas un run aveugle identique.
- Le nouveau RED moteur, le correctif et les non-régressions ne sont pas encore livrés.
- Gaps, revue indépendante et audit final restent différés jusqu'à l'intégration réelle de la correction. Débloquer une dépendance du board ne les publie pas.
- Délégation actualisée persistée dans resume.md et transmise au nouveau propriétaire ; aucun GO intermédiaire, changement de modèles/QA ou élargissement de droits.

## Annexe technique

- Auteur workflow : commits84bacef puis `f8417d73bc5b9a7fa3cdeeab4f5e2116accc643a`, tâche `task_mvsa43rc_0007_cq2xsm` clôturée après intégration.
- Intégration lead : `44943016b8b732d843e58b8137044890c03b3ba8`, seulement `.github/workflows/cmake-single-platform.yml`, `scripts/linux-evidence.py`, `scripts/tests/test_linux_evidence.py`, `research/sessions/2026-10-28-linux-qa/opengl-ci.md`.
- Commandes runnerlead : Python3.13Windows et Python3.12Linux, `-B scripts/tests/test_linux_evidence.py -v`,6/6 chacun, code0. Fixtures privées, pas build/GL réel. Logs `out/logs/mission-linux-qa/helper-integrated-{windows,linux}.txt`.
- Hash helper `5a0fbdda5697dcbd79b1c6137c77a21f0b3c5436008ce8fc6aa80e3dfd03170c`, tests `b9a3f8e8680d1ae31e9cfe0f2dec1bcc9e7517f30c251269e5630d1bd985e04a`, identiques dans la copie Linux utilisée.
- Run corrigé recoupé : `20261028T180529Z-749ee7459ea74ad39cb91a11a932127e`, manifestFAIL,506CPU et75GL/3failure,code8. Archives complètes `out/logs/mission-linux-qa/opengl-ci-author/`.
- Correction : `task_mvsggywg_0028_23u2s9`, pool-9/gpu-engineer, complexité84/risque76 PM/LLM, publication après producteursarchitecte/QA intégrés et transfert de bail effectif. Aucun override de modèle.
