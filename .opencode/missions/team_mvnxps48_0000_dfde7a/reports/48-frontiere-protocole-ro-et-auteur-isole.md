# 48 — Frontière livrée RO et admission distincte de l'auteur source

2026-10-26. Décision46/protocole44 adoptés, source moteur82bf52d inchangée. Aucun source/test sonde encore écrit ou exécuté.

## Producteur réellement livré, dette source non effacée

S task_mvpdowje_000b_13u1km a été publiée avec **première phase RO/admission+protocole SEULE** et n'a reçu que le GO RO41. Ses sept fragments/FIN sont intégrés44 dans efd4bb4 et protocole explicitement adopté46. Le coordinateur fige maintenant cette frontière comme **S_RO livré**, sans dire sonde produite ou validée. Le développement source sera un **consommateur S_CODE distinct**, mêmes scores développement70/66 du PM37, pas une réouverture B1 ou un completed destiné à masquer une dette. I/V/D/A attendent toujours code/preuves effectivement intégrés.

Cette séparation permet une admission native fresh plutôt que réutiliser l'historique E1 pour écrire. Pas de nouvelle autorisation de modèle/score/provider ; pas d'élargissement de profil.

## Préconditions de retrait du seul producteur RO

pool-5 idle, même ancienne session ; son worktree enregistré ensemble-2dc31e0a-dfde7a-pool-5 vérifié par git status --porcelain vide, HEADb7cc884, rev-list main..HEAD=0. Aucun livrable source/commit à récupérer. Acter S_RO après livraison intégrée, puis un seul shutdown natif de ce producteur terminé **avant publication S_CODE** : aucune tâche source à libérer ni campagne à relancer. Préserver branche/worktree, team_merge de contrôle après shutdown ; jamais nettoyage destructif ni boucle shutdown/réadmission. Si un doublon ressort malgré ces préconditions, arrêter cette voie et garder l'affectation inactive.

## Diagnostic et admission de S_CODE

Le code installé de l'importeur/spawn distingue les branches fresh et héritées :14996–15008, move explicite pour héritée, création fresh avec workspaceID. GET natif de pool-5 expose seulement location.directory canonique, pas workspaceID ; le worktree enregistré seul ne prouvait donc pas isolation. Ce constat statique ne prouve pas le comportement d'un futur worker.

S_CODE doit être admis avec profil cpp-engineer, contexte natif fresh, worktree nouveau **après** QA/contrat37/38/44/46 commis. Une fois les données natives observées : si la session reste au canonique alors qu'un worktree correspondant neuf propre existe, utiliser la voie publique **opencode.session_move** sur ce child vers son worktree enregistré, sans modifier modèle/permissions ni base Ensemble. Vérifier ensuite GETlocation, HEAD réel, diff, profils et lecture/restitution dans ce répertoire avant GO. Refus/divergence = BLOCKED, aucun code. Aucun move d'un état dirty ou vers un worktree tiers.

task_add n'expose pas context/worktree : maintenir admission seule avant vérification réelle ; ne pas prétendre configurer ces champs par la prose. L'ancien worker cpp retiré ne doit plus pouvoir reprendre S_CODE. Auto-dispatch éventuel d'un autre worker non fresh = gate, pas contournement. Native explicit spawn avec claim_task si tâche encore pending, sans modèle imposé.

## Détails locaux d'interface retenus avant auteur

- controls.exe --case native-read-whole --root ABS_NOUVEAU : quatre tailles0/4096/8192/12325, octets complets ; exit0 tous conformes, exit1 assertions échouées avec motif explicite, exit20 précondition/setup manquante, exit30 cleanup/contrat d'outil violé. Le RED attendu est **exit1,12325→8192,ReadFile réussi**, jamais compilation/setup cassé. Autres trois tailles vertes. Ancienne archive intacte ; helper repris sans changement algorithmique pour RED puis boucle minimale après preuve.
- probe.exe --case m0|m1-text|m1-binary --root ABS_NOUVEAU : exit0 observation valide et favorable/conforme, exit10 observation valide mais compatibilité/erreurCRT défavorable, exit20 non discriminant/précondition, exit30 instrumentation/cleanup invalide. Afficher explicitement les noms d'issue ; aucun exit0 global s'il existe une ligne non conforme/inconclusive.
- CLI refuse options inconnues/racine existante sans la supprimer ; pas de mode/processus caché supplémentaire. Logs complets hors fixtures, octetsNUL représentés en hex, longueur/octets/états et primaires-secondaires distincts. Les contrôles supplémentaires proposés au44 ne sont pas une campagne implicite : seules garanties d'acquisition/CLI nécessaires et matrice adoptée.
- Commandes compilateur et d'exécution intégrales restent à dériver depuis entrée réelle, figer/hash/approuver avant runner ; ces formes de CLI ne sont **pas** des commandes déjà exécutées ni un binaire existant.

## Tranche initiale S_CODE autorisable après admission

Auteur écrit seulement controls.cpp et helper hérité inchangé + protocole RED ; aucune boucle corrigée ni sonde exploratoire implémentée avant preuveREDlead. Runner lead distinct, source réellement intégrée avant essai. Même auteur poursuit GREEN après preuve, puis instrumentation caractérisation adoptée avec contrôles ; pas phases parallèles ni assertions affaiblies. Aucun Reader/CMake/test moteur, nouvelle dépendance/service/install/GL/ASan. Ownership exclusif scripts/probes/qa-f8-stdio/** ; sorties out/qa-f8/stdio-ID lead seulement. Futur audit fresh et décision Reader distincts inchangés.
