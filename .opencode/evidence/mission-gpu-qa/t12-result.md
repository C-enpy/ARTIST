# T12 — Assets, performance et validation commune

Date : 2026-10-16. Auteur : pool-11 / asset-engineer. Tâche
`task_mvbbntgk_0036_scsn03`. PM/LLM brut/retenu **complexity 74 / risk 57**,
sans override modèle/variant ni sous-délégation.

## Candidat et périmètre

Base vérifiée avant écriture : `40a7cc3eac80a4a2d344116365384d48bf92ae48`, worktree
initialement propre. Worktree écrivain exclusif :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-11`.
Identité de branche assignée :
`ensemble-artist-mcp-lifecycle-mission-e52f73a55e40983f#mzvoa2-pool-11` ; Git
observait un HEAD détaché dans ce worktree, sans tentative de réattachement.
Le commit de livraison sera communiqué au lead avec le résultat.

Le cwd initial pointait sur le canonique ; aucune écriture n'y a eu lieu.
Déplacement de session vers le worktree attribué avant les modifications.
Contrat T03 lu en premier ; QA plan, T02 inventaire/sources, préflight natif et
brief d'exécution présents. Canonique AGENTS.md, GPU.md et QA.md lus seulement ;
aucun diff utilisateur copié. Canon docs/engineering-principles, ADR README et
correction-plan consulté. Seuls les chemins exacts du manifest T12 sont modifiés.

Fichiers livrés (14) : deux agents, trois SKILL.md, cinq références, trois
fichiers de contrat/fixtures et ce rapport. Liste exhaustive dans
`.opencode/tests/agentic/t12-assets-performance-validation/manifest.json`.
Permissions existantes conservées à l'identique (deny subagent pour les deux,
deny edit pour performance-reviewer) : aucune résolution implicite de D-PERM.
Moteur/shaders, build/Conan, build-engineer, AGENTS, voisins, commandes, politique,
ensemble.json et configuration globale exclus. Aucun nouveau skill/profil.

## Pin observé

`PipelineContext.hpp` recoupé : addPass/stockage `unique_ptr`, getPass emprunt.
La photographie shared_ptr dans engineering-principles est historique ; ni elle
ni le profil sont réécrits automatiquement. OpenGL est le backend présent ;
Vulkan/D3D12/Metal restent des expertises. Cooker/streaming/animation non établis
par le pin : aucune API de ces sous-systèmes inventée. Recherche GrepAI canonique
« asset cooking texture mesh animation streaming import cache » : quatre hits
dans research, pas preuve d'implémentation. Aucun chemin source nommé cook/stream/
anim/mesh/texture dans la liste ARTIST/include ; ce filtre n'est pas une preuve
exhaustive d'absence de fonctionnalité. Serena ARTIST canonique confirmée, aucune
réactivation ni analyse de comportement C++ revendiquée.

ADR-0001/2/4/6/7 Proposed, 0003/5 Accepted/partial, 0008 Windows vérifié :
statuts inchangés. Conan 2/CMakeDeps/CMakeToolchain recoupés dans conanfile.txt ;
le profil build-engineer obsolète reste réservé à T13. Baseline du skill build
2026-10-03 (87 unitaires + 22 OpenGL) conservée comme historique, pas nouveau run.
Aucun comptage historique des notes/QMD n'est promu en preuve de mission.

## Contrat livré

- `artist-asset-cooking` : entrée légère ; références **data-contract**, **geometry**
  et **textures** séparées. Source/IR/cook immuable/runtime, unités/espaces,
  endianness/alignement/version/sections, validation overflow/offsets, dépendances
  et clés d'invalidation. Identité persistante/révision/handle/résidence/objet GPU
  séparés ; annulation/remplacement et staging en vol restent contrat transmis
  au propriétaire ressources. PSO natif lié device/driver, pas cache portable.
- Géométrie : tangentes/handedness et UV miroir, indices/remaps/cache,
  quantification, meshlets/LOD avec borne d'erreur ; aucun mesh shader garanti.
  Textures : DDS legacy/DX10, BC/ASTC, mips/sRGB/alpha, blocs/pitch/offsets,
  tailles checked ; DDS ne sélectionne pas D3D12. Bibliothèques candidates
  évaluables par fidélité/déterminisme/plateforme/licence/coût, non adoptées.
- `performance-reviewer` / `artist-performance` : diagnostic RO, A/B équitable,
  warm-up/répétitions/dispersion, CPU submit/GPU/waits/spacing/compteurs,
  code-size/build et coûts déplacés. Correctif ensuite par auteur du goulot
  choisi via lead, pas nouveau profil auteur performance.
- `artist-build-validation` : référence commune **validation-levels**, CPU,
  offline, GPU, image, performance ; candidat/comptages/skips/stale, niveau de
  preuve et verdict borné. Aucun skill `gpu-validation` redondant.

## Exécution locale RED → GREEN et non-régression

Environnement : Windows x64, **Node v24.11.1** vérifié par `node --version` ;
Context Mode `ctx_execute` appelle le Node du PATH (le runtime Bun du sandbox
n'est pas la toolchain des tests). Commandes depuis le worktree ci-dessus :

```text
node --test .opencode/tests/agentic/t12-assets-performance-validation/contract.test.mjs
git diff --check
git diff --name-only
git ls-files --others --exclude-standard
```

Oracle écrit avant modification des agents/skills à partir de T03 §3/4/5/6 et
QA plan G-01..08/T-07. Test Node autonome, sans import T04, branche voisine,
stub/routeur, dépendance installée, ni exécution de commande issue de fixture.

- **RED observé** : exit 1, 16 tests exécutés, 3 PASS / 13 FAIL, 0 skipped,
  0 cancelled, 0 todo. Échecs causaux : sections/exclusions/références attendues
  absentes dans les documents préexistants ; notamment DDS et diagnostic RO.
  Assertions lisibles « contrat documentaire absent », pas erreur toolchain.
  Le second relevé identique expose les comptages (durée 85.0292 ms).
- Premier contrôle après rédaction : 14 PASS / 2 FAIL, 0 skipped. Les mots
  « handle générationnel » étaient coupés entre lignes et « offline » n'était
  présent qu'en titre majuscule. Correction rédactionnelle, tests inchangés ;
  aucune assertion/tolérance affaiblie. Aucun faux RED moteur revendiqué.
- **GREEN** : deux runs 16/16, exit 0, aucun skip/échec/annulation/todo ;
  durées 103.777 puis 96.4697 ms. Hash agrégé des 13 fichiers alors présents
  identique avant/après :
  `3d20b746ec030ad4608ca9c7b1229d6706505fba2c4a0590fefafce29b635b3c`.
  Cela prouve seulement répétabilité/sans mutation du contrôle documentaire.
- **Non-régression** après précision V2 `name`/ID : le 2026-10-16 à
  19:04:00Z, 16/16, exit 0, 0 skipped/cancelled/todo, 103.2162 ms.
  `git diff --check` exit 0 ; cinq fichiers suivis modifiés + huit nouveaux
  avant ce rapport, aucun hors allowlist. Durées de tests ≠ benchmark moteur.

Seize tests distincts par run, pas cumulés en une fausse suite plus grande.
L'exécution prouve les contrôles locaux de texte/liens/metadata et conservation
des permissions déclarées ; elle ne prouve pas parser YAML V2 complet, routage,
enforcement, import DDS, GPU ou gain. Les deux erreurs de scripts exploratoires
(échappement newline, `tools` indisponible dans sandbox) ont été corrigées, pas
qualifiées de panne MCP ou de RED. Aucun test C++/GPU/benchmark lancé.

## Fixtures, couverture et limites de preuve

`fixtures/scenarios.json` est inerte, oracle explicite :
- D-01 : DDS BC1 5×3, trois mips 16/8/8, offsets 128/144/152, 160 octets.
  D-02 : 159 octets tronqué ; D-03 : format non supporté. Arithmétique vérifiée
  dans le contrat, mais aucun fichier DDS binaire ni importeur exécuté.
- P-01 : goulot sans mesure → HYPOTHESIS, diagnostic RO ; P-02 : A/B inéquitable ;
  P-03 : capture seule/run unique → aucun gain prouvé.
- V-01..06 : CPU réussi/GPU indisponible, zéro test, skip requis, ancien candidat,
  FAIL avec blocage conservé et PASS CPU limité au critère satisfait.

G-01 : sources/dispositions documentées, aucune copie ; G-02 : périmètre T12
couvert par procédures/oracles ; G-03 : metadata/liens locaux contrôlés, découverte
native **INCONCLUSIVE**. G-04 : DDS/goulot décrits, sélection native **non exécutée**.
G-05/G-06 : distinctions de preuve contrôlées textuellement, pas validation moteur.
G-07 : contrôle du delta T12 PASS, pas canari permissions. G-08 : réexécution locale
stable, pas idempotence du futur harness. T-07 : hashes ci-dessous, méthode stale
documentée ; enforcement de validation du candidat T04/T13 non évalué.

Round-trip, déterminisme du cook, invalidation de cache et annulation en vol :
oracles proposés, **INCONCLUSIVE** sans sous-système/fixtures exécutables moteur.
Coûts **import**, **cook**, **load** : non mesurés séparément, aucun chiffre ni
gain inventé ; coûts frame/GPU, code-size et build également non mesurés.

## Sources et dispositions

Rédaction originale. T02 utilisé comme audit documentaire, jamais autorisation
d'import. Aucune adaptation/copie de tiers, aucune résolution tree SHA → commit
prétendue, aucune dépendance choisie/installée. Licence inconnue : lien seulement.

Consultations HTTP via Context Mode le 2026-10-16 : OpenCode **V2** agents/skills
(2/2 accessibles), Microsoft DDS guide, dépôts officiels DirectXTex, DirectXMesh,
meshoptimizer, ARM astc-encoder (5/5 accessibles). Concordance DDS pitch/blocs et
familles de traitement recoupée ; pages courantes, pas pins de versions exactes.
DirectXTex/DirectXMesh MIT selon audit T02, sans adoption ni réutilisation de code.
Liens additionnels MikkTSpace, Nsight Systems, GPUOpen, Google Benchmark, CTest et
Conan 2 servent de références/candidats ; versions/licences précises non auditées
pour copie. Toute future adaptation exige commit/chemin/notices exacts.

## DEFER_TO et verdict

**PASS local documentaire** : 16 contrôles, rédaction dans l'allowlist, aucune
permission modifiée. **INCONCLUSIVE natif/GPU/performance**, hors campagne T12.
`DEFER_TO` lead/T13 après shutdown/merge : union manifests/T04, discovery globale
et observations natives DDS + goulot, puis enforcement sous ruling D-PERM/bail.
Les futurs IDs de spécialistes sont références du contrat, pas stubs à créer ici.
Lead → gpu-resource-engineer pour uploads/retrait, gpu-portability-engineer pour
formats/features, shader-engineer pour ABI si une implémentation est demandée.
Pas de sonde catalogue/canari, capture/benchmark ou indexeur partagé en parallèle.
Aucun Docker créé, aucun registre Docker requis ; aucune installation/SDK/driver.

## SHA256 du candidat testé (bytes worktree, hors ce rapport)

```text
2eb9d3c0b868c2d3f15d189abc8b6930056c17f7e9b054b2e5bd40db9416611b .opencode/agents/asset-engineer.md
96790babf3c8a7f2037afac90b7d1ba0ef13701ee7372c0153fec1522fd44a9b .opencode/agents/performance-reviewer.md
5cc59f516db967eb3bf68f20e98a1213e227d9546bec0afbd1b1243cb0ef5098 .opencode/skills/artist-asset-cooking/SKILL.md
caa25dcf0a641a234393e2a3f5f41a6cc54f12e3c3c8a2d00809813b98fb4cd5 .opencode/skills/artist-asset-cooking/references/data-contract.md
390c8df8f919f5d106bfdb70fba42ff244bf2bdaed70f731cd12832e8d19be48 .opencode/skills/artist-asset-cooking/references/geometry.md
93007eef89de8ac78db4e10eda6ea127e122d3412ddd5090c9cf5e0971d1fceb .opencode/skills/artist-asset-cooking/references/textures.md
2355fa56d91d40df53f72b55739e58e029865d4892ad0b5a7d24335820ef1169 .opencode/skills/artist-performance/SKILL.md
430f3ee28f421c426e13cddef371dbfadd5936180d25fb0b1b2f6c46dc3210b8 .opencode/skills/artist-performance/references/protocol.md
01cdc5a5959fc436c1739da1a04c81cd9c37eebc1b1852357445d9b5c95cbfdc .opencode/skills/artist-build-validation/SKILL.md
c0e0a23eb1713c73d701bcffc71ee710c365110e81959339b8343f6fe558d6ff .opencode/skills/artist-build-validation/references/validation-levels.md
dfbbb407ffdce533627abe9b537d6a9727f0398b1001531890f5cc456a8297fd .opencode/tests/agentic/t12-assets-performance-validation/contract.test.mjs
ba23d1b9a3219b986d206ea05fb887e1a3c517de97768d53a86a5631cb855a86 .opencode/tests/agentic/t12-assets-performance-validation/manifest.json
51e6c6e00f32e3fa59d6f33c4a0fabb9273a52e30292973c2e5ce26d9ca78ca1 .opencode/tests/agentic/t12-assets-performance-validation/fixtures/scenarios.json
```
