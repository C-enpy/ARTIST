# Aperçu borné — ingestion Attribute-GL

Historique de préparation, 2026-10-18. Politique native ensuite approuvée v1,
hash e29794644ac1b57495684a9abe8fa08b58cb27a40d33086822815aa6a354da4e.
P0 a livré son plan RO ; P1 a réconcilié les sources. Après demande utilisateur
de proportionner l'ingestion puis « go », la suite détaillée ci-dessous est
remplacée par une rédaction directe lead et la checklist
`.opencode/missions/team_mve3tz7l_000i_xtayy5/ingestion-checklist.md`.
Une note, preuves historiques qualifiées et garde-fous proposés seulement ;
pas de nouvelle délégation ni de modification opérationnelle de règles.
Les tâches P2–P9 ci-dessous restent l'ancien plan, non une file à publier.
Référence : `.opencode/missions/attribute-session-ingestion-preparation.md`.

## Candidat et objectif

HEAD observé : `f7bb171e1c8ca10929467a0fcd50469dbfa69837`.
Arbre dirty externe : profils QA/orchestrator, AGENTS, harness, configuration
GrepAI, notes de planification et index/log. Ne pas absorber ce diff, le commiter
globalement, le nettoyer ou le transmettre implicitement aux worktrees.

Ingérer l'historique Attribute-GL accessible dans `artist-wiki`, puis ne retenir
que des garde-fous locaux étayés et non redondants. Sources, interprétations,
comportements exécutés et mesures restent distincts. Pas de changement de
permissions, routage, politique globale, plugin ou script exécutable.

Corpus annoncé par la préparation : 13 sessions historiques, 850 entrées natives,
borne inclusive `msg_150091c600010Tw3DVy0Rhrz5E`, racine
`ses_eb1605bf2ffeAdDg1L6ZxSRQ90`. Ces comptes et les exports ne sont pas revérifiés
dans cette préparation. Exclure l'ingestion elle-même du corpus historique.
Raw privé dans `out/session-ingestion-2026-10-18/private-raw/` : ni Git ni QMD,
ni raisonnement interne, secrets ou prompts système dans les notes dérivées.

## Politique réellement lue

`model-policy.json`, version 1, identique à la proposition utilisateur.
Hash de proposition communiqué :
`e29794644ac1b57495684a9abe8fa08b58cb27a40d33086822815aa6a354da4e`
(identifiant communiqué, pas une nouvelle vérification cryptographique).

Providers autorisés : cursor, openai, opencode, opencode-go, zai-coding-plan.
Bandes C et R : 0–33, 34–66, 67–100. Lignes R croissant, colonnes C croissant :
`light / standard / strong` ; `standard / standard / strong` ;
`strong / strong / strong`.

- light : `openai/gpt-6-luna`, variante `high`.
- standard : `opencode-go/glm-5.3-flash`, variante `max`.
- strong : `openai/gpt-6.1-sol`, variante `high`.

Aucune admission ni exécution provider testée ; aucun modèle forcé.
Correction d'autorisation catalogue projet séparément approuvée et contrôlée :
voir `catalogue-authorization-repair.md`. Aucun start ni levée de pause induit.
Évaluation proposée PM/LLM, reprise du plan PM existant et validation locale des
deux scores indépendants ; aucun appel JEV. Le diagnostic natif annonce encore
la classification JEV : ne pas la déclencher ni la modifier implicitement.
À publication, deux scores manuels doivent rendre les tâches `not_required`.

## Graphe proposé — pas de tâches publiées

Scores bruts = retenus ci-dessous : aucune tranche de code n'est autorisée.
Tout ajout de test, script, harness ou refactoring exécutable impose une nouvelle
évaluation avec C retenue = max(70, C brute), et RED/GREEN adapté.
Tous les workers utilisent un contexte fresh ; lecteurs sans worktree.

- P0 — qa-planner, C54/R76, strong : plan QA texte, confidentialité, couverture,
  nominal/négatif, top3 risques et registre preuves↔exigences. Le risque vient
  de la publication persistante de données sensibles, pas de la taille du patch.
- P1 — lead, C78/R88 : réconcilier inventaire, bornes, hashes, compactions et
  assainissement. Reconstruction complexe et confidentialité forte. Aucun nouveau
  collecteur/script. P0 et P1 indépendants, lectures parallélisables.
- B1 — après approbation native : plan QA persisté/commité seul, corpus dérivé
  assaini/hashé disponible et HEAD/diff transmis avant les worktrees auteurs.
- P2 — correctness-reviewer, C65/R59, standard : vérifier les enseignements
  techniques Attribute/OpenGL, sources et contre-exemples ; aucune exécution GPU.
- P3 — correctness-reviewer distinct, C62/R70, strong : vérifier orchestration,
  incidents et règles déjà présentes. Risque élevé de garde-fous erronés durables.
  P2 et P3 parallèles après B1, aucun auteur de production.
- B2 — rapports intégrés et relus, contradictions conservées, allowlist arrêtée.
- P4 — general, C63/R62, standard : nouvelles notes
  `docs/references/session-2026-10-18-attribute-gl*` et manifeste assaini
  `docs/_meta/`. Profil documentaire dédié absent : substitution explicitée,
  skills wiki/Obsidian pertinents à charger avant rédaction.
- P5 — general distinct, C74/R80, strong : garde-fous locaux nouveaux et propres,
  seulement si un gap prouvé existe ; référence canonique plutôt que duplication.
  Forte complexité de conciliation, risque de prescriptions fausses persistantes.
  P4 et P5 parallèles avec fichiers disjoints ; aucun hunk dirty autorisé.
- P6 — lead, C60/R78 : shutdown/merge, diff allowlist, confidentialité, sources,
  hashes, frontmatter/liens et invariance des permissions/routage. Intégration
  obligatoire avant les consommateurs, pas seulement fin des auteurs.
- P7 — test-engineer, C50/R65, standard : proposer les contrôles documentaires
  manquants, oracles et KEEP/MODIFY/REJECT/DEFER ; aucun code ou runner autonome.
- P8 — test-reviewer fresh HORS board, C61/R76, strong : audit indépendant du
  candidat intégré et des preuves ; corrections propriétaires puis ré-audit.
- P9 — lead, C38/R48 : après acceptation, un seul indexeur, `artist-wiki` seul ;
  `node .opencode/scripts/update-wiki.mjs`, puis `qmd embed -c artist-wiki`,
  recherche avec intent explicite et get pour constater la retrouvabilité.

Les scores lead ne constituent pas une admission de worker. Consommateurs gardés
hors board jusqu'à intégration de leurs entrées ; vrais IDs seulement à publication.
Avant publication : `team_tasks_add({tasks: [], require_context: true})`.

## Contrats et critères de sortie

Transmettre engineering-principles, ADR-0003/0005 Accepted partial, ADR-0008
vérifié Windows Debug seulement ; ADR-0001/2/4/6/7 Proposed, non promus ici.
Une observation de driver ne prouve pas le contrat moteur ; compilation, rendu
et performance ne sont pas interchangeables. Pas de correction moteur dans ce lot.

QA obligatoire (risques PM >=50). Le plan distinguera explicite/inféré et
utilisera I×P sur 1–5, sans confondre ce produit avec R PM ; tout QA >=10 exige
déclencheur, blast radius, détection, mitigation et gap. Prévoir setup, 75 % de
travail planifié et 25 % de buffer ; si le corpus manque, arrêter la distillation
correspondante plutôt que reconstruire un verbatim supposé.

Sortie : chaque leçon reliée à sourceID/messageID, rectifications conservées,
couverture/exclusions explicites, raw non publié, notes retrouvables, pas de
finding bloquant à l'audit. Contrôles de documentation, pas de RED comportemental
fabriqué. Le lead exécute et attribue les contrôles ; preuves absentes NOT_RUN
ou INCONCLUSIVE. Aucun PASS GPU, performance ou exhaustivité interne revendiqué.

Les profils QA restent lecture seule : le texte local autorisant execute entre
en conflit avec les restrictions supérieures ; ne pas élargir les permissions.
Restitution indisponible = BLOCKED et décision humaine, jamais contournement.

## Approbation attendue

Corrections de plan/politique possibles dans le chat ; toute correction de
politique doit être persistée puis représentée par `/ensemble-mission config`.
Seul `/ensemble-mission start` demandé par l'utilisateur approuve la proposition.
Si une pause opérateur distincte persiste, attendre aussi son GO de reprise.
