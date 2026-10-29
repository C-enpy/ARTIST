# 001 — Préparation et décisions avant configuration

## Objectif et état

Préparer B1–B6 de `docs/architecture/missions/2026-10-23-qa-harness-debt-batch.md`, sélectionnées par `docs/architecture/next-mission.md`. Mission en pause, politique non approuvée. Aucun worker, classifier, test ou changement de source lancé. Plan préparé localement : le PM ne peut pas être admis avant approbation ; validation PM après start avant publication des tâches d'exécution.

## Observé et changements

- HEAD observé : `d4f5852cd21723ff9b5d66533ba601ffa6ac34e1`. `git status --short` inspecté via Context Mode ; aucune intégration ni compilation effectuée. La fiche historique était préparée contre `b368a35`, pas contre ce candidat : références et ownership à recouper avant auteurs.
- Lus : fiche de mission, pointeur, principes, registre ADR et correction-plan. D est déjà déclaré effectué : ne pas le refaire comme branche autonome.
- Brouillon `model-policy.json` : niveau `proportionate` conservé, critères d'acceptation ajoutés. Catalogue, scores modèles et seuil 50 inchangés ; ancien hash de proposition désormais périmé.
- Recherche QMD `artist-wiki`, lex `qa-harness-debt-batch`, sans résultat. La fiche existe sur disque ; fraîcheur/couverture de l'index non démontrée. Aucune réindexation lancée en préparation.

## Aperçu des tâches — non publiées

Préparation K : confirmer B1–B6 et autorisation B1 ; plan QA obligatoire persisté/commité avant worktrees auteurs. PM valide graphe, contexte fresh, ownership exact et scores. Les estimations ci-dessous proviennent de la fiche, non d'un classifier ; complexité retenue au plancher 70 pour développement.

1. **B1 — shuffle**, principal cpp-engineer : reproduire seeds 20261017/7, isoler lifecycle des mocks ; correctif seulement après RED causal. Complexité brute/retenue 70/70, risque 55. Ownership fixtures à confirmer, aucun contrat moteur élargi.
2. **B3 — F8 réel**, principal cpp-engineer : branches authentiques ifstream/fermeture ; oracle avant correctif. 70/70, risque 50. Ownership loaders à confirmer sans conflit B1.
3. **B4 — unbind/helpers**, principal cpp-engineer : trois tests nommés, garde existant et égalité stricte des erreurs. 70/70, risque 45. Tests seulement.
4. **B5 — C2**, principal build-engineer : reproduction ou justification documentée ; pas de fermeture sans acceptation opérateur si non reproduit. 65/70, risque 45. Diagnostic sans mutation concurrente d'environnement.
5. **B6 — getStackTraceString**, principal cpp-engineer, renfort build si nécessaire : coupe compile-time bornée, non-régression MSVC ; ne pas annoncer GCC validé. 70/70, risque 50.
6. **B2 — campagne finale**, principal build-engineer pour préparation du protocole, lead runner : sélection ASan + 17 GL déclarée avant run. 65/70, risque 50. Préparation indépendante en parallèle ; exécution finale après intégration B1/B3/B4/B6 et traitement des préconditions B5, sinon résultat seulement intermédiaire.
7. **Intégration et QA** : lead intègre et relit avant consommateurs ; revue correctness fresh ciblée pour changements sensibles, generator de gaps après intégration, test-reviewer fresh hors board selon contrat obligatoire. Lead seul exécute les tests. Documentation clôturée après preuves, pas après témoignage d'auteur.

B1/B3/B4/B6 parallélisables seulement après confirmation de fichiers disjoints. B5 et protocole B2 peuvent être instruits indépendamment sans modifier un environnement partagé. Un build par arbre, ASan exclusif et GPU `artist_opengl` sérialisé. Pas de branche moteur, de plateforme supplémentaire ni d'outillage de test neuf implicite.

## Contrats transmis après approbation

Principes : structure statique, contenu cook, état vivant en données ; correctness/lifetime avant optimisation. ADR-0002 Proposed/partial, ADR-0003 et 0005 Accepted/partial ; ADR-0001/4/6/7 Proposed, ADR-0008 vérifié Windows Debug uniquement. Ne pas promouvoir un ADR. Chaque brief portera HEAD/diff intégré réel, symboles et références, invariants, essais antérieurs, preuves attendues, ressources et modifications autorisées.

## Politique réelle et conflits à résoudre

- Catalogue actuel : tous les providers de la proposition sont encore autorisés, pas uniquement les trois favoris ; Sol high est encore marqué réserve. Aucune préférence par nature de tâche n'a été rendue exécutoire.
- Seuil de planification 50 : Sol high vaut 50 ; Grok Extra High vaut 46 ; OpenAI Luna high vaut 33. Les deux derniers ne satisfont pas le seuil courant. Ne pas falsifier leurs scores, forcer un spawn ou promettre un routage non établi. Recommandation : Sol high pour planification ; faire réviser en config la préférence Grok et l'éligibilité Luna pour documentation/recherche simple. Ne pas abaisser globalement le seuil sans décision.
- `proportionate` convient aux vérifications ciblées. `light` ne supprime pas TDD ni les obligations du projet ; `reinforced` ajouterait un plan explicite/revue sans nécessité démontrée. Le contrat projet impose déjà plan QA puis contrôles de gaps et revue aux déclencheurs applicables ici (comportement, intégration étendue, risque >= 50). La proposition « aucune chaîne obligatoire » ne peut pas les annuler silencieusement : elles restent dans le plan. La restriction QA développeur deny-all prévaut sur le texte local divergent autorisant execute.
- Méthode : PM/LLM et scores manuels exigés par le contrat projet ; classification automatique/JEV non demandée et non lancée. Ces scores ne choisissent pas un modèle en contournant la politique native.

## Preuves, limites et décision demandée

Commandes réellement exécutées : lectures Git (`git status --short`, `git rev-parse HEAD`) et lectures de fichiers via Context Mode/outils fichiers ; recherche documentaire QMD. Aucun test : NOT_RUN. Commandes de validation exactes à fixer après vérification des paramètres scripts, outils et sélection ASan ; `scripts/build-windows.ps1` et `scripts/asan-env.ps1` sont présents, ce qui ne prouve pas leur succès.

Décisions regroupées : conserver QA proportionnée avec obligations projet ; confirmer B1–B6 et autorisation d'enquête B1 ; résoudre préférence modèles face au seuil 50 (recommandation ci-dessus). C2 reste ouvert sans preuve/acceptation. Corriger le brouillon en chat puis demander `/ensemble-mission config` pour revoir la politique effective ; seul `/ensemble-mission start` utilisateur approuve, sans lever automatiquement une pause opérateur distincte.
