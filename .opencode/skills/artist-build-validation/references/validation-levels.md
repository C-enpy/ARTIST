# Validation commune : oracle, candidat, portée

Référence partagée par build, assets, shaders et reviewers. Ni runner nouveau,
ni API GPU, ni permission d'exécuter implicite. Préparer la preuve minimale qui
répond au critère ; ne pas lancer tous les niveaux par défaut.

## Préconditions et identité

1. Lire plan/critère, invariant et intent ; définir nominal/négatif AVANT code.
   TDD comportemental : RED causal puis GREEN et non-régression. Docs/configs :
   contrôles de structure/cohérence adaptés, pas échec d'environnement inventé.
2. Relever candidat HEAD, delta (staged/unstaged/untracked utile), hashes des
   entrées, sources/shaders/assets/outils/options et binaires produits. Associer
   chaque commande à son cwd/build-dir, versions, configuration et heure UTC.
3. Vérifier fraîcheur des outputs, configuration, fixture, toolchain et device.
   Un binaire plus ancien ou une donnée QMD non à jour ne certifie pas le diff.
   Modification après run → preuve périmée pour le nouveau candidat ; ne pas
   mélanger logs d'un HEAD et hashes d'un autre. Reprendre les tests impactés.
4. Vérifier disponibilité ET autorisation : toolchain fonctionnelle pour négatif
   de compilation, contexte/device pour GPU, outils/counters pour performance.
   Aucun SDK/driver/outil installé implicitement ; ressource partagée sous bail.

## Niveaux indépendants

Cinq niveaux distincts : CPU, offline, GPU, image, performance.

- **CPU / compilation** : concepts/types positifs et négatifs avec diagnostic
  attendu, tests comportementaux d'import/offsets/erreurs/ownership CPU. Outils
  de compilation réellement fonctionnels ; une absence d'include n'est pas le
  RED attendu d'un concept. Un mock établit un contrat CPU, pas le driver.
- **Offline** : importer/cooker/décodeur, compilation shader intermédiaire et
  validation de module/reflection/ABI sans device. Nominal : format valide,
  layout et hashes attendus ; négatif : tronqué/overflow/ABI invalide/version
  non supportée. SPIR-V/DXIL/MSL produit ne prouve pas compilation finale,
  link/pipeline, bindings ni consommation GPU. MSL généré Windows ≠ exécution Apple.
- **GPU réel** : contexte/API/version/profil, extensions/features actives,
  device/driver, formats, buffers/bindings, errors/layers et synchronisation.
  Nominal : exécuter et observer sorties ; négatif : erreur ciblée reproduite
  au bon niveau. Renderer logiciel indiqué séparément, jamais substitution
  silencieuse à un matériel demandé. Validation layers propres ≠ image correcte.
- **Image** : golden indépendant/versionné et provenance, mêmes caméra/scène/
  résolution/seed/exposition, espaces et colorimétrie ; métrique et tolérance
  justifiées avant run, comparaison de pixels/regions et inspection si requise.
  Négatif : altération visible ciblée détectée. Ne pas régénérer le golden du
  candidat pour masquer un défaut ; capture seule ≠ oracle de qualité.
- **Performance** : `artist-performance`, A/B à qualité/workload/options égaux,
  répétitions/dispersion, données brutes, coûts total/déplacés et budget préalable.
  Négatif : travail différent/bruit/capture seule → pas gain prouvé. Debug ou
  sanitizer n'est pas automatiquement représentatif d'une livraison optimisée.

Compiler ne prouve ni image ni performance ; données cookées ≠ consommation GPU.
Une preuve partielle peut PASS pour son niveau pendant que le critère global
reste INCONCLUSIVE. Une erreur reproduite n'est pas effacée par ce blocage.

## Comptages, résultat et cas discriminants

Rapport par critère : obligation, oracle attendu/observé, verdict et portée,
préconditions/outils, commande exacte, environnement, candidat HEAD/delta/hashes,
artefacts et première erreur utile. Compter **découverts**, sélectionnés,
**exécutés**, **échoués**, **skipped** ; distinguer filtrés/non exécutés. Recouper
CTest discovery et logs/JUnit, pas seulement exit 0. Doublons/noms manquants,
suite vide ou sorties anciennes doivent être signalés. Réexécution ne réutilise
pas silencieusement un succès antérieur et ne duplique pas les preuves.

Qualifier `evidenceKind` : `executed` pour action réellement observée,
`static-presence` pour texte/structure seulement, `inference` pour déduction.
Un regex test exécuté donne une preuve exécutée du contrôle textuel, pas une
preuve native de sélection du profil ni d'enforcement. Ne pas ré-étiqueter une
trace simulée/fixture comme observation runtime. Zéro contrôle n'est pas PASS.

- **V-01** : CPU réussi, GPU requis indisponible → PASS CPU, INCONCLUSIVE GPU.
- **V-02** : zéro test découvert/exécuté → INCONCLUSIVE, rechercher filtre/cwd/
  configuration ; `--no-tests=error` évite exit 0 trompeur, pas preuve GPU.
- **V-03** : skip requis / NOT_AVAILABLE → INCONCLUSIVE pour ce critère. Documenter
  motif et portée ; un skip hors contrat ne retire pas une autre preuve acquise.
- **V-04** : ancien HEAD, mauvais hash ou modification après run → preuve périmée,
  INCONCLUSIVE sur candidat courant ; conserver les preuves historiques datées.
- **V-05** : échec reproduit + précondition manquante → FAIL avec blocage conservé,
  pas remplacement de l'échec par INCONCLUSIVE ou retrait du test rouge.
- **V-06** : oracle CPU satisfait, candidat frais, un test exécuté, zéro skip →
  PASS CPU seulement, aucun PASS GPU/image/performance implicite.

**PASS** : critère démontré ; **FAIL** : écart reproduit ; **INCONCLUSIVE** :
précondition ou preuve manquante. Conserver baselines datées sans les promouvoir
en preuve nouvelle. L'historique Windows ne valide pas autre OS, Release ou CI.

## Sources, limites et handoff

Rédaction originale, liens officiels seulement :
- [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) : discovery,
  sélection et `--no-tests=error` selon version réellement installée.
- [Conan 2 CMakeToolchain](https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html) : chaîne CMake ; versions locales et options à relever.
- [OpenCode V2 agents](https://opencode.ai/v2/docs/agents) et
  [skills](https://opencode.ai/v2/docs/skills) : formats/discovery natifs, pas
  déduction d'enforcement depuis un frontmatter. Le chemin détermine l'ID ;
  `name` est un libellé, son égalité avec l'ID est une convention locale.

Aucune copie/adaptation tierce ici ; vérifier licence, notices, version/commit
exact avant réutilisation, licence inconnue = lien seulement. Aucun exécutable
inventé à partir de ces liens. Absence d'outil/device peut être un verdict honnête.
`DEFER_TO` au lead pour runner autorisé, preuve indépendante ou arbitrage de
précondition. Ni transfert de droits ni sous-délégation pour obtenir un PASS.
