# 04 — Préparation corrigée : une seule tâche initiale, suite après intégration

## But et décision du coordinateur

Livrer le float préparé sûr et son consommateur graphique, sans élargir la mission.
Le contrat reste adopté et la politique OpenAI/long_run/proportionate inchangée.
Le nouveau blocage PM porte seulement sur la publication du graphe, pas sur le C++.

Le coordinateur choisit la publication échelonnée prévue par ARTIST : importer
initialement seulement la qualification QA ; garder les cinq autres fiches dans
le plan documentaire ; les publier ensuite par ajout natif au fur et à mesure
des intégrations vérifiées. Cela ne change ni l'architecture ni les exigences
utilisateur et ne nécessite pas une nouvelle question d'autorisation.

## Éléments réellement vérifiés

Le catalogue exposé de team_tasks_add contient désormais complexity, risk,
agent, context, worktree et report_mode. Les descriptions annoncent la conservation
du contrat par auto-scale et le refus des overrides contradictoires. Aucun appel
mutateur n'a été effectué pour le tester dans la mission moteur.

Le candidat PM complet a été lu nativement : version2,6tâches,71critères/contrôles,
parallelGroups vide. Les six fiches encodent structurellement scores, contexte,
agent, worktree et report_mode avec les valeurs déjà établies. Il ne reste qu'une
question bloquante : importer six tâches maintenant ou publier progressivement.

La référence Ensemble preparation.md déjà consultée décrit l'import initial
avant start et les ajouts ultérieurs comme des ajouts ordinaires au board, non
une seconde importation. La voie team_tasks_add est exposée avec les champs requis.
Elle permet donc de conserver un petit import initial sans perdre le graphe complet
du plan. Ni l'import ni un document ready ne vaudraient approbation start.

## Déploiement transmis, pas preuve moteur

La session plugin rapporte paquet20261027-preparation-admission-ac2bbe7eb142,
migration29,1807tests et build/typecheck passants, import atomique/replay stable,
contrat manuel+auto-scale et final_response avec profil natif sans grants/handoff.
Ces résultats restent attribués à cette session, pas réexécutés par le lead ARTIST.

Son essai natif explore a admis une session fresh/worktree:false/final_response,
puis échoué au quota Go429 avant lecture. Admission observée ne prouve donc ni
lecture/restitution ni qualification qa-planner ARTIST sous catalogue OpenAI.
La première tâche conserve explicitement ces contrôles. Aucun changement de
modèle, budget, permissions ou exigences pour contourner cet échec.

## Séquence ferme

1. Import initial : qa-contract seulement, scores66/76, qa-planner fresh,
   worktree false, final_response. Pas de dépendance vers une tâche non importée.
2. Après start utilisateur : qualification native, lecture/restitution QA,
   validation lead et commit sélectif du contrat/plan H0 avant auteur.
3. Ajout natif prepared-float, scores84/90, cpp-engineer fresh, worktree true,
   team_message, dépendance avec le vrai ID QA reçu.
4. Après intégration effective du mécanisme H1 : ajout consumer-pixels,74/77,
   gpu-engineer fresh/worktree true/team_message et vrai ID du mécanisme.
5. Après intégration H2 : revue sûreté76/85 et gaps70/68, tous deux fresh,
   worktree false/final_response, vrais IDs de producteurs ; lectures parallèles.
6. Lead applique/exécute, corrections intégrées H3 et audit fresh hors board.
   Seulement ensuite ajout documentation43/50, worktree true/team_message.

Les barrières sont possédées par le coordinateur, pas des tâches factices.
Complétion d'auteur seule insuffisante ; shutdown/intégration native, diff et
preuves contrôlés, puis HEAD intégré transmis avant publication du consommateur.

## Suite recommandée

Une reprise PM utile doit limiter tasks à qa-contract et conserver les cinq
fiches différées dans le handoff/context avec leurs sources locales. Pas de
réécriture technique, classifier, seconde importation globale, réparation du
harness ou assouplissement des droits. Le statut ready éventuel doit venir de
ce candidat corrigé/validé, jamais être fabriqué sur celui à six tâches bloqué.

Le lead n'a ni auto-resume ni invoqué start, ni publié de tâche ou lancé de worker.
La qualification QA reste NOT_RUN pour cette nouvelle admission ; auteurs fermés.

## Pièces et traçabilité

- [Contrat adopté](../contract.md), [QA conditionnelle](../qa-plan.md).
- [Graphe complet](../task-preview.md), [contrats de publication](../publication-plan.json).
- PM ses_e8375229fffe2i54YJk4AVWb5i, candidat public
  msg_17d281f6e0014In1LWYaHURuMI ; api get /api/session/{id}/context via CLI existante,
  sélection du texte public et parsing intégral ; reasoning/corps d'outils exclus.
- Rapport plugin transmis :
  C:/Users/djden/source/repos/opencode-ensemble-session/docs/plans/2026-10-27-preparation-admission-gap.md.
- Aucune nouvelle compilation, exécution moteur, preuve GPU/pixels ou autorisation
  d'exécution. Politique et sources moteur inchangées par cette clarification.
