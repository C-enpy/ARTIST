# PROBE_FIX_READY — persistance avant refus de precondition

## Cause observee, sans resultat fonctionnel

Sur `2e5b18a21c12f6216a4db4fbe0bfc02b66ba1717`, le lead a lance seulement
Mode=function. Refus ligne29, code1, AVANT l'experience ; la trace n'etait
ecrite qu'en fin de script, donc perdue. Logs existants analyses via Context Mode :
`out/verification-synthetic/windows-return-boundary-2026-10-30T03-41-39-929Z/`.
Aucun temoin execute. Resultat **INCONCLUSIVE**, ni RED fonctionnel ni preuve
du premier changement entre entree et retour. Getters produit pas rerun.

Le refus etait une disjonction de quatre conditions : initialNull, Length,
keep, absent. Il ne prouve PAS a lui seul que EMPTY etait absent ; un temoin
keep/absent incorrect pouvait aussi suffire. Ne pas choisir la cause sans trace.

## Correction diagnostique seule

`return_boundary_probe.ps1` seulement, plus notes de remise :

- garde initiale inchangee sur null/longueur/keep/absent, deplacee APRES la
  capture caller-before. Elle verifie aussi caller-before pour ne pas tester
  une fonction avec des marqueurs devenus invalides pendant la preparation ;
- initial et after-strict captures inline, et caller-before enrichi de keep/absent ;
- destination owned verifiee comme avant, fichier ouvre CreateNew, aucun ecrasement ;
- initial/after-strict ecrits et Flush, puis caller-before ecrit et Flush AVANT
  tout throw de precondition des marqueurs. Definitions de fonction uniquement
  pendant cette preparation, aucun appel cible tant que la garde n'est pas valide ;
- finally conserve les lignes restantes, notamment experiment-not-run, puis
  ferme writer et fichier. Pas de JSON, fonction de trace, env restore ou PInvoke.

Si la destination est invalide/non owned ou si IO echoue, rester fail-closed :
il serait incorrect d'ecrire ailleurs pour fabriquer une trace. La garantie de
persistance concerne le refus des marqueurs sur destination owned utilisable,
pas un succes IO garanti quand le stockage refuse l'ecriture.

Produit, pin, tests/assertions15/37, native_fixture et enfant **inchanges**.
Aucun Start/Kill/enfant/outilmoteur, jonction ou droit nouveau. Le code n'a pas
ete execute par l'auteur, ni sur cas nominal ni sur precondition invalide.

## Comparaison source avec le script lead commite 047a00d

Fichier lu depuis l'objet Git, sans copier/modifier la source lead :
`.opencode/evidence/mission-uniform-pipeline/diagnostics/windows-env-getters.ps1`.

- Lead : Mode valide mais non Mandatory, OutputFile Mandatory ; StrictMode puis
  ErrorActionPreference, controle File.Exists, puis premier getter EMPTY.
- Probe : Mode Mandatory et CaseDirectory Mandatory ; ErrorActionPreference,
  premier getter EMPTY AVANT StrictMode, puis allocation/controle du dossier.
- Lead : initial ne mesure que EMPTY ; keep/absent sont mesures en fin de cellule.
  Probe : garde initiale sur EMPTY ET keep=owned/absent ; cette difference suffit
  a rendre le refus precedent ambigu, sans prouver un probleme du param binder.
- Probe : recuperation du ScriptBlock fonction et controle owned supplementaires.
  Ces instructions etaient APRES le throw ligne29 de l'ancien probe : elles ne
  peuvent pas expliquer l'echec initial deja observe. La nouvelle mesure
  caller-before pourra montrer un effet plus tardif de preparation, sans le
  confondre avec l'etat initial.

**Premier point mesurable** : le getter EMPTY au debut du corps, APRES parsing,
param binding et toute preparation du launcher/host ; aucune observation de ce
script ne precede ces etapes. initial invalide = verifier d'abord booleens des
trois marqueurs prepares par le launcher et sa commande exacte (-File direct ou
script invoque depuis -Command), sans dump du reste de l'environnement.
Ne pas affirmer que le param binding a supprime EMPTY faute de point anterieur.

initial valide mais after-strict invalide localise entre ces observations
(lectures des temoins et StrictMode inclus, pas une attribution a StrictMode) ;
initial/after-strict valides mais caller-before invalide localise la preparation
restante, qui inclut IO/persistance et definition/recuperation du corps. Ce ne
serait pas encore une preuve de perte a l'entree ou au retour de la fonction.

## Proposition runner lead, apres revue uniquement

Meme invocation Mode=function, meme host frais assaini, mais dossier neuf owned
et source corrigee identifies. Commande/cwd/environnement dans
RETURN_BOUNDARY_PROPOSAL.md ; aucun changement de transport ou restauration des
marqueurs dans ce correctif. Conserver stdout/stderr, manifest et function.trace
meme si code1. Analyser initial, after-strict, caller-before et experiment-not-run.

Si precondition invalide : **stop**, aucun temoin, ni cinq cellules ni doublet
automatique. Le lead compare alors setup/argv de cette cellule a 047a00d et
rend la premiere divergence mesuree, pas une attribution devinee.
Si precondition valide ET experience probante : regle deja proposee d'au plus
un temoin conditionnel en host frais ; aucune nouvelle autorisation automatique.

Matrice auteur : lectures de source/logs existants, diff et parsing statiques
seulement. Nouveau probe : NON_EXECUTE. Aucune nouvelle preuve GREEN/37tests,
aucun changement au risque Dispose/writers herites ou au refus du helper complet.
