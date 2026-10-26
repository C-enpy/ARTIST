# 87 — Voie native disponible et proposition simplifiée, sans développement d'outillage

2026-10-26. Décision opérateur après86 : commandes existantes autorisées pour affecter correctement l'agent ; **aucun nouveau chantier de développement d'outillage**. Si insuffisant, exposer le blocage précis et proposer plus simple. Cette décision ne constitue pas une dérogation implicite aux barrières QA.

## Vérification bornée effectuée

Consultation des références V2 API/Ensemble puis commandes natives de lecture via la CLI existante :

- `api get /api/command` : seule commande Ensemble enregistrée, `ensemble-mission`.
- `api get /openapi.json` : schéma exact de l'exécution d'une commande et de sa découverte ; aucun appel de mutation effectué.
- `api get /api/plugin` : confirme le bundle Ensemble chargé `C:/Users/djden/.config/opencode/local-plugins/ensemble/20261025-pm-review-0f9902409237/index.js`.
- Lecture ciblée du parseur de cette commande dans ce bundle, sans édition : seuls `prepare`, `config`, `start`. Un texte `hold/assign/dispatch/help` inconnu serait traité comme **objectif de préparation**, pas contrôle de tâche ; ne pas l'invoquer à l'aveugle. `config` n'est pas une commande de réaffectation et suspendrait de nouvelles admissions ; `start` demeure réservé à l'utilisateur.
- Le même bundle recoupe les refus déjà observés86 : mission autonome exige `claim_task` et reporting collaboratif ; le mode restrictif `final_response` est refusé ici. Un claim exige tâche non affectée/pending. Le schéma exposé d'ajout ne fournit ni réservation, ni contexte/worktree, ni unassign ; le spawn le fournit mais arrive après auto-affectation. Aucune nouvelle publication ou tentative en boucle.

Portée : les commandes/outils **exposés dans cette session** ne résolvent pas l'admission restrictive demandée ; pas affirmation qu'aucun autre client ne pourrait la résoudre. Le planner auto-affecté reste dans sa session historique avec permissions effectives incompatibles (preuve86). La lecture/restitution fonctionne ; l'identité restrictive et le contexte requis ne sont pas appliqués. Aucun diagnostic C++/Reader ne découle de ce blocage.

Aucune écriture de code/config/profil/plugin/base interne, installation, service, restart/reload, changement de modèle ou nouveau worker. Une erreur de syntaxe d'un appel ponctuel de recherche a été corrigée par une lecture ciblée ; pas panne MCP ni motif de nouveau chantier. Pas de probe/build/test exécuté.

## Organisation plus simple proposée

**Proposition, non exécutée et non autorisée implicitement** : une seule tranche locale de nettoyage avec le lead comme auteur/runner, au lieu d'un board de nouveaux auteurs et d'une chaîne planner/generator/audit automatique.

1. Le lead rédige/commit l'amendement QA ciblé au contrat85 : matrice obligations/oracles et pertes explicitement nommées ; pas de nouveau profil/outillage. Les remises architecte/PM acquises suffisent comme entrées, sans réenquête.
2. Caractérisations du Reader actuel exécutées par le lead **avant** refactoring ; puis modification limitée Reader/tests et retrait nominatif des13 fichiers après contrôle exact des consommateurs. Un seul écrivain ; tout écart comportemental nouveau exige RED causal, sinon arrêt.
3. Build/découverte/sélection pré-run, tests Windows Debug et ASan proportionnés selon protocole existant, logs/JUnit au nouveau candidat. Aucun ancien comptage/verdict automatiquement repris ; pas de campagne expérimentale.
4. Une seule revue indépendante adaptée au diff intégré, par `correctness-reviewer` natif fresh si son admission effective est vérifiable. Pas la présenter comme l'audit `test-reviewer` restrictif requis actuellement. En cas d'admission incompatible, signaler la limite sans autre substitution ni réparation du harness.
5. Photographie documentaire et bilan des gaps ; aucune clôture de catch/close authentiques par suppression de tests, aucun PASS global, reprise B1–B6 selon dispositions acquises.

Gain : pas de développement d'outillage, ni nouvelle équipe ou chaîne de tâches ; coût : lead unique, parallélisme D/C remplacé par une séquence pour simplifier ownership et contrôle. Les investigations RO architecte/PM ont déjà été parallélisées et leurs résultats sont réutilisés.

**Décision encore nécessaire pour cette variante** : exception explicite limitée à cette tranche à l'exigence de profils QA séparés (qa-planner/test-engineer/test-reviewer) et à la topologie85. La QA reste effectuée, mais son plan/gap-check est local et non indépendant ; la revue C++ demeure indépendante si admise. Ni la demande de simplification ni l'autorisation de commandes seules ne sont présentées comme cette exception. À défaut d'accord : maintenir la barrière avant source, sans développer l'outillage.
