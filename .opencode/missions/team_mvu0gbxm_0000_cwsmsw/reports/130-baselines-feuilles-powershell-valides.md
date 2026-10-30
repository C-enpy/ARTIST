# 130 — Baselines des feuilles PowerShell valides

Le coordinateur a relu le nouveau programme de test, son extracteur, ses limites
d'effets et ses vingt scénarios de source. Les **six baselines passent**, sans
échec, erreur ou skip, et les empreintes restent stables. Le seul programme lancé
est PowerShell avec des fragments relus et un objet processus en mémoire dépourvu
de Start/Kill/Wait ; la primitive native de capture n'est pas chargée.

Les vingt tests de source ne sont pas exécutés : le binding et l'exporteur sont
encore absents et non admis. Cette absence reste une limite de harnais, pas un
nouveau RED comportemental. Le RED causal antérieur reste celui du prepare
autorisé bloqué par la garde, obtenu après sept baselines valides au rapport122.

Les coutures privées proposées sont ratifiées : contexte par invocation, ports
de factory/horloge/observations, exporteur littéral allowlisté et nouvelle région
de définitions séparée des primitives pinées. Leur validation fonctionnelle reste
à faire. Les mappings des métadonnées VSINSTALLDIR et de l'extraction d'entrée
sont acceptés dans leur portée précise, sans affaiblir les assertions métier.

Le GREEN minimal du helper prepare/build est autorisé sur ce contrat et ces tests
déjà écrits. Le propriétaire doit remettre la source et les nouveaux hashes
proposés pour relecture avant admission et exécution par le lead. Aucun repin
WRAPPERS/NATIVE, nouveau backend de capture ou helper-run n'est autorisé.
La couture avec les reçus attend toujours l'intégration effective des producteurs.

## Preuves et limites

- Candidat tests `29036c92fc96bcb2936eb2aaf0bc381d4779e2da`, propre, quatre ajouts
  seulement depuis44d3a079 ; production et anciens tests inchangés.
- Auteur pool-31 ; exécutant coordinateur. Commande : `C:/Python313/python.exe -B -m unittest -v test_native_binding.NativeBindingHarnessTests`, cwd tests Windows du worktree auteur.
- Journaux et empreintes : `out/verification-synthetic/windows-native-binding-baseline-2026-10-30T09-51-19-076Z/` ; cas bruts pointés dans stdout.
- Nouveau contexte de test Global exclusivement dans l'interpréteur isolé,
  collision refusée et nettoyage vérifié ; aucun Global de production autorisé.

Aucun helper complet, cmd/vcvars, outil de build, moteur, GPU, rendu, benchmark,
installation ou cloud exécuté. Les six baselines ne prouvent pas le futur binding,
la source exporteur ou une qualification native.
