---
name: artist-cpp-contracts
description: Implémentation et revue C++23 bas niveau pour ARTIST. À utiliser pour templates, concepts, ownership, lifetime, mémoire, layouts, concurrence et chemins d'erreur.
---

# Contrats C++ bas niveau

Partir du diff, du contrat observable et des types réellement instanciés.
Explorer les symboles/références avec Serena si disponible ; confirmer les résultats
sur le code et la configuration de compilation.

## Invariants à expliciter

- **Propriété** : qui crée, possède, emprunte, déplace et détruit ? Un owner unique
  par défaut lorsque le domaine l'autorise ; partage uniquement s'il est réel.
- **Invalidation** : réallocation, destruction, reload, frame suivante, changement
  de génération, annulation d'upload. Un `span`/pointeur n'étend pas le lifetime.
- **Ressources GPU** : scope C++ et fin d'utilisation GPU sont distincts ; préciser
  contexte/thread, fence ou mécanisme de retraite approprié au backend.
- **Mémoire** : bornes, offsets, conversions signées, overflow, alignement,
  padding, aliasing et durée de vie des objets. `reinterpret_cast` n'est pas une
  validation de layout. Vérifier ce qui traverse l'ABI CPU/GPU.
- **Concurrence** : propriétaire des mutations, publication, ordres mémoire,
  contention et false sharing. Une preuve de lock-free n'est pas une preuve de
  progrès ou de performance. Tester le scénario concurrent réellement promis.
- **Erreurs** : état après échec partiel, nettoyage, exception safety, `noexcept`
  réellement tenu. Ne pas faire disparaître une erreur de shader dans un booléen
  ignoré ou un état « valide » par défaut.

## Métaprogrammation

Les concepts valident le contrat requis, pas seulement l'existence d'un nom.
Une opération obligatoire absente doit être rejetée ; une opération facultative
doit avoir une sémantique explicite. Tester les configurations invalides pertinentes
avec `static_assert`/requires ou une compilation négative ciblée.

Examiner visibilité des définitions, ODR, instanciations multiples et coût en TU.
`constexpr` n'impose pas une évaluation constante ; `consteval` n'est justifié que
si l'invariant doit exister avant l'exécution. Ni CRTP ni `always_inline` par rituel.
Ne pas imposer header-only à un nouveau sous-système pour reproduire l'existant.

## Validation

Charge `artist-build-validation`. Choisir tests de comportement, compilation
positive/négative, sanitizers ou instrumentation selon le risque. Identifier les
limitations de la toolchain : ASan/UBSan/TSan ne sont pas interchangeables et
ne démontrent pas l'absence d'erreurs de synchronisation GPU.

Un test doit discriminer un bug plausible : vide/zéro/max, erreur partielle,
déplacement, invalidation, opération hors ordre. Ne pas écrire un test qui se
contente de répéter le détail du mock. Conserver le style local et limiter le diff.

Retour : invariants, changements, preuve et incertitudes restantes.
