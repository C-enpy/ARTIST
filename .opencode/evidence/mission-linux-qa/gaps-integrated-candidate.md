# Compléments ciblés du contrôleur de preuves

## Remise récupérée et décisions du lead

La remise publique complète du test-engineer a été récupérée par lecture de l'API native de session, après défaut de restitution automatique. Session `ses_e7c018392ffe2JtNa0aT1b7hlp`, proposition `msg_18401930b0017Na1kDlTvrYX9a`, conclusion `msg_184107ce8001lO152wFN74FU2J`. Profil effectivement retourné : test-engineer ; fichiers de snapshot inchangés. Lecture locale et messages avaient été observés. Aucun test, édition ou sous-agent exécuté par ce QA.

Textes publics archivés sous `out/logs/mission-linux-qa/qa-generator-<message>.md`, hashes respectifs `27f5754356fcbe634846e430c15c16a19f597a86fd00baf6033a60f7e4a6fdb6` et `8bac0b3788165ca0e6963558f1ea6c3a7bac9b52bac80b67b05afe08431c3f46`. Le défaut de restitution reste une limite du runtime ; il n'est pas assimilé à l'absence du livrable. Aucun accès /api/agent refusé n'a été répété.

Le candidat analysé était 0a4a671. Depuis, la réparation du gate est intégrée à 78e7b74 et son finding fermé par ré-audit indépendant. Les trois propositions restantes ne modifient que `scripts/tests/test_linux_evidence.py` ; les gardes du contrôleur de production sont inchangées entre ces candidats.

## Oracles adoptés avant code

1. **KEEP — renderer réellement journalisé.** Fixture contrôleur complet, codes enfants nuls et populations concordantes. Chaque cas graphique avec contexte llvmpipe donne PASS ; contexte absent ou autre renderer donne retour non nul et manifeste FAIL pour cause de contexte. Utiliser le format réellement observé ; une fixture n'est pas une preuve GPU.
2. **KEEP — environnement ASan hostile à la frontière de l'adaptateur.** Options parentes absentes puis permissives/redirigées, activation différée présente : chaque commande enfant doit recevoir exactement CANONICAL_ASAN_OPTIONS, sans ASAN_ACTIVATION_OPTIONS ; environnement parent inchangé. Ne pas dupliquer les tests du lanceur inférieur, cette frontière prépare séparément canaris et CTest.
3. **KEEP — identité modifiée pendant la campagne.** XML verts et llvmpipe ; témoin stable donne PASS. Modifier après le premier relevé un fichier de fixture effectivement inclus dans identity() doit donner retour non nul et manifeste FAIL, diagnostic d'identité changée. Pas de nouvelle infrastructure ou mutation de sources réelles.

Le gate GLEW est un seul défaut, déjà corrigé par le lead en RED/GREEN puis relu indépendamment. **DEFER** : explication complète de l'anomalie de dépendances incrémentales Windows ; les échecs et l'expérience de reconstruction à sources inchangées restent conservés. **REJECT** : doublons des négatifs existants, quotas de branches et élargissement aux plateformes ou campagnes hors mandat.

## Exécution séparée et portée

Le lead est propriétaire de la persistance et runner distinct des propositions QA. Petite tranche indivisible, évaluation locale PM/LLM : complexité brute 43, retenue 70 ; risque 40 (fixtures du contrôleur, aucun moteur ou commande de production modifiée). Plan QA existant obligations O4/O6/O7 ; pas de nouvelle architecture ni politique de routage.

Il s'agit de tests de caractérisation de gardes existantes et de comblement de gaps après intégration, pas du RED auteur d'une fonctionnalité nouvelle. Aucun échec artificiel exigé pour prétendre un RED : exécuter les tests puis qualifier leur détection par scénarios discriminants/mutations temporaires en mémoire, sans modifier les sources de production. Les défaillances inattendues restent rouges ; deux réparations au maximum, jamais assouplissement d'oracle.

Résultats runner lead : `python -B scripts/tests/test_linux_evidence.py -v`, Windows Python 3.13 et Linux Python 3.12 : **9/9 chacun**, dont trois nouveaux cas, aucun test retiré. Logs `out/logs/mission-linux-qa/qa-gaps-{windows,linux}.log`. Test SHA256 `677def32a33472e51879811e79866b327c4e54948b3700279ed4a25875b4f502`. Production inchangée.

Expériences discriminantes exécutées par le lead, mutations uniquement en mémoire : retirer la garde renderer déclenche deux échecs d'assertion dans le test concerné ; conserver les options ASan hostiles en déclenche quinze (enfants distincts) ; retirer la garde d'identité en déclenche un. Chaque expérience exécute un test, sans erreur d'exécution ; les trois mutants sont détectés. `qa-gaps-mutations.json`. Aucun fichier de production muté sur disque, aucune réparation ou assertion affaiblie, pas de RED auteur réinventé.

Instrumentation moteur Linux contrôlée séparément du canari : 43 TU tests et 13 TU composants ont `-fsanitize=address` dans leurs commandes de compilation ; les deux exécutables portent 34/36 références ASan et lient libasan, hashes binaires identiques aux manifestes exécutés. Résumé `gate-final-linux-instrumentation.json`. Canaris WSL et CI : sain 0, use-after-free 86, options canoniques, detect_leaks=0.

L'audit final fresh hors board attend l'intégration et sa qualification effective d'admission. Pas de performance, Release, UBSan, fuites exhaustives, GPU matériel Linux ou garantie universelle OpenGL 3.3.
