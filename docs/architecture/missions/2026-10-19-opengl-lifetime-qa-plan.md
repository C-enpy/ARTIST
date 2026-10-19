---
title: "Plan QA — lifetime OpenGL 2026-10-19"
category: architecture
tags: [artist, architecture, testing, qa]
aliases: [opengl-lifetime-qa-plan]
summary: "Plan QA de la mission lifetime OpenGL : exigences, registre preuves↔obligations K1–K5, top 3 risques I×P, budget et décisions lead."
status: adopted-mission
implementation: not-started
source_revision: cd1b598c77bc22ae876543bea6e0aae104792649
created: 2026-10-19
updated: 2026-10-19
sources:
  - "opencode:qa-planner-pool-3-2026-10-19"
  - "architecture/missions/2026-10-19-opengl-lifetime-contracts.md"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Plan QA — lifetime OpenGL 2026-10-19

> [!important] Barrière
> Plan préparé en lecture seule par `qa-planner`, persisté et committé par le
> lead **avant tout worktree auteur**. Les contrats de
> [[architecture/missions/2026-10-19-opengl-lifetime-contracts|K]] sont la
> référence, non redéfinis. Statuts cités : *static-presence* sauf baseline =
> *executed*.

## 1) Exigences

### Explicites (contrats/gate) — invariants

- **K1 VBO (A)** : ledger `Empty/Owned/Retired` + `MovedFrom` ; `setBufferID`
  adoption exclusive (même ID no-op, ID diff = prévalidation puis release
  ancien sous bon contexte avant adoption, 0 = release) ; `getBufferID` =
  emprunt sans droit de suppression ; free idempotent, no-op moved-from, garde
  CPU, ID→0, bind recrée, set sans buffer = `BUFFER_ID_NOT_SET` ; `glGenBuffers`
  init 0, retour 0 = `BUFFER_CREATION_FAILED` sans publication, rebind conserve
  le nom ; dtor `Owned` sans contexte exact = fail-fast zéro appel GL ; rejets
  sans mutation ; move sans copie CPU, self-move no-op ; exact-once = une
  émission `glDeleteBuffers` par ownership nonzero. *Intent : un owner natif
  unique, un delete par nom, jamais d'appel GL hors contexte.*
- **K2 réflexion (B)** : staging local par Reader, succès = remplace toute la
  map + retire noms disparus ; count 0 = commit vide, count<0 =
  `INVALID_COUNT` sans mutation ; échec tardif = zéro préfixe, borrows
  invalidés au commit seulement ; pas de transaction Pass ; prévalidation hook
  A avant swap, retrait anciens owners sous bon contexte, zéro `glDelete`
  direct PassContext. *Intent : frontière de publication claire, pas de
  demi-état visible.*
- **K3 Uniform (C)** : refus déterministes avant `setValue` (ancienne
  valeur+type ou `UNINITIALIZED`, zéro `glUniform*`) ; cardinalité 1, size 1
  préservé ; 34 familles ; matrice GLSL sans implicites float↔int/uint↔int/
  float↔double, double≠bool, samplers via GLint scalaire ; programme courant
  nonzero + égalité ID avant upload ; chemin manuel `glType0`/`programID0`
  préservé ; fp64 conditionnel, indisponible = refus avant mutation, skip
  natif = INCONCLUSIVE attribué. *Intent : zéro upload partiel, état CPU
  jamais mensonger.*
- **K4** : séquencement Q→M?→A‖C‖H→B→I→G‖R→F→V ; ownership fichiers par
  branche ; oracles RED par branche (référence).
- **K5** : baseline 326/326 sur `cd1b598`, 0 skip ; FAIL frontal traité/
  arbitré, jamais absorbé ; audit final 12 formes = obligation QA persistante
  (V).

### Inférées (à valider en cours)

Re-run non-régression sur candidat final avec delta HEAD documenté ;
découverte non nulle + nouveaux tests présents après reconfig ; sérialisation
bail GPU/fixtures ; migration cleanups legacy sans double suppression ;
**constat** : `INVALID_COUNT` absent des deux Readers aujourd'hui
(early-return silencieux) — écart observable.

## 2) Registre bidirectionnel (obligation → preuve cible → statut)

Les obligations de changement sont des GAP par construction
(implémentation not-started).

- **K1 → oracles K4-A** : exact-once, move chargé→chargé, self-move, rebind,
  free×2, gen 0 (CPU mock) ; death test dtor sans contexte distinguant
  fail-fast vs appel GL interdit ; rejet mauvais contexte conserve l'owner
  sans GL (compteur mock) ; upload + teardown natif (GPU réel, fixtures
  existantes). Preuve existante : **AUCUNE** pour le cycle VBO ;
  `Binder.hpp:27-33` (`glGenBuffers` sans contrôle retour, aucun
  `glDeleteBuffers` du dépôt) = écart constaté. Précondition : mock
  `glDeleteBuffers` + compteur absents de `tests/mock/opengl/glFunctionMock.hpp`
  (**M**).
- **K1-migration** : cleanups manuels des tests « Loaded » (int présent ≠ VBO
  chargé) à migrer sans double suppression → re-run tests composants attribute
  après A.
- **K2 → oracles K4-B** : première entrée valide puis seconde invalide avec
  ancienne map identifiable = aucun préfixe ; succès efface disparus ; vide
  efface ; remplacement VBO = delete 1 ; Uniform succès/Attribute échec sans
  transaction. Preuves existantes **PÉRIMÉES** vs contrat (amendements K2 sans
  affaiblissement) : `UniformReaderTests.cpp:174` (actuel : reloaded size 3
  publié, ligne 195 ; K2 : ancien size 2/pointee conservé, pas 3) ;
  `UniformReaderTests.cpp:139` et `AttributeReaderTests.cpp:131` (actuels :
  préfixe size 1 publié après DuplicateName sur map vide ; K2 : map vide reste
  vide — attente inversée — plus cas map préremplie identique à ajouter).
- **K2 GAP sans preuve existante** : retrait des noms disparus au succès ;
  borrow à travers un second read réussi ; `INVALID_COUNT` count<0 (deux
  Readers) ; commit vide sur map préremplie (`ReadUniforms_EmptyProgramAddsNothing`
  part de map vide, ne discrimine pas).
- **K3 → oracles K4-C via API publique** : rejets taille/type/programme/fp64 →
  ancien état, zéro `glUniform*` ; 34 familles + bool/sampler contre faux garde
  enum==enum ; deux programmes même location ; defaults/size1. Natif :
  readback cible+decoy. Écart constaté : `Uniform.hpp:93-94` stocke avant
  `uniform/Setter.hpp:21-30` `requireSingleValue` (store-then-reject) ;
  aucune garde programme/ID/fp64 aujourd'hui.
- **K5 → COVERED (executed)** sur `cd1b598` uniquement ; preuve périmée dès
  mutation, re-run par critère au candidat final.
- **Orphelins** (preuve sans obligation K, non bloquants) :
  `ReadAttributes_CallGetProgramiv` (fumée, doublon faible) ;
  `FaultInjection_ExactArraySuffixAndUnsupportedEnumDescription` (frontière
  suffixe `[0]` à NE PAS importer côté Uniform) ; `LoadPassTest_*` (hors
  contrats K, couverts par non-régression H).

## 3) Top 3 risques (QA I×P ; scores PM publiés inchangés)

- **R1 = 20 critique (I5×P4)** — double-libération/exact-once violé.
  Déclencheur : remplacement d'un Attribute owner-VBO par B pendant qu'un
  cleanup manuel legacy subsiste, ou adoption ID différent + delete résiduel.
  Blast radius : double `glDeleteBuffers` (UB driver), état GL corrompu pour
  le bail GPU partagé, crash teardown. Détection actuelle : aucune.
  Mitigation : oracle exact-once K4-A, prévalidation hook A avant swap,
  séquencement A avant B. Gap : extension M absente = précondition de
  l'oracle.
- **R2 = 12 haut (I4×P3)** — fail-fast dtor mal discriminé. Déclencheur : dtor
  `Owned` sans contexte exact courant. Blast radius : appel GL hors contexte =
  UB, crash non diagnostiqué de la suite ; faux vert si l'oracle confond crash
  GL et diagnostic attendu. Détection : aucune (dtors default aujourd'hui).
  Mitigation : preuve dédiée + compteur GL=0 avant diagnostic. Gap : modalité
  d'isolation du death test dans la suite/bail GPU non écrite.
- **R3 = 12 haut (I4×P3)** — borrow à travers commit réussi. Déclencheur :
  pointeur `getUniform`/`getAttribute` conservé puis second read réussi (map
  remplacée, `unique_ptr` détruits). Blast radius : use-after-free CPU chez
  les consumers. Détection : aucun test existant ne pose un borrow avant un
  read réussi. Mitigation : oracle K4-B + égalité de pointee. Gap : preuve à
  créer.
- Secondaires <10 : shuffle H I4×P2=8 (masquage fixture) ; fp64 skip natif
  I2×P3=6 ; `INVALID_COUNT` I3×P2=6.

## 4) Budget (h-p ; scores PM non modifiés)

Setup 7 (fourchette 5–9) : fixtures existantes réutilisées, reconfig CMake
après ajout de sources, bail GPU sérialisé, extension M si go. Planifié 40 /
buffer 13 ≈ 75/25 (travail 32–50, buffer 11–17) : A 10, B 9, C 8, H 4, I 5,
ré-audit/corrections 4. Buffer nommé : FAIL frontal, ré-audit post-mutation,
arbitrage précondition GPU. Irréalisable si : M non activé alors que l'oracle
exact-once est exigé, ou death test non isolable. Coupe par risque×effort :
R1/R2 avant confort ; H jamais élargie sans cause reproduite.

## 5) Sorties attendues et décisions

Sorties : plan commité avant tout auteur ; verdict par obligation au candidat
final (COVERED preuve qualifiée discriminante / GAP / GAP-accept / GAP-defer
avec propriétaire+échéance / blocked avec précondition exacte) ; rapport par
critère : HEAD, delta, hashes, commandes, environnement, comptages
découverts/exécutés/échoués/skips, attendu/observé ; audit 12 formes (V
fresh) ; aucun PASS global tant qu'un critère requis est FAIL/INCONCLUSIVE ;
zéro test = INCONCLUSIVE ; MOCK_GL ne prouve pas le GPU.

### Décisions lead (addendum 2026-10-19, dans le cadre ratifié — veto possible en chat)

1. **M activé** : les oracles exact-once et zéro-upload l'exigent
   (préconditions nommées). Portée gelée pool-2 : `glDeleteBuffers`, contexte
   courant, programme courant, capacités — compteurs stricts, aucun défaut qui
   masque une précondition. M reste gated (strong) → paire preflight→fork.
2. **Readback natif C = échantillon** (cible+decoy), conforme à K4-C et au
   retrait des sondes de conformité driver (`f7bb171`). Pas de balayage 34
   familles natif.
3. **Death test** : gtest death tests peu fiables sous MSVC — l'oracle minimal
   est le **compteur mock (zéro appel GL après dtor Owned sans contexte)** +
   observabilité du diagnostic ; la forme exacte du fail-fast est tranchée par
   l'auteur de A sous la garantie ratifiée, preuve sur binaire unit isolé.
4. **Chemin du plan** : `docs/architecture/missions/2026-10-19-opengl-lifetime-qa-plan.md` (celui-ci).
5. **Périmètre B** : B possède aussi le thin `opengl/context/PassContext.hpp`
   si le hook A y est appelé ; un seul écrivain par fichier, gel dans le brief
   de B à la publication (après intégration A+C).
6. **Bornes H** : seeds historiques + une permutation inversée + un contrôle
   négatif ; élargissement seulement sur cause reproduite.
7. **Lacune persistante assumée** : pas de stratégie écrite des niveaux de
   mocks dans ce plan ; le brief de M fixe les niveaux minimaux (compteurs
   stricts, enregistrement sans falsification de sémantique).
