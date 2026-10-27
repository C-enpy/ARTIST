# 24 — Bilan final : accès float préparé livré et vérifié au périmètre Windows Debug

## Résultat livré

Préparer un uniform float une fois puis l'écrire par accès typé est utilisable
dans ARTIST/OpenGL. Le contrat est limité au float strictement scalaire : arrays
de taille1 compris refusés. Owners uniques, accès emprunté révocable et rejet
avant pointees/miroir/upload conservés. L'API par nom et le miroir CPU historique
restent disponibles ; aucun nouveau backend ou gain de performance annoncé.

Le consommateur target utilise un accès préparé unique, decoy reste par nom.
Readbacks GL et quatre pixels fixes non vides vérifient les deux routes. Les
omissions de valeur/draw et les mutants C/L/A sont discriminés. Après audit,
deux tests supplémentaires couvrent le contexte de préparation et le retrait
partiel ; leurs mutants cohérents sont détectés puis les sources restaurées.

**Final : 581/581 normaux et 581/581 ASan**, chacun506CPU+75OpenGL,
mêmes581noms uniques, zérofailure/error/skip, canariASan0/86. Exécution runner
lead ; QA et reviewers lecture seule. Revue C++ sans défaut bloquant démontré ;
audit fresh hors board puis ré-audit **PASS borné**, F1/F2 fermés, aucun finding
bloquant maintenu. L'historique RED reste explicitement INCONCLUSIVE à ses limites.

## Commandes réellement exécutées

Toutes les commandes suivantes ont terminé à zéro sur le candidat final :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel unit
ctest --test-dir out/build/windows-msvc-debug -N
ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error -j 1
ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error -j 1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer
```

Les deux CTest de run ajoutaient --output-junit vers chemins absolus uniques.
Découvertes JSON normales/ASan recoupées. Les deux presets ont auparavant été
reconstruits avec --clean-first --parallel1,69étapes chacun, pour éviter les
artefacts d'incrémentalité observés. Pas de campagne réexécutée pour les seuls
commits documentaires ultérieurs.

Contrôles documentaires : JSON/YAML, dates/source_revision, tags préservés,
fences et cibles wikilinks des dix notes, quinze empreintes source et cinq binaires,
manifeste et cinq sorties de commandes recoupés, git diff --check réussi.
Deux erreurs d'échappement du script d'analyse ont été corrigées par une version
sans échappements imbriqués ; aucune panne MCP ou résultat moteur n'en a été déduit.

QMD : node .opencode/scripts/update-wiki.mjs exit0, seulement artist-wiki,
10notes actualisées/48inchangées/0retirée. qmd embed -c artist-wiki exit0,
94chunks ; status MCP needsEmbedding0 et nouvelle note ADR-0004 retrouvée/lue
avec source3b1b102,581tests et ré-audit accepté. Aucune autre collection modifiée.

## Fichiers et preuves utiles

- API : ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp ; témoins
  et invalidation dans contexts communs/OpenGL, Freer et UniformReader.
- Contrats CPU : ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp.
- GL : ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp,
  PreparedFloatUniformRenderTests.cpp et ReflectedProgramGuardCrossTests.cpp.
- Usage : docs/usage/uniform-attribute.md et exemple local prepared-float-usage.md.
- ADR-0004/registre, photographie, principes, correction-plan, incidents, build
  et pointeur prochaine mission actualisés. ADR0004 reste Proposed/partial ;
  les autres statuts restent bornés comme avant. Aucune mission suivante sélectionnée.
- Preuve portable : docs/_meta/session-2026-10-27-prepared-float.json.
- Archive finale : out/qa-prepared-float/lead-followup-20261027/ ; manifeste
  SHA25629a4a2b5ac8a7a17a0dfd00de75d1a6fdf3861401b9312dfac00a1a7cfe4231b.
  Étapes579 et auteurs575/577 conservées séparément, pas réétiquetées581.

## Limites conservées et état de clôture

- Plusieurs tests lifetime ont été ajoutés après les gardes ; empreinte source
  RED initiale absente. Ni mutants ni GREEN ne reconstruisent ce passé.
- Contrat mono-thread/nonréentrant, fenêtre vivante, aucun relink/delete externe.
  Pas de retraite GPU ou RAII exhaustive, pas de garantie zéro allocation.
- detect_leaks0 ; Release/Linux/autresAPI/performance non validés. Le petit oracle
  pixel ne démontre pas une scène produit ou une suite d'images moteur complète.
- Dépendances Ninja vides et expériences avec binaire périmé/définitions inline
  mixtes explicitement exclues ; clean-first final, pas de correction toolchain.
- Récupération automatique des finales QA encore défaillante ; réponses natives
  réelles reçues/lues/persistées. Restriction native de la seule exception navigateur
  héritée du reviewer, aucun grant, correction de profil ou réparation de harness.
- Board recoupé6/6completed, aucune tâche active/pending ; audit hors board reçu
  et accepté. Équipe/worktrees/branches conservés, non archivés : le libellé natif
  long_run/running n'est pas présenté comme une tâche encore en cours ni comme
  une clôture runtime effectuée. Aucun push/purge/nettoyage forcé.
- Diffs étrangers non touchés ni committés ; renommage stagé et octets .grepai
  comparés à la sauvegarde initiale et préservés exactement. Aucun Docker créé.

## Repères techniques

Source finale3b1b1028dcd64ca46e0a359365ae096be1a38ac4 ; moteurH1
8b60b57898b28a7ceb7b5d4434aadcd4684581b9 ; consommateurH2
c283f806f93167b86ab0278d58714764873022ca ; photographie documentaire
e2813051b8f61f9091f8b5f5d1c20f18a5c37861. Politique native révision1 approuvée,
OpenAI uniquement vérifié aux admissions, aucun override de modèle/spawn.

[Ré-audit accepté](23-reaudit-accepte-et-photographie-bornée.md) ·
[Validation renouvelée](22-frontieres-detectees-et-validation-renouvelee.md) ·
[Contrat adopté](../contract.md).
