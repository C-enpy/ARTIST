Tu es dans un dépôt qui a déjà un profil project-manager (PM) pour Ensemble. Ne le recrée pas et ne le réécris pas. Lis-le, ainsi que le coordinateur existant (orchestrator, lead, ou la section de AGENTS.md qui dit qui publie les tâches). Crée la boucle QA ci-dessous, calibrée sur CE dépôt, puis branche-la sur ce coordinateur.



Ne copie aucune règle d’un autre produit (pas d’ADR, de services, de commandes cargo, de Kind, ni de commits historiques étrangers). Si un chemin ou un identifiant n’existe pas ici, ce n’est pas une règle.



\## Fichiers



À côté du PM, au même format de frontmatter que les profils déjà là :



1\. `qa-planner.md` — subagent, lecture seule sauf l’artefact de plan QA. Pas de sous-agents.

2\. `test-engineer.md` — subagent auteur, worktree isolé. Il n’écrit que des tests, fixtures et config de test.

3\. `test-reviewer.md` — s’il existe, enrichis-le sans effacer sa méthode propre (bugs historiques, format de findings, DEFER\_TO). S’il n’existe pas, crée un reviewer lecture seule, contexte fresh, qui persiste un briefing.



Ne crée pas un second coordinateur. Si un orchestrator existe, ajoute-lui la boucle. Sinon, écris la boucle là où le PM remet déjà son plan (profil lead ou AGENTS.md). N’ajoute `ensemble.json` que s’il est absent et que ce dépôt utilise déjà le plugin Ensemble avec auto-dispatch ; sinon ne touche pas à la config Ensemble.



Modèles : ceux déjà utilisés par le PM et les reviewers de ce dépôt. Ne copie pas les identifiants de modèle d’un autre repo. `qa-planner` sur un tier de raisonnement haut, `test-engineer` sur le tier d’implémentation, `test-reviewer` sur le tier de revue lecture seule.



\## Deux couches dans chaque profil QA



Méthode portable, puis pin projet. Le pin est ce que tu as vérifié ici. La méthode dit explicitement : si le pin ne correspond pas au dépôt ouvert, ne pas l’appliquer, ne pas réécrire le profil, produire une section `Pin observé`.



Ajoute au PM existant, seulement si ce paragraphe n’y est pas : son score de risque route le modèle Ensemble ; la matrice du qa-planner route la profondeur de test. Ne pas modifier l’un pour corriger l’autre. Ne pas changer le plancher, le barème, ni le mode JEV déjà définis.



\## Méthode à écrire dans les profils



Boucle : `qa-planner` (plan de lot, avant le code) → auteurs → `test-engineer` (generator) → `test-reviewer` (auditeur). Le healer n’est pas un agent : c’est un mode borné de `test-engineer`.



qa-planner, avant implémentation, sans écrire de test :

\- Risque = impact × probabilité, scorés séparément. Le churn, l’absence de tests et la logique conditionnelle élèvent la probabilité, jamais l’impact. La gravité seule n’est pas la priorité.

\- Zones : 15–25 critique (automatiser, chemins d’erreur inclus) ; 10–14 haut ; 5–9 moyen (nominal + erreurs clés) ; 1–4 bas (fumée ou skip).

\- Pour chaque item ≥ 10 : mode de défaillance avec déclencheur, blast radius, détection actuelle, mitigation actuelle, gap. Un gap vide = analyse non faite.

\- Anti-patterns : couverture égale partout, théâtre du risque (matrice qui ne change pas l’allocation), matrice one-shot, near-miss ignoré, ancrage seulement sur un vieil incident.

\- Passe heuristique en plus des features : états, frontières, ordre, idempotence.

\- Obligations explicites et inférées séparées. Une inférence n’est pas une spec. Un invariant est une obligation. Une ambiguïté est une décision humaine, pas un choix silencieux.

\- Registre dans les deux sens : obligation sans preuve, et test orphelin, preuve périmée ou doublon faible. Un fichier ou un nom proche n’est pas une exécution. États : COVERED (fichier:symbole + pourquoi ça prouve), GAP, GAP-accept, GAP-defer, unassessed ou blocked. Faits, inférences, recommandations et décisions humaines restent distincts.

\- Chaque obligation matérielle : au moins un scénario nominal et un négatif. Chaque invariant a un intent. Pas deux lignes pour le même comportement. Catégories selon le lot : nominal, frontière, négatif, transition d’état, concurrence. Sécurité ou accessibilité seulement si le lot les touche.

\- Ceci est un plan de lot, pas une stratégie d’entreprise. Si aucune stratégie écrite ne fixe les niveaux et ce qui peut être mocké, nomme cette absence comme décision humaine. N’importe pas une pyramide générique.

\- Budget : 70–80 % planifié, 20–30 % de buffer nommé, plus une ligne setup (fixtures, données, environnement, souvent 20–40 %). Au-delà de la capacité, coupe par risque × effort : un risque haut peu coûteux avant un risque haut coûteux. Ne sacrifie jamais le buffer, ni un item haut pour garder des items moyens.

\- Artefact : un fichier par lot, commité par le lead avant les auteurs isolés. Un fichier non commité n’arrive pas dans les worktrees. Le coordinateur reçoit le chemin, le top 3 des risques, les gaps bloquants, les critères de sortie, les décisions ouvertes.



test-engineer :

\- Ordre obligatoire, même en notes courtes : obligation, risque ou invariant, scénario, oracle seulement ensuite, code, puis KEEP / MODIFY / REJECT / DEFER par test. REJECT si API inventée, mauvais niveau, ou oracle recopié de l’implémentation.

\- Pas de code avant le plan. Assertions sur l’observable. Pas d’assertion « pas d’exception » seule. Chaque symbole et fixture appelé existe vraiment. Le test ciblé a été exécuté. Réutiliser le seed ou la fixture déjà là. Tracer chaque test vers G-<n> ou T-<n>.

\- Si le même contexte d’agent a écrit le code et les tests, le dire. L’oracle reste celui du plan ou du canon.

\- Un test qui révèle un bug de prod reste rouge. Pas de fix de prod.

\- Healer : seulement sur un test qu’il vient d’écrire. Il peut corriger fixture, nom d’helper, attente ou données. Il ne change pas l’oracle pour coller au comportement observé. Deux auto-réparations maximum. Un skip n’est permis que s’il déclare la fonctionnalité cassée.



test-reviewer :

\- Question : les tests attraperaient-ils un bug plausible introduit par ce diff ? Pas « le code est-il correct ? ».

\- Mutation mentale : inverser une condition, retirer une validation, retourner un défaut, ignorer une erreur, exécuter deux fois, déplacer une frontière. Sans scénario input → bug survivant → assertion, pas de finding haut.

\- Smells bornés au diff, symptômes et non verdicts. Priorité : fiabilité, diagnostic, design, lisibilité, puis suspicion de génération (API inventée, oracle recopié, boucle fermée code+tests) et couverture réelle. Frontières : borne, vide, doublon, ordre, hors plage.

\- Preuve classée executed, static-presence ou inference.

\- Un test verdi en affaiblissant l’assertion par rapport au plan QA est un finding.

\- S’il y a un plan QA, les findings référencent ses lignes. Sinon, registre borné au lot, même discipline faits / décisions.

\- PASS est valide. Ne pas réécrire les tests, sauf APPLY explicite du lead dans les permissions du profil.

\- DEFER\_TO vers les reviewers déjà présents (architecture, sécurité, perf, correction de prod). L’écriture des tests manquants va à test-engineer. Le cadrage d’un lot entier va à qa-planner.



\## Pin à découvrir avant d’écrire les profils



Vérifie dans ce dépôt, et inscris seulement ce que tu as lu :



\- Canon : décisions acceptées, contrats, ce qui prime sur le code.

\- Stratégie de test écrite, ou son absence. Niveaux réels, ce qui peut être mocké, ce qui ne doit pas l’être.

\- Commande de test ciblée réelle. Ressources exclusives (un seul build, fixtures, runtime).

\- Seed ou fixture déjà là. Où vivent les tests.

\- 3 à 5 catégories de bugs dont les commits sont dans CE dépôt. Pas d’historique inventé.

\- Emplacement d’artefact déjà utilisé pour les revues ou les handoffs. Réutilise-le. Sinon `.cursor/qa-plans/` pour les plans et le briefing de revue à côté des briefings existants.

\- Outils de recherche déjà prescrits par le PM (index sémantique, symboles, docs, digestion de longs outputs). Ne nomme un outil que s’il est déjà disponible ici.



\## Branchement Ensemble



Le PM découpe et score. Le coordinateur :



1\. Si le lot a des comportements sous contrat, une suite d’intégration étendue, ou un risque PM ≥ 50 (même échelle 0–100 que le PM ; sinon le seuil médian de son barème), lance `qa-planner` avant les auteurs.

2\. Le plan QA ne change pas les scores ni le plancher. Il devient critères de sortie des tâches de code, ou tâches de test séparées qui dépendent du code producteur.

3\. Le lead commit le plan QA avant les worktrees.

4\. Après intégration du code, `test-reviewer` en contexte fresh. S’il demande des tests, `test-engineer` sur ces findings, puis ré-audit.

5\. Cette boucle ne vaut pas l’approbation `start`. Seul l’utilisateur lance la mission.

6\. Les tâches de test restent des tâches d’implémentation pour le routage modèle du PM. Le reviewer n’est pas une tâche du board : c’est un spawn du lead après merge.



À la fin, rends les chemins créés ou modifiés, le pin observé en dix lignes, et les décisions de test que tu n’as pas tranchées.

