# QA Linux — démarrage approuvé et préconditions observées

## Objectif et progression

Rattraper les obligations QA Windows sous Linux, avec distribution WSL2 dédiée sur I: puis reproduction CI, en mutualisant les outils existants. La politique native est maintenant approuvée en révision 1 et le mandat long_run actif. Il n'est pas nécessaire de demander un nouveau GO à chaque jalon.

Le runtime a affecté uniquement le contrat de parité à `pool-1`, profil `qa-planner`, contexte fresh, sans worktree et restitution final_response. Les sept consommateurs restent deferred et toutes les clôtures sont réservées au lead. Aucun second worker n'a été lancé.

## Résultats vérifiés

- `team_status` : approved v1, long_run/running, classification manuelle sans appel ; une tâche in_progress, sept blocked ; un membre qa-planner.
- `team_view` : session du qa-planner `ses_e7ca47812ffeHh4wj56IDQaTVO`, profil qa-planner observé. Les règles sur disque ont été relues ; cela ne prouve pas à lui seul leur enforcement natif. Une lecture locale réussie et une remise effectivement reçue sont encore attendues.
- HEAD ARTIST : `bcd3857494b361fab079092a9880b35522130022`. Diff local préexistant : profils/harness, AGENTS, gitignore/GrepAI et provenance documentaire, pas les sources et tests moteur. Ces changements ne seront pas incorporés silencieusement aux commits de cette mission.
- La preuve float préparé conserve sa source `3b1b1028dcd64ca46e0a359365ae096be1a38ac4` ; 506 CPU + 75 OpenGL normaux et ASan sont une référence historique, pas un quota du nouveau candidat.
- Préflight WSL en lecture seule : version WSL 2.6.1.0, noyau 6.6.87.2-1, seule distribution listée docker-desktop, WSL2 en cours d'exécution. Aucune action sur cette distribution.
- L'aide locale WSL expose `--install` et `--location`. L'appel d'aide a renvoyé 255 tout en affichant l'aide ; ce code n'est pas un résultat de test moteur.
- Volume I: NTFS, Healthy, 1 248 615 821 312 octets disponibles lors de l'inventaire. Destination `I:/WSL/ARTIST-Ubuntu` et parent `I:/WSL` absents au contrôle exact avec chemins à slashs.
- Inventaire CI en lecture seule : origin `https://github.com/C-enpy/ARTIST`, branche par défaut main, GitHub CLI présente et accès dépôt confirmé. Les droits retournés incluent push/admin, mais aucune écriture distante, changement de règles ou activation de runner n'a été effectué. Les trois derniers runs listés du workflow « Build, Test, Valgrind » sont en échec le 25 octobre sur d'autres commits ; leur cause n'est pas encore diagnostiquée et ils ne qualifient pas le candidat courant. La formulation historique « pas de CI distante exécutée dans cette session » ne doit pas être interprétée comme absence globale de runs distants.

## Limites et prochaine barrière

Premier message du qa-planner reçu effectivement via `team_results` : lectures locales AGENTS/profil/skill et preuves Windows déclarées réussies, aucun shell/edit/test ; le plan complet n'est pas encore récupéré. Cet échange vérifie un transport de message, pas encore le mécanisme de remise finale. Le chemin de persistance décidé par le lead est `.opencode/evidence/mission-linux-qa/qa-plan.md`, afin de ne pas écraser le plan de la mission GPU historique. La tâche QA reste ouverte.

Le dernier échec CI historique a été précisé en lecture seule : run `40219338519`, candidat `b7cc8848ca730d4c060ffad68e82de2144eb0785`, échec « Set up job » ; annotation GitHub : « Unable to resolve action sonarsource/sonarcloud-github-c-cpp, repository not found ». Il ne s'agit donc pas d'un échec moteur reproduit ni d'un RED du portage. Cette preuve justifie de découpler la baseline obligatoire de l'intégration Sonar facultative au jalon CI, comme déjà prévu. Aucun workflow n'est modifié ou relancé maintenant.

La commande CLI `opencode` n'est pas présente dans le PATH de l'exécuteur utilisé pour une lecture de métadonnées de session. Ce n'est ni un refus de permission QA ni une raison d'installer/réparer OpenCode. Le profil observé par les outils natifs reste qa-planner ; aucun droit n'a été élargi.

Le lead a transmis au qa-planner HEAD/diff, statut des ADR, référence Windows et limites de son mandat. La remise QA doit être récupérée, relue, persistée et commitée avant publication WSL et avant les worktrees auteurs. WSL n'a pas été créé ; aucun build/test moteur ou run CI n'a été exécuté à ce jalon.

## Méthode et responsabilité

Localisation : CMake/Conan/CTest existants et module ASan MSVC ; pas d'exploration source supplémentaire nécessaire à ces préflights. Impact : l'inventaire système prépare la future tâche WSL sans la publier. Limites : aucun PASS Linux ou enforcement de permissions déduit de la configuration.

Commandes réellement exercées par le lead via Context Mode : git rev-parse/status/diff et remote get-url ; wsl --version/list --verbose/help ; Get-Volume I: et Test-Path sur la destination ; Get-Command des outils hôte ; gh api repos/C-enpy/ARTIST (métadonnées filtrées) et gh run list (3 runs). Aucun Docker créé ni modifié.
