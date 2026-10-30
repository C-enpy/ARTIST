# Admission separee du pin — GETTERS_FIX_READY a relire

## Identite et diff source

Source corrigee : `4361426bdb660e41ad9349afb0b754e0a5106f12`.
Fixture, diff distinct a revoir : `9554672bac101a07a58b06a12531b765fcbcc14d`.
Proposition reduite vide, non executee : `416e44221115fcfa12da80294af8e88b01aa7fa9`.
Base RED observee lead : `77900399fb9f18bc9f75d17904f8f8195b138321`.
Source precedente `71cf8b0f032794814c3697e1f2fe3720af7c4759` et admission
`d6715a21cce6732f2a54bb4ebe010f6f6edee21e` relues, mais aucun run accorde.
Le quota non ratifie a ete retire dans `80f5a2f4f4f28be5cf0c7113e55a2ba8630ae87d`.
La tentative sur `90c26f8ff99cd809b487a8c53c31f7d70f134891` a ensuite echoue :
selfcheck14 PASS, 13 methodes / 14 failures, anciens37 non relances. Getters
adaptes fautifs confirmes par les deux cellules lead ; perte du vide reproduite
sur retour pur SANS produit. `GETTERS_FIX_READY.md` distingue correction getter
et limite de host encore ouverte. Aucun resultat GREEN observe ou presume.

Revue exacte du produit, sans execution :

```powershell
git diff 77900399fb9f18bc9f75d17904f8f8195b138321 4361426bdb660e41ad9349afb0b754e0a5106f12 -- scripts/build-windows.ps1
git show 4361426bdb660e41ad9349afb0b754e0a5106f12 -- scripts/build-windows.ps1
git show 9554672bac101a07a58b06a12531b765fcbcc14d -- scripts/tests/verification_windows/native_fixture.ps1
```

Seule la region NATIVE du helper change. WRAPPERS, DISPATCH, declarations CLI,
refus du helper complet et code historique MSVC/build sont identiques a la base.
NATIVE_GREEN.md fournit effets, risques et commandes selectionnees.

Ce diff d'admission modifie le fichier pin, actualise les notes et ajoute
GETTERS_FIX_READY.md ; pas un corps, une assertion, une fixture, un delai ou
l'enfant. Le diff fixture de deux getters est isole dans son commit ci-dessus.

## Empreinte explicite

Extraction identique au lecteur de test : texte UTF-8 du helper, fins de ligne
LF normalisees par read_text Python, entre les marqueurs BEGIN/END NATIVE,
marqueurs exclus, whitespace initial/final de la region conserve.

- SHA256 historique des quatre refus RED :
  `7a9bdcddee678471e7577d7812556e77992c5550280352f3e3bb32dfc66f337d`.
- Pin precedent du candidat non admis :
  `9fcebf385d23d97c2c48a9c4cbdc07b914485577f27fc414884f278abb8c29d1`.
- Pin precedent sans quota, sur lequel les echecs ont ete observes :
  `7c667fed1b31e35c75291e7092530d66c4209e70d349042b620f223d3d4463d9`.
- Nouveau pin du source corrige avec getters explicites, sans quota :
  `8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de`.
- Taille de region : 21128 unites UTF-16, sous la limite fixture 65536.
- AST : quatre definitions de premier niveau ; sous-fonctions privees de lecture
  JSON et drain seulement a l'interieur de leur fonction consommatrice.

Le lead doit relire le source corrige et CE changement de pin AVANT un run.
L'egalite de hash n'est ni confinement, ni preuve de correction, ni autorisation
automatique de lancer un autre code. Le controle d'un source modifie reste dans
test14 ; ses assertions sont inchangees.

## Frontieres d'admission conservees

Seulement le Python/enfant fini identifie et son Process/StartInfo fourni par
la fixture. Un seul Start sur cet objet, aucun autre enfant/StartInfo substitue,
aucune commande MSVC/vcvars/outil/helper reel. Fin/streams bruts/timeout sur fake
ne demontrent PAS gestion d'arbre ou reclamation face a des descendants qui
retiennent les pipes. Un stop/drain/lecteur non reclaim donne une panne explicite,
jamais un succes ; ce risque ne peut pas etre leve par le pin.
Dispose du lecteur precede pending.Wait et reste une operation synchrone fragile
face aux writers herites. Pas de delai absolu ni de reclamation generale prouves.
Revue independante de la capture de production requise avant helper complet ;
aucun parent/descendant reel supplementaire autorise par cette remise.

test06 reparse reste intact et NON_EXECUTE, sans nouvelle jonction ou privilege.
Inspection reparse SOURCE_ONLY ; aucune couverture des deux symlinks core SKIP.
Pas d'integration windows.py/parser/core ou de BuildReceipt recopie/reconstruit.

## Preuve disponible et attendus

Auteur : syntaxe PowerShell/AST et comparaison Git/digest seulement. Aucun test,
fixture/selfcheck, enfant, fonction native ou workload execute.

La proposition courante est RETURN_BOUNDARY_PROPOSAL.md : une cellule inline
retour, puis au plus UN temoin conditionnel en host frais, lead seul apres revue.
Ne pas relancer cinq cellules ni annoncer que les getters corrigent la perte
vide. Les commandes 14+37 de NATIVE_GREEN.md restent des reprises historiques,
pas un resultat GREEN ni une autorisation automatique de nouvelle suite.
Conserver logs out/, identites et premiere divergence ; aucun resultat de
campagne GPU/CPU, performance, toolchain Windows ou qualification Linux revendique.
