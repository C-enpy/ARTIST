# 08 — Contrat et QA committés, première tranche C++ publiée

## Résultat et passage à l'implémentation

Le plan QA final réel a été contrôlé puis réconcilié par le lead. Le paquet H0
est maintenant committé avant le worktree auteur ; la complétion native de la
tâche QA a réussi après cette intégration. Le lead a publié uniquement la tranche
C++ accès float/invalidation. Le consommateur et les autres fiches restent différés.

Cette avancée ne prouve aucun comportement du futur accès : le RED/GREEN et
les campagnes moteur sont encore à produire. Aucun nouveau GO utilisateur demandé.

## Preuves effectivement obtenues

- Contrôles locaux des12fichiers possédés : JSON parsables, absence d'espaces finaux,
  plan actif contenant provenance native, oracle pixel et ΔC=ΔL=ΔA=0.
- git add limité au dossier de mission, git diff --cached --check ciblé,
  puis git commit --only sur ce même dossier. Aucun commit des diffs FOREIGN.
- H0 : 49005ed2ca1803eb57726b27247da01f31555a7d,
  « docs: adopt prepared float contract and qualified QA plan ».
- SHA256 qa-plan.md :
  cc418a8ebaabe771a0a7cc9afbf4984ce7b226a24b350c0e6abece5eda026d05.
- Index FOREIGN identique avant/après : renommage stagé .grepai/config.yaml vers
  .grepai/config.example.yaml conservé. Source moteur/tests/shaders inchangée au H0.
- team_tasks_complete(task_mvqmfwpt_0001_vim66t) accepté nativement après réception
  du texte final réel et commit ; aucune complétion fictive ni contournement du
  mécanisme de clôture. L'échec de récupération automatique reste décrit au07.
- team_tasks_add a ajouté une seule tâche avec le vrai ID QA complété : scores84/90,
  agent cpp-engineer, contexte fresh, worktree true, team_message et planned/code.
  Aucun modèle/variante/fournisseur forcé, aucune classification déclenchée.

## Admission auteur et isolation à vérifier avant écriture

Auto-admission observée sous cpp-engineer ; session native OpenAI/gpt-6.1-sol,
variante xhigh du catalogue approuvé. Le worktree existe, HEAD H0 et présence
effective du contrat/plan QA vérifiés par le lead, empreinte identique.

Le GET natif indiquait encore la location canonique pendant la phase planned.
Le lead ne présente donc pas l'existence du worktree comme preuve que toute
écriture s'y déroule déjà. Message de contrôle transmis à l'auteur : avant
édition/build, vérifier la location effective après transition native, ne jamais
écrire dans le canonique, rendre un constat borné si le raccord n'est pas effectif.
Pas de réparation de harness ni d'API de session forcée demandée.

Cette vérification appartient aux préconditions locales de la tranche, sans
réouverture d'architecture ni nouveau jalon d'approbation utilisateur. Un problème
réel de location maintiendrait les écritures fermées, pas les lectures autorisées.

## Ownership et suite

Auteur unique sur contexts et composants pass/uniform nécessaires, nouvelle API
OpenGL float et tests dédiés préautorisés. Pas de modification du consommateur
ReflectedProgramGuardCrossTests, du test pixel, d'Attribute/Pipeline, des fichiers
Reader, de la toolchain ou des profils. Le brief transmet les findings établis,
les préconditions TDD et les ressources exclusives.

Après livraison : arrêter/intégrer par voie native, relire le diff et les preuves,
fournir H1 avant publication du consommateur. Un message d'auteur ou une tâche
completed sans intégration ne libère pas cette prochaine étape.

## Traçabilité et limites

- Tâche C++ : task_mvqn5vg6_0003_gblyx2 ; membre pool-2 ;
  session ses_e82bd3877ffe5ny3I9Xdv55nxv.
- Worktree : C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-f8a8ki-pool-2.
- Board contrôlé à l'admission :2tâches,1completed QA et1in_progress C++.
- Aucun run moteur, preuve GPU, pixels ou mesure de performance revendiqués ici.
- [Plan QA actif](../qa-plan.md), [contrat](../contract.md),
  [publication échelonnée](../publication-plan.json), [réception QA](07-restitution-qa-recuperee-et-plan-valide.md).
