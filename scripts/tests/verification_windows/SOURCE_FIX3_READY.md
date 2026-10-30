# SOURCE_FIX3_READY — GREEN3 minimal proposé, sans run auteur

## Identité, autorité et RED acquis par le lead

Base propre vérifiée `de752f8fc9044d759a79209114f4bb21fe4656ac`.
Revue indépendante : `result_mvuvfbfr_007l_n44e8z`. Autorisation GREEN3 reçue après
preuve RED lead et ratification ponctuelle du mapping rapport136/`95968ed`.
Ce rapport de ratification est cité d'après le message lead, pas réinterprété
comme une nouvelle admission automatique des pins ou un résultat runtime auteur.

Preuve **LEAD sur de752f8**, pas sur ce delta : 5/5 nouveaux contrôles PASS,
puis trois SourceTests / cinq failures avec sous-cas, zéro error/skip, hashes
stables. Logs fournis par le lead :
`out/verification-synthetic/windows-helper-review-red-2026-10-30T11-21-13-861Z/`.
Ces logs ne sont pas réexécutés par l'auteur. Le chemin brut 20261030T112* n'est
pas présent sous out/verification-synthetic/windows-native-binding de ce worktree
au contrôle local.

- F1, cas `280dab0088fc467dab340bae0a63e2bc` : InvalidMsvcEnvironment metadata
  differs, harness_error null, seulement python-pip/msvc-env, OS parent intact.
- F2 : token réellement capturé sans STRING ; cache UNINITIALIZED dérivé refusé
  avant observations/process/horloge. Deux frontières causales d'une méthode.
- F3 : source_copy={COPY_KEYS_CANARY:null}, filtered={}, au lieu des maps attendues ;
  snapshot synthétique intact. Deux frontières causales d'une méthode.

Les trois causes sont désormais RED causal acquis par le lead, pas un refus de
pin, une fonction absente ou un parse error. Aucun outil natif/CMake/moteur réel
dans cette preuve. **GREEN du nouveau candidat NON ACQUIS.**

## Correction bornée dans le helper

### F1 — identité de répertoire sélectionné/observé

Dans Read-VerificationMsvcEnvironmentLeaf seulement :
- conserver Get-VerificationAbsolutePath existant, sans le modifier globalement ;
- normaliser le chemin sélectionné après cette validation, avant toolset,
  SelectedVisualStudio, chemins de transport et futur témoin ;
- normaliser VSINSTALLDIR exporté avant comparaison à la sélection ;
- retirer uniquement les séparateurs finaux `\` et `/`, puis rétablir GetPathRoot
  si ce retrait raccourcirait la racine. Ne jamais transformer une racine de
  lecteur C:\ en C:, ni tronquer une racine UNC ;
- comparer les deux chaînes normalisées par le même -ine qu'avant ; puis garder
  l'affectation VSINSTALLDIR=installation sélectionnée, utilisée pour Conan host
  ET build. Pas de valeur exportée libre injectée dans les arguments Conan.

Les refus de chemin absolu, installation incomplète, caractères de transport
interdits, toolset/arch/SDK incohérents restent en place. Aucun nouveau fallback,
adoption ou changement des garanties existantes sur les reparses. Les comparaisons de
préparation/build déjà présentes ne sont pas refactorées globalement. La
préservation des racines est ici une propriété du code relu, pas une campagne
runtime supplémentaire sur toutes les formes Windows/UNC.

### F2 — type cache produit explicitement

Seul l'argument conditionnel du configure devient
`-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING=PRE_TEST`.
BUILD_TESTING=ON et ce token restent conditionnés par la sélection d'une cible
de tests ; autrement BUILD_TESTING=OFF. Assert-VerificationPreparedTree est
strictement inchangé : STRING et PRE_TEST exigés, mauvaise valeur/type et doublons
restent des refus. Pas de CMake réel ni de cache réparé par la correction.

### F3 — vraie boucle snapshot

Un seul accès modifié dans Read-VerificationHostObservationLeaf :
`$block.Keys` → `$block.PSBase.Keys`. La vraie copie complète reste avant le filtre.
Pas de retrait du canari depuis les données, filtre dupliqué, faux snapshot OS ou
nouvelle API. La sonde F3 déjà relue admet exactement cette grammaire ; aucune
fixture/allowlist supplémentaire n'est modifiée ici.

## Mapping de test explicitement ratifié

`test_helper_functional.py`, seule assertion concernée :

```python
self.assertEqual("-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING=PRE_TEST" in cmake, testing)
```

Seul le littéral passe de -D…=PRE_TEST à -D…:STRING=PRE_TEST, sur autorisation lead
rapport136/95968ed antérieure à l'édition. Même assertEqual, même variable testing,
même condition de cibles, pas d'assertion supprimée ni de tolérance élargie. Ce
mapping est renforcé par l'oracle argv exact/producteur-consommateur RED3 et les
négatifs typés. Aucun autre oracle ne change. Les 26 tests natifs historiques,
3 nouveaux SourceTests et 5 nouveaux contrôles/leurs assertions sont intacts.

## Hashes proposés — ne pas admettre automatiquement

UTF8 sans BOM / LF. Les deux JSON restent propriété du lead et INCHANGÉS sur disque.
Calculs par lecture/hash de données, sans import de cible ni exécution de fragment.

Proposition pour `native_binding_admission.json` (provenance du commit à compléter
par le lead après revue ; exporter inchangé) :

```json
{
  "source_sha256": "88f1751bed1b897a7d3df965de7533ec20e12a95746ab2c75eb8d34606880545",
  "native_sha256": "8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de",
  "fragments": {
    "parameters": "8d1b63c0e5807d215c95a51f17ed2dfdb7895029979bb719a082c31b1abac539",
    "wrappers": "470e8dfec02f65700766b312b54e8a4dad6e5d56ddad2160afbc7ae85d238371",
    "data": "8b62b2ac8816a0a36a4a5627749dbfc1c6a9dd999b1c94cfbbeb8073b23afc6b",
    "root": "be460e7a3996b10e90c2359b408f68499dd309a98c4b3b60284e1080cde46c4a",
    "entry": "340390c41092a9b9fab80def7bcdfee1870a2580739ffb8aa89977a84dab1597",
    "binding": "7ae73cbe6780c13de521be870c197990d8017db869a47b412e83400d9773496e"
  },
  "exporter_sha256": "002648e6c0615049cece76cbef4cc52a47e26b6f6932c68098ad2f5b1a970046"
}
```

Proposition pour `helper_fragments_red.json` :

```json
{
  "source_sha256": "88f1751bed1b897a7d3df965de7533ec20e12a95746ab2c75eb8d34606880545",
  "binding_sha256": "7ae73cbe6780c13de521be870c197990d8017db869a47b412e83400d9773496e",
  "fragments": {
    "parameters": "8d1b63c0e5807d215c95a51f17ed2dfdb7895029979bb719a082c31b1abac539",
    "definitions": "b3a52340eeb9b263fdddcb65412ecd4557c36de0917c7e6081133a1287679278",
    "entry": "340390c41092a9b9fab80def7bcdfee1870a2580739ffb8aa89977a84dab1597",
    "root_binding": "be460e7a3996b10e90c2359b408f68499dd309a98c4b3b60284e1080cde46c4a"
  }
}
```

Changements réels de hashes : source, BINDING et entrée. Les definitions historiques
chargent les quatre helpers purs, pas ReadHost/ReadMsvc : leur hash reste inchangé.
WRAPPERS/NATIVE, params/root, exporter et consommateur de cache sont recoupés intacts.

## Vérifications auteur et commandes réservées au lead

- Auteur Windows : diff/lectures, comparaison des régions, hashes, AST Python sans
  import ; seul littéral ratifié changé dans les 18 hauts niveaux. Aucun PS lancé.
- Candidat GREEN3 : 5 contrôles, 3 discriminants et régressions 6/20/7/11 NON_EXECUTE.
- Syntaxe/runtime PS, Linux, compilateur/STL/ABI/compile_commands, CMake/CTest,
  outils, moteur, GL logiciel/matériel, benchmark et cloud : NON_EXECUTE.
- Legacy/WRAPPERS/NATIVE, core/adapters/CMake/CI et fixtures restent inchangés.

Après relecture et nouvelles admissions LEAD séparées, depuis
`scripts/tests/verification_windows/`, contrôles puis discriminants :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ReviewRegressionHarnessTests
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ReviewRegressionSourceTests
```

Puis non-régression, dans l'ordre fixé par le lead :

```powershell
C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingHarnessTests
C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingSourceTests
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalHarnessTests
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalAdmissionTests
```

Attendu NON OBSERVÉ : 5/5 puis 3/3, puis 6/20/7/11 sans failure/error/skip. Un
refus d'identité/parse/baseline invalide reste HARNESS et bloque la preuve, pas RED
produit. Réaudit indépendant pool36 et couture effective main/collecteur9aedc0a
restent requis avant intégration. Le candidat n'est PAS intégrable par cette note.

L'encodage Default.CodePage vs cmd pour chemins non ASCII reste une limite
distincte NON ÉTABLIE, sans probe ni modification préemptive. Le gap natif
Dispose/writers hérités n'est ni corrigé ni qualifié par ces trois changements.
Aucune qualification native, mesure de performance ou campagne moteur revendiquée.
