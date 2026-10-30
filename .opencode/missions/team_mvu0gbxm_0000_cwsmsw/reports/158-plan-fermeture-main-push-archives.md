# 158 — Fermeture sûre : main, push, archives et sessions

L'utilisateur demande explicitement de mettre tout le travail effectué sur main,
le committer et le pousser, fermer les sessions workers et clôturer la mission.
La livraison du pipeline reste celle du rapport156 ; cette phase ne rouvre ni
campagne moteur/GPU ni développement du runtime Ensemble.

## Inventaire réellement obtenu

Le dépôt principal est déjà sur **main**, HEAD21b10df. Après fetch ciblé réussi,
origin/main fcf14a1 est ancêtre de main, avec0 commit distant absent localement
et155 commits locaux en avance. Remote vérifié : origin, dépôt GitHub C-enpy/ARTIST.
Aucun push, force, checkout ou mutation de worker encore effectué.

Huit worktrees workers de cette mission sont encore présents. pool-30/42/43 sont
propres et leurs HEAD ancêtres de main. Les autres contributions à disposer :
- pool-6 :6 commits uniques, propositions/interfaces et tests Windows historiques.
- pool-13 :1 checkpoint RED contexte historique.
- pool-15 :1 checkpoint RED collecteur contexte.
- pool-29 :1 checkpoint RED Linux ; branche réelle pool29-linux-binding-red,
  différente du checkout indiqué par les métadonnées d'équipe, à réconcilier
  uniquement par voie Git/native autorisée avant retrait.
- pool-19 : HEAD déjà ancêtre de main, mais six chemins non committés : helper
  Linux modifié, plan/README, deux tests et ancien adaptateur Linux proposé.

Ces propositions ont été remplacées par les livrables revus et testés. Les
réappliquer aux chemins actifs pourrait régresser le pipeline. Leur contenu et
provenance seront conservés en archives **inertes versionnées sur main**, avec
empreintes et disposition explicite, plutôt qu'effacés ou présentés comme du code
livré actuel. Avant toute réconciliation : comparer les chemins et vérifier que
les archives restituent les octets d'origine. Les preuves ignorées des worktrees
seront inventoriées et préservées hors des espaces supprimables.

Le principal contient aussi20 profils d'agents modifiés, trois rapports d'une
autre mission et .tmp. Ce n'est pas du travail de cette livraison. Les diffs de
profils ajoutent notamment edit:* allow, y compris à des reviewers : aucune
adoption de ces modifications de permissions n'est autorisée par un push de
clôture. Leur état est sauvegardé séparément et laissé intact, sans staging global.

## Plan PM retenu et barrières

Plan lecture seule `ses_e72f30c3dffexhiU7aGHaIt45R`, aucune commande, édition,
délégation ou tâche publiée par le PM. Actions du coordinateur, pas nouveaux auteurs :
1. Inventaire/disposition46/62 PM/LLM : faits Git, attribution et état des copies.
2. QA de fermeture40/56 : contrôles natifs/destruction et sauvegardes, lecture seule.
3. Préservation/shutdown/réconciliation59/78 : archives restituables avant retrait,
   shutdown sans force, native team_merge et inspection de chaque delta.
4. Commit/main/push45/70 : chemins explicites, aucun force-push, comparaison du SHA
   local à ls-remote après publication, préservation de la dirtiness étrangère.
5. Cleanup natif52/82 : seulement après disposition des branches et vérification
   du push, sans force/purge/acknowledge_uncommitted utilisé comme raccourci.

Scores administratifs bruts/retenus identiques, pas code exécutable ni plancher
développement artificiel. Toute correction de code nécessiterait une tranche
distincte ; elle n'est pas autorisée par ce plan. Politique/modèles inchangés.
Inventaire lead et proposition PM ont avancé indépendamment ; toutes mutations
Git, sessions, sauvegardes et publication seront sérialisées.

Barrières : disposition et QA réconciliées → archives vérifiées → shutdown →
diff des intégrations nativement accepté → sources testées encore identiques →
main committée et SHA distant égal → cleanup attesté → reçu final. Pas de fusion
Git manuelle de teammates ou modification de la base runtime. Un refus natif
reste une frontière à diagnostiquer, pas une invitation à forcer.

Les commits de clôture conserveront [skip ci] pour ne pas demander une nouvelle
campagne via le push. Le workflow de vérification livré reste manuel uniquement.
Sans trace distante, aucun résultat CI/moteur nouveau ne sera revendiqué.

## Preuves et portée

- Inventaire : out/verification-synthetic/mission-close-inventory-2026-10-30T13-43-45-896Z/ :
  manifest.json, git-status.txt, worktrees.txt, worker-git-inventory.json,
  preexisting-agent-diff.patch, fetch-main.*.log et remote-main.json.
- Premier script de lecture avait une regex mal échappée avant toute exécution ;
  corrigé avec parsing URL et sans regex imbriquée. Aucune mutation par cet échec.
- Aucune nouvelle suite synthétique ou native, moteur, build, rendu, benchmark,
  installation ou réservation cloud. Aucun fichier worker ou main modifié à ce stade.
