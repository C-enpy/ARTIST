# Audit final indépendant — remise reçue, réserve à corriger

**État ultérieur : réserve F1 fermée par ré-audit indépendant sur f58a49f**, avec nouveau RED causal puis GREEN Python Windows/Linux. Voir `final-test-reaudit.md`. Les verdicts historiques ci-dessous décrivent la première remise, non le statut de cette réserve après réparation. Les campagnes runtime de l'adaptateur renforcé sont qualifiées séparément par le lead.

Reviewer natif test-reviewer, contexte fresh hors board, session `ses_e79498a83ffe3GxE7urS3rUXcG`. Remise finale publique `msg_186ba2f980015xLyxmHfyBmyfW`, reçue via récupération native puis notification finale. Texte complet conservé sans modification dans `out/logs/mission-linux-qa/final-audit-msg_186ba2f980015xLyxmHfyBmyfW.md`. Le présent document en persiste la disposition, pas un audit nouveau du lead.

Admission et indépendance constatées : profil natif et permissions préalablement vérifiés par le lead ; lecture locale AGENTS et skill validation réussis. Le reviewer n'a ni écrit, exécuté, délégué ou consulté les justifications privées auteurs. Un essai de restitution team_message a retourné « This session is not in a team. » ; aucun nouvel essai, finale native utilisée et effectivement reçue. Ce refus ne justifie aucune ouverture de droits ou nouveau bootstrap.

## Verdict transmis

**PASS borné** pour les comportements des six configurations qualifiées, les scénarios Attribute A1–A7 et les gardes renderer/environnement/codes d'erreur. A8 repose aussi sur présence statique et non-régression, pas mesure de coût. Aucun défaut bloquant démontré dans les campagnes moteur finales.

**INCONCLUSIVE pour O7 complet et satisfaction intégrale du plan** : l'identité du contrôleur n'inclut pas les shaders consommés. Le reviewer ne prétend pas qu'une telle modification est survenue pendant les six campagnes et ne les invalide pas rétroactivement.

## Finding retenu — identité des entrées shader

Localisation : `scripts/linux-evidence.py:63–70,114–115` ; fixture/tests dans `scripts/tests/test_linux_evidence.py`. Plan O7. Les sources C++/CMake et binaires sont hashés, pas les fichiers de `test-datas/shaders/` effectivement ouverts par les tests.

Scénario discriminant proposé avant code : après le premier relevé, modifier un shader temporaire consommé, par exemple en lui ajoutant un commentaire ; XML et pixels peuvent rester verts, mais le manifeste doit FAIL pour identité changée. Le test actuel ne modifie que CMakeLists. Constat statique et inférence du reviewer, pas reproduction par celui-ci.

Décision lead : **MODIFY dans le mandat existant O7**, aucune nouvelle stratégie QA. Le lead reprend cette petite tranche indivisible : fixture négative shader, RED causal avant correction du helper, inclusion bornée des fichiers de test-datas dans les identités, GREEN Windows/Linux puis ré-audit du reviewer existant. Les DEFER_TO du reviewer sont résolus par le coordinateur ; aucun worker supplémentaire nécessaire.

Ownership : `scripts/linux-evidence.py`, `scripts/tests/test_linux_evidence.py`, preuves de cette tranche ; aucun auteur actif sur ces fichiers. PM/LLM local : complexité brute 48, retenue 70 ; risque 50, incertitude périmètre des entrées et fraîcheur des campagnes. Plan QA initial et complément déjà commités ; ce document conserve l'oracle avant code. Deux réparations de test maximum, aucun assouplissement d'assertion.

Le runtime C++/shaders eux-mêmes restent inchangés. Après correction de l'adaptateur, renouveler les campagnes Linux de preuve impactées avec son identité renforcée, mais ne pas rebaptiser les six campagnes antérieures. Les résultats Windows moteur restent réutilisables par identité du runtime/config ; exécuter les tests Python sur les deux OS. Pas de nouvelle infrastructure ni campagne de performance.

## Limites conservées

Premier RED classification sans delta complet hashé à chaud ; RED gate ultérieur identifié. Échecs incrémentaux Windows archivés et cause exacte non entièrement expliquée. Fallback OpenGL 3.3 ambigu sans route moderne non universellement qualifié. Pas Release/UBSan/performance/fuites exhaustives/GPU matériel Linux ; detect_leaks=0. La lecture du reviewer n'est jamais une exécution de runner.
