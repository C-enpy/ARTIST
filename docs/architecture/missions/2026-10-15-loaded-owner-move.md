---
title: Mission terminée — assignation move et validation lifetime
category: architecture
tags: [artist, ownership, cpp, testing]
summary: Assignation move Shader/Pass corrigée et vérifiée sous Windows Debug ; suites normales 137/137 et ASan 106 CPU plus 9 lifetime OpenGL.
status: completed
implementation: verified
source_revision: e589aeece2c5abc46aedb089c6a88a6f7dbaa239
source_diff_fingerprint: a1e66dd728c8e6b0700cf7db694040c9592ce18acaf53145590ad0e85053c234
created: 2026-10-15
updated: 2026-10-15
---

# Mission terminée : assignation move d'un owner chargé

**Terminée dans le périmètre Windows x64 Debug le 2026-10-15.** Le risque reproduit
est corrigé avant toute extension au Pipeline. Cette mission applique [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
(Accepted/partial), pas une adoption des ADR-0002/0004/0006 encore Proposed.

## Résultat vérifié

- RED CPU et driver observé avant correction ; cleanup avant transfert, bases
  non publiquement assignables, owners concrets `final`, assignation non-`noexcept`.
- Validation indépendante du diff intégré : **137/137** (106 unit + 31 OpenGL),
  dont 34 lifetime ; aucun échec ni skip. Revue indépendante : PASS de lecture.
- ASan séparé : **106/106 CPU + 9/9 lifetime OpenGL**, 115 cas uniques, 36 TU
  instrumentées. Les 22 autres tests GL ne sont pas validés sous ASan.
- Preuves, commandes, réserve d'environnement ASan et limites :
  [[references/session-2026-10-15-loaded-owner-move|synthèse de mission]] et
  `_meta/session-2026-10-15-loaded-owner-move-validation.json`.

HEAD principal reste `e589aee`, avec les livrables D `0bbca504` et H `a423fc25`
intégrés dans le diff non commité ; l'empreinte ci-dessus porte sur 15 fichiers
source/build. Les modifications utilisateur n'ont pas été commitées pour la QA.
Aucune nouvelle mission n'est sélectionnée ici : ne pas démarrer arbitrairement
Pipeline ou un autre finding. Les sections suivantes conservent le contrat initial.

## État de départ et objectif initial

Le code/build est committé en `4d29842`. Vérification du 2026-10-15 : build Debug
MSVC réussi, **95 unit + 27 OpenGL = 122/122**, sans échec ni skip.
Preuves : `_meta/precommit-2026-10-15-validation.json`. À la reprise, vérifier
HEAD/diff : ce contrat n'autorise pas à écraser d'autres modifications.

Observation initiale reproduite : `IShader::operator=(IShader&&)` et
`IPass::operator=(IPass&&)` remplaçaient `m_context` sans libération native explicite
de l'ancienne destination ; les opérateurs dérivés sont defaulted. La construction
par déplacement testée ne prouve pas cette assignation. Objectif : reproduire puis
corriger la perte du handle destination, sans double destruction ni purecall.

## Périmètre et invariants

- Headers : `ARTIST/include/graphic/pipeline/{Shader,Pass}.hpp` ; composants
  OpenGL `shader/Freer.hpp` et `pass/Freer.hpp` seulement si la correction le requiert.
- Tests : `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp` et
  `ARTIST/component-tests/src/graphic/shader/ShaderLifetimeComponentTests.cpp`.
- La destination libère exactement une fois son ancien handle ; la source cède
  le sien, devient moved-from et ne le détruit plus. Préserver les règles de
  destruction programme/shaders et définir explicitement self-move et états vides.
- Les handles sont du runtime ; ne pas ajouter un booléen « loaded » concurrent
  du handle ni encoder l'état vivant dans un type C++ pour contourner ce problème.
- Contexte GL valide et courant pendant cleanup ; pas de synchronisation GPU
  globale ajoutée. Ne pas réintroduire d'appel virtuel pur depuis une base détruite.
- Le contexte de `IPass` reste partagé dans cette tranche. Couvrir ou qualifier
  le cas où un consommateur en retient une copie, sans refonte Pipeline/Uniform.
- Examiner le contrat `noexcept` avec les composants réellement appelés ; ne pas
  le conserver ou le supprimer sans preuve. Une frontière polymorphique ouverte
  exige un avis `engine-architect` avant le contrat d'implémentation.

## Preuves de réussite

1. Test régressif échouant avant fix : source et destination chargées, assignation
   move, suppression de l'ancien handle destination puis du transféré une fois.
2. Cas destination vide, source moved-from et self-move explicitement testés.
   Les traits `is_move_assignable`/`noexcept` seuls ne suffisent pas.
3. Répéter le cycle sur contexte OpenGL réel ; vérifier la libération selon les
   règles GL sans supposer que les IDs ne seront jamais réutilisés.
4. Rejouer les suites complètes, sérialisées, avec nouveaux JUnit et nombres réels.
   Revue indépendante `correctness-reviewer` sur le diff final et les échecs partiels.
5. Expérience **ASan séparée**, instrumentation effectivement démontrée, puis
   tests ciblés et suite CPU applicable. ASan ne détecte pas à lui seul les fuites
   d'objets driver : garder les preuves GL distinctes. Signaler toute limite de
   dépendances non instrumentées et tout résultat INCONCLUSIVE.

## Exécution et liberté locale

Implémentation C++ à `cpp-engineer` si la spécialisation aide ; `build-engineer`
pour un blocage de l'expérience ASan. Workers feuilles, un écrivain par fichier,
un build par arbre. Charger `artist-cpp-contracts` et `artist-build-validation`.
ASan reste opt-in, dans un arbre séparé ; les flags peuvent dépendre du compilateur,
mais l'API moteur ne devient pas Windows-only. Aucun shim de compatibilité requis.

Préserver les preuves du 2026-10-03 et du 2026-10-15 avec de nouveaux noms de logs.
Les autres findings (logs GL vides, bornes, fichier manquant, transaction de reload)
restent suivis séparément dans [[development/known-incidents|le registre]] ; pas
de nettoyage opportuniste. Après correction vérifiée, actualiser ADR-0003, état
réel, mémoires Serena et index QMD. Aucun gain performance revendiqué ici.

Navigation : [[architecture/correction-plan|Plan]] · [[development/build-and-test|Build]] ·
[[references/session-2026-10-03-shader-pass|Historique et tentatives antérieures]].
