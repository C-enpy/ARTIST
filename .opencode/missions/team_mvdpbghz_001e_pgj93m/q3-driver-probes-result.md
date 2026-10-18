# Q3 — résultat vérifié du retrait des sondes driver

Plan persisté avant édition : commit f9c71a5, q3-driver-probes-plan.md.
Proposition QA pool-11, candidat result_mve2xqm2_000f_9lxe5p accepté après
persistance ; auteur et runner du retrait : lead. Aucun agent QA n'a édité
ou exécuté les tests. Évaluation PM/LLM source :38→70,risque38.

## Diff et portée

Source : ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp.
Diff exact contre la base :0ajout,29suppressions dans ce seul fichier source.
Pas de production, CMake, shader, test unitaire ou mock changé. git diff --check
réussi. Suppressions : test/classe struct, boucle bool/dvec3, probe bufSize4.
Les trois dernières sondes sont des tranches internes : un seul nom CTest retiré.
Nom NativeInterfacesNamesAndRefusals conservé pour limiter le delta de découverte ;
son corps restant vérifie uniquement le setup valide des oracles moteur.

Conservés : capacités/profil/alias, spelling arrays, matrices, nom long naturel,
12interfaces/TFseparate ; tous Reader/Setter/upload/TF/payload/gel ; propagation
des erreurs Shader::load et cleanup ARTIST, négatifs CPU injectés.
Les échecs driver historiques ne sont ni supprimés des artefacts ni requalifiés.

## Commandes réellement exécutées par le lead

- Avant et après rebuild : ctest --test-dir out/build/windows-msvc-debug
  --show-only=json-v1, snapshots discovery-before.json/discovery-after.json.
- powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
  -Configuration Debug -TestLabel unit -Jobs 1 : exit0,266/266unit PASS.
- ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure
  --no-tests=error --parallel 1 --output-junit
  C:/Users/djden/source/repos/ARTIST/out/attribute-gl/q3-driver-probes/unit.xml.
- Même commande label opengl, sortie opengl.xml ; aucun -E/-R/filtre de noms.

Résultats explicites après rebuild :266unit+60opengl=326/326 PASS,
0failure,0skip,0disabled. Tous les tests découverts sont exécutés.
Ensemble découvert avant327/après326 ; différence unique :
opengl.AttributeQaDriverConformanceTest.StructVertexInputIsRejected.
Aucun nom ajouté, ensemble unit identique, aucun DISABLED_ de substitution.

## Identité et preuves

HEAD de run :f9c71a569c830681333a9b6466397324c3ceeffa + le seul diff source décrit.
SHA256 source compilée :
768d0a318a960f4174bc5bea111daef56e07a2d383d509202150669bc14f9f96.
Artefacts :out/attribute-gl/q3-driver-probes/ ; manifeste result.json SHA256
72c8ed1c8429334cd63574227d0bdce8e83870d21911f0df4f728d6375213d14.
JUnit unit SHA256 :7d816402d10fca7ef2c429667fd5fba987b468d8dfc00d906b7b89d33841c3fa.
JUnit opengl SHA256 :5a91ae169cf4584ada7a492da5ea2e5d4ae02b499deadbd87f17661140d9421a.
Exe unit :0e9cde86e8abbae0bf8e784d3875fbd90cdfe8630f61ff0100b29813f30fc404.
Exe GL :11d620d05b9e6dda8832707725548de7ae49c0c861bd5d8f45642d90db6fc564.
Source vérifiée inchangée pendant build. Manifeste conserve les commandes complètes.

## Revue indépendante

test-reviewer fresh hors board, session ses_eb0019d62ffeU9ojVycGXJn7aE,
scores PM/LLM48/48,risque42, lecture seule sans outil d'exécution ni édition.
Verdict : PASS statique borné, aucun finding établi. Les scénarios discriminants
conservent nom300, int16777217 via INTEGER/TF, VBO leurre, array size1 et freeze.
Le reviewer confirme l'état présent, pas le diff Git ni un run indépendant.
Le lead ferme séparément ces limites par comparaison du diff et des ensembles
CTest et par les runs ci-dessus. Aucun correctif post-revue, donc pas de
ré-audit de correction nécessaire. Pas de mutation exécutée revendiquée.

## Limites

PASS de ce retrait et des326tests Windows Debug sélectionnés par labels entiers,
pas certification de conformité NVIDIA ni clôture globale de la mission Attribute.
Reproduction historique struct :1FAIL toujours conservé sous
out/attribute-gl/lead-1eb71ae/driver-conformance.xml et dans
native-preflight-struct-incident.md ; ce n'est plus un critère d'acceptation courant.
Pas image/performance/ASan/Release/autreOS/API. ADR inchangés.
Les modifications externes harness/docs/profils/AGENTS/.grepai restent hors commit.
