# 13 — Premier contrôle pixel rouge puis vert, avant livraison finale

## Objectif et résultat de cette étape

La tranche doit montrer que l'accès préparé déplace réellement le triangle comme
l'ancienne interface, pas seulement qu'un appel OpenGL réussit. Le lead a recoupé
les premiers artefacts de la branche auteur : la suppression du second update
fait échouer les pixels attendus ; son raccord minimal rend les tests ciblés verts.
La livraison n'est pas encore intégrée. Aucun nouveau GO n'est nécessaire.

## Preuves recoupées

- Avant modification : 502 tests CPU et le consommateur historique 1/1 passants,
  aucun failure/error/skip dans leurs JUnit.
- RED : un test pixel exécuté, un échec, CTest exit 8. À la seconde image, (8,8)
  reste blanc au lieu du fond et (24,24) reste fond au lieu du blanc ; miroir CPU
  et lecture native restent à 0 au lieu de 0.5. Le test détecte aussi l'égalité
  indue des images avant/après. Source rouge conservée avec les artefacts.
- GREEN ciblé : trois tests exécutés et passants, CTest exit 0, zéro skip :
  pixels legacy/préparés, absence de nouvelle résolution native, consommateur
  avec rejet de mauvais programme et reprise.
- Le journal d'observation distingue setup 5, calibration 1, préparation 0,
  updates 0 pour la résolution native. Les recherches CPU par nom et extractions
  effacées restent couvertes séparément par le producteur ; ce compteur seul ne
  prouve pas leur absence ni une absence d'allocation.

Ces commandes ont été exécutées par l'auteur gpu-engineer ; le lead a seulement
copié et analysé leurs preuves, sans run concurrent ni modification de sa branche.
L'identité finale, les suites complètes et ASan restent à recouper à la livraison.
Le GPU annoncé par l'auteur est RTX 3090, driver 621.01, contexte 3.3 core ; cette
identification reste une déclaration tant que son journal final n'est pas recoupé.

## Portée exacte du RED et suite

L'omission volontaire démontre la sensibilité du nouvel oracle et le raccord
manquant du consommateur en construction. Elle ne démontre pas un défaut de
l'API déjà intégrée et ne reconstitue pas son historique TDD. Les attendus fixes
du plan QA ne sont pas adaptés aux images. L'auteur poursuit le contrôle tout
noir, la restauration du candidat, les non-régressions et ASan sous bail exclusif.
Les revues et le complément des gaps attendent son intégration effective.

## Preuves techniques

- Copies lead : `out/qa-prepared-float/consumer-red-green-snapshot/`, neuf fichiers
  figés sans overwrite, issus de `out/logs/prepared-float-consumer/` dans le worktree.
- RED : `06-red-pixels.command.json`, `.junit.xml`, `.log` et source
  `05-red-render-source.cpp`. SHA256 JUnit :
  `1ea3f508e121690748e27a59e3246a4c16b8b4c76bf7b6e9b084622a4ed6dc23`.
  SHA256 source rouge : `cff4f404cc901a7c6b46022f87873394cb28f7b5152b80e2321425cda067400b`.
- GREEN : `08-green-focused.command.json`, `.junit.xml`, `.log`.
  SHA256 JUnit : `b2a8ca697054af1d12c909855dd7f9603b6b429ea2b029c0d3796c94dcf323ad`.
- Commandes : `ctest --test-dir out/build/windows-msvc-debug -R` avec sélection
  pixel seule pour RED, puis pixel/observation/consommateur pour GREEN ; options
  `--no-tests=error --output-on-failure -j 1 --output-junit <chemin unique>`,
  `-V` ajouté au GREEN. Arguments et exits exacts dans les JSON ci-dessus.
- [Contrat](../contract.md), [plan QA](../qa-plan.md),
  [intégration du mécanisme](12-mecanisme-integre-et-consommateur-debloque.md).
