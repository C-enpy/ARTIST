# Révision locale des capacités QA — plan préalable

Date : 2026-10-18. Proposition : `qa-planner`, session
`ses_eb0fa9b3fffeEq67zPJgqxD2g5`, contexte fresh, lecture seule.
Synthèse et décisions : lead ; aucun test exécuté par le planner.
Base : `98072802c0dde2688570520ae6a02fe4f53395e1`, worktree déjà dirty.

## Contrat et décisions du lead

Demande explicite : autoriser `execute`, `shell`, `subagent` dans les trois
profils QA, maintenir `edit` interdit et vérifier les permissions imbriquées.
Ownership : `.opencode/agents/{qa-planner,test-engineer,test-reviewer}.md` ;
preuves dans ce répertoire et photographie `docs/agentic/gpu-qa-harness.md`.
Ne pas modifier les autres changements utilisateur ni la politique de mission.
Travail local indivisible, pas de mission/équipe/worktree auteur.
PM/LLM local : complexité brute 35, retenue 70 par prudence si code de contrôle ;
risque brut/retenu 45. Ni JEV, ni modification des scores du catalogue.

Décisions résolues à partir de la demande utilisateur :
- Les trois actions portent sur `*`, comme les règles existantes ; seul leur
  effet passe à `allow`. Deny-all, read/glob/skill, secrets, edit et externe restent.
- Aucun allow MCP ou `team_*` implicite. La documentation V2 dit que chaque outil
  imbriqué applique sa propre règle ; vérifier, ne pas déduire son accès d'execute.
- Les QA restent des feuilles et ne lancent ni agents ni tests moteur dans leur
  mandat. L'allow natif shell/subagent n'est PAS une garantie technique de ce rôle.
  Pas de sandbox filesystem revendiquée : un shell/exécuteur autorisé peut avoir
  des capacités que `edit deny` ne confine pas.
- Contrôles natifs inoffensifs seulement : calcul en mémoire, découverte, lecture
  d'une fixture non secrète ou tentative d'un outil sans mutation. Pas de test
  destructif, de vrais secrets, d'API modifiant des sessions/grants ni de cascade.
- Conserver le fallback texte si le transport reste refusé. Un blocage MCP n'est
  pas réparé par une ouverture générale ; signaler le grant exact manquant.

Les ADR ne changent pas : 0001/2/4/6/7 Proposed, 0003/5 Accepted partial,
0008 vérifié Windows Debug seulement. Aucun résultat moteur nouveau.

## Obligations, oracles et registre bidirectionnel

Explicites : trois actions ouvertes, edit interdit, vérification Code Mode.
Inférées et retenues : préserver secrets/externe/deny-all, cohérence du texte,
attribution au runner lead et séparation présence/découverte/refus natif.

- O1 ↔ T1 : syntaxe/frontmatter et delta exact. Nominal : trois effets changés
  par profil ; négatif : règle tardive ou autorisation supplémentaire détectée.
  Oracle : comparer les règles complètes avant/après, pas une recherche de chaînes.
- O2 ↔ T2 : protections conservées. Nominal : read/glob/skill inchangés ; négatif :
  edit, secrets synthétiques, externe et MCP sans exception restent deny dans la
  matrice résolue. Oracle : dernier match natif quand observable ; évaluation locale
  seule = static-presence, jamais preuve d'enforcement.
- O3 ↔ T3 : execute et outils imbriqués. Nominal : calcul mémoire accessible ;
  négatif : outil imbriqué non autorisé absent/refusé, distinction entre catalogue
  filtré, refus natif et abstention du modèle. Aucun outil mutateur invoqué.
- O4 ↔ T4 : contrat textuel. Nominal : Context Mode/transport permis sous leurs
  règles propres ; négatif : ancienne fermeture absolue supprimée. Pas de promesse
  de confinement, de délégation autorisée ni de tests moteur par le QA.
- O5 ↔ T5 : candidat et traces. HEAD, SHA256, commande, runner, version et sessions
  doivent accompagner les résultats. Candidat chargé obsolète = INCONCLUSIVE.

Tous les contrôles sont proposés au runner lead ; aucun test orphelin identifié.
Un FAIL natif prime sur une preuve statique ; zéro contrôle n'est pas PASS.
Configuration : contrôles adaptés, pas de faux RED comportemental exigé.
Les anciens oracles T04/T05 pinnaient les trois actions à deny : conserver leur
historique et signaler leur incompatibilité si exercés, ne pas les affaiblir.

## Top 3 risques QA

1. I5×P4=20 : ouvrir un exécuteur permet des effets indirects. Déclencheur : shell
   ou MCP autorisé ; blast : données accessibles au processus. Détection actuelle :
   règles statiques seulement ; mitigation : mandat borné, aucun mutateur testé.
   Gap : pas de confinement filesystem démontré ; ne pas le revendiquer.
2. I4×P3=12 : sous-délégation/exécution métier. Déclencheur : subagent allow ;
   blast : sessions et ressources. Détection : consignes feuilles, pas gate prouvé ;
   mitigation : aucun lancement par QA. Gap : allow natif et mandat restent distincts.
3. I3×P4=12 : ouverture ineffective ou preuve sur ancien profil. Déclencheur :
   héritage/ordre/rechargement ; blast : trois QA et transport. Détection : API
   resolved agents + contrôle natif ciblé ; mitigation : empreintes et traces.
   Gap : chaque outil imbriqué doit être distingué de l'ouverture d'execute.

## Ordre, ressources, budget, sortie

Plan persisté/commité → patch local → contrôles syntaxe/règles et découverte native
indépendants → probes natifs séquentiels → revue fresh → photographie documentaire.
Un seul écrivain sur les profils ; aucune compilation/indexation concurrente.
Pas de builds C++/GPU. Les enquêtes de lecture peuvent être parallèles.
Budget relatif : setup 25 ; restant 75 = 56,25 planifié et 18,75 buffer (75/25).
Le buffer couvre découverte runtime, refus/absence d'outil et ré-audit.

Sortie : delta autorisé, protections déclarées conservées, résultats natifs bornés
et limites explicites. Le transport Ensemble réel sans équipe existante peut rester
NOT_RUN ; absence/refus d'un MCP ne devient pas PASS fonctionnel Context Mode.
Une ouverture supplémentaire réclame une décision distincte, pas une réparation
automatique de ce lot. Plan terminé ne vaut pas PASS de validation.
