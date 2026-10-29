# 36 — P-CAL produit, restitution bloquée par la taille

2026-10-26. Diagnostic borné, aucun changement de source ou test.

Les notifications sans remise accessible ne prouvaient pas une nouvelle livraison. Contrôle ciblé : team_status annonce1message non livré et pool-7 actif ; team_results global puis ciblé renvoie aucun message non lu. Hypothèse discriminante : échec du transport auteur plutôt qu'analyse absente.

Lecture publique native ciblée de la session pool-7 ses_e87558e7affeOtee8XOFUkxYPZ : appels team_message msg_178ad5a280012dW02g64zzHNaY puis msg_178af8c53001j3Hvvyxuz1uDn2 retournent **Message content exceeds 10KB limit**. Le texte de graphe est déjà produit ; aucun besoin de refaire l'enquête. Seuls appels/publics et résultats de transport consultés, aucun raisonnement privé.

Demande au même worker : fragmentation numérotée <3500octets UTF8, appels séquentiels, puis FIN_P_CAL ; pas de nouvelle analyse, permission, worktree ou boucle de bootstrap. Ce refus de taille autorise une correction du format de restitution, pas un grant ni une répétition identique plus coûteuse. P-CAL reste in_progress avant intégration.

Un premier script de lecture native avait une erreur locale de chaîne newline ; corrigé par String.fromCharCode(10), puis lecture réussie. Ce n'était pas une panne Context Mode.

HEAD documentaire decd225 ; source82bf52d et absence de diff ARTIST conservées. La photographie C2/B3 a été actualisée puis indexée : update-wiki exit0,2notes ; embed artist-wiki exit0,38chunks/2documents. Aucune calibration exécutée. Après réception : intégrer le graphe/contrat de sonde, puis publier seulement les deux branches prêtes sous les barrières QA/admission déjà fixées.
