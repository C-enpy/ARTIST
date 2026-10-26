# 43 — C : recherche close bornée terminée, obligation toujours ouverte

2026-10-26. Auteur b3-contract/engine-architect, trois fragments C_RO et résultat terminal FIN_C_RO reçus. Persistance lead. Analyse RO dans contexte E3 conservé explicitement autorisé au41, **pas fresh ni revue indépendante**. Aucun fichier source modifié, build/test/installation/handoff/délégation/claim ou nouvelle commande exécutée par chercheur.

## Identité et références

HEAD4b0626e vérifié à l'admission ; source82bf52d/diffARTIST attribués au brief lead. Sources37/38/34/contratB3 et QA17 amendé lues ;41/42 lues après levée des suspensions. Sources CRT extérieures/hashes attribués aux lectures lead42, non recalculés par chercheur. Refus fresh initial et demandes CRT manquants sont historiques, pas un verdict actuel de panne. Recherche localisée sans résultat CRT dans le dépôt ne prouvait pas absence sur la machine.

## Hypothèse et conclusion

Hypothèse unique : après lecture logique achevée, fclose pourrait réconcilier un buffer d'entrée par seek/flush défaillant. Dans la source UCRT10.0.26100.0 digérée au42, **hypothèse non soutenue** : fclose appelle directement __acrt_stdio_flush_nolock, lequel retourne0 pour le flux d'entrée non flushable ; pas seek de réconciliation. Le chemin _fflush_nolock_internal avec _commit est distinct, non appelé ici.

Ce résultat est statique pour cette source/version installée ; SDK rc.exe26100 ne prouve pas la correspondance avec la DLL CRT Debug chargée. Ni erreur close exécutée, ni impossibilité universelle, ni abandon d'obligation.

## Chaîne close et manque exact

Pour fichier ordinaire lecture seule et owner unique sans concurrence, la chaîne restante fournie42 appelle CloseHandle puis GetLastError sur refus ; _close_nolock_internal dissocie/libère le fd avant mapping erreur, fclose consomme le FILE*. Un retour fclose négatif ne permet donc pas de retry/réutilisation du FILE*. Cela ne prouve pas que le handle natif a été détruit ; récupération éventuelle nécessite contrat explicite.

Manquent : condition OS **admissible** refusant réellement CloseHandle après lecture complète sur handle valide, stratégie de récupération sûre après consommation CRT, identité de CRT exécutée, puis traversée de cette erreur jusqu'à non-publication Reader. Flags EOF/ferror/errno seuls ne prouvent pas l'erreur native de fermeture. Lock/rename/suppression/double-close/handle invalidé ne répondent pas au contrat.

La piste distincte HANDLE_FLAG_PROTECT_FROM_CLOSE a été signalée, **non autorisée, non étudiée, aucun flag appliqué ni handle extrait**. Son étude exigerait décision explicite de substitution d'hypothèse et arbitrage sur l'interdiction des flags fabriquant preuve. Aucune nouvelle recherche commencée au-delà de la borne.

## Contrat et éventuelle expérience future

Contrat cible Reader demeure proposé/non adopté : lecture complète et fermeture réussie avant publication ; erreur close empêche publication ; erreur primaire lecture/allocation préservée même si cleanup échoue. Mapping Win32→errno→message métier à décider, non démontré.

Si un dispositif est adopté ultérieurement : fixture possédée/flux valide lecture seule, lecture complète vérifiée, condition OS ciblée autorisée, exactement un fclose avec résultat/errno immédiats, état publié vide, récupération native conforme à l'échec sans réutiliser FILE*, cleanup/voisins vérifiés. Oracle = refus natif relié causalement à la condition, pas errno ou code final seuls. Aucune commande actionnable honnête sans primitive et cleanup attestés ; pas de commande spéculative fournie. Runner lead distinct après protocole exact approuvé.

Binding : structure/CRT compile/link, issue close runtime par fichier ; pas type par asset, render graph, nouveau backend ou retraiteGPU. Aucun nouveau PASS F8/catch/close, statuts ADR inchangés.

## Dispositions à soumettre à l'opérateur

- A : nouvelle hypothèse OS bornée, condition admissible/identité CRT/ownership/cleanup/protocole approuvés ; protection de handle nécessite arbitrage explicite, pas autorisation implicite.
- B : disposition résiduelle motivée explicitement acceptée, conservant absence de preuve, impact, propriétaire et condition de reprise. Ni choix ni DEFER automatique décidés par chercheur.

Recherche C terminée à sa portée ; obligation close encore ouverte. Tâche C ne sera complétée qu'après intégration réelle de ce livrable ; cela ne vaut pas traitement de M5 par preuve ni adoption du futur Reader.
