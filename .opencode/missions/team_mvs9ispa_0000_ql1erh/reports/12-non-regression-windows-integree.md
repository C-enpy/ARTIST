# QA Linux — non-régression Windows du candidat intégré vérifiée

## Résultat observé

Pendant la préparation Linux/CI par pool-5, le lead a exécuté les deux configurations Windows en séquence, sur `a4c0f82e1feecf5fc8dda5ab3fa3d32355c19530` contenant les deux producteurs intégrés.

- **Normal : 581/581**, dont 506 CPU et 75 OpenGL réels, zéro failure/error/skip.
- **ASan : 581/581**, même répartition, zéro failure/error/skip ; canari préalable sain0/UAF86.
- Vérification JUnit par bibliothèque XML standard : 581 noms uniques dans chaque configuration, ensembles strictement identiques normal/ASan. Pas d'extrapolation depuis le seul code retour CTest.

## Commandes réellement exécutées

1. `powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Jobs 1`
2. `powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -AddressSanitizer -Jobs 1`

Les deux terminent code 0. Le Bypass est limité à leur processus. Pas de compilation Windows concurrente ni de collision de fixture/GPU. Les sorties sont distinctes par preset et les JUnit ont été archivés avant remplacement ultérieur.

## Preuves

Sous `out/logs/mission-linux-qa/` :
- `integrated-windows-normal.log` et `.xml` ; SHA256 XML `485202f649f1bb50e96ee301fde01d4a12c34ae18c7ffbcfe35a83d27d7ed384`.
- `integrated-windows-asan.log` et `.xml` ; SHA256 XML `8e46c5dadb5334a4648f7d2d88d5c1e38a1fd00861b81e99d0d91a4ad3eca8f5`.
- `integrated-windows-summary.json` : HEAD, attribution runner lead, comptages, égalité des noms et hashes source des presets/module/lanceurs/test stacktrace.

## Portée et suite

Ces preuves valident Windows Debug normal/ASan sur le candidat indiqué ; elles ne prouvent pas Linux, les drivers Linux matériels, Release, UBSan, fuites ou performance. Leur réutilisation après intégration du workflow exigera de contrôler que les entrées testées sont inchangées, plutôt que de transférer implicitement un PASS à un nouveau diff.

Pool-5 a reçu ce résultat et conserve les campagnes Linux/CI ; pas de duplication des builds Windows demandée. La campagne OpenGL/CI Linux est encore en cours, et les tâches gaps/audit/documentation restent deferred. Aucun nouveau GO requis.

## Préparation d'admission QA en lecture seule

Pendant l'attente, le lead a découvert les routes agent via l'OpenAPI live puis lu les profils configurés par l'API native, avec la portée explicite `location[directory]=C:/Users/djden/source/repos/ARTIST`. Cette portée retourne 30 agents (23 locaux et 7 natifs) ; une première requête non scopée ne montrait que les natifs et des GET par nom non scopés retournaient 404, sans prouver l'absence des profils ARTIST.

Les profils configurés test-engineer/test-reviewer sont présents en mode subagent ; leurs règles finales comprennent deny-all puis read/glob/grep/skill ciblé/execute/team_message, avec edit/shell/subagent et lectures env interdits, contexte canonique ARTIST explicitement autorisé. Aucun profil ou droit modifié. Ce constat est une lecture de configuration native, pas encore une admission, une preuve de lecture/restitution du futur worker, ni une sandbox. Execute reste ouvert selon la politique de communication et ne démontre pas une invisibilité technique générale du board. L'audit reste deferred et son contrat hors board doit être qualifié au jalon correspondant.
