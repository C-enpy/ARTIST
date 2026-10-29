# QA Linux — audit natif indépendant et publication bornée

## Audit en cours

La consultation locale autorisée a confirmé les permissions restrictives du profil. Le reviewer final est maintenant invoqué directement par ce profil natif, dans une session neuve et hors du board d'implémentation. La session réelle confirme `test-reviewer`, provider OpenAI et racine ARTIST ; aucun choix explicite de modèle ou modification des permissions.

Le lancement Ensemble sans tâche avait été refusé avant création car son mode autonome exige une tâche du board. Ce refus n'a pas été réessayé. L'invocation native indépendante disponible est utilisée pour respecter le contrat d'audit hors board, sans créer/reconfigurer d'équipe ou publier l'audit aux auteurs. Le contrôle de lecture et de restitution effective reste à recouper sur sa réponse finale.

Pendant cette lecture, le lead a préparé les sources documentaires et préservé le diff local antérieur des notes d'état, accueil et journal. La publication documentaire finale attend le verdict ; aucun test moteur n'est relancé pour occuper cette attente.

## Préparation de la pull request

La comparaison avec main distant révèle **78 commits locaux antérieurs ou courants non intégrés à main**, dont des missions précédentes. Ouvrir directement la pull request Linux vers main inclurait ces travaux hors mandat : cette voie est rejetée.

Décision technique déléguée : préparer une pull request empilée, ciblée sur une branche de référence au **candidat de départ exact de cette mission**. Ce candidat est déjà ancêtre de la branche de mission publiée ; le nom de branche ne publie donc pas un contenu nouveau. Le diff propre à Linux reste ainsi isolé, sans réécriture d'historique ni push/merge de main. La future PR devra annoncer clairement sa base et la nécessité d'intégrer séparément l'historique précédent avant de la retargeter vers main. Aucun délai ou résultat de merge distant n'est promis.

Il n'est pas nécessaire d'élargir la mission aux précédentes livraisons, ni de rejouer leurs tests : on préserve les dépendances et rend la limite de publication explicite.

## Annexe technique

- Identité native audit : `out/logs/mission-linux-qa/final-audit-native-session.json`, session `ses_e79498a83ffe3GxE7urS3rUXcG`, test-reviewer, openai/gpt-6.1-sol high (sélection native hors board, aucun override).
- Permissions constatées : `final-audit-profile-admission.json`. Ce snapshot est distinct d'une preuve de sandbox ou du simple fichier de profil.
- Base distante main lue par fetch : `b7cc8848ca730d4c060ffad68e82de2144eb0785`. Merge-base identique, 78 commits supplémentaires au moment du contrôle.
- Candidat initial confirmé par le plan QA : `bcd3857494b361fab079092a9880b35522130022`, ancêtre vérifié. Diff mission : 36 fichiers avant documentation finale, contre les nombreuses missions incluses dans main...HEAD.
- Listes de chemins conservées : `pr-base-diff-names.txt` et `pr-scoped-diff-names.txt` ; diff préexistant documentaire archivé : `preexisting-docs-before-final.diff`.
- État des branches distantes inchangé à cette étape ; création de branche de référence et PR prévues après audit/documentation, pas encore effectuées.
