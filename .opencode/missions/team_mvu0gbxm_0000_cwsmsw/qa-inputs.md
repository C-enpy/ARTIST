# Entrees du plan QA — fournies par le coordinateur

Ce document fournit le contexte ARTIST necessaire au profil QA restrictif. Il ne requiert ni QMD, ni Serena, ni shell, ni chemin externe, ni acces aux sessions precedentes. Le contrat ratifie sera fourni separement apres sa remise complete.

## Mandat et perimetre

Politique native approuvee revision 1, OpenAI uniquement, long run, QA proportionnee. Livrer pipeline et scenarios reels pour qualification ulterieure Windows/Linux. Executions autorisees : tests du pipeline sur donnees synthetiques et processus factices. Interdits : compilation moteur, suites moteur, rendu, benchmark et cloud. Ces interdictions s'appliquent aussi aux commandes de validation qui lanceraient indirectement ces programmes.

Les profils QA rendent propositions textuelles et verdicts au coordinateur par final_response. Aucun edit, shell, API HTTP, delegation ou outil d'equipe requis. Les chemins du present dossier sont locaux au projet. Une lecture autorisee et une restitution doivent etre constatees pour qualifier l'admission ; un refus ne doit etre essaye qu'une fois.

## Etat source connu

HEAD avant contrat : fcf14a125e6b8e77e7ae65bcd4de2f7d654a56c6. Le diff preexistant des profils d'agents, des rapports d'une autre mission et de .tmp est exclu de cette livraison. Le futur brief indiquera le commit contenant le contrat. Aucun auteur ne doit supposer un transfert de fichiers non commites.

ADR-0001/0002/0004/0006/0007 Proposed ; ADR-0003/0005 Accepted partial ; ADR-0008 baseline Windows Debug verifiee seulement. Tranche float scalaire adoptee localement, pas adoption globale des layouts ni du protocole performance. Les comptages historiques ne qualifient pas le nouveau candidat.

## Trajet code recoupe par le lead

- UniformReader.hpp : la reflexion appelle glGetUniformLocation et memorise la location pendant la preparation.
- Pass.hpp : withUniform(name, value) cherche l'uniform puis appelle son set<T>.
- PassContext.hpp : getUniform fait contains puis at, soit deux recherches de nom sur succes.
- Uniform.hpp : validate avant commit du miroir std::any, puis Setter::on qui valide a nouveau.
- Setter.hpp float : glUniform1f avec valeur obtenue par getValue<GLfloat>/any_cast.
- PreparedFloatUniform.hpp : prepare apres link/reflexion ; set verifie deux temoins faibles et generations, contexte et programme courant, commit std::any, puis glUniform1f sans recherche de nom ni any_cast. Les gardes ne sont pas supprimees pour mesurer.

A/B compare donc plusieurs mecanismes et ne prouve pas l'effet isole du cache natif, deja present.

## Oracles existants recoupes, non reexecutes

PreparedFloatUniformRenderTests.cpp : FBO RGBA8 lineaire 32x32, triangle, GLSL330 ; valeurs 0 et .5 deplacent le triangle. Chaque route satisfait un oracle fixe sur des pixels loin des bords, puis egalite A/B et difference entre valeurs. Miroir CPU et readback natif verifies. Logs, assertions, glReadPixels et glGetUniform sont actuellement dans le test : hors fenetre chronometree dans le futur banc.

Tests deja presents pour mauvais programme, absence de contexte, retrait/revocation et ancienne identite numerique reutilisee. Ces scenarios sont a reutiliser/reference, pas a reexecuter dans cette mission. Un nouveau C++ graphique est livre NON_EXECUTE et revu statiquement ; aucun RED/GREEN graphique invente.

## Risques et preuves attendues

1. Faux resultat : selection vide, omissions/doublons, valeurs figees, travail different, rapport partiel presente vert. Oracles synthetiques independants et scenarios graphiques discriminants livres.
2. Provenance : source, shader, configuration ou executable modifie/périmé ; identites avant/apres et coherence d'artefacts, pas transactionnalite exhaustive promise.
3. Environnement : CPU sans GL distinct d'OpenGL logiciel et materiel ; pas de fallback silencieux ; une mesure CPU des uploads OpenGL exige aussi GL.
4. Effets implicites : list/plan ne creent aucun dossier/cache, ne lancent aucun enfant ni reseau et ne chargent aucun workload ; prepare/build/run sont explicites.
5. Erreurs : retours enfants, timeout et interruption conservent logs disponibles et verdicts distincts. Resultat nul/negatif valide ne devient pas erreur d'outil ; mesure absente reste absente.

Runner reel des tests synthetiques : coordinateur. Les auteurs livrent tests et preuves/propositions ; attribution auteur/executant claire. Les commandes exactes seront celles des fichiers integres, pas des noms fictifs supposes existants. Aucun nouveau banc generique, profiler ou infrastructure n'est exige par le niveau QA.

## Restitution attendue

Plan proportionne avant auteurs : exigences explicites/inference, nominal/negatif, matrice I et P chacun 1–5 (sans rapport aux scores PM), top risques avec declencheur/portee/detection/mitigation/gap, obligations vers preuves et preuves vers obligations, conditions d'arret et limites. Reutiliser le travail QA anterieur fourni dans le brief, sans obligation de relire toute la suite ou de demander des moyens interdits.
