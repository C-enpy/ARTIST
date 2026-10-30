# 127 — Collecteur : comparaison de chemin Windows à corriger

Le correctif du collecteur a été relu, mais n'est pas vert. Le coordinateur a
exécuté le seul module nouveau : **20 tests, 49 échecs avec sous-cas, deux skips,
aucune erreur**. Les empreintes sont stables ; la séquence a été arrêtée avant
les suites de non-régression. Aucun avis favorable ou intégration n'est prononcé.

Une cause commune est reproduite : le témoin déclare un chemin commençant par
`c:`, tandis que la racine résolue du cœur commence par `C:`. Ces chemins sont
égaux selon la sémantique Windows, mais leur comparaison textuelle stricte refuse
la préparation nominale. Ce refus masque ensuite plusieurs tests de construction
et de réadmission ; les 49 échecs ne sont pas 49 défauts indépendants établis.

Le propriétaire reçoit une correction bornée de cette comparaison, en conservant
le chemin absolu et la liaison à l'arbre attendu. Aucun changement des vingt tests,
de leur fixture ou des assertions historiques n'est autorisé pour masquer l'écart.
Les autres écarts éventuels seront examinés après ce correctif. Le helper
PowerShell reste fermé et son producteur indépendant poursuit sa préparation.

## Preuves et limites

- Candidat `f428dfbb4d9c9676700ac8401a276709a8033d3e`, propre ; auteur pool-34,
  exécutant coordinateur. Diff vérifié : evidence.py et seules données des deux
  fakes mappés ; tests RED inchangés.
- Commande : `C:/Python313/python.exe -B -m unittest -v test_windows_helper_evidence_contract`, cwd tests core du worktree auteur.
- Journaux, manifeste et diagnostic : `out/verification-synthetic/windows-helper-core-green-2026-10-30T09-14-34-977Z/`.
- Diagnostic mémoire sous les sentinelles du test : chemins synthétiques `c:`/`C:`,
  égalité Path vraie, égalité texte fausse ; commande et sortie dans `path-diagnostic.json`.
- Deux skips concernent les préconditions de liens symboliques/reparse. Les
  suites bundle, Windows54, core et autres non-régressions restent NON_EXECUTE ici.
- Un premier script de diagnostic avait une erreur d'échappement avant exécution ;
  une première sonde Python n'avait pas lié sink.plan. Ces erreurs de diagnostic
  ont été corrigées et ne constituent pas des RED de production supplémentaires.

Aucun helper complet, outil natif, moteur, GPU, rendu, benchmark, installation
ou cloud exécuté. Tests synthétiques et données possédées uniquement.
