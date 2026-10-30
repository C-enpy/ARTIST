# Helper Windows — SOURCE_GREEN_READY, admission runner encore fermée

## Candidat et autorité

Entrée propre `29036c92fc96bcb2936eb2aaf0bc381d4779e2da`. Autorisation lead après
rapport130 `dad23ce` : GREEN minimal du seul helper et présente note. Preuves lead
réutilisées : 7 baselines puis RED prepare NOTIMPLEMENTED ; nouvelles baselines
P0 6/6. Les vingt tests source n'ont pas été exécutés. Scores78/68 inchangés.
Le collecteur core reste sur main (`9aedc0a`), sans copie/merge dans ce worktree.

**AUTEUR NON RUNNER** : aucun PS, unittest, import de test, faux enfant, helper,
outil, configure/build/CTest, moteur, GPU, installation ou cloud exécuté. Aucune
qualification native ni identité binaire ni performance démontrée. Cette remise
est une source proposée, pas un GREEN observé. Les JSON d'admission restent
strictement inchangés ; leurs anciens hashes doivent refuser cette source.

## Implémentation bornée

- Région BINDING : définitions seulement, immédiatement après END_NATIVE. Les
  trois affectations root/tooling/venv réelles sont partagées avant usage ; elles
  ne sont ni remontées par un extracteur ni injectées par une fixture.
- Entrée : flags natifs/defaults puis refus run explicite, logs/bundle/payload
  quatre champs, arbre/config/cibles, préconditions build avant contexte/observation/
  horloge. Aucun fallback vers legacy. Le chemin sans phase garde son tail exact,
  sauf le déplacement des trois affectations partagées.
- Contexte script privé, non réentrant, créé après admission et retiré finally.
  Snapshot dictionnaire en mémoire seulement ; Read/Write-EnvironmentLeaf copient
  ou mutent cette map, null retire et vide reste présent. Aucun SetEnvironmentVariable
  ni mutation de l'environnement OS/Python ou du cwd de l'appelant.
- Tooling prepare opt-in : Python initial résolu explicitement, venv/ensurepip
  si absents, requirements existants pip, Conan2.12.2/Ninja1.13.0 et CMake préinstallé
  >=3.25. Build : outils existants du témoin, versions déclarées relues, zéro
  bootstrap/configure/réparation. Pas de sonde --version cachée dans un rôle nouveau.
- MSVC : VS explicite invalide sans recherche de remplacement ; sinon vswhere
  UTF8 puis 14.44/Hostx64/x64. Build reprend le VS du témoin sans vswhere. Le
  vcvars fixe exporte uniquement l'allowlist via le script littéral ; VSINSTALLDIR
  observé normalisé nourrit les deux options Conan host/build. x64 host/target,
  versions MSVC/SDK et chemin cl sont contrôlés ; ce n'est pas une sonde STL.
- Commandes : un objet Process/factory par lancement, cwd/env/outils explicites,
  budget global restant, primitive pinée appelée une fois, Dispose une fois,
  premier code/erreur disponible conservé et nettoyage ajouté en secondaire.
  command_line vient du StartInfo.Arguments réellement utilisé, pas d'argv reconstruit.
- Rôles fermés, dossiers non adoptés, stdout/stderr de la primitive et command.json
  exclusif ; transports possédés connus seulement supprimés après consommation.
  Partiels laissés disponibles en échec. Le core déjà intégré demeure propriétaire
  de leur collecte/hashes avant receipt ; aucune API publique ni capture backend.
- Témoin plat douze chaînes après succès MSVC+Conan+configure seulement ; build
  réadmet cache/toolchain/build.ninja et choix cohérents sans réécriture. Le témoin
  décrit chemins/versions, PAS l'identité binaire des outils.

Le gap Dispose/writers hérités reste ouvert. Le timeout avant lancement ne promet
pas une borne nouvelle de Dispose/drain ni la collecte/terminaison des descendants.
Les primitives NATIVE et les wrappers sont conservés octet pour octet après
normalisation historique UTF8 sans BOM/LF. L'ancien commentaire NATIVE de fermeture
reste historique dans la région intangible, il ne décrit plus la nouvelle entrée.

## Coutures conservées et points de revue

Fonctions ratifiées présentes : Enter/Get/Exit-VerificationContext ;
Read/Write-VerificationEnvironmentLeaf ; Initialize/Get-VerificationTooling ;
Assert-VerificationPreparedTree ; Read-VerificationMsvcEnvironmentLeaf ;
Set-VerificationCommandRole ; Invoke-VerificationProcessLeaf ; les ports
New-VerificationProcessObjectLeaf, Read-VerificationClockLeaf,
Read-VerificationHostObservationLeaf et Read-VerificationToolMetadataLeaf.
Get-VerificationExporterSource reste un retour d'un seul literal fixe.

Assert-VerificationPreparedTree garde ses deux paramètres ; ses seules données
supplémentaires sont VerificationBundleRoot et VerificationTargets, paramètres/
valeurs réellement admis dans l'entrée SOURCE avant l'appel. Pas de variable
supplémentaire secrètement initialisée par une fixture. Les helpers internes de
chemins/JSON ne lisent pas le contexte, l'environnement ou l'horloge.

L'allowlist producteur ajoute explicitement OS, PROCESSOR_ARCHITECTURE et
PROCESSOR_ARCHITEW6432 aux noms du plan de tests, pour le bootstrap Windows/vcvars.
Ils sont copiés seulement s'ils existent, jamais inventés. Extension nominative
à relire ; aucune permission wildcard. Les données synthétiques actuelles ne les
portent pas. Proposition séparée : aligner TOOL_ENVIRONMENT_NAMES sur ces trois
noms avant un futur test les injectant ; conserver les canaris et l'oracle de
sous-ensemble. Aucun test n'a été modifié ici.

## Adaptations de harness indispensables — proposées, PAS appliquées

### 1. Nouveau programme natif : donnée VS incomplète

`test_native_binding.py::NativeBindingCase.run_case` crée le faux cl mais pas
`VC/Auxiliary/Build/vcvarsall.bat`. Le producteur exige ce fichier avant cmd :
ne pas retirer ce contrôle pour verdir. Proposition de donnée seule avant run :

```python
vcvars = case / 'Visual Studio/VC/Auxiliary/Build/vcvarsall.bat'
vcvars.parent.mkdir(parents=True, exist_ok=True)
vcvars.write_bytes(b'TEST DATA ONLY - NEVER EXECUTE')
```

Ce faux .bat ne sera jamais lancé ; la factory/capture mémoire reste la frontière.
Ni nouveau booléen « VS valide », ni validateur réécrit dans le fake. Assertions
de chemins, choix Conan, metadata, codes et absence de fallback conservées.

### 2. Dix-huit hauts niveaux : sélection SOURCE et doubles supérieurs

L'extracteur historique prend encore toute la portion END_NATIVE→END_DISPATCH :
il inclut désormais les définitions BINDING. Avant admission de son nouveau pin :

- vérifier région BINDING unique, composée de définitions avant tout exécutable,
  puis l'exclure de l'entrée extraite, sans changer l'ordre des exécutables ;
- ajouter aux huit définitions historiques seulement les quatre corps SOURCE purs
  Get-VerificationAbsolutePath, Assert-VerificationRegularPath,
  Get-VerificationEnvironmentNames et Select-VerificationEnvironment ;
- adapter la population contrôlée à douze définitions, pas charger les feuilles
  natives réelles sous les doubles hauts niveaux ;
- créer le dossier logs/windows-helper-PHASE dans les données de chaque cas, comme
  le fait le sink réel, sans rendre favorable un chemin mauvais/étranger ;
- ajouter le seul VSINSTALLDIR au faux MSVC depuis le VS explicitement demandé,
  ou une installation factice explicitement déclarée pour le cas par défaut ;
- déclarer doubles supérieurs data-only Enter/Get/Exit-VerificationContext,
  Set-VerificationCommandRole et Write-VerificationToolchainWitness. Ils ne valident
  ni contexte ni témoin : ces preuves sont les vingt tests SOURCE et le core.
  Aucun de ces doubles ne doit être présenté comme une preuve native/artifact.

Allowlist entrée nominative supplémentaire : Get-VerificationAbsolutePath,
Assert-VerificationRegularPath, Resolve-VerificationPhaseFlags,
Read-VerificationPayloadFile, Resolve-VerificationInputs,
Select-VerificationEnvironment, Enter/Exit-VerificationContext,
Set-VerificationCommandRole, Write-VerificationToolchainWitness ; les appels déjà
présents restent. Variables ajoutées : verificationRequested, verificationFlags,
admittedPhase, bundlePath, phaseLog, verificationValues, preparedWitness, admission,
tools. Membres supplémentaires uniquement PSBoundParameters.ContainsKey($name)
et IO.Path.Combine pour les chemins de phase ; pas de méthode arbitraire.

Le filtre réel Select-VerificationEnvironment est exécuté avant Set-Msvc dans
l'entrée : l'ancien oracle « enfant sans secret/KEEP » n'est pas contourné par
un faux filtre dans Invoke-FixtureChild. Les assertions existantes des 7+11,
notamment root/source, jobs/config/host-build et restauration, restent intactes.
Ces adaptations sont une proposition au lead, pas une édition admise du harness.

### 3. Ancien oracle de fermeture — patch proposé au lead uniquement

`test_wrappers.py::WindowsRealHelperBarrierTests` : conserver cette classe et ses
autres assertions, remplacer seulement l'oracle global par le mapping ratifié :

```python
def test_real_helper_admits_only_validated_prepare_build_before_effects(self):
    source = HELPER.read_text(encoding='utf-8-sig')
    prefix = source.split('# END ARTIST_VERIFICATION_BINDING', 1)[1].split(
        '# BEGIN ARTIST_VERIFICATION_DISPATCH', 1)[0]
    root = prefix.index('$root = Split-Path $PSScriptRoot -Parent')
    flags = prefix.index('$admittedPhase = Resolve-VerificationPhaseFlags')
    run_refusal = prefix.index("if ($admittedPhase -ceq 'run')")
    payload = prefix.index('Read-VerificationPayloadFile')
    inputs = prefix.index('Resolve-VerificationInputs')
    self.assertLess(root, flags)
    self.assertLess(flags, run_refusal)
    self.assertLess(run_refusal, payload)
    self.assertLess(payload, inputs)
    self.assertIn('Assert-VerificationPreparedTree', prefix)
    for effect in ('Initialize-VerificationTooling', 'Get-VerificationTooling',
                   'Get-VerificationMsvcEnvironment', 'Enter-VerificationContext',
                   'Invoke-VerificationChild', 'Invoke-VerificationProcessLeaf'):
        self.assertNotIn(effect, prefix)
```

Ce test de structure ne remplace PAS les refus exécutés 18/20 et sentinelles
anti-fallthrough. Ne pas exécuter le helper complet pour cet oracle.
`test_dispatch.py`/dispatch_fixture historiques prennent le seul prototype dispatch
avec variables injectées et autorisent encore un run logique : cette preuve n'est
pas le nouveau raccordement. Toute reprise de ces tests exige un mapping séparé
conservant l'oracle de quoting au niveau wrapper/direct-adapter ; ne pas les lancer
en découverte globale ni réintroduire helper-run pour les satisfaire. Test06
jonction reste NON_EXECUTE ; seules sélections fondations relues par le lead.

## Pins proposés — jamais persistés par l'auteur

Calculs de données UTF8 sans BOM / LF, aucun appel de fonction du helper :

```json
{
  "source_sha256": "5f8a8e2b0860179b89f186e6a4225cce50aba0bc442615a9ee5d52d3e76832b6",
  "native_sha256": "8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de",
  "fragments": {
    "parameters": "8d1b63c0e5807d215c95a51f17ed2dfdb7895029979bb719a082c31b1abac539",
    "wrappers": "470e8dfec02f65700766b312b54e8a4dad6e5d56ddad2160afbc7ae85d238371",
    "data": "8b62b2ac8816a0a36a4a5627749dbfc1c6a9dd999b1c94cfbbeb8073b23afc6b",
    "root": "be460e7a3996b10e90c2359b408f68499dd309a98c4b3b60284e1080cde46c4a",
    "entry": "319e78647763f4b4b4cf4e3a9f81f6b4d02b3350e05829c6b4eea2ea4a2ab799",
    "binding": "2185760d0aa09835cb1937077cdd7a7d9a0b25af22c230863993bc3b9dcdcd84"
  },
  "exporter_sha256": "002648e6c0615049cece76cbef4cc52a47e26b6f6932c68098ad2f5b1a970046"
}
```

Pour le **pin historique adapté**, proposition de definitions SHA256 :
`b3a52340eeb9b263fdddcb65412ecd4557c36de0917c7e6081133a1287679278`.
Construction précise : ancien WRAPPERS + LF + native[Read-Payload..avant-NewStartInfo]
+ LF + les quatre tranches SOURCE dans l'ordre AbsolutePath, RegularPath,
EnvironmentNames, SelectEnvironment (chaque tranche va jusqu'au début de la fonction
suivante, sans changer les whitespace). Entrée/root/parameters selon ci-dessus.
Ce hash reste conditionnel à cette construction exacte, pas un pin déjà accepté.

Le lead relit la source, confirme via AST le literal exporteur (hors newline de
fermeture), puis admet explicitement native_binding_admission.json et le pin
d'entrée historique adapté. Aucun de ces fichiers n'est modifié ici. Ne pas mettre
binding/exporter automatiquement à jour en setUp. Les deux régions intangibles et
le tail legacy (hors trois lignes root partagées) ont été comparés égaux à l'entrée.

## Vérification et suite lead

Réellement effectué : lecture/diff/hash des régions, contrôle du tail legacy et
AST Python sans import local. Syntaxe/runtime PowerShell NON_EXECUTE.
Après revue/admissions privées et seules adaptations de données ratifiées :
six baselines, vingt SourceTests, dix-huit hauts niveaux et fondations ciblées ;
revue indépendante fresh avant merge natif. Couture sur main contenant le
collecteur intégré et témoin réellement empreinté/réadmis, pas sur un faux receipt
déclaré favorable. Aucun résultat de ces étapes n'est acquis par cette remise.
