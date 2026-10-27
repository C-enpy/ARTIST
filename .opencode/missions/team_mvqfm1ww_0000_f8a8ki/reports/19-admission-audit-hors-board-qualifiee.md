# 19 — Audit hors board qualifié, exception héritée révoquée nativement

## Problème et résolution bornée

La validation exécutée reste acquise au périmètre du rapport18. L'audit final ne
pouvait pas commencer par la voie Ensemble essayée : les workers autonomes exigent
une tâche attribuée, alors que le contrat ARTIST exige cet audit hors board.
Aucune tâche d'audit n'a été publiée pour contourner cette exigence.

Le lead a employé la session native directe du profil test-reviewer exposé, avec
la sélection native OpenAI/Sol high, sans override de modèle. Une qualification
de lecture seulement a précédé l'audit, sans historique d'auteur ni conclusion
de reviewer. Lectures relatives AGENTS et entrée neutre réussies, réponse finale
directe réellement reçue. L'essai team_message hors équipe a donné le refus
connu ; il ne doit pas être répété et la réponse finale est la voie retenue.

Le contrôle natif a révélé une exception de session browser/*/allow héritée.
L'admission a été bloquée avant l'audit. La documentation publique V2 expose
session.update pour modifier le seul champ permissions. Le lead a révoqué cette
unique exception avec permissions=[], puis relu la session : profil, modèle et
racine inchangés, plus aucun override. Aucun grant, demande d'autorisation,
modification de profil, règle globale, harness ou source n'a été effectué.

La réponse PATCH était vide ; une tentative de la parser comme JSON a échoué.
Le lead n'a pas répété la mutation : GET a confirmé la restriction appliquée.
Cette vérification n'est pas une affirmation de hot reload, sandbox universelle
ou test exhaustif d'enforcement ; elle établit l'état natif et la restitution
observés de la session effectivement utilisée avant l'audit.

## Preuves et suite

- Session directe `ses_e824af8b9ffe0vrlb3N50WZoXE`, agent natif test-reviewer,
  openai/gpt-6.1-sol high, racine canonique ARTIST, permissions de session[].
- Contexte de qualification fresh : neuf événements, une entrée user ; trois
  lectures completed, finale `msg_17db54e39001gKxSls92BX6fv7` stop/completed.
- Profil natif résolu restrictif déjà interrogé, aucune édition/shell/subagent
  autorisée par le contrat ; aucune commande de test effectuée par la QA.
- Preuve de restriction :
  out/qa-prepared-float/lead-runner-20261027/audit-admission-restriction.json.
- Contrat public consulté : https://opencode.ai/v2/docs/api et OpenAPI V2,
  PATCH /api/session/{sessionID}, operation session.update, champ permissions.

La même session peut recevoir maintenant le mandat d'audit neutre. Son résultat
reste attendu et ne vaut pas PASS par la seule qualification. Aucun nouveau GO
utilisateur ni chantier d'outillage nécessaire : seuls les droits excédentaires
de cette instance ont été réduits par la voie native documentée.

[Entrées d'audit](../final-audit-input.md) ·
[Validation intégrée](18-validation-integree-normale-et-asan.md).
