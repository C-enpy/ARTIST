# Sources Metal et dispositions

Consultation : 2026-10-16. **rédaction originale** française ; liens de référence,
aucun extrait de code/skill tiers importé, aucune installation. Les commandes
illustratives ne téléchargent rien. Les pages peuvent évoluer : avant usage,
recouper SDK, OS, availability et headers de la cible, pas seulement cette date.

## Sources officielles consultées

- [Metal](https://developer.apple.com/metal/) : API/familles et outils Apple.
  Page web accessible ; ni SDK ni device installé par cette lecture.
- [metal-cpp](https://developer.apple.com/metal/cpp/) : interface C++ et règles
  d'allocation Cocoa, absence d'ARC C++ ; wrapper, pas un port Metal Windows.
  Pas de téléchargement d'archive ni reprise du code d'exemple.
- [Core API Metal 4](https://developer.apple.com/documentation/metal/understanding-the-metal-4-core-api) :
  différences command buffer/allocator, rétention, argument tables et queues.
- [MTL4CommandQueue](https://developer.apple.com/documentation/metal/mtl4commandqueue) et
  [MTL4CommandBuffer](https://developer.apple.com/documentation/metal/mtl4commandbuffer) :
  availability OS 26.0 observée, pas garantie de compatibilité de chaque device.
- [Resource fundamentals](https://developer.apple.com/documentation/metal/resource-fundamentals) :
  storage/hazard modes et distinction du chemin Metal 4.
- [Resource synchronization](https://developer.apple.com/documentation/metal/resource-synchronization) :
  conflits, attachments, hazardTrackingMode sans effet sur MTL4CommandQueue,
  synchronisation intra/inter-pass/queues/CPU et compute concurrent.
- [Retained references](https://developer.apple.com/documentation/metal/mtlcommandbuffer/retainedreferences) :
  différence command buffer classique retained/unretained et durée des références.
- [Residency sets](https://developer.apple.com/documentation/metal/simplifying-gpu-resource-management-with-residency-sets) :
  allocations/heaps, commit et association buffers/queues ; résidence ≠ lifetime.
- [Triangle Metal 4](https://developer.apple.com/documentation/metal/drawing-a-triangle-with-metal-4) :
  protocole drawable wait/commit/signal/present et slots de frame ; procédure
  consultée, sample non téléchargé, non compilé, non copié.
- [MTLCaptureManager](https://developer.apple.com/documentation/metal/mtlcapturemanager) :
  capture conditionnelle, scope et options Xcode ; pas un outil découvert localement.

Les pages `/documentation/metal/…` ont été lues via leur représentation JSON
officielle `/tutorials/data/documentation/metal/….json` (HTTP 200). Les types et
prose sont utilisés pour raisonner, pas comme source de code à incorporer.
Les feature tables/headers exacts et les outils installés de la prochaine cible
doivent être inspectés alors ; aucune compilation Apple n'a été exécutée ici.

## Format OpenCode V2

- [Agents](https://opencode.ai/v2/docs/agents),
  [skills](https://opencode.ai/v2/docs/skills),
  [permissions](https://opencode.ai/v2/docs/permissions) : pages V2 consultées.
  `mode: subagent`, permissions ordonnées action/resource/effect ; metadata
  name/description du skill et disclosure à la demande. Pas de champ Cursor,
  modèle fixé, dispatcher maison ni duplication de l'orchestrateur.
- `name` égal à l'ID est la convention ARTIST ; ne pas en faire une obligation
  V2 inventée. Deny subagent déclaré n'est pas un confinement OS/shell/MCP prouvé.
  Le lead observe chargement et règles effectives après intégration.

## Corpus T02 — référencer, pas adapter sans provenance complète

L'audit T02 mentionne [Apple game-porting-toolkit](https://github.com/apple/game-porting-toolkit)
et ses `game-porting-skills/skills/` : Metal 4 API/synchronization/resources,
shader pipelines, metal-cpp lifetimes, drawable, shader converter, MetalFX,
fenêtre et outils Apple. Disposition T08 : **référencer** seulement ; pas
d'import du superviseur ni des recettes. Le hash
`4f42344c90be22112f46becc3fb9687f423f2f88` est un **arbre SHA**, pas un commit
résolu. L'audit signale Apache-2.0 et des notices de samples distinctes ; cela
n'autorise aucune copie globale. Toute future adaptation exige commit exact,
chemin, licence et notice applicables à chaque fichier. Pour une licence inconnue,
conserver uniquement le lien. Les guides génériques d'auteur ne remplacent pas
la documentation native V2 ; leur code n'est pas repris.

## Limites de preuve

T02 n'a pas résolu les outils Apple sur le PATH Windows ; ce n'est ni absence
absolue ni impossibilité d'une cible distante. Ce lot ne sonde pas d'installation,
de catalogue natif, de GPU ou de capture partagée. MSL généré, compile Apple,
création de pipeline, exécution GPU, image et performance ont des oracles séparés.
Les procédures sont livrables sans SDK ; la validation finale Apple reste
INCONCLUSIVE sans appareil/outils et bail autorisés.
