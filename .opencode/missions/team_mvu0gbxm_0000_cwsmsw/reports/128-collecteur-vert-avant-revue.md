# 128 — Collecteur privé vert, revue avant intégration

Le coordinateur a relu la correction de comparaison : chemin absolu et égalité
native des chemins, sans résolution ni réécriture de la déclaration du helper.
Les vingt tests, leur fixture et les assertions historiques sont inchangés.

Résultats exécutés sur le candidat corrigé :
- Collecteur : **18 succès et deux skips sur20**.
- Contrats bundle : **21 succès et un skip sur22**.
- Adaptateur Windows : **54/54 succès**.
- Cœur complet : **219 succès et cinq skips sur224**.
- Contexte **28/28**, scénarios **31/31**, intégration **11/11**.

Aucun échec ou erreur ; empreintes avant/après identiques. Les skips concernent
les préconditions de liens symboliques/reparse indisponibles sous Windows ;
ils ne sont ni contournés ni comptés comme succès. Les suites ciblées se recoupent
avec le cœur complet : ces nombres ne sont pas un total de tests distincts.

Une revue indépendante fresh est publiée avant intégration. Le candidat reste
stable et non intégré. Le producteur PowerShell est indépendant et le helper
reste fermé ; la couture définitive attendra leurs deux livrables intégrés.

## Preuves

- Candidat `b9e1d524dc1976c7ab3f889ba190fdfc0a59e8f1`, auteur pool-34 ; exécutant coordinateur.
- Commandes nominatives : `C:/Python313/python.exe -B -m unittest -v test_windows_helper_evidence_contract`, puis `test_bundle_preparation_contract` depuis tests core ; quatre modules Windows depuis tests Windows. Découvertes `-B -m unittest discover -s scripts/tests/<suite> -p test_*.py -v` pour core/contexte/scénarios/intégration depuis racine worktree.
- Commandes exactes, cwd, empreintes et journaux : `out/verification-synthetic/windows-helper-core-green-v2-2026-10-30T09-17-05-427Z/`.
- Revue `task_mvurfax6_0070_35m4xh`, fresh, lecture seule ; PM/LLM68/72.

Aucun PowerShell, helper complet, outil natif, moteur, rendu, benchmark,
installation ou cloud exécuté. Seuls les tests synthétiques du pipeline sont
concernés ; les anciens enfants Python factices finis restent limités aux tests
historiques du cœur. Qualification native Windows/Linux et GPU NON_EXECUTE.
