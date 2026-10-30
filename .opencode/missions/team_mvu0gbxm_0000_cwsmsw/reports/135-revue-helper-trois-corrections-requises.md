# 135 — Trois écarts bloquent l'intégration du helper

La revue indépendante identifie trois défauts dans la source, sans exécution
par le reviewer. Le coordinateur conserve le blocage d'intégration et demande
trois régressions synthétiques au propriétaire avant correction.

1. **Installation Visual Studio valide refusée** : la comparaison textuelle
   précède la normalisation du séparateur final exporté par les scripts MSVC.
   Le scénario sélection sans séparateur/export avec séparateur doit réussir.
2. **Préparation des tests incohérente avec la construction** : prepare demande
   PRE_TEST sans type de cache explicite, alors que build exige STRING. Le test
   doit relier la commande productrice à la représentation admise, sans lancer CMake.
3. **Copie du parent faussée par une variable légale Keys** : l'accès PowerShell
   `$block.Keys` peut être masqué par une entrée du dictionnaire. Le test doit
   exercer la vraie boucle source avec un dictionnaire synthétique et vérifier
   la copie complète avant filtrage, sans modifier l'environnement OS.

Les tests verts du rapport134 restent valides à leur portée mais ne couvrent pas
ces coutures. Aucune reproduction dynamique de ces nouveaux cas n'est encore
revendiquée. Les nouveaux tests seront relus et exécutés par le lead ; fonction
absente, pin refusé ou harnais cassé ne constitueront pas un RED comportemental.
La source et les admissions restent inchangées à ce stade.

Une question distincte demeure sur l'encodage des chemins non ASCII dans le batch
cmd. Le reviewer ne la classe pas comme défaut natif reproduit. Elle reste à
documenter ou préparer pour qualification, sans autoriser cmd/vcvars ici.

## Références

- Avis `result_mvuvfbfr_007l_n44e8z`, reviewer pool-36, lecture seule, aucun test,
  processus, édition ou délégation.
- Candidat `1d0fba7b2914c9d610f9c1857a7f1b0f3637abc0`, source f534bd2.
- Helper : Read-VerificationMsvcEnvironmentLeaf1045–1052 ;
  Assert-VerificationPreparedTree1123/1134 et dispatch configure ;
  Read-VerificationHostObservationLeaf660. Ces relations ont été relues par le lead.
- Un message de retour au reviewer a d'abord rencontré une erreur de chaîne
  JavaScript avant appel natif ; l'appel corrigé a été envoyé, sans doublon exécuté.

Aucun nouveau test, outil natif, helper complet, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté. La livraison Windows complète n'est pas reçue.
