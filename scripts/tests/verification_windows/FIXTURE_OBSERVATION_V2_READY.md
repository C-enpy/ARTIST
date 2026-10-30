# FIXTURE_OBSERVATION_V2_READY — snapshot seul oracle parent

**SOURCE_READY Windows INTERMEDIAIRE**, pas reception finale. Code candidat :
`78c91d698be97b7d38ac780971924e0c246ee5ac`. Correction documentaire du seul
demonstrateur annoncee avant revue independante ; aucun test/fixture/produit
modifie pour l'obtenir, aucune relance demandee ou executee par l'auteur.

## GREEN attribue au lead

Le lead a observe selfcheck1/1 + contrats13/13 + anciens37/37 = **51 PASS**,
zero failure/error/skip, empreintes stables sur le candidat ci-dessus.
Test06 reparse est NON_EXECUTE, pas un SKIP ajoute.
Preuve : `out/verification-synthetic/windows-observation-v2-object-compare-2026-10-30T04-34-29-285Z/`.
Manifest et journaux existants relus via Context Mode, sans nouvelle execution.

Avant ces tests, le demonstrateur textuel original avait echoue sur EMPTY/null.
Le lead a caracterise en readonly la forme d'appel PowerShell :
StringComparer.Ordinal.Equals('', null)=True par binding, alors que
Object.Equals('', null)=False et ReferenceEquals('', null)=False. Ce constat
porte sur cette invocation PS, pas sur un changement du contrat .NET du comparer.
Le lead a corrige UNIQUEMENT son snippet de demo vers Object.Equals et observe
PASS ; scripts/commandes sont conserves dans le manifest. Aucun fichier de test,
fixture ou produit n'a ete modifie par cette correction de demonstration.

## Cause de l'arret lead

Tentative lead sur a5628d, fixture identique a 6a19ad7 : selfcheck14 seul,
1 failure HARNESS96 ; aucun des 13 contrats restants ou anciens37 lance.
Logs existants relus via Context Mode :
`out/verification-synthetic/windows-observation-green-2026-10-30T04-23-29-066Z/`.
Le cas pin modifie est refuse comme attendu. Pour selfcheck, snapshot EMPTY
present/vide et sanitized_initial=true, lecture assignee longueur0, mais getter
type NULL/non-vide/non-matching, avant et apres. Ce desaccord n'est ni preuve
de suppression ENV ni RED produit ; la garde d'observation V1 etait fautive.

Decision lead appliquee : snapshot Process neuf autoritatif, les deux getters
restent DIAGNOSTIQUES. Aucun mecanisme host n'est attribue plus loin, aucune
nouvelle campagne ni restauration/normalisation d'environnement.

## Diff borne

Dans native_fixture.ps1 seulement :

- deux gardes avant/apres ne refusent PLUS sur typed_null/empty/matches ;
- elles conservent le refus d'une valeur snapshot presente non string ;
- commentaire de l'oracle corrige, aucune autre logique/modification de process.

Snapshot neuf, Contains/get_Item sur les quatre seuls noms, absence null/vide '',
precondition initiale exacte et maps avant/apres sont INCHANGES. Lectures et
diagnostics assigned/typed restent visibles dans parent_observation. Un desaccord
de ces champs seul ne vaut plus HARNESS ; ce n'est pas un accord artificiellement
force a true ni une valeur vide reconstruite a partir d'un getter NULL.

Les echecs de lecture du snapshot, types invalides et precondition initiale
incorrecte conservent le chemin HARNESS96. Apres appel, toute vraie modification
du map reste detectee par l'assertion avant/apres existante, pas une garde qui
redefinit la mutation comme autorisee. Aucun cast [string] normalisant.

Produit, pin, cinq substitutions de getters relues, tests/assertions15 et37,
enfant, autres fixtures/audits, delais et protocoles inchanges. Pas de nouveau
processus, pipeline, outil, Env restore/PInvoke/install ou harness general.

## Conservation de l'oracle et limite explicite

L'oracle est une map complete de KEEP/EMPTY/DROP/ONLY prefixes ARTIST_NATIVE_.
Les valeurs sont celles du snapshot Process, sans filtrer les differences :

- avant EMPTY='' / apres EMPTY absent => '' contre null : echec de comparaison ;
- avant KEEP='parent' / apres KEEP='Parent' => strings distinctes : echec ;
- avant ONLY absent / apres ONLY='' => null contre '' : echec ;
- quatre entrees inchangees => egalite, sans consulter le verdict du getter.

Le snapshot est l'API d'observation retenue par le lead, pas une preuve Win32
independante obtenue ici. Les formes de getter precedemment essayees sont
instables a ce callsite et ne valident/ne refutent plus cet oracle. On conserve
leurs resultats pour diagnostic, sans les utiliser pour ignorer une difference
du snapshot ou inventer une violation ENV. Les assertions metier ne changent pas.

## Demonstrateur sur maps synthetiques — correction doc observee par le lead

NON_EXECUTE par l'auteur et non raccorde a la fixture : aucun nouveau test/methode,
process, lecture ENV ou OS write. Il reprend le meme Contains/get_Item sans cast.
Le premier map represente avant ; les deux autres des variantes apres. Les
comparaisons Object.Equals par cle gardent strings case-sensitive et null
distinct de vide, sans le binding string observe avec Ordinal.Equals. La vraie
comparaison avant/apres reste l'egalite de dict Python des tests INCHANGES.

```powershell
$names = @('ARTIST_NATIVE_KEEP', 'ARTIST_NATIVE_EMPTY', 'ARTIST_NATIVE_DROP', 'ARTIST_NATIVE_ONLY')
$baseline = @{ ARTIST_NATIVE_KEEP = 'parent'; ARTIST_NATIVE_EMPTY = ''; ARTIST_NATIVE_DROP = 'parent-only' }
$blocks = @($baseline.Clone(), $baseline.Clone(), $baseline.Clone())
$blocks[1].Remove('ARTIST_NATIVE_EMPTY')
$blocks[2]['ARTIST_NATIVE_KEEP'] = 'Parent'
$observed = @()
foreach ($block in $blocks) {
    $map = @{}
    foreach ($name in $names) {
        $value = $null
        if ($block.Contains($name)) {
            $value = $block.get_Item($name)
            if ($value -isnot [string]) { throw 'DEMO: present snapshot value must be string.' }
        }
        $map[$name] = $value
    }
    $observed += ,$map
}
if ($observed[0].Count -ne 4 -or
    -not [object]::Equals($observed[0]['ARTIST_NATIVE_EMPTY'], [string]::Empty) -or
    -not [object]::ReferenceEquals($observed[0]['ARTIST_NATIVE_ONLY'], $null) -or
    -not [object]::ReferenceEquals($observed[1]['ARTIST_NATIVE_EMPTY'], $null)) {
    throw 'DEMO: empty/absent representation was collapsed.'
}
$emptyDifferences = @()
$keepDifferences = @()
foreach ($name in $names) {
    if (-not [object]::Equals($observed[0][$name], $observed[1][$name])) {
        $emptyDifferences += $name
    }
    if (-not [object]::Equals($observed[0][$name], $observed[2][$name])) {
        $keepDifferences += $name
    }
}
if ($emptyDifferences.Count -ne 1 -or $emptyDifferences[0] -cne 'ARTIST_NATIVE_EMPTY') {
    throw 'DEMO: empty-to-absent was not detected exactly.'
}
if ($keepDifferences.Count -ne 1 -or $keepDifferences[0] -cne 'ARTIST_NATIVE_KEEP') {
    throw 'DEMO: KEEP case-sensitive change was not detected exactly.'
}
```

Observe LEAD sur la demo corrigee : map complet/valeurs preservees ; variantes
apres detectees exactement sur EMPTY et KEEP. NON_EXECUTE par l'auteur ; ce
demonstrateur ne remplace pas les assertions15 actuelles. Les iterations portent
sur trois donnees fixes, pas une boucle d'essais OS/host. Aucun nouvel outil.

## Commandes de reprise conservees — deja executees lead, pas demande de rerun

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline/scripts/tests/verification_windows`

Via Context Mode, en conservant logs/identites source/fixture/enfant :

```powershell
C:/Python313/python.exe -B -m unittest -v test_native_contracts.WindowsNativeContractTests.test_14_review_pin_and_independent_native_fixture_selfcheck
```

Stop a son premier echec causal ; pas de nouveau diagnostic de host automatique.
Seulement si PASS, reutiliser les 13 noms deja selectionnes hors test06 et sans
rejouer14, puis les anciens37. Pas de discovery native globale. Commande37 :

```powershell
C:/Python313/python.exe -B -m unittest -v test_psbase_audit test_targeted_contracts test_wrappers test_dispatch test_helper_contract
```

Observe LEAD sur le code 78c91d6 : 1+13+37 passes, zero failure/error/skip.
Ce correctif doc ne cree pas une nouvelle execution ni une nouvelle preuve
sur un autre candidat. Test06 reste NON_EXECUTE, aucun SKIP ajoute.

Auteur : lecture des logs EXISTANTS + diff/parsing statiques seulement. Aucun
demonstrateur, fixture, selfcheck, test, enfant ou moteur execute. Helper complet
et mecanismes reels MSVC/vcvars/OS-API non qualifies restent bloques/SOURCE_ONLY.
Les 51 tests synthetiques ne prouvent ni rendu, backend, performance ou qualification
MSVC. Les getters types restent diagnostiques ; oracle metier = dict Python.

Dispose lecteur avant Wait/writers herites reste non qualifie pour la capture
privee. Selon le binding ratifie `bac910520118676951a8a37d45260af03f4ad268`,
helper-run/capture privee n'est plus critere du run commun DIRECT ; ce gap est
conserve, pas efface et ne justifie pas supprimer les fonctions existantes.
Aucune implementation windows.py ou interface future avant point Git/QA coordonne.
