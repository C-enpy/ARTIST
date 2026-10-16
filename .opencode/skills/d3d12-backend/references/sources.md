# Sources et dispositions D3D12

Consultation documentaire : 2026-10-16. Contenu du skill en **rédaction originale** ;
liens seulement, aucun sample/code/instruction tiers copié ou adapté. Une page
accessible ne prouve ni fonctionnalité locale ni licence de tous ses composants.
Les versions ci-dessous sont celles observées, pas des pins d'installation.

## Contrats API Microsoft

Références officielles Microsoft Learn, pages HTTP 200 lors de la consultation :

- [Root signatures](https://learn.microsoft.com/en-us/windows/win32/direct3d12/root-signatures) : layout et liaison aux shaders.
- [Descriptor heaps](https://learn.microsoft.com/en-us/windows/win32/direct3d12/descriptor-heaps) : heaps shader-visible vs staging/RTV/DSV.
- [Pipeline state](https://learn.microsoft.com/en-us/windows/win32/direct3d12/managing-graphics-pipeline-state-in-direct3d-12) : état PSO et état restant sur command list.
- [Allocator Reset](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12commandallocator-reset) et [command-list Reset](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-reset) : conditions de réutilisation différentes.
- [Resource barriers legacy](https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12) : transitions/UAV/aliasing, promotion et decay sous conditions.
- [Queue Wait](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-wait) : attente GPU distincte de l'attente CPU par événement.
- [OPTIONS12](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_feature_data_d3d12_options12) et [GraphicsCommandList7::Barrier](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist7-barrier) : support optionnel et SDK/runtime/interface requis ; vérifier les versions exactes avant usage, pas le seul numéro DXC.
- [EnableDebugLayer](https://learn.microsoft.com/en-us/windows/win32/api/d3d12sdklayers/nf-d3d12sdklayers-id3d12debug-enabledebuglayer) et [GPU-based validation](https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-d3d12-debug-layer-gpu-based-validation) : configuration avant device, diagnostic et coût non représentatif.
- [Residency](https://learn.microsoft.com/en-us/windows/win32/direct3d12/residency) : objets pageables et budget distincts de la vie COM.

Disposition : **référencer**. Dépendances : Windows/runtime D3D12, SDK/interfaces
et features du device pour exécution. Licence des fichiers précis non auditée
ici : aucune réutilisation textuelle/code autorisée par ces liens.

## Corpus audité T02, sans import

Audit parent : `.opencode/evidence/mission-gpu-qa/t02-external-sources.md` et
`t02-source-inventory.md`. Leurs dates/arbre SHA identifient une observation ;
**arbre SHA ≠ commit**. Avant toute adaptation future, résoudre un commit exact,
chemin et notice de chaque fichier et dépendance ; licence inconnue = lien seul.

- [DirectX-Graphics-Samples](https://github.com/microsoft/DirectX-Graphics-Samples) :
  exemples, T02 2026-10-13, MIT au niveau dépôt ; Windows SDK/device/features requis.
  Référencer root signatures/PSO/queues, pas importer le framework des samples.
- [DirectXShaderCompiler](https://github.com/microsoft/DirectXShaderCompiler) :
  compilateur/bibliothèque, T02 2026-10-16, NOASSERTION/multi-licences de composants.
  DXC 1.8 local observé par T02 ; aucune copie, aucun device/frame déduit.
- [DirectX-Headers](https://github.com/microsoft/DirectX-Headers) : headers,
  T02 2026-10-15, MIT dépôt ; ABI/SDK ≠ runtime. Référencer, ne pas installer.
- [D3D12MemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator) :
  bibliothèque, T02 2026-10-15, MIT dépôt ; référence allocation/budget/résidence,
  pas adoption automatique ni remplacement du contrat d'ownership ARTIST.
- [DirectXTex](https://github.com/microsoft/DirectXTex) : texture offline,
  T02 2026-10-16, MIT dépôt ; référence cook/format vers asset-engineer.
- [DirectXMesh](https://github.com/microsoft/DirectXMesh) : géométrie offline,
  T02 2026-10-11, MIT dépôt ; référence cook/mesh vers asset-engineer.

Les skills DirectXTex/DirectXMesh audités ne couvrent pas le cœur D3D12 de ce lot.
Pas de copie de superviseur tiers, pas de loader/plugin supplémentaire. Un besoin
d'installation ou de réutilisation licenciée sort du lot et retourne au lead.

## Format du profil et limites d'outils

Format relu dans les pages OpenCode **V2** le 2026-10-16 :
[agents](https://opencode.ai/v2/docs/agents),
[skills](https://opencode.ai/v2/docs/skills),
[permissions](https://opencode.ai/v2/docs/permissions).
`name` égal ID est une convention ARTIST ; le chemin détermine l'ID natif.
Les permissions déclarées ne sont pas un sandbox ni une preuve d'enforcement.
Le présent lot n'interroge pas le catalogue et ne lance pas de canari : bail lead.

PIX/RenderDoc/debug layer/GBV : vérifier disponibilité, version et autorisation
avant emploi. Aucun MCP PIX, téléchargement, capture, SDK ou benchmark installé
par ce skill. Source, compilation, validation API, image et mesure restent des
niveaux de preuve distincts.
