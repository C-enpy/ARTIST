# Tranche consolidee Windows OS/transport — plan et RED_READY propose

**Admission revisee apres revue lead :** `HARNESS_FIX_READY.md` fait foi pour
les delais et la commande. Test_06 reparse reste NON_EXECUTE ; selection
explicite des 14 autres methodes seulement. La proposition jonction ci-dessous
n'est pas autorisee a l'execution pour cette admission et ne couvre aucun
des deux symlink core SKIP. Ne pas lancer la discovery globale historique.

## Entrees et mandat

Base : `769a55a8ae98d5425ec5625c3577fb39acca31a4`.
GREEN lead 37/37, zero error/skip, empreintes et anciennes assertions stables.
Logs annonces : `out/verification-synthetic/windows-targeted-green-2026-10-30T01-54-56-375Z/`.
Keys/cibles sont qualifies seulement pour les maps et commandes factices.

Demande suivante : traiter ensemble transport/flags/natif/env/logs/timeout,
sans encore windows.py (core commun non integre), ni vrai MSVC/vcvars/outils,
installation, moteur, cloud ou helper complet. L'auteur ne lance aucun test.
Le nouveau confinement ci-dessous est une **proposition a relire avant run**.

## Une tranche, quatre entrees RED

Le helper ajoute uniquement quatre definitions a refus explicite, region NATIVE.
Ce sont points d'entree delibérés, pas erreurs d'import/dependance :

1. Read-VerificationPayloadFile(InputFile, BundleRoot) : fichier UTF-8 strict,
   <=1048576 octets, objet a quatre champs existants build_directory/targets/
   executable/arguments. Aucune copie Request, receipt ou modele core. Rejeter
   noms inconnus/dupliques (casse comprise), texte malforme, octets UTF-8 invalides,
   escape hors bundle et tout composant reparse. La validation logique existante
   des valeurs et scalaires reste celle de Resolve-VerificationInputs.
2. Resolve-VerificationPhaseFlags(Flags) : retourne seulement legacy/prepare/
   build/run, pas un nouveau schema. Les noms de flags suivent le helper :
   VerificationPhase, VerificationInputFile, VerificationBundleRoot,
   VerificationLogDirectory, VerificationTimeoutSeconds, Configuration, Jobs,
   AllowDependencyResolution et les flags historiques. Aucune declaration CLI
   nouvelle ni suppression du refus global avant revue du raccordement final.
3. New-VerificationNativeStartInfo(Program, CommandLine, WorkingDirectory,
   Environment) : objet standard ProcessStartInfo, pas nouveau CommandSpec ;
   UseShellExecute=false, redirects, CreateNoWindow=true, env enfant explicite
   apres Clear(). Ne jamais muter le vrai environnement du PowerShell parent.
4. Invoke-VerificationNativeProcess(Process, LogDirectory, TimeoutMilliseconds) :
   recoit l'objet Process **cree et possede par l'appelant**, non demarre,
   StartInfo deja valide. Preflight logs avant Start, capture stdout/stderr en
   octets sans troncature/deadlock, arret de cet enfant sur timeout et drain.
   Resultat de fonction interne : returncode int/null, timed_out bool,
   stdout_path/stderr_path ; pas de schema de receipt ou resultat workload.

Le refus complet avant dispatch reste en place. Aucun des quatre corps n'est
GREEN/operationnel. Les anciens audits/37 tests restent inchangés.

## Flags/transport et limites exactes

- Aucun flag verification sans phase ; aucun melange ConfigureOnly/ASan/
  preflight avec le mode verification ; flag inconnu refuse.
- Prepare exige l'opt-in dependency resolution avant effets ; build/run
  n'acceptent aucun bootstrap. Le mode legacy n'est qu'un resultat de validation
  des anciens flags, jamais une campagne executee par la fixture.
- InputFile, BundleRoot, LogDirectory requis pour les phases verification ;
  timeout >0 et <=3600 secondes, jobs bornes. Le transport est un fichier, pas
  du base64 qui reduirait artificiellement les valeurs Unicode admissibles.
- 4096 scalaires astraux par valeur restent valides. La limite CreateProcessW
  est verifiee sur la ligne COMPLETE APRES quoting, argv0 et NUL compris,
  <=32767 unites UTF-16, jamais en octets UTF-8. Test constructeur seulement
  pour les limites extremes ; aucune commande geante n'est executee.
- Fichier et logs ne doivent pas etre reconstruits/ecrases depuis une preuve
  ancienne. Les flags/chemins restent privés au helper, pas extensions du core.

## Confinement propose, a arbitrer avant run

Cette tranche n'elargit pas les audits logiques existants en pseudo-sandbox.
La region native, qui devra utiliser des API OS, est **epinglee par SHA256** :
native_source.sha256 vaut actuellement
`7a9bdcddee678471e7577d7812556e77992c5550280352f3e3bb32dfc66f337d`.
Ce digest concerne le texte exact normalise LF de quatre refus RED seulement.
Le lead doit relire tout futur corps GREEN et son nouveau pin avant execution.
Un pin modifie n'est pas une assertion metier affaiblie : c'est une nouvelle
admission de source, obligatoirement explicite. Ce n'est PAS une sandbox contre
du code hostile ; le risque et la confiance de cette proposition sont exposes.

- Source seule chargeable : ces quatre FunctionDefinitionAst distincts, pin
  identique, taille <=65536 caracteres. Jamais le helper complet ou ses branches
  historiques. Un test de pin modifie doit donner 96 avant evaluation.
- Le parent Python lance PowerShell avec un environnement **sane explicite** :
  SystemRoot/WINDIR, PATH Python, TEMP/TMP propres et quatre marqueurs factices.
  Aucun dump ou heritage complet de l'environnement du lead. La comparaison
  avant/apres en Python reste en memoire, assertion booleenne sans diff secret.
- L'enfant autorise reste wrapper_child.py, meme fichier revu, nouveau mode
  opt-in reserve __artist_native_fixture_v1__. Son ancien protocole est inchange.
  Seuls effets : fichiers fixes voisins de native-case.json, flux binaires
  bornes, pause <=5 secondes, code 0/17. Aucun subprocess/eval/import utilisateur.
- Le processus est un objet possede par la fixture, StartInfo valide avant
  appel. FileName = interpreteur Python hashé ; prefixe -I -S -B + enfant fixe
  + config fixe ; cwd/env exacts, aucun shell ni autre programme.
  La fixture reprend/kill/dispose cet objet en finally si capture echoue,
  jamais un PID arbitraire. Un enfant laisse vivant par capture est enregistre
  AVANT ce nettoyage, donc le test ne masque pas la fuite.
- Reparse : jonction NTFS creee uniquement entre deux dossiers de ce cas,
  via New-Item Junction, jamais cmd/mklink ni elevation. Si indisponible,
  HARNESS precondition explicite : ce n'est pas RED de l'implementation.
- Fichiers de test et resultats sous out/verification-synthetic/windows-native-fixtures.
  Toutes les mauvaises destinations testees restent dans le meme cas possede,
  mais hors bundle ; aucun fichier systeme/secret ne sert de contre-exemple.
- Lancement du vrai programme factice seulement : selfcheck independant,
  capture et composition. Les seuls corps RED ne demarrent pas cet enfant.
  NewStartInfo/limites extremes n'appellent jamais Start.

**Question de confinement pour le lead** : accepter cette porte d'admission
source-pin + objets/processus/fichiers bornes apres revue, ou demander une
restriction supplementaire avant run. L'auteur n'a execute ni gate ni selfcheck.
Pas de promesse que le pin remplace une isolation OS ; source GREEN doit aussi
etre relue pour interdire remplacement de Process/StartInfo ou demarrage hors
de l'objet fourni. Il n'est pas permis d'executer un corps simplement parce que
l'auteur a recalcule le pin.

## Suite de validation consolidee

Nouveau test_native_contracts.py, 15 methodes :

- flags nominaux/legacy et demandes invalides/exclusives, opt-in et timeout ;
- UTF-8 fichier, grands scalaires, doublons/unknowns/malforme/size ;
- containment et reparse reel de fixture ;
- env enfant exact, vide present/absent distingue, parents inchanges ;
- 32767/32768 unites UTF-16 apres argv0/NUL ;
- codes 0/17 et octets stdout/stderr complets ;
- deux flux >buffer pipe draines sans deadlock ;
- timeout avec traces partielles deja flushees, readiness et fin d'enfant ;
- journaux anciens jamais ecrases et aucun enfant lance en precondition invalide ;
- composition flags -> fichier -> start info -> capture ;
- selfcheck independant code17/env vide et gate pin avant evaluation.
- six noms CTest publics uniquement s'ils sont explicitement declares pour
  l'enfant ; des valeurs ambiantes synthetiques dans son parent ne les activent pas.

L'enfant timeout est fini (pause 5 s). Le timeout native 3 s est recevable pour
le test de partiels seulement si native-ready.txt confirme son demarrage et le
flush des deux flux : sinon analyser HARNESS/precondition, pas RED invente.
Apres sa pause finie, le fake emet des marqueurs late-* : attendre simplement
sa sortie naturelle puis annoncer un timeout ne satisfait donc pas les octets
partiels attendus. Les grosses ecritures de pipe bouclent sur les short writes.
Le watchdog Python 25 s est distinct, et ne valide jamais le timeout metier.

## Commande proposee au runner lead via Context Mode

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

Seulement apres acceptation du confinement/pin/source/extension enfant :

```powershell
# NE PAS EXECUTER cette ancienne selection globale pour cette admission.
# Utiliser les 14 noms explicites de HARNESS_FIX_READY.md, hors test_06.
```

Attendu par lecture : 15 methodes, 14 rouges / 39 failures avec sous-cas,
1 verte (selfcheck+pin), zero error/skip. Premiere cause attendue : refus
Resolve-VerificationPhaseFlags, puis refus transport/start-info/capture. Les
prefixes InvalidVerificationFlags/Payload/CommandLine/Log des cas negatifs
doivent etre obtenus en GREEN, pas remplaces par le refus generique RED.
Une erreur de gate, jonction, lancement, import, rapport manquant ou timeout
watchdog invalide cet attendu. Les refus ne sont pas des schemas core manquants.

La non-regression 37 precedente doit aussi etre relancee par le lead apres
revue du mode enfant opt-in ; aucune ancienne assertion n'est modifiee.

## GREEN a livrer apres RED, en une correction consolidee

Implementer les quatre fonctions et leur coordination ; adapter le pin dans un
diff d'admission separe et relu. Puis proposer le cablage CLI final avec ces
primitives/env session en memoire, sans PInvoke d'environnement, sans fallback
install et sans nouveau modele Request. Le refus global ne saute qu'apres
revue du cablage et preuves pertinentes. Runtime windows.py attend toujours
le core effectivement integre, pas copie de schema pour contourner l'attente.

Pieces SOURCE_ONLY a relire explicitement si inexécutables dans le mandat :
import vcvars/toolset14.44 et resolution des vrais outils/venv existants,
gestion d'arbre de processus reels (le fake ne cree aucun descendant), et
mapping des captures vers receipts du core. La capture d'un dump complet
vcvars/lead dans les journaux serait interdite : garder l'env de travail en
memoire et ne publier qu'un environnement filtre sans secrets.

Ces pieces ne deviennent pas PASS MSVC parce que le faux enfant fonctionne.
Les preconditions ABI/STL/format/stacktrace restent celles du vrai CMake,
Conan 2 avant configuration, et non des numeros fournis par une map.

## Matrice actuelle

- Ancienne preuve 37/37 : lead sur la base uniquement.
- Nouvelle tranche/native child/gate/jonction : NON_EXECUTE par l'auteur.
- Auteur : lectures, edition, SHA de source et syntaxe statique seulement.
- Helper complet/MSVC/cmd/vcvars/install/CMake/Conan/CTest/moteur CPU/GL : NON_EXECUTE.
- Linux/image/performance : NON_EXECUTE ; aucun Docker/cloud ni reservation.

Remise intermediaire pour revue/RED, pas pipeline Windows complet ni garantie
de confinement contre code arbitraire. Aucun resultat de campagne requis ou affirme.

## Complement de raccordement fourni par le lead, commit 50af792

Contrat lu depuis l'objet Git commite, sans checkout/copie de branche :
`.opencode/evidence/mission-uniform-pipeline/legacy-context-observation-v1.md`.
Les futurs CommandSpec.env_delta CTest doivent transporter depuis le plan et
les receipts verifies, et uniquement s'ils sont declares :

- ARTIST_VERIFICATION_CONTEXT_V1=opengl-hardware ;
- ARTIST_VERIFICATION_RUN_ID ; ARTIST_VERIFICATION_PLAN_SHA256 ;
- ARTIST_VERIFICATION_REQUEST_ID, unique par invocation CTest ;
- ARTIST_VERIFICATION_EXPECTED_GL_VENDOR ; ARTIST_VERIFICATION_EXPECTED_GL_RENDERER.

Les six valeurs ambiantes ne valent pas declaration : les enlever de la base
enfant avant application du seul env_delta explicite, parent inchange. Le test
15 caracterise cette propriete de l'environnement enfant explicite avec six
valeurs **factices**, sans contexte GL ni record d'observation. Il ne pretend
pas avoir valide BuildReceipt ou la qualification hardware.

Autorite des executables : Resolve-VerificationInputs ne fait qu'une validation
de forme/layout. La reconstruction d'un chemin attendu ou son egalite a un argv
factice ne prouve JAMAIS l'autorisation d'executer un workload. windows.py devra
consommer les chemins/empreintes depuis les BuildReceipt verifies par le core,
puis reverifier les identites au lancement ; pas creer un faux receipt local
ni copier un schema pour contourner cette dependance. Les seuls executables
de cette fixture sont des programmes de test identifies, pas des workloads.

Parser contexte/producteur GL : consommation seulement APRES integration du
module du producteur ; aucun parser JSON parallele, aucune edition fixture GL.
Le raccordement CTest runtime et ses validations restent du cote integration.

Le lead signale quatre findings core et un alignement test-first du suffixe
Windows -verification. Les chemins de source/build de ce harnais sont des
donnees factices, pas un choix de racine impose au core. Ne pas cabler l'adaptateur
runtime ou changer cette racine commune sans l'entree integree et son oracle.
