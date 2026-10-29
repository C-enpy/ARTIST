# Ré-audit ciblé indépendant — F1 fermé

Remise du même reviewer natif, session `ses_e79498a83ffe3GxE7urS3rUXcG`, reçue en finale native après la réparation intégrée `f58a49f4261e018749c59b0d8aab9dd7e85ba53e`. Aucun nouvel essai team_message, aucune édition, exécution ou délégation du reviewer.

**PASS borné — F1 fermé.** Le contrôleur inclut désormais récursivement tous les fichiers de `test-datas`, sans filtre d'extension, dans ses SHA256. La comparaison finale obligatoire reste en place. Pas de nouveau finding dans ce delta direct, ni de complément demandé.

## Preuves relues et attribution

Runner lead : `shader-identity-red.patch` ne change que les tests ; `shader-identity-red.json` conserve le hash du helper original. RED : 10 tests, exactement deux sous-échecs pour `.vert` et `.frag` modifiés, retour incorrectement nul. GREEN Windows et Linux :10/10 chacun. Aucun oracle assoupli entre RED et GREEN.

Le test `test_controller_rejects_shader_change_after_green_tests` écrit un XML graphique simulé vert, modifie ensuite le shader, exige un retour non nul, manifeste FAIL et diagnostic exact. Le compteur graphique vérifie que le rejet survient après son exécution simulée ; les deux témoins stables restent PASS. L'identité réelle n'est pas mockée, seule l'exécution enfant l'est.

Hash test RED/GREEN/intégration : `847f28936a81f057945a8aa84c99608dc4e359242d1b2e0c3142e4b249c23feb` ; hash helper GREEN/intégré : `b4b3050fd770967283c4cd845089eab7dc778270fdc0e038955b1696a65fdbd1`. Ces hashes sont consignés par le lead et relus par le reviewer, pas recalculés par lui.

Scénarios d'ajout/suppression persistants et erreurs de lecture : étayés par présence statique/inférence, pas nouveaux tests exécutés revendiqués. Les tests Python ne sont pas une preuve OpenGL.

La fermeture porte sur l'omission des entrées `test-datas` et sa détection. Les nouvelles campagnes WSL/CI n'étaient pas disponibles dans les artefacts examinés par le reviewer : qualification runtime séparée à restituer par le lead. Toutes les autres limites du rapport initial sont conservées. Ce document persiste la réponse indépendante reçue, pas une auto-revue.
