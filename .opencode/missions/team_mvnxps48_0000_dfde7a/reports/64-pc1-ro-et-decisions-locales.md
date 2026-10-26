# 64 — PC1_RO intégré ; décisions locales A–D et tranche source étroite

2026-10-26. Proposition pool-9/cpp-engineer six fragments et FIN_PC1_RO récupérés par leurs appels publics team_message natifs, pas nouvelle analyse. Admission fresh/isolation/lecture60/transport déjà vérifiés ; snapshot auteuraddb7cc. Aucun code/compilateur/processus/flag créé. Une lecture de politique externe refusée une fois, non répétée ; aucun élargissement.

## Proposition intégrée

Prédicat privé monomorphe constexpr can_reap_process(DWORD) noexcept : seul WAIT_OBJECT_0 autorise collecte/cleanup. Future obligation de consommation réelle à chaque porte première attente, attente après HOLD et garde du parent ; helper décoratif rejeté. DWORD/handles restent données runtime, pas architecture moteur.

Tranche proposée : wait_policy.hpp déclaration sans définition, wait_policy_baseline.cpp inclut header/macros sans appel, wait_policy_contract.cpp quatre static_assert (OBJECT_0 vrai ; TIMEOUT/FAILED/ABANDONED_0 faux), PC1-PROTOCOL.md. Baseline compile0 d'abord ; contrat RED uniquement diagnostic de définition constexpr indisponible lors de l'évaluation, jamais header/toolchain/syntaxe ou mauvaise valeur fabriquée. Après preuve lead, corps minimal et deux TU GREEN avec assertions immuables. Aucune exécution runtime dans ces contrôles.

Parent futur : superviseur construit avant spawn, buffers/commandline/log/bail prêts ; acquisition hProcess/hThread immédiatement sans allocation jetante. PID+ctime depuis ce même hProcess, pas réouverture PID recyclable. Seul wait signalé permet collecte ; code259/main/log/suppression ne prouve pas fin. Timeout10000ms marque HOLD, attend INFINITE même handle ; fin ultérieure permet cleanup mais aucun PASS rétroactif/bras suivant après incident.

WAIT_FAILED : GetLastError immédiat ; inattendu/ABANDONED ne sont pas fin. Marker préacquis puis Sleep(INFINITE) quiescent, handles/bail conservés, pas retryloop/cleanup/kill. Marker échoué dégrade visibilité sans autoriser unwind abandonnant enfant. Garde de destruction parent : aucun enfant ou fin observée seulement permet retour ; sinon attente INFINITE, puis HOLD si échec. Catch C++ rejoint cette supervision ; /EHsc ne garantit pas récupération SEH/crash fatal/hôte arrêté. Ces limites restent explicites, pas service/watchdog ajouté.

Identité CRT future : adresse réellement appelée fclose, GetModuleHandleEx(FROM_ADDRESS), base/chemin sans troncature, imports recoupés. Thunk EXE/forwarder non résolu=identité absente, pas substitution silencieuse de fonction. Version/SHA fichier DLL par runner lead ; pas hash mémoire ni correspondance sourceSDK certifiée. IsDebuggerPresent et étapes enregistrés. Avant close préparer record ; capture retour/GLE contextuel/errno immédiatement, GLE aprèsCRT ne prouve pas syscall interne.

Enfant : toutes sorties après fopen acquis rejoignent unique épilogue, même erreur lecture/instrumentation ; retirer capacité AVANT tentative empêche retry mais signifie tentative engagée, pas consommation prouvée sur exception. Aucun usage FILE/fd/emprunt ensuite, déprotection/CloseHandle manuel ou garde aveugle. Retour/exception distingués par phases et statut, aucune nouvelle SEH seam. Identité/setup/nominal incomplets arrêtent la campagne. Signal processus précède statut/log/cleanup ; lecture et protection confirmées ne prouvent pas non-publication Reader.

## Décisions locales lead A–D

**A adopté** : prédicat pertinent seulement avec consommation réelle vérifiée aux futurs callers. **B adopté** : première tranche limitée aux quatre fichiers de déclaration/baseline/contrat/note ; pas parent/enfant/flags ni corps GREEN avant RED causal lead. **C adopté documentairement** : HOLD quiescent et garde bloquante conformes60, limites panne fatale conservées ; aucun scénario de blocage provoqué ni RUN autorisé maintenant. **D adopté comme méthode proposée** : adresse/module/imports + version/hash fichier lead, ambiguïté bloque calibration, pas nouveau traceur.

Ces détails restent dans le mandat52/60 approuvé, aucune obligation remplacée ni nouveau moteur/service/droit. CLI/codes et RUN parent **non figés**, à soumettre avant code parent/essais. Calibration partielle seulement, M5Reader/catch/non-publication ouverts.

## Gate source et runner

GO source après commit de ce rapport et recontrôle natif session/worktree9/propreté : uniquement les quatre fichiers ci-dessus sous scripts/probes/qa-f8-close/**. Aucun build/test auteur. Intégration des vrais blobs avant runner ; lead dérive deux argv /c de compile_commands et préenregistre/approuve commandes/SHA/sélections. Baseline non verte ou motif RED différent=STOP/INCONCLUSIVE, pas changement de flags ou répétition aveugle. Même propriétaire pour GREEN après preuve, scores PM55 C79/R84 inchangés. TâchePC1 entière reste in_progress ; cette remise RO ne vaut ni sonde produite ni preuve close.
