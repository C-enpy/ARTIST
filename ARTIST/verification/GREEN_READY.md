# Uniform scenarios — protocole Python, GREEN_READY

## Preuve RED du runner et autorisation

Le lead rapporte sur `b38c3492a98cdf64530228a5820770c0fa28db9e` : 17 tests,
84 echecs d'assertions/sous-cas, code 1 ; tests inchanges et sources stables.
Les validations absentes atteignaient bien les assertions `ValueError`, sans
`SCENARIO_PROTOCOL_MISSING`. Journaux annonces par le lead :
`out/verification-synthetic/scenario-protocol-semantic-red-2026-10-29T23-11-13-594Z/`.
Cette preuve est attribuee au runner lead, pas a une execution de l'auteur.
Le lead a ensuite autorise les validations minimales de cette tranche pure.

## Candidat GREEN propose, non execute

Seul `scripts/verification/scenarios/protocol.py` change pour le comportement :

- selections fermees et routes compatibles ; preparation/lifetime exclus car
  producteurs CTest, pas workloads ; CPU-only sans GL ni mesure de soumission ;
- iterations/repetitions/lots et produit bornes selon le plan QA ; bool, float
  et texte refuses comme entiers ; identifiants non vides et empreinte SHA-256 ;
- champs/resultats v1 stricts ; JSON duplique/tronque/non-objet refuse ;
  identites/parametres verifies **avant** verdict, y compris FAIL ;
- compteurs entiers non negatifs, noms reconcilies ; updates declares egaux a
  iterations*repetitions*batch, draws aussi pour GL (zero CPU) ; PASS exige les
  operations declarees, FAIL explicite conserve ses compteurs discordants ;
- samples attribues au temps mural CPU de soumission, ns, population
  iterations*batch ; repetitions uniques et completes pour VALID ; valeurs
  finies, pas bool, temps strictement positif pour VALID ;
- ABSENT reste ABSENT sans sample ; mesure non demandee distincte ; une mesure
  VALID exige PASS. Aucun oracle de gain, temps GPU ou resultat reel fabrique.

Les 17 tests de `scripts/tests/verification_scenarios/test_contract.py` restent
strictement inchanges depuis `c36d714e47325331569b6abebcf3d9ecc4d31f41`.
Empreinte Git attendue : `7fe6c23fbac2e63d6f90776e70562aeecb580084`.
Le module n'importe ni modele core, ni outil natif et n'effectue aucune I/O.

## Commande lead et limites

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Attendu : 17 tests reussis sans modification des assertions ; non observe par
l'auteur. Executions auteur : 0. GREEN/non-regression : INCONCLUSIVE en attente
du runner lead. La qualification renderer, les empreintes de fichiers, le
confinement des artefacts, les receipts et l'agregation restent au collecteur
core ; ce parseur en memoire ne les remplace pas.

C++ et raccordements reels non ajoutes, leur autorisation restant conditionnee
au GREEN observe. Compilation/execution moteur/GL, rendu et benchmark restent
NON_EXECUTE. Aucun helper, installation ou cloud execute. Les remises
RED_READY/SEMANTIC_RED_READY sont des historiques, ce document est la remise
courante. Revue sensible independante reservee au lead avant reception finale.
