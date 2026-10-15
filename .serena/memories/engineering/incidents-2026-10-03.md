# Incidents et diagnostics corrigés ARTIST (2026-10-03)

Détail : docs/development/known-incidents.md ; chronologie/sessions : docs/references/session-2026-10-03-shader-pass.md ; empreintes : docs/_meta/session-2026-10-03-validation.json.

1. Heap corruption/dialogue CRT + abort/hangs : fixtures globales au même nom avec définitions divergentes entre TU -> ODR. Renommées ShaderLoaderTests/PipelineUserTests/PassFreerTests. Fixture partagée dans même header légitime. Purecall séparé : destruction base ne redescend pas dans override pur ; free déplacé/maintenu dans dtor dérivé. Attribution chaque hang au dialogue CRT rapportée, pas prouvée exhaustivement. Suites finales vertes, pas certification toutes causes.

2. FAUSSE PISTE À NE PAS REPRODUIRE : « GMock sans default action fabrique une référence pendante » est faux. GMock1.14 gmock-spec-builders.h DefaultValue<Result>::Exists -> runtime_error/arrêt selon config ; DefaultValue<T&> requiert référence définie. ReturnRef devient dangling si objet réel meurt. MockShader getContext supprimé pour utiliser contexte réellement possédé, mais ne pas attribuer toute corruption à l’absence d’action par défaut.

3. C1060 compiler heap : out/logs/build-guards.log, pression mémoire/commit Windows ; compilation séquentielle réussie ensuite. Mesurer committed/commit limit ET RAM. Ne pas répéter builds après deux échecs sans hypothèse nouvelle. Un seul build --parallel 1. GCC MSYS2 13.2 sans stacktrace = précondition toolchain, pas bug moteur.

4. Tests adaptés sans affaiblissement : context unique_ptr non copiable -> move ; PassAccessorsOnLiveObject oubliait addShader -> count0 ; test d’ordre destructor avait IDs seedés puis load -> rejet B, laisser mock glCreate produire IDs. AttributeReader.hpp inclut composants de son instanciation concrète, pas dépendance à include-order consommateur.

5. ASYNC : run-build.py bufferise sortie, mais notification outil background peut bien arriver. Ne pas poller/attendre par sleep. Worker completed peut encore avoir commande en cours ; recouper logs. Pas de deux builds concurrents ; identifier PID/parent/commande orphelin avant arrêt, pas kill tous processus homonymes. Reboots serveur/sorties tronquées ne sont pas preuves de succès.

6. QMD : artist-wiki* était faute de commande. qmd embed -c artist-wiki (standard) a loggé CUDA NoBinaryFoundError -> CPU puis allocation538443776 refusée. Cause spécifique sandbox non établie malgré affirmation passée. CUDA fonctionne selon utilisateur en console et embedding standard précédent réussi. Pas de politique CPU permanente. MCP QMD non exposé lors ingestion -> CLI search lex + get, artist-wiki uniquement, intent explicite de réconciliation. node .opencode/scripts/update-wiki.mjs cible collection ; pas qmd update -c, pas .qmd local, pas mutations index concurrentes.

7. Suivi reviewer ouvert : F4 getShader(index) non borné ; F5 &infoLog[0] sur log0 ; F6 shared Pass context ; F7 UniformReader absent concept ; F8 fichier absent (badbit seul) -> erreur compilation trompeuse ; F9 gardes davantage testées mais pas exhaustives ; F10 glUseProgram(0) valide, désarme programme (contrat à décider). F11 faux positif : shared_ptr d’objet non copiable est valide. Risque hors conclusion reviewer : move-assignment owner chargé sans native free, à reproduire prioritairement.

ASan demandé, jamais exécuté ; expérience séparée, instrumentée, pas Windows-only. Images, benchmark, Release/autres plateformes/CI distante absents. Source current resume : `mem:project/artist-reprise` et `mem:architecture/shader-pass-adr0003`.


## QMD : mise à jour pendant ingestion
Configuration standard : qmd embed -c artist-wiki a réussi (code0, 38 chunks/10 documents, 6s) après mesure RAM disponible10802MiB et commit disponible10502MiB. Vsearch ciblé retrouve ADR-0003, search/get retrouvent nouvelle synthèse ; vault24 notes. Log out/logs/qmd-session-ingest-embed.log. Aucun forçage CPU ni restriction CUDA. Ce succès ne démontre pas la cause précise des erreurs précédentes.
