# 67 — GREEN policy intégré, deux compilations approuvées

2026-10-26. Source auteurpool9 **0e70b002f49c831838efc3af5b1765ca61f48c0b** ; seuls wait_policy.hpp et PC1-PROTOCOL.md changent. Intégration lead **f235db8f72316d707a6be757b4162f57becc354a**, quatre blobs vérifiés contre auteur : headerb119275c1908b5777ab1c99e2d4a7161d46ec92e ; note8b604207bdd2c1ce2d691d49cc668a2580b5f5fd ; baseline0d7683984a6d2a6746e77988a145a37be3e5dd7a et contrat9587989ce2eeae52030464694dfbc8200a7b7459 inchangés.

Le premier patch d'intégration avait omis une actualisation documentaire historique ; comparaison blob l'a détectée avant tout commit/build. Diff ciblé lu et passage ajouté, identités toutes conformes ensuite ; aucune correction autonome de l'implémentation auteur ou des assertions. Prédicat minimal `wait_result == WAIT_OBJECT_0`, aucun runtime/ownership ajouté ; parent/enfant encore absents.

## Commandes exactes préapprouvées

Manifeste prébuild `out/qa-f8/close-policy-green-20261026-01/candidate-before-build.json`, SHA **869f99f5786fec333fe3960fa2fa1fdc8cab91fc81a5a81407a66779c201bc22** : deux argv complets baseline/contrat, mêmes options/protections MSVC14.44 Debug et outils/hash que RED65/66. Seuls chemins de sortie/tags changent ; header et note au nouveau candidat, baseline/assertions identiques par SHA. Sortie/logs neufs, aucune archive écrasée.

Lead approuve ces deux argv exactement avant invocation. Compilation seule -c via wrapper existant, baseline puis contrat, séquentielles ; attendu deux exit0/objets nouveaux, aucun diagnostic d'erreur ou avertissement inattendu. Si baseline échoue, STOP. Source/config/compiler avant/après, logs bruts/codes/horodatages complets ; pas invocation du futur probe. Aucun changement de flags pour obtenir du vert, aucune toolchain installée.

Aucune compilation encore exécutée à cette approbation. RED66 causal conservé, GREEN reste à prouver ; pas CTest/ASan/GPU/runtime/close/Reader inféré. PC1 global non terminé : futurs callers réels et protocole parent/enfant/CLI/RUN restent à traiter séparément.
