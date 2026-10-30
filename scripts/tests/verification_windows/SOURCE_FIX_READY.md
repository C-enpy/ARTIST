# SOURCE_FIX_READY — admission des arguments et restauration par étape

## Candidat, autorité et preuves antérieures

Base propre `8f6fce2d9d521713aefd63056c76f090ebef1d7f`. Autorisation lead :
correction minimale entry/dispatch, plus ajout nominatif des variables nécessaires
au contrôle AST haut niveau. Aucun run auteur, aucune admission automatique.

Résultats **lead sur la base antérieure**, pas sur cette correction :
- NativeHarness 6/6 PASS puis SourceTests 20/20 PASS ;
- hauts niveaux Harness 7/7 PASS ; Admission 11 méthodes, 2 failures, zéro error/skip ;
- hashes stables, PowerShell fixe + fragments de données + doubles mémoire seulement.
GREEN global NON ACQUIS. Ce ne sont ni une qualification native ni des résultats
de compilation, rendu, moteur ou performance.

Manifest lead :
`out/verification-synthetic/windows-helper-binding-green-v2-2026-10-30T10-54-43-645Z/manifest.json`.
Ce manifest n'est pas présent à ce chemin dans ce worktree ; les cas locaux ont
été relus via Context Mode dans `out/verification-synthetic/windows-helper-functional/` :
- groupe `20261030T105517Z-9c0fd0ca49424b72a6cb6d3c1502e912`, 68 rapports analysés ;
- cas `case with spaces 3521967e6d044945b0edacdbb7f8d386` : UnknownFlag présent,
  harness_error/phase_error null, prepare-tooling/MSVC/env-write puis deux child ;
- cas `case with spaces c51f54ec6c0c43c28bbf33a772652c35` : tooling absent,
  MissingVerificationTooling puis événements existing-tooling + trois env-write.
Aucune valeur d'argument arbitraire ni environnement n'est reproduite ici.

## Premières causes et correction bornée

**F1.** Le script non avancé n'a pas CmdletBinding. Son admission transmet seulement
PSBoundParameters au résolveur de flags ; les arguments non liés dans `$args` ne
sont jamais examinés. L'invocation de fixture transmet réellement le dictionnaire
avec `& $program @flags`, sans filtrer UnknownFlag. Le succès/les événements du cas
ci-dessus concordent avec cette omission ; aucun nouveau runtime auteur ne la simule.

Ajout au tout début de `if ($verificationRequested)` : refus si `$args.Count` est
non nul. Message constant `InvalidVerificationFlags: UnknownFlag or unexpected argument.`
Il ne concatène ni nom reçu ni valeur/secrets. Le refus précède même le résolveur,
la lecture payload, le contexte, le snapshot, l'horloge, le tooling et MSVC.
Sans mode vérification demandé, cette branche n'est pas évaluée : legacy inchangé.
Pas de CmdletBinding, de nouveau paramètre, d'interprétation des arguments libres
ou de modification du résolveur NATIVE. Ce refus concerne aussi les positionnels
supplémentaires, sans inventer une nouvelle API publique.

**F2.** Le snapshot original est acquis avant Get-VerificationTooling. Si cet appel
lève, l'ancien finally appelle quand même Set-VerificationProcessEnvironment ;
ce wrapper écrit chaque valeur même identique. Cela explique les trois env-write,
sans prouver une mutation OS réelle (le port agit sur une map privée).

Ajout du booléen local `verificationEnvironmentAttempted`, faux avant try, vrai
immédiatement AVANT Set-VerificationProcessEnvironment(msvc). Le finally restaure
seulement si ce drapeau et le snapshot sont disponibles. Les échecs tooling/MSVC/
sélection précédant l'application ne déclenchent aucune écriture de restauration.
Une application partielle, un code enfant non nul ou un lancement normal restent
dans le chemin de restauration ; Exit-VerificationContext reste dans le finally
imbriqué. Aucun catch nouveau, aucune cause/code existant remplacé, aucune copie
ou filtre dans une fixture pour masquer les écritures du port.

## Impact et limites d'outillage

- Source : entry/dispatch seulement. Paramètres, WRAPPERS, NATIVE, BINDING/exporter,
  root et tail legacy comparés inchangés.
- Fixture haut niveau : seulement deux noms dans allowedVariables, `args` et
  `verificationEnvironmentAttempted`. Pas de commande/membre/type autorisé en plus,
  pas de wildcard, aucune donnée/événement/assertion modifiée.
- Les 26 méthodes natives et 18 hauts niveaux, leurs fichiers et assertions sont
  inchangés ; AST Python analysé sans import. Les deux JSON d'admission et pins
  NATIVE restent intacts, propriété du lead.
- Serena confirmé sur ARTIST canonique, backend C++ uniquement, pas le worktree
  PowerShell courant : aucun résultat symbolique prétendu pour ce delta. Inspection
  exacte des appels/consommateurs de fragments sur disque et logs via Context Mode.
- CMake, presets, Conan et CI relus sans exécution ; intégration Conan2 CMakeDeps/
  CMakeToolchain inchangée. Aucun configure/build, sonde STL/ABI, compile_commands,
  CTest, GPU ou ressource cloud dans le périmètre auteur.

## Hashes proposés — données seulement, NON ADMIS

Normalisation UTF8 sans BOM, LF. Les deux fichiers d'admission restent inchangés.
Pour **les deux JSON**, seuls source_sha256 et fragments.entry changent :

```json
{
  "source_sha256": "2424062ecf17bdd5f679506aeb11b816365ed10a2df3bf60274952cc9ae86e2a",
  "fragments.entry": "73331a4b7a244dddd27be7c8bb4c2190d1c05530137df7de056b463ddd1e6d43"
}
```

Empreintes inchangées recoupées, pour la revue indépendante des admissions :
- native_sha256 : `8c9a56ffeb6f082e79bbe569ac519cbbf31cc3c2002e988aac9a82e61216e3de`
- parameters : `8d1b63c0e5807d215c95a51f17ed2dfdb7895029979bb719a082c31b1abac539`
- wrappers : `470e8dfec02f65700766b312b54e8a4dad6e5d56ddad2160afbc7ae85d238371`
- data : `8b62b2ac8816a0a36a4a5627749dbfc1c6a9dd999b1c94cfbbeb8073b23afc6b`
- root / root_binding : `be460e7a3996b10e90c2359b408f68499dd309a98c4b3b60284e1080cde46c4a`
- binding / binding_sha256 : `2185760d0aa09835cb1937077cdd7a7d9a0b25af22c230863993bc3b9dcdcd84`
- exporter_sha256 : `002648e6c0615049cece76cbef4cc52a47e26b6f6932c68098ad2f5b1a970046`
- definitions historique : `b3a52340eeb9b263fdddcb65412ecd4557c36de0917c7e6081133a1287679278`

Les champs de provenance source_head/reference_head seront décidés/persistés
séparément par le lead après relecture du commit final. Aucun setUp ne repin.

## Matrice réellement exercée et suite lead

- Auteur Windows : lectures, analyse Context Mode des rapports existants,
  comparaison des régions, hashes et AST Python sans import ; aucune exécution PS.
- Candidat corrigé : deux tests discriminants et toute régression NON_EXECUTE.
- Linux, native tooling, compilation/STL, moteur/GL logiciel/matériel : NON_EXECUTE.
- Les PASS/FAIL lead ci-dessus restent attribués uniquement à 8f6fce2.

Après relecture, recoupement et admission explicite LEAD des deux nouveaux hashes,
depuis `scripts/tests/verification_windows/`, deux tests discriminants seulement :

```powershell
C:/Python313/python.exe -B -m unittest -v test_helper_functional.HelperFunctionalAdmissionTests.test_invalid_phases_opt_in_and_legacy_flags_have_zero_effects test_helper_functional.HelperFunctionalAdmissionTests.test_missing_build_tooling_does_not_bootstrap_or_configure
```

Attendu : UnknownFlag refusé avant tout événement ; tooling absent donne seulement
existing-tooling. Puis régression requise au lead (six baselines avant SourceTests,
20 SourceTests, 7 hauts niveaux Harness et 11 Admission) et revue fresh avant
intégration. Conserver assertions de restauration partielle/normale et codes bruts.
Pas de GREEN global, qualification native ni performance acquis par cette remise.

---

> Archive historique conservée intégralement ci-dessous. Ses identités, commandes
> et refus concernent une ancienne correction de capture, pas cette admission.

# SOURCE_FIX_READY — correction de l'ecart de capture, sans run

Base de revue : source `71cf8b0f032794814c3697e1f2fe3720af7c4759`,
admission candidate `d6715a21cce6732f2a54bb4ebe010f6f6edee21e`.
Le lead a relu ces candidats et **n'a accorde aucun run**. Aucun resultat GREEN
n'a ete observe ; la preuve RED reste celle sur `77900399fb9f18bc9f75d17904f8f8195b138321`.

## Premiere cause et correction autorisee

Ecart source constate : un quota cumule par flux, non ratifie, pouvait ecrire
une fraction du bloc lu puis echouer. Cela ne respecte pas la conservation des
journaux bruts complets ; les petits cas factices ne pouvaient pas qualifier ce
choix et aucun passage de test n'est revendique.

Correction unique : supprimer le quota, sa branche de troncature, le compteur
associe et les mentions le presentant comme contrat. Chaque bloc recu est
desormais ecrit en entier. Aucun autre plafond cumule ni relabel de quota.
Buffers/chunks de 64 Kio inchanges ; ce bornage de memoire n'est pas un quota
sur les journaux. Les exceptions IO, stop et drain restent des pannes explicites.
Les logs deja ecrits restent disponibles si une erreur IO empeche de continuer ;
le candidat ne pretend pas garantir l'ecriture au-dela d'une erreur de stockage.

Le code source est commite separement du nouveau pin/admission, a relire
ensemble avant toute execution. NATIVE_PIN_ADMISSION.md donne les identites
exactes. Le pin demeure une porte de revue, pas une sandbox.

## Frontieres preservees

- Quatre entrees seulement ; aucune autre fonction, schema/core, CLI ou outil.
- 15 methodes natives et 37 anciennes, assertions, fixtures et enfant inchanges.
- test06 reparse NON_EXECUTE, pas d'autre jonction/symlink/privilege.
- Le faux enfant n'a aucun descendant ; aucune nouvelle experience de processus.
- Dispose du lecteur avant pending.Wait demeure une operation synchrone fragile
  face a un writer herite. Les delais de Wait n'en font pas un delai absolu et
  ne prouvent pas la reclamation generale. Aucun fix de ce risque n'est invente
  ou revendique dans la correction de quota.
- Capture de production : revue independante requise avant helper complet.
  Refus du helper complet maintenu ; le pin ne leve pas cette frontiere.

## Matrice reelle et commandes

- Auteur : comparaison Git, digest et parsing AST statiques seulement.
- Nouveau source, pin-gate, 14 tests selectionnes et anciens 37 : NON_EXECUTE.
- Runtime, reclamation lecteurs, arbres de processus et payload reparse :
  aucune preuve nouvelle. Aucun moteur/MSVC/vcvars/build/install/GL/cloud.

Les commandes exactes et leur cwd demeurent dans NATIVE_GREEN.md : 14 noms
explicites hors06, puis les 37 tests historiques. Elles sont proposees au lead
uniquement APRES nouvelle revue/admission, pas autorisees par cette note.
Attendu theorique inchange : 14/14 + 37/37 sans failure/error/skip. Cela n'est
ni un resultat observe ni une preuve de correction de la capture de production.
