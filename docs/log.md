---
title: Journal documentaire ARTIST
category: journal
tags: [artist, documentation]
summary: Historique append-only des transformations et mises à jour de la documentation ARTIST.
created: 2026-10-03
updated: 2026-10-15
---

# Journal documentaire

- [2026-10-03] INIT vault_path="docs" collection="artist-wiki" config=".wiki.json".
- [2026-10-03] SOURCES preserved=4 location="research" hashes="_meta/research-sources.json".
- [2026-10-03] RECONCILE sources="code@887548ba8a2839daf55ca5636f1519110b4624a7 + R1..R4" corrected="Readme.md,GUIDELINE.md,CONTRIBUTING.md,engineering-principles.md".
- [2026-10-03] ADR_CREATE range="ADR-0001..ADR-0007" status="Proposed" implementation="not-started".
- [2026-10-03] TAG_INIT vocabulary="_meta/taxonomy.md".
- [2026-10-03] DOC_CHECK notes=21 wikilinks=127 broken=0 orphan=0 source_hashes=4 result="PASS".
- [2026-10-03] QMD_REGISTER collection="artist-wiki" scope="docs/**/*.md" indexed=21 lexical_retrieval="PASS" maintenance="node .opencode/scripts/update-wiki.mjs".
- [2026-10-03] QMD_EMBED collection="artist-wiki" result="INCONCLUSIVE" reason="native CUDA load exit=9; CPU single-context timeout=240s" details="_meta/wiki-maintenance.md".
- [2026-10-03] QMD_EMBED_RESUME collection="artist-wiki" result="PASS" mode="CPU, 1 context, 1 document/batch" remaining_completed=17 vector_query="PASS via SDK, artist-wiki only".

Navigation : [[index|Accueil]] · [[_meta/wiki-maintenance|Maintenance]] · [[adr/README|ADR]].

- [2026-10-03] BUILD_BASELINE platform="Windows x64 Debug" compiler="MSVC 19.44" conan="2.12.2" tests="109/109 (87 unit + 22 OpenGL)" result="PASS" evidence="_meta/build-validation.json".
- [2026-10-03] ADR_UPDATE accepted="ADR-0005 partial, ADR-0008 verified Windows Debug" reason="Conan 2 exact dependencies installed and reproducible script executed".
- [2026-10-03] TAG_AUDIT tags_normalized=0 unknown_tags=0 pages_modified=0 notes=22.
- [2026-10-03] QMD_GUIDANCE mode="standard console configuration" source="user confirms CUDA works" removed="permanent CUDA restriction and CPU-forcing procedure".
- [2026-10-03] QMD_REFRESH collection="artist-wiki" notes=22 result="PASS" mode="standard" embedded_documents=9 chunks=19 seconds=11.5 vector_query="PASS via MCP, ADR-0008 and build guide retrieved" evidence="out/logs/qmd-final-embed.log".
- [2026-10-03] ADR_UPDATE adr="ADR-0003" status="Accepted" implementation="partial" scope="migration franche tranche Shader→Pass (rien en production)" reason="ownership unique_ptr + emprunts références exécutés et vérifiés".
- [2026-10-03] LIFETIME_IMPL scope="Shader→Pass" owner="IShader/Sont PassContext containers" signatures="on(Context&)" shared_removed="Pass::shared()" result="PASS".
- [2026-10-03] TEST_SUITE labels="unit=90, opengl=26" total=116 failures=0 skipped=0 timeouts=0 result="PASS" evidence="ctest-unit.xml, ctest-opengl.xml, ctest-unit-v.log, ctest-opengl-v.log".
- [2026-10-03] INCIDENT_FIX symptom="heap corruption C0000374 + purecall R6025 + hangs 60s" root_cause="ODR fixture renames (LoaderTests→ShaderLoaderTests, UserTests→PipelineUserTests) + base-dtor pure override + mock getContext reference-return" fix_documented=wiki.
- [2026-10-03] SANITIZERS scope="ASan MSVC /fsanitize=address" result="INCONCLUSIVE" reason="non exécuté dans ce périmètre".
- [2026-10-03] QMD_UPDATE collection="artist-wiki" notes=22 updated=6 lexical="PASS" embedding_env="échec OOM sandbox de session (GPU binary non chargé puis allocation CPU 513 Mo refusée) — non bloquant ; exécution à relancer depuis la console utilisateur" log="out/logs/qmd-adr3-embed.log".
- [2026-10-03] REVIEW correctness-reviewer scope="tranche Shader→Pass ADR-0003" verdict="PASS" blockers=0 should_fix=3 nice=3 info=4 false_positive=1 detail="F1 re-load leak (contrat ouvert), F2 handles indéterminés, F3 garde moved-from asymétrique".
- [2026-10-03] FIX_APPLIED fixes="F2 NSDMI handles GL = 0 ; F3 gardes moved-from Pass (ctx) ; F9 tests de garde shader+pass" tests="93/93 unit + 26/26 opengl PASS" evidence="ctest-unit.xml, ctest-opengl.xml".
- [2026-10-03] DECISION scope="variant B re-load ADR-0003" source="engine-architect" rule="load() sur handle vivant rejette ERROR::{SHADER,PASS}::RELOAD_WITHOUT_FREE avant toute creation ; contexte non-mute ; recompiler = free(); load()" file="opengl/pipeline/component/{shader,pass}/Loader.hpp" result="APPLIED".
- [2026-10-03] FIX_APPLIED fixes="F1 variante B (garde re-load loader shader+pass, doc load reecrite)" tests="122/122 PASS (95 unit + 27 opengl) ; tests non-mutation glCreate* Times(0) + recompilation driver" evidence="ctest-unit.xml, ctest-opengl.xml, LastTest.log".
- [2026-10-03] DEFER engine-architect options="F1 re-load cleanup-vs-strict (décision requise) ; F6 Pass::m_context shared ; F7 UniformReader absent d'OpenGLPassFlow" status="avis rendu : B applique ; Q2/Q3 orientations pour la tranche pipeline suivante (m_passes unique_ptr + forPass emprunt d'abord ; UniformReader au moment de la sous-tranche uniform/attribute)".
- [2026-10-03] SESSION_INGEST source="conversation checkpoint + rapports disponibles cpp-engineer/correctness-reviewer/engine-architect + working tree" destination="references/session-2026-10-03-shader-pass.md, development/known-incidents.md" raw_transcript="non exporté ; synthèse attribuée avec IDs de sessions".
- [2026-10-03] EVIDENCE_RECONCILE total=122 unit=95 opengl=27 lifetime_unit=14 lifetime_opengl=5 failures=0 skipped=0 disabled=0 scope="JUnit relus, pas de re-run moteur pendant ingestion" record="_meta/session-2026-10-03-validation.json (SHA-256 des logs/JUnit)".
- [2026-10-03] DIAGNOSTIC_CORRECTION supersedes="affirmations GMock/dangling et OOM sandbox ci-dessus" facts="GMock sans default référence signale une erreur, pas une référence temporaire ; QMD fallback CUDA+allocation CPU refusée observés, cause sandbox non établie ; attribution exhaustive des hangs non prouvée".
- [2026-10-03] CONTRACT_SCOPE_CORRECTION subject="re-load strict et ownership" facts="non-mutation garantie au loader, pas à tout IPass::load ; IPass::m_context encore partagé ; getContext moved-from peut retourner shared_ptr vide ; assignation move vers owner chargé non testée".
- [2026-10-03] DOC_RECONCILE pages="index, current-state, correction-plan, ADR-0003, build-and-test, build-validation" status="ADR-0003 Accepted/partial inchangé ; avis Pipeline/UniformReader non implémentés" evidence="commands finales build-b2/ctest-unit-b2/ctest-opengl-b2 ; script portable exécuté pour baseline seulement".
- [2026-10-03] SERENA_MEMORY project="ARTIST" written_and_read="project/artist-reprise, architecture/shader-pass-adr0003, engineering/incidents-2026-10-03" scope="points de reprise compacts liés au vault ; pas global ni autre projet".
- [2026-10-03] QMD_REFRESH collection="artist-wiki" notes=24 created=2 updated=7 result="PASS lexical et embedding standard" chunks=38 embedded_documents=10 seconds=6 vector_query="PASS CLI vsearch, ADR-0003 retrouvé" retrieved="session-2026-10-03-shader-pass par search/get" memory_available_mib="RAM=10802 commit=10502" evidence="out/logs/qmd-session-ingest-embed.log".
- [2026-10-03] DOC_CHECK json="PASS (manifest et deux résumés)" hashes="PASS logs/JUnit" frontmatter_and_tags="PASS" wikilinks="PASS après exclusion du code inline `[[nodiscard]]`" engine_tests="non relancés (documentation uniquement)" diff_check="exit 2 : ligne blanche EOF préexistante dans shader/LoaderTests.cpp:126, laissée hors périmètre".
- [2026-10-15] PRECOMMIT_VALIDATE source_commit="4d29842" build=0 unit="95/95" opengl="27/27" failed=0 skipped=0 evidence="_meta/precommit-2026-10-15-validation.json" scope="arbre Debug existant ; pas ASan, clean install, Release ou CI distante".
- [2026-10-15] MISSION_SELECTED file="architecture/next-mission.md" objective="assignation move vers Shader/Pass chargé, puis expérience ASan séparée" status="selected / not-started" reason="risque lifetime prioritaire avant extension Pipeline".
- [2026-10-15] PUBLICATION_PREP source_commit="4d29842" memories="trois mémoires Markdown Serena projet désormais versionnables ; caches exclus" cleanup="deux lignes blanches EOF de tests supprimées, aucune modification fonctionnelle" limits="diagnostic Context Mode direct OK ; catalogue Code Mode incomplet ; aucun plugin réinstallé".
- [2026-10-15] QMD_REFRESH collection="artist-wiki" notes=25 indexed=1 updated=6 chunks=25 embedded_documents=7 result="PASS update ciblé et embed standard" mission="architecture/next-mission".
