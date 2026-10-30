# 122 — Harnais valide et RED comportemental du helper observé

Le coordinateur a relu la correction de scope : état explicitement test-owned,
collision refusée et nettoyage en finally, sans root/tooling injectés. La
production et les pins restent inchangés. Les **7/7 baselines passent** désormais,
avec comparaison des chemins, code0/17, exception factice et audits de refus.

Le seul test prepare nominal est ensuite exécuté : **un test, un échec attendu**,
aucune erreur ou skip. Le refus actuel « non implémenté » est bien la cause,
avec événements vides, environnement synthétique intact, état de fixture nettoyé
et aucune erreur de harnais. Empreintes stables entre baseline et RED.

Le mapping proposé de l'ancienne garde est ratifié : admission fine prepare/build,
refus invalides/run avant effets, absence de fallback legacy et helper complet
jamais exécuté. Il ne s'agit pas de supprimer une assertion pour verdir.

Le GREEN de production attend encore le court plan de raccordement des feuilles
réelles annoncé dans la remise : tooling, MSVC, environnement, admission de l'arbre,
processus possédé et journaux. Les tests haut niveau remplacent la feuille enfant
et ne prouvent pas son câblage natif. Le lead demande les signatures, effets et
oracles bornés avant code, sans nouveau schéma ou capture backend. Aucun RED des
autres cas masqués n'est inventé.

## Preuves

- Candidat `44d3a0795414c2fb0d4fb24776014f4d1fd45ccb`, auteur pool-31 ; runner coordinateur.
- Commandes nominatives : `C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalHarnessTests`, puis `test_helper_functional.HelperFunctionalAdmissionTests.test_prepare_reaches_real_binding`, cwd tests Windows du worktree auteur.
- Baseline : `out/verification-synthetic/windows-helper-functional-baseline-v2-2026-10-30T08-20-00-800Z/`.
- RED : `out/verification-synthetic/windows-helper-functional-red-2026-10-30T08-20-15-769Z/` ; les journaux pointent vers les cas bruts du worktree auteur.

Seul PowerShell test-owned avec fragments pinés et feuilles factices est lancé.
Aucun helper complet, outil de build réel, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté.
