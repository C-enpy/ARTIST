---
title: "Contrats lifetime OpenGL — mission 2026-10-19 (K)"
category: architecture
tags: [artist, architecture, rendering, testing, contracts]
aliases: [opengl-lifetime-contracts]
summary: "Contrats adoptés pour VBO, réflexion Pass, refus Uniform et fixtures Loader ; interfaces gelées A/C/H→B ; oracles RED et séquencement."
status: adopted-mission
implementation: not-started
source_revision: cd1b598c77bc22ae876543bea6e0aae104792649
created: 2026-10-19
updated: 2026-10-19
sources:
  - "opencode:ses_eacc59811ffepcZr1QoG3IMZr3"
  - "architecture/next-mission.md"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Contrats lifetime OpenGL — mission 2026-10-19 (K)

> [!important] Portée et autorité
> Contrats adoptés pour la mission [[architecture/next-mission|lifetime OpenGL]],
> valables pour elle. **Aucune promotion d'ADR** : 0001/2/4/6/7 restent
> Proposed, 0003/5 Accepted partial, 0008 vérifié Windows Debug seul.
> Implémentation non commencée au moment de l'adoption.

## Autorité et décisions

- **Gate humain 2026-10-19** (5 ratifications) : atomicité par Reader ; rejet
  Uniform avant mutation CPU ; owner natif VBO au contexte Attribute avec
  emprunt legacy explicite ; dtor owner non vide sans contexte = fail-fast
  sans appel GL ; tranche runtime = contexte courant exact GLFW/GLEW,
  sans gestionnaire global ni share-groups.
- **Décisions lead complémentaires** (coordination, dans le cadre ratifié) :
  `setBufferID` = adoption exclusive (l'alternative `adoptBufferID` séparée
  ajouterait une API publique non ratifiée, évolution future explicite) ;
  gel d'interfaces hook/programID ; séquencement A‖C‖H → B ; M écrivain
  unique des mocks communs.
- **Baseline exécutée (K5)** : [[/docs/_meta/session-2026-10-19-opengl-lifetime-baseline.json|manifeste]]
  — 326/326 PASS (266 unit + 60 opengl), 0 skip, logs et hashes binaires
  archivés sous `out/`.

## K1 — VBO (branche A, gpu-engineer)

Ledger : `Empty` (id 0), `Owned` (id nonzero + token de contexte GLFW exact),
`Retired` (retrait logique après release), wrapper `MovedFrom` (`unique_ptr`
nul).

- `setBufferID(nonzero)` = **adoption exclusive** : transfert du nom ; même
  ID = no-op ; ID différent = prévalidation puis release de l'ancien sous le
  bon contexte **avant** adoption ; zéro = release. Le caller cesse son
  cleanup manuel après transfert ; pas de mode VBO externe borrowed.
- `getBufferID` = emprunt seul ; ne confère aucun droit de suppression ni
  d'usage après release.
- Interfaces gelées A : `validateDestructionContext() const` (base générique
  no-op, OpenGL vérifie si `Owned`) et `releaseNativeResources()` ;
  `IAttribute::free()` idempotent, no-op en moved-from.
- `free` conserve CPU/metadata et remet l'ID à 0 ; `bind` peut recréer ;
  `set` sans buffer = `BUFFER_ID_NOT_SET`.
- `glGenBuffers` local initialisé à 0 ; retour 0 → `BUFFER_CREATION_FAILED`,
  sans publication. Rebind conserve le nom. `glIsBuffer` n'est pas exigé pour
  adopter un nom glGen pas encore bindé.
- Dtor `Owned` : contexte exact courant exigé ; absent/mauvais → diagnostic
  fail-fast **sans aucun appel GL**. Release explicite, move-assignment et
  adoption en mauvais contexte : rejet avant suppression/transfert, owners
  inchangés. `Empty`/moved-from ne demandent pas de contexte.
- **Extension ratifiée (gate du 2026-10-19, R-Q1)** : `Attribute::set` prévalide
  le contexte **avant toute mutation CPU** (parité K3) — sous mauvais/absent
  contexte, `set(valueB)` rejette sans upload **et sans remplacer** `valueA` ;
  zéro appel GL ; ancienne valeur CPU et ID conservés. (Le comportement
  historique stockait la CPU avant le garde du Setter.)
- Move : ctor du wrapper transfère le pointeur sans GL ; move chargé→chargé
  prévalide puis retire la destination avant transfert, sans copie CPU ;
  self-move = no-op ; contextes natifs non copiables, leur move éventuel
  transfère/vident l'identité.
- **Exact-once** = une émission `glDeleteBuffers` par ownership nonzero ; ne
  prouve pas la libération physique. Pas de nouvelle utilisation après
  retrait ; commandes déjà soumises et références VAO relèvent du driver GL.
  Pas de `glFinish`/fence par Attribute, pas de registre/générations/
  share-groups/gestionnaire global. Le contexte parent reste vivant jusqu'au
  dernier owner ; release avant teardown GLFW ; le token ne prolonge pas la
  vie du contexte.

## K2 — Réflexion Pass (branche B, cpp-engineer)

- Staging local complet **par Reader** ; validation/allocation avant
  publication ; succès = remplace **toute** la map du Reader et retire les
  anciens noms absents.
- Count 0 = commit vide ; count négatif = `INVALID_COUNT` sans mutation.
- Échec tardif : ancienne map/pointees/types/valeurs intacts, **aucun
  préfixe** ; les borrows anciens sont invalidés **au commit seulement** ;
  un échec les conserve.
- Pas de transaction Pass : Uniform peut réussir puis Attribute échouer ;
  le metadata ancien conservé ne restaure pas l'ancien programme. Les
  bindings après free/relink sont invalides pour upload même si les objets
  CPU vivent ; `programID` n'est pas une génération.
- B possède les maps `aliases`/`replaceUniforms`/`replaceAttributes` ;
  prévalide **tous** les anciens Attribute via le hook A avant swap, sans
  allocation ; retrait des anciens owners sous bon contexte avant retour.
  `addAttribute` ponctuel prévalide l'ancien owner avant remplacement ;
  jamais de `glDelete` direct dans PassContext.
- **Amendements de tests explicites** (sans suppression d'assertions) :
  `ReadUniforms_LateFailureKeepsPublishedPrefixAndPreviousEntries` — ancien
  `reloaded` size 2/pointee conservé, **pas** size 3 publié ; les deux
  `DuplicateName` — map vide reste vide, map préremplie reste identique.

## K3 — Uniform (branche C, cpp-engineer)

- Refus déterministes **avant** `setValue` ; après rejet : ancienne valeur ET
  type observables (ou `UNINITIALIZED`), zéro `glUniform*`. Zéro upload ≠
  zéro query GL de validation. Erreur de copie = CPU conservé avant upload.
  Pas de rollback driver/image promis après erreur native non interceptée.
  Le Setter direct reste défensif ; `context.setValue` demeure une mutation
  bas niveau.
- Cardinalité admise = 1 : size 0/>1 → `ARRAY_NOT_SUPPORTED` avant mutation.
  **Préserver** le tableau mono-élément size 1 déjà admis ; ne pas importer
  le refus Attribute fondé sur le suffixe `[0]`.
- 34 familles CPU inchangées (GLfloat/GLint/GLuint/GLdouble ; vec/dvec/ivec/
  uvec 2/3/4 ; 9 matrices float + 9 double). Pas de bool/bvec C++ nouveau.
  `HasComponent` vérifie l'existence du type, pas `::on` : `set<bool>` peut
  être **ill-formed**, pas une exception `UNSUPPORTED_TYPE` prouvée.
- Matrice GLSL admise : float/vecN→float/vecN ou bool/bvecN ;
  int/ivecN→int/ivecN ou bool/bvecN ; uint/uvecN→uint/uvecN ou bool/bvecN ;
  GLint scalaire→samplers (isampler/usampler inclus), pas GLuint/vector ;
  matrices→précision et dimensions identiques ; double/dvec→double/dvec,
  pas bool. Pas d'implicites float↔int, uint↔int, float↔double. Bool GPU =
  0/nonzero sans normalisation de la valeur CPU ; sampler = indice d'unité,
  pas un nom de texture. Égalité naïve d'enums rejetée.
- C ajoute `setProgramID`/`getProgramID`, défaut 0 = manuel/inconnu ; B
  renseigne l'ID après réflexion. Programme courant nonzero et, si ID connu,
  égalité **avant** mutation/upload ; aucun `glUseProgram` implicite, aucun
  `glProgramUniform` nouveau.
- Chemin manuel `glType0`/`programID0` préservé sous préconditions caller
  (type/location/programme) ; pas de validation exhaustive prétendue, pas de
  support/rejet universel des extensions inconnues.
- Fp64 conditionnel : GL 4.0 ou ARB_gpu_shader_fp64 + fonctions disponibles ;
  indisponible = refus avant mutation/upload, pas de downcast. Natif sans
  capacité = skip/INCONCLUSIVE attribué.

## K4 — Séquencement et oracles

Ordre : contrats + Q committés → M commun intégré si utile → A‖C‖H →
headers A+C réellement intégrés → B → I croisé → G‖R → corrections →
V fresh.

Ownership : A = `Attribute`, contextes, composants `attribute/`, tests
Attribute ; C = `Uniform`, contextes, `uniform/Setter.hpp`, tests ; B =
`PassContext`, Readers, tests Readers ; H = les deux `LoaderTests.cpp` +
helper local neuf si nécessaire. M = écrivain unique des mocks communs
(`glDeleteBuffers`, contexte courant, programme courant, capacités), sans
défaut qui masque les préconditions.

Oracles RED attendus :

- **A** : création, rebind, free×2, dtor, move chargé→chargé, self-move,
  remplacement exact-once ; gen 0 ; rejet mauvais contexte conserve l'owner
  sans GL ; death test dtor sans contexte (distingue le fail-fast d'un appel
  GL interdit) ; preuve native upload + teardown. Migrer les cleanups
  manuels des noms transférés (pas de double suppression) ; « Loaded » des
  tests existants = int présent, pas un VBO chargé.
- **B** : première entrée valide, seconde invalide avec ancienne map
  identifiable → aucun préfixe ; succès efface les noms disparus ; vide
  efface ; remplacement VBO = delete 1 ; Uniform succès / Attribute échec
  sans transaction Pass.
- **C** : via API publique — ancienne valeur puis rejet taille/type/
  programme/fp64 → ancien type/valeur ou UNINITIALIZED, zéro upload ;
  positifs 34 familles + bool/sampler contre un faux garde enum==enum ;
  deux programmes avec même location ; defaults/size1 conservés. Natif :
  readback typé cible + decoy inchangé, pas de sonde de conformité driver
  sans obligation moteur.
- **H** : permutations/seeds historiques + contrôle négatif sur candidat
  identifié ; patch seulement si une cause fixture se reproduit, sinon
  caractérisation sans RED artificiel.

## K5 — Baseline exécutée

Manifeste : `docs/_meta/session-2026-10-19-opengl-lifetime-baseline.json`.
Build OK (0 erreur), découverte non nulle, 266/266 unit + 60/60 opengl PASS,
0 skip, sur `cd1b598c77bc22ae876543bea6e0aae104792649` — preuve fraîche
attribuée au lead-runner, non héritée de `f7bb171`. Aucun FAIL causal au
relevé ; un futur FAIL frontal est traité ou arbitré, jamais absorbé par le
326. L'audit final des 12 formes reste une obligation QA persistante.

## Écarts factuels constatés (cible vs code actuel)

`attribute/Binder.hpp` ~27–33, `context/PassContext.hpp` ~51–74, Readers
`pass/AttributeReader.hpp` et `pass/UniformReader.hpp`,
`pipeline/Uniform.hpp` ~93–94.

## Alternatives rejetées

Publication partielle (statu quo), transaction Pass globale, store-then-
reject, borrow importé via setter, gestionnaire global de contexte, retraite
GPU générale, wait/fence par owner.

## Cible statique bornée et coûts

1 profil Classic, 12 layouts Attribute, 34 familles Uniform — pas de variante
par asset. Réflexion et validation du programme réellement lié = préparation
runtime après link GLSL ; aucun nouveau choix link-time C++ ou cook ; le
metadata n'est ni un plan cook immuable ni un état de graphe authoring/frame.
Staging O(n) temporaire, prévalidation O(n) au Reader, queries
contexte/programme par opération ; **aucun gain de performance revendiqué**.
GLFW/GLEW sont déjà des dépendances (`ARTIST/CMakeLists.txt` ~13–15).

## Limites

Nouveaux backends, OS, ASan, Release, image de référence et performance non
validés. Les fichiers externes sales (AGENTS, profils, harness, `.grepai`)
restent préservés hors périmètre.
