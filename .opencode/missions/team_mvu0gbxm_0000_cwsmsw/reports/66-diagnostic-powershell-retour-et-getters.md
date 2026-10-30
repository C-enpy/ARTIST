# 66 — Diagnostic PowerShell : retour pur et getters explicités

## Résultats discriminants

Le coordinateur a exécuté deux cellules dans des PowerShell frais, avec uniquement des marqueurs factices et un environnement explicitement assaini. Aucun helper ou enfant applicatif n'est lancé.

**Retour pur :** la variable vide existe au premier snapshot (longueur zéro) et après affectation directe. Après appel d'une fonction renvoyant un littéral constant, elle est absente. Les témoins non vide et absent restent corrects. La perte est donc reproduite sans le produit ; cela écarte l'attribution au parser ou au constructeur comme cause nécessaire. La frontière exacte entrée/retour et son mécanisme restent à préciser.

**Getters :** sur un unique objet Process jamais démarré, les propriétés adaptées `Id` et `HasExited` donnent null ; les appels explicites `get_Id()` et `get_HasExited()` lèvent `InvalidOperationException`. La variable vide reste présente dans cette cellule. Le refus erroné du processus non démarré est donc expliqué par le contrôle utilisant l'adaptateur de propriétés.

## Suite autorisée

La correction ciblée des getters est autorisée, avec diff produit/harnais séparés et pin relu ; les assertions restent inchangées. Pour la variable vide, une expérience entrée/avant retour/après appel est demandée avant choix de correction. Ni restauration par mutation d'environnement, ni assimilation du vide à l'absence, ni nouvelle dépendance ou installation ne sont autorisées.

Ces diagnostics ne constituent pas un GREEN du produit et ne relancent pas les 37 anciens tests. L'intégration Windows reste bloquée. Aucune preuve d'enfant réellement vivant ne peut être déduite du précédent marqueur construit à partir d'une propriété null.

## Preuves

- Script lead : `.opencode/evidence/mission-uniform-pipeline/diagnostics/windows-env-getters.ps1`, deux modes `return` et `getters` ; pas de Start/Kill, sérialisation JSON ou fonction de trace.
- Logs, traces, commandes, empreintes avant/après du script et identité PowerShell : `out/verification-synthetic/windows-env-getters-diagnosis-2026-10-30T03-32-22-311Z/`.
- Deux cellules, codes 0 ; instantanés en booléens/longueur seulement, aucune valeur d'environnement utilisateur exposée.
- Intervention locale diagnostic indivisible : évaluation PM/LLM brute 40/25, complexité retenue 70, risque 25 ; plan d'expérience préalablement relu, aucune admission de worker ou politique modifiée.

Aucun moteur, compilation, OpenGL, rendu, benchmark, installation ou cloud exécuté.
