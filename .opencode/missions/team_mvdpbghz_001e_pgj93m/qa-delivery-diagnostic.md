# Q — diagnostic de restitution native, 2026-10-18

Diagnostic demandé par l'utilisateur : aucun worker actif dans l'UI alors que
le coordinateur annonçait Q en cours. Lecture seule du runtime et de ses messages,
aucun restart, relancement de worker, changement de politique/permission ou base.

## Faits observés

- team_status : 0 running, 0 starting ; pool-2 idle, aucun message d'équipe ;
  tâche task_mvdqhtoe_001j_edgftw encore in_progress.
- team_view pool-2 : session ses_eb1423176ffeiluzzxpHDO6TeQ, status ready.
- API native session : agent qa-planner, cursor/grok-4.7,
  variante Grok 4.7 Extra High (politique approuvée respectée).
- Message msg_14ecc836a001ntwPgJ0voVwcDF : plan QA, finish stop,
  terminé 2026-10-18T11:44:08.702Z ; texte 32528 caractères,
  SHA256 30a80ea78c282c8ba6d08931921a9f31a9d8905186f6e0478092b53ec5f23776.
- Message msg_14ed38a9f0015I3QUA63okelE1 : annonce de transmission au lead,
  finish error, terminé 2026-10-18T11:47:06.160Z ; erreur provider.unknown :
  Cursor semantic-progress timeout after 120000ms after visible output or
  stateful server activity; automatic retry unsafe (attempt 1/3).
- Message msg_14ed646e4001X4QKwJz3l6EDTj : complément final du plan,
  finish stop, terminé 2026-10-18T11:48:45.789Z ; texte 5105 caractères,
  SHA256 1256465a52d7f1cfd64b3336b3417457c9a16be5620c30c733ad4f14b4f6ec23.
- Événement idle msg_14ed7c46c0011u3q0kUpIx4FiM : outcome succeeded.

Les empreintes portent sur les parties textuelles assistant concaténées par LF,
pas sur une exportation JSON complète. Aucun secret/identifiant d'auth exporté.

## Conclusion bornée

L'UI montrant zéro worker actif est cohérente avec l'état natif. Le board seul
ne prouve pas l'activité. Le plan existe en réponses finales, mais n'a pas été
livré au transport d'équipe ni clôturé sur le board. Un timeout Cursor est
reproduit dans l'historique de transmission ; le lien causal complet avec la
non-clôture du board n'est pas démontré. Pas de preuve d'un bug d'affichage UI.

Le profil QA prévoit la réponse finale à relever par le lead (lignes120–125),
sans ouvrir execute/team_*. Le diagnostic utilise uniquement des GET natifs
par le lead, pas un contournement de ces permissions. Autonomie QA inchangée.

## Accès et commandes réellement employés

Skill opencode ; documentation V2 API et troubleshooting chargée via Context Mode.
`where.exe opencode` : absent PATH. `Get-Process *opencode*` : binaire CLI trouvé.
CLI : C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.0.32-x64/opencode-cli.exe.
Appels via Context Mode, sorties filtrées :
- service status ;
- api get /openapi.json (découverte des routes GET) ;
- api get /api/session/ses_eb1423176ffeiluzzxpHDO6TeQ ;
- api get /api/session/ses_eb1423176ffeiluzzxpHDO6TeQ/message?limit=8&order=desc ;
- même liste avec limit=12 pour structure/contenu des messages ;
- GET des trois messages identifiés ci-dessus pour horodatages/empreintes.

## Suite nécessaire, non exécutée par ce diagnostic

Relever le plan et son complément, relire les gaps/arbitrages, persister via
outil d'édition puis commit lead avant auteur. Ne pas relancer Q pour recréer
un livrable déjà présent. Ne pas qualifier le plan de validé/commité sur la seule
présence de texte. Aucun RED/GREEN ni résultat GPU nouveau. Tâche auteur bloquée.

## Diagnostic structurel complémentaire

Inspection en lecture seule du plugin chargé :
`C:/Users/djden/.config/opencode/local-plugins/ensemble/20261016-mission-policy-5504fe8e8df8/index.js`.
Recherche exacte de trois littéraux, sans modification du plugin :

- offset 567406 : le prompt de fin de tâche demande
  `mark it complete using team_tasks_complete` et un rapport par `team_message` ;
- offset 568576 : `Your plain text output is NOT visible to the team. You MUST
  use team_message to communicate.` ;
- offset 822921 : en absence de rapport, `nudgeMember` émet
  `[System]: You completed your work but did not report results. Send your
  findings to the lead via team_message now.`

Ce dernier rappel est présent dans la session QA sous
`msg_14ed38a2c001R4bu4NP4Vzv3N0`. Il précède le timeout de transmission déjà relevé.
L'inspection du code montre un garde `nudgedMembers` et un contrôle
`hasReportedCompletion` : il ne s'agit pas d'une preuve de boucle infinie.

En parallèle, le profil `.opencode/agents/qa-planner.md` refuse explicitement
execute/shell/subagent et n'autorise que read/glob/skill. Ses lignes120–125
prévoient le relais par le lead d'une réponse finale si le transport manque.
Le brief Q appliquait ce relais. L'injection générique du plugin et son rappel
de fin ne respectent donc pas ce protocole de restitution particulier.

Conclusion prouvée : incompatibilité du protocole de clôture Ensemble avec ce
profil QA, plus board non clôturé malgré une sortie finale présente. La cause
interne du timeout Cursor n'est pas démontrée par cette contradiction seule.
La même déclaration restrictive existe pour test-engineer et test-reviewer :
risque structurel partagé, pas reproduction native effectuée sur ces deux rôles.

Deuxième cause de latence : le lead a initialement confondu in_progress du board
et activité réelle, puis a rendu la main sans relever la réponse finale. Cette
erreur de coordination est distincte du timeout et d'un éventuel défaut d'UI.

## Mesures bornées

Pas de nouvel appel QA pour régénérer le plan ; pas de nouvelle permission,
configuration globale, redémarrage de service ou patch plugin. La voie locale
déjà prévue est le relais lead de la réponse finale avec provenance/empreintes.
Le plan doit garder ses décisions humaines ouvertes avant l'auteur ; la présence
du livrable ne suffit pas à autoriser ses propositions de réduction de preuve.
Une correction durable du plugin exige une mission distincte et des tests de
non-régression du protocole de restitution, pas un assouplissement du deny-all.
