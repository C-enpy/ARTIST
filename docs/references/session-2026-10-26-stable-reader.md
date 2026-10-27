---
title: "Reader stable — bilan, sous-agents et erreurs de coordination"
category: reference
tags: [artist, reference, testing, tooling]
summary: "Nettoyage Reader validé, six remises de sous-agents archivées, clôture28/28 et leçons sur preuves, admission QA et annonces prématurées."
created: 2026-10-27
updated: 2026-10-27
source_revision: a96104c7aaeede7dd6915bfeac86f53565615114
sources:
  - research/sessions/stable-reader-20261027/index.json
  - docs/_meta/session-2026-10-26-stable-reader-cleanup.json
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/95-trois-remises-finales-integrees.md
  - .opencode/missions/team_mvnxps48_0000_dfde7a/reports/96-cloture-native-28-sur-28.md
provenance:
  extracted: 0.8
  inferred: 0.2
  ambiguous: 0.0
---

# Reader stable : résultat et enseignements

## Résultat et portée des preuves

Le contrat adopté est un fichier stable pendant sa lecture, sans auteur concurrent
garanti. À `a96104c`, callback et instrumentation supprimés,13fichiers probes retirés
après consommateurs vérifiés ; contrôles et fixtures utiles conservés.15Reader
caractérisés avant refactor, puis555/555 normaux et555/555ASan Windows Debug,
chacun484CPU+71OpenGLréels. Trois tests retirés/trois ajoutés : même effectif,
pas population historique identique. Aucun nouveau gain de performance démontré.

Extraction/close authentiquement échoués restent sans preuve dynamique ; retirer
des injections ne les résout pas. Les probes annulés demeurent NOT_RUN. B1 est
traité pour deux contaminations ON_CALL précises, pas replay176 ni resetexhaustif.
B5/C2 a une disposition administrative acceptée, pas reconstruction exhaustive.
Voir [[architecture/current-state|photographie]], [[development/build-and-test|commandes]],
[[development/known-incidents|gaps]] et preuveportable citée en sources.

## Sous-agents : apports et limites

Six exports publics ciblés sont conservés sous
`research/sessions/stable-reader-20261027/` : texte assistant et restitutions
team_message uniquement, sans raisonnement privé ni traces d'outils exhaustives.
L'index donne session, rôle, SHA256 et nombre de livraisons. Les anciens messages
de ces sessions ne deviennent pas des preuves du candidat courant par cet export.

- **PM** : inventaire KEEP/MODIFY/REMOVE/HISTORICAL/FOREIGN, consommateurs,
  ownership et graphe ; plan, pas validation exécutée.
- **Architecte** : contrat portable, API `on` conservée et ordre fermeture/publication,
  binding compile-time du profil distinct de l'I/O runtime ; proposition puis adoption.
- **Correctness-reviewer fresh primaire** : diff Reader et scénarios discriminants,
  causes B1 puis cohérence des archives ; pas de tests exécutés ni hashes recalculés.
- **Auteur close, remise finale** : disposition des engagements annulés et contrôles
  résiduels de consommateurs ; aucune expérience close réalisée. Une enveloppe
  `failed` signifiait absence de clôture réservée au lead, pas un échec runtime.
- **Reviewer stdio, remise finale** : revue du retrait et migration des oracles.
  Un résumé d'autre revue a été vu dans la métadonnée autorisée : aveuglement limité
  déclaré, pas utilisé comme unique preuve indépendante.
- **Qa-planner natif direct fresh** : audit rétrospectif du plan89, registre R1–R6,
  risques I×P et correction budget20/60/20,75/25aprèssetup. Ce n'est pas une QA
  indépendante préalable antidatée. Transport final_response fonctionnel ; refus
  team_message horséquipe tenté une fois. Pas de permission élargie ou sandbox certifiée.

## Erreurs évitables et mesures proposées

1. **Clôture annoncée avant vérification du board.** Le lead a annoncé le travail
   terminé à25/28 ; trois affectations restaient in_progress. Après reprise, remises
   réellement intégrées872915b, complétions natives puis contrôle28/28,0pending/
   in_progress/blocked, aucun workerbusy. Règle proposée : distinguer systématiquement
   livraison technique, disposition des tâches supersédées et clôture administrative,
   vérifier ces trois états avant « terminé ». ^[inferred]
2. **Scénario expérimental devenu disproportionné.** Les branches stdio/close ont
   produit auxiliaires et coordination avant recentrage sur fichier stable. Définir
   plus tôt le scénario normal, ses exclusions et le coût de preuve ; une expérience
   hors contrat ne doit pas imposer une architecture de production. ^[inferred]
3. **Affinité QA prise pour admission effective.** L'auto-dispatch a réutilisé une
   session historique aux permissions incompatibles ; les champs fresh/worktree
   n'étaient pas configurables dans l'ajout de tâches exposé. Le contrôler avant
   publication exécutable et préparer un transport natif compatible, sans corriger
   l'outillage par défaut. Le natif direct finalement utilisé n'est pas une réparation
   démontrée d'Ensemble. ^[inferred]
4. **Notifications sans contenu exploitable.** Des appels team_results vides ont
   accompagné des notifications répétées. Limiter la récupération à une tentative
   puis une expérience discriminante ou un diagnostic ; ne pas multiplier réponses
   et appels identiques sans information nouvelle. Cause runtime non établie.
   ^[inferred]
5. **Budget QA mal normalisé.**20setup/55plan/25buffer donne68,75/31,25horssetup,
   pas la cible. Audit natif : correction prospective20/60/20, sans inventer de
   consommation passée. Calculer explicitement le dénominateur avant validation.
   ^[inferred]
6. **Indépendance fragilisée par le paquet documentaire.** Fournir contrat, candidat
   et preuves neutres ; exclure les conclusions d'autres reviewers avant une lecture
   aveugle. Déclarer la contamination si elle survient, sans maquiller une seconde
   lecture en nouvelle indépendance. ^[inferred]

Ces mesures sont des enseignements de méthode, pas de nouveaux verrous exécutables
ni une promotion de [[adr/README|statut ADR]]. [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
et [[adr/0005-build-reproductible-et-validation|ADR-0005]] restent Accepted/partial ;
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]] reste Proposed.

## Clôture sans effacement des dettes

Board vérifié28/28, équipe/worktrees conservés ; le champ long_run/running n'est pas
une tâche active. Anciennes expériences clôturées pour supersession, jamais pour
succès runtime. Les gaps moteur restent nommés ; aucun PASSglobal ou validation
GCC/Linux/Release/image/performance n'est déduit de la clôture du board.
