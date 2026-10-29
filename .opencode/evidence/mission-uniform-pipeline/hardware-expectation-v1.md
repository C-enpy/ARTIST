# Precision ratifiee — cible OpenGL attendue

Decision locale du lead dans le mandat approuve, sans changement de modele, QA, backend ou autorisation d'execution. Complete le contrat v1 sur la compatibilite materielle attendue. Proposition source uniform-scenarios 60f32c52102f7a5df38116147a458638aecdfbbc.

## Champs et transport

Request ajoute deux champs optionnels : expected_gl_vendor: str|None=None et expected_gl_renderer: str|None=None. Ils sont fournis ensemble, non vides, sans NUL/controle et de longueur <=4096 caracteres chacun ; admissibles uniquement pour le profil opengl-hardware. Pas de cible devinee ni de variable d'environnement cachee.

Les anciens dictionnaires Request restent valides. to_dict omet les deux champs si tous deux absents afin de conserver les representations et hashes preexistants sans cible ; s'ils sont presents, les inclure tous deux dans le JSON canonique du plan et donc dans son hash. from_dict/construction directe appliquent la meme validation. Aucun test preexistant affaibli.

CLI Python : --expected-gl-vendor TEXT et --expected-gl-renderer TEXT pour la selection materielle. plan/list restent sans effet ; plan materiel sans cible est imprimable mais porte la precondition unresolved. run materiel sans cible est bloque avant execution de scenario. Une paire partielle est une demande invalide, pas une observation ambigue.

build_workload_argv ajoute les kwargs optionnels expected_gl_vendor et expected_gl_renderer. Sans paire, les argv historiques restent identiques ; avec paire, append les flags --expected-gl-vendor et --expected-gl-renderer aux argv GL. Les valeurs restent chacune un argument, espaces compris. Pas de flags CPU/logiciel supplementaires.

Le parseur natif reconnait ces deux options strictes, refuse doublons/paire partielle/profil incompatible, et conserve la cible declaree ainsi que les chaines observees dans environment du resultat. Les quatre champs parameters du protocole precedent restent inchanges : la cible est deja liee a l'identite via plan_sha256 et les receipts ; aucune nouvelle cle top-level n'est ajoutee au JSON resultat.

## Observation et gardes

Creer/verifier le contexte 3.3 core courant, recueillir vendor/renderer/version/capacites observees AVANT programme/upload/draw. Pour hardware, exiger la paire declaree et une egalite exacte avec les chaines observees. Refuser un renderer logiciel connu meme si la cible declaree lui correspond ; absence, discordance ou ambiguite => UNVERIFIED, code20 natif, INCONCLUSIVE, aucun upload/draw ni mesure. Le collecteur compare aussi la cible du plan aux observations conservees, sans se contenter du code enfant.

Cette correspondance attribue la cible demandee/observee dans un environnement maitrise. Elle ne prouve ni une acceleration physique universelle, ni une capacite avancee deduite du nom, ni une performance. Les tests CTest historiques ne consommant pas cette CLI doivent beneficier d'un controle de contexte equivalent avant leur campagne future ; aucune qualification materielle basee seulement sur l'absence de llvmpipe.

## TDD et integration

Nouveaux controles synthetiques avant GREEN : paire nominale et espaces preserves, omission compatible, paire partielle/nonchaine/vide/controle/trop longue rejetee, mauvais profil refuse, cible participant au hash/receipts, run bloque sans cible, observations synthétiques absentes/differentes/logicielles refusees. Les tests existants restent inchanges. C++/GL toujours revu statiquement, sans compilation/execution.

Ownership : modele/planning/CLI au core ; protocole argv/producteurs aux scenarios ; transport phases/env aux adaptateurs selon leurs interfaces ; raccordement final seulement apres integration des producteurs. Aucun fichier d'une autre branche copie ou edite pour appliquer cette precision.
