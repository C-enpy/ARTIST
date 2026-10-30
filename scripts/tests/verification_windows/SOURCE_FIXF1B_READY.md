# SOURCE_FIXF1B_READY — guard build VS explicite, GREEN proposé sans run

## Base, autorité et RED acquis par le lead

Base propre `71333efc4d0ddebcae22b647047ef266a321f3e7`.
Le lead a constaté sur cette base : trois contrôles F1B PASS, puis un test/une
failure causale, zéro error/skip et hashes stables. Cause exacte du rapport :
`InvalidMsvcEnvironment: explicit Visual Studio differs from preparation.`
harness_error null, captures/Process mémoire vides, OS parent intact ; flag avec
suffixe et témoin normalisé vérifiés avant l'assertion de succès.
Logs lead :
`out/verification-synthetic/windows-helper-explicit-vs-red-2026-10-30T11-41-01-816Z/`.
Cette preuve est réutilisée comme RED attribué au lead, pas reproduite par l'auteur.

Autorisation GREEN limitée à la garde build dans Read-VerificationMsvcEnvironmentLeaf.
F2/F3 et F1 export gardent leur portée de preuve antérieure ; aucun GREEN du présent
candidat, qualification native ou intégration n'est acquis par cette remise.

## Cause et correction bornée

La garde comparait Get-VerificationAbsolutePath(requested) à celui du témoin avant
le retrait des séparateurs finaux déjà effectué plus bas pour la sélection/export.
La normalisation tardive ne pouvait donc pas éviter le refus du même répertoire.

Le seul bloc changé est le contrôle de VS explicite en phase build :
1. Si un argument explicite existe, conserver les deux appels au validateur absolu
   existant ; ne pas modifier Get-VerificationAbsolutePath globalement.
2. Construire deux valeurs LOCALES requested/prepared. Une boucle sur ces deux
   noms littéraux applique symétriquement le même algorithme relu : GetPathRoot,
   TrimEnd des seuls séparateurs `\` et `/`, puis rétablissement de la racine si
   le retrait la raccourcit (pas de conversion C:\ vers C:, pas de racine UNC perdue).
3. Comparer ces deux valeurs normalisées par le -ine existant. Un répertoire
   véritablement différent conserve exactement le même refus causal avant
   découverte, transport MSVC, capture ou build. Aucun fallback/réparation ajouté.
4. Après la garde, conserver l'affectation installation=Prepared.visual_studio_installation
   et toute la sélection/normalisation/observation déjà relue. Le témoin, et non
   un argument libre divergent, reste la source de choix de build.

Cette boucle partage une seule normalisation pour les deux côtés du guard ; elle
ne crée aucun helper public, scriptblock évalué, nouvelle API ou changement large.
Les blocs sélection/export plus bas emploient le même algorithme et restent
textuellement intacts. La correction ne change ni politique de chemins relatifs,
ni validation des caractères de transport, ni garantie existante de reparse.
Les formes racines sont préservées par construction ; pas de qualification
runtime nouvelle de tous les chemins Windows/UNC revendiquée.

## Invariants et diff

- Le préfixe ET le suffixe complets du script, hors ce seul bloc de garde, ont
  été comparés identiques à la base. WRAPPERS/NATIVE, legacy, phase prepare,
  cache/admission stricte, exporter, témoin et arguments Conan host/build intacts.
- Aucun changement des 56 tests courants (26+18+8+4), de leurs assertions/support,
  fixtures mémoire ou sonde F3. Pas d'adaptation d'oracle nécessaire ici.
- Les deux JSON d'admission et le pin NATIVE sont INCHANGÉS, propriété du lead.
  Core/adapters/CMake/CI, preuves précédentes et encodage non ASCII hors delta.
- Aucun test, import cible, faux processus, PowerShell ou outil exécuté par l'auteur.

## Hashes proposés — aucune admission automatique

UTF8 sans BOM / LF. Les calculs portent sur les fragments effectivement présents,
sans importer l'extracteur ni invoquer le helper. Les seuls hashes à modifier
après revue LEAD sont source et BINDING :

```json
{
  "source_sha256": "833b1b596629252b5442f659404ed12c5b83dca37919ccc58da9b6784308c8c2",
  "binding_sha256": "1362da7f4eb3e95cddf47da27d68d5bb01738d4d162dd2f712554a989e824876",
  "entry_sha256": "340390c41092a9b9fab80def7bcdfee1870a2580739ffb8aa89977a84dab1597",
  "exporter_sha256": "002648e6c0615049cece76cbef4cc52a47e26b6f6932c68098ad2f5b1a970046",
  "historical_definitions_sha256": "b3a52340eeb9b263fdddcb65412ecd4557c36de0917c7e6081133a1287679278",
  "native_sha256": "8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de"
}
```

Mapping de proposition, pas édition de JSON :
- native_binding_admission.json : source_sha256 et fragments.binding ; fragments.entry
  demeure l'entry_sha256 ci-dessus, exporter_sha256 reste inchangé ;
- helper_fragments_red.json : source_sha256 et binding_sha256 ; fragments.entry
  inchangé, fragments.definitions conserve historical_definitions_sha256 ;
- provenance du commit source final à renseigner séparément par le lead.
Les autres fragments ne dépendent pas de la garde ReadMsvc et restent intacts.

## Matrice réellement exercée et commandes réservées au lead

- Auteur Windows : lectures/diff via Context Mode, hashes, AST Python sans import
  des modules de test et comparaison des 56 méthodes/support/assertions ; aucune
  exécution du candidat ni du helper complet.
- Candidat corrigé : trois contrôles F1B + un discriminant et non-régressions
  NON_EXECUTE. Syntaxe/runtime PowerShell NON_EXECUTE.
- Linux, compilateur/STL/ABI/compile_commands, Conan/CMake/CTest, moteur, GL logiciel
  ou matériel, performances, installation et cloud : NON_EXECUTE.

Après revue et nouvelles admissions LEAD séparées, depuis
`scripts/tests/verification_windows/` :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ExplicitVsDirectoryHarnessTests
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ExplicitVsDirectorySourceTests.test_build_explicit_suffix_accepts_same_normalized_prepared_directory
```

Attendu NON OBSERVÉ : trois contrôles verts puis un discriminant vert, zéro
failure/error/skip ; répertoire réellement différent toujours refusé sans capture.
Les tests nécessaires de non-régression et le réaudit pool36 restent à mener par
le lead. Un refus de pin/parse/baseline n'est pas une preuve produit.
La couture réelle d'un même témoin/bundle sur main après intégration des deux
producteurs reste DISTINCTE : ces cas isolés ne prouvent pas une campagne réelle.
Aucun changement des limites encodage/Dispose/writers hérités ni qualification
native/performance revendiquée. Pas d'intégration autorisée par cette note.
