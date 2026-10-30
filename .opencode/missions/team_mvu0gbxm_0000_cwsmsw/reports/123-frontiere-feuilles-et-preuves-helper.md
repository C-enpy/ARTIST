# 123 — Fixer la propriété des feuilles et preuves privées avant code

Le plan natif du helper est reçu sans nouvelle écriture. Il propose un contexte
privé par invocation pour relier cwd, environnement enfant, outils absolus, logs
et budget aux wrappers pinés existants. La map enfant ne modifierait pas
l'environnement OS ; les primitives natives et leur ownership resteraient inchangés.

Le coordinateur confirme un gap dans le cœur : les dossiers privés prepare/build
sont créés, mais leurs fichiers ne sont pas indexés par BundleSink.command, qui
conserve seulement les stdout/stderr primaires du helper. Une petite identité
privée des outils est aussi proposée pour réadmettre la même installation en build.

Une consultation d'architecture est ouverte pour retenir la plus petite couture
de collecte et d'admission, sans nouveau schéma public ou registre général. Elle
doit distinguer cohérence de chemins/métadonnées et identité des binaires réellement
observée, préserver les logs partiels sous lease et éviter réparation silencieuse.
Les nouveaux oracles et l'ownership core seront fixés après cette décision et le
complément QA, avant toute écriture. Le GREEN de production reste suspendu.

Le contexte privé, l'exporteur MSVC allowlisté et la factory devront être testables
sans outil réel. Les dix-huit tests haut niveau remplacent la feuille enfant et
ne prouvent donc pas son raccordement natif. Le RED et les baselines du rapport122
restent acquis, sans relance ni extension de leur portée.

## Références

- Plan auteur `result_mvups06p_0000_as9dx4`, pool-31 ; candidat `44d3a0795414c2fb0d4fb24776014f4d1fd45ccb` inchangé annoncé propre.
- Code recoupé : `evidence.py::_windows_helper_logs/_prepare_windows_helper/_admit_windows_helper`, lignes269–307 ; `BundleSink.command`, lignes335–373.
- Primitives natives et legacy relus dans `scripts/build-windows.ps1` ; aucune modification exécutée.

Aucun nouveau test, helper complet, outil natif, moteur, GPU, rendu, benchmark,
installation ou cloud lancé. Aucun gain ou qualification de toolchain revendiqué.
