# Limite d'admission QA dans la mission isolée

Date : 2026-10-16. Candidat intégré `0727bbcf37e012a9c5945e2dfe69546971c3417f`.
Deux GET natifs authentifiés, exécutés par le lead avec le CLI découvert :
`api agent.list --param location[directory]=<root>`.

- Répertoire propriétaire de la mission `C:/Users/djden/source/repos/ARTIST` :
  **18 profils résolus**, qa-planner/test-engineer/test-reviewer absents.
- Worktree d'intégration `.../gpu-qa-integration` : **30 profils résolus**, les
  trois QA présents (23 locaux + 7 natifs).
- Raw catalogue du candidat :
  `out/agentic/mission-gpu-qa/lead-t13/native-agents-0727bbc.json`.

Le plugin a déjà refusé team_spawn depuis le worktree d'intégration en exigeant
le répertoire propriétaire (voir isolation-cleanup.json). Aucun fichier n'est
copié sur main pour contourner cette limite et aucune nouvelle mission/politique
ou grant n'est créé. Les trois profils QA livrés gardent leurs 27 règles exactes,
execute/shell/edit/subagent fermés, sans exception de transport ajoutée.

Décision de coordination conforme au fallback de profil non exposé : le travail
complémentaire T14 peut être préparé par un spécialiste disponible, appliquant
le contrat méthodologique complet du test-engineer en lecture seule/proposition
texte, puis persistance et tests relus exécutés par le lead distinct. Cette
substitution n'est **ni un lancement du profil QA natif ni sa preuve d'enforcement**.
Ne pas désigner ce spécialiste comme un runner QA autonome ; ne pas changer les
trois profils ou leurs droits pour lui donner les mêmes outils.

Le rapport doit attribuer séparément profil exécuté, méthode appliquée, auteur
de proposition, personne qui la persiste et runner réel. Une restitution bloquée
reste blocked : aucune extension team_*, session API ou MCP pour la débloquer.
Les canaris et exécutions des vrais profils dans un contexte d'admission adéquat
restent NOT_RUN/INCONCLUSIVE et ne sont pas remplacés par le fallback. La revue
finale fresh aura la même limite de profil native si le canonique n'a pas intégré
ces définitions ; elle doit être signalée, sans auto-revue prétendument indépendante.
