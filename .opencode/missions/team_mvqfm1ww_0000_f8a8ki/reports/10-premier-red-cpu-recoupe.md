# 10 — Premier RED CPU recoupé : préparation stricte, pas régression legacy

## Résultat observé

L'auteur a exécuté la première commande convenue dans son worktree avec une
toolchain fonctionnelle. Le lead a copié le journal et le JUnit avant leur futur
remplacement par GREEN, puis les a analysés dans Context Mode.

Population JUnit :488tests CPU,488noms uniques,485PASS et3FAIL attendus,
0error et0skip. Il s'agit d'un RED du nouvel accès préparé ; ni un PASS global,
ni une preuve GPU, ni un bug des compatibilités legacy conservées.

## Causes des trois échecs

- ApiExistsAndCannotBeForgedFromNativeNames : FAIL explicite « PreparedFloatUniform
  API is not implemented », dans un test compilé/exécuté, pas toolchain absente.
- ArrayOfOneRejectedBeforeCpuMutationOrUpload : l'adaptateur de référence legacy
  accepte array[1], n'émet pas l'erreur requise par le futur accès strict et appelle
  glUniform1f. Le miroir est ensuite modifié ; un GET<int> type-mismatch secondaire
  est visible dans la sortie. Les premiers écarts de rejet/upload sont déjà causaux.
- BooleanCompatibilityDoesNotLeakIntoStrictFloatAccess : le legacy accepte
  GL_BOOL via float et émet l'upload, contrairement à la nouvelle préparation
  strictement GL_FLOAT. L'ancienne interface ne doit pas être durcie pour le vert.

ScalarWritesTypedArgumentAndHistoricalCpuMirror passe sur la référence legacy :
cela caractérise le nominal de départ, pas une implémentation préparée déjà livrée.
Le test détecte explicitement l'absence de nouvelle API ; le lead a demandé que
GREEN exige réellement cette API dans tous les tests préparés et ne puisse passer
par un fallback silencieux. Les compatibilités historiques gardent leurs tests.

## Environnement et limites de la preuve

Log recoupé : MSVC19.44.35221/toolset14.44.35207, SDK10.0.26100,
Conan2.12.2 ; CMake3.31.7 annoncé par l'auteur. Configuration Debug, un job,
tests label unit. Le journal est UTF-16 avec BOM : un premier filtrage UTF-8
n'a trouvé aucune ligne utile ; le décodage a été corrigé sans relancer de test.

Commande auteur : powershell -NoProfile -ExecutionPolicy Bypass -File
scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit.
Le journal confirme «3 tests failed out of488» et les trois noms ci-dessus.
La découverte complète -N du candidat final n'est pas encore recoupée ici.

L'absence de modification de header production à cet instant est une déclaration
de l'auteur ; une identité/diff exacts des sources RED ont été demandés si capturés.
Le lead n'antidate pas un snapshot après changement. Ce RED borne l'incrément
préparation/type ; il ne vaut pas preuve RED des futurs cas d'invalidation.
L'auteur a reçu la demande de tranches TDD causales sur ces gardes et de limites
explicites si un cas est seulement ajouté après son implémentation.

## Archives détenues par le lead

Source : worktree pool-2, out/logs/prepared-float/red-build.log et
out/build/windows-msvc-debug/ctest.xml. Copies canoniques sans overwrite :
out/qa-prepared-float/author-red-20261027/.

- red-build.log :2002102octets, SHA256
  426de8d608e78843686181b1bb9961df577bc8e6f450fe0e0cbca0905911fb42.
- red-unit.xml :526828octets, SHA256
  d4ea6a0032cd7fcf43fbdccb716416df0fc973133b782971e53a7f27c21f276d.

Runner réel du RED : auteur cpp-engineer. Copie/analyse/recoupement : lead.
Aucun test relancé par le lead, aucune mutation du worktree auteur et aucun
conflit de bail build/GPU. Le GREEN, les régressions et l'intégration H1 restent
à obtenir ; le consommateur demeure non publié.
