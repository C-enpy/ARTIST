# QA Linux — WSL vérifié et deux producteurs prêts en parallèle

## Résultat acquis

La nouvelle distribution ARTIST-Ubuntu est créée et sa qualification initiale est recoupée par le lead. Ce jalon valide l'environnement et la sonde C++23, pas le moteur, ASan ou OpenGL.

- Registre Windows relu : BasePath `I:\WSL\ARTIST-Ubuntu`, Version 2 ; fichier ext4.vhdx effectivement présent (2 730 491 904 octets au contrôle lead). docker-desktop reste enregistré sur son emplacement E:.
- Lecture Linux indépendante : utilisateur non privilégié artist UID1000 sans groupe sudo, checkout sur `/dev/sdf ext4`, GCC14.2, CMake3.28.3, Conan2.12.2 et Ninja1.13.0.
- Sonde existante : le log et cache montrent l'échec de linkage sans bibliothèque de support puis succès avec `stdc++exp`. Le contrat format/stacktrace n'a pas été affaibli. Le lead a relu les preuves sans prétendre réexécuter leur compilation.
- Aucune élévation, reboot, migration, unregister ou arrêt de distribution existante rapporté par l'auteur. Aucun changement système supplémentaire de ce type effectué par le lead.

## Intégration réelle avant consommateurs

Rapport auteur `0bf0e978986998bb906397963f5ee7c8004b5788`, un seul fichier et worktree propre. Après shutdown puis `team_merge`, le rapport est présent dans l'arbre lead. Le merge natif a réussi mais son nettoyage de worktree a échoué ; branche/worktree conservés, pas de second cycle shutdown/merge ni suppression forcée.

Commit d'intégration lead vérifié : `4375080d7dff52819578382d3aca3832b11d065b`, uniquement `research/sessions/2026-10-28-linux-qa/wsl-bootstrap.md`. Aucun diff source moteur/CMake/scripts n'a été introduit. Le merge natif a désindexé les changements ; les contenus préexistants restent hors du commit de cette mission.

Le lead a transféré ce HEAD par bundle dans le checkout Linux, après vérification de sa propreté, puis a contrôlé son HEAD **4375080** et `git status --porcelain` vide. La référence Linux dispose donc de l'entrée intégrée, pas seulement de la fin d'auteur.

Logs auteur copiés sous `out/logs/mission-linux-qa/wsl-bootstrap/` :
- CMakeConfigureLog.yaml : SHA256 `d2e81785206c8f55255cc7a7563e8ecc878d218638516824f62a5f8562cc341d`.
- cxx23-probe.log : SHA256 `cdd162a7e178223a5932b9db5790727376f1ad410e40d888b56b99b8c5045c16`.

## Publication et ownership

La tâche WSL `task_mvsa43rc_0004_dujtol` a été clôturée par le lead après ces contrôles. Le shutdown l'avait remise pending sans doublon lancé ; la clôture correspond au livrable intégré, elle n'est pas utilisée pour masquer un défaut runtime.

Deux publications explicites successives, avec contrôle de l'affectation entre elles :

- **Build/CPU** `task_mvsa43rc_0005_r040cn` → pool-3, build-engineer fresh : presets Linux normaux/ASan, profil Conan, lanceur minimal/tests. Ne possède pas module/probe/vérificateur ASan. Miroir Linux privé proposé `/home/artist/src/ARTIST-build`.
- **ASan GCC** `task_mvsa43rc_0006_6jw879` → pool-4, build-engineer fresh : module/probe/vérificateur et tests autonomes. Ne possède pas presets/profil/lanceurs. Miroir Linux privé proposé `/home/artist/src/ARTIST-asan`.

Les deux ont reçu le plan QA, HEAD/diff intégré, bootstrap, statut des ADR, limites et ownership. Modèles/efforts restent ceux de la politique native approuvée ; aucun override. PM/LLM : build C74/R58 ; ASan C78/R72.

Ressources : caches et sorties privés, un job par défaut. Si des paquets système supplémentaires sont nécessaires, pool-3 possède seul le bail apt/dpkg ; pool-4 remonte son besoin avant mutation. Pas de compilation concurrente d'un même arbre, ni écriture dans le checkout Linux de référence. Les fixtures ASan autonomes permettent le parallélisme sans attendre le lanceur non intégré.

## Prochaines barrières et limites

### Précision procédurale reçue après intégration

Une notification supplémentaire pool-2 ne donnait aucun message non lu dans `team_results`. L'inspection native ciblée de sa session, sans relance, a retrouvé le texte public `msg_18368ec3c001noD2fbGGro08Xj` : « Plan QA lu et confirmé présent dans la base 9bf0850. Lecture effectuée après création : limite procédurale signalée au lead, sans revendiquer une vérification préalable inexistante. »

Le plan commité avant le worktree et l'identité de sa base ont bien été vérifiés. En revanche, **la lecture par l'auteur avant mutation système demandée dans le brief n'a pas eu lieu : écart de procédure établi**. Ne pas présenter la lecture tardive comme une précondition satisfaite à temps. Les contrôles indépendants de stockage, utilisateur, identité du checkout et sonde C++23 restent valides à leur portée ; ils n'effacent pas l'écart de chronologie.

Décision lead dans le mandat autonome : conserver l'environnement vérifié, sans destruction/recréation qui n'apporterait aucune preuve corrective de la chronologie passée. L'acceptation du livrable WSL ne qualifie pas cette étape de procédure conforme. Les auteurs build et ASan ont reçu un rappel ciblé exigeant lecture effective et restitution honnête de leur chronologie avant mutations ; aucun nouveau GO de routine ajouté et aucun droit élargi.

OpenGL/CI, gaps, audit et documentation restent deferred. Leurs dépendances résolues ne les publient pas. Aucun run moteur/GPU/ASan ou CI n'est revendiqué par cette qualification WSL.

Les deux livrables doivent être intégrés et contrôlés ensemble avant publication OpenGL/CI. L'audit final reste conditionnel à son contrat natif hors board et sa restitution, non qualifiés par le présent jalon. Aucune nouvelle approbation utilisateur de routine n'est nécessaire.
