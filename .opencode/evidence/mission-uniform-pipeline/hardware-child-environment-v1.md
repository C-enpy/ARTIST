# Environnement enfant matériel — contrat privé ratifié

Décision lead après avis `result_mvumaqrx_0000_w3jsme`, finding indépendant
`result_mvum8csu_005b_84umaw`. Ce contrat corrige le pipeline existant, sans
nouveau schéma, backend ou autorisation d'exécution native.

Dans `SubprocessRunner.run`, copier le parent puis retirer les six variables
publiques de contexte ambiantes comme actuellement. Déclencher le contrôle suivant
uniquement lorsque `command.env_delta['ARTIST_VERIFICATION_CONTEXT_V1']` vaut
exactement `opengl-hardware` :

1. Exiger les six champs de transport explicites et non vides. Leur sémantique
   et l'admission des identités restent au core, pas dans un nouveau parser runner.
2. Refuser avant lancement si `env_delta` contient `LIBGL_ALWAYS_SOFTWARE` ou
   `GALLIUM_DRIVER`, quelle que soit leur valeur ; aucune réintroduction permise.
3. Retirer ces deux variables de la copie enfant, puis appliquer le delta.

Le parent demeure inchangé. Contrat identique pour workloads, CTest et découverte
CTest portant cette déclaration ; aucun choix selon argv ou nom du GPU.
Déclaration matérielle seulement ambiante : ne déclenche pas l'assainissement,
ses variables de contexte restent supprimées. CPU/sans déclaration : héritage
de ces deux variables inchangé. Logiciel explicite : valeurs déclarées conservées.
Ne pas employer None ou une valeur vide artificielle pour simuler une suppression.

Le profil demandé est ainsi réalisé sans fallback, mais un renderer matériel
n'est pas garanti : contrôle GL et cible exacte restent obligatoires. D'autres
mécanismes externes de choix de driver ne sont pas couverts par cette correction.

Périmètre futur : runner.py et tests core sous propriétaire unique explicitement
admis après complément QA commité. Le propriétaire Linux garde son candidat
stable et peut préparer ses tests d'intégration, pas modifier le runner.
Le lead reste runner de tests : faux popen_factory strict uniquement, aucun
processus réel pour cette tranche. RED avant implémentation, GREEN/non-régression
et revue indépendante avant intégration puis revalidation Linux.

Oracles minimaux : matériel complet/parent logiciel, invocation workload/CTest,
transport incomplet refusé, forçage explicitement réintroduit refusé, contexte
matériel seulement ambiant, logiciel explicite, CPU, parent intact et commandes
matériel/logiciel successives sans contamination. Les refus sont des erreurs
du pipeline, pas un résultat favorable ou une précondition GPU réelle.
