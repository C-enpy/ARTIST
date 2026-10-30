# HARNESS_ADAPTATION_READY — source 27dd1ed stable, aucun run auteur

## Autorité et périmètre

Entrée propre `27dd1ed16b69104501237e9de4d9ffea618689a4`. Relecture source et
ratification du mapping hauts niveaux reçues du lead. Modifications bornées aux
harness : nouveau faux vcvars, allowlist/test de conservation de trois noms OS,
extraction historique, données logs/VS et doubles hauts niveaux explicites.
Le helper, WRAPPERS/NATIVE, les deux JSON d'admission, native_source.sha256,
test_wrappers, test_dispatch, core/adaptateurs/fakes Python/CMake/CI restent intacts.
Le JSON natif est propriété du lead ; aucune admission automatique ni copie main.

**NON_EXECUTE auteur** : aucun PowerShell, unittest, import de cible, faux enfant,
helper, outil, moteur, GPU, installation ou cloud exécuté. La syntaxe/runtime PS
restent à vérifier par le runner. Aucun GREEN ni qualification native revendiqué.

## Changements natifs de données seulement

`test_native_binding.py::NativeBindingCase.run_case` crée maintenant le fichier
factice VC/Auxiliary/Build/vcvarsall.bat demandé par le vrai préflight du producteur.
Contenu explicite TEST DATA ONLY - NEVER EXECUTE. Ni interprétation ni lancement
du .bat : la factory/capture mémoire et tous les oracles restent en place.

TOOL_ENVIRONMENT_NAMES ajoute exactement OS, PROCESSOR_ARCHITECTURE et
PROCESSOR_ARCHITEW6432. Les trois valeurs synthétiques sont injectées dans parent
puis héritées par le faux snapshot MSVC ; assertions supplémentaires de conservation
exacte dans l'enfant et l'exporter. Sous-ensemble fermé et canaris inchangés.
26 méthodes natives conservées, aucune assertion précédente retirée.

## Extraction haute niveau et contrôle d'admission

`helper_fragment_support.extract` accepte le helper jusqu'à262144 caractères,
mais garde paramètres≤4096, chaque fragment exécutable≤32768 et BINDING≤65536.
La région BINDING doit être unique, avant le dispatch et avant tout exécutable
d'entrée. Elle est exclue de l'entrée **sans hoisting de root ou d'appel**.

La fixture reçoit binding_source comme donnée : hash séparé puis AST top-level
definitions-only. Ce bloc complet n'est JAMAIS évalué. L'ancien ensemble de quatre
fragments paramètres/définitions/entrée/root est conservé, donc l'oracle de
population historique reste inchangé. Le pin historique a besoin d'un champ privé
supplémentaire binding_sha256, proposé ci-dessous, jamais écrit par l'auteur.

Seuls quatre corps SOURCE purs s'ajoutent aux huit définitions historiques :
Get-VerificationAbsolutePath, Assert-VerificationRegularPath,
Get-VerificationEnvironmentNames et Select-VerificationEnvironment. Les bornes
de chaque tranche sont uniques et ordonnées ; population AST exacte de douze.
Ni StartInfo natif ni primitive capture ni vraie feuille effet ne sont chargés.
Le filtre exécuté est le VRAI Select-VerificationEnvironment, pas une copie fake.

## Doubles supérieurs et données, pas preuves natives

La fixture déclare explicitement Enter/Get/Exit-VerificationContext,
Set-VerificationCommandRole et Write-VerificationToolchainWitness comme doubles
DATA-ONLY : booléen d'activité, rôle et demande de témoin seulement. Ils ne valident
pas une entrée, un contexte, une réentrance ou un témoin ; aucun fichier/receipt
favorable n'est fabriqué. La preuve de ces propriétés reste les vingt SourceTests
et le collecteur core. Aucun root/tooling/venv n'est injecté dans l'entrée.

Chaque cas crée le dossier standard logs/windows-helper-PHASE comme le sink réel,
avant les overrides négatifs (qui ne sont pas réparés). Le faux MSVC reçoit le seul
VSINSTALLDIR supplémentaire : VS explicitement demandé ou sélection factice par
défaut déclarée dans input.json ; une valeur explicitement fournie n'est pas écrasée.
Les autres métadonnées invalides, le canari, chemins, codes et raw logs sont conservés.
18 méthodes haut niveau et leurs assertions métier conservées.

L'AST d'entrée n'ajoute que les appels nommés ratifiés. Membres supplémentaires :
PSBoundParameters.ContainsKey($name) et IO.Path.Combine ; TypeExpression uniquement
IO.Path. Variables ajoutées : verificationRequested/verificationFlags/admittedPhase,
bundlePath/phaseLog/verificationValues/preparedWitness/admission. Les autres règles
de refus restent : pas d'OS/global env, &, dot-source, fonctions d'entrée, redirections,
code dynamique ou commande non listée. Les sentinelles et le cleanup d'état isolé
restent inchangés. Ce contrôle d'admission de source relue n'est pas un sandbox général.

## Pin historique proposé — non persisté

Les champs calculés sont proposés pour relecture/admission lead, pas acceptés par
un setUp automatique. UTF8 sans BOM, LF ; corps purs concaténés dans l'ordre du
mapping, en conservant leurs espaces jusqu'au début de la fonction suivante.

```json
{
  "source_head": "27dd1ed16b69104501237e9de4d9ffea618689a4",
  "normalization": "UTF-8 without BOM; universal newlines (LF)",
  "source_sha256": "5f8a8e2b0860179b89f186e6a4225cce50aba0bc442615a9ee5d52d3e76832b6",
  "binding_sha256": "2185760d0aa09835cb1937077cdd7a7d9a0b25af22c230863993bc3b9dcdcd84",
  "fragments": {
    "parameters": "8d1b63c0e5807d215c95a51f17ed2dfdb7895029979bb719a082c31b1abac539",
    "definitions": "b3a52340eeb9b263fdddcb65412ecd4557c36de0917c7e6081133a1287679278",
    "entry": "319e78647763f4b4b4cf4e3a9f81f6b4d02b3350e05829c6b4eea2ea4a2ab799",
    "root_binding": "be460e7a3996b10e90c2359b408f68499dd309a98c4b3b60284e1080cde46c4a"
  }
}
```

Le pin historique encore ancien est censé refuser ce candidat avant effets.
Ne pas confondre ce HARNESS attendu avant admission avec un RED production.
L'ancien oracle de fermeture dans test_wrappers reste proposition au lead dans
SOURCE_GREEN_READY.md : aucune modification ici. Prototype test_dispatch hors scope,
pas de découverte Windows globale ; test06 jonction NON_EXECUTE.

## Remise runner

Contrôles auteur : AST Python sans import local, compte des méthodes/assertions,
comparaison source/pins et diff. Aucun programme factice ou source PS exécuté.
Comparaison AST au parent27dd1ed : mêmes 26/18 méthodes, zéro appel d'assertion
antérieur retiré ou changé ; seules assertions de conservation des trois champs
OS ajoutées. Les hashes proposés de source/binding/définitions/entrée ont été
recoupés par analyse de données, sans appeler l'extracteur candidat.
Après relecture et admissions explicitement persistées par le lead, depuis
scripts/tests/verification_windows/ :

```powershell
C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingHarnessTests
# Seulement après six baselines valides et revue du source/pin natif :
C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingSourceTests
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalHarnessTests
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalAdmissionTests
```

Couture core uniquement après intégration effective des producteurs. Baselines
antérieures 6/6 et 7/7/RED sont historiques ; aucune preuve runtime du nouveau
harness n'est acquise par cette remise.
