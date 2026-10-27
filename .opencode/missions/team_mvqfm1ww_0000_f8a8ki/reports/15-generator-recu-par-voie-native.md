# 15 — Propositions QA réelles reçues, récupération automatique défaillante

## Résultat et décision

La QA a terminé avec deux réponses finales publiques présentes dans sa session.
Ensemble n'a pas récupéré ces textes et n'a complété aucune tâche. Le lead les a
lus intégralement par la voie native de la même session, sans relance ni réparation.
Il retient deux nouveaux scénarios CPU et trois assertions de miroir, dans un
seul fichier de tests, ainsi que les contrôles discriminants déjà prévus.

Les ajouts vérifient la révocation du pass indépendamment de l'uniform, la
préservation de l'ancienne publication après échec tardif de réflexion, et l'absence
de mutation CPU après chaque refus de contexte/programme. Aucun défaut de
production n'est démontré par cette analyse. La QA n'a rien écrit ni exécuté.

## Préconditions et portée

Le HEAD courant et H2 diffèrent uniquement par trois documents ; les quinze
empreintes de la tranche ont été recoupées par le lead. La réserve initiale de la
QA est explicitement remplacée par cette corroboration dans son addendum.
Les 577 tests restent des exécutions auteur, pas une nouvelle campagne intégrée.

Profil test-engineer réel, contexte fresh et permissions de session sans override
vérifiés. Lectures locales et messages reçus ; final stop/completed et outcome
succeeded observés. Pas de certification sandbox ou de récupération Ensemble
réussie. Un glob de chemin inexistant a été corrigé par découverte locale, pas
par élargissement de droits. Le brief complet et l'identité lui ont été retransmis
par le lead après son signalement de contexte initial incomplet.

## Suite autonome

Persister les propositions et leur acceptation avant application, puis compléter
la tâche native uniquement sur cette remise réelle intégrée. Attendre la fin de
la revue du snapshot avant les expériences qui changent temporairement les headers.
Runner lead distinct, tests puis mutants séparés/restauration et suites finales,
audit fresh hors board. Aucune nouvelle infrastructure ni GO de jalon nécessaire.

## Traçabilité

- [Propositions et mandat runner](../qa-gap-proposals.md).
- Session `ses_e82683352ffeLTe6E71BlFTId4`, finales
  `msg_17d9a1799001Uyd3gfEFfNE9Yz` et `msg_17d9b405b001CvPnBOlDvoHFUY`.
- Tâche generator `task_mvqqgu7a_000q_77126c`, restitution native lue par le lead.
- Empreinte agrégée quinze chemins/empreintes triés LF :
  `b1b84aa5ae63c820cdc7079decfac48fd230f50dc864071a2e37d7d85055873f`.
- [Entrées locales](../qa-integrated-input.md), [état intégré](14-consommateur-integre-et-revues-pretes.md).
