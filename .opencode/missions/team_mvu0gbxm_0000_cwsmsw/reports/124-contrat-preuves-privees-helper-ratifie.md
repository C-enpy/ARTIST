# 124 — Convention privée des preuves fixée avant les tests

Le coordinateur retient l'avis d'architecture avec un choix minimal explicite :
contexte privé du helper, témoin prepare/build de **chemins et versions déclarées**,
et collecteur core à population fermée. Le témoin ne certifie pas les binaires
outils ni leurs dépendances ; aucun nouveau champ public ou registre général.

Les noms, champs et rôles privés sont fixés dans un contrat local : logs bruts et
trace par rôle, témoin écrit seulement après prepare réussi, transports MSVC
temporaires allowlistés. Le core collecte sous lease, y compris les fichiers
partiels sur échec, avant d'émettre le reçu ; il refuse absence/incohérence de
preuve nécessaire au succès, fichiers inconnus et alias, sans réparation.

Le complément QA sera reçu et commité avant auteurs de ce delta. Le collecteur
Python et le raccordement PowerShell ont des fichiers distincts et des tests
synthétiques indépendants ; ils peuvent avancer en parallèle une fois ce gate
franchi. Les fakes historiques doivent simuler la nouvelle preuve, pas perdre
leurs assertions. Leur mapping sera proposé avant modification.

Le helper reste fermé et la production inchangée à cette étape. Les baselines
et RED déjà constatés restent acquis, mais ne prouvent pas les nouvelles feuilles.
La réception attendra les deux producteurs réellement intégrés et leur couture.

## Références et évaluations

- Avis `result_mvupwdk8_0001_thq007`, architecte lecture seule, pas d'exécution.
- Contrat `windows-helper-evidence-v1.md` ; source du gap au rapport123.
- Planification locale : tranche core indivisible PM/LLM brut/retenu78, risque72,
  incertitude moyenne ; PowerShell conserve sa tranche78/68 déjà admise. Plancher
  développement respecté, aucune modification de modèle ou grille pour contourner le routage.

Aucun nouveau test, outil natif, moteur, GPU, rendu, benchmark, installation
ou cloud exécuté. Qualification native et identité binaire des outils NON_EXECUTE.
