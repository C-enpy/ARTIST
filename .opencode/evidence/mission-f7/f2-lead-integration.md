# F2 — intégration et revalidation lead, 2026-10-18

Base auteur d24340f ; commits livrés f51e16d (code/tests/fixtures) et 0673137
(note auteur). Worktree propre constaté avant shutdown. Preuves copiées vers
`out/mission-f7/f2/` du dépôt principal avant shutdown, puis team_merge natif.

Manifeste conservé SHA256
`40d30bb9165b0c69385aced5a6b23c3d1a22ad9815a770c8889a18c8c592079f`.
Sept sources/fixtures et34 artefacts recoupés par SHA256, zéro divergence.
Sept fichiers intégrés de nouveau comparés au manifeste auteur, zéro divergence.
Le BOM UTF8 du manifeste est ignoré au parsing seulement (octets inchangés).

Relecture lead : init size1/id0/type0 ; helper requireSingleValue size!=1
avant glUniform sur les34 spécialisations ; aucun getGLSize restant comme count
GL ; six oracles historiques renforcés0→1. Inventaire auteur34/30 recoupé.
Reader/Pass/Attribute/T1-1/mocks inchangés. Dérive .grepai/config.yaml exclue.

## Commande réellement exécutée sur candidat intégré

Avec cwd principal, même processus cmd : chargement out/tooling/load-msvc.cmd,
`cmake --preset windows-msvc-debug`, puis
`cmake --build --preset windows-msvc-debug --parallel 1`, puis
`ctest --preset windows-msvc-debug --no-tests=error --output-on-failure
--output-junit <absolu>/out/mission-f7/f2/lead-integrated-native.xml`.

Configure/re-glob, build et CTest exit0. JUnit :297 cas, failures0, disabled0,
skipped0 ; décompte249 unit +48 OpenGL. Log complet
`out/mission-f7/f2/lead-integrated-native.log`. Exécution GPU exclusive lead,
baux auteur rendus. Erreur de regex du script Context Mode corrigée avant
exécution : pas un test RED ni un échec de build.

## Qualification

Résultats auteur : RED CPU sûrs22échecs puis six oracles count1 échouant avant
production, GREEN et readbacks natifs consignés dans f2-author.md/manifeste.
Le lead a revalidé la suite intégrée ; pas de nouveau run ASan F2 revendiqué.
Familles natives non exercées distinctes de l'inventaire statique ; aucune
performance, image, absence exhaustive d'UB ou garantie driver ASan annoncée.
Revue indépendante fresh requise avant clôture du finding F2.
