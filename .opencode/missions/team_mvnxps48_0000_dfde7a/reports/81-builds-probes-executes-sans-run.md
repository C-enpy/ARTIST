# 81 — Deux builds exécutés, aucune caractérisation encore lancée

2026-10-26. CommandesCOMPILE intégrales80 approuvées avant invocation, manifestes/SHA/sourcesd03f996 et56296fe réellementintégrés. Runnerleadseul, stdio puisclose en série, mêmesoutilsDebugpinnés, sorties/logs neufs. Source/configavantaprès inchangées, stderrwrappervide.

## Résultats réels

- **Stdio**,18:19:13–18:19:16Z, tagqa-f8-stdio-cal-20261026-01-build : compile/link **exit0**, aucunwarning/error. exeSHA **8ba7972b00f0faa8333701cc9dc8782b0e97ccb46c7396ef7095e5a823cd494b**, buildlogSHA **25d4f66c7c01d5b35c220cb97ffdd1608405f4ab45160497e6cc7ea3079676cf**.
- **Close**,18:19:16–18:19:17Z, tagqa-f8-close-cal-20261026-01-build : compile/link **exit0**, **warningC4244 conservé** xutility4813, instanciationclose_probe.cpp209 wchar_t→char lorsconversiontoken. exeSHA **e4eb246151ce488c893f6e24c048cd9137f146048334d2d3d86302c690a34f0b**, buildlogSHA **c0df0095413e2773c2032267b552c689efe736a7ce7d02e9dab034de90553210**. Le contratCLI borne lestokensASCII ; warning signalé pour revue, pas effacé/corrigéspéculativement ou renvoyé à un reruncosmétique.

Logsbruts/copiesstdoutstderrcommandes/hours/codes/exe+sourceSHA danschaque out/qa-f8/{stdio,close}-cal-20261026-01/build-result.json. stdoutstderrcompilateurfusionnésdocumentés, octetsOEMlusCP850pourdiagnostics. AucunCMake/Conanconfigure/install, nouvellelib ouoptionpermissive. Deux builds≠tests, pas TDDcausalparent accompli.

## Gates après build

BuildPASS borné compile/link pourchaquecandidat ; aucunexecutableprobe/contrôlePS/CLI lancé parcesinvocations. ParentcloseRUN attend revuefresh79 et protocoleexactbail/identité/imports/manifestepre-run. WarningC4244 n'est pas unéchec maisrestevisible, indépendammentdepreuvesmanquantes. NativeCRTretour/exception/consommation/processcleanup toujoursnonmesurés.

Stdio peut préparer ses3casfigés en séquence sousrunnerlead, après argv/sélections/vecteurs/nomsACP/locale/binaire/sentinellepréenregistrés ; sortieM0défavorable/inconclusive n'estpasbugàcorrigerpourvert, touteerreurinstrumentationarrêteavantrepetition. Revuefreshsensiblestdio avantacceptationfinale, aucuneautoredémonstrationASan555/GPU/Reader/catch/nonpublication.

PC1globalnoncomplété. S_PROBE_SOURCE peutêtreactéecommelivrable **sourceintégréeetbuildée**, pas«calibrationterminée». OwnershipauteursenHOLDpour findings éventuels, passhutdown/libérationd'un travailencoursni nouvelleanalysehistorique.
