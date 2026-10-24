# 11 — C2 : provenance bornée, comparaison primaire bloquée

Date : 2026-10-24. Étape : enquête historique B5 ; pas de fermeture de C2.

## Résultat reçu et recoupement du lead

`pool-3` restitue une enquête lecture seule, statut `failed` car les XML primaires manquent. Aucun correctif, essai, build, édition ou commit déclaré. La provenance documentaire est améliorée ; la cause de la baseline 20:35 reste INCONCLUSIVE.

Le lead a directement relu `docs/_meta/session-2026-10-15-failure-contracts-validation.json:30–101`, confirmant :

- base `30a46b5`, RED auteur C2 `4af1312`, GREEN `0857ba4`, intégration `e736bba` ; ReaderTests inchangé entre RED/GREEN selon les preuves documentées ; arbres GREEN/intégration identiques selon manifeste ;
- baseline `out/logs/mission-c2-baseline-ctest.xml`, située au **worktree auteur cpp-c2**, non versionnée : 165 cas/13 échecs, explicitement **pas RED causal** ;
- exact `out/logs/mission-c2-red-exact.xml`, même provenance : 16 cas/11 échecs, RED causal historique C2 ;
- SHA baseline documenté `271bc222a6d00f18d8374716587b934fc5ba5159015dc049a930312c0311b68f`, exact `25353bfed4316ed9775ff00195345a216cbe983cf706aae1d7a3ad21462d1a64` ; **non recalculés** faute d'artefacts ;
- 20:35/20:38 sont des `local_mtime`, pas des heures de lancement attestées ; les empreintes sources finales ne rattachent pas le binaire baseline à un pin.

Le lead a effectué deux recherches exactes bornées `**/mission-c2-*.xml`, fichiers cachés inclus :
1. `C:/Users/djden/source/repos/ARTIST/out` : aucun fichier trouvé ;
2. `C:/Users/djden/.local/share/opencode/worktree/3a086c` : aucun fichier trouvé.

Cela confirme l'absence dans **ces deux périmètres**, pas la disparition de toute archive externe. Aucune recherche globale disque, restauration, compilation historique ou nouveau dispositif engagés.

## Ce qui reste inconnu

Identité source/binaire 20:35, commandes/CWD/fixtures exacts, exécutable/PDB et différences contrôlées avec 20:38. Le commit `4af1312` est un candidat documentaire de replay, pas une identité attestée de la baseline. Un run vert courant ou un clean rebuild de ce candidat ne fermerait pas l'anomalie 20:35.

L'hypothèse fixture/version de tests demeure hypothèse. Ne pas importer les fixtures actuelles dans un replay présenté comme historique. L'anomalie ASan 21:00 est distincte et ne prouve pas la cause de C2.

## Disposition proposée, non appliquée au registre

Conserver **C2 OPEN**, cause **INCONCLUSIVE**. Si aucune archive n'est disponible après collecte bornée : DEFER proposé avec propriétaire coordinateur + build-engineer, reprise sur XML aux SHA attendus et éléments liant sources/binaires/fixtures. Aucune fermeture motivée sans acceptation explicite opérateur.

Action utile ultérieure : fournir le chemin absolu de l'archive `cpp-c2` ou des deux XML ; vérifier SHA puis comparer cas, sorties et provenance avant décider d'un replay. Pas de replay coûteux sans pin/expérience discriminante.

## Autres retours et barrières

`pool-4` a retourné un arrêt de substitution QA, aucun H_Q ni fichier lu/modifié déclaré. Ce retour ne débloque pas Q ; shutdown non forcé demandé. Aucun nouveau profil QA n'est qualifié par ce résultat.

Shutdown non forcé demandé pour `pool-3`. Tentative `team_merge` refusée car shutdown encore demandé/actif : aucun merge réalisé ; attendre la notification de fin avant de recouper la branche, sans polling. Ce refus est un état de cycle de vie, pas une perte de livrable.

Mise à jour après notification de fin de `pool-3` : `team_merge` a ensuite confirmé **aucun commit et aucun changement non commité**, rien à intégrer. L'enquête demeure sans reproduction et C2 reste OPEN/INCONCLUSIVE ; aucune tâche déclarée réussie.

H_A est intégré à `354b53f` (rapport 10), H_Q toujours absent et transport QA restrictif bloqué. Protocole ASan sur Sol encore attendu. Aucun auteur débloqué, aucune preuve RED/GREEN courante. Les décisions d'archives et de voie QA pourront être groupées après les enquêtes en cours ; aucune nouvelle demande d'approbation n'est nécessaire pour conserver OPEN.
