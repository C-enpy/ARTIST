# G — renforcements des tests Reader intégrés par le lead

Proposition : test-engineer/pool-9, tâche `task_mve1nuav_0005_gfolf7`,
candidat natif `result_mve1soct_0009_bn2d36`. Lecture seule, aucune exécution
ou édition par G. Base moteur `1eb71ae`. Scores PM/LLM : brut64/retenu70,
risque58. Application et exécution : lead, mandat de runner distinct.

## Décisions et oracles avant application

Trois MODIFY acceptés, dans le seul fichier
`ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp` :

- G-1 : nom vide, longueur0 et NUL cohérents : INVALID_NAME sans publication.
  Discriminant proposé : mutation `length <= 0` en `length < 0`.
- G-2 : doublon aux locations successives0/2 : DUPLICATE_NAME et première
  location0 conservée. Le second lookup reste facultatif : rejet précoce autorisé.
- G-3 : helper `reflectedInput` attend exactement ACTIVE_ATTRIBUTES et
  ACTIVE_ATTRIBUTE_MAX_LENGTH, au lieu d'accepter tout pname comme maximum.

Aucun code de production, shader, référence ou tolérance modifié. KEEP pour
les matrices CPU/TF, arrays, noms longs, reload, priorités et freeze existants.
REJECT pour skip/masquage du FAIL struct ou extension hors contrat.
DEFER pour résultat dynamique du cleanup fatal et lifetime GPU global.
Les mutations sont des scénarios proposés, NON exécutés : pas de RED revendiqué
pour ces compléments QA. Ils ne remplacent pas le RED de l'auteur D.

## Exécutions lead

Commande build :
`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration Debug -TestLabel unit -Jobs 1`.
Premier essai : compilation et lien réalisés, découverte GoogleTest interrompue
par timeout5s. Échec environnement/découverte, pas RED comportemental.
Expérience discriminante : même binaire `--gtest_list_tests`, exit0 en60ms,
sortie13147octets. Nouvelle exécution incrémentale de la commande build, sans
modifier délai ni flags : exit0,266/266 unitaires PASS.

Puis CTest depuis `out/build/windows-msvc-debug`, options communes
`--output-on-failure --no-tests=error --parallel 1 --output-junit <artefact>` :
- `-L unit -R ^unit[.]AttributeReaderTests[.]` :10/10 PASS ;
- `-L unit` :266/266 PASS.

Zéro skip/disabled/échec dans les deux JUnit. Correction du comptage proposé
par G : le filtre Reader découvre10 tests, pas12 ; les cas internes supplémentaires
ne sont pas de nouveaux noms. Aucun faux comptage de12 tests exécutés.
Logs et JUnit : `out/attribute-gl/g-run/`.
SHA256 Reader JUnit :
`4fa54a5c523aa26accd447179d78035ca81648d7982f1661d24ccc1d429ac603`.
SHA256 unit JUnit :
`55d35399b400f2f07d249b3451806a8c5b7c96a50df9cb15117747396dba77e4`.
SHA256 fichier tests après application :
`73743c65b4336270d43ef95fcba03098064f0f0eeb2187033c6b659af5b3c2c6`.

## Limites et suite

L'exécution GL du lead sur D intégré, avant ce patch CPU, reste distincte sous
`out/attribute-gl/lead-1eb71ae/` :266unit+60GL fonctionnels PASS,
conformité struct1FAIL,327 exécutés sans skip. Captures fraîches reconstruites
depuis les payloads du contrat et comparées par lead :120octets+16sentinelles
et separate4/4/4/16+16sentinelles chacun conformes.
Ces exécutions précédentes ne sont pas annoncées comme une relance GL après G.

R a déjà terminé sa revue indépendante ; restitution complète demandée après
compaction pour récupérer précisément son scénario payload après refus GLSL.
Ce complément éventuel doit être intégré avant audit fresh T hors board.
Conformité NVIDIA struct reste FAIL conservé suivant arbitrage humain ; aucun
PASS global, image/performance/ASan/Release/autreOS/API déduit.
ADR0003/5 restent Accepted/partial,0008 Windows Debug seulement,
0001/2/4/6/7 Proposed. Modifications externes du harness/index préservées.
