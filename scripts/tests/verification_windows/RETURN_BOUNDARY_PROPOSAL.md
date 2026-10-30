# Retour PS — proposition reduite, lead runner seulement

**Plan de deux cellules epuise :** les diagnostics lead inline et dot-source ont
montre NULL des callee-entry ; dot ne contourne pas l'observation. Ne pas rejouer
dot/InvokeReturnAsIs ou appliquer des changements de callsites. Proposition
courante : `READONLY_ENV_PROPOSAL.md`, presence ENV vs observation de chaine,
une cellule readonly a revoir, aucun run auteur.

**Correction diagnostique courante :** `PROBE_FIX_READY.md`. La tentative
Mode=function sur 2e5b18a a refuse une precondition AVANT experience, sans trace ;
elle est INCONCLUSIVE, pas une preuve d'entree/retour. Aucun temoin n'a ete lance.

## Preuve reutilisee, pas de nouvelle campagne

Deux cellules lead deja executees, source diagnostique lead
`.opencode/evidence/mission-uniform-pipeline/diagnostics/windows-env-getters.ps1`.
Logs existants relus via Context Mode :
`out/verification-synthetic/windows-env-getters-diagnosis-2026-10-30T03-32-22-311Z/`.

- return.trace : initial vide present longueur0, affectation directe le conserve,
  retour d'une fonction pure donne absent ; keep/absent temoins corrects.
- getters.trace : Id/HasExited adaptes null, getters explicites .NET levent
  InvalidOperationException sur l'objet non associe ; parent vide preserve.

Getter fautif confirme ; la perte vide est reproduite SANS produit. Ni parser
ni constructeur ne doivent recevoir un correctif d'environnement sur cette base.
La tentative produit n'est pas GREEN, les 37 anciens restent non reexecutes.

## Une experience + UN temoin conditionnel au maximum

`return_boundary_probe.ps1` est une proposition diagnostique autonome, pas un
test fonctionnel ou un changement de native_fixture/pin. Auteur : parsing
statique seulement. Le lead revoit avant run. Aucun produit charge, fonction
native, enfant, ProcessStartInfo, PID, Start/Kill, JSON ou modification env.

Le script trace INLINE, sans fonction de trace : caller-before, callee-entry,
callee-before-return, caller-after, etat initial et temoins keep/absent.
Seulement null/longueur/booleens synthetiques, aucun dump d'environnement.
Il persiste initial, after-strict et caller-before par IO owned CreateNew/Flush
AVANT le refus de precondition des marqueurs. Si ce refus arrive, un statut
experiment-not-run est aussi conserve par finally ; la garde reste active et
aucun appel fonction/dot/InvokeReturnAsIs ne peut la contourner.

1. Executer seulement Mode=function dans un PS frais assaini.
2. Si la premiere perte est des callee-entry, choisir **dot**, PS frais : le meme
   corps est appele au scope du caller sans fonction imbriquee supplementaire.
3. Si callee-entry et before-return preservent le vide mais caller-after le perd,
   choisir **return-as-is**, PS frais : le MEME ScriptBlock est invoque par
   InvokeReturnAsIs, sans pipeline explicite d'enumeration du resultat.
4. Si setup/caller-before perd deja le vide, ou si l'instrumentation fait
   disparaitre le phenomene, NE PAS enchaîner les alternatives : rapporter ce
   contre-resultat/effet d'observation. Aucun cinq-cellules rejoue.

Le temoin ne prouve qu'une semantique locale sur literal. Meme s'il preserve le
vide, il n'autorise pas un remplacement massif de callsites ni un changement du
profil host : il faut proposer un diff local explicite puis une nouvelle preuve
sur les contrats existants, sans modifier les assertions ou le sens vide/absent.
Si les deux chemins perdent le vide, conserver la limite de host et presenter au
lead le plus petit arbitrage necessaire ; aucun fallback global/install/PInvoke.

## Commande exacte proposee au runner lead

Utiliser le meme Python et le meme lanceur PowerShell assaini que sa precedente
experience (stdout/stderr/manifest et timeout 10 s conserves). Cwd : ce worktree.
Environnement PS explicite : SystemRoot/WINDIR, PATH Python, TEMP/TMP possedes,
ARTIST_NATIVE_KEEP=owned, ARTIST_NATIVE_EMPTY chaine vide, ARTIST_NATIVE_ABSENT
absent. Aucun heritage/dump de l'environnement lead, aucune restauration par
SetEnvironmentVariable/PInvoke. Le watchdog possede seulement le PS sans enfant.

Creer un dossier neuf sous
`out/verification-synthetic/windows-return-boundary-fixtures/` pour chaque cellule.
Le probe n'ecrase jamais de trace et refuse une feuille reparse. Aucun dossier
ou lien vers un contenu etranger ne sert de fixture. Invocation a fournir au
lanceur existant, jamais depuis un PS parent non assaini :

```powershell
C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File scripts/tests/verification_windows/return_boundary_probe.ps1 -Mode function -CaseDirectory '<dossier-neuf-absolu-possede>'
```

L'alternative selectionnee remplace uniquement Mode et le dossier dans une
nouvelle invocation assainie ; ne pas lancer les trois modes automatiquement.
Une precondition invalide est une erreur de diagnostic, pas assertion ignorable.
Inspecter les quatre champs initial/caller-before, pas seulement null : le
precedent refus pouvait aussi venir de keep ou absent. Ne pas attribuer ce refus
a une perte du vide tant que la trace persistee ne le montre pas.

## Limites conservees

Produit/getters corriges et nouveau pin restent a relire ; aucune execution
auteur ni nouveau resultat GREEN. Aucune modification de test/assertion ou
restauration de marqueur. Test06 et tout nouveau parent/descendant reel restent
NON_EXECUTE. Le risque Dispose avant pending.Wait face aux writers herites reste
ouvert ; ce probe sans enfant ne le qualifie pas. Helper complet toujours bloque.
