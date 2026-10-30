# Pont privé CTest — tests RED supplémentaires seulement

## État et autorité

- Auteur pool-23, tâche `task_mvukzhsc_0052_f1bbfq`, ownership Windows inchangé.
  HEAD auteur confirmé avant cette écriture :
  `f14ea7115394c2a5e072ee402823b9063cfb3007`, worktree
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-23`,
  branche `ensemble-4b686588-c97c-46ab-99a1-e176a14b9184` propre à l'entrée.
- Lead : RED initial déjà observé sur ce commit, 28 tests /25 échecs d'assertion
  avec sous-cas, zéro erreur/skip, empreintes stables. Logs
  `out/verification-synthetic/windows-binding-red-2026-10-30T06-41-49-105Z/`.
  Preuve attribuée au coordinateur, pas à l'auteur, aucun rerun auteur.
- Ratification lue directement dans le canonique autorisé :
  `.opencode/evidence/mission-uniform-pipeline/windows-adapter-binding-v1.md`
  §« Précision ratifiée — localisation privée de CTest » et rapport98,
  commit lead `71465aa2b32109e5ae3fa03676ea783d94c94cad`, consultation
  `result_mvulud8x_0000_07pkef`. Aucune branche copiée ou mergée ; le paragraphe
  du premier BINDING_RED_READY qui présentait encore une proposition est
  historiquement conservé, pas revendiqué comme statut de décision actuel.

## Fichiers et frontières de cette remise

- Nouveau `test_ctest_cache_binding.py` : 19 méthodes proposées, sélection
  nominative indépendante des suites Windows natives.
- Nouveau `cache_binding_fixtures.py` : compteur de lecture local en mémoire,
  aucun enfant, programme, sonde, état global de dernier bundle ou I/O à l'import.
- Cette note. Aucune édition des28 tests existants, binding_fixtures.py,
  windows.py, helper, anciennes assertions51, fixtures natives ou pins.
  Aucun core/modèle/runner/public-schema/CMake/preset/workflow modifié.

Les fixtures réutilisent WindowsBindingFixture et les helpers de sentinelles du
premier lot sans héritage de TestCase (aucune redécouverte des27 méthodes). Les
PreparationReceipt et BuildReceipt sont produits par le cœur intégré, avec les
seuls enfants remplacés par les doubles en processus déjà relus. Avant build,
le test écrit le cache synthétique voulu ; le vrai snapshot en calcule l'empreinte.
Le test07 seul dérive explicitement un reçu pour corrompre la clé d'inventaire :
ce dérivé teste directement le descriptor, pas une admission core favorable.

## Oracles discriminants après GREEN

Chaque rejet passe par `bound` : NotImplementedError du scaffold devient une
AssertionError **hors du type de rejet attendu**. Les négatifs n'acceptent donc
pas le placeholder comme un refus valide. Ils demandent un ValueError/EvidenceError
causal au cache, aucune commande du runner et aucune réparation ; les sentinelles
de processus/réseau/écriture restent actives pendant la description.

- **01** : nominal cache inventoré du même arbre ; CTest du cache avec espaces
  est employé INCHANGÉ en argv0 pour découverte et sélection. Le nominal test07
  initial reste intact et fait toujours partie du lot précédent.
- **02** : comparaison Windows indépendamment de l'hôte (casse/séparateurs,
  composants . et .., UNC), argv conserve la graphie déclarée. Les outils de ces
  chaînes ne sont jamais créés, recherchés ou exécutés ; aucune prétention
  d'installation ou de support UNC réseau.
- **03** : plan mixte workloads/CTest, exactement une ouverture rb et une lecture
  complète. Le compteur retournerait des octets d'une autre installation à une
  seconde ouverture. L'empreinte du reçu correspond aux premiers octets, les
  argv CTest doivent venir de ces MÊMES octets. Une seconde lecture pour parser
  ou pour une autre invocation fait échouer le compteur. Le test06 exige en
  complément le rejet digest AVANT décodage d'un cache modifié invalide UTF-8.
- **04–07** : absence sans fallback vers un autre arbre/configuration,
  fichier présent mais non inventorié, bytes modifiés, inventaire attribué à
  l'autre configuration. Aucun lancement/repair, bytes existants préservés.
- **08–13** : CHACUNE des deux clés absente, doublée identiquement ou avec valeur
  divergente, type autre qu'INTERNAL, syntaxe malformée, valeur vide/blanche.
  Les mauvais types et les lignes malformées/vides sont aussi placés À CÔTÉ
  d'une définition correcte : les ignorer ne peut pas satisfaire le test.
- **14–17** : chaque chemin indépendamment relatif, drive-relative, root-relative
  ou Posix absolu ; mauvais basename exe, répertoires Windows divergents ; UTF-8
  invalide dont le hash EST admis (ne pas confondre avec cache modifié).
- **18** : pas de PATH/which, expansion d'environnement ou de home ; les `%...%`
  dans un chemin Windows déjà absolu restent des caractères de nom littéraux,
  jamais une demande d'expansion. Pas d'existence de binaire supposée.
- **19** : workloads-only fonctionne SANS cache et sans entrée d'inventaire ;
  toute lecture du cache est une assertion sentinelle, exécutable pris dans le
  mapping du BuildReceipt. Un plan workload ne dépend pas d'un outil CTest.

Le compteur intercepte concrètement io.open/builtins.open, dont Path.read_bytes.
Il compte ouvertures, modes et bytes lus, peut fournir un second contenu sans
mutation de fichier pendant la description. Il ne prétend pas être une sandbox
pour accès arbitraires par fd natif ; ces tests visent l'implémentation Python
bornée prévue, et restent à relire avant exécution par le lead.

Le hash du cache authentifie uniquement les bytes DÉCLARANT le chemin : il ne
mesure/identifie pas le binaire CTest. Remplacement à chemin constant, outil
absent/inexécutable, machine et contexte restent hors de cette preuve. Les tests
ne qualifient ni CMake/CTest réels ni un GPU et n'ajoutent pas de schéma outil.

## Commande RED du lead — NON exécutée par l'auteur

Depuis le worktree auteur :

```powershell
Set-Location scripts/tests/verification_windows
C:/Python313/python.exe -B -m unittest -v test_ctest_cache_binding
```

Attendu : nonzero, assertions causales `CAUSAL RED: Windows direct execution
binding is not implemented`, pas ImportError ni réussite des négatifs sur le
placeholder. **19 méthodes selon source/AST, pas un comptage découvert/exécuté.**
Le lead conserve commande/cwd/interpréteur/empreintes, compteurs, stdout/stderr,
comptages et résultat bruts sous une nouvelle sortie exclusive
`out/verification-synthetic/`. Les seules fixtures temporaires possédées sont
nettoyées par leurs contextes de test, pas les preuves de campagne du lead.

Après RED supplémentaire observé, le lead pourra autoriser GREEN Python et
paramblock seulement. Le helper fonctionnel reste FERMÉ : mapping de fermeture
initial non suffisant. L'ouverture attend une tranche séparée avec mapping exact
des fragments entrée/dispatch/env/restore, doubles de feuilles/outils/Process,
revue puis RED observé AVANT correction. Aucune région PowerShell n'est modifiée
ici ; test06 jonction NON_EXECUTE et pas de discovery native globale.

## Matrice réelle de l'auteur et limites

- Lecture seulement de la décision canonique/rapport98 et de l'état Git ; aucune
  copie de branche ou invocation de suite pour récupérer des interfaces.
- Syntaxe réellement vérifiée via Context Mode : AST des deux nouveaux .py
  accepté sous `C:/Python313/python.exe` 3.13.1, 19 méthodes de tests comptées
  dans le nouveau module, zéro dans la fixture ; aucun import projet.
  Aucun unittest, enfant factice, PowerShell ou helper exécuté par l'auteur.
- RED supplémentaire/GREEN : NON_EXECUTE par l'auteur, première cause attendue
  explicite, pas observée. Les28 premiers tests ont la preuve LEAD ci-dessus.
- Windows Debug/Release natifs, Linux, ASan, outils réels, moteur CPU/GL, rendu,
  benchmark, installation et cloud : NON_EXECUTE ; aucun changement de toolchain,
  version/STL/ABI prétendument qualifiée ou compile database factice.
- Aucun sous-agent, team tool, auto-claim ou clôture. Remise au lead puis STOP,
  auteur distinct du runner ; scores/politique inchangés.
