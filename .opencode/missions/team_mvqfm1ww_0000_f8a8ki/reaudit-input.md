# Entrées neutres de ré-audit — frontières préparation et retrait partiel

Source exécutée finale : `3b1b1028dcd64ca46e0a359365ae096be1a38ac4`.
Depuis le premier audit source3d0e145, seul fichier C++ modifié :
`ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`.
Deux nouveaux tests (22 au total dans cette suite), includes de composants shader
existants. Aucun header de production, mock, fixture globale, CMake ou script changé.
Les documents de photographie non committés ne sont pas une évolution source.

## Scénarios actuels à ré-auditer

1. `PreparationRejectsMissingAndOtherContextBeforeNativeQueriesAndRecovers` :
   réflexion valide, miroir .25 et témoin/génération conservés. Préparer sous
   nullptr puis autre contexte doit donner les erreurs exactes, optional d'accès
   vide, zéro appel GL supplémentaire, aucune mutation ; contexte propriétaire
   restauré puis préparation et set .75 nominal, location/payload exacts.
2. `PartialDetachFailureRetiresBeforeFirstEffectAndRejectsRepeatedSets` :
   programme7, shaders61/62, accès préparé/mirror .25. Second wrapper déplacé,
   premier shader effectivement détaché, témoin vérifié révoqué dans cette action
   sans réentrer l'accès ; second wrapper provoque l'erreur attendue. Wrapper
   restauré avant assertions/destruction ; deux refus stale sans query/upload
   ni mutation CPU, puis cleanup explicite attendu.

Aucun appel à l'accesseur protected de génération ; seul le témoin public est lu.
Ces descriptions sont données de contrat/candidat, pas une conclusion d'auteur
ou d'autre reviewer. Les autres obligations de final-audit-input.md restent celles
du premier audit, sans élargissement de mission.

## Preuves nouvelles exécutées par le lead, pas par l'auditeur

Sous `out/qa-prepared-float/lead-followup-20261027/` :
- `02-nominal-focused.junit.xml` : deux tests, zéro failure/error/skip.
- `03-mutant-F1` diff/commande/JUnit/log : suppression de requireContext dans
  prepare, compilation0 et test8/failure1. Accès retourné sous chacun des deux
  contextes invalides et glCalls+1 sont détectés. Header restauré/hashé.
- `04b-mutant-F2-coherent` : révocation déplacée après détachements, tous les
  objets unitaires reconstruits avec cette même définition, compilation0 et
  test8/failure1. Témoin encore valide au premier effet, deux writes acceptés,
  queries/uploads/mirror modifiés sont détectés. Header restauré/hashé.
- EXCLURE `04-mutant-F2` : compilation d'un seul TU, autres objets gardant la
  définition inline d'origine ; test passé non probant. Différentes définitions
  dans le même binaire ne constituent pas une expérience cohérente du mutant.
- `05-normal-clean-build` et `12-asan-clean-build` : clean-first complet,
  69 étapes chacun, un job, sorties0, après restaurations.
- `06-final-build-unit` : commande convenue Debug/Jobs1/TestLabel unit exit0.
- `07-final-discovery.log` : CTest -N, 581 ; JSON `08-normal-discovery-json.log`.
- `09-final-unit.junit.xml` : 506 uniques, zéro failure/error/skip.
- `10-final-opengl.junit.xml` : 75 uniques, zéro failure/error/skip.
- `13-final-asan.junit.xml` : 581 uniques, même ensemble de noms que union
  normale ; zéro failure/error/skip, commande convenue AddressSanitizer exit0.
- `14-asan-discovery.json` : mêmes581. `final-asan-preflight/summary.json` :
  healthy0/UAF86, detect_leaks0 et options canoniques ; pas preuve de fuite.
- `final-validation.json`, SHA256
  `29a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b` :
  quinze hashes source, cinq binaires/probe, commandes et noms recoupés.

Le corps du test C/L/A est comparé byte-text à celui de3d0e145 et identique ;
tous les headers de production gardent leurs empreintes. Les expériences C/L/A
initiales restent celles décrites par final-audit-input et leurs archives ne sont
pas réétiquetées nouveaux runs. La suite complète est en revanche renouvelée581.

SHA256 test actuel : `2bf8ded00380889471ef851197fe3ec3826e68a4a15c77a26f611d6ac28c958d`.
Header PreparedFloatUniform : `a791eb6bb0ebd092112e3557a00a41416d59baf3df48455d738e9650e2b1c02d`.
Freer : `8bfe24d6525a204d40d47d67caf333d8be41fc6bf2b3731031844ee4f0cd7c62`.

## Restitution

Même session test-reviewer indépendante hors board et restrictions inchangées,
réponse finale directe uniquement ; aucun team_message hors équipe à répéter.
Lire les deux tests/diffs/preuves nécessaires, juger fermeture ou non de vos F1/F2,
puis confirmer la portée de votre verdict précédent à source/identité renouvelées.
Scénario discriminant avant tout finding haut. Aucun edit/shell/test/délégation,
grant ou réparation ; les limites historiques RED/fuites/OS/performance restent.
