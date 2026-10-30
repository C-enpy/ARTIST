# Diff d'audit isole — acces bruts aux cles, lecture seule

Autorisation explicite lead apres RED cible sur d048c7d ; proposition exacte
de TARGETED_RED approuvee. Ce document ne presume pas d'un run GREEN.

## Perimetre du diff

Un seul bloc modifie dans wrapper_fixture.ps1 : le rejet des proprietes hors
Keys/Count/Length recoit l'exception PSBase suivante, sans toucher aux listes
de commandes/types/variables, aux methodes autorisees ou a l'audit dispatch.

- fonction source exactement Set-VerificationProcessEnvironment ;
- noeud non invoque PSBase, exactement recepteur direct d'un MemberExpression ;
- expression complete exactement `$current.PSBase.Keys` ou `$Values.PSBase.Keys` ;
- current garde son controle Hashtable avant acces ; Values reste [hashtable] ;
- aucun PSBase autonome ni autre recepteur/propriete/methode ;
- rejet si la chaine se trouve dans un LHS d'affectation (direct, compose,
  indexe/parenthese), ou dans une expression d'increment/decrement ;
- une lecture sur RHS, telle que `$names = @($current.PSBase.Keys)`, reste admise.

Les offsets AST servent uniquement a verifier l'appartenance au LHS : une
affectation contenant l'acces a droite n'est pas confondue avec une ecriture.
La verification d'invocation de methode reste celle existante (Path.GetFullPath
pur seulement). Aucune evaluation du code avant audit, aucun nouvel outil.

## Gardes ajoutees hors des 26 tests figes

Nouveau fichier test_psbase_audit.py, quatre methodes, meme factory/fixture :

1. lectures exactes sur les deux maps, sans mutation ;
2. refus metadata.PSBase.Keys, Values.PSBase.Values, GetType(), PSBase isole,
   et variante de casse non exactement approuvee ;
3. refus de la meme lecture dans Get-VerificationProcessEnvironment ;
4. refus =, +=, indexation LHS et ++ sur chacune des deux chaines approuvees.

Ces contre-exemples sont des donnees pour l'audit. Ils ne nomment aucun outil,
processus externe ou chemin de fichier ; meme leur evaluation accidentelle
ne peut atteindre un MSVC/build/helper reel. Le code 96 et zero evenement sont
exiges : une exception runtime de propriete readonly ne suffit pas a les valider.

## Commande proposee au lead, NON_EXECUTE par l'auteur

Cwd du worktree Windows :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_psbase_audit.py' -v
```

Attendu : 4/4, zero failure/error/skip. Le lead relit ce diff avant run.
Le helper complet et les primitives OS demeurent non raccordes/interdits.
