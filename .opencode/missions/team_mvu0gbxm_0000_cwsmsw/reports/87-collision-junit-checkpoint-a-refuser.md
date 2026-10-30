# 87 — Réserver le chemin du résumé sans écraser les résultats bruts

Le ré-audit indépendant lève les trois écarts précédents dans leur portée statique. Il établit toutefois une collision nouvelle liée au checkpoint : un adaptateur peut décrire une sortie JUnit au chemin `raw/ctest-population.json`, encore absent. L'admission accepte ce chemin, puis l'enfant y écrit son XML. Son enregistrement dans le manifeste satisfait ensuite à tort le contrôle d'empreinte du checkpoint, qui le remplace par du JSON.

Une empreinte correcte prouve ici l'identité, pas que le fichier appartient au producteur de résumé. Les octets bruts peuvent être perdus avant l'échec final de parsing. Le coordinateur a recoupé l'admission et le remplacement ; la reproduction reste NON_EXECUTE.

Une régression ciblée sur fixture nominale est demandée avant correction, sans modifier les 171 tests acquis. Le correctif retenu sera une réservation privée du chemin interne après canonicalisation, avant tout enfant, pour les sorties de protocoles. Aucun nouveau schéma ou changement d'API n'est nécessaire. Les chemins équivalents relatifs/absolus ne devront pas contourner ce contrôle.

Le collecteur reste non intégré. Les résultats verts précédents et les levées de findings ne sont pas annulés, mais ne couvrent pas cette collision. Les consommateurs d'interface bundle restent différés. Aucun nouveau test ou outil moteur exécuté lors de cette réception.

## Références

- Ré-audit `result_mvuipu8a_0000_llrtn2`, candidat `617e939bd8b76b0d44fdb35b6e77c3df1b3fb971` ; reviewer lecture seule.
- Localisation recoupée : `evidence.py::_execution_requests` 639–646 et `_publish_ctest_population` 843–848.
- Test proposé : `test_context_checkpoint_ownership.py`, auteur pool-14 ; runner coordinateur après remise RED_READY et relecture.

Aucun processus réel, moteur, compilation, CTest réel, OpenGL, rendu, benchmark, installation ou cloud exécuté.
