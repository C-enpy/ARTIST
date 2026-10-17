# T04-R2 — recoupement intégré par le lead

Date : 2026-10-16. Producteur fix-qa-template, commit
`e9448e8c372f1c2368ebe4b1622ffcf9f7b9b469`, parent
`40243373aed800f7f510fd2431808ebd11baf673`.
Shutdown/team_merge, **5/5 blobs identiques**, aucun profil/oracle T05 changé.
Lecture lead du diff validateur, template de confiance/identité et lexer borné ;
runner/helper/isolation Git inchangés depuis leur revue indépendante.

## Exécutions du lead sur les blobs intégrés

Windows x64, Node v24.11.1 ; aucune exécution par les profils QA restreints.

- `node .opencode/tests/agentic/t04-harness/run-tests.mjs all` : **355/355**,
  zéro échec/skip/cancel. Log `out/agentic/mission-gpu-qa/lead-r2/harness.log`,
  SHA256 `931b2450a7557f41d1bb7bb246fdaa1b11840ad94a63138c3cd8a342ba1498dd`.
- `node --test --test-reporter=tap --test-concurrency=1` suivi des huit
  `contract.test.mjs` T05–T12 : **170/170**, zéro échec/skip/cancel. Log
  `lead-r2/docs.log`, SHA256
  `1f3162c86e00d1533531492604886f9b5843e467517d7ecdc49c61de9607ac34`.
- CLI `validate-agentic.mjs --mode offline --root <integration-root> --manifest
  .opencode/tests/agentic/<lot>/manifest.json` : **8/8 PASS offline**, exit0,
  missionVerdict **INCONCLUSIVE** pour chaque lot. Contrôles comptés : T05=100,
  T06=95, T07=94, T08=98, T09=104, T10=107, T11=109, T12=102.
  JSON conservés sous `lead-r2/<lot>-offline.json`.
- Vrai T05 : trois sujets nommés qa-planner/test-engineer/test-reviewer reconnus
  en forme B, `qa-readonly-declared-v1`, **static-presence**. Log initial croisé
  `lead-r2/t05-offline.log`, SHA256
  `b383837ec5752e19646068130828257299b45cdea7531211c597424f622784cd`.

Le JSON de chaque run conserve argv, environnement et identité HEAD/delta/hashes
du candidat avant commit. Le commit d'intégration change ensuite le HEAD, pas les
blobs relus. Une preuve native liée à un ancien HEAD ne sera pas recyclée comme
preuve du futur raccordement T13.

## Preuves auteur et limites

Neuf logs auteur non versionnés ont été copiés, sans modification des sources,
dans `out/agentic/mission-gpu-qa/author-t04-r2/` AVANT nettoyage éventuel.
`green-all.tap` y est recoupé au hash annoncé
`6eab6795bdb5f45a92ecc60b57e8b14f41644c1a4e322c819b2999af84e7e6a5`.
RED et premières causes : `t04-r2-result.md`. Le lead n'a pas prétendu rejouer
l'ancienne implémentation pour recertifier ces RED historiques.

PASS borné : fixtures locales et cohérence des huit vrais manifests avec le
validateur intégré. Les 525 tests ne prouvent ni sélection native, appel effectif
des skills, enforcement, API GPU, image ou performance. Le template n'est pas un
évaluateur V2 ni une sandbox. Ces critères restent INCONCLUSIVE/blocked, runner
distinct conservé. Revue fresh de la correction avant publication de T13.
