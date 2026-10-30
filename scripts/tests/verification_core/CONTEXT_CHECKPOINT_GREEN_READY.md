# GREEN_READY — réservation du checkpoint interne à l'admission

## Entrée et attribution

Base : `a4aa6bbe58650f789e7f0e41325b23802452416d`.
RED communiqué par le lead : une méthode, quatre failures de sous-cas (absolu et
relatif normalisé, admission avant runner et octets bruts), zéro error/skip,
baseline PASS atteinte, hashes stables. Chaque variante exécutait deux commandes
supplémentaires et remplaçait le XML par du JSON.

Logs lead :
`out/verification-synthetic/context-checkpoint-red-2026-10-30T05-21-49-998Z/`.
Le ré-audit précédent avait levé statiquement les trois anciens findings ; cette
collision était un nouveau bloc, non couvert par les preuves antérieures.

Worktree :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-14`.
Branche : `ensemble-3b74f9ed-91ef-410f-8ac6-431ecb896a6d`.

## Cause et correction minimale

Le chemin encore absent `raw/ctest-population.json` pouvait être admis comme
destination enfant. Le core attribuait alors ce XML brut, puis le writer de
checkpoint confondait son hash identique avec une autorisation de remplacement.
Le code4 tardif du parseur ne réparait ni le lancement interdit ni le brut perdu.

Modification limitée à `scripts/verification/evidence.py` :

- `_CTEST_POPULATION_RELATIVE`, constante privée, donne un seul chemin commun à
  l'admission et à la publication ; aucune option ou donnée publique ajoutée.
- `_execution_requests` canonicalise ce chemin par `_within`. Dans la boucle
  existante des destinations `--result` et `--output-junit`, chaque destination
  canonique égale au checkpoint est refusée avec protocole4, avant les commandes
  d'exécution, y compris la découverte. Le refus ne dépend pas de l'existence du
  fichier et couvre aussi l'alias relatif normalisé.
- `_publish_ctest_population` conserve sa publication atomique et ses vérifications
  d'intégrité existantes. Son commentaire explicite que la propriété interne vient
  de la réservation à l'admission, pas de l'égalité d'une empreinte de brut enfant.

Pas de nouveau schéma, capability cache, quota, API ou mécanisme de capture.
L'index des artefacts ne peut plus rendre une destination déclarée enfant éligible
au remplacement comme publication interne. Les destinations externes au bundle,
existantes ou dupliquées conservent leurs gardes acquises. Les checkpoints internes
successifs restent autorisés par leur writer unique et leur hash précédent.

## Invariants et limites d'ownership

Un seul écrivain possède le bundle ; aucun raw enfant ne doit occuper le nom
réservé. La comparaison porte sur les chemins canonicalisés, pas la chaîne argv
originale. Aucun symlink n'est créé par cette tranche ; les vérifications de
containment existantes restent actives. Pas de mutation de cwd/environnement,
de preuve GPU ou de durée de vie native déduite de cette réservation de fichier.

Les 172 tests et toutes leurs assertions/mappings restent inchangés. Aucun autre
fichier Python, runner, adaptateur, modèle, parser, CLI, C++ ou API bundle/payload
Windows n'est modifié. Aucun changement des verdicts, causes ou sorties brutes des
exécutions admissibles ; la collision est refusée avant qu'elles puissent partir.

## Vérification proposée au lead

**Aucun test exécuté par l'auteur. GREEN_READY n'est pas un résultat GREEN.**
Après lecture du diff, exécuter séquentiellement dans ce worktree :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_checkpoint_ownership.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p 'test_*.py' -v
```

Inventaire attendu à recouper : un ciblé, 172 core, 28 parser ; pas de comptage
historique codé en dur dans le pipeline. Conserver nombres, codes, logs et hashes
effectifs, puis ré-audit pool18 avant intégration. Les skips historiques ne sont
pas requalifiés par cette correction.

Les contrôles auteur sont uniquement statiques (AST/diff/ownership/existence des
fichiers), distincts d'une exécution du candidat. GREEN et non-régression ici :
NON_EXECUTE ; garde native/C++/GL/rendu/performances NON_COMPILE/NON_EXECUTE.
Aucun enfant de test/workload, outil moteur, CMake/CTest, installation, benchmark
ou cloud exécuté par l'auteur. L'intégration reste bloquée jusqu'aux preuves et à
la revue ; les adaptateurs natifs restent une qualification distincte.

Contrôles statiques effectués : AST du module valide, diff-check code0, régression
inchangée (SHA-256 `0cc19defdb00fce50c95bb023f001ae48daecc97dee08ec04432690b31fe1138`),
aucun diff contre `a4aa6bb` sur tests/domaines exclus ; document présent sur disque.
Ces contrôles ne sont pas une exécution du test ciblé ni du pipeline.
