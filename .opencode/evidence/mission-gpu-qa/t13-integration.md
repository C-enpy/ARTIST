# T13 — intégration et recoupement du coordinateur

Date : 2026-10-16. Producteur integrate-gpu-qa, source `89bf66f19bd6ef648b5a7d43f68ce668d7f9d722`,
rapports seuls `13ab25c617d9ba9817c827c9d7f0c6112207512e`, input
`1821af67ce75f838d2ae35349a1085efa8d3168b`. PM/LLM C83/R80 inchangés.
Shutdown puis team_merge dans le worktree d'intégration dédié ; **16/16 blobs**
identiques au delta complet du producteur, aucun fichier hors allowlist.

Le lead a relu les suppressions des fichiers partagés : correction textuelle de
Conan 1 en Conan 2, remplacement du routage OpenGL-tout-GPU par les responsabilités
spécialisées et précision de l'intégration avant consommateurs/QA finale. Les
méthodes préexistantes, cycle natif/start utilisateur et politique sont conservés.
La base opérateur reste séparément accessible au commit 82edafe ; elle n'est pas
attribuée à l'auteur. Readme.md et ses crédits restent inchangés par T13.

## Exécutions indépendantes de l'auteur, par le lead mandaté

Sur les blobs intégrés avant leur commit, avec runner/helper T04 relus et
inchangés. T13 teste des copies dans des mkdtemp courts possédés, Git isolé et
cleanup finally, jamais le .git canonique ou des chemins hérités à supprimer.

- `node --test --test-concurrency=1 --test-reporter=tap
  .opencode/tests/agentic/t13-integration/contract.test.mjs` : **63/63**.
- `node .opencode/tests/agentic/t04-harness/run-tests.mjs all` : **355/355**.
- `node --test --test-concurrency=1 --test-reporter=tap` puis les huit
  contract.test.mjs T05–T12 : **170/170**.
- Total distinct : **588/588**, zéro fail/skip/cancel.
- Union `validate-agentic.mjs --mode <syntax|schema|offline> --root <integration>
  --manifest .opencode/tests/agentic/t13-integration/manifest.json` : exit0,
  PASS local **41 / 81 / 215 contrôles**, missionVerdict **INCONCLUSIVE** à chaque
  niveau. Un check n'est pas compté comme un test Node distinct.
- `git diff --check` : exit0.

Logs locaux sous `out/agentic/mission-gpu-qa/lead-t13/` :

- t13.tap : `392e12d987549285c7e132e40eb4182894eccbdcf1b9008755d0f6705677dc49`.
- t04.tap : `402d0eafd28f6ace93badd61fc02e688035982760c1f3d28c8c02fd69a97c2ab`.
- producers.tap : `c05a52698ad6d312171371fd0e28b52ef170800aa45989bf48a08e1951f29c18`.
- union-syntax.json / union-schema.json / union-offline.json : argv,
  environnement et HEAD/delta/hashes du candidat conservés dans les JSON.

Les **43 logs auteur** ont aussi été copiés depuis son worktree propre au HEAD
exact annoncé vers `out/agentic/mission-gpu-qa/author-t13/` avant toute suppression
éventuelle. Les échecs de présence/config avant raccordement ne sont pas promus
en RED comportemental ; Filename too long reste un incident de fixture, pas un
RED de production. Voir t13-result.md pour les historiques et leur qualification.

## Portée et prochaine barrière

Catalogue manuel = oracle versionné, pas dispatcher. Règles QA = déclaration
statique, pas preuve d'isolation. Aucun profil/oracle QA ni skill/test T04–T12
modifié. Aucun changement moteur, installation, grant, C++/GPU, performance ou
indexation QMD/GrepAI. Le vault QMD canonique n'est pas réaffecté au worktree.

PASS : 588 tests locaux et union exécutée sur le diff intégré. NOT_RUN/
INCONCLUSIVE : S01–S12 natifs, OP1–OP3 réels, canaris/gates, rendu/image/perf.
La découverte native historique ne prouve pas leur exécution. Pas de PASS global.
T14 peut consommer seulement après ce commit ; revue test-reviewer finale fresh
hors board, puis ré-audit si findings. Le lead reste runner distinct de confiance.
