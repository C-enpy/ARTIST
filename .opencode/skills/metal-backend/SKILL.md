---
name: metal-backend
description: Diagnostiquer ou concevoir un chemin API Metal 3/4 et metal-cpp, ses familles/SDK Apple, lifetimes, heaps/résidence, encoders/pipelines, hazards et drawable/present/capture. Utiliser pour une question Metal native ou un portage qui touche ces contrats ; séparer revue Windows et validation Apple.
---

# Metal — expertise API, sans backend ARTIST implicite

## Déclencheurs et exclusions

Charger pour une question explicite Metal/metal-cpp, une erreur d'encoder ou de
pipeline Apple, un resource lifetime, une résidence ou présentation Metal.
Commencer par demander l'API si elle manque : un shader `.metal`, un Mac dans
une description ou un DDS ne prouve pas le chemin API réellement utilisé.

- Pour « shader Vulkan vers Metal », `shader-engineer` reste principal avec
  [gpu-shader-compilation](../gpu-shader-compilation/SKILL.md) et
  [gpu-backend-portability](../gpu-backend-portability/SKILL.md). Charger ce skill
  seulement quand famille, ABI native, pipeline ou ressources Metal le demandent.
  Expertise Slang possible **sans adoption de Slang** dans le moteur.
- Pour un contrat général de ressources/hazards, passer par le lead vers
  `gpu-resource-engineer` et
  [gpu-resource-lifecycle](../gpu-resource-lifecycle/SKILL.md) ; ici, traduire le
  contrat en mécanismes Metal, pas redéfinir le graphe ou le stockage des assets.
- CPU seul → `cpp-engineer` ; OpenGL noir → `gpu-engineer` ; diagnostic de
  performance → `performance-reviewer`. Ne pas charger tout le catalogue.

## Pin ARTIST et binding times

Pin documentaire de départ : HEAD `40a7cc3eac80a4a2d344116365384d48bf92ae48`.
Relever à chaque mission **HEAD, hashes, delta** ; signaler un pin divergent sans
réécrire les consignes. Le backend implémenté est OpenGL 3.3 core dans les tests,
pas Metal. `PipelineContext` stocke des `unique_ptr<IPass<API>>` et `getPass`
retourne un emprunt ; ne pas réintroduire le stockage partagé historique.

ADR 0001/0002/0004/0006/0007 : **Proposed** ; 0003/0005 : **Accepted/partial** ;
0008 : **Accepted, vérifié Windows Debug**. Ce skill n'adopte aucune ADR.
Conserver structures et contrats de compilation statiques, shaders/variants
bornés au cook, état vivant compact au runtime. Pas de type par texture/émetteur.
Un artefact cook immuable/versionné a une clé source/options/cible/ABI ; il ne
possède pas les objets driver, les drawables ni les ressources d'une frame.
La création/link du pipeline natif et les capacités du device restent à vérifier.
Le batching peut amortir encodage/bindings CPU ; il ne supprime ni hazards ni
coût GPU et ne démontre aucun gain sans mesure.

## Préconditions et procédure minimale

1. Définir symptôme et attendu, candidature, allowlist et bail exclusif pour les
   tests/captures. Ne rien installer, ne pas sonder le catalogue ou un GPU partagé
   sans autorisation. Windows autorise une analyse documentaire/offline, pas une
   validation Apple finale implicite.
2. Pour OS/famille/SDK, appliquer **P1** dans les
   [procédures](references/procedures.md). Décrire observé, supporté, activé et
   compatible séparément, puis fallback/refus avant appel non disponible.
3. Pour un lifetime/hazard, suivre **P2–P3** : owner/emprunts/génération, portée
   CPU/thread, allocations et sous-plages, upload, bindings, encoders/queues,
   consommation, complétion, retrait. Une résidence n'est pas un retain CPU.
4. Pour un shader/pipeline/image, suivre **P4**, isoler un pass, vérifier ABI et
   colorimétrie avant de toucher l'ordonnancement. Pour un drawable/capture,
   suivre **P5**. Ne pas remplacer ce diagnostic par un wait global permanent.
5. Établir les oracles **P6** avant correction : nominal, ressource en vol,
   famille non supportée, ABI incompatible. Charger
   [artist-build-validation](../artist-build-validation/SKILL.md) avant exécution.
6. Lire les [sources et dispositions](references/sources.md) pour vérifier la
   version des APIs utilisées. Si l'environnement diffère, conserver les inconnues
   plutôt que copier une recette Apple 26 dans un chemin Metal antérieur.

Les commandes des références sont des procédures conditionnelles, pas des
scripts exécutés au chargement. Les liens vers skills T10/T11 sont contractuels ;
leur existence et leur découverte natives se contrôlent après intégration.

## Oracle et retour par niveau

- **Documentaire / offline** : source/options/cible et ABI inspectées ; texte MSL
  généré, compilation Apple et pipeline natif sont trois preuves différentes.
- **Natif OpenCode** : sélection/appel de ce skill à observer par le lead sur le
  candidat intégré. Présence statique ou regex ≠ routage ou permission effective.
- **GPU Apple** : device/queues, erreurs de compilation/pipeline/command buffer,
  validations activées et résultat sur appareil réel. SDK absent du PATH ne suffit
  pas à conclure à une absence absolue ; précondition non obtenue = INCONCLUSIVE.
- **Image** : scène/caméra/frame/résolution/seed gelées, métrique et tolérance
  décidées avant l'essai. Une capture API sans comparaison ne prouve pas l'image.
- **Performance** : CPU submit, GPU, attentes et pacing séparés, runs comparables
  et dispersion ; une capture ou un seul temps ne prouve pas une amélioration.

PASS uniquement pour le critère observé ; FAIL si écart reproduit ; INCONCLUSIVE
si précondition manque. **zéro test**, skip requis, mock ou validation offline ne
valent jamais PASS GPU. Rapporter commandes/versions, HEAD/hashes/delta,
découverts/exécutés/échoués/skipped, logs/messages utiles et artefacts réellement
créés. Aucun coût CPU/GPU mesuré n'est fourni par ce skill documentaire.

## Handoff

Agent feuille : `DEFER_TO` au lead avec contrat, chemins propriétaires, capacités,
oracle, essais, inconnues, bail et preuves. Demander `rendering-verifier` pour une
vérification indépendante ; l'auteur ne se qualifie pas d'indépendant. L'absence
de SDK Apple bloque la preuve native, pas la rédaction honnête d'une procédure.
