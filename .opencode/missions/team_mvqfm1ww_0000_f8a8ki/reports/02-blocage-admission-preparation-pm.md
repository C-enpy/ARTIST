# 02 — Plan technique acquis, raccord d'admission encore bloqué

## Objectif et état

Préparer un accès OpenGL float une fois, puis l'utiliser sans recherche répétée,
avec invalidation sûre, consommateur existant et résultat graphique vérifié.
Le contrat et le choix du consommateur ont déjà été adoptés. Il ne manque pas
une nouvelle décision d'architecture ou une autorisation générale de réparation.

L'annonce native de préparation bloquée a été vérifiée contre le candidat complet
de la session PM, et non utilisée seule. Six tâches décrivent la fermeture QA,
le mécanisme, le consommateur/pixels, la revue lifetime, les gaps et la documentation.
Le candidat reste blocked ; aucune tâche importée, publiée ou exécutée par le lead.
La politique OpenAI/long_run/proportionate présentée n'est pas modifiée ici.

## Faits vérifiés et distinctions

1. Le catalogue team_tasks_add expose agent, context, complexity et risk pour les
   nouvelles tâches ; execution_decisions pour les tâches existantes ne modifie
   pas leurs scores, leur isolation ou leur mode de restitution. Le champ
   require_context demandé par ARTIST n'est pas exposé.
2. Le catalogue team_spawn expose context, worktree et report_mode final_response.
   Sa description déclare préserver les permissions natives des profils
   restrictifs. Cette présence est une procédure disponible, pas une observation
   de profil effectivement chargé ni un essai de restitution dans cette mission.
3. L'import de préparation décrit ne transmet pas scores/worktree/report_mode.
   Aucun raccord autorisé n'a été démontré pour fixer tous ces paramètres sur
   les tâches importées avant admission automatique. Ni leur écriture dans un
   handoff ni un futur spawn supposé prioritaire sur l'auto-dispatch ne le prouve.
4. La qualification QA effective est un livrable préalable aux auteurs, déjà
   décrit dans la première tâche. Son absence aujourd'hui n'est pas à elle seule
   une lacune du plan technique. Elle reste néanmoins une barrière réelle :
   ancienne remise conditionnelle non promue en admission acquise, lecture et
   restitution directes historiques distinguées des permissions/profil/fresh.
5. La procédure QA envisageable, conditionnée à un raccord d'admission sûr, est
   un lancement natif du profil qa-planner avec contexte fresh, worktree false
   et final_response, puis contrôle natif du profil/contexte/permissions,
   lecture locale et restitution réellement reçue avant analyse/auteurs.
   Aucun worker créé pour l'essayer ici ; aucun team_message hors équipe répété.

## Points à corriger lors d'une reprise utile

- Garder le blocage d'admission-native tant que le raccord n'est pas prouvé.
- Traiter la qualification QA comme obligation de la première tâche et barrière
  avant auteurs, pas comme preuve déjà obtenue ni décision utilisateur manquante.
- Le groupe parallèle du candidat déclare isolation separate-worktrees tout en
  demandant worktree false pour les deux lecteurs. Ne pas redéfinir cet enum par
  prose : conserver le parallélisme des lectures dans le plan, hors d'un groupe
  d'isolation incompatible, ou fournir une stratégie native réellement cohérente.
- Les consommateurs d'auteurs isolés et la documentation finale restent non
  publiés jusqu'à intégration effective et mise à disposition de leurs entrées.
  Le graphe complet n'est pas une autorisation de tous les importer en exécutable.

L'oracle proposé par le PM est concret (FBO RGBA8 32×32, triangle existant,
f2Scalar 0/0.5, samples intérieurs/fond) ; il doit encore être réconcilié dans
le plan QA avant auteur. Ce n'est ni un test exécuté ni un motif pour refaire
l'étude d'architecture.

## Recommandation et conséquences

Ne pas demander start sur ce candidat bloqué. Ne pas fabriquer ready, modifier
la base interne, supprimer les exigences ARTIST, désactiver l'auto-dispatch ou
ouvrir les permissions QA. Aucune réparation du harness dans ce mandat.

Une clarification PM peut réduire les blocages aux informations réellement
manquantes et corriger le groupe parallèle, mais ne crée pas le raccord absent.
Éviter les reprises répétées ou une nouvelle enquête identique pour espérer le
débloquer. Le lead n'a ni auto-resume ni invoqué start. Une voie native conforme
ou une évolution explicitement autorisée des exigences serait nécessaire pour
franchir cette limite ; aucune exception n'est présumée ou demandée par défaut.

## Preuves techniques et limites

- Session native PM : ses_e8375229fffe2i54YJk4AVWb5i.
- Dernier candidat public lu : msg_17c8d7cd8001l4uSTFBrrOBLo0,
  JSON version2 de34487caractères,6tâches,2questions supervisor bloquantes.
- Lecture native par CLI existante : opencode-cli.exe api get
  /api/session/ses_e8375229fffe2i54YJk4AVWb5i/context ; réponse data de77messages.
  Texte public assistant sélectionné par le schéma Session.Message.Assistant/Text,
  reasoning et outils exclus. Candidat complet parsé et fiches lues.
- Documentation V2 API et schéma public consultés via Context Mode ; première
  extraction de style role/parts inapplicable à la forme V2, corrigée depuis le
  schéma sans mutation de session. Pas de refus de permission ou panne inventé.
- Catalogue découvert en lecture seule : team_tasks_add et team_spawn ; aucune
  tentative d'appel mutateur, classification, admission ou correction d'outillage.
- Pas de build, test moteur, preuve GPU/image ou requalification native QA nouvelle.
