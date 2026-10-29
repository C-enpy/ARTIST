# 19 — Préconditions auteurs B4/B6 : worktree et rattachement natif

Date : 2026-10-24. Étape : accès auteurs, avant mutation. H_Q64d97dc et H_A354b53f restent acquis ; aucun RED/GREEN moteur.

## Retours réels reçus

- **pool-10 / gpu-engineer / B4-U** : outils fichiers au canonique, pas de worktree fourni, aucune édition. Caractérisation statique limitée : trois débinds terminaux après ASSERT, ProgramUnbinder existante et voisins ClosedExecution. Ni preuve GL ni négatif exécutés.
- **pool-11 / cpp-engineer / B6** : un worktree existe dans les métadonnées Git, mais lecture H_Q absolue refusée **permission.rejected / external_directory** ; outils fichiers et Serena au canonique. Aucune analyse auteur complète, modification ou test. Ce refus est une preuve runtime d'accès, pas une erreur moteur.
- **pool-5** : confirme seulement livraison du plan H_Q acceptée/intégrée, aucune collecte ou mutation nouvelle.

## Diagnostic du lead

Résolution native des sessions, sans navigation du client utilisateur :
- pool-11 : `ses_e91b1804affejuxCBFanksZ3zD` ;
- pool-10 : `ses_e91b1c3fcffej6E80MnJZGglrj`.

Contrôle filesystem/Git via Context Mode : worktree pool-11 existant et propre, HEAD64d97dc, H_Q présent. Chemin standard pool-10 absent ; `team_status` confirme pool-10 state off sans worktree, contrairement à pool-11 state ready avec worktree. Ne pas déduire un worktree de la seule prose du contrat.

## Corrections natives bornées

1. `opencode.session_move` de la même session pool-11 vers son worktree existant `.../ensemble-2dc31e0a-6wzibc-pool-11` : réponse native confirme la destination. Aucun nouveau worktree ni changement de modèle.
2. `opencode.worktree_create({name:'qa-harness-b4-u'})` : création native à `C:/Users/djden/.local/share/opencode/worktree/3a086c/qa-harness-b4-u`, **après** commit H_Q. `opencode.session_move` de la même session pool-10 vers ce répertoire : réponse native confirme destination.

Contrôles du nouveau worktree B4 : racine exacte, HEAD64d97dc, statut propre, H_Q présent, SHA256 H_Q `78c5f1aae2e51774698e8c1550fe9e60eebf801d907b3bad4e01980926f83c7f`. La branche courante renvoyée par Git est vide (HEAD détaché) ; worktree natif déclaré de stratégie git. L'association future à l'intégration Ensemble n'est pas prouvée par sa création : **pas d'édition B4 autorisée avant contrôle d'identité et de voie d'intégration**.

Pas de suppression/recréation des sessions, de grant de permissions, de changement de provider/modèle ni de mutation de Serena partagée. La racine Serena canonique est conservée, valable pour la lecture du code identique mais pas preuve du contenu d'un futur diff worktree.

## Nouvelle expérience discriminante demandée

Chaque membre a reçu une reprise sur sa même tâche : à la frontière du nouveau tour, lire `.git` comme **fichier pointeur** du worktree (pas `.git/HEAD` comme au canonique), puis H_Q via chemin **relatif**. Rendre racine/contrat lisibles ou refus exact après un seul essai. Aucun shell/build/test/installation ; analyses/protocoles seulement, B4 texte de proposition sans édition pour l'instant. B6 reste sans correction avant oracle/RED validés.

La réponse native session_move ne prouve pas que tous les outils provider ont effectivement changé de répertoire ni que la permission external_directory a disparu. Le retour des auteurs doit le confirmer ; ne pas qualifier le rattachement de corrigé au runtime avant ce retour.

## État et limites

Aucun code/test modifié, aucune caractérisation GL ou compilation exécutée. B1/B3 ne sont pas qualifiés accessibles par ces opérations sur B4/B6. Les consommateurs d'intégration restent bloqués jusqu'à de vrais livrables. Prochaine étape : accusés d'accès et plans discriminants, puis qualification du runner et RED ciblés sous bail ; pas répétition coûteuse du même refus ni relance générique.
