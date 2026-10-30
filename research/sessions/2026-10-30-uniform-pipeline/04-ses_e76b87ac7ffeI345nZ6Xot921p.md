# Borner mission de comparaison

Session: ses_e76b87ac7ffeI345nZ6Xot921p
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T20:09:22.910Z — msg_189480cda0013amPsDWfsxyuN3

## Avis de priorité

**Oui : évaluer le float préparé avant d’étendre vecteurs/matrices ou de comparer les pipelines.** C’est la tranche la plus courte pour répondre à une question utile : « produit-on la même image, et le coût de préparation est-il amorti ? » Ce choix reste un conseil, pas un lancement.

### Faits et écarts

- `docs/adr/README.md` et `docs/architecture/correction-plan.md` confirment : float livré, **aucune performance mesurée** ; ADR-0004 **Proposed/partial**, ADR-0006 **Proposed/not-started**.
- Le briefing source identifie dans `PreparedFloatUniformRenderTests.cpp` les tests `LegacyAndPreparedMatchFixedNonemptyPixels` et `PreparedUpdatesDoNotResolveNativeLocationsAgain`. L’équivalence bornée possède donc déjà un oracle ; ne pas présenter sa vérification comme une fonctionnalité nouvelle.
- Selon l’analyse Serena fournie, `ClosedExecution` appelle deux `IPass&.use()` sans dessin : une comparaison pipeline demanderait d’abord un consommateur graphique équivalent.
- Les 589 tests historiques prouvent leur périmètre de correctness, pas un gain CPU.

### Contrat proposé

**Une seule expérience OpenGL, float scalaire, deux chemins existants.**

- Même programme, mêmes valeurs, même miroir CPU, mêmes updates utiles et mêmes dessins ; seul change l’accès **par nom / préparé**.
- Réutiliser le consommateur graphique existant, avec un petit nombre fixé de volumes d’updates. Éviter les milliers d’écritures écrasées avant un unique dessin : elles amplifieraient artificiellement le mécanisme.
- Mesurer séparément :
  1. préparation additionnelle ;
  2. coût CPU par lot d’updates ;
  3. coût CPU de la séquence updates + dessins.
- Revalider l’oracle fixe hors des fenêtres chronométrées.

**Frontières :** types et sélection OpenGL à la compilation ; options d’optimisation/link identiques ; résolution nom/type **runtime après link du programme GL**, pas cook-time ; valeurs et soumission runtime. Aucun nouveau plan, graphe, type de pipeline ou objet driver. Deux chemins comparés, aucune nouvelle famille de variantes.

**Lifetime :** bindings empruntés révocables, owner conservé pendant leur usage ; aucune réutilisation après invalidation. Préserver les règles existantes de contexte/destruction OpenGL : les générations ne prouvent pas la fin d’utilisation GPU.

### Exclusions et risques

Pas de vecteurs/matrices, refonte, backend supplémentaire, framework général de benchmark ou scène 3D imposée.

Comparaisons trompeuses à prévenir : Debug/ASan contre Release, lookup différent du chemin réellement livré, travail supprimé côté préparé, readback ou attente GPU dans le chrono CPU, ordre A/B fixe, instrumentation intrusive. Aucun résultat llvmpipe ne devient une conclusion sur GPU matériel.

### Validation et décision

- Configuration optimisée qualifiée ; identité source/diff, compilateur, flags, driver et matériel conservés.
- Warm-up, répétitions alternées A/B, données brutes et dispersion ; seuil de pertinence fixé avant mesure.
- Amortissement calculé seulement si l’économie par lot est positive et discernable :  
  **surcoût de préparation / économie par lot**.
- **Résultat nul ou négatif = expérience réussie** si valide : conserver l’API par nom comme choix simple, sans généralisation forcée.
- Bruit ou comparaison non équivalente = **INCONCLUSIVE**, pas « aucun gain ».

Le lead pourrait adopter **localement les clauses utiles d’ADR-0006**, avec référence explicite au contrat de mission ; ni adoption globale ni qualification complète de l’ADR. L’extension vecteurs/matrices redeviendrait prioritaire si un consommateur concret la réclame, plutôt que pour prolonger une optimisation supposée.

**Limite :** conseil documentaire et briefing source ; aucune exécution, édition ou nouvelle mesure effectuée.