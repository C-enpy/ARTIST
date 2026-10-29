# Brief commun auteurs — pipeline uniforms

Entrees integrees : contrat .opencode/evidence/mission-uniform-pipeline/contract.md et plan qa-plan.md dans le meme dossier ; QA commitee en fab738eec2089335300ab4b24b7c21a6e7e9e9c0. Le HEAD de creation du worktree doit contenir ces fichiers. Les chemins ratifies du contrat remplacent les anciennes suggestions de fiches PM.

## Mandat ferme

Livrer pipeline Windows/Linux, pas campagne. Aucun changement ARTIST/include, API moteur, autre backend, profiler, nouvelle dependance, infrastructure ou secret. Aucun CMake/Conan/CTest, helperbuild reel, moteur, rendu, benchmark ou cloud execute. Les seuls tests executables sont Python et programmes factices appartenant aux tests. Le C++/GL est livre NON_EXECUTE, y compris compilation ; ne pas inventer un RED ou un PASS graphique.

Modele/provider/effort sont ceux de la politique Ensemble ; pas de contournement. Pas de sousdelegation. Profil QA prealable et contrat sont acquis ; leurs documents sont fournis localement, aucun acces aux sessions historiques requis.

## Premiere remise et TDD

Presenter un plan borne, les fichiers possedes, la premiere tranche de tests et la commande exacte avant implementation. Conserver le mode natif planned jusqu'a la transition autorisee de la tache ; demander au lead l'accord de plan si requis. Une fois la tranche de tests ecrite/commitee, remettre une reponse autonome « RED_READY » avec chemin absolu du worktree, commit et tests : le lead relit puis execute dans ce worktree, via Context Mode. **Ne pas implementer le code qui doit rendre ces tests verts avant que le lead ait observe le RED causal et donne la suite.** Apres observation, implementation minimale puis nouvelle remise « GREEN_READY » ; runner lead verifie GREEN/non-regression. Aucun test de perf mesuree n'est attendu.

La preuve RED doit etre une assertion discriminante sur le contrat, pas une erreur accidentelle d'import, une toolchain manquante ou zero test. Si un module/interface du core non integre manque a une autre branche, signaler cette vraie dependance : ne pas copier une branche non commitee ou fabriquer un RED d'environnement. Une remise de tests texte/patch est possible si l'edition native n'est pas encore ouverte. Les seules phases executees par le lead sont celles explicitement relues et autorisees. Ne pas auto-clore la tache : completion au lead apres integration.

Un seul ecrivain par fichier ; aucun raccordement au fichier d'une autre branche. Les tests peuvent employer doubles structurels et runners injectes pour isoler leur propre contrat ; ne pas creer de copie de model.py ou de schemas contradictoires. Type imports sous TYPE_CHECKING et composition structurelle sont possibles, sans forcer un type d'implementation. Les raccordements finaux sont a l'integrateur.

## Ownerships ratifies

Core : scripts/verify.py ; scripts/verification/__init__.py, model.py, catalogue.py, planning.py, runner.py, evidence.py, reporting.py ; scripts/verification/adapters/__init__.py, base.py ; scripts/tests/verification_core/.

Windows : scripts/verification/adapters/windows.py ; scripts/build-windows.ps1 si necessaire ; scripts/tests/verification_windows/. Pas de CMakePresets ni core. Conserver MSVC environnement pour enfants et restauration. Aucun appel reel PowerShell de build ; commandes sous doubles.

Linux : scripts/verification/adapters/linux.py ; scripts/build-linux.py ; scripts/linux-evidence.py si necessaire ; CMakePresets.json ; conan/profiles/linux-gcc ; scripts/tests/verification_linux/. Presets partages possedes uniquement ici. Release host/build coherent, garanties historiques llvmpipe intactes, materiel jamais infere par simple exclusion. Extraction interbranche attend integration.

Scenarios : ARTIST/verification/ et supports locaux ; scripts/verification/scenarios/ si utile ; scripts/tests/verification_scenarios/. Pas de ARTIST/benchmarks, pas de tests existants ni de CMake parents. Deux routes principales, standard library, shaders existants f2-single-values.vert/minimal.frag, protocole CLI a arguments avec resultats JSON. Remettre les selections precises des tests existants et instructions de raccordement au catalogue central a l'integrateur, pas d'edition catalogue.py ici.

Integration : pas encore admise ; raccorde producteurs effectivement integres, options CMake OFF par defaut, workflow exclusivement manuel et documentation. Gaps et audit final restent differes.

## Conditions de preuve

Les scripts ne doivent ni importer un workload natif ni utiliser un outil reel depuis un test factice. Les tests list/plan attrapent effets caches, bytecode et subprocess. Resultats code0 sans preuves, selection vide, doublon, stale, mauvais renderer, logs partiels et timeout sont discriminants prioritaires. Receipts et noms/populations doivent correspondre au plan. Le schema ratifie, les codes et bornes QA sont prioritaires. Seul le lead arbitre une divergence interbranche.

Tous les livrables sont isoles et commites par leur auteur dans le worktree natif. Rapports de remise autonomes (ne pas renvoyer seulement a une reponse finale precedente) ; indiquer limites et non-executions. Le lead integre via shutdown/merge natifs, pas fin d'auteur supposee integree.
