# PC1 — contrat de compilation de la collecte

Tranche policy initiale autorisée par les rapports60 et64 (décisions locales du lead,
commit46dd3893b742170bfaab317b4c622e2a561eb3c9). Aucun parent/enfant,
processus, handle, flag de protection ou correction Reader dans cette tranche.

## Contrat et consommateurs

`artist::qa::close_probe::can_reap_process(DWORD)` doit autoriser collecte et
cleanup uniquement pour `WAIT_OBJECT_0`. Les trois autres résultats testés ne
prouvent jamais la terminaison. Le futur parent devra réellement consommer ce
prédicat aux portes de collecte/cleanup ; les assertions seules ne prouvent ni
ces appels, ni leur ordre, ni la supervision d'un enfant.

La tranche RED initiale contenait volontairement une déclaration constexpr
**sans définition**, pas une implémentation volontairement fausse. La signature `noexcept`
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
callers du futur parent.

### Progression de la tranche

Le lead a intégré les quatre blobs RED de `cd75653` à l'identique dans `da6d438`.
Sa preuve `b9ffc406e81737dd2e1319ac312ef8411fc82da8`, rapport66, constate le
2026-10-26 : baseline exit0 à17:20:50Z ; contrat exit2 à17:20:52Z, exactement
quatre C2131 aux lignes3–6 avec notes sur la fonction constexpr non définie.
Archives : `out/qa-f8/close-policy-red-20261026-01`. Attribution : runner lead,
pas exécution auteur. Le décodage de synthèse a été corrigé sans rerun.

Après cette preuve et GO lead, le même auteur livre uniquement la définition
`wait_result == WAIT_OBJECT_0` et cette annotation ; baseline et quatre assertions
inchangées. La recompilation GREEN des deux TU reste à effectuer par le lead
après intégration des vrais blobs. Aucune preuve GREEN déduite de la lecture.

## État et limites

Aucune compilation ou exécution par l'auteur. Les argv RED ont été figés et
exécutés par le lead selon rapport66 ; les commandes GREEN doivent être validées
par lui avant ses essais. Aucune ligne supposée valide n'est présentée comme
exécutée. Cette compilation ne crée ni processus
ni fixture et n'appelle aucune API d'attente. Pas de CMake/CTest/ASan/GPU requis
pour cette seule capacité. Calibration FILE*52, récupération processus,
Reader/nonpublication et catch historique restent distincts et non démontrés.

## Nouvelle tranche parent/enfant — décisions75/76

Autorisation explicite opérateur et QA amendé, commit87d3625, avant logique.
`PARENT-ORACLES.md` a été écrit avant `close_probe.cpp` ; `controls.ps1`
matérialise ensuite cinq contrôles CLI négatifs sans timeout/kill. Ce sont des
contrôles de caractérisation, pas un RED comportemental revendiqué. GAP RED
comportemental du nouveau superviseur conservé. Tout défaut observé ultérieur
doit garder sa preuve causale avant correction minimale et GREEN.

Le header policy et les deux TU restent immuables dans cette tranche.
Le parent appelle réellement la policy via `Supervisor::observe` sur ses trois
chemins d'attente : initial10000ms, attenteINFINITE après HOLD, destruction.
WAIT_FAILED/inattendu conserve les owners et attend quiescent dans Sleep ;
timeout ne libère pas le bail ni ne permet un bras suivant. Une fin tardive
permet cleanup après signal, mais reste exit40. Crash fatal/hôte hors garantie.

CLI publique : `close_probe.exe --root ABS_NOUVEAU --run-id TOKEN_ASCII`.
Le mode enfant interne porte arm/root/run-id/parent-pid/parent-ctime ; ce n'est
pas une authentification/sandbox. Hypothèse : environnement local possédé,
sans mutateur concurrent. Seuls chemins locaux sur lecteur, sans composants
dot ni suffixe point/espace sont admis. Logs128KiB, records8192octets ; valeurs
variables autorisées sont token ASCII validé et cheminmodule encodé UTF16hex.

La valeur d'adresse réellement appelée pour fclose est attribuée à un module
non EXE sans charger une autre CRT. Imports, version et hashfichier de ce module
restent à recouper par le lead ; le probe ne certifie pas les sources SDK ni le
syscall CloseHandle interne. Exception native : aucun handler ajouté, aucune
consommation FILE inventée. Une garde ne réessaie jamais une tentative engagée.

Codes20 usage/setup/identité/nominal ;30 instrumentation/cleanup ;40 fin tardive ;
10 cible fclose0 ou observation incomplète/exception ;0 uniquement nominal
conforme, cibleprotégée fcloseEOF, signaux observés et cleanup réussi. Champs
finaux distincts, toujours reader_proven=false. Le root/log/lease restent
artefacts ; seul fixture/target.dat puis fixture sont supprimés après fin.
Une acquisition setup échouée peut laisser des artefacts privés à inspecter,
sans enfant abandonné ni suppression récursive.

Runner : COMPILE argv exacts à dériver/valider après intégration. Contrôles CLI
via controls.ps1 avec Exe absolu et Root absolu neuf. RUN parent exclusivement
functions.shell background=true timeout=0, jamais le wrapper timeout1200.
Aucun build, contrôle PowerShell, processus ou flag n'a été exécuté par auteur.
La compilation, les scénarios runtime et la revue fresh restent non effectués
à la remise source ; ne pas transformer la lecture en verdict de validation.
