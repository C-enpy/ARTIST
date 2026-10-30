# QA administrative de fermeture — lot distinct

Proposition texte `result_mvv1atrl_009l_itsley`, qa-planner/pool-46, reçue après
admission native recoupée par team_view. Lecture/skill/restitution seulement,
aucune exécution Git, restauration, édition, intégration, push ou cleanup du
planner. Le chemin mission-gpu-qa évoqué par sa restitution n'est pas utilisé :
ce plan appartient à la mission uniform-pipeline et ne modifie pas cet autre lot.

Pin fourni7393acd, code pipeline165aee5 inchangé, inventaire initial21b10df.
155 commits d'avance et0 distant absent sont un instantané, pas l'attendu final.
PM/LLM40/56 administratif inchangé ; matrice QA indépendante du routage.

## Obligations et inférences

Explicites : livrables retenus et travail remplacé conservés sur main, commit et
push, shutdown/intégrations natifs, clôture sans perte. Pas réactivation d'anciens
RED/prototypes. Préserver20 profils étrangers, trois rapports d'autre mission,
.tmp et preuves principales ; pas staging global/adoption edit:* allow.
Pas git merge manuel des teammates, force, purge, reset hard ou modification DB.
Pas acknowledge_uncommitted comme raccourci. Push normal [skip ci], SHA distant
égal avant retrait. Pas de nouveau test moteur/GPU ou développement runtime.

Inférences retenues : présence/hash d'une archive ne suffit pas à prouver sa
restitution ; patches et bundle ont des rôles différents. Cleanup est intégrateur
et destructif. Toute modification active inattendue bloque l'action et ses
dépendances, pas les diagnostics indépendants autorisés.

## Risques prioritaires, tous >=10

R1 perte de travail, I5×P3=15. Déclencheur : retrait avant sauvegarde, mauvaise
branche ou archive incomplète. Portée neuf commits uniques/six brouillons19.
Détection inventaire et hashes ; mitigation archives inertes versionnées, bundle
local et provenance avant mutation. Gap : exhaustivité/restauration à prouver.

R2 régression, I5×P3=15. Déclencheur branche ancienne/auto-merge ou mismatch29.
Portée sources et garanties du pipeline accepté. Détection dispositions connues
et comparaison d'arbres ; mitigation pathlists exactes et inspection du résultat
complet de chaque fusion native. Gap : compatibilité réelle de la préparation
avec la fusion trois voies et les métadonnées natives.

R3 publication/clôture erronée, I5×P2=10. Déclencheur staging global, mauvais remote,
mouvement distant, suppression avant push ou adoption de permissions étrangères.
Portée dépôt/preuves/permissions. Mitigation staging explicite, push normal et
égalité SHA, dirtiness avant/après. Gap : reçus publication/cleanup finaux.

## F3 acceptable conditionnellement

Avant remplacement, préserver originaux et provenance, puis vérifier restitution.
Établir par branche paths/suppressions/arbre attendus ; aligner uniquement ces
chemins aux octets acceptés sur main, archives inertes hors découverte/build.
Aucun remplacement global/reset forcé/histoire forcée. Les6 brouillons19 doivent
être préservés avant tout remplacement. pool29 : identité Git réelle et cible
native doivent concorder avant opération ambiguë ; pas de correction de DB.

La fusion finale doit produire uniquement archives et administration attendues.
Un alignement ciblé seul ne prouve pas l'innocuité trois voies : comparer l'arbre
intégré complet. Si impossible, bloquer cette branche et son cleanup, pas forcer
ou inventer un mode archive-only. Préserver les copies et exposer le dilemme.

## Checklist et oracles lead, sans suite moteur

- Inventaire figé :8 worktrees, HEAD/branches/pathlists/dirtiness attribués ;
  élément inconnu ou changement d'identité suspend sa disposition.
- Préservation :9 commits/six brouillons représentés, bytes/hash/provenance ;
  archive manquante/tronquée/casse/chemin différent interdit le retrait.
- Restitution : bundle vérifiable et patches/copies reconstruits dans un espace
  de contrôle possédé, jamais index/worktree principal ; comparaison aux originaux.
  Illisible/non restituable ou dépendance absente = sauvegarde non qualifiée.
- Anciennes branches : archives seulement dans résultat complet ; tout ancien
  prototype actif, suppression inattendue ou conflit arrête l'intégration.
- Mismatch29 : branche réelle/worktree/cible native concordants avant action ;
  si divergence persistante, aucune opération native ambiguë sur ce worker.
- Shutdown : reçu natif et contenu inchangé depuis sauvegarde ; nouvelle écriture
  impose réinventaire ciblé, pas poursuite avec hypothèse périmée.
- Main/push : liste staged explicite, commit[skip ci], remote correct, égalité
  SHA ; fichier étranger, rejet ou divergence = ni force ni cleanup.
- Cleanup : aucune fusion inattendue/warning/conflit ; sources et dirtiness
  préservées et reçu natif d'archivage. Pas d'affirmation closed si pending.

Ordre : préservation → restitution → shutdown → intégrations relues → publication
vérifiée → cleanup. Après interruption, lire reçus/état avant reprise ; aucune
répétition aveugle ou restauration globale de la dirtiness étrangère.

## Registre obligation ↔ preuve

- Originaux : inventaire158, futurs manifestes/bundle/patches/hashes/restitution.
- Pipeline inchangé : référence165aee5 et manifeste source_after, arbre complet
  avant/après chaque merge et cleanup.
- Identité29 : refs Git réels et reçu natif concordants.
- Étrangers : backup patch et comparaison de bytes/status avant/après.
- Publication : staged/commit, reçu push et ls-remote égal.
- Clôture : reçus shutdown/merge/cleanup et absence d'avertissement non résolu.
- Admission : lectures/skill/routage natif/restitution reçus, limité à ce profil.

Le rapport158 n'est pas une fermeture exécutée ; hash ne prouve pas restaurabilité ;
push ne prouve pas innocence du cleanup ; archives historiques pas code qualifié.
Chaque preuve lead conserve date/source, HEAD/delta/pathlist/hash/action/cwd/
attendu-observé et opérateur. Aucun nouveau test recensé : NON_EXECUTE du lot,
pas zéro test=PASS. Preuves synthétiques antérieures restent leurs candidats.

## Budget, sortie et arrêts

100 unités indicatives, pas durée : setup25 ; planifié56,25 (75% des75 restants),
buffer18,75 (25%). Priorité sauvegardes/restauration, branches/mismatch, push puis
cleanup. Ne pas supprimer un contrôle haut pour la documentation secondaire.
Réestimer avant l'action destructive si restauration/mismatch consomment le buffer.

Sortie : archives complètes/inertes/versionnées/restituées, sources inchangées,
étrangers préservés, main publiée normalement SHA égal, cleanup sans warning et
reçu final. Livraison reste pour qualification ultérieure, aucun résultat natif.
Refus natif, ambiguïté, archive incorrecte, delta actif inattendu, concurrence ou
option interdite = arrêt ciblé et diagnostic lead, pas autre transport contournant.
Arbitrage humain seulement si perte, action interdite, changement du pipeline ou
extension d'objectif réellement indispensable. Aucun GO routine supplémentaire.
