# 6 — Contrat integre ; corriger les droits QA avant admission

## Objectif et etape

Livrer le pipeline de verification Windows/Linux sans campagne moteur. Le contrat complet a ete ratifie et commite : .opencode/evidence/mission-uniform-pipeline/contract.md, commit 003ad9c5804a31523b46150579589abba3a30f65. Le contexte de preparation est commite separement en 8e86c87. La tache de contrat est terminee apres integration ; le lecteur architecte est arrete. Aucun auteur ni profil QA suivant n'est admis.

Decisions locales : dependances standard library seulement, modules adaptateurs Windows/Linux distincts pour paralleler sans conflit, deux routes principales et arguments C++ bornes plutot qu'un nouveau parseur JSON generaliste. Les sorties restent JSON versionne ; aucune API moteur changee.

## Controle d'admission et probleme concret

Le diff preexistant des profils avait ete exclu de la mission. Avant admission QA, le superviseur a interroge en lecture seule le serveur OpenCode 2.2.1 actif, via son CLI effectivement localise parmi les processus locaux, puis le schema OpenAPI servi.

Requete native agent.list, location ARTIST : profil qa-planner, mode subagent, contient les refus edit/shell/subagent, MAIS une regle edit:* allow ulterieure est egalement servie. Cette regle correspond au changement preexistant observe sur disque. La restriction d'edition n'est donc pas suffisamment garantie pour admettre ce profil comme lecteur restrictif. La simple instruction « ne pas ecrire » ne sera pas annoncee comme enforcement natif.

Les autres fichiers QA test-engineer et test-reviewer possedent le meme ajout dans le diff preexistant ; leur statut runtime reste a recouper avant leur propre admission. Aucun test d'ecriture, de shell interdit ni de secret n'a ete tente. Aucun refus contourne et aucune permission modifiee par API.

## Portee du blocage

Admission QA prealable seulement. Le contrat et la preparation restent acquis. Les auteurs attendent le plan QA integre par obligation du projet ; les lancer avant ce controle contournerait cette barriere. Il ne s'agit pas d'une panne des tests ou du moteur.

## Decision recommandee

Autoriser une correction minimale des trois seuls profils QA : retirer l'autorisation globale d'edition ajoutee en fin de permissions, conserver les droits de lecture/restitution et les autres modifications utilisateur. Le superviseur verifiera ensuite le profil effectivement servi ; aucune garantie de rechargement deduite du seul fichier. Si la voie native reste incoherente, admission maintenue bloquee, sans elargissement des droits.

Alternative : l'utilisateur corrige lui-meme les profils, puis le superviseur recoupe le runtime. Dans les deux cas, catalogue OpenAI, autonomie, niveau QA, limites de tests et cloud ne changent pas. Aucun cout cloud et aucun nouveau test moteur ne sont necessaires. Sans correction, le developpement reste en attente de cette precondition obligatoire.

## Annexe technique

Diagnostics reellement executes : git rev-parse/status, diff check, commits locaux des seuls documents propres ; Get-Process filtre opencode ; CLI natif api get /api/info ; api get /openapi.json ; api get /api/agent avec location ARTIST. Resultats API filtres dans Context Mode pour examiner id/mode/permissions uniquement. La commande opencode n'etait pas sur PATH ; l'executable natif observe est utilise sans installation.

Le premier git add du contrat a signale le repertoire evidence ignore. La preparation seule a ete commitee ; le contrat documentaire precis a ensuite ete ajoute explicitement avec -f et commite. Aucun autre contenu ignore ni changement de profil utilisateur integre. Pas de compilation, test ou benchmark execute.
