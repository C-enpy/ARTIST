# PLAN QA — F7 option A (P1–P3, T1–T3, barrière T4) — v1

Producteur : pool-1 (qa-planner), 2026-10-17. Persisté et commité par le lead (barrière 2
de la boucle QA) — **ce commit précède tout worktree auteur T1–T3**.
Sources : `docs/architecture/next-mission.md` (§Contrats attendus + §Preuves attendues) ;
ruling `t03-contract.md` §6–8 ; format témoin `qa-plan.md`.
Exécution au planner : **lecture seule, zéro exécution** — les commandes ci-dessous
appartiennent au runner mandaté, jamais au planner.

## PIN OBSERVÉ

Pin 2026-10-16/base e4b0aa7 reconduit : ADR-0001/0002/0004/0006/0007 Proposed ;
0003/0005 Accepted/partial ; 0008 verified Windows Debug ; PipelineContext unique_ptr,
getPass emprunt. Brief HEAD eb0dd14, arbre propre. Écarts : b9c90fd (2026-10-17) est
post-pin — statuts non réécrits ; invérifiable ici (git interdit au planner) → le runner
confirme HEAD/delta/SHA256 avant T1, sinon T1 blocked. ADR-0004 reste Proposed : ce plan
n'adopte rien.

## NIVEAUX

Intention = contrats mission ; code observé statique (frais) : O1 UniformContext.hpp:24,31
any_cast forme POINTEUR (résultat jeté, catch mort, TYPE_MISMATCH inatteignable ; message
« SET » pour le GET ; *nullptr si any vide OU incohérent = UB). O2 Attribute.hpp:88,90
hardcode Setter<shared_ptr<int>> : l'oracle mock b9c90fd caractérise le bug, il ne prouve
pas un dispatch correct. O3 opengl/.../attribute/Setter.hpp:30-32
sizeof(attrValue.get()) = 8 constant (x64) pour la taille glBufferData ET le stride ;
data null possible avalée par GL. Inférence : chemin réel (API réelle) ≠ chemin mock —
un set valide peut tomber en getValue mismatch/TYPE_MISMATCH interne ; à confirmer par
RED. Exécution : zéro test ici, unassessed jusqu'au RED/GREEN ; totaux historiques
(165/143/215/176/198) pas des attendus du lot. Mesure : aucune prétention perf ; pas
d'image/draw/readback.

## OBLIGATIONS — nominal/négatif + oracle

- **U1 (T1) TYPE_MISMATCH levé** : nominal setValue(0.5f)→getValue<float> = valeur ;
  négatif getValue<int> → TraceableException motif TYPE_MISMATCH
  (UniformContext.hpp:17-32, Uniform.hpp:91). RED pour absence d'exception/UB, jamais
  toolchain. D1 : texte GET vs SET.
- **U2 (T1) null rejeté** : getValue sur uniforme non-initialisé → erreur typée, pas UB ;
  cas ASan CPU mort (sélection déclarée).
- **U3 (T1) wrapper unique + getContext emprunt** : static_assert !copyable/movable ;
  pattern OwnerState{Loaded,Empty,MovedFrom} (ShaderLifetimeTests.cpp:75-98, précédent
  MovedFromPassContextGetterThrows) ; moved-from → rejet typé (Uniform.hpp:52,96,102).
- **U4 (T1) static_asserts pos/neg motif** : fake-API (PipelineBorrowedContractTests.cpp:
  15-47) : on() emprunté void accepté ; on(shared_ptr) rejeté ; on() non-void rejeté.
  UniformContractTests conservé.
- **U5 (inférée) setValue échouée ne modifie pas m_value** (pas d'écriture partielle).
- **U6 non-rég** : glUniform*_mock exacts inchangés des 9 tests UniformTests
  (witness b9c90fd).
- **A1 (T2) dispatch générique Setter<shared_ptr<T>>** : RED sur l'oracle actuel
  (MockSetter<shared_ptr<int>>) conservé, puis mise à jour strictement plus forte :
  set<float> → MockSetter<shared_ptr<float>> Times(1) identity ; sans composant →
  UNSUPPORTED_TYPE (Attribute.hpp:92-94) ; fake-API matrix.
- **A2 (T2) sizeof(T)** : RED LITTÉRAL set<float> → glBufferData(...,4,data,usage)
  (observé 8) + 2e type vec3→12 ; stride glVertexAttribPointer idem ;
  SetterTests.cpp:39,59 (tautologiques, static-presence) mis à jour SEULEMENT après RED
  conservé, jamais wildcard.
- **A3 (T2) null** : NON_OPENGL_CONTEXT/BUFFER_ID_NOT_SET conservés
  (SetterTests.cpp:66-83) + valeur stockée null → rejet typé AVANT glBufferData
  (aujourd'hui 8+null part au driver).
- **A4 (T2) ownership unique IAttribute + virtuals bind/unbind en emprunt** ;
  MockAttribute adapté même règle que T1.
- **A5 (T2) GL réel (nouveau, label opengl, OpenGLComponentTests.hpp 3.3 core)** :
  bind+set réels sans erreur ; mock ≠ preuve GPU.
- **R1 (T3) concept négatif Reader** : static_assert rejeté au motif
  (UniformContractTests.cpp:27-29).
- **R2 (T3) lecteur durci** : passID==0 → INVALID_PROGRAM_ID ; le négatif uniform-side
  n'existe pas → à ajouter (pattern AttributeReaderTests.cpp:79-87) ; construction avec
  wrapper ownership unique.
- **R3 (T3) GL réel** : 1 uniform + 1 attribute populate ; pas d'image.
- **C1** RED causal par tranche, artifacts out/mission-f7/<tranche>/ (exit, comptages,
  raison). **C2** identité HEAD/delta/SHA256 avant-après, découverte>0, JUnit témoin 65536.
  **C3** ASan clean-first + sélection déclarée avant run + canari artist_asan_probe
  (0 puis 86), premier FAIL conservé ; re-glob CMake à chaque ajout de fichier.
  **C4** registre bidirectionnel : COVERED seulement preuve exécutée qualifiée ;
  static-presence/inference gardent leur tag.

## REGISTRE EXISTANT (sens inverse)

UniformTests (9) : COVERED historique à reprouver sur diff. UniformContractTests : COVERED
static. AttributeTests.SetTest b9c90fd : static-presence de l'oracle du bug (modifiable
après RED dédié). SetterTests CallBufferData/VertexAttribPointer : static-presence
tautologique → abattu par A2. ShaderLifetime/PipelineBorrowedContract : seeds
réutilisables. UniformReaderTests : négatif passID absent → GAP. Nouvelles preuves :
unassessed.

## RISQUES I×P (zones 15-25 critique, 10-14 haut, 5-9 moyen)

- **R2 sizeof silencieux I5×P4=20 prio1.** Déclencheur : tout set attribute. Portée :
  uploads GL réels faux (8 partout). Détection : AUCUNE (oracle tautologique).
  Mitigation : aucune. Gap : oracle littéral + réel-GL (A2/A5).
- **R1 if constexpr I5×P4=20 prio1 ex aequo.** Déclencheur : réécriture A1. Portée :
  chemin réel ≠ mock (TYPE_MISMATCH interne/UB sur usage valide). Détection : mock seul.
  Mitigation : identity + réel-GL + fake-API. Gap : sémantique de A (D2).
- **R3 tolérance élargie I4×P4=16 prio2.** Déclencheur : oracle réécrit sans RED,
  wildcard, ASSERT élargie. Portée : chaîne de preuve. Détection : T5/T6 après T4
  seulement. Mitigation : règle RED-before-oracle + registre. Gap : enforcement
  technique absent (règle + T4, jamais PASS de confinement).
- **R5 preuve périmée I4×P3=12. R4 churn consommateurs I3×P3=9** (flip signatures 30+
  setters uniform T1, 3 comps T2). Réévaluation à l'intégration/finding/near-miss,
  jamais one-shot.

## BUDGET 100

Setup 25 (seeds OwnerState, matrix MockApi, OpenGLComponentTests, oracle b9c90fd,
empreintes, env ASan). Planifié 55/75 (73 %) : T1=14, T2=20 (RED sizeof + réel-GL
priorité), T3=8, commun/audit=13 (C1–C5, registre). Buffer 20 nommé : flakes driver
réel-GL, rebuild/re-glob, ré-audit T5/T6, churn D2/D3. Débordement : haut peu coûteux
avant haut coûteux, jamais buffer ni un haut pour du moyen ; toute coupe = GAP-defer
(propriétaire/échéance/impact).

## DÉCISIONS HUMAINES (tranchées au commit par le lead)

- **D1 — TRANCHÉE** : messages TYPE_MISMATCH distingués GET vs SET, contenant le type
  réel observé (`m_value.type().name()`), motif TYPE_MISMATCH — corrige le message
  trompeur « SET » constaté par P1 ; oracle = motif, pas le texte entier.
- **D2 — PARTIELLE** : la CONTRAINTE est ferme dès maintenant (oracle b9c90fd jamais
  réécrit silencieusement ; update strictement plus forte après RED conservé). Le choix
  du corps du setter générique ( getValue<A>() vs usage valide ) reste au fork T2 après
  consultation de P2 ; décision consignée dans la note T2.
- **D3 — TRANCHÉE** : flips mécaniques compil-only par tranche (T1 : consommateurs
  Uniform ; T2 : consommateurs Attribute/Setter ; T3 : readers durcis), précédent
  Pipeline A2 (adaptation mécanique d'API chez l'écrivain de la tranche). Le
  durcissement sémantique reste T3.
- **D4 — TRANCHÉE** : périmètre GL réel confirmé par le contrat mission : bind/set
  réels sans erreur ; pas de draw/image/readback.
- **D5 — TRANCHÉE** : la sélection ASan des nouveaux cas est déclarée et datée par le
  lead à la barrière T4, avant tout run.

## GAPS

G1 HEAD/identité du candidat invérifiable au planner → runner confirme avant T1.
G2 fixtures réel-GL uniform/attribute inexistantes → tests nouveaux (label opengl,
re-glob). G3 négatif uniform-side lecteur absent → à ajouter. Aucune extension
d'autorisations du planner.

## CRITÈRES DE SORTIE

1. Chaque obligation a ≥1 nominal + ≥1 négatif, oracle discriminant, tag
   executed/static-presence/inference.
2. Comptages unit+opengl non nuls, zéro skip inexpliqué ; ASan C3 ; artifacts RED avant
   toute réécriture d'oracle.
3. Aucun verdict global : PASS/FAIL/INCONCLUSIVE par critère ; zéro test ou skip exigé
   ≠ PASS.
4. Registre sans doublon faible/orphelin non arbitré.
5. Artifacts out/mission-f7/ ; plan commité par le lead AVANT worktree T1–T3.

## Commandes runner (jamais au planner)

- `scripts/build-windows.ps1 [-TestLabel unit|opengl] -Jobs 1`
- `ctest --test-dir out/build/windows-msvc-debug -L unit|-L opengl --output-on-failure
  --no-tests=error`
- ASan : build `-AddressSanitizer` séparé, sélection déclarée.
