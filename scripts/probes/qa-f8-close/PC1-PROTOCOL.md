# PC1 — contrat de compilation de la collecte

Tranche initiale autorisée par les rapports60 et64 (décisions locales du lead,
commit46dd3893b742170bfaab317b4c622e2a561eb3c9). Aucun parent/enfant,
processus, handle, flag de protection ou correction Reader dans cette tranche.

## Contrat et consommateurs

`artist::qa::close_probe::can_reap_process(DWORD)` doit autoriser collecte et
cleanup uniquement pour `WAIT_OBJECT_0`. Les trois autres résultats testés ne
prouvent jamais la terminaison. Le futur parent devra réellement consommer ce
prédicat aux portes de collecte/cleanup ; les assertions seules ne prouvent ni
ces appels, ni leur ordre, ni la supervision d'un enfant.

Le header contient volontairement une déclaration constexpr **sans définition**.
Ce n'est pas une implémentation volontairement fausse. La signature `noexcept`
adoptée60 correspond au futur prédicat limité à une comparaison scalaire.
Les seuls consommateurs actuels sont les quatre assertions du contrat ; la TU
baseline n'appelle pas la fonction et vérifie les constantes du SDK Windows.
Aucune ressource possédée/empruntée, invalidation ou synchronisation runtime
n'est introduite par ces TU de compilation seule.

## Runner lead : baseline puis RED

1. Intégrer les vrais blobs avant compilation ; figer candidat, empreintes,
   environnement et les deux argv complets avant essai. Sortie exclusive neuve
   `out/qa-f8/close-policy-red-ID`, sérialisée avec les builds stdio.
2. Dériver les commandes de l'entrée réelle `TraceableExceptionTests.cpp`
   et du wrapper existant, sans inventer de chemin compilateur/SDK. Compiler
   chaque TU séparément avec `/c`, objets/PDB distincts, MSVC14.44 Debug,
   `/RTC1`, `/MDd`, `/Zi`, C++latest et protections/macros/includes conservés.
   Le wrapper de compilation peut être utilisé ; aucune commande RUNclose ici.
3. Compiler `wait_policy_baseline.cpp` : exit0 requis. Header/SDK manquant,
   erreur de syntaxe ou déclaration seule rejetée par les options courantes
   rendent ce protocole INCONCLUSIVE ; ne pas affaiblir les options.
4. Compiler `wait_policy_contract.cpp` : rejet requis pour définition constexpr
   indisponible de `can_reap_process` lors des static_assert. Archiver logs
   complets, code de sortie et notes causales au symbole. Autre motif de rejet,
   erreur de linkage ou mauvaise valeur calculée ne constituent pas ce RED.

Le RED attendu concerne une capacité statique nouvelle non implémentée, pas
un comportement parent, une erreur OS, un incident historique ou Reader.

## GREEN ultérieur, uniquement après preuve et GO distinct

Même auteur, mêmes quatre assertions et même baseline. Après RED causal intégré
et GO lead, ajouter seulement la définition minimale du prédicat ; recompiler
les deux TU avec les mêmes options : deux exit0 requis. Relire ensuite les
callers du futur parent. Aucun corps GREEN n'est livré dans cette tranche.

## État et limites

Aucune compilation, exécution ou preuve RED/GREEN par l'auteur. Les argv exacts
restent à dériver/approuver par le lead avant ses essais ; aucune ligne supposée
valide n'est présentée comme exécutée. Cette compilation ne crée ni processus
ni fixture et n'appelle aucune API d'attente. Pas de CMake/CTest/ASan/GPU requis
pour cette seule capacité. Calibration FILE*52, récupération processus,
Reader/nonpublication et catch historique restent distincts et non démontrés.
