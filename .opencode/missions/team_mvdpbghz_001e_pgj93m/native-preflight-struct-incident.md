# Préflight Q2 — acceptation inattendue d'un input struct

## Candidat, exécutant et état

D/pool-6, tâche task_mvdy0o26_000s_jn4jct ; résultat retenu
result_mvdzqwmf_001e_9pl2bd, session ses_eb081586bffeMrgdWiIzsU9HZl,
message msg_14faadae50018pbqD9Fsox3oc1. Ce résultat de blocage ne termine pas D.
HEAD source1f1ad63, production ARTIST/include sans diff selon auteur ; seuls
préflights237lignes dans AttributeComponentTests.cpp et7shaders neufs.
Hash diff suivi rapporté67a071ce6650718cb95ff8ea327adf237ac286220411902c17ccc6dca19ced79
(ne couvre pas les shaders non suivis). Aucun RED fonctionnel ni correction.

Le setup court est opérationnel après l'A/B /Fo documenté séparément.
Baseline fraîche rapportée puis recoupée par lead sur les en-têtes JUnit :
251unit +48opengl =299exécutés,0failure/skip/disabled.
Artefacts S/logs/baseline-unit.xml et baseline-opengl.xml,
S=C:/Users/djden/AppData/Local/Temp/opencode/b-nj1cs8.
Ces299 tests caractérisent l'ancien comportement, pas le nouveau contrat.
Les empreintes des binaires de cette baseline restent à relever au registre D.

## Préflight positif rapporté par D

NVIDIA Corporation,RTX3090/PCIe/SSE2,OpenGL3.3.0 NVIDIA617.42,
GLSL3.30 NVIDIA via Cg compiler,profilcore1,contexte non partagé courant.
Limites attribs16,vertexoutputs128,TFinterleaved128,separateattribs4,
separatecomponents4,varying124. Première purge GLEW lue0 puisGL_NO_ERROR.
Alias GLM/GLint/GLuint/highp/i32/u32 identiques compilés.
qaArray[0] :length10,size1,type35665,location0,NUL0,MAX_LENGTH11.
qaArray2[0] :length11,size2,type35665,location0,NUL0,MAX_LENGTH12.
Les deux tableaux compilent/lient comme prévu par GLSL330.
Premier préflight1/1PASS,logSHA256
0fcdc9f1110652d4e0cc62a634cd6ac7de3cbb5442a746a4e1879964054ed1a3,
JUnitSHA256a64766298a69a67b289107eec790768555820d1f5ee9ef40f3cc7a12952d6815.

Second préflight,relus par lead dans S/logs/preflight-interface.xml :
12entrées a0..a11 actives aux locations0..11 et12varyings linkent ;
programme separate4 et programme vec3+mat4 linkent.
Identifier naturel300lettres lié et exactement réfléchi,length300,MAX_LENGTH301,
size1,type35665,location0,NUL0. APIbufSize4=>aaa,length3,NUL0,horsReader.
dvec3 sans extension refusé :C7532 version400/410 ou extensions fp64/attrib64bit.

## Échec conservé, non assimilé à un RED moteur

Shader vertex soumis sous version330core :
```glsl
#version 330 core
struct S { vec3 p; };
in S bad;
void main() { gl_Position = vec4(bad.p, 1.0); }
```
Attendu du préflight Q2 :compilation ou link refusé avant Reader.
Observé :vertexcompile1,fragmentcompile1,link1,logsvides.
ASSERTfatal attendfalse reçoittrue :
`Q2 stop: unexpected accepted vertex interface struct`.

Commande :ctest.exe --test-dir S/build -L opengl
-R AttributeQaPreflightTest.NativeInterfacesNamesAndRefusals
--no-tests=error --parallel1 --verbose --output-junit S/logs/preflight-interface.xml.
Exit8 ;1découvert/exécuté/échoué,0skip. Cas bool suivant NON EXÉCUTÉ.
Selon auteur,aucun Reader/Setter/draw/TF pour cette sonde,cleanup avant terminate.
LogSHA2561ba5ab296053200fee6604e096e6cea0957ed9fcec5c48cb220e39b6834d4669,
JUnitSHA2563bfdc20cc22f1e28ba2484109d4211d64463898e7aeb8a3af99e3d45db76cba8.
Le lead a recoupé le JUnit court77lignes,les logsstatuts et l'assertion exacte ;
ces empreintes demeurent attribuées au relevé D,sans assertion de recalcul lead.

La source primaire GLSL330rev6§4.3.4(p29/PDF35) exclut les structs d'inputs vertex.
Le succès du driver ne rend pas ce shader conforme à ce contrat de langage.
Cause de permissivité du compilateur NVIDIA non investiguée ici ;ne pas inférer
une extension activée ni annoncer support moteur struct.

## Arbitrage DH-REFUS requis

Conserver le FAIL du critère de conformité driver,ne pas le transformer en PASS
par modification silencieuse d'assertion/skip. Source de production inchangée.
Choix humain requis avant reprise :poursuivre le contrat moteur limité aux12types
en séparant cet écart driver et en excluant les structs GLSL de tout chemin
Reader/Setter/draw/TF du lot ;ou maintenir le blocage et exiger un environnement
de compilation conforme. En cas de continuation,le cas bool reste à exécuter
indépendamment,les12captures restent obligatoires,et le FAIL conformance demeure
visible dans le bilan. Aucune révision globale de GLSL/API/lifetime autorisée.

## Arbitrage humain obtenu

Choix explicite : « Poursuivre, écart driver conservé ».
Le lead a présenté le JUnitFAIL,la règle GLSL330 et l'absence de12captures à ce
stade. L'utilisateur autorise la poursuite du contrat moteur sur les seules
interfaces autorisées,sans utiliser une struct GLSL dans Reader/Setter/draw/TF.
Le cas bool doit être exécuté séparément ; cette décision ne présume pas son issue.

Le FAIL de conformité struct reste conservé avec son attendu/observé. Il ne doit
pas être transformé en PASS,skip ou assertion affaiblie. Pour continuer les
autres sondes,extraire ce contrôle en sonde/test de conformité indépendant,avec
le même oracle de refus,et en identifier explicitement classe/nom/sélection.
Les résultats moteur et conformité driver seront rapportés séparément ; ne pas
annoncer une suite globale PASS si le contrôle de conformité échoue.

Une éventuelle sélection fonctionnelle excluant cette sonde unique doit être
déclarée avant run (nom exact),avec comptes découverts/exécutés/non sélectionnés,
JUnit fonctionnel ET JUnit de conformité. Aucun échec de moteur n'est exclu.
Conserver aussi le JUnit initial de l'arrêt Q2 sans réécriture.
Ce mandat n'autorise ni support des structs par le moteur,ni suppression de la
fermeture des types C++,ni nouvelle dérogation pour bool/dvec3 ou autre condition.
Après bool conforme et autrespréflights satisfaits,D peut poursuivre son cycle
RED/GREEN/REFACTOR selon les contrats intégrés ; toute autre divergence=>arrêt.
