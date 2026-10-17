# Intégration T04-R — preuves et incompatibilité restante

Date : 2026-10-16. Producteur : fix-harness-review, commit
`d3dd07e26b5cfc84ac2dfb31b467768fc5e59850`, parent
`3e3445f795ae11bff9212ab7b0ff5ea2b0d87dad`. Huit fichiers dans son allowlist,
shutdown puis team_merge. Ancien rapport T04 historique conservé.

## Recoupement du lead

Après lecture du runner, helper Git et surfaces d'exécution/nettoyage des tests :

`node .opencode/tests/agentic/t04-harness/run-tests.mjs all`

Exit0, **103/103**, zéro échec/skip, aucun répertoire `.run-*` restant. Exécutant :
lead, non profils QA. Log `out/agentic/mission-gpu-qa/t04r-lead-green.tap`, SHA256
`efa37438244115fb4fdc2faea49c140463c18adea4d82f7a4469cfa6329c26af`.
Cela recoupe le GREEN du producteur, pas ses RED historiques par une nouvelle
exécution, ni une certification indépendante de toutes les voies d'environnement.
RED F1–F5, identités et sentinelles sont consignés dans `t04-fix-result.md`.

## F6 — incompatibilité exécutée du vrai consommateur T05

Commande discriminante du lead :

```text
node .opencode/scripts/validate-agentic.mjs --mode offline --root <integration-root> --manifest .opencode/tests/agentic/t05-qa/manifest.json
```

Observé : **exit1 / FAIL**, PERMISSION_QA échoue pour qa-planner, test-engineer et
test-reviewer ; seules exceptions incriminées : `read:*` et `glob:*`.
Log JSON : `out/agentic/mission-gpu-qa/t04r-real-t05-offline.json`.

Cause : T04-R a choisi un sous-ensemble conservateur à fichiers exacts, sans
prendre en charge le template T05 approuvé (DENY-ALL, lecture/glob, skill exact,
refus secrets de toutes les casses et external_directory, aucune édition,
exécution ou délégation). T03 §7 exigeait de préserver les protections héritées
si read:* était utilisé ; le ruling humain autorise le runner séparé, pas un
grant d'écriture. Le message « D-PERM non résolu » est également périmé : le mode
est décidé, son enforcement reste non prouvé.

Les 103 fixtures unitaires vertes ne deviennent donc pas un PASS d'intégration.
Il faut arbitrer explicitement le contrat du contrôle statique : reconnaître un
template de lecture sous gardes ou documenter une précondition manquante, sans
prétendre simuler la sandbox native. Aucun refus global non protégé, réouverture
de secret, shell/Code Mode/MCP mutateur ou écriture ne doit être blanchi.

Décision lead : consultation architecte bornée avant correction F6 ; RED causal
sur le template réel et mutations de ses gardes, pas d'assouplissement aveugle.
T13 non publié et revue indépendante finale requise. Aucun GPU, gain ou canari
natif d'enforcement exécuté dans ce recoupement.
