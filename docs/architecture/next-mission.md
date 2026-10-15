---
title: Prochaine mission — diagnostics GL vides et nettoyage sûr
category: architecture
tags: [artist, ownership, cpp, testing]
summary: Sécuriser les chemins d'échec compilation/link lorsque le diagnostic GL est vide, sans masquer l'erreur ni empêcher le nettoyage natif.
status: selected
implementation: not-started
source_revision: e589aeece2c5abc46aedb089c6a88a6f7dbaa239
source_diff_fingerprint: a1e66dd728c8e6b0700cf7db694040c9592ce18acaf53145590ad0e85053c234
created: 2026-10-15
updated: 2026-10-15
sources: [docs/development/known-incidents.md, docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, docs/_meta/next-mission-2026-10-15-selection.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Prochaine mission : un diagnostic GL vide ne doit pas casser le chemin d'erreur

**Sélectionnée à la demande de l'utilisateur, pas commencée.** Cette fiche prépare
F5 ; elle ne lance ni auteur, ni build, ni tests. Aucune tâche d'implémentation
n'est publiée. Le contrat précédent est conservé dans
[[architecture/missions/2026-10-15-loaded-owner-move|l'archive loaded-owner move]].

## Pourquoi cette mission avant Pipeline

Les deux loaders peuvent construire un vecteur vide pour `GL_INFO_LOG_LENGTH=0`,
utiliser `&infoLog[0]`, puis interpréter `data()` comme chaîne C. Une erreur
secondaire peut ainsi masquer l'erreur GL et empêcher le cleanup prévu après
le retour `false`. **Risque confirmé dans le source ; aucun RED exécuté.**

PM et architecte recommandent F5 seule. Pipeline demande une coupe cohérente
contexte/wrappers, copies/moves, emprunts, bornes/curseur et fixtures ; changer
uniquement son conteneur ne suffit pas. F4 (bornes), F8 (fichier absent), Q1
(environnement ASan), bindings et transaction reload restent hors mission.

La tranche applique les chemins d'erreur/lifetime de
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003 Accepted/partial]]. ADR-0005 reste
Accepted/partial et ADR-0008 vérifié dans sa baseline Windows Debug. Les ADR-0002,
0004 et 0006 restent Proposed, sans adoption implicite.

## Base, périmètre et invariants

- HEAD `e589aeece2c5abc46aedb089c6a88a6f7dbaa239` + D `0bbca504` et H `a423fc25`
  intégrés non commités. L'empreinte en frontmatter porte sur les 15 entrées
  source/build de la base, pas sur une correction F5.
- Baseline antérieure : 137/137 normaux (106 CPU + 31 GL), ASan 106 CPU + 9 GL
  lifetime = 115 uniques. [[references/session-2026-10-15-loaded-owner-move|Preuves et limites]].
  Aucun test relancé pour cette sélection ; préserver le diff utilisateur.
- D-F5 possède `ARTIST/include/graphic/opengl/pipeline/component/{shader,pass}/Loader.hpp`
  et les deux `ARTIST/tests/src/graphic/opengl/pipeline/component/{shader,pass}/LoaderTests.cpp`.
- Préserver les erreurs publiques `ERROR::SHADER::COMPILATION_FAILED` et
  `ERROR::SHADER::PROGRAM::LINK_FAILED`, leur nature et leur distinction. Un log
  absent n'est jamais un succès ; aucune lecture de chaîne non bornée.
- Après l'échec GL testé, supprimer exactement une fois le shader/programme
  acquis, sans handle libéré conservé. Ne pas libérer implicitement les shaders
  possédés par le Pass à cause d'un échec de link.
- Préserver succès, rejet du reload vivant et contrat loaded-owner move. Pas de
  changement d'ownership, de `noexcept`, de profil, de dépendance, de lecture de
  fichiers, d'API Pipeline ni de framework de diagnostic général.
- Profils/contrats compile-time inchangés ; aucune nouvelle résolution link/cook.
  Diagnostic en données runtime sur chemin d'erreur. Contexte GL compatible courant
  pour les opérations natives, aucune attente GPU globale ni performance revendiquée.

## Critères observables

1. **RED causal avant fix** sur log zéro des deux loaders. Couvrir aussi longueur
   un (terminateur seul) et message normal, sans fabriquer un échec pour ces cas
   déjà corrects. Si la STL Debug termine le processus, isoler le cas et conserver
   le diagnostic causal ; panne d'environnement/timeout non expliqué ≠ RED.
2. **GREEN minimal** : diagnostic sûr, exception attendue, cleanup unique et handle
   cohérent. Répéter cycle/free pour détecter une double suppression. Ne pas
   affaiblir assertions/mocks ou contourner le cas vide.
3. REFACTOR puis suites normales CPU/GL complètes, sérialisées, JUnit neufs et
   nombres réels (137 existants plus ajouts, pas de total futur présumé).
4. ASan séparé : instrumentation prouvée, nouveaux cas zéro/un/message normal et
   suite CPU complète, puis les 9 GL lifetime existants ciblés. Options moteur et
   canari fail-closed consignées, pas de `log_path` permissif. Pas d'extension
   implicite aux autres GL sous ASan ; Q1 du script reste non corrigé.
5. Revue indépendante des buffers, longueurs/terminaison, cleanup et preuves RED.
   Le driver peut fournir un log non vide : le zéro est déterministe dans les
   mocks ; tests driver et sanitizer restent des preuves distinctes. Ni ASan ni
   cette tranche ne prouvent une absence exhaustive de fuite driver/RAII défaillante.

## Graphe et fiches prévisionnelles — non publiés

**B0 → D-F5 → I0 → (R ∥ V) → DOC → I1**. Références locales, pas IDs Ensemble.
PM/LLM, deux scores entiers distincts, heuristiques et non probabilités.

- **D-F5, `gpu-engineer`** : brut C64 → retenu **70**, R74 ; développement TDD,
  propriétaire unique des quatre fichiers et du cycle RED/GREEN/REFACTOR. Entrée B0.
  `fresh`, `worktree:true` ; skills GPU/C++/build ; un build/arbre, un job hôte.
- **R, `correctness-reviewer`** : C49/R66, lecture seule du diff I0 et des preuves,
  aucun test source écrit ni build/GPU. `fresh`, `worktree:false`, après I0.
- **V, `rendering-verifier`** : C58/R65, QA sans code nouveau : build/découverte,
  suites normales complètes puis ASan CPU et GL ciblés ci-dessus. Sorties propres
  sous `out/`, baux compilation/mémoire/GPU sérialisés. `fresh`, `worktree:false`,
  après I0, en parallèle à R ; défaut renvoyé à l'auteur, pas corrigé par la QA.
- **DOC, `build-engineer` limité aux notes/manifeste** : C27/R32, après R/V acceptés,
  fermer seulement F5 prouvé et actualiser incidents/plan/référence/preuves/journal.
  `fresh`, `worktree:true` avec entrées disponibles ; sinon action documentaire
  locale du coordinateur sous contrat explicite. Aucun script exécutable nouveau.

Barrières du coordinateur, pas workers factices :

- **B0** : vérifier HEAD/diff/préconditions et réserver ressources ; transporter
  réellement D+H et les fichiers non suivis requis dans le worktree auteur, puis
  contrôler les empreintes. Aucun transfert implicite depuis le principal, aucun
  commit global des edits utilisateur. Résoudre ce transport avant publication.
- **I0** : shutdown/intégration auteur, contrôle du périmètre, identité source figée
  avant publication R/V avec le vrai ID du producteur. Fin auteur ≠ intégration.
- **I1** : intégrer DOC, contrôler le diff puis actualiser mémoires et index QMD
  du principal avec un seul indexeur ; pas de mutation QMD pendant les tests GPU.

Un seul auteur évite un micro-découpage des deux chemins homologues ; R/V sont
indépendants après I0. Pas de chantier harness requis. Toute écriture de code/test/
harness supplémentaire réévalue le contrat et le plancher développement à 70.
Commandes possibles : scripts Windows/Conan existants, CTest ciblé puis suites et
ASan du [[development/build-and-test|guide]], avec logs/JUnit inédits.

## Autorité et limites de la sélection

PM `ses_ec0352fbcffeI9COZTIb08MXH7`, avis révisé F5 seule après l'architecte
`ses_ec0352fa0ffej7oEkfSQ7dBmQ5` ; consultations en lecture seule. Le coordinateur
valide ce choix et les scores sans publier les tâches. Les propositions Pipeline
ne constituent ni un contrat adopté ni une implémentation.

À l'exécution seulement : activer l'exigence de contexte avant publication,
transmettre les deux scores entiers et conserver le routage opérateur ; préférence
utilisateur OpenCode Go puis OpenAI, sans modèle/effort forcé.

La prochaine exécution devra franchir B0 et obtenir ses propres RED/GREEN/QA.
F5 est **sélectionné, pas corrigé**. Conserver preuves historiques et statut
partiel de l'ADR ; aucun PASS global ne résulte de cette sélection.

Navigation : [[architecture/correction-plan|Plan]] · [[development/known-incidents|Incidents]] ·
[[architecture/missions/2026-10-15-loaded-owner-move|Mission précédente archivée]].
