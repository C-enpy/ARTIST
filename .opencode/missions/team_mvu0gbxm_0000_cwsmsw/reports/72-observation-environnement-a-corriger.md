# 72 — Corriger l'observation, pas restaurer l'environnement

## Résultats de la cellule en lecture seule

Le premier diagnostic compare le getter assigné, un délégué .NET typé et un snapshot neuf du processus. Le getter initial donne null, puis les deux observateurs indépendants indiquent une clé présente et vide. Le script conserve cette contradiction et n'appelle pas la fonction cible, conformément à sa garde initiale. Code 0 ne signifie pas que l'expérience de fonction est réalisée.

Une seconde cellule, fondée sur une hypothèse nouvelle, admet la fonction uniquement si le getter typé et le snapshot s'accordent sur présence et vide ; le résultat initial null reste dans la trace, sans conversion. Avant, dans et après la fonction, ces deux observateurs concordent : clé présente, valeur vide, témoins non vide et absent corrects. Le getter assigné est lui aussi devenu concordant après le setup.

Ces preuves corrigent l'interprétation trop forte d'une disparition réelle à l'entrée de fonction : les traces antérieures établissaient une différence d'observation, pas une suppression du bloc ENV. Le mécanisme exact de liaison PowerShell ou l'effet du setup n'est pas déterminé ; aucune mutation/restauration n'a été effectuée.

## Décision locale

Le coordinateur autorise une correction bornée de l'observateur du harnais : snapshot neuf du processus, puis extraction des seuls marqueurs connus avec présence/valeur exactes. Le getter ambigu est remplacé, sans assimilation du vide à l'absence, sans dump des autres clés, sans PInvoke et sans mutation d'environnement. L'assertion de non-mutation avant/après reste inchangée.

Le propriétaire doit présenter ce diff et la précondition de caractérisation avant relance. Les getters du produit restent la correction séparée déjà relue. Aucun nouveau GREEN ni réception Windows n'est prononcé par ces diagnostics.

## Preuves et portée

- Proposition auteur `17597662efa8a47efb1452792d0b13666d44db4a`, `READONLY_ENV_PROPOSAL.md`.
- Diagnostic lead : `.opencode/evidence/mission-uniform-pipeline/diagnostics/windows-env-readonly.ps1` ; valeurs publiées limitées à booléens/longueurs des marqueurs factices.
- Première cellule : `out/verification-synthetic/windows-env-readonly-2026-10-30T03-58-25-449Z/`.
- Suivi à garde indépendante : `out/verification-synthetic/windows-env-readonly-snapshot-gate-2026-10-30T03-59-10-257Z/`.
- Empreintes du script, commandes et traces complètes conservées dans chaque manifeste. Intervention diagnostique locale évaluée PM/LLM brute 45/30, complexité retenue 70, risque 30 ; pas de changement de politique.

Aucun produit, helper, ProcessStartInfo, enfant applicatif, compilation, moteur, OpenGL, rendu, benchmark, installation ou cloud exécuté.
