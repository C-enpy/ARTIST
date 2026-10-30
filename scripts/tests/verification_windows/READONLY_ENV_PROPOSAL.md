# Observation readonly ENV — proposition, code produit gele

## Resultats lead reutilises

Traces existantes relues via Context Mode, aucune nouvelle execution auteur :

- `windows-return-inline-diagnosis-2026-10-30T03-42-43-811Z/` : initial vide present,
  callee-entry NULL, avant return NULL, caller NULL ; temoins keep/absent valides.
- `windows-return-dot-diagnosis-2026-10-30T03-43-00-307Z/` : meme sequence en dot-source.

Ces lectures de retour de GetEnvironmentVariable ne suffisent PAS encore a
conclure que la cle a ete supprimee du bloc ENV reel. Elles prouvent une difference
observee des l'entree de scope. Dot-source ne contourne pas le probleme ; les deux
cellules utiles sont epuisees. Aucune relance automatique dot/InvokeReturnAsIs,
aucun changement massif de callsite, aucune restauration ou normalisation.

Getters produit/fixture et pin restent stables. Aucun nouveau resultat GREEN ou
relance des 37. Le probe corrige `eed329517fc3876d7fcbae2f91489cd90add5a1d` conserve
initial et caller-before avant son refus ; il n'a pas ete execute par l'auteur.
Le champ fautif de l'ancienne precondition reste inconnu sans cette trace :
EMPTY, longueur, keep ou absent. Ne pas conclure retrospectivement EMPTY absent.

## Une seule cellule readonly proposee au lead, apres revue

Reutiliser son diagnostic fonctionnel avec un PS frais assaini et la fonction
pure Get-OwnedLiteral, PAS le produit, PSI ou enfant. Les observations ci-dessous
sont INLINE : initial/caller-before, entree de la fonction, apres retour caller.
La fonction cible retourne seulement son literal habituel ; aucune fonction PS
ne sert a retourner la valeur d'environnement ou a produire la trace.

Comparer dans cet ordre, a chaque stage :

1. Getter actuel assigne dans une variable, null/longueur/keep/absent (temoin).
2. Getter .NET a signature explicite, null et egalite a la chaine vide evalues
   directement en booleens .NET, sans emission d'une chaine par une fonction PS.
3. Snapshot Process neuf de GetEnvironmentVariables, presence de la cle vide
   dans le dictionnaire (booleen retourne par les operations .NET), valeur nulle/
   vide uniquement pour les TROIS marqueurs factices. Pas de dump des autres cles.
4. Relecture du getter actuel APRES snapshot pour detecter si l'observateur lui-
   meme change le resultat. Un retour vide reapparu a ce point ne serait pas
   automatiquement une mutation/restauration OS : signaler l'effet d'observation.

Le snapshot provient de l'API .NET de l'environnement du processus ; il apporte
une presence de cle independante du seul resultat string du getter assigne.
Ce n'est pas un nouveau helper Win32 compilé ni une preuve brute GetEnvironmentVariableW.
Si les observateurs sont contradictoires, rester INCONCLUSIVE au lieu de declarer
une suppression OS. Aucun PInvoke (lecture ou ecriture), csc/Add-Type/install.

### Getter type : setup fixe, public et readonly, pas reflection arbitraire

Snippet propose, NON_EXECUTE. Creation une fois APRES l'observation initiale
existante ; tracer aussi avant/apres ce setup pour ne pas cacher son effet.
Seul le nom/type ci-dessous est admissible, aucun nom fourni par payload :

```powershell
$signature = [type[]]@([string], [EnvironmentVariableTarget])
$method = [Environment].GetMethod('GetEnvironmentVariable', $signature)
if ($null -eq $method) { throw 'HARNESS: exact public getter unavailable.' }
$typedGetter = [Delegate]::CreateDelegate(
    [Func[string, EnvironmentVariableTarget, string]], $method)
# A recopier inline a chaque stage, pas dans une fonction d'observation PS.
$typedNull = [object]::ReferenceEquals(
    $typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), $null)
$typedEmpty = [StringComparer]::Ordinal.Equals(
    $typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), [string]::Empty)
```

Les deux lectures sont distinctes : typedNull=true ET typedEmpty=true serait
incoherent et doit etre rapporte, pas force dans une classe vide/absent.
Ne PAS caster le resultat en [string], ce qui pourrait convertir null en vide.
Une indisponibilite de signature/delegate est une limite de l'experience, pas
permission d'installer ou de substituer une fonction PS au delegate.

### Presence par snapshot Process, sans return PS de chaine

Exemple pour EMPTY uniquement ; appliquer les memes lectures inline aux deux
temoins fixes, sans publier aucune autre entree du snapshot en memoire :

```powershell
[Collections.IDictionary]$block = [Environment]::GetEnvironmentVariables(
    [EnvironmentVariableTarget]::Process)
$matches = 0
$blockNull = $null
$blockEmpty = $null
foreach ($key in $block.Keys) {
    if ([StringComparer]::OrdinalIgnoreCase.Equals($key, 'ARTIST_NATIVE_EMPTY')) {
        $matches += 1
        $blockNull = [object]::ReferenceEquals($block.get_Item($key), $null)
        $blockEmpty = [StringComparer]::Ordinal.Equals($block.get_Item($key), [string]::Empty)
    }
}
$block = $null
# Persister seulement matches/blockNull/blockEmpty et les temoins connus.
```

matches=0 signifie aucune cle de ce nom dans ce snapshot, matches=1 une cle
presente ; >1 est anomalie a conserver, jamais effacer par normalisation de casse.
Ne pas assimiler cle presente a valeur vide sans les autres observations. Un
snapshot contradictoire avec les getters reste a expliquer (ordre/representation).

## Persistance et preconditions

Reprendre la correction de PROBE_FIX_READY : destination owned valide, IO
CreateNew, trace bool/longueur initiale et caller-before Flush AVANT tout throw
de precondition marqueur, finally de persistance/fermeture. Pas de fonction de
trace, pipeline JSON ou dump. Chaque nouveau reader a un tag d'ordre fixe.

Environnement du seul PS diagnostique assaini explicitement par le runner :
SystemRoot/WINDIR, PATH Python fixe, TEMP/TMP propres, KEEP=owned, EMPTY='',
ABSENT omis (noms complets ARTIST_NATIVE_*). Aucun env lead herite/dumpe ou
modifie ; aucune restauration de EMPTY meme si un reader le dit absent.
Si initial/caller-before sont invalides, persister quel champ echoue et STOP :
aucune fonction cible ni temoin supplementaire. Pas de relance des cinq cellules.
Le premier point du corps reste APRES param binding/launch : conserver l'argv
assaini et distinguer -File direct d'une commande PS qui aurait appele le script.

## Interpretation et invariant parent, a ratifier par le lead

- Getter assigne NULL, mais typed getter vide et cle presente/vide dans le
  snapshot : piste observation/representation, PAS preuve de mutation ENV reel.
  Proposer ensuite seulement une lecture locale plus fidele sous revue, sans
  affaiblir l'assertion parent ou redéfinir vide=absent.
- Les lecteurs concordent sur une cle initialement presente/vide puis absente
  au stage entree : preuve plus forte d'une limite host affectant le processus
  reel. Faire ratifier au lead la frontiere de l'invariant parent ; aucun env
  restore/PInvoke ni nouveau contrat applique silencieusement.
- Snapshot presente mais valeur NULL adaptee, lecteurs contradictoires, ou
  resultat change apres enumeration : INCONCLUSIVE, expliquer l'observateur
  avant de conclure. Pas de nouvel essai automatique ou changement global de host.

## Pourquoi limit/selfcheck ne resolvent pas encore le paradoxe

Le harnais natif utilise le MEME getter assigne pour parent_before et parent_after
dans toutes les branches. Les succes limit/selfcheck sont de vrais contre-temoins
a toute affirmation que chaque appel PS detruit toujours le vide. Ils ne prouvent
pas pour autant la representation du bloc ENV au stage callee-entry (non mesure).
Les chemins different : limit consomme un ProcessStartInfo, flags un literal,
payload appelle des lecteurs de chaines imbriques ; selfcheck lance son enfant
avec un environnement EXPLICITE. L'observation Python du vide dans cet enfant
prouve son transport explicite, pas l'etat ENV du parent a chaque scope.

Ne pas choisir une explication par type de retour ou effet du lancement sans
mesure. La cellule readonly doit d'abord separer presence ENV et observation de
chaine. Aucun PSI/enfant/selfcheck ne sera rejoue pour expliquer ce paradoxe ;
les logs existants restent la preuve dans leur seule portee.

## Matrice de cette remise

Auteur : source/notes seulement et lecture de traces lead EXISTANTES via Context
Mode. Aucun snippet readonly, delegate/getter/snapshot, probe, produit, enfant,
Start/Kill, outil moteur/MSVC/vcvars/GL ou installation execute.
Code produit/fixture/tests/assertions/pin inchanges. Proposition d'une cellule,
pas un outil integre, une nouvelle suite ou une autorisation runtime. Revue lead
requise avant admission ; le risque Dispose/writers herites reste ouvert.
