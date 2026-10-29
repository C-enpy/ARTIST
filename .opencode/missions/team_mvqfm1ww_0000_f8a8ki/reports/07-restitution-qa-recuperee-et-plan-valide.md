# 07 — Plan QA reçu nativement, contrôlé et réconcilié avant le code

## Résultat et limite de récupération

L'accès float et son contrat ne changent pas. Le qa-planner admis sous OpenAI a
réellement lu les sources et produit un plan final réconcilié. La notification
Ensemble a annoncé une absence de candidat final récupérable ; le lead n'a ni
réessayé un transport interdit ni relancé le worker.

L'inspection native de cette même session a trouvé outcome succeeded, message
assistant final finish=stop avec time.completed, et texte public complet de21977
caractères. Le lead l'a récupéré par l'API publique autorisée, lu en entier et
réconcilié dans qa-plan.md. Ce n'est pas un livrable inventé ou une fin supposée
à partir du statut inactive. Les appels d'outils de la session sont completed ;
aucun refus ou appel d'équipe par ce lecteur n'est observé.

La rétention automatique de final_response par Ensemble reste non vérifiée et
son annonce d'échec est conservée. La réception du texte natif et son acceptation
par le lead sont distinctes de ce problème de récupération. Aucun correctif du
harness ou changement de permissions n'est engagé. Si l'outil natif de complétion
refuse cette acceptation après H0, la barrière auteur restera fermée sans contournement.

## Qualification et vérification indépendante du lead

Profil natif qa-planner, modèle openai/gpt-6.1-sol high et permissions de session
[] contrôlés ; profil résolu ARTIST restrictif lu par API native. Nouveau contexte
de10événements au premier contrôle, une seule entrée user, pas l'ancienne session.
Les entrées read completed comprennent réellement AGENTS, contrat, plan, profil
et sources ciblées ; artist-build-validation a été chargé. La réponse ne prétend
ni attester elle-même son admission native ni avoir exécuté les tests futurs.
Le lead complète ces attestations natives, sans promettre sandbox/enforcement
universels ni tester des actions interdites pour prouver leur refus.

## Plan retenu et réconciliation

- Exigences explicites/inférées rectifiées : publication/ordre des gardes,
  préservation et mono-thread appartiennent déjà au contrat adopté.
- Risques QA20/15/12, registre bidirectionnel, budget20/60/20 conservés.
- Oracle fixé avant code : FBO32×32 RGBA8, programme existant, triangle et états
  explicites, floats0/.5, pixels(8,8),(24,24),(18,16),(2,2), blanc/fond exacts.
  Calcul géométrique du lead cohérent ; les contrôles communs de la proposition
  finale sont retenus. Aucun rendu n'a servi à ajuster ces attendus.
- Mesures séparées C/L/A, mode mock existant et compteur GL pass-through borné,
  calibration et contrôles discriminants. Pas d'infrastructure générale,
  compteur d'allocation, benchmark ou promesse de gain.
- RED/GREEN auteurs, intégration avant consommateurs, gaps après intégration,
  runner lead distinct, audit fresh hors board et documentation bornée conservés.

Le plan actif est qa-plan.md uniquement. Les observations historiques des rapports
précédents restent datées et ne sont pas réétiquetées en PASS du nouveau chemin.
Les cinq commandes convenues sont encore NOT_RUN dans cette mission. Aucun test
de production, pixel, ASan nouveau ou résultat de performance n'est acquis.

## Étape suivante autorisée, sous conditions vérifiées

Le lead relit les fichiers possédés et commit sélectivement le paquet contrat/QA
H0, en préservant l'index FOREIGN. Il appelle ensuite la complétion native de la
tâche QA sur la base de ce livrable réel intégré, une seule fois. Publication du
premier auteur seulement si cette étape réussit ; aucun GO utilisateur nouveau
n'est nécessaire dans le périmètre long run adopté.

## Traçabilité

- Tâche task_mvqmfwpt_0001_vim66t, membre pool-1,
  session ses_e82cd2b71ffeDxUbgW6PUuLh1C.
- Réponse finale publique msg_17d3440ed0012NHVdJIJ0Svs3y,
  completed1793102164493, finish=stop, outcome succeeded.
- API native session/context via CLI existante ; extraction de texte et métadonnées
  publiques seulement. Pas de reasoning ou corps privé exporté.
- Notification système conservée : inactive sans completed final récupéré ; aucun
  task completed automatiquement. Le premier team_results filtré pool-1 était vide,
  la lecture non filtrée a reçu le diagnostic système, pas un candidat.
- [Admission observée](06-start-approuve-et-admission-qa-observee.md),
  [plan actif](../qa-plan.md), [contrat](../contract.md).
