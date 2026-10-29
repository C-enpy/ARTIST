# QA Linux — audit reçu, fraîcheur des shaders à renforcer

## Résultat de l'audit

La remise indépendante est réellement reçue. Elle confirme les principaux scénarios exécutés et ne démontre aucun défaut bloquant dans les campagnes moteur. Elle conserve toutefois une réserve sur le contrôleur de preuves : ses empreintes ne couvrent pas les shaders chargés par les tests.

Un shader pourrait ainsi changer pendant une campagne sans que l'identité finale le signale, même si les tests restent verts. **Ce scénario n'est pas observé dans les campagnes existantes** ; c'est une lacune démontrée dans le périmètre de la garde, avec un scénario de test précis. Le PASS intégral de l'obligation de fraîcheur n'est pas annoncé.

## Action dans le mandat approuvé

Le lead ajoute ce négatif à la fixture temporaire existante, observe son RED, puis inclut les fichiers de données des tests dans les empreintes. Tests Python sur Windows et Linux, puis ré-audit ciblé. Les campagnes Linux de l'adaptateur seront renouvelées pour produire la preuve renforcée ; aucun changement moteur, shader ou nouveau harnais.

La tranche reste locale et indivisible, sans nouveau worker, modèle, permission ou demande utilisateur. Ownership helper/tests ; complexité PM/LLM 70, risque 50, oracles persistés avant code dans `final-test-audit.md`. La documentation finale et la PR attendent ce point, pas un nouveau GO.

## Réception et preuve

Le profil natif a lu AGENTS et le skill de validation ; son essai team_message a échoué une seule fois (« This session is not in a team. »), puis il a rendu une finale native. Le défaut de transport n'a pas été confondu avec l'absence de livrable ; texte récupéré puis notification finale reçue, pas d'édition/test par le reviewer.

Original : `out/logs/mission-linux-qa/final-audit-msg_186ba2f980015xLyxmHfyBmyfW.md`, session `ses_e79498a83ffe3GxE7urS3rUXcG`. Disposition persistée : `.opencode/evidence/mission-linux-qa/final-test-audit.md`. Les six configurations à 589 et les 9 tests Python restent acquis à leur périmètre antérieur.
