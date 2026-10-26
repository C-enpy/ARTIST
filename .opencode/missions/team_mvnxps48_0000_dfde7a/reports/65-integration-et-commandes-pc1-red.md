# 65 — PC1 source RED intégrée et commandes approuvées avant compilation

2026-10-26. Auteurpool9 commitcd7565308ff7e902ebebb95f7dd74f1857766aed, quatre fichiers/87lignes lus par lead. Intégration par patch exact, sans shutdown libérant PC1 encore actif ; identités Git des quatre blobs comparées au commit auteur, toutes conformes. Commit intégré **da6d4384db658f259e72f1b3d9146566506947c4**, aucun diffARTIST, aucun parent/enfant/flag/corps GREEN. Branche/worktree auteur conservés.

## Préenregistrement et approbation lead

Sortie exclusive neuve `out/qa-f8/close-policy-red-20261026-01`. Manifeste `candidate-before-build.json` SHA256 **e22f760c7377723bc1209c0d2bcfb24c0fcb4a156e83bd9bf461d5e526354d4d** contient les deux argv complets, candidat/source/config/compiler/wrappersSHA, entrée originale et substitutions. Les deux argv sont **approuvés tels quels avant invocation**, baseline puis contrat seulement si baseline verte ; pas d'interpolation shell ou d'options modifiées ensuite.

Dérivation CommandLineToArgvW depuis unique entrée actuelle TraceableExceptionTests.cpp ; conserve -c (compilation seule), includes/macros/protections /RTC1/-MDd/-Zi/C++latest et MSVC14.44, remplace uniquement source et /Fo,/Fd vers sorties distinctes. Aucun link, CMake/CTest/Conanconfigure/install, aucune fixture/attenteprocessus/protectionOS exécutée. Outils/config/compilateur comparés aux SHA acquis stdioGREEN, identiques ; ancien dossier non écrasé. Deux commandes wrapper existant, tags qa-f8-close-policy-red-20261026-01-baseline et -contract, logs complets séparés. Runner lead seul, commandes sérialisées ; environnement process sans GTEST_*/ASAN_OPTIONS/ASAN_ACTIVATION_OPTIONS.

## Oracle et arrêt

Baseline : exit0, SDK/constants et déclaration non utilisée compilent ; sinon STOP/INCONCLUSIVE, pas RED. Contrat : quatre static_assert présents, rejet spécifique car can_reap_process déclaré constexpr sans définition disponible. Diagnostic causal et notes du symbole requis ; motif header/toolchain/syntaxe/linkage ou mauvaise valeur fabriquée rejeté. Code exact de diagnostic peut dépendre MSVC ; C2131 attendu à titre indicatif, pas substitut à lecture des notes.

Source/config/SHA recontrôlés avant/après, commandes/logs/horodatages/codes conservés ; aucun objet stale (sortie neuve). Ce RED serait celui d'une capacité statique nouvelle, ni bug historique ni preuve de supervision/close/Reader. Pas nouvelle campagneCPU484/ASan555/GL ; niveaux non exécutés restent absents. GREEN interdit à l'auteur jusqu'à preuve RED intégrée et GO distinct ; tâche PC1 globale reste en cours.
