# RED_READY — collision JUnit/checkpoint interne

## Entrée et portée

Ré-audit `result_mvuipu8a` sur le collecteur corrigé : les trois précédents findings
sont levés statiquement ; nouveau bloc no-clobber confirmé par le lead. Base locale
`617e939bd8b76b0d44fdb35b6e77c3df1b3fb971`.

L'admission accepte un `--output-junit` visant le checkpoint encore absent
`raw/ctest-population.json`. L'enfant factice y écrit du XML, que le sink attribue
comme brut. `_publish_ctest_population` confond ensuite cette attribution/hash avec
une propriété de publication interne et remplace le XML par du JSON.

Cette remise ajoute uniquement `test_context_checkpoint_ownership.py` et ce
document. Aucune production ni assertion acquise modifiée. Les 171 tests et le
mapping ratifié restent inchangés. Aucun GREEN anticipé, schéma/API, runner,
adaptateur de production, capture ou interface bundle Windows modifié.

## Régression unique et préconditions

Une seule méthode, avec deux sous-cas de destination :

- chemin absolu du checkpoint ;
- chemin relatif au cwd déclaré, avec segments `./` normalisés vers le même
  checkpoint. Pas de parent traversal ni de symlink créé.

Avant les variantes, un parcours hardware nominal complet doit donner receipt
COMPLETED/0 et rapport PASS/0 avec le vrai parser de contexte et les XML intacts.
Chaque variante possède une nouvelle fixture et un build synthétique conforme.
Le checkpoint doit être absent avant l'admission. Le test vérifie que la seule
modification de CommandSpec est la valeur du `--output-junit` : cwd, environnement,
timeout, effets, options et découverte restent identiques.

Attendu : protocole4 **avant tout nouvel appel au runner**, sans verdict favorable.
Les appels prepare/build antérieurs sont explicitement comptés et exclus. Le
rapport est readonly sous sentinelles et comparaison de tous les octets du bundle.
Un second oracle vérifie les octets XML capturés par le double si le bug a déjà
atteint l'écriture : un checkpoint interne ne doit jamais écraser un brut enfant.

Les deux oracles sont des sous-tests indépendants afin de voir le défaut
d'admission et le clobber dans la même régression, sans ajouter une campagne. Un
code4 tardif causé par le parsing du XML déjà écrasé ne satisfait pas le contrat.

RED causal attendu sur cette base : nouveaux appels runner atteints et/ou octets
XML remplacés. Un ImportError, TypeError, fichier mal résolu ou échec de baseline
nominale ne constitue pas le RED demandé.

## Réemploi et confinement

Les fixtures/doubles intégrés sont réutilisés. Une petite sous-classe locale du
double de fichiers résout le seul chemin relatif depuis CommandSpec.cwd, car le
double historique ouvre directement Path(argv) et ne crée pas réellement un
processus avec cwd. Le cwd du processus auteur/test n'est jamais modifié ; la
provenance du double conserve le CommandSpec original. Aucun nouveau runner ou
adaptateur de production.

Répertoires temporaires possédés avec espaces, faux binaires inertes, XML et logs
de données seulement ; injections explicites de runner et lecteur de révision.
Popen/subprocess.run interdits pendant toutes les phases synthétiques. Aucun
processus natif, moteur, CMake/CTest, GL, symlink réel, installation ou cloud lancé.
Les fichiers temporaires sont nettoyés par les cleanups existants.

Le risque est une confusion d'ownership de fichier, pas une preuve GPU : attribution
d'un brut/hash ne signifie pas autorisation de remplacement par une publication
interne. La stratégie de futur GREEN retenue par le lead est de réserver ce chemin
à l'admission `_execution_requests`, après canonicalisation `_within`, pour les
destinations `--output-junit` et `--result` applicables. Elle n'est pas implémentée
ici et n'introduit ni capability cache ni nouveau format public.

## Commande lead et limites

Worktree :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-14`.

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_checkpoint_ownership.py' -v
```

Un test est proposé à découvrir, pas un résultat observé. Le lead conserve codes,
comptages, logs et hashes, puis autorise seulement le GREEN causal. Les preuves
antérieures ne couvrent pas cette collision ; intégration toujours bloquée.

Contrôles auteur effectués sans import/exécution du module : AST valide, une seule
méthode test recensée, diff-check code0, aucun diff contre `617e939` sur production
ou fichiers de tests déjà suivis ; fichiers de remise présents. Ces contrôles
statiques ne remplacent pas la baseline ni le RED exécutés par le lead.

Tests auteur : zéro. C++/garde native/GL/rendu/performances NON_COMPILE/NON_EXECUTE.
Les diagnostics statiques Python/Git et la persistance du candidat sont séparés
de toute exécution de tests. Aucun contrôle ou résultat de qualification native
Windows/Linux n'est revendiqué.
