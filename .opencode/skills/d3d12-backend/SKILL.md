---
name: d3d12-backend
description: Diagnostique le cœur API Direct3D 12 explicite (root signatures, descriptor heaps, PSO, allocators/lists, queues, states/barrières, fences, résidence, debug layer et GBV). Procédures Windows conditionnelles, distinctes du cook DDS/mesh et du contrat ressources inter-API ; aucune exécution GPU présumée.
---

# Expertise Direct3D 12

## Déclencheurs et exclusions

Charger pour un problème D3D12 nommé : binding/root signature/PSO incompatible,
allocator réutilisé en vol, transition ou dépendance de queues manquante,
descriptor périmé, résidence ou message debug layer. Lire ensuite seulement la
section utile du [diagnostic](references/diagnostic.md).

- **Scénario 3 — resource barrier D3D12** : principal `gpu-resource-engineer`,
  minimum `gpu-resource-lifecycle` + `d3d12-backend`. Le premier possède le
  contrat d'accès/ownership ; le second traduit l'API, sans voler le principal.
- **DDS seul ne sélectionne pas D3D12** : `asset-engineer` et
  `artist-asset-cooking` pour texture/mesh offline. DirectXTex/DirectXMesh ne
  traitent pas root signatures, queues, PSO et durée de vie en vol.
- API absente → clarification. CPU seul → `cpp-engineer`. Compilation HLSL/DXIL
  sans question de binding/device → `shader-engineer` ; graphe de dépendances
  → `render-graph-engineer`. Pas de migration OpenGL implicite.
- `DEFER_TO` signifie retour au lead avec invariant, fichiers, essais et inconnues,
  pas sous-délégation. Les IDs futurs sont un contrat de routage ; leur découverte
  native sera contrôlée après intégration, pas par ce texte.

## Pin ARTIST et binding times

Le backend livré est **OpenGL** (`ARTIST/include/graphic/opengl/`). Cette
expertise n'est **pas un backend D3D12 implémenté**. Pin recoupé au contrat T03 :
`PipelineContext` possède ses passes par `unique_ptr` et `getPass` retourne un
emprunt ; ne pas réintroduire l'ancien stockage partagé. Un emprunt CPU ne
garantit pas la complétion GPU.

ADR-0001/0002/0004/0006/0007 restent **Proposed** ; ADR-0003/0005 **Accepted / partial** ;
ADR-0008 **Accepted / verified**, seulement Windows Debug. Revalider le pin si le
code diverge ; ne pas réécrire une ADR pour correspondre au profil.

- **compile** : contrats/types et axes bornés ; pas un type par texture/asset.
- **cook** : DXIL, réflexion/layout et métadonnées immuables/versionnées ; clé
  source/options/entry/cible/compilateur/ABI. Changement → invalidation, pas cache
  réutilisé par nom de shader seul. Un blob PSO dépend aussi du device/driver.
- **runtime** : device/capacités, root signatures/PSO créés, ressources résidentes,
  allocations/descriptors et état de frame. La sélection de variante est amortie
  par lot ; les états/ressources vivants ne deviennent pas des paramètres de type.

## Préconditions

Lire `AGENTS.md` ; charger `artist-build-validation` avant compilation/test.
Pour une frame incorrecte, ajouter `artist-gpu-debug`, pas tous les skills.
Relever Windows/build, adapter LUID/GPU/driver, SDK/runtime (Agility si utilisé),
interfaces et résultats `CheckFeatureSupport`. Une voie WARP doit être déclarée
comme logicielle ; elle ne prouve pas le comportement/performance du GPU cible.

L'audit T02 a observé **DXC 1.8** ; c'est **ni device ni frame**. Toolchain/headers,
compilation DXIL, PSO créé, exécution GPU et image correcte sont des preuves
différentes. Sans Windows/device/capacité ou harness autorisé, la procédure reste
documentaire et l'exécution **INCONCLUSIVE**. Aucun SDK/driver à installer.

Debug layer et GBV sont conditionnels et configurés avant device ; validation
offline et réflexion ne les remplacent pas. PIX/RenderDoc ne sont pas présumés
disponibles : aucun MCP de capture garanti, aucune installation au chargement.
Le lead réserve les sondes catalogue/canaris/GPU et sorties partagées ; ne pas
sonder en parallèle ni utiliser un autre transport pour contourner ce bail.

## Procédure

1. Écrire la reproduction et l'oracle **avant** modification. D12-N1 du diagnostic
   propose une frame minimale ; ce n'est pas un test exécuté dans ARTIST.
2. Tracer création/upload/binding/draw ou dispatch/consommation/retrait, avec
   propriétaire, génération, subresources, queue, état/accès et token de fin.
3. Vérifier root signatures, heaps, PSO et ABI CPU/GPU ; aucun wait ne corrige un
   mauvais binding. Conserver rendu et ownership lors du patch.
4. Traduire le ledger RAW/WAR/WAW du propriétaire ressources : transitions,
   UAV/aliasing, fences inter-queues, retraite CPU ; Enhanced Barriers seulement
   après gate. Une génération ne vaut pas attente GPU.
5. Tester le nominal puis le négatif correspondant : D12-X1 allocator en vol,
   X2 state absent, X3 binding incompatible, X4 owner périmé, X5 feature absente.
   Ne pas provoquer d'UB GPU pour démontrer un garde de réutilisation CPU.
6. Rapporter logs/HRESULT et observation ; retrait toutes queues avant réemploi,
   pas un wait global conservé pour cacher le défaut. Une optimisation exige sa
   campagne séparée à qualité/workload égaux.

## Preuves et retour

Fournir backend, HEAD, hashes, delta, chemins/ownership, binding times, matériel,
capacités observées/activées, commandes et options exactes, nombre de tests,
échoués/skips, shaders/scène/frame, attendu/observé, source de chaque résultat.
**PASS** = critère réellement vérifié ; **FAIL** = écart reproduit ;
**INCONCLUSIVE** = précondition ou preuve manquante. zéro test ou skip requis
ne vaut pas PASS. Distinguer `executed`, `static-presence`, `inference`.

Texte/regex ≠ chargement natif, sélection, enforcement des permissions ou GPU.
Une capture n'est pas une mesure ; qualifier le coût CPU/GPU comme estimé ou
mesuré, avec protocole dans le second cas. Toute revue indépendante est demandée
via le lead.

## Références et limites

[Sources officielles et dispositions](references/sources.md) : rédaction originale,
liens seulement vers les tiers ; aucune copie/import ni nouveau superviseur.
Ces références donnent des contrats API, pas des backends ARTIST adoptés ni une
garantie de disponibilité de leurs versions sur la machine.
