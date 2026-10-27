# 20 — Audit reçu : deux frontières négatives à couvrir avant clôture

## Résultat et décision du lead

L'audit test-reviewer fresh hors board conclut MODIFY/INCONCLUSIVE pour la
clôture complète. Aucun défaut de production reproduit ni finding haut. Les
579 tests normaux/ASan restent recevables dans leur périmètre ; ils ne couvrent
pas à eux seuls deux frontières négatives explicites du contrat.

Le lead retient les deux gaps et poursuit la boucle de tests déjà autorisée,
sans modifier la production ni demander de nouveau GO. Aucun changement
d'architecture, d'objectif, de budget/routage ou d'infrastructure n'est proposé.

## Gaps acceptés, scénarios discriminants avant code

1. **Contexte lors de la préparation.** Les refus existants visent set après une
   préparation valide. Un mutant supprimant requireContext dans prepare pourrait
   survivre. Ajouter deux préparations sous contexte nul/autre : refus avant
   glGetProgramiv, sans mutation du miroir ni publication d'accès ; restauration
   du bon contexte puis préparation/usage nominal. Sources :
   PreparedFloatUniformTests.cpp et PreparedFloatUniform.hpp:47.
2. **Retrait après détachement partiel.** Le test d'accès préparé échoue à delete
   sans shader ; ceux des shaders partiels ne retiennent pas d'accès préparé.
   Un mutant déplaçant retirePreparedUniforms après la boucle de détachement
   pourrait survivre. Ajouter un accès préparé au scénario existant : premier
   shader détaché, second wrapper déplacé provoquant le rejet ; refus stale répété
   sans query/upload ni mutation du miroir. Sources : Freer.hpp:35–47 et
   ShaderLifetimeTests.cpp:630–694. Ce sont des gaps inférés, pas FAIL exécutés.

Autres obligations : publication, trois updates sans C/L/A, target/decoy et pixels
indépendants non vides PASS bornés de l'audit. Identité finale recevable, historique
RED incomplet inchangé. Aucune reconstitution de cet historique demandée.

## Contrat de la boucle corrective

Le generator propose uniquement des patchs texte ciblés et oracles, après lecture
de ce candidat intégré. Le lead persiste/relit/applique et exécute séparément.
Pas d'édition production, de nouvel helper global ou d'affaiblissement d'assertion.
Préférer les helpers et scénarios déjà présents ; si un nouveau chemin de test
est nécessaire, le signaler avant application. Limite healer : une réparation
test-only déjà consommée sur deux ; un bug production reste rouge et est retourné
au propriétaire. L'ajout de ces deux scénarios n'est pas une réparation de moteur.

PM/LLM local pour ce complément de tests : complexité brute66/retenue70, risque72.
Graphe : propositions QA → persistance → runner/tests et preuves renouvelées →
ré-audit de la même session indépendante. Une seule écriture par fichier, builds
et GL sérialisés. Les sources sont encore 3d0e145 ; les documents de photographie
en préparation ne prétendent pas que l'audit est déjà accepté.

## Traçabilité

Auditeur direct qualifié : session `ses_e824af8b9ffe0vrlb3N50WZoXE`, profil natif
test-reviewer, OpenAI high, permissions de session[] contrôlées après continuation.
Retour final reçu réellement par l'outil subagent ; aucun test/edit/délégation.
Entrées neutres : final-audit-input.md, contrat et plan QA ; pas de conclusions
d'auteurs, de generator ou de board données pour cet audit.

Les findings moyens sont à traiter, pas à transformer silencieusement en PASS.
Les limites Linux/Release/autres API/leaks/perf et RED historique restent DEFER.
[Validation précédente](18-validation-integree-normale-et-asan.md) ·
[Admission effective](19-admission-audit-hors-board-qualifiee.md).
