# HARNESS_FIX3_READY — valeur synthétique sélectionnée pour VSINSTALLDIR

## Base et preuve lead, sans nouveau run auteur

Base propre `7fcee245ec14b95fea97cd60fa483ea04f8e308e` : admissions LEAD explicites,
source `ead7f8374206062892dfb752626ffc1e76ac3dfc` inchangée,
SHA256 normalisé `88f1751bed1b897a7d3df965de7533ec20e12a95746ab2c75eb8d34606880545`.
Les JSON d'admission ne sont pas modifiés par cette correction de fixture.

Lead : 5/5 nouveaux contrôles PASS, puis trois SourceTests / une failure HARNESS,
zéro error/skip et hashes stables. F2 producteur/consommateur et F3 copie/filtre
GREEN sur cette base ; F1 **INCONCLUSIVE/HARNESS**, pas un nouveau RED produit.
Non-régression6/20/7/11 NON_EXECUTE après arrêt de la séquence.

Manifest fourni :
`out/verification-synthetic/windows-helper-review-green-2026-10-30T11-28-41-518Z/manifest.json`.
Cas local relu via Context Mode, sans exécution :
`out/verification-synthetic/windows-native-binding/20261030T112846Z-a906510ced75499ea49b4eadf7fb76e9/case with spaces c89312afbf774b1b8e21c0c7fe6e91a5/`.

## Première cause confirmée par lecture et données existantes

Le VSINSTALLDIR brut synthétique de input.msvc est exactement l'installation
sélectionnée de expected_witness avec un séparateur final supplémentaire. Après
le vrai ReadMsvc corrigé, l'environnement privé expose l'installation sélectionnée
normalisée. La garde de capture comparait ce champ à la seule graphie brute de
input.msvc, donc refusait cette valeur pourtant synthétique et attendue.

Rapport observé : harness_error « non-synthetic child environment value »,
phase_error null, captures seulement python-pip/msvc-env ; trois objets mémoire
créés, chacun Dispose une fois, OS parent inchangé et état retiré. Le troisième
objet n'est pas enregistré en capture : refus avant l'ajout à la liste. Aucun
argument ni valeur d'environnement n'est recopié dans le présent diagnostic.

## Delta exact autorisé

Dans Invoke-BindingMemoryCapture, après les branches PATH et CONAN_HOME, ajouter
la seule branche nominative VSINSTALLDIR :
- la clé doit déjà exister dans le dictionnaire synthétique (garde inchangée) ;
- valeur autorisée = expected_witness.visual_studio_installation, déjà donnée
  synthétique explicite de sélection ; Assert-BindingOwnedPath la confine au cas ;
- comparaison Ordinal EXACTE, sans GetFullPath/TrimEnd/casefold/tolérance pour ce
  nouveau champ. La fixture n'implémente pas la normalisation producteur ;
- toute autre valeur échoue avant sérialisation, avec message constant contenant
  seulement le nom de clé sûr VSINSTALLDIR. Aucun argument, valeur OS/cmd/env,
  Extent.Text ou exception détaillée n'est imprimé.

La donnée raw input.msvc n'est PAS réécrite, le canari n'est pas filtré et le test
F1 garde toutes ses assertions sur succès, témoin et choix Conan host/build.
PATH/CONAN_HOME et la comparaison stricte des autres valeurs restent textuellement
inchangés ; pas de permission par rôle, suppression de garde ou exception F1.
Pas de nouvelle commande, processus, capture native ni adoption de chemin externe.

## Invariants et matrice réellement exercée

- Auteur Windows : lecture de la fixture/source et JSON existants via Context Mode,
  analyse de données, diff/hashes et AST Python sans import ; aucun PS exécuté.
- Production/WRAPPERS/NATIVE/legacy, les deux admissions JSON, 26+18 anciens tests
  et huit nouveaux tests/assertions/sonde F3 : inchangés.
- Syntaxe/runtime PS, F1 corrigé et toute non-régression du nouveau candidat :
  NON_EXECUTE. Aucun GREEN global acquis par cette remise.
- Linux, tooling/CMake/CTest, helper complet, moteur, GPU/benchmark, installation
  et cloud : NON_EXECUTE. Aucun changement de contrat d'encodage non ASCII ni de
  qualification native/Dispose/writers hérités.

## Relance proposée au lead après relecture de ce delta

Depuis `scripts/tests/verification_windows/`, trois discriminants existants :

```powershell
C:/Python313/python.exe -B -m unittest -v test_review_regressions.ReviewRegressionSourceTests
```

Attendu NON OBSERVÉ : trois méthodes valides, zéro failure/error/skip ; en F1,
la capture Conan accepte la seule valeur synthétique sélectionnée puis les
assertions métier intactes prouvent le comportement. Toute autre panne de fixture
reste HARNESS, sans retirer normalisation ou assertions pour la masquer.
Le lead conserve les contrôles/non-régressions nécessaires puis le réaudit pool36
avant intégration ; aucune exécution ni intégration autorisée par cette note.
