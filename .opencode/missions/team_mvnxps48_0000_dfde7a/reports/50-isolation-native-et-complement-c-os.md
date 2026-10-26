# 50 — Auteur fresh isolé nativement ; complément C-OS intégré

2026-10-26. Milestone d'admission/documentation ; aucune source/test sonde ni flag appliqué à ce jalon.

## S_CODE_RED : contrôle réel, pas prose d'isolation

S_RO terminé après protocole intégré/adopté et frontière48, pool-5 arrêté ; team_merge confirme branche préservée sans commit ni changement. Pas de tâche source libérée à tort ni ancien producteur relancé.

S_CODE_RED task_mvpexfbb_0019_pk2j2z publiée comme consommateur, PMbrut68→70/R66 sans modèle imposé. Auto-scaling crée pool-8/cpp-engineer, **nouvelle session** ses_e872aea31ffeoxqiz6Gi4MqvIY ; contrôle du contexte natif : seule affectationpool8,12messages d'admission/lectures, pas historique E1/E3. Son worktree enregistré ensemble-2dc31e0a-dfde7a-pool-8 : gitstatus vide, HEAD622a2f840bd2216768d85ba8d85f12a1fc8ae3b2, paquet49 présent. Né après QA/amendements commis.

GET initial pointait malgré tout au canonique ; code auteur resté bloqué. Voie publique **opencode.session_move** avec sessionID/sonworktree : résultat réussi, puis GET restitue **location.directory du worktree exact**, agentcpp-engineer. Aucun changement de modèle/permissions demandé ; modèle rendu Sol/high selon routage natif, aucun override. Aucune base interne ni fichier plugin modifié.

Ultime preuve de lecture effective demandée au worker : read .git relatif doit lire le fichier gitdir du worktree (pas dossier canonique), puis paquet49 relatif et restitution. **GO source non donné avant cette preuve**. Seule tranche après admission : controls+helper hérité inchangé, pas boucleGREEN/probe avant preuveREDlead, source intégrée avant runner. L'isolation ultérieure et l'indépendance du reviewer restent des obligations, pas garanties générales du plugin.

**Mise à jour après restitution** : pool-8 a réellement lu .git relatif, fichier pointant ARTIST/.git/worktrees/ensemble-2dc31e0a-dfde7a-pool-8, puis paquet49 relatif45lignes ; env.directory et workspace root rendus dans son worktree. Transport reçu via team_results. Admission désormais validée par le lead et **GO source limité à la tranche RED49** envoyé dans la même session. Aucun compilateur/test/run auteur, aucune boucleGREEN/probe/Reader autorisés. Les anciens blocked/failed d'admission restent datés, pas des FAIL comportementaux. Transition native de phase si exigée par la politique admise, sans agent/modèle imposé, autorisée pour cette seule édition. Les sources n'étaient pas encore produites au présent checkpoint.

## C-OS : complément récupérable à la frontière processus

FIN_C_OS_COMPLEMENT reçu, après lecture directe ExitProcess par architecte : https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess (metadata2025-08-27). Lead avait fetché la même page au47. Étape6 ferme tous les handles du processus ; étape8 signale sa terminaison **après** fermeture ; retourmain mène à ExitProcess. ProtectionA vise CloseHandle, pas cette récupération de fin de processus.

**Conclusion documentaire conditionnelle révisée** : enfant dédié peut retenir le handle natif après fclose négative puis terminer normalement, sans CloseHandle sur l'emprunt ni dérogation aux avertissements _get_osfhandle/fclose. Cela amende uniquement l'absence de récupération établie dans les seules A–D du47 ; aucune exécution/garantie universelle.

Ownership proposé/non adopté : enfant seul propriétaire fichier/flux/handleprotégé, sans transmission/héritage/duplication hors enfant ; après uniquefclose, FILE*/fd jamais réutilisés, aucun garde/atexit/destructeur ne réessaie. Valeur native log diagnostic uniquement, parent ne l'utilise pas. Parent possède ses propres handles process/thread, attend signal de terminaison puis statut/logs et contrôle fixture/voisins ; suppression seule ne prouve pas fermeture (share-delete). Fermeture des handles de supervision reste son obligation.

ExitProcess appelle DLL_PROCESS_DETACH avant récupération ; deadlock documenté possible avec threads/verrous. Enfant minimal sans threads auxiliaires volontaires/plugin ne constitue pas preuve universelle de sûreté des DLL/CRTchargées. Timeout ≠ terminaison : pas de récupération présumée ou nettoyage prématuré ; conserver état/remonter au lead. TerminateProcess non adopté comme fallback, autres enfants non terminés implicitement.

Restent avant expérience : admissibilité explicite de l'injection de politiqueOS, identitéCRTchargée/chemin42, retour/exceptiondebugger/mappingOS, protocole enfant/supervision/timeout et oracles/commandes approuvés. Ni fclosecalibrée ni finprocessus ne prouveraient non-publicationReader ou catch historique. Aucune applicationflag, handle extrait/manipulé, processus d'essai créé, code/test/commande exécutée. M5/F8 toujours non prouvés. Étude C-OS terminée à sa portée, admissibilité expérimentale non décidée.
