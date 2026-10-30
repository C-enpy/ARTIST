# Cible OpenGL attendue — HARDWARE_RED_READY, tests seulement

## Autorisation et lecture

Precision ratifiee annoncee par le lead au commit
`c7c3d14380de18adeb74bbadb8cbccd3f0979b44` :
`.opencode/evidence/mission-uniform-pipeline/hardware-expectation-v1.md`.
Fichier lu dans la racine canonique ARTIST via chemin absolu ; aucune copie
interbranche, modification core ou integration implicite dans ce worktree.

Cette remise ajoute uniquement des tests et ce document. Le protocole Python,
les 17 tests precedents, les sources natives et les descriptions du commit
`60f32c52102f7a5df38116147a458638aecdfbbc` restent inchanges.

## Plan borne et frontiere locale

1. Runner lead : constater le RED de disponibilite de la nouvelle cible sur les
   tests ajoutes ; aucun test execute par l'auteur.
2. Sur autorisation seulement : accepter la paire optionnelle dans argv et la
   paire attendue en entree locale du parseur, puis constater les branches
   semantiques encore manquantes avant leurs validations GREEN si necessaire.
3. Apres preuve et autorisation : validations minimales, non-regression par le
   lead ; ensuite seulement adaptation native en SOURCE_ONLY et revue statique.
   Aucun changement natif ou GREEN anticipe dans cette remise.

`build_workload_argv` recevra les deux kwargs normatifs optionnels, defaults
None ; sans paire, argv historique identique. Avec paire, les quatre arguments
normatifs sont ajoutes apres les flags GL existants, sans interpolation shell.
Paire complete, chaines non vides, sans controle/NUL, <=4096 **caracteres** ;
hardware uniquement. C0/DEL/C1 sont controles ; la borne Unicode n'est pas une
borne de bytes UTF-8. Profil CPU/logiciel sans cible reste compatible.

Pour les tests du **parseur local**, `expected` conserve ses neuf champs
d'identite precedents et peut porter en plus expected_gl_vendor/renderer. Ces
deux valeurs ne sont ni des champs top-level du resultat ni des parameters :
les quatre parameters existants restent strictement identiques. Aucun Request,
ScenarioSpec ou schema de hash/receipt core n'est copie ou teste ici.

Environment conserve expected_gl_vendor, expected_gl_renderer, gl_vendor,
gl_renderer et qualification. Le marqueur propose localement est
MATCHED_EXPECTATION (pas une preuve universelle de hardware). Il ne suffit pas
a lui seul : egalite exacte des deux chaines declarees/observees avec la cible,
refus des logiciels connus meme si noms concordants, et metadonnees observables
de contexte courant >=3.3 core. Ces observations sont toutes synthetiques dans
les tests. Les tests sans paire gardent le contrat historique ; le core bloque
le run hardware sans cible avant execution, selon sa propre tranche TDD.

## Premiere tranche

`scripts/tests/verification_scenarios/test_hardware_expectation.py` : 12 tests
ajoutes (S2/S5/S7, E2/E4/E5), zero processus, sonde, reseau ou fixture sur disque.
Les fixtures adjacentes test_contract sont reutilisees sans modification et
sans importer sa classe TestCase dans ce nouveau module (pas de doublon de
decouverte). Donnees GPU/driver fictives, aucune qualification materielle.

Positifs : flags exacts avec espaces, omission compatible tous profils, limite
Unicode inclusive, resultat de meme forme avec quatre parameters, FAIL domaine
attribue et preserve. Negatifs : paire partielle/nonchaine, vide/controle/borne,
profil incorrect, preuve absente/differente, logiciel concordant, contexte ou
qualification non conformes, identite/target differente meme en verdict FAIL.

Les negatifs commencent par une assertion de baseline cible valide acceptee,
afin de ne pas passer pour une mauvaise raison (ancien schema refusant toute
cible). Au commit source precedent, attendu **non observe** : assertions
HARDWARE_ARGV_CONTRACT_MISSING (kwargs encore absents) et
HARDWARE_RESULT_EXPECTATION_NOT_ACCEPTED (attente cible encore refusee).
C'est un RED de disponibilite de l'extension ; pas encore une preuve que ses
branches de validation ont ete atteintes. Aucun import accidentel ou modele
core absent n'est le motif attendu. Le lead qualifiera l'observation avant suite.

## Commandes proposees au lead uniquement

Tranche nouvelle :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_hardware_expectation.py" -v
```

Puis non-regression complete, une fois GREEN autorise :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Decouverte attendue : 12 nouveaux tests ; 29 avec les 17 anciens. Ce sont des
nombres de definitions sur disque, pas des comptages executes par l'auteur.
La validation de l'inclusion de cible dans plan_sha256/receipts et le blocage
core de run sans cible restent au proprietaire core ; pas de fausse preuve ici.
Creation/refus natif code20 INCONCLUSIVE UNVERIFIED, avant upload/draw, sera
revu statiquement apres autorisation, sans compilation/execution.

Executions auteur : 0. RED/GREEN de cette extension : INCONCLUSIVE, runner lead
attendu. C++/GL/moteur/benchmark/cloud : NON_EXECUTE. Les preuves anterieures du
protocole initial restent attribuees au runner lead, pas a cette extension.
