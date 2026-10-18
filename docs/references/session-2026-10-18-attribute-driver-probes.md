---
title: "Attribute-GL — retrait des sondes de conformité driver"
category: reference
tags: [artist, rendering, testing, reference]
summary: "Décision utilisateur : acceptation ARTIST limitée aux obligations moteur ; quatre contrôles de driver retirés, 326 tests Windows Debug passent sans exclusion."
created: 2026-10-18
updated: 2026-10-18
source_revision: f9c71a569c830681333a9b6466397324c3ceeffa
---

# Retrait des sondes de conformité driver

## Décision et cause

Le driver NVIDIA617.42 a accepté un input vertex struct GLSL330, alors qu'une
sonde de conformité attendait son refus. Cet échec historique reste archivé.
Mais cette sonde n'appelait ni Reader, ni Setter, ni upload ARTIST : son verdict
jugeait le compilateur du driver. L'utilisateur a demandé son retrait, ainsi que
l'audit des autres contrôles de même nature. Ce n'est pas une correction du driver
ni un support moteur des structs GLSL ; le contrat de12formes CPU reste fermé.

## Changements vérifiés

Seul fichier source :
`ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`.
29lignes supprimées : sonde struct, boucle des refus GLSL bruts bool/dvec3 et
troncature provoquée par glGetActiveAttrib avec bufSize4.
Un seul test CTest disparaît, les autres retraits étant des assertions internes.

Les capacités et préflights nécessaires aux preuves moteur restent présents :
arrays, nom long, matrices,12interfaces et transform feedback separate.
Les tests des refus et erreurs ARTIST restent : Shader::load, Reader/Setter,
types non supportés, cardinalité, payload et gel, fautes injectées CPU.
Pas de modification du moteur, des shaders ou de CMake. Aucun glslang ajouté.

## Preuves du candidat

Runner lead ; Windows x64 Debug, flux existant MSVC/Conan2/CMake.
HEAD f9c71a5 + seul diff source décrit, puis source hashée pour rattacher le run.
Build : `scripts/build-windows.ps1 -Configuration Debug -TestLabel unit -Jobs 1`.
Découverte CTest JSON avant/après :327→326, seul nom retiré
`opengl.AttributeQaDriverConformanceTest.StructVertexInputIsRejected`.
Runs complets labels unit et opengl avec `--no-tests=error --parallel 1`,
**sans exclusion de nom :266CPU+60OpenGL=326/326PASS,0skip/disabled**.
Revue test-reviewer fresh hors board : aucun finding statique établi ;
les exécutions sont celles du lead, pas celles du reviewer.

Artefacts : `out/attribute-gl/q3-driver-probes/` (logs,JUnits,découvertes,result.json).
Contrat et détails persistés :
`.opencode/missions/team_mvdpbghz_001e_pgj93m/q3-driver-probes-{plan,result}.md`.
SHA256 source :768d0a318a960f4174bc5bea111daef56e07a2d383d509202150669bc14f9f96.
L'ancien FAIL reste sous `out/attribute-gl/lead-1eb71ae/driver-conformance.xml` et
dans la note d'incident de mission ; aucun ancien résultat n'est réécrit en PASS.

## Portée

Ce résultat valide le retrait et la suite exécutée, pas la conformité du driver
ni la clôture générale de la mission. Image,performance,ASan,Release,autresOS/API
ne sont pas démontrés. Les [[adr/README|statuts ADR]] restent inchangés :
0003/0005 Accepted/partial,0008WindowsDebug seulement,autresProposed.
Voir [[development/build-and-test|Build et tests]] et le
[[architecture/correction-plan|plan de correction]].
