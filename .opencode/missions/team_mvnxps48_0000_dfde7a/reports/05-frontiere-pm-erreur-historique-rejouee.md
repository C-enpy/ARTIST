# 05 — Frontière PM récupérée : erreur historique rejouée

2026-10-25. Diagnostic lecture seule du transport/préparation. Aucun nouveau resume, import, worker, changement runtime ou démarrage.

## 1. Résultat et portée

L'entrée recherchée est désormais récupérée par l'API native publique de contexte de la session PM. **Le premier rejet planning.reason est causal ; les messages de correction suivants réemploient ce diagnostic historique avec des candidats désormais conformes.** Ce n'est pas une preuve de perte des cinq chaînes au transport ni un problème de droit d'écriture du PM.

Le diagnostic INCONCLUSIVE du rapport04 est levé pour cette comparaison des messages et du schéma. La version exacte en mémoire ayant traité chaque rejet initial n'est pas attestée par une capture instrumentée ; les constats ci-dessous reposent sur messages natifs, chargement observé et replay du parseur installé.

## 2. Méthode et provenance

Documentation API V2 consultée : `https://opencode.ai/v2/docs/api`.

Commande réelle en lecture seule, exécutée depuis Context Mode avec arguments en tableau, sorties brutes gardées en mémoire uniquement :

`C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.1.4-x64/opencode-cli.exe api get /api/session/ses_e8c54a903ffeiXdD07VFjUaCZR/context`

Exit0 ; réponse `data` analysée sans publication du mandat privé. Utilisation en mémoire des fonctions `assistantBody`, `parseJson2` et du bloc `parsePreparationDocument` de la version configurée/chargée **20261025-pm-preparation-da5345a2385c** (SHA index.js au rapport04). Aucun import du plugin avec effets de bord, aucune base interne consultée/modifiée.

Pour chaque prompt natif de correction, extraction de son `Rejected candidate:` jusqu'au marqueur du contrat JSON, puis comparaison exacte avec le texte assistant natif correspondant. Les trois comparaisons sont identiques après retrait du seul whitespace d'enveloppe. Les SHA ci-dessous portent le corps texte assistant complet, pas un JSON resérialisé.

## 3. Résultats discriminants

### Premier candidat : erreur réelle

- Assistant `msg_173ac625a001t3S3W3jMmI938F`.
- SHA256 `bb3643a8ecfce9f895f286a0261cef5799b4588433989ad17940f003544007e8`.
- Cinq tâches H3/ASAN/QA-PORTABILITE/AUDIT/CLOTURE ; leurs `planning.reason` sont tous **undefined**, pas des chaînes.
- Parseur installé : rejet exact `tasks[0..4].planning.reason must be a string` (cinq lignes séparées).
- Correction native `msg_173aed038001GgOwRU63lE2KHh` contient exactement ce candidat et ce diagnostic. Rejet justifié.

### Deuxième candidat : corrigé, schéma valide

- Assistant `msg_173aed05d0014ZB458xdrEXNt9`.
- SHA256 `172237331ca7608590a265c0435ee7bac510a80b4ba1a0e11e125c0777335fe6`.
- Les cinq raisons sont des chaînes ; longueurs brutes/trim identiques, dans l'ordre : **120,120,140,137,154**.
- `parsePreparationDocument(parseJson2(body))` : **PASS**, document version2, readiness **blocked**, cinq tâches.
- Correction suivante `msg_173b6f431001v3Cy3x1G9Og85p` embarque exactement ce candidat valide mais répète les cinq anciennes erreurs « must be a string ».

### Troisième candidat : valide également

- Assistant `msg_173b74a0e0011EgyWCtxiYsYq5`.
- SHA256 `f2a0080004b941ff712d55174640b98132f7b6c52ecec925ccc57eb775987769`.
- Cinq chaînes, longueurs brutes/trim **79,90,98,109,110**.
- Même parseur : **PASS**, version2, readiness **blocked**, cinq tâches.
- Correction `msg_173ba862c001n4YsEHQZoGyg4c` contient exactement ce candidat, mais encore les cinq anciennes erreurs.

### Réponse de diagnostic suivante

- Assistant `msg_173bafbde001AhrJm0gpgwMsVZ`, SHA256 `1b027aac15de6f927d2857284ad3fd3f7b0708c81f973eb594041661290178ca`.
- Version2, readiness **blocked**, zéro tâche ; parseur **PASS**.
- Une réponse syntaxiquement valide et blocked ne constitue pas un plan ready et n'autorise aucun import/exécution.

## 4. Cause dans le code observé

Version da5345a2385c, `index.js` :

- `importReply`, lignes20802–20811 : parse le document ; pour un document non ready, met à jour **readiness et candidate_json**, puis renvoie un résultat paused. **errors_json n'est pas réinitialisé dans ce chemin.**
- `runBoundedLoop`, lignes20894–20900 : recharge **errors_json** et le candidat courant ; si l'historique d'erreurs n'est pas vide, construit un `correctionPrompt` avec sa dernière erreur et le candidat actuel.
- Lignes20913–20919 : l'historique d'erreurs est alimenté après un vrai rejet de schéma. Il demeure distinct de la mise à jour non-ready décrite ci-dessus.

Ces chemins expliquent les observations : erreur du premier candidat conservée, candidat remplacé par un document valide mais bloqué pour d'autres raisons, puis ancienne erreur présentée comme raison de correction du nouveau candidat lors de la reprise. Aucun besoin de modifier planning.reason à nouveau. Ce diagnostic est plus précis que « version différente » ou « champ perdu ».

## 5. Conséquences pour la préparation

Ne plus demander une réécriture des cinq champs ni prétendre que le candidat corrigé échoue à leur validation. Les blocages métier du handover et readiness sont distincts du schéma. Appliquer le cadrage du rapport04 : équipe historique = provenance, Linux = analyse sans campagne implicite, audit = gate natif à rendre explicite, pas rattachement ancien à imposer.

**Aucune correction du plugin engagée ici.** Une réparation de sa reprise exigerait une tranche séparée avec test : premier candidat invalide → second valide mais blocked → resume ne doit pas qualifier l'ancienne erreur de rejet actuel. Ne pas effacer l'historique, fabriquer readiness ready, modifier la base interne ou contourner l'approbation pour débloquer l'import.

Clarification pour le prochain resume natif : lire ce rapport et04 ; l'erreur planning.reason est historique et résolue, les deux candidats corrigés passent le schéma. Ne pas la corriger de nouveau. Préparer les décisions réellement restantes, sans assimiler schéma valide à plan exécutable. Aucun start au nom de l'utilisateur.
