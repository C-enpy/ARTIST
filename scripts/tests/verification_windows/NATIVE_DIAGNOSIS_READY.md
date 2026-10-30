# DIAGNOSIS_READY — getters adaptes et marqueur vide, sans nouveau run

**Proposition historique remplacee :** apres les deux cellules lead concluantes,
utiliser `RETURN_BOUNDARY_PROPOSAL.md` (une cellule + un temoin conditionnel au
maximum). Ne pas rejouer les cinq cellules ci-dessous. Les observations et
l'absence de correctif de cette ancienne remise restent datees de sa base.

## Candidat gele et preuve existante

HEAD analyse : `90c26f8ff99cd809b487a8c53c31f7d70f134891`.
Pin natif inchange :
`7c667fed1b31e35c75291e7092530d66c4209e70d349042b620f223d3d4463d9`.
Aucun correctif produit, test, fixture/assertion, enfant ou pin dans cette remise.
Le lead seul executera un eventuel diagnostic APRES revue de sa proposition.

Tentative GREEN lead, pas un GREEN obtenu : selfcheck14 PASS puis 13 methodes,
14 failures, zero error/skip. Les 37 anciens n'ont PAS ete relances apres cet
echec. L'erreur JavaScript/regex initiale precedait les tests et ne compte pas
comme echec produit. Cas06 reparse toujours NON_EXECUTE, aucun nouveau privilege.

Logs relus via Context Mode, depuis la racine canonique donnee par git-common-dir :
`C:/Users/djden/source/repos/ARTIST/out/verification-synthetic/windows-native-selected-green-2026-10-30T03-21-46-777Z/`.
Le stdout confirme les fixtures dans ce worktree :
`out/verification-synthetic/windows-native-fixtures/20261030T032147Z-f26450d69ecd49319537a762b722e69a/`.
Analyse fichiers existants uniquement, aucun test/processus de diagnostic lance.

## Deux phenomenes a separer

1. Capture/composed refusent avant Start :
   `InvalidVerificationProcess: caller must provide an unstarted process.`
   Le garde fait `[void]$Process.Id` puis associated=true, en supposant que le
   property adapter PowerShell leve toujours l'exception du getter .NET. Cette
   supposition peut etre fausse : un retour null adapte ne prouve pas association.
   De meme `-not $ownedProcess.HasExited` peut convertir un null adapte en true.
   owned_alive_after_call=true n'est donc PAS ici une preuve d'enfant vivant.
   Aucun PID, Kill de tiers ou lancement additionnel ne doit en etre deduit.
2. Perte du marqueur vide parent, cause NON ATTRIBUEE. Agregation des result.json
   existants, sans publier de valeurs d'environnement autres que leurs etats :
   - flags nominaux : 4 cas, empty -> null ; flags refuses : 14, empty -> empty ;
   - payload nominal : 1, empty -> null ; payloads refuses : 7, empty -> empty ;
   - start nominal : 3, empty -> null ;
   - limit nominal : 2, empty -> empty ; limit refuse : 1, empty -> empty ;
   - capture refusee : 5, et composed refuse : 1, empty -> empty.

Deux discriminants importants : flags n'accede a aucun environnement, et limit
appelle le MEME constructeur que start sans perte observee. Le constructeur
seul n'est donc pas une explication suffisante. Le selfcheck prouve par ailleurs
que le host peut transporter une variable vide jusqu'au faux enfant ; il ne
dispense pas de diagnostiquer ce changement parent. Ne pas desactiver/normaliser
l'assertion, ni confondre absent et vide pour faire passer les tests.

## Hypothese nouvelle, falsifiable

Chercher d'abord un effet de bord ou d'observation a une frontiere PowerShell
(entree/sortie de fonction, consommation de son retour, extraction d'une map,
initialisation paresseuse du provider ou emission/serialization du rapport),
plutot qu'attribuer une mutation a ProcessStartInfo ou au parser sans preuve.
Le retour string de flags, le retour map de payload et l'extraction de map dans
start fournissent des temoins distincts ; limit fournit le controle constructeur.
Ce n'est PAS une cause trouvee. L'experience ci-dessous doit localiser le PREMIER
changement, ou refuter cette piste si toutes ses etapes preservent le vide.

## Experience minimale proposee au lead, pas d'edition du harnais fige

Un programme diagnostique separe sous un dossier out/ possede, pas une nouvelle
suite de tests ni une version affaiblie de native_fixture.ps1. Meme Python et
Windows PowerShell deja identifies. Cinq cellules au maximum, chacune dans un
PowerShell FRAIS, afin qu'un effet precedent ne contamine pas les suivantes :

1. **return-control** : affectation directe d'un literal, puis fonction pure qui
   renvoie ce meme literal. Tracer avant appel, entree de la fonction, juste
   AVANT return, juste APRES affectation du retour au caller. Pas de constructeur,
   fichier payload ou commande JSON. Le controle ne charge pas le produit.
2. **flags-pinned** : charger seulement les quatre definitions NATIVE de la source
   inchangee, apres verification explicite du pin et de leurs quatre noms comme
   dans le harnais revu. Aucun patch/instrumentation cache du texte epingle.
   Appeler seulement Resolve-VerificationPhaseFlags avec un map nominal factice.
   Tracer avant chargement, apres chargement, avant appel, apres retour assigne,
   apres construction d'un petit objet de rapport. Le mode legacy ou run valide
   suffit ; aucune action prepare/build/run n'est executee par la fonction.
3. **serialize-control** : petit rapport factice compose de string/map et des
   snapshots de marqueurs ; tracer avant construction, apres construction,
   avant ConvertTo-Json et apres affectation de sa sortie. Conserver separement
   le JSON controle pour comparaison cote Python. Ne PAS serializer le journal
   des traces avec ConvertTo-Json, sinon l'observateur utilise la frontiere testee.
4. **constructor-control** : ProcessStartInfo uniquement, JAMAIS Start. Tracer
   avant/apres new, avant/apres premiere lecture EnvironmentVariables, apres
   Clear, apres ajout des entrees factices, apres lecture des trois cles choisies,
   apres extraction de ces seules entrees dans un map. Aucun dump des autres
   variables ; le dictionnaire enfant reste exclusivement factice explicite.
5. **getters-control** : creer UN Process non associe, jamais Start/Kill, puis
   comparer adapter Id/get_Id et adapter HasExited/get_HasExited ; relever
   seulement null/booleen/type d'exception, pas une valeur de PID. Dispose de cet
   objet possede en finally, sans recherche de processus ou operateur tiers.

Le parent Python fournit explicitement l'environnement du seul PS diagnostique :
SystemRoot/WINDIR deja publics, PATH Python fixe, TEMP/TMP dans ce dossier, et
ARTIST_NATIVE_KEEP='owned', ARTIST_NATIVE_EMPTY='', ARTIST_NATIVE_ABSENT omis.
Ne pas heriter/dumper l'environnement du lead, ne jamais modifier ce dernier,
ne pas reintroduire le vide par SetEnvironmentVariable dans le PS fils.
Comparer l'environnement Python avant/apres en memoire avec un booleen seulement.
Un watchdog fini (p.ex. 10 s/cellule) tue uniquement ce PowerShell diagnostique
possede si necessaire ; aucun enfant n'etant lance par lui, pas d'orphelin natif
a recreer dans cette experience. Ne pas lancer le full helper ou le faux enfant.

### Observation sans fonction de trace ni pipeline de serialization

Chaque snapshot doit etre INLIGNE : une fonction de trace invoquee par pipeline
avant la fonction cible pourrait provoquer le phenomene qu'on cherche. Lire le
vide AVANT allocation d'une collection de traces, pour un vrai point initial.
Exemple de mesure a repeter aux seules etapes fixes ci-dessus (tags litteraux) :

```powershell
$empty = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
$isNull = [object]::ReferenceEquals($empty, $null)
$length = -1
if (-not $isNull) { $length = $empty.Length }
$keepOK = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_KEEP', 'Process') -ceq 'owned'
$absentOK = [object]::ReferenceEquals(
    [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_ABSENT', 'Process'), $null)
# Ajouter ces booleens/longueur + tag fixe a la trace, pas la valeur d'env.
```

Conserver la trace en lignes de tags/booleens/longueurs et la deposer par
[IO.File]::WriteAllText dans un fichier propre avec non-ecrasement verifie,
sans ConvertTo-Json/Set-Content/Write-Output pour cette trace. La cellule serialize
garde SEULE sa conversion JSON intentionnelle. Cote Python, comparer trace et
JSON du petit rapport, sans relancer de fonction .NET/PS pour reinterpretation.
Absence de premier snapshot vide = experience invalide, pas assertion ignoree.

## Getters explicites proposes, PAS encore appliques au produit/fixture

Preflight du produit, apres diagnostic/revue source/pin seulement :

```powershell
$associated = $false
try { [void]$Process.get_Id(); $associated = $true }
catch {
    if ($_.Exception.GetBaseException() -isnot [InvalidOperationException]) {
        throw 'InvalidVerificationProcess: cannot inspect owned process.'
    }
}
# associated=true uniquement si l'appel .NET reussit, pas si l'adapter rend null.
```

Pour etat/cleanup, proposer la meme distinction avec `.get_HasExited()` aux
points actuellement adaptes, dans le produit ET le harnais a revoir separement.
Un InvalidOperationException sur l'objet non associe ne doit pas se transformer
en owned_alive=true, ni en tentative de tuer un enfant suppose. Une lecture
reussie retourne un booleen .NET ; les autres exceptions restent des pannes.
Ne pas modifier l'assertion de vie du test pour admettre la valeur adaptee null.

Ces snippets de remplacement sont des propositions, pas un correctif commite.
Le produit/pin ne changent qu'apres le diagnostic et une nouvelle admission
explicite. Si une instrumentation de la region est finalement necessaire,
presenter son diff/digest comme nouvelle source diagnostique a revoir ; ne PAS
recalculer silencieusement le pin fonctionnel pour l'executer.

## Decision selon la premiere divergence

- Si return-control perd le vide : isoler la frontiere PS/host avant toucher au
  parser/constructeur. Repeter uniquement cette cellule minimale, pas la suite.
- Si flags-pinned seul diverge : entree/retour du code reel est localise mais
  mecanisme encore inconnu ; demander une instrumentation explicite des quelques
  instructions de flags, jamais un chargement du helper complet.
- Si trace brute reste vide mais JSON devient null : frontiere d'emission,
  pas preuve de mutation parent. Corriger le producteur de rapport sans rendre
  les assertions permissives ; garder le controle d'etat brut.
- Si seul constructor-control diverge : le premier sous-stage new/get/Clear/Add
  distingue la cause ; les resultats limit existants restent un contre-temoin
  a expliquer avant une correction du constructeur.
- Si aucune cellule ne diverge : garder la cause NON ATTRIBUEE ; proposer ensuite
  uniquement la reproduction du branchement exact de fixture avec snapshots
  inlines, diff diagnostique explicite a revoir. Pas de nouvelle suite moteur.

La cellule getters doit confirmer la difference null adapte/exception explicite.
Un echec du controle n'autorise aucun Kill tiers ni reinterpretation du compteur
owned_alive_after_call. Le risque Dispose synchrone avant Wait face aux writers
herites reste ouvert ; cette experience sans Start ne le qualifie pas.

## Matrice et restitution attendue

Auteur : lecture du source et analyse des logs/resultats EXISTANTS via Context
Mode uniquement. Aucun getter/constructeur, cellule diagnostique, test, enfant,
helper, MSVC/vcvars/outil moteur/GL ou installation execute dans cette remise.

Runner lead : rapporter par cellule le premier stage divergent, les seuls etats
des trois marqueurs, getter null/exception/booleen, identites source/script/PS et
logs bruts propres. Pas de verdict GREEN, pas de relance automatique des 37 avant
correction et nouvelle admission. Aucun schema core/parser/Windows adapter touche.
