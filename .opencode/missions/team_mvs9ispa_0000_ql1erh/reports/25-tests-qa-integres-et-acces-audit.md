# QA Linux — tests terminés, accès de qualification de l'audit manquant

## Objectif et résultat

La mission vise les mêmes obligations QA sous Windows et Linux, sans masquer les échecs ni créer un second harnais. Les campagnes moteur sont maintenant vertes : **589/589 dans les six configurations Windows, WSL et CI Ubuntu, normales et ASan**. Le défaut GLEW trouvé en revue a été corrigé puis fermé par ré-audit indépendant.

Depuis le précédent point, les trois tests recommandés par le QA ont été intégrés : mauvais contexte graphique, environnement ASan hostile et identité changeant pendant une campagne. **9/9 tests du contrôleur passent sous Windows et Linux**. Trois mutations temporaires en mémoire sont détectées par ces tests ; aucune source de production n'a été modifiée pour ces expériences. L'instrumentation ASan des exécutables moteur a été contrôlée séparément des canaris.

## Pourquoi une décision est nécessaire maintenant

Il reste l'audit final indépendant des tests, exigé par les règles du projet. Avant d'admettre ce reviewer, ces mêmes règles imposent de vérifier son profil réellement chargé, ses permissions et sa restitution. Le fichier de profil a été relu, mais ce fichier ne prouve pas les permissions actuelles du runtime.

La consultation native de rafraîchissement des profils (`GET /api/agent` sur le service OpenCode local, dans le contexte ARTIST) avait été refusée. Ce refus n'a pas été répété ni contourné. Les permissions historiques des autres admissions et une lecture de fichier ne suffisent pas pour certifier cette nouvelle admission. Aucun nouveau reviewer n'a donc été lancé et aucun audit fictif n'est annoncé.

## Choix et recommandation

**Recommandation : autoriser une seule consultation native en lecture seule des profils effectifs**, puis continuer l'audit uniquement si elle confirme le profil restrictif et la voie de restitution attendus. Cette autorisation ne change aucun droit d'édition, modèle, politique, configuration ou source. Elle permet seulement de retenter le contrôle précédemment refusé. Aucun délai n'est estimé ; les campagnes moteur déjà valides ne seront pas rejouées pour remplir ce point.

**Reporter cette consultation** conserve les tests et corrections intégrés, sans changement de permissions. L'audit final, la clôture QA et la PR finale restent en attente ; la parité exécutée reste acquise à son périmètre, mais pas l'acceptation globale de mission.

La décision demandée porte exclusivement sur cette consultation locale en lecture seule, pas sur une nouvelle approbation de mission ni une permission générale.

## Annexe technique

### Suite de la décision utilisateur

L'utilisateur a choisi **Autoriser la lecture locale**. L'unique consultation autorisée a réussi ; snapshot `out/logs/mission-linux-qa/final-audit-profile-admission.json` : profil natif test-reviewer/subagent, deny-all suivi des permissions restrictives attendues, édition/shell/délégation interdits. Aucun fichier de configuration, modèle ou droit modifié. Hash du profil sur disque : `14c5b6fa14d091c5012eec518fe133cf26e80e221bf583ce31ea7dadfad33591` ; le snapshot natif, et non ce hash seul, fonde le contrôle.

Le premier lancement team_spawn hors board a été refusé avant création : « Autonomous workers require claim_task ». Il n'a pas été répété et l'audit n'a pas été publié sur le board d'implémentation. Le profil natif test-reviewer disponible dans cette session a ensuite été invoqué directement en contexte neuf, sans modèle explicite, worktree ou tâche d'implémentation : session `ses_e79498a83ffe3GxE7urS3rUXcG`, sélection native de session inchangée. C'est l'invocation indépendante hors routage Ensemble prévue par le contrat d'audit, pas une équipe recréée ou un contournement d'une permission refusée. Lecture et restitution effectives restent à vérifier à sa remise.

L'audit est donc en cours, plus bloqué sur la consultation. La préparation documentaire peut avancer indépendamment ; publication finale et acceptation attendent son résultat réel.

- Tests intégrés : `1532b78d96489923773e7b2a8b355dc91f94345c`, runtime/config identiques au candidat moteur exécuté `78e7b74`.
- Propositions et résultats : `.opencode/evidence/mission-linux-qa/gaps-integrated-candidate.md`. Logs runner lead `qa-gaps-{windows,linux}.log`, expériences `qa-gaps-mutations.json`, sous `out/logs/mission-linux-qa/`.
- Préparation neutre de l'audit : `.opencode/evidence/mission-linux-qa/final-audit-input.md`, sans justifications privées d'auteurs. Aucun worktree ou nouveau worker créé pour ce document.
- ASan : 43 TU unit et 13 TU composants instrumentées en WSL et CI ; deux exécutables WSL liés à libasan avec références ASan, hashes concordants. Canaris sain 0 / faute 86, detect_leaks=0.
- CI recoupée : [40760506988](https://github.com/C-enpy/ARTIST/actions/runs/40760506988). Voir rapport 24 pour les archives des six campagnes.
- Diff préexistant profils/harness/GrepAI/docs préservé ; aucune modification de ces permissions ou de politique pour obtenir le passage des contrôles. Explication des dépendances incrémentales Windows encore incomplète ; anciens FAIL conservés.
