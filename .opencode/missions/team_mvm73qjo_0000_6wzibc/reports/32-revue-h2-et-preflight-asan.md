# 32 — Revue H2 reçue, préflight ASan exécuté, restitution QA à contrôler

2026-10-24. Candidat gelé **318bf7a0e5da2a65cc437ccc29baa92418822bc4**. Suite normale555/555 au rapport31 ; aucune mutation source depuis. Étape de vérification, pas clôture.

## Revue indépendante

Tâche `task_mvml0las_001d_p1gdy4`, pool-14 au profil correctness-reviewer natif. Le lead a publié la revue après intégration, proposé fresh/read-only, puis le dispatcher l'a déjà affectée avant le spawn explicite ; aucun worker doublon créé. Le reviewer a lu le canonique, pas la branche de worktree annoncée. Son blocage initial de pin/diff a été résolu par lecture directe de .git/refs/heads/main, confirmant318bf7a, et remise du diff scoped `out/qa-h2/scoped.diff` (ecb548b→318bf7a). SHA fourni par lead `d74d68aa8609f02df63d5c7d2dd84b7ee6d6f083fcd4d7d1eba8a458d9455afc`, non recalculé par reviewer.

Revue statique : **aucun défaut bloquant démontré dans le scope**, ni nouvelle suggestion justifiant élargissement. Const helper privé, ownership/binding/ordre de destruction B4, capture étroite des failures, égalité complète, catch par catégorie et attentes complétées recoupés au code. Les lambdas consolidées sont copiables dans les usages actuels. typeid(float) est dérivé de la même fixture/build, pas un spelling ABI fixé. Aucun test/build/GPU exécuté par le reviewer ; ceci ne vaut pas revue indépendante des tests par test-reviewer ni PASS GCC/ASan.

## ASan : préparation réellement exécutée par lead

Cache séparé existant windows-msvc-debug-asan identifié : Debug/ASanON/toolchain Conan existante/racineARTIST, CXX23_NATIVE=1. Environnement des enfants normalisé au canon `halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0` et ASAN_ACTIVATION_OPTIONS retiré ; pas de mutation système/parent ni d'installation.

Via wrapper python local, commandes :
```text
cmake --preset windows-msvc-debug-asan
cmake --build --preset windows-msvc-debug-asan --clean-first --target ArtistLib_tests ArtistLib_component_tests artist_asan_probe --parallel 1
<python> scripts/verify_asan.py out/build/windows-msvc-debug-asan/artist_asan_probe.exe --log-dir out/qa-b2-h3/asan-canary
ctest --preset windows-msvc-debug-asan --show-only=json-v1
```
Toutes exit0. Logs `out/logs/b2-h2-asan-{configure,clean,canary,discovery}.log`, contrôles canari sous `out/qa-b2-h3/asan-canary`. **Sain exit0 et marqueur attendu ; faute exit86 et diagnostic heap-use-after-free vérifiés par verify_asan.py.** Pas un crash seulement.

Découverte fraîche555 noms uniques, dont les17 cas historiques du rapport13 tous présents exactement une fois. Nouveau TU TestUtilsTests instrumenté /fsanitize=address. /RTC1 et MDd volontairement conservés selon cmake/AddressSanitizer.cmake34–43/83 ; un premier contrôle ad hoc du lead supposait à tort que RTC1 devait être absent et s'est arrêté. Lecture de la commande réelle puis du contrat CMake corrige cette hypothèse : **aucune panne de compilation ASan**, aucun flag supprimé, aucun rerun coûteux pour ce contrôle.

**Suite ASan du candidat NOT_RUN** : ce préflight ne vaut pas campagne finale. Sélection finale et H3 attendent la disposition des gaps ; aucune revendication PASS555 ASan.

## Admission restrictive QA : problème observé, pas contourné

Tâche `task_mvml0lat_001e_u5r71a` test-engineer publiée pour analyse de gaps, pas édition autonome. Profil canonique lu :27 règles, read/glob/skill autorisés, execute/shell/edit/subagent fermés, marqueur final_response. Tentative explicite de spawn fresh/worktree:false/report_mode:final_response refusée : **« Autonomous workers require claim_task and collaborative reporting »**, malgré claim_task fourni. Aucun modèle imposé ni droits ouverts.

Auto-dispatch a néanmoins admis pool-15 au profil natif test-engineer, dans un worktree ; `team_status` le montre working mais ce statut ne prouve ni permissions effectives ni restitution native. Le lead a demandé une seule finale native avec sections réalisables et diagnostic, sans execute/transport alternatif/collecte répétée ; aucun nouveau droit ni hot reload supposé. À cette étape aucune réponse non lue récupérée de pool-15. **Remise QA non encore vérifiée ; pas de PASS ou d'acceptation de ce livrable sur la seule présence du profil.** Si la voie finale est réellement impossible, conserver BLOCKED et arbitrer une correction native séparée, sans élargir ce profil ni substituer build.

## Prochaine barrière

Exploiter une restitution QA réellement reçue ou constater son blocage unique ; intégrer seulement les propositions acceptées avant H3. Puis campagne ASan et audit fresh selon contrat, documentation/disposition des tâches. Aucun shutdown répété ou nouvelle admission de remplacement pour contourner les refus. F8/C2 ouverts et B1 historique non fermé ; pas de clôture globale de mission.
