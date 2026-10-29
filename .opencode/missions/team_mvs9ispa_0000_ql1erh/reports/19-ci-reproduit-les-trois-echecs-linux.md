# QA Linux — la CI propre reproduit les trois échecs locaux

## Résultat distant vérifié

La CI GitHub est terminée sur la branche dédiée. Les deux configurations, normale et ASan, échouent au même contrôle graphique que sous WSL : **506/506 tests CPU réussis, 72/75 OpenGL, trois échecs identiques et aucun skip**.

La reproduction dans un environnement distant propre est donc acquise. **La parité reste FAIL** : ce résultat ne constitue pas une réussite globale, mais il confirme que le défaut n'est pas limité au checkout WSL. Aucun nouveau run n'est lancé pour répéter ces échecs inchangés.

## Recoupement par le lead

Le lead a lu l'état natif du run puis téléchargé ses deux archives officielles dans un répertoire de preuves neuf. Il a analysé les JUnit et le canari, pas seulement les conclusions des jobs :
- Normal :506cas CPU uniques sans échec/error/skip ;75cas graphiques uniques dont les trois Attribute déjà connus, sans error/skip.
- ASan : mêmes comptes et mêmes trois noms de failure.
- Renderer des JUnit graphiques : Mesa25.2.8, llvmpipeLLVM20.1.2, OpenGL4.5core, comme l'environnement local observé.
- Canari ASan : contrôle sain0, faute volontaire86, politique canonique et statutPASS borné. Détection de fuites désactivée explicitement ; pas de preuve d'absence générale de défaut mémoire.
- Les fichiers head.txt des deux archives indiquent bien le candidat distant annoncé.

Les manifestes distants viennent de la version initiale du helper et indiquent encoreINCONCLUSIVE. Ils restent conservés tels quels ; les JUnit et les codesCTest8 établissent leFAIL. La correction du helper n'est pas attribuée rétroactivement à ce run.

## Suite dans le mandat approuvé

Le propriétaire peut rendre workflow/helpers et leurs preuves corrigées sans modifier le C++. Le lead a demandé de ne pas relancer aveuglément la CI tant que la cause de classification reste inchangée et de signaler la libération du bail Linux/GPU avant son transfert.

Le contrat de correction compatible est déjà commité après l'avis d'architecture. Le complément QA est en cours sur ce contrat, avant tout auteur. Le routage effectif du qa-planner a été contrôlé : session fresh dans ARTIST, permissions de session vides, règles natives restrictives lues. Une lecture locale réussie et une remise effectivement reçue restent à vérifier ; ces observations ne prouvent pas une sandbox ou l'enforcement exhaustif des règles.

Mise à jour d'admission : le message de progression du qa-planner a été effectivement reçu via team_results. Il confirme la lecture locale d'AGENTS, du skill de validation, du contrat Attribute et du plan existant, sans edit/build/test ni consommation de la branche du producteur. La voie de message et la lecture sont donc constatées ; la remise finale complète reste attendue, et n'est pas remplacée par cet accusé. Aucun auteur admis ou tâche QA clôturée à ce stade.

Aucune source de production n'est modifiée à ce jalon. Pas de main poussé ou fusionné, de changement de protection ou secret, ni de runner auto-hébergé. Les tâches de correction, de gaps et de vérification finale ne sont pas déclarées réussies par ce résultat distant.

## Annexe de preuves

- Run : [Linux Debug parity — résultat distant](https://github.com/C-enpy/ARTIST/actions/runs/40741589259), conclusionfailure, branche `mission/linux-qa-parity`, candidat `84bacef5fcf3f13fad5b9b29e18eeaa70b72f911`.
- Étape échouée des deux jobs : « Build and validate the required population », commandeCTest labelopengl, code8.
- État et log natifs archivés : `out/logs/mission-linux-qa/ci-40741589259-status.json` et `ci-40741589259-failed.log`.
- Archives officielles téléchargées : `out/logs/mission-linux-qa/ci-40741589259-artifacts/{normal,asan}/` ; archives annoncées170874 et175302octets, non expirées lors de la lecture.
- JUnit normal CPU SHA256 `3838a75d6ef9904e76496c1733d7186b867c5415e81c5226ca7349437317c99e`, graphique `ed09a1dcb9226378cd6e42bc22add5b1126ab95588404a655b607fb54440ed70`.
- JUnit ASan CPU `44d83ecf276fdacc5c296de7350ba449fa9db5935644d1c662416b0fc063ca7a`, graphique `8a70dec246b1857f603fabf83dfb9b514b6bb974bb1cc0ea44da7f14f1979ac1`.
- Commandes réellement exécutées : gh run view, gh api métadonnées d'artefacts, gh run download dans répertoire neuf, analyse PythonXML/JSON via Context Mode. Pas de build/test relancé par le lead.
- Complément QA : `task_mvsg0uhi_001w_zcu4t6`, pool-8/qa-planner, session `ses_e7c3ef5a5ffe79FU279WXBegrw`, contextecanonique. Contrat d'entrée commité `38d03b7`.
