# 156 — Pipeline livré pour qualification Windows/Linux

Le pipeline de vérification des uniforms et paramètres GPU est livré pour une
qualification ultérieure sur machines maîtrisées. Catalogue sélectionnable,
scénarios réels, phases distinctes, modes list/plan consultatifs, adaptateurs
Windows/Linux et conservation des preuves sont intégrés. Aucun gain de performance
ou résultat de campagne native n'est requis ou revendiqué pour cette livraison.

La dernière revue indépendante est reçue intégralement : **aucun défaut bloquant
établi dans les assertions examinées**, aucun correctif demandé. Elle confirme
une capacité discriminante des compléments G1–G4, sans prétendre certifier tout
l'inventaire historique ou l'audit AST. Le coordinateur accepte la livraison
dans cette portée ; un PASS local ou une revue statique ne devient pas un PASS
du moteur, du GPU ou du fonctionnement natif Windows/Linux.

## Vérifications effectivement obtenues

- Dernière branche Windows : **19/19 tests synthétiques**, sans erreur ou skip,
  dont environnement réellement remis à la factory, admission fine, migration
  des anciens oracles, quoting/code17 et raccordement simulé StartInfo.
- Couture du même bundle : **6/6 baselines puis4/4 caractérisations**, sans erreur
  ou skip. Vrais reçus, mêmes octets du témoin, refus mutation/absence avant build,
  partiels et première cause Conan17 conservés.
- Empreintes avant/après stables et comparées aux sources actuelles. Les53 fichiers
  production/cœur/tests core des preuves antérieures sont inchangés. Les comptages
  historiques du rapport142 sont réutilisés avec leur candidat, pas rejoués ni
  ajoutés aux sélections recouvrantes :56 helper,54 Windows,224 cœur dont219 succès
  et5 skips,28 contexte,31 scénarios et11 intégration.
- Reviewer indépendant fresh, profil test-reviewer recoupé nativement, brief privé
  neutre hors contenu de board d'implémentation. Lecture seule, aucun run reviewer.
  Avis `result_mvuzl44v_0000_r21li8`, persisté dans final-test-audit-result.md.

Les erreurs de harnais du premier candidat couture sont conservées au rapport150.
Leur correctif d'une ligne a été relu, intégré puis vérifié ; elles ne sont pas
présentées comme un RED de production. Les résultats verts reposent sur le candidat
corrigé, pas sur une répétition ou une réécriture silencieuse des preuves.

## Limites de réception

Compilation/exécution C++, suites moteur même CPU, helper complet, MSVC/Conan/CMake/
CTest réels, cmd/vcvars, Windows/Linux natifs, OpenGL logiciel/matériel, image,
benchmark/performance, workflow et cloud : **NON_EXECUTE dans cette mission**.
Seuls les tests synthétiques et programmes factices relus sont exécutés.
Les rôles python-pip et outils sont simulés, pas une installation réelle.

Cinq skips historiques symlink/reparse et jonction06 restent non qualifiés,
ainsi que l'encodage cmd non ASCII et le gap Dispose/writers hérités. Le témoin
toolchain est déclaratif ; son hash JSON ne prouve pas l'identité binaire des outils.
La politique QA ne devient pas une sandbox ; enforcement négatif non testé et
exposition fetch dans Code Mode ne sont pas présentés comme des droits sûrs.
Statuts ADR conservés, aucun nouveau backend, fournisseur ou modèle imposé.

## Fichiers utiles

- Usage et futures commandes : `docs/development/verification-pipeline.md`.
- Entrées : `scripts/verify.py`, `scripts/verification_workflow.py` ; cœur et
  adaptateurs sous `scripts/verification/` ; helpers Windows/Linux existants.
- Workloads livrés non compilés : `ARTIST/verification/` ; raccordement CMake
  optionnel et `.github/workflows/verification-pipeline.yml` manuel seulement.
- Nouveaux tests : `scripts/tests/verification_seams/` et tests Windows migrés.
- Contrats/QA/avis : `.opencode/evidence/mission-uniform-pipeline/`.

## Commandes réellement exécutées pour les derniers compléments

Depuis `scripts/tests/verification_windows`, avec `C:/Python313/python.exe` :

```text
-B -m unittest -v test_effective_windows_environment.EffectiveWindowsEnvironmentTests test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_admits_only_validated_prepare_build_before_effects
-B -m unittest -v test_dispatch.WindowsDispatchMigrationTests
-B -m unittest -v test_wrappers.WindowsWrapperTests test_native_binding.NativeBindingSourceTests.test_real_child_wrapper_to_real_binding_records_exact_start_info
```

Depuis la racine ARTIST, même interpréteur :

```text
-B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1BaselineTests -v
-B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1CharacterizationTests -v
```

Commandes complètes, cwd, versions/candidat, hashes, sorties et arbres bruts :
- `out/verification-synthetic/windows-migration-integrated-2026-10-30T12-31-46-914Z/`
  (candidat intégréa130773, auteur pool-40, runner coordinateur).
- `out/verification-synthetic/windows-seam-baseline-v2-2026-10-30T12-50-42-192Z/`
  et `windows-seam-characterizations-2026-10-30T12-51-14-668Z/` sous le même parent
  (code165aee5, auteur pool-41, réparation pool-44, runner coordinateur).

## Clôture des suivis, sans travail doublon

Le collecteur/JUnit intégré813b84e (rapport90), l'adaptateur Windows86b51ad
(rapport113), le helper52e7330 (rapport142), les complémentsa130773/165aee5 et leur
revue permettent de clore les anciennes tâches parent réellement satisfaites.
La seule fiche d'admission du reviewer est close sur les lectures/routage/remise
vérifiés ; la revue elle-même est une restitution privée, pas une tâche inventée.

Les fiches différées du plan initial intégration/generator/audit ont été remplacées
par ces tranches et restitutions réelles : maintien inactif avec traçabilité, pas
nouveau départ ou faux completed pour effacer une absence de livrable. Équipe,
branches et preuves conservées ; aucun nettoyage, purge, push ou merge manuel.
Les modifications préexistantes des profils agents et autres fichiers hors
mission ne sont ni incluses ni supprimées.

Après enregistrement natif, aucune tâche n'est en cours. Les trois anciennes
fiches restent explicitement deferred avec le motif de remplacement ; l'une
est pending mais non publiée, les deux autres blocked/deferred. Elles ne sont
ni des départs autorisés ni des livrables manquants masqués par un faux completed.
Le contrôle final du diff n'a trouvé aucune divergence des sources testées,
aucun lien/source manquant dans le guide et aucune erreur de whitespace.
Avis/QA/guide et ce rapport sont persistés dans le commit de réception111879f ;
la présente précision de suivi ne modifie aucune source ou preuve de test.
