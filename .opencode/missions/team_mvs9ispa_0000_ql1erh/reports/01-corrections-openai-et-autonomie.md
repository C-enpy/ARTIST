# Préparation du rattrapage QA Linux — catalogue OpenAI et autonomie

## Objectif et étape

Préparer le rattrapage QA Linux sous WSL2 puis en CI, en mutualisant les traitements Windows/Linux. La mission reste non approuvée : aucun worker, classifieur, build, installation WSL ou changement de source moteur n'a été lancé.

## Corrections persistées

- Provider autorisé : `openai` uniquement. Le catalogue passe de 27 à 11 couples modèle/variante, sans modifier les métadonnées des couples conservés.
- Mandat `autonomy.mode: long_run` : best effort, parité avec les obligations Windows sans affaiblissement, mutualisation prioritaire, outils existants et absence de surconception. Distribution WSL2 dédiée sur `I:` ; permissions et exigences du projet conservées.
- QA `proportionate`, seuil initial 50 et paramètres existants inchangés. Aucune classification ajoutée, aucune configuration globale modifiée.

## Vérifications et limites

Lecture du brouillon puis contrôle JavaScript via Context Mode : JSON lisible, provider unique, 11 couples distincts, seuil et QA inchangés, structure et limites de longueur du mandat autonome vérifiées.

Le champ `autonomy` est explicitement documenté pour les politiques v2 dans la référence Ensemble locale `references/mission-policy.md`, section « Long-run autonomy ». La validation native des modèles/variantes et la nouvelle présentation restent à faire par `config`. Ce contrôle local n'est pas une approbation native ni une preuve Linux.

## Suite recommandée

Conserver la QA proportionnée proposée et exécuter `/ensemble-mission config` pour présenter la version modifiée. Des corrections peuvent encore être faites dans le chat ; seul le `/ensemble-mission start` de l'utilisateur approuvera la révision présentée. Ne pas interpréter le mandat autonome comme une approbation d'exécution anticipée.

## Annexe technique

- Brouillon : `.opencode/missions/team_mvs9ispa_0000_ql1erh/model-policy.json`.
- SHA-256 du fichier après édition : `e4b6fe33392e7c66ce2eaf7d81885294e85e562179a81cd341c18552cd2f4b71` ; ce n'est pas le hash de présentation natif.
- Modèles conservés : `openai/gpt-6.1-sol` (medium, max, high, low, xhigh), `openai/gpt-6-astra` (high, max, low, medium, xhigh), `openai/gpt-6-luna` (high).
