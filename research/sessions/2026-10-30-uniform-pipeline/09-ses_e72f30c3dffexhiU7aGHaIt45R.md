# Planifier clôture Git sûre

Session: ses_e72f30c3dffexhiU7aGHaIt45R
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T13:43:58.193Z — msg_18d0d8ded001Buxo5hnw9ofbIr

## Plan de fermeture — remise à l’orchestrateur uniquement

**Aucune mutation, commande Git, exécution de test ou sous-délégation effectuée.** Ce plan ne publie aucune tâche et n’autorise aucun nouveau worker.

### Base vérifiée et limites

- Instructions `AGENTS.md` prises en compte ; rapports **156/157** et politique persistée lus.
- Réception : pipeline **livré pour qualification**, code Windows `a130773`, couture `165aee5`, revue G1–G4 sans défaut bloquant établi dans sa portée.
- Rapport157 : fermeture native potentiellement destructive ; `team_cleanup` peut fusionner des branches, supprimer des références et retirer des worktrees. Ce n’est pas un simple archivage.
- Contrat documentaire `artist-wiki`, mission F7, lignes45–48 : préserver les preuves avant retrait des espaces auteurs ; comparaison de preuves ≠ nouvelle exécution.
- La politique existante reste inchangée. Aucun barème spécifique de complexité/risque n’apparaît dans le fichier lu : évaluations **PM/LLM**, selon les dimensions ARTIST.

**À prouver par l’inventaire du lead :** HEAD courant, contenu effectif de `main`, remote cible, écarts, modifications locales, références des workers et comportement natif disponible aujourd’hui. Les commits cités ne constituent pas ces preuves.

## Contrat commun

- Un seul propriétaire des mutations : **coordinateur existant**, sans nouvelle admission auteur.
- Aucun run moteur, GPU, CMake, benchmark, installation, cloud **ni nouvelle campagne synthétique** nécessaire à cette fermeture.
- Aucun `force`, purge, reset destructif, nettoyage global ou staging global.
- Fusion de contributions workers : **`team_shutdown` puis `team_merge`**, jamais fusion Git manuelle de branches teammates ; ne pas demander aux workers de committer manuellement.
- Préserver les 20 profils modifiés, rapports d’autre mission et `.tmp` préexistants ; ne pas les attribuer automatiquement à cette mission.
- Les trois anciennes fiches restent **deferred/non publiées**, avec correspondance vers leurs remplacements. Aucun faux `completed`, aucune réaffectation générique.

### Champs natifs

Le schéma découvert supporte `agent`, `context`, `worktree`, `depends_on`, `complexity`, `risk`, `report_mode` et `completion_policy`.

Les fiches ci-dessous sont **des actions du lead**, pas un board à auto-dispatcher. Le profil conseillé est indiqué en prose ; aucune affinité `agent: orchestrator` ne doit provoquer une redélégation du coordinateur à lui-même. Pour une éventuelle lecture indépendante seulement : profil effectivement disponible et vérifié, `context: {mode:"fresh"}`, `worktree:false`, `report_mode:"final_response"`. Les références locales doivent être remplacées par de vrais IDs uniquement si publication ultérieure justifiée.

## Fiches exécutables

### F1 — Inventaire et matrice de disposition

- **Objectif/livrable :** photographie datée des branches, HEAD, remote/main, worktrees, dirtiness et sessions ; chaque contribution classée : intégrée, équivalente, remplacée, unique à préserver, étrangère à la mission.
- **Type / profil conseillé :** investigation administrative ; coordinateur.
- **Entrées / ownership :** inventaire Git déjà entrepris par le lead, rapports156/157, reçus natifs. Possession du seul manifeste de fermeture sous le répertoire de rapports de cette mission.
- **Invariants :** aucun contenu perdu ; aucun fichier étranger ajouté au commit de mission. Un ancien commit porté autrement demande une preuve d’équivalence, pas seulement un test d’ascendance.
- **Vérification :** commandes de diagnostic proposées au lead : `git status --porcelain=v1`, `git branch -vv`, `git worktree list --porcelain`, `git log`, `git diff`, `git merge-base --is-ancestor`. Aucun secret de configuration distante dans le rapport.
- **Dépendances :** aucune.
- **Exclusivité :** photographier sous exclusion des mutations Git ; lecture du runtime sans boucle d’attente.
- **Contexte / worktree :** contexte actuel du lead utile et autorisé ; aucun nouveau contexte, `worktree:false`.
- **Estimation PM/LLM :** brute **46**, retenue **46**, risque **62** ; attribution et conservation difficiles, erreur potentiellement destructive. **30–60 min**, selon inventaire.

### F2 — Contrôle QA de fermeture et frontière native

- **Objectif/livrable :** checklist indépendante des préconditions shutdown/merge/cleanup, traitement des brouillons et preuve attendue d’archivage.
- **Type / profil conseillé :** QA administrative en lecture seule ; `qa-planner` fresh si déjà disponible et admissible, sinon constat de limite par le lead, sans admission automatique.
- **Entrées / ownership :** brief autonome contenant rapports156/157 et extraits non secrets du contrat natif actuel ; aucun ancien worktree à explorer par ce profil.
- **Invariants :** ne pas inventer `close`, d’option « préserver toutes les branches » ou de commande d’annulation ; aucun test, shell, édition ou délégation demandé au profil restrictif.
- **Vérification :** restitution native : conditions réalisables, refus éventuels, limites ; confirmer les paramètres et effets réellement exposés. Matrice QA I×P séparée des scores PM ; pour tout produit ≥10, expliciter déclencheur, portée, détection, mitigation et gap.
- **Dépendances :** indépendante de F1 pour l’analyse documentaire ; réconciliation avec F1 avant F3.
- **Exclusivité :** aucune mutation ; lectures parallélisables.
- **Contexte / worktree :** `fresh`, indépendance ; `worktree:false`.
- **Estimation PM/LLM :** brute **40**, retenue **40**, risque **56** ; effets destructifs indirects du cleanup. **20–40 min**. Prérequis : lecture et restitution effectivement permises.

### F3 — Préserver, arrêter et réconcilier les contributions

- **Objectif/livrable :** preuves et brouillons sauvegardés, workers arrêtés, contributions nécessaires intégrées nativement, disposition complète des branches historiques.
- **Type / profil conseillé :** administration/intégration ; coordinateur exclusivement.
- **Entrées / ownership :** F1 et F2 réconciliés ; références et espaces identifiés. Ownership temporaire des opérations natives workers et du manifeste.
- **Invariants :**
  1. Avant tout retrait possible : préserver preuves ignorées, diffs uniques et références, avec empreintes et vérification de restitution, dans un emplacement durable hors worktrees supprimables.
  2. Sauvegarde à liste blanche ; pas d’export massif de profils, credentials ou configuration sensible.
  3. `team_shutdown` sans force ; constater l’arrêt avant chaque `team_merge` nécessaire.
  4. Vérifier le diff après chaque intégration. Une branche stale n’est pas admissible parce qu’elle porte un ancien commit connu.
- **Vérification :** reçus natifs, snapshots avant/après, comparaison exacte des chemins acceptés et de leurs empreintes. Aucun test.
- **Dépendances :** F1+F2 : connaître ce qui sera perdu et les effets natifs avant action.
- **Exclusivité :** état équipe, branches, index Git, worktrees et sauvegardes ; opérations sérialisées.
- **Contexte / worktree :** coordinateur existant ; `worktree:false`, aucun worktree de développement.
- **Estimation PM/LLM :** brute **59**, retenue **59**, risque **78** ; perte de données ou réintroduction de code remplacé. **45–90 min**, hors blocage natif.

**Traitement minimal des brouillons remplacés :** ne pas les remettre dans les chemins actifs. Conserver leur contenu et provenance ; une archive inerte versionnée sur `main`, sans secrets et clairement exclue du produit livré, peut satisfaire la conservation de « tout le travail ». **Une sauvegarde seule ne prouve pas que ces brouillons sont mergés sur main.** Si leur intégration active est réellement exigée, ou si cleanup impose leur fusion sans voie sûre de neutralisation, demander uniquement cet arbitrage de périmètre ; ne pas écraser le candidat testé.

### F4 — Promotion finale sur main et push vérifié

- **Objectif/livrable :** tous les livrables retenus, preuves versionnables et disposition des brouillons committés sur `main`, puis publiés sur le remote attendu.
- **Type / profil conseillé :** administration Git ; coordinateur.
- **Entrées / ownership :** résultat F3 réellement présent dans le worktree principal ; HEAD et liste exacte des fichiers admissibles.
- **Invariants :** aucune contribution implicite non committée ; staging explicite ; pas de commit englobant les modifications étrangères ; pas de force-push. Préserver la dirtiness préexistante sans checkout destructif.
- **Vérification :** `git diff --check`, inspection des commits et arbres, preuve d’inclusion des livrables ; promotion autorisée de la branche principale après contrôle des divergences ; `git push <remote-validé> main`, puis comparaison de `git rev-parse main` avec `git ls-remote <remote-validé> refs/heads/main`. Un push refusé n’autorise aucun contournement.
- **Dépendances :** F3 : aucun consommateur ne suppose une intégration absente.
- **Exclusivité :** Git principal et publication distante.
- **Contexte / worktree :** lead existant, `worktree:false`.
- **Estimation PM/LLM :** brute **45**, retenue **45**, risque **70** ; publication erronée ou contamination de main. **20–45 min**.

### F5 — Cleanup natif et attestation terminale

- **Objectif/livrable :** sessions arrêtées, mission archivée/clôturée nativement, sauvegardes préservées, état final attesté.
- **Type / profil conseillé :** administration native ; coordinateur.
- **Entrées / ownership :** F4, manifeste F3, état runtime actualisé.
- **Contrat :** appeler `team_cleanup` uniquement lorsque toutes ses fusions potentielles sont démontrées inoffensives et toutes ses suppressions couvertes. Ni `force`, ni purge, ni `acknowledge_uncommitted:true` utilisé comme raccourci. Les modifications étrangères restent intactes.
- **Vérification :** reçu de cleanup réussi, état archivé et absence de worker actif ; absence d’altération inattendue de main/dirtiness. Si cleanup produit des changements inattendus, ne pas les pousser automatiquement et ne pas annoncer la clôture.
- **Dépendances :** F4 : publication et conservation prouvées avant retrait.
- **Exclusivité :** runtime équipe, worktrees et références Git.
- **Contexte / worktree :** lead existant, `worktree:false`.
- **Estimation PM/LLM :** brute **52**, retenue **52**, risque **82** ; suppression et fusion implicite. **15–30 min**, si préconditions satisfaites.

Le rapport versionné préalable doit décrire la fermeture préparée, sans prétendre qu’elle a déjà réussi. Après cleanup, le reçu terminal peut être remis dans la session native ; tout rapport final ajouté au dépôt exige un dernier commit/push et une nouvelle preuve d’égalité distante.

## Graphe, barrières et décisions

- **Parallèle réel :** F1 et analyse documentaire F2, sans mutation concurrente.
- **Chemin critique :** `F1 + F2 → F3 → F4 → F5`.
- **Barrières :** inventaire réconcilié ; sauvegardes restaurables ; shutdown confirmé ; diff de chaque merge accepté ; HEAD final sur main ; même SHA sur le remote ; cleanup attesté.
- **Points sensibles :** publication distante, retrait des worktrees/références, archivage natif.
- **Décisions restantes :** disposition exacte des brouillons uniques et capacité réelle du cleanup à éviter leur réintroduction. Aucun autre GO intermédiaire de routine n’est nécessaire.
- **Si une précondition échoue :** préserver et signaler précisément l’étape bloquée ; ne pas qualifier une mission encore `running` de clôturée.

**Tous les scores concernent de l’administration ou de la lecture : aucun plancher développement artificiel. Toute correction de code, script ou harness découvrirait une nouvelle tranche, non autorisée par ce plan.**