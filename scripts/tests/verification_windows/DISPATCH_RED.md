# Deuxieme tranche Windows — RED_READY comportemental sous fixture

## Etat et autorisation

Base de tranche : `01e970b682a90d32e1be71ad997475c9d206bf2c`.
Le lead a observe GREEN statique 5/5, code 0, assertions inchangees et sources
stables ; logs annonces :
`out/verification-synthetic/windows-helper-static-green-2026-10-29T23-15-35-041Z/`.
Cette interface intermediaire n'est pas une fonctionnalite integrable.

Autorisation suivante recue : preparer/ecrire les tests confines ; le lead
examine le harnais et les commandes avant toute execution, puis doit observer
le RED causal avant logique operationnelle GREEN. L'auteur n'execute rien.

## Fichiers et absence de changement operationnel

- `test_dispatch.py` : dix methodes unittest, sous-cas de refus/erreurs/env.
- `dispatch_fixture.ps1` : programme factice possede par les tests.
- Helper : ajout de deux **commentaires marqueurs seulement**, autour du refus
  existant. Aucun nouveau comportement prepare/build/run ni correction de code
  enfant avant le RED lead. Les assertions de premiere tranche sont inchangees.
- Aucun core importe, modele/schema/receipt copie, ni fichier d'un autre auteur.

## Frontiere de confinement soumise a revue du lead

1. Python ne lance qu'un interpreteur Windows PowerShell connu, avec `-File`
   pointant exactement vers le programme factice versionne. `shell=False`,
   timeout de 15 s. Le helper original n'est **jamais** une cible de processus,
   d'import, de dot-source ou d'Invoke-Expression.
2. Python lit le helper comme texte, extrait uniquement la region entre deux
   marqueurs uniques, et la transmet comme donnee JSON. Le reste du helper
   (bootstrap, fichiers, vcvars, Conan, CMake, CTest) n'est pas transmis au moteur
   d'execution du harnais. Les arguments de workload sont des chaines factices,
   pas des chemins executables ouverts.
3. Le harnais parse la region **avant** ScriptBlock.Create : seules quatre
   commandes nommees sont autorisees, toutes des fonctions factices definies
   localement. Pas d'appel dynamique/dot-source, commande native, type/.NET,
   acces membre general, fonction/classe, redirection, provider env, variable
   scopee ni scriptblock imbrique ; la seule methode admise est le test actuel
   PSBoundParameters.ContainsKey('VerificationPhase'). Bornes source/AST.
4. Les fonctions factices enregistrent des donnees et retournent un code enfant
   synthetique ou remplacent une **hashtable**, jamais le vrai environnement de
   processus. Elles ne contiennent aucun fallback vers un outil reel. L'enfant
   de code 17/23 est un evenement, pas un executable. Les commandes Conan/CMake
   attendues sont uniquement des donnees dans le journal.
5. Le seul suffixe ajoute a la region est un throw sentinelle de fallthrough,
   pas le corps historique du helper. Les seuls fichiers ecrits par PowerShell
   sont son resultat de fixture ; par Python : entrees/resultats/logs propres a
   un dossier unique sous out/verification-synthetic/windows-dispatch-fixtures.
6. Le test negatif du harnais transmet commandes/methodes interdites comme
   donnees ; code attendu 96 avant evaluation, zero evenement, environnement
   factice identique. Ce controle n'etablit PAS une sandbox PowerShell generale.
   Le lead relit le programme versionne et la region extraite avant lancement.
   Un autotest positif distinct utilise un mini-script synthetique, pas le
   helper, pour verifier journal factice, finally et codes de processus 0/17.
   S'il echoue, la panne du harnais doit etre corrigee avant de retenir le RED.

Limite assumee : cette restriction AST est une frontiere etroite pour code relu,
pas une garantie contre code arbitraire hostile. Si elle refuse une future
implementation, c'est un probleme de harnais/interface a arbitrer, pas un RED
fonctionnel ni une raison de retirer les gardes en silence.

## Interface privee proposee pour le futur dispatch, pas schema core

Variables injectees : VerificationPhase, AllowDependencyResolution,
Configuration, VerificationTargets, VerificationExecutable, VerificationArguments,
VerificationBuildDirectory, VisualStudioPath, Python, Jobs, root.
Ce sont entrees de fixture/helper, pas une nouvelle Request ou un receipt.

Frontieres d'effets proposees :

- Get-VerificationProcessEnvironment -> copie d'une map pour snapshot ;
- Get-VerificationMsvcEnvironment -> environnement enfant simule ;
- Set-VerificationProcessEnvironment(Values) -> remplacement exact de la map ;
- Invoke-VerificationChild(Program, Arguments) -> code entier, journal d'argv/env.

Le dispatch doit posseder le try/finally de restauration et le court-circuit
sur code enfant non nul ; les doubles ne le font pas a sa place. Il doit etre
terminal (return/exit/refus), jamais tomber sur la campagne historique.
Le raccordement des frontieres a MSVC reel sera une tranche ulterieure revue,
pas une propriete demontree par cette fixture. Aucun corps operationnel nouveau
ni wrapper reel n'est encore livre.

## Commandes proposees au lead, pas executees par l'auteur

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline`

RED cible, seulement apres revue/autorisation du harnais :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_dispatch.py' -v
```

Sous-processus unique possible par cas :
`<SystemRoot>/System32/WindowsPowerShell/v1.0/powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File <worktree>/scripts/tests/verification_windows/dispatch_fixture.ps1 -InputFile <fixture>/input.json -OutputFile <fixture>/result.json`.
Aucun vrai build-windows.ps1, outil de build ou workload n'est lance.

Non-regression statique separable :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_helper_contract.py' -v
```

## Oracles et attendu par lecture (NON_EXECUTE)

- Sans consentement prepare : code non nul, aucun evenement ni mutation.
- Prepare autorise : code 0, enfants Conan install -> CMake configuration
  uniquement, host/build Release et repertoire distinct explicites. Le preset
  Windows existant est conserve, avec -B et --toolchain explicites pour ne pas
  reutiliser le cache/toolchain de l'arbre historique. Verification ON et
  BUILD_TESTING OFF pour cette selection CPU (pas de suite ni discovery GL).
- Build : un CMake --build du repertoire et de la cible CPU declares uniquement.
- Run : executable et argv factices exacts, espaces/point-virgule jamais interpretes.
- Echec premier enfant 17/23 : meme code de processus, pas d'enfant suivant.
- Succes/echec : enfant voit env MSVC factice ; snapshot initial restaure exactement,
  cles absentes et valeurs vides comprises. Aucun MSVC reel n'est sonde.
- Tous les modes : absence d'atteinte de la sentinelle legacy.
- Harnais : les constructions interdites sont refusees avant evaluation.

Attendu sur cette base : 10 methodes, 6 methodes en echec fonctionnel
(11 assertions failures avec leurs sous-cas), 4 succes, aucun error/skip sous Windows equipe
de PowerShell. Cause attendue : le refus actuel "Verification phases are not
implemented yet" renvoie code 1 au lieu de la preparation/build/run attendus,
ou du code enfant 17/23. Le premier echec dans l'ordre unittest devrait etre
`test_build_only_selected_target_without_bootstrap_or_configure`.

Un interpreteur absent, erreur AST du harnais, sortie de fixture manquante,
timeout ou zero test n'est PAS ce RED. Les resultat/logs bruts sont conserves
par cas ; le lead doit distinguer panne du harnais et assertion de dispatch.

Les comptages sont des attendus de lecture, pas une execution attribuee.
Les 5 tests statiques precedents devraient rester verts ; ne pas modifier leurs
assertions pour ce passage.

## Matrice et dependances

- Fixture Python/PowerShell synthetique : NON_EXECUTE par l'auteur.
- Helper reel/MSVC, installation, CMake/Conan/CTest : NON_EXECUTE.
- Workload CPU, GL logiciel/materiel, images/performance/cloud : NON_EXECUTE.
- Core modele/baseProtocol non integre : adaptateur differe, aucun faux RED
  d'import. Les tests ci-dessus n'en dependent pas.
- Aucun Docker/cloud ni ressource reservee. Git/diff/lectures et analyses de
  syntaxe statiques seulement : ast.parse du fichier Python (sans import du
  module) et Parser.ParseFile du harnais PowerShell (sans evaluation). Aucune
  invocation du programme factice ni de sa region de dispatch par l'auteur.

Demande au lead : revue du confinement puis execution du RED cible si accepte.
Ne pas autoriser GREEN operationnel sur une simple panne de harnais.
