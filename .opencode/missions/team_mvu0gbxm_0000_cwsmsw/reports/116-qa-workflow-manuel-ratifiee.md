# 116 — Workflow manuel conservé et plan QA complet

La rectification du planner retire l'omission du workflow : le livrable explicite
du contrat est conservé. Le coordinateur ratifie les contrôles B5/B6 au §13 :
dispatch manuel seul, machines préparées self-hosted, entrées littérales bornées,
cible matérielle exacte, prédécesseur absent refusé sans cascade et conservation
des codes/bruts. Un upload ne constitue jamais un succès des tests.

Le plan complet est commité avant auteur de cette branche. CMake/workflow peut
désormais avancer indépendamment du helper Windows : fichiers distincts,
aucun arbre build ou outil natif lancé. Les détails de transport du workflow
doivent être proposés avec les tests RED avant implémentation, sans nouveau
framework, schéma public ou installation de parser YAML.

Le workflow ne sera pas déclenché pendant cette mission et ne provisionnera
aucune ressource cloud. Ses commandes resteront livrées pour qualification future,
pas annoncées comme exécutées. La documentation finale attend les deux branches
effectivement intégrées.

## Références

- Proposition rectifiée `result_mvuo5wsb_0002_hma7aa`, qa-planner/pool-21, lecture seule.
- Plan `qa-plan.md` §13 B5/B6/T4 ; contrat ligne28 ; plan PM rapport114.
- Évaluation branche B validée : PM/LLM brut53/retenu70, risque44, incertitude moyenne.

Aucun test, workflow, build, moteur, GPU, rendu, benchmark, installation ou
cloud exécuté pour cette ratification. Aucun nouveau droit ou modèle imposé.
