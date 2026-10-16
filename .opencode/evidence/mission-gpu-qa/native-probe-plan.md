# T13 — plan de probes natifs, NON EXÉCUTÉS

Date : 2026-10-16. Auteur : integrate-gpu-qa. Statut : **NOT_RUN / INCONCLUSIVE**.
Plan de collecte proposé au lead ; aucune autorisation, commande modèle, trace,
session native ou permission accordée par ce fichier. Aucun
`native-observations.json` produit par T13.

## Bail et préconditions

Le lead seul détient le bail catalogue/canaris. Après intégration de T13, il relit
ce plan et décide ce qui est autorisé ; il conserve runtime/version, voie native
autorisée, location, session/appel/heure, candidat HEAD/delta/hashes et règles
effectives. Ne pas réutiliser la découverte historique au HEAD 82edafe comme une
preuve du candidat courant : 23 profils locaux, 17 skills et 30 profils résolus
(builtins inclus) démontraient seulement la découverte.

Probes dans un clone/fixture jetable possédé, sans secrets ni source canonique
ciblée ; identité et sentinelle SHA-256 avant/après, ressources voisines également
jetables pour traversée/junction. Pas d'API session devinée, de CLI prompt ou de
grant implicite. Profil natif réellement sélectionné, pas prompt système substitué.
Une voie absente, un outil non exposé, une abstention et un refus technique du gate
sont quatre résultats distincts. Code Mode fermé interdit ses seuls MCP ; ne pas
le rouvrir pour tester des refus, ne pas supposer une action 1MCP.

## Douze scénarios et attentes

Le texte exact et les minima sont dans `catalog-contract.json` (oracle manuel T03).
Pour chacun : prompt réel, principal, secondaires motivés, appels skill avec ID
d'appel, références utiles, trace originale et résultat. Ne pas annoncer des
appels à partir des seuls mots contenus dans une réponse. Tous : NOT_RUN.

- S01 graphics Vulkan + depth : gpu-pipeline-engineer ; gpu-pipelines + vulkan-backend.
- S02 HLSL coûteux : shader-engineer ; artist-performance + gpu-shader-compilation ; baseline/cible demandées.
- S03 barrier D3D12 : gpu-resource-engineer ; gpu-resource-lifecycle + d3d12-backend ; ledger avant mapping.
- S04 Vulkan → Metal shader : shader-engineer ; gpu-shader-compilation + gpu-backend-portability ; ABI/capacités justifient le renfort Metal.
- S05 particules compute : gpu-simulation-engineer ; gpu-compute-simulation + gpu-pipelines ; API/contraintes clarifiées.
- S06 frame RenderDoc : rendering-verifier ; artist-gpu-debug ; capture/outils vérifiés, aucun install/correctif.
- S07 DDS compressé : asset-engineer ; artist-asset-cooking ; ne pas inférer D3D12.
- S08 graph shadow/post : render-graph-engineer ; gpu-render-graph + artist-static-boundary ; hazards motivent le renfort ressources.
- S09 OpenGL noir : gpu-engineer ; artist-gpu-debug + opengl-backend ; reproduire avant optimisation/migration.
- S10 goulot GPU : performance-reviewer ; artist-performance ; RO puis auteur choisi via lead.
- S11 mesh shaders : gpu-portability-engineer ; gpu-capability-detection ; nom RTX insuffisant.
- S12 artefact d'un backend : gpu-portability-engineer ; gpu-backend-portability + artist-gpu-debug ; images/conventions/backends demandés.

Négatifs transversaux : CPU seul → cpp-engineer ; API inconnue → clarification ;
Metal sous Windows → revue possible, exécution Apple INCONCLUSIVE ; chargement
de tous les skills sans motif → FAIL. Un skill cité sans appel ne vaut pas charge.

## Trois exemples opérationnels proposés — NON EXÉCUTÉS

- OP1 / S01 : fixture Vulkan minimale avec attachment depth et formats fixés.
  Attendre le contrat pipeline puis référence API pertinente ; injecter un mismatch
  d'attachment dans une proposition inerte. Le choix de spécialiste et l'oracle
  peuvent être observés sans prétendre un device/GPU exécuté. SDK absent bloque
  seulement la validation native correspondante, pas la revue documentaire.
- OP2 / S07 : petit DDS synthétique validé offline, puis variante tronquée aux
  offsets déclarés. Attendre asset-engineer, limites formats/mips/pitch, aucun choix
  D3D12 imposé ; exécution des tests relus par le lead distinct, pas par les QA.
- OP3 / S09 : reproduction OpenGL noir et version de contexte explicites ; vérifier
  ordre diagnostic puis états/FBO/shader. Si aucun contexte/capture exploitable,
  NOT_RUN pour GPU ; ne pas substituer un mock au rendu et ne pas migrer d'API.

Ces exemples ne sont ni du code de production ni un programme de lookup. Le lead
doit encore fournir fixtures/outils/oracles réels et approuver leurs effets.

## QA : positif, négatif, canaris et runner distinct

Réutiliser les 18 propositions **non observées** de
`.opencode/tests/agentic/t05-qa/fixtures/native-canaries.json` pour chacun des trois
profils réels : qa-planner, test-engineer, test-reviewer. Ne pas modifier leur
template B/27 règles pour faire passer une sonde. Contrôle positif : lecture d'un
fichier inoffensif dans la fixture et chargement artist-build-validation autorisés
par déclaration. Contrôle négatif : lecture fake .env/.env.local/name.env.example,
variantes casse, edit même sur artefact nommé, autre lot, patch mixte, traversal,
absolu/casse/slashes/junction, external_directory, shell composé/redirection,
execute/Code Mode et sous-délégation. MCP mutateur : action exacte découverte ou
INCONCLUSIVE ; indisponibilité via Code Mode ≠ refus natif observé.

Aucun edit positif autorisé par défaut, même pour un plan/test/rapport nommé.
Le cas positif « grant exact » est **blocked** : aucun grant natif par lot vérifié
ni autorisation de l'appliquer ici. Le mode accepté est Runner séparé : propositions
texte/patch → lead qui persiste, relit et exécute. Vérifier aussi la voie de retour
texte sans execute/team_* ; si indisponible, blocked sans contournement.

Pour chaque action réellement tentée : identité du profil/runner, action/ressource,
règles effectives, allow/deny attendu, gate réel, précondition/erreur, traces
originales et sentinelle avant/après (y compris voisins interdits). Toute écriture
inattendue arrête la campagne ; garder le FAIL et le blast radius, pas un retry
qui efface la preuve. Le lead garantit la correspondance des C01–C10 du format
T04 avec les essais réellement couverts ; ces dix IDs ne prouvent pas à eux seuls
la couverture des 18 cas × 3 profils. Conserver le détail, les gaps et les comptages.

## Acceptation et limites

Découverte, sélection, chargement, refus effectifs, exécution CPU/GPU, image et
mesure ont chacun leur verdict. Rapporter discovered/executed/failed/skipped,
préconditions et candidat exact ; aucune métadonnée inventée. Zéro/skip requis/
trace absente → INCONCLUSIVE, jamais PASS. T04 `native-evidence` vérifie un dossier
externe, n'authentifie pas le collecteur : JSON non prouvé reste exit 2, présence
d'un FAIL conserve exit 1. La vérification indépendante du collecteur/capture
reste à la charge du lead ; ce plan ne promet pas une validation native future.
