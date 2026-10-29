# 38 — QA réadmise sous révision3 ; lecture et transport observés

2026-10-25. Objectif : poursuivre le generator H2 sans Go, sans refaire les résultats acquis. Approbation native reçue : révision3, hash `16384ce225b6a451fa55a81dc520ff236091dfb47d8dc370a8fef35200c77a5a` ; statut runtime approved v3 confirmé. Le numéro de révision ne migre pas le format JSON de politique (version2 conservée).

## Admission et ownership

Tentative historique pool-15/task_mvml0lat_001e_u5r71a conservée en erreur/inactive sous son ancienne politique ; aucun changement de modèle, réveil ou faux completed.

Reprise réelle `task_mvnswi1s_0000_eufeda` : analyse read-only test-engineer, PM/LLM50/R55, contexte fresh, candidat318bf7a. Le spawn explicite fresh/worktree:false n'a pas créé de doublon : tâche déjà affectée par auto-dispatch. Contrôle natif : **pool-16 au profil test-engineer**, worktree créé par runtime. Modèle laissé intégralement à la politique approuvée.

Premier mandat borné à l'admission : lire le profil local et TestUtilsTests, envoyer un accusé puis attendre. Avant analyse, le lead a synchronisé les profils et instructions canoniques actualisés dans le nouveau worktree, dont HEAD318bf7a a été vérifié ; aucune source moteur changée. Contrôle diff des deux profils test-engineer/test-reviewer propre.

## Preuve effectivement reçue

Message **H2-ADMISSION** de pool-16 reçu par `team_results` : lectures profil et TestUtilsTests réussies ; execute et team_message allow observés, edit/shell/subagent deny ; envoi execute→team_message réussi. Cela démontre lecture et restitution de ce worker, pas sandbox/enforcement global ni rechargement des anciennes sessions.

Ses retours blocked/failed suivants décrivent la barrière d'attente du lead, pas un test rouge ni un défaut produit. Le lead a ensuite confirmé l'admission dans la **même session**, fourni pin/hash attribués, H_Q17, clauses E5/E6/E8, chemins locaux et delta. Pas de collecte QMD/commande Git imposée au profil restrictif ; un diff ignoré absent du worktree ne doit pas bloquer les sections réalisables. Analyse de gaps maintenant autorisée, restitution complète encore attendue. Pas de consumer final publié avant livraison/intégration.

## Parallélisme et blocage restant

Analyse QA indépendante en cours ; le lead réutilise555/555 normaux, revue et préflight ASan acquis. Pas de build ou test relancé depuis l'approbation. F8/C2 restent ouverts et B1 historique non fermé.

Context Mode demeure absent du catalogue post-redémarrage (recherche ciblée sans résultat). **Aucune analyse volumineuse ni campagne finale exécutée sans lui.** Pour la suite ASan, choix opérateur nécessaire si ce MCP reste indisponible : rétablir sa connexion, ou autoriser explicitement le runner existant avec traitement local borné et logs archivés sans Context Mode. Recommandation : rétablir Context Mode ; ne pas installer son ancien plugin ni lancer de réparation globale implicite. Cette décision n'affecte pas l'analyse QA read-only déjà admise.

## Mise à jour — remise trop volumineuse, récupération uniquement

L'opérateur a ensuite demandé l'arrêt si Context Mode reste absent : absence confirmée, mission arrêtée, sans campagne supplémentaire. Sa nouvelle demande de récupérer le rapport sur disque est traitée comme un mandat **limité à la remise**, pas comme GO global.

Capture opérateur : le worker signale explicitement **« Message content exceeds10KB limit »**. Les notifications et finales bloquées répètent cette cause. Le transport de petits messages ayant déjà réussi, ce refus de taille ne justifie ni nouvelle permission ni répétition de l'envoi identique.

Le profil conserve edit/shell/subagent deny : aucun droit natif d'écriture exact vérifié ne permet de lui demander directement un fichier. Le lead lui a donc demandé le texte déjà rédigé en fragments numérotés sous4000 octets UTF8, avec FIN_H2_GAPS et total, puis persistance par le lead dans `reports/39-h2-generator.md` de cette mission. Pas de collecte/analyse/test nouveau, pas de nouveau modèle ni de transport alternatif. La réduction de taille est une expérience distincte et ciblée, non une relance aveugle du refus précédent. Rapport complet et marqueur de fin encore attendus ; aucun fichier de rapport QA complet n'est revendiqué avant réception.
