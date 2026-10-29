# 14 — Profil QA corrigé dans la base puis le worktree, sans grant

Date : 2026-10-24. Demande opérateur : corriger d'abord le profil de base puis celui du worktree pour interrompre la boucle du QA planner.

## Inspection et limite structurante

Fichiers effectivement lus/comparés :
- base : `C:/Users/djden/source/repos/ARTIST/.opencode/agents/qa-planner.md` ;
- worktree pool-5 : `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-6wzibc-pool-5/.opencode/agents/qa-planner.md`.

Ils étaient identiques, avec les **27 règles** imposées : deny-all, read/glob et skill artist-build-validation autorisés, secrets et external_directory fermés, edit/shell/execute/subagent refusés. Le brief précédent demandant de lire des chemins canoniques hors worktree pouvait rencontrer external_directory deny ; il s'agit d'une hypothèse de blocage, pas du dernier refus effectif récupéré du worker.

Les instructions de coordination imposent de conserver ces règles exactes et interdisent d'ouvrir execute/team_* pour la restitution QA. La demande utilisateur ne permet donc pas de présenter une ouverture de ces droits comme conforme. **Aucune permission n'a été élargie.** Les deux profils n'étaient pas désynchronisés sur les permissions ; ce n'est pas un diagnostic de YAML divergent.

## Correction appliquée, dans l'ordre demandé

1. Patch du corps du profil canonique.
2. Même patch du corps dans le worktree pool-5.

Ajout unique de 24 lignes « Refus d'accès : restitution bornée, pas de boucle » :
- un refus/outil absent identifié ne doit pas être retenté ni provoquer recherche de transport de remplacement ;
- utiliser les extraits/digestions fournis par le lead avec provenance explicite, sans bootstrap interdit ;
- chemin canonique externe refusé : signaler une fois, utiliser contexte transmis ou fichier local fourni par le lead, sans changement de racine/grant ;
- produire une réponse finale texte unique avec sections réalisables et BLOCKED précis, sans attendre une nouvelle permission ;
- aucune nouvelle collecte/auto-attribution après remise ; runner/persistance/commit au lead.

Le lead a transmis ces instructions directement au membre actif, avec résumé complet H_A, K et état B2/C2 : aucune nécessité de relire les rapports externes pour comprendre ce contrat. Un fichier de profil modifié ne prouve pas son rechargement à chaud ; le message sert de rappel de tâche, pas de changement de permissions/session/modèle. Demande séparée au membre de restituer son dernier refus déjà observé, sans le reproduire.

## Vérifications réellement exécutées

Lecture/dérivation Context Mode avant/après : frontmatter SHA256 identique **`12b8c66a34f0da14a745b2f3d6d1b21b19548e4d3ca7d7950fc39b3b455ea665`**, 27 règles dans les deux copies ; un seul bloc ajouté ; fichiers finaux base/worktree identiques octet pour octet.

`git diff --check -- .opencode/agents/qa-planner.md` exécuté dans chaque arbre : aucune erreur. `git diff --stat` : +24 lignes dans chaque copie, aucune modification du frontmatter. Contrôle adapté à une correction de consignes : pas de RED comportemental artificiel ni de build moteur.

Le script `lint-agent-profiles.ts` recommandé par le skill setup n'a pas été trouvé dans son répertoire installé après glob ciblé ; aucun parseur installé ni validation native inventée. Le frontmatter n'ayant pas changé, son intégrité est vérifiée directement par hash. Aucun autre profil/configuration globale/paramètre de modèle modifié.

## Ownership et limites restantes

Les deux modifications sont celles du **lead**, non du QA planner ; non commitées à cette étape. Préserver le delta du worktree avant arrêt/nettoyage, ne pas l'attribuer à une édition autonome QA. Aucun test moteur ou H_Q exécuté/produit par cette correction.

La correction vise la boucle d'instructions et l'accès au contexte fourni ; elle **ne démontre pas encore le déblocage runtime**. Si le runtime exige un outil interdit même pour la réponse finale, le problème reste BLOCKED à la frontière du harness, sans droit nouveau implicite. Les auteurs restent bloqués jusqu'à réception, contrôle et commit de H_Q.
