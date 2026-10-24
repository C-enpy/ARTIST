# 08 — F8 : proposition de frontière de preuve H_A

Date : 2026-10-24. Étape : décision architecturale préalable. Proposition seulement, pas H_A accepté/commité.

**Décision ultérieure de l'opérateur : option A acceptée**, réponse « Sonde ciblée + close différé ». Le contrat ci-dessous devient le contrat H_A accepté ; il n'est pas encore commité ni exécuté. Voir le rapport 09 pour sa persistance dans le brouillon de vérification et les barrières restantes.

## Évolution et provenance

Le membre `f8-contract` a livré son analyse en lecture seule via le transport natif, statut auteur `completed`. Aucun essai, build, édition ou commit déclaré. Le rapport 05 ne doit donc plus être lu comme une absence de livrable F8 : une **proposition** est disponible, sans nouvelle preuve runtime.

Le lead persiste cette synthèse ; l'inspection du code détaillée ci-dessous est attribuée à l'architecte, pas présentée comme une exécution ou une contre-revue du lead. Les limites documentaires historiques avaient été recoupées par le lead via QMD (rapport 05).

La tâche `task_mvmbkkq8_0002_5aeiej` n'est pas encore déclarée complète par le lead : l'acceptation de H_A, sa persistance/commit et H_Q restent des barrières avant tout auteur. La révision de catalogue sans Cursor du rapport 07 attend toujours config/start ; aucune nouvelle admission dans cette étape.

## Localisation et contrat observé par l'architecte

- `ARTIST/include/graphic/opengl/pipeline/component/shader/Reader.hpp:65–111` : contexte vidé, ifstream local, masque badbit, extraction locale, clear/close, contrôle fail puis publication ; catch limité à ifstream::failure.
- `extractShaderSource:30–51` : read/gcount, fin courte EOF+fail acceptée, fail sans EOF rejeté. Callback d'extraction emprunté, synchrone et non retenu, sans fermeture/remplacement du fichier par le callback.
- Consommation : `graphic/pipeline/Shader.hpp` read/load et alias Reader dans `opengl/context/ShaderContext.hpp`, puis `shader/Loader.hpp` pour compilation GL. Le rejet Reader doit interrompre cette chaîne avant GL.
- `ReaderTests.cpp` : les tests de sentinel/bad_alloc utilisent un autre istream sur streambuf scripté, pas le filebuf/OS du fichier ouvert. Le test close-avant-publication prouve une fermeture réussie préalable, pas une fermeture échouée.
- Limite d'analyse : références LSP parfois vides malgré appels présents ; complément grep exact ciblé. Headers CRT externes non lus. Interprétation MSVC/filebuf à calibrer.

ADR inchangés : 0003/0005 Accepted/partial ; 0008 Windows Debug seulement ; 0004 Proposed exclu. Aucune seam, API, variante Reader ou infrastructure supplémentaire proposée pour l'option recommandée.

## Option A — calibration Windows ciblée, recommandée

Autoriser un protocole de sonde sur fichier **non vide**, dans une fixture exclusivement possédée, avec second handle Windows et verrou exclusif de plage `LockFileEx`, partage permettant l'ouverture en lecture. Le Reader garde son ifstream/filebuf réel, sans accès à son handle caché. Nettoyage des seuls handle/verrou de test par garde RAII.

Ordre discriminant, exécutant lead distinct :

1. Sans verrou : ouverture/lecture exacte des octets et fermeture réussie.
2. Sous verrou : prouver que l'ouverture ifstream réussit ; vérifier séparément le refus de lecture sur handle de contrôle et conserver le code OS. `ERROR_LOCK_VIOLATION` est une hypothèse, pas un résultat.
3. Calibration ifstream même toolchain/fixture, masque badbit : conserver exception éventuelle, gcount et états bad/eof/fail. Une erreur OS ne garantit pas une ifstream::failure ; le CRT peut la présenter comme une extraction courte/EOF.
4. Même fixture via le Reader réel, sans extraction scriptée : contexte prérempli pour vérifier son effacement, type/message complet et absence de publication partielle. Une éventuelle vérification downstream IShader::load part avec code vide et exige zéro compilation GL après rejet.

Interprétation obligatoire :

- Exception ifstream authentique puis traduction : preuve ciblée du catch réel.
- Erreur OS mais EOF normal et publication vide/tronquée : **candidat défaut causal de lecture**, pas preuve du catch. Retour au lead pour définir la tranche corrective ; pas de correctif anticipé.
- Ouverture refusée ou absence de panne discriminante : INCONCLUSIVE pour badbit réel.

Ne pas changer le masque, poser setstate ni jeter directement depuis le callback pour transformer un résultat non discriminant en succès. Deux essais sans information nouvelle maximum, puis nouvelle hypothèse/expérience ou retour lead. Pas d'installation, service, privilège ou dépendance supplémentaire autorisé.

## Fermeture échouée — maintien explicite de la dette

Le verrou de lecture ne fait pas échouer close sur fichier read-only. Fichier absent, suppression/rename, retrait d'ACL ou fermeture de pipe ne prouvent pas le retour en échec de filebuf::close. Double-close, fermeture sauvage du handle caché, remplacement rdbuf et close dans le callback sont exclus.

Recommandation associée à A : **DEFER** de la fermeture réellement échouée, sans la déclarer couverte ni fermer globalement F8. Propriétaire de reprise : coordinateur avec spécialiste build/OS ; condition : dispositif borné établissant un retour observable close échoué après lecture intégrale réussie, approuvé séparément. Aucune nouvelle infrastructure dans cette mission pour effacer INCONCLUSIVE.

Alternative lourde non autorisée : filesystem/partage laboratoire à injection contrôlée, avec traces OS et calibration CRT ; WinFsp/Dokany ou service dédié ne sont ni disponibles ni installés par hypothèse. Une erreur noyau ne suffit pas à établir une erreur observable close ; un shim simulant le retour fclose reste une simulation.

## Oracles et TDD

- Succès : octets exacts ; fermeture préalable observée séparément.
- Échec lecture : opération réelle ouverte puis défaillante identifiée et reliée à l'essai ; type/message exact attendus, contexte vide, aucune publication partielle ni compilation GL downstream.
- Fermeture rejetée, si un jour atteinte : lecture complète réussie avant close, close effectivement échoué ; branche fail() attend le message exact `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\nClosing the file failed.` ; close jetant réellement ifstream::failure relève de sa traduction exacte. Une panne lecture n'est pas une preuve de panne fermeture.
- Le test seam bad_alloc existant reste utile pour non-traduction, pas une preuve d'OOM réel ; aucune pénurie mémoire globale à provoquer.
- RED seulement si propriété réellement violée ; une caractérisation verte n'est pas RED moteur. Précondition manquante = INCONCLUSIVE. Toute correction reste bornée au résultat causal, sans nouvelle API implicite.

## Décision demandée et suite

Choisir **A : calibration ciblée + DEFER explicite du close réel**, ou **différer les deux sondes réelles** et conserver les preuves existantes avec limites. Recommandation A : exploiter une expérience native bornée sans créer une infrastructure de filesystem.

La décision accepte un contrat de preuve, pas un résultat ni un correctif. Après acceptation, le lead devra commiter le contrat et le plan QA avant worktree auteur ; aucune exécution supplémentaire tant que les barrières et la nouvelle approbation de catalogue ne sont pas satisfaites. Les paramètres précis de fixture et commandes seront consignés avant les essais. Les autres branches et QA proportionate restent inchangées.
