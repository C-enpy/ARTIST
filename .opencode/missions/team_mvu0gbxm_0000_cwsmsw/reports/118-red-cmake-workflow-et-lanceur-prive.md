# 118 — RED CMake/workflow observé, lanceur privé ratifié

Le coordinateur a relu les trois fichiers de tests et leur plan : contrôles
structurels CMake/YAML bornés et transport testé par extraction des deux fonctions
privées, sans initialisation du module ou processus. Les calibrations synthétiques
ne remplacent pas le contrôle des sources réelles.

Résultat exécuté : **11 tests, 8 échecs d'assertion attendus**, aucune erreur ou skip,
empreintes stables. Option root, workflow et lanceur manquent effectivement.
Les assertions de parsing/transport situées après ces absences ne sont pas toutes
prétendues individuellement discriminées par ce RED.

Le coordinateur ratifie le petit lanceur privé `scripts/verification_workflow.py` :
construction pure d'argv puis appel unique à la CLI existante, sans enfant, shell,
schéma ou capture supplémentaires. Le point d'entrée et sa liaison réelle restent
à relire : extraire des fonctions ne prouve pas leur lancement par GitHub Actions.

GREEN minimal autorisé sur parents CMake, workflow manuel et ce lanceur ; les
onze tests et assertions restent intacts. Machines préparées self-hosted seulement,
phases explicitement choisies, codes et preuves conservés, aucun bootstrap/réparation
implicite. Le helper Windows reste un producteur indépendant, non modifiable par
cet auteur. Une sélection de scénario par workflow est acceptée ; la CLI multi
reste inchangée.

## Preuves

- Candidat `76ae5fc8114e9d64c1913a26279e3188127caaf7`, auteur pool-32 ; runner coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_integration -p test_*.py -v`, worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/cmake-workflow-red-2026-10-30T08-01-45-007Z/`.
- Un nom de fichier de test supposé a été constaté absent avant lecture ; le dossier
  a ensuite été lu pour prendre les noms réels. Ce diagnostic n'est pas un RED.

Aucun CMake configure/build, action GitHub, shell, enfant, outil natif, moteur,
GPU, benchmark, installation ou cloud exécuté. Qualification native NON_EXECUTE.
