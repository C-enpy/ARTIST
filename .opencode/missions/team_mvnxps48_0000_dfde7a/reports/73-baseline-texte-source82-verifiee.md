# 73 — Baseline texte : extracteur production exact accessible sans mutation

2026-10-26. Réponse au gate final de72. Lecture/symboles lead seulement, aucuncode/essai/sourceproductionmodifié.

## Localisation et identité

Serena skill/initial_instructions/get_current_config : projetcanonique ARTIST confirmé, LSPcppready, pasactivate/reconfiguration. find_symbol ciblé `ARTIST/include/graphic/opengl/pipeline/component/shader/Reader.hpp` pour extractShaderSource puis readShaderFile a rendu leurs corps. Lecture fichiers limitée aux headers/namespace/préconditions pertinents. LSPready n'est pas une compilation de ce probe.

Reader.hpp canonique **et worktreepool8** comparés au blob du pinmoteur82bf52d52e594fae02593866669a1af715127d1f : mêmesblob **63bfcbf4ae7d6b983b05fa4a19e3329f8f021a43**, SHA256 **ce97d9e3ecfcf163ab757bb9c1dbf29116fb52505443c17a8725da5c0a90a284**. Cela établit la mêmeversiondufichier, pas exécution.

## Corps et voie d'appel

Fonctioninline namespace `artist::graphic::opengl::pipeline::component::shader::detail::extractShaderSource(std::istream&)`, lignes30–52, accessible depuis le header (pas membrefonctionprivée, pas macroprivatepublic/friendnouveau). Code : stringcode/charbuffer4096, while(source.read(buffer,sizeofbuffer))appendgcount ; appendderniergcount ; si fail&&!eof lanceTraceableExceptionruntimeerror ReadingstoppedwithoutEOF ; returncode. Précondition documentée : streamvivant, masquebadbitactif, fonction ne ferme/retient/modifiemasque ni contexte.

readShaderFile65–112 utilise ifstreamlocal, exceptions(badbit), open(context.getShaderPath()), extraction,puisclear/close/failcheckavantpublication. La baselineprobe peut **inclure Reader.hpp et appeler directement cette fonctionproduction** sur ifstreamtexte frais/narrowpathACP, badbitactif, mêmeconventiondechemin. Pasnouvellecopiedeboucle présuméeéquivalente ; pasappeltransaction/Loader/contextepublication, sourceproductioninchangée. Snapshotslectureavantcleanup et closeowner séparés ; nettoyage naturel du résiduEOF ne devient pas injection d'erreur ni preuvecloseéchoué.

## Transmission et limites

Digestcorps/masque/namespace/identité fourni à pool8 par message, référencesrelativesReader.hpp déjàprésentesdanssonworktree : aucunelecturecanoniqueexterne nécessaire. Complémentcourtdemandé pour confirmervoie/accessibilité/préconditions, pasrefairematrice72/rechercheshistoriques. **GOsourcepasdonné.** RevueTDD/instrumentation descontrôlesneufs et adoptionmatrice/issues restent à traiter ; cette analyse baseline ne les dispensepas.

Impact : oraclecomparaisoncompatibilitétexte du futurprobe seulement, sansmodifAPI/contrat/ownershipproduction ; lesmodestexte/binairesReader et migration restent nonadoptés. AucunepreuveGenuinecatch/close/nonpublication,moteur/GPU/perf/ASan nouvelle.
