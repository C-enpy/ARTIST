---
title: Prochaine mission — assignation move et validation lifetime
category: architecture
tags: [artist, ownership, cpp, testing]
summary: Corriger l'assignation par déplacement vers un Shader/Pass chargé, prouver le nettoyage natif, puis mener l'expérience ASan séparée.
status: selected
implementation: not-started
source_revision: 4d298425d7c0ecd87101409eb9ce78bac9b855de
created: 2026-10-15
updated: 2026-10-15
---

# Prochaine mission : assignation move d'un owner chargé

**Sélectionnée, pas commencée.** Fermer ce risque de lifetime avant d'étendre la
migration au Pipeline. Cette mission applique [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
(Accepted/partial), pas une adoption des ADR-0002/0004/0006 encore Proposed.

## État de départ et objectif

Le code/build est committé en `4d29842`. Vérification du 2026-10-15 : build Debug
MSVC réussi, **95 unit + 27 OpenGL = 122/122**, sans échec ni skip.
Preuves : `_meta/precommit-2026-10-15-validation.json`. À la reprise, vérifier
HEAD/diff : ce contrat n'autorise pas à écraser d'autres modifications.

Observation déjà établie : `IShader::operator=(IShader&&)` et
`IPass::operator=(IPass&&)` remplacent `m_context` sans libération native explicite
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
