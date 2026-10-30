# 68 — Le vide devient absent dès l'entrée de fonction observée

## Expériences et résultats

Le diff des getters produit/harnais est relu : substitutions explicites seulement, assertions conservées. Aucun nouveau GREEN produit n'est exécuté à cette étape.

La sonde proposée par l'auteur échoue sur sa précondition avant l'expérience, sans écrire ses instantanés : résultat INCONCLUSIVE, pas un RED produit. Aucun témoin conditionnel de cette sonde n'est lancé.

Pour obtenir une information nouvelle, le coordinateur reprend le diagnostic minimal déjà fonctionnel et ajoute deux instantanés dans la fonction pure. Le vide est présent initialement et après affectation directe, mais déjà observé absent à l'entrée de la fonction, avant son retour. Un seul témoin conditionnel appelle ensuite le même corps par dot-sourcing dans un PowerShell frais : même résultat. Les témoins non vide et absent restent corrects ; les deux cellules terminent avec code 0.

Le dot-sourcing n'est donc pas un contournement démontré ; aucun remplacement massif de callsites n'est autorisé. Ces observations ne suffisent pas encore à distinguer la disparition réelle dans l'environnement du processus d'une particularité d'observation/transport des chaînes vides dans PowerShell. Aucun correctif d'environnement n'est attribué au produit sur cette base.

## Suite limitée

Le propriétaire doit proposer un discriminant en lecture seule entre état natif et observation PowerShell, sans restauration d'environnement, PInvoke de mutation, nouvelle dépendance ou installation. La sonde de précondition doit aussi conserver ses états booléens avant un éventuel arrêt. Les assertions vide/absent restent inchangées ; aucune nouvelle série complète de cellules n'est relancée automatiquement.

## Preuves

- Sonde auteur, candidat `2e5b18a21c12f6216a4db4fbe0bfc02b66ba1717` : `out/verification-synthetic/windows-return-boundary-2026-10-30T03-41-39-929Z/`.
- Diagnostic lead instrumenté : `out/verification-synthetic/windows-return-inline-diagnosis-2026-10-30T03-42-43-811Z/`.
- Témoin dot-sourcing : `out/verification-synthetic/windows-return-dot-diagnosis-2026-10-30T03-43-00-307Z/`.
- Script lead : `.opencode/evidence/mission-uniform-pipeline/diagnostics/windows-env-getters.ps1`, instantanés inline et mode dot ajoutés ; identités exactes dans les manifestes.

Aucun produit, helper, enfant applicatif, moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté par ces diagnostics.
