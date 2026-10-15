# ARTIST — reprise (2026-10-03)

Projet confirmé par get_current_config (ARTIST) et .wiki.json ; racine C:/Users/djden/source/repos/ARTIST. Vault versionné docs/, QMD artist-wiki. Ne pas activer un autre projet sur instance 1MCP partagée ni utiliser le vault global texturetonevariations.

Lire AGENTS.md, docs/engineering-principles.md, docs/adr/README.md, docs/architecture/correction-plan.md et docs/architecture/current-state.md. Français. Structure statique, contenu cooké, état runtime, dispatch amorti par lots ; pas de performance déduite des templates.

Mise à jour 2026-10-15 : code/build committé en 4d298425d7c0ecd87101409eb9ce78bac9b855de (base 887548b). Build existant et deux suites REJOUÉS avant publication : 95 unit + 27 OpenGL = 122, failures/skips/disabled 0 ; dont 14+5 lifetime. Preuves nouvelles : docs/_meta/precommit-2026-10-15-validation.json. Windows x64 Debug MSVC seulement, pas clean install/Release/ASan. Baseline initiale 109/109 (87+22) conservée. Vérifier git HEAD/status/remote à la reprise pour l'état exact de publication.

ADR-0003 Accepted/partial, ADR-0005 Accepted/partial, ADR-0008 Accepted/verified dans sa baseline Windows Debug ; ADR-0001/0002/0004/0006/0007 Proposed/not-started. Pas de PASS global, pas ASan ni Release/Linux/CI distante/images/benchmarks.

Sources durables : docs/references/session-2026-10-03-shader-pass.md (conversation/sous-agents attribués, pas export verbatim), docs/development/known-incidents.md, docs/_meta/session-2026-10-03-validation.json (commandes/hashes JUnit/logs). Ne pas recréer des faits à partir du seul HEAD. Les preuves locales out/ sont mutables ; hashes ne les archivent pas.

Utilisateur : rien en production, pas de consommateur externe -> migration franche sans shim. ASan demandé dans arbre séparé avec instrumentation prouvée, sans dépendance Windows-only. QMD configuration standard console ; CUDA marche selon utilisateur ; pas de forçage CPU durable. Travail bloquant en background, un seul écrivain/build par arbre. Demande conditionnelle astra max, modèle non sélectionné dans le worker historique.

Mission sélectionnée le 2026-10-15, NON COMMENCÉE : docs/architecture/next-mission.md. Reproduire puis corriger l'assignation move vers Shader/Pass déjà chargé (cleanup destination une fois, transfert, moved-from/self-move, contexte partagé/noexcept), tests mock et driver, revue indépendante, puis expérience ASan séparée avec instrumentation prouvée. ASan ne prouve pas la libération des handles driver. Pas de migration Pipeline ni de refonte transactionnelle reload dans cette mission. Voir `mem:architecture/shader-pass-adr0003` et `mem:engineering/incidents-2026-10-03`.


## QMD : mise à jour pendant ingestion
Configuration standard : qmd embed -c artist-wiki a réussi (code0, 38 chunks/10 documents, 6s) après mesure RAM disponible10802MiB et commit disponible10502MiB. Vsearch ciblé retrouve ADR-0003, search/get retrouvent nouvelle synthèse ; vault24 notes. Log out/logs/qmd-session-ingest-embed.log. Aucun forçage CPU ni restriction CUDA. Ce succès ne démontre pas la cause précise des erreurs précédentes.
