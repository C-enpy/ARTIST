# 15 — Prévention durable des blocages d'admission QA

Date : 2026-10-24. Mandat opérateur : pérenniser les corrections d'instructions et de permissions afin de ne pas répéter les incidents de cette mission.

## Contrat local et périmètre

Correction documentaire/configuration indivisible, propriétaire lead. Objectif : supprimer les consignes contradictoires et imposer les contrôles avant admission, sans prétendre corriger le runtime Ensemble. Évaluation locale PM/LLM : complexité brute/retenue38, risque40 ; aucun script/code exécutable ni classifier. Vérification adaptée : diff, frontmatter intact, cohérence des copies et absence de règle contradictoire ; pas de RED comportemental artificiel.

La demande autorise les changements du dépôt ; elle ne permet pas de contredire les instructions de coordination imposant les trois profils QA deny-all/read/glob/skill et leurs27 règles exactes. **Aucun droit execute/édition/shell/subagent/transport n'a été ouvert.** Un blocage exigeant ces droits reste à traiter dans le contrat/runtime, pas par un grant implicite dans ces fichiers.

## Changements pérennes

Dans la base canonique puis dans le worktree actif pool-5 :

1. `AGENTS.md` : suppression du texte contradictoire autorisant execute QA et de sa ligne dupliquée. Contrat runner séparé réconcilié avec les profils et le coordinateur. Nouvelle section admission : contrôler les champs effectifs, fournir le contexte lisible, signaler un refus une seule fois, ne pas répéter shutdown/réadmission créant un doublon, ne pas inventer completed ou modifier la base runtime.
2. `.opencode/agents/project-manager.md` : distinguer profil en prose et affinité machine importée ; champs supportés ou blocage explicite, remise compatible avec permissions, disposition des tâches remplacées.
3. `.opencode/agents/orchestrator.md` : contrôle du profil/permissions/contexte/HEAD/restitution avant admission, pas de substitut build ni d'execute ajouté à QA ; contexte dans brief ou fichiers locaux autorisés, pas de hot reload déduit du disque ; arrêt de la boucle de doublons par voie native, sans fausse complétion.
4. `.opencode/agents/qa-planner.md` : correction anti-boucle du rapport14 conservée, frontmatter inchangé ; restitution finale unique avec sections réalisables et refus précis, utilisation des digestions déjà fournies.

Pas de profil global ni de configuration utilisateur modifié. Pas de modification du modèle/provider/score de la mission ; aucun redémarrage de service, plugin ou nouveau smoke d'équipe. Les deux autres QA conservent leur profil inchangé et bénéficient des consignes projet communes.

## Vérifications exécutées

Via Context Mode : comparaison base/worktree des quatre fichiers, **identiques** ; frontmatters des trois profils modifiés comparés à `git show HEAD:<profil>` : **inchangés**. Comptage des trois QA : **27 règles chacun**. Texte d'autorisation execute QA contradictoire et ligne dupliquée absents.

`git diff --check -- AGENTS.md .opencode/agents/project-manager.md .opencode/agents/orchestrator.md .opencode/agents/qa-planner.md` : propre dans les deux arbres. Diff canonique : quatre fichiers,72 insertions/7 suppressions. Le script de lint-agent-profiles non trouvé dans le skill installé n'a pas été installé ni remplacé par une prétendue validation native.

La persistance versionnée canonique est réalisée par commit ciblé de ces fichiers et du présent rapport ; référence du commit donnée dans le retour du lead. Les deltas worktree appartiennent au lead, pas au QA ; ne pas les supprimer au nettoyage ni les attribuer à l'auteur.

## Résultat exact et limites

**Vérifié :** instructions locales réconciliées, garde-fous documentaires persistés et copies cohérentes, permissions préservées. **Non vérifié :** rechargement effectif du profil actif, capacité de restitution finale du QA et comportement d'import/routage lors d'une prochaine mission. Les instructions ne sont pas un verrou technique : elles ne garantissent pas l'absence de récidive du plugin.

Le lead a demandé au membre QA de restituer une seule fois ses sections prêtes et son refus effectif, sans rejouer l'appel. Aucun retour H_Q récupéré à cette étape ; aucun auteur débloqué. Une impossibilité de transport natif doit rester BLOCKED plutôt que faire repartir une boucle, ouvrir des droits interdits ou qualifier les règles statiques de preuve runtime.
