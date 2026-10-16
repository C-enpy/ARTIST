# Préflight natif OpenCode V2 — découverte initiale

Date : 2026-10-16. Exécuté par le coordinateur après T01/T02 intégrés.
Candidat : HEAD `3c17f0ce321b9d779dd1c6ba66954b7b160fedc6`, diff utilisateur
initial conservé. Aucun modèle, canari d'écriture ou contexte graphique lancé.

## Hypothèses et expériences discriminantes

1. `opencode` absent du PATH ne suffit pas à conclure à l'absence du runtime.
   `Get-Process -Name '*opencode*'` a observé les exécutables actifs :
   - `C:/Users/djden/AppData/Local/Programs/@opencodedesktop/OpenCode.exe`
   - `C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.0.30-x64/opencode-cli.exe`
2. Le CLI actif a exécuté `api get /api/agent`, exit 0, mais seulement sept
   agents natifs : une requête non scoped n'est pas une preuve du projet ARTIST.
3. `api --help` expose `--param key=value`. Passer `location={JSON}` a été
   rejeté pour agent.list et skill.list, exit 1 : `Expected object | undefined
   at ["location"]`, kind Query. Ce sont des erreurs d'encodage, pas un RED
   comportemental ni une panne de découverte.
4. `api get /openapi.json`, exit 0, a fourni le schéma du serveur actif : OpenAPI
   3.1.0 ; paramètre location des deux endpoints = `style: deepObject`,
   `explode: true`, objet `{directory?: string|null}`.
5. La sérialisation deepObject ci-dessous a réussi, exit 0, avec
   `response.location.directory = C:\Users\djden\source\repos\ARTIST`.

## Commandes reproductibles prouvées

PowerShell, depuis la racine ARTIST :

```powershell
$cli = 'C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.0.30-x64/opencode-cli.exe'
& $cli api agent.list --param 'location[directory]=C:/Users/djden/source/repos/ARTIST'
& $cli api skill.list --param 'location[directory]=C:/Users/djden/source/repos/ARTIST'
& $cli api get /openapi.json
```

Les appels observés ont utilisé subprocess argv équivalents via Context Mode,
avec timeout, parsing JSON et vérification explicite de la location retournée.
Le CLI assure la découverte/authentification native ; aucun port ni secret deviné,
aucune requête locale non authentifiée de contournement, aucune installation.

## Résultats bornés

- `agent.list` : **18 agents résolus**, dont les onze profils locaux et sept
  natifs. `orchestrator` est primary ; les dix spécialistes/PM sont subagent.
- `skill.list` : **92 skills résolus**, globaux et locaux confondus, dont les six
  ARTIST attendus : artist-asset-cooking, artist-build-validation,
  artist-cpp-contracts, artist-gpu-debug, artist-performance, artist-static-boundary.
- Le catalogue agent expose les permissions résolues. Les règles initiales
  observées du PM/verifier contiennent allow global, deny subagent et deny edit,
  **pas une preuve de refus des écritures shell/MCP**. Ne pas assimiler ces
  métadonnées à un canari d'enforcement exécuté.

## Verdicts et conséquences

- **PASS borné** : accès authentifié au catalogue natif scoped ARTIST et
  découverte des profils/skills **préexistants**. La précondition D2 du plan QA
  n'est plus bloquée par la seule absence PATH.
- **INCONCLUSIVE** : découverte des futurs profils, chargement effectif des skills
  dans une mission, sélection sur les douze scénarios, enforcement négatif des
  permissions QA. Aucune de ces preuves ne découle d'un GET de métadonnées.
- **Non exercé** : exécution moteur, compilation shader, rendu, capture et
  performance GPU. Aucun PASS GPU ou résultat de performance.

Le bail de lecture du catalogue pris par le lead a été libéré et ces commandes
transmises à T03 pour figer les préconditions de T04. Ne pas modifier le PATH
global pour rendre ce préflight reproductible : utiliser le binaire découvert,
ou une autre installation explicitement découverte lors d'une reprise.
